#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QJniObject>
#include <QTimer>
#include "usagequery.h"
#include "appsettings.h"
#include "widgetbridge.h"

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
        QJniObject::fromString("com.glm.usage.ACTION").object<jstring>());

    return action.isValid() && action.toString() == "REFRESH";
}

static void finishActivity()
{
    QJniObject activity = QJniObject::callStaticObjectMethod(
        "org/qtproject/qt/android/QtNative", "activity", "()Landroid/app/Activity;");
    if (activity.isValid())
        activity.callMethod<void>("finish");
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("GLM Usage");
    app.setOrganizationName("com.glm.usage");

    UsageQuery query;
    WidgetBridge bridge;

    QObject::connect(&query, &UsageQuery::queryFinished,
                     &bridge, &WidgetBridge::onPlatformFinished);
    QObject::connect(&query, &UsageQuery::queryAllFinished,
                     &bridge, &WidgetBridge::onAllFinished);
    QObject::connect(&query, &UsageQuery::queryFailed,
                     [](const QString &error) { qWarning() << "Query failed:" << error; });

    bool refreshOnly = checkWidgetRefreshIntent();

    if (refreshOnly) {
        QObject::connect(&query, &UsageQuery::queryAllFinished, &app, [&]() {
            QTimer::singleShot(500, []() { finishActivity(); });
        });
    }

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("usageQuery", &query);
    engine.rootContext()->setContextProperty("appSettings", &AppSettings::instance());

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);
    engine.load(QUrl(QStringLiteral("qrc:/src/qml/MainPage.qml")));

    if (refreshOnly || AppSettings::instance().isConfigured())
        query.query();

    int interval = AppSettings::instance().autoRefreshInterval();
    if (interval > 0)
        query.setAutoRefresh(interval);

    return app.exec();
}
