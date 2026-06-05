#include "trayiconmanager.h"
#include "floatingball.h"
#include "mainwindow.h"
#include "usagequery.h"
#include "appsettings.h"
#include "datamanager.h"
#include "theme.h"

#include <QApplication>
#include <QAction>
#include <QPainter>
#include <QCoreApplication>

TrayIconManager::TrayIconManager(FloatingBall *ball, MainWindow *window,
                                 UsageQuery *query, QObject *parent)
    : QObject(parent)
    , m_ball(ball)
    , m_window(window)
    , m_query(query)
    , m_dm(&DataManager::instance())
{
    m_menu = new QMenu();

    m_tray = new QSystemTrayIcon(this);
    m_tray->setContextMenu(m_menu);
    m_tray->setToolTip(tr("TokenCheck"));

    QPixmap pix(":/icon.png");
    if (pix.isNull()) {
        pix = QPixmap(32, 32);
        pix.fill(Qt::transparent);
        QPainter painter(&pix);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(Theme::accent);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(2, 2, 28, 28);
    }
    m_tray->setIcon(QIcon(pix));

    connect(m_tray, &QSystemTrayIcon::activated, this,
            [this](QSystemTrayIcon::ActivationReason reason) {
                if (reason == QSystemTrayIcon::DoubleClick)
                    onShowDetail();
            });

    connect(m_dm, &DataManager::allDataUpdated, this, &TrayIconManager::checkQuotaNotifications);

    rebuildMenu();
}

void TrayIconManager::show()
{
    m_tray->show();
}

void TrayIconManager::rebuildMenu()
{
    m_menu->clear();

    auto *queryAction = m_menu->addAction(tr("Query Now"));
    auto *detailAction = m_menu->addAction(tr("Show Detail"));
    auto *settingsAction = m_menu->addAction(tr("Settings..."));
    m_menu->addSeparator();

    auto names = m_dm->accountNames();
    if (!names.isEmpty()) {
        for (const auto &n : names) {
            auto *act = m_menu->addAction(n);
            act->setCheckable(true);
            act->setChecked(n == m_dm->currentAccount());
            connect(act, &QAction::triggered, this, [this, n]() {
                m_dm->setCurrentAccount(n);
            });
        }
        m_menu->addSeparator();
    }

    auto *quitAction = m_menu->addAction(tr("Quit"));

    connect(queryAction, &QAction::triggered, this, &TrayIconManager::onQueryNow);
    connect(detailAction, &QAction::triggered, this, &TrayIconManager::onShowDetail);
    connect(settingsAction, &QAction::triggered, this, &TrayIconManager::showSettingsTab);
    connect(quitAction, &QAction::triggered, this, &TrayIconManager::quitRequested);
}

void TrayIconManager::updateTooltip()
{
    auto names = m_dm->accountNames();
    if (names.isEmpty())
        m_tray->setToolTip(tr("GLM Usage Monitor"));
    else
        m_tray->setToolTip("GLM: " + names.join(", "));
}

void TrayIconManager::checkQuotaNotifications()
{
    int threshold = AppSettings::instance().notifyThreshold();
    auto allData = m_dm->allData();
    QSet<QString> currentNotified;

    for (const auto &d : allData) {
        if (!d.isValid)
            continue;

        if (d.platformType != "deepseek") {
            double tokenPct = d.tokenPercentage();
            if (tokenPct >= 0 && tokenPct > (100 - threshold)) {
                QString key = d.platformName + "_token";
                currentNotified.insert(key);
                if (!m_notifiedQuotas.contains(key)) {
                    m_tray->showMessage(
                        tr("Quota Warning"),
                        QString("%1: ").arg(d.platformName) + tr("Token used %1%").arg(tokenPct, 0, 'f', 1),
                        QSystemTrayIcon::Warning, 5000);
                }
            }

            double mcpPct = d.mcpPercentage();
            if (mcpPct >= 0 && mcpPct > (100 - threshold)) {
                QString key = d.platformName + "_mcp";
                currentNotified.insert(key);
                if (!m_notifiedQuotas.contains(key)) {
                    m_tray->showMessage(
                        tr("Quota Warning"),
                        QString("%1: ").arg(d.platformName) + tr("MCP used %1%").arg(mcpPct, 0, 'f', 1),
                        QSystemTrayIcon::Warning, 5000);
                }
            }
        } else {
            double bal = d.balanceTotal();
            if (bal >= 0 && bal < 1.0) {
                QString key = d.platformName + "_balance";
                currentNotified.insert(key);
                if (!m_notifiedQuotas.contains(key)) {
                    m_tray->showMessage(
                        tr("Low Balance"),
                        QString("%1: %2 %3").arg(d.platformName).arg(bal, 0, 'f', 2).arg(d.balanceCurrency()),
                        QSystemTrayIcon::Warning, 5000);
                }
            }
        }
    }

    m_notifiedQuotas = currentNotified;
}

void TrayIconManager::onQueryNow()
{
    m_ball->setStatus("...");
    m_query->query();
}

void TrayIconManager::onShowDetail()
{
    m_window->showCardList();
}

void TrayIconManager::showSettingsTab()
{
    m_window->showCardList();
    emit settingsChanged();
}
