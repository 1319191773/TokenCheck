#include "ui/floatingball.h"
#include "ui/mainwindow.h"
#include "ui/globalhotkey.h"
#include "ui/trayiconmanager.h"
#include "ui/theme.h"
#include "network/usagequery.h"
#include "core/appsettings.h"
#include "core/platform_registry.h"
#include "core/datamanager.h"

#include <QApplication>
#include <QMessageBox>
#include <QTranslator>
#include <QLocale>
#include <QFile>

static void loadTranslation(const QString &lang)
{
    static QTranslator translator;
    QString locale = lang;

    if (locale.isEmpty()) {
        QString sysLocale = QLocale::system().name();
        if (sysLocale.startsWith("zh"))
            locale = "zh_CN";
        else
            locale = "en";
    }

    if (locale == "en")
        return;

    QString qmPath = QCoreApplication::applicationDirPath()
                     + "/TokenCheckPC_" + locale + ".qm";
    if (translator.load(qmPath))
        QCoreApplication::installTranslator(&translator);
}

int main(int argc, char *argv[])
{
    qputenv("QT_LOGGING_RULES", "qt.gui.icc=false");

    QApplication app(argc, argv);
    app.setOrganizationName("ZaiTech");
    app.setApplicationName("TokenCheckPC");
    app.setQuitOnLastWindowClosed(false);

    QString iconPath = QCoreApplication::applicationDirPath() + "/icon.png";
    if (QFile::exists(iconPath))
        app.setWindowIcon(QIcon(iconPath));

    loadTranslation(AppSettings::instance().language());

    Theme::setTheme(AppSettings::instance().themeId());
    app.setStyleSheet(Theme::globalStyleSheet());

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, QObject::tr("Error"),
                              QObject::tr("System tray is not available."));
        return 1;
    }

    PlatformRegistry::init();

    FloatingBall ball;
    MainWindow window;
    UsageQuery query;
    GlobalHotkey hotkey;

    QObject::connect(&query, &UsageQuery::queryFinished,
                     &DataManager::instance(), &DataManager::setUsageData);
    QObject::connect(&query, &UsageQuery::queryFailed, &ball,
                     [&ball](const QString &) { ball.setStatus("Err"); });
    QObject::connect(&query, &UsageQuery::queryAllFinished, &DataManager::instance(),
                     []() { emit DataManager::instance().allDataUpdated(); });
    QObject::connect(&query, &UsageQuery::cachedDataAvailable,
                     &DataManager::instance(),
                     [](const QList<UsageData> &list) {
                         QMap<QString, UsageData> map;
                         for (const auto &d : list)
                             map[d.platformName] = d;
                         DataManager::instance().setAllData(map);
                     });

    QObject::connect(&DataManager::instance(), &DataManager::allDataUpdated,
                     &window, &MainWindow::onAllDataUpdated);

    QObject::connect(&ball, &FloatingBall::singleClicked, &query,
                     &UsageQuery::query);
    QObject::connect(&ball, &FloatingBall::doubleClicked, &window,
                     [&window]() { window.showTab(0); });
    QObject::connect(&ball, &FloatingBall::detailRequested, &window,
                     [&window]() { window.showTab(0); });

    TrayIconManager tray(&ball, &window, &query);
    QObject::connect(&ball, &FloatingBall::settingsRequested,
                     &tray, &TrayIconManager::showSettingsTab);
    QObject::connect(&ball, &FloatingBall::quitRequested, &app,
                     &QApplication::quit);
    QObject::connect(&tray, &TrayIconManager::quitRequested, &app,
                     &QApplication::quit);

    QObject::connect(&window, &MainWindow::refreshRequested, &query,
                     &UsageQuery::query);
    QObject::connect(&window, &MainWindow::settingsApplied, &ball,
                     &FloatingBall::applyBallSize);
    QObject::connect(&window, &MainWindow::settingsApplied, &tray, [&tray]() {
        Theme::setTheme(AppSettings::instance().themeId());
        qApp->setStyleSheet(Theme::globalStyleSheet());
        tray.rebuildMenu();
        tray.updateTooltip();
    });
    QObject::connect(&window, &MainWindow::settingsApplied, &query, [&query]() {
        query.stopAutoRefresh();
        int interval = AppSettings::instance().autoRefreshInterval();
        if (interval > 0)
            query.setAutoRefresh(interval);
    });

    hotkey.registerHotkey(1, AppSettings::instance().hotkeyToggle());
    hotkey.registerHotkey(2, AppSettings::instance().hotkeyRefresh());
    QObject::connect(&hotkey, &GlobalHotkey::hotkeyPressed, &app,
                     [&ball, &query](int id) {
                         if (id == 1)
                             ball.setBallVisible(!ball.isBallVisible());
                         else if (id == 2) {
                             ball.setStatus("...");
                             query.query();
                         }
                     });
    QObject::connect(&tray, &TrayIconManager::settingsChanged, &hotkey,
                     [&hotkey]() {
                         hotkey.unregisterAll();
                         hotkey.registerHotkey(1, AppSettings::instance().hotkeyToggle());
                         hotkey.registerHotkey(2, AppSettings::instance().hotkeyRefresh());
                     });

    tray.show();
    ball.show();
    tray.updateTooltip();

    query.loadCache();

    if (!AppSettings::instance().isConfigured()) {
        window.showTab(1);
    } else {
        query.query();
    }

    int interval = AppSettings::instance().autoRefreshInterval();
    if (interval > 0)
        query.setAutoRefresh(interval);

    return app.exec();
}
