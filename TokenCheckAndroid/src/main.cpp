#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include "usagequery.h"
#include "appsettings.h"
#include "platform_registry.h"

#ifdef Q_OS_ANDROID
#include <QJniObject>
#include "widgetbridge.h"
#endif

#ifdef Q_OS_IOS
#include "iosappgroups.h"
#endif

#ifdef Q_OS_ANDROID
class AndroidHelper : public QObject
{
    Q_OBJECT
public:
    explicit AndroidHelper(QObject *parent = nullptr) : QObject(parent) {}
    Q_INVOKABLE void moveToBack()
    {
        QJniObject activity = QJniObject::callStaticObjectMethod(
            "org/qtproject/qt/android/QtNative", "activity", "()Landroid/app/Activity;");
        if (activity.isValid())
            activity.callMethod<void>("moveTaskToBack", "(Z)V", true);
    }
};

static bool checkWidgetRefreshIntent()
{
    QJniObject activity = QJniObject::callStaticObjectMethod(
        "org/qtproject/qt/android/QtNative", "activity", "()Landroid/app/Activity;");
    if (!activity.isValid())
        return false;

    QJniObject intent = activity.callObjectMethod("getIntent", "()Landroid/content/Intent;");
    if (!intent.isValid())
        return false;

    QJniObject action = intent.callObjectMethod(
        "getStringExtra",
        "(Ljava/lang/String;)Ljava/lang/String;",
        QJniObject::fromString("com.tokencheck.android.ACTION").object<jstring>());

    return action.isValid() && action.toString() == "REFRESH";
}

static void finishActivity()
{
    QJniObject activity = QJniObject::callStaticObjectMethod(
        "org/qtproject/qt/android/QtNative", "activity", "()Landroid/app/Activity;");
    if (activity.isValid())
        activity.callMethod<void>("finish");
}
#endif

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("TokenCheck");
    app.setOrganizationName("com.tokencheck");

    PlatformRegistry::init();

    UsageQuery query;

#ifdef Q_OS_ANDROID
    WidgetBridge bridge;
    QObject::connect(&query, &UsageQuery::queryFinished,
                     &bridge, &WidgetBridge::onPlatformFinished);
    QObject::connect(&query, &UsageQuery::queryAllFinished,
                     &bridge, &WidgetBridge::onAllFinished);
#endif

#ifdef Q_OS_IOS
    IOSAppGroups iosBridge;
    QObject::connect(&query, &UsageQuery::queryFinished,
                     &iosBridge, &IOSAppGroups::onPlatformFinished);
    QObject::connect(&query, &UsageQuery::queryAllFinished,
                     &iosBridge, &IOSAppGroups::onAllFinished);
#endif

    QObject::connect(&query, &UsageQuery::queryFailed,
                     [](const QString &error) { qWarning() << "Query failed:" << error; });

#ifdef Q_OS_ANDROID
    bool refreshOnly = checkWidgetRefreshIntent();

    if (refreshOnly) {
        QObject::connect(&query, &UsageQuery::queryAllFinished, &app, [&]() {
            QTimer::singleShot(500, []() { finishActivity(); });
        });
    }
#endif

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("usageQuery", &query);
    engine.rootContext()->setContextProperty("appSettings", &AppSettings::instance());
    engine.rootContext()->setContextProperty("platformRegistry", &PlatformRegistry::instance());

#ifdef Q_OS_ANDROID
    engine.rootContext()->setContextProperty("androidHelper", new AndroidHelper(&app));
#endif

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);
    engine.load(QUrl(QStringLiteral("qrc:/src/qml/MainPage.qml")));

    QTimer::singleShot(0, []() {
        AppSettings::instance().syncWidgetConfig();
    });

    if (
#ifdef Q_OS_ANDROID
        refreshOnly ||
#endif
        AppSettings::instance().isConfigured())
        query.query();

    int interval = AppSettings::instance().autoRefreshInterval();
    if (interval > 0)
        query.setAutoRefresh(interval);

    return app.exec();
}

#include "main.moc"
