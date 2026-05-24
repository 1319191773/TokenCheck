#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QJniObject>
#include "usagequery.h"
#include "appsettings.h"
#include "widgetbridge.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("TokenCheck");

    UsageQuery query;
    WidgetBridge bridge;

    QObject::connect(&query, &UsageQuery::queryFinished,
                     &bridge, &WidgetBridge::onPlatformFinished);
    QObject::connect(&query, &UsageQuery::queryAllFinished, &bridge, &WidgetBridge::onAllFinished);
    QObject::connect(&query, &UsageQuery::queryFailed,
                     [](const QString &error) { qWarning() << "Query failed:" << error; });

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("usageQuery", &query);
    engine.rootContext()->setContextProperty("appSettings", &AppSettings::instance());

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);
    engine.load(QUrl(QStringLiteral("qrc:/src/qml/MainPage.qml")));

    if (AppSettings::instance().isConfigured()) {
        query.query();
    }

    int interval = AppSettings::instance().autoRefreshInterval();
    if (interval > 0)
        query.setAutoRefresh(interval);

    return app.exec();
}
