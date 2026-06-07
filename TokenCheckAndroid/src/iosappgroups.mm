#include "iosappgroups.h"
#include "appsettings.h"
#include "platform_registry.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

#import <Foundation/Foundation.h>
#import <WidgetKit/WidgetKit.h>

static const char *kGroupIdentifier = "group.com.tokencheck.shared";

IOSAppGroups::IOSAppGroups(QObject *parent) : QObject(parent) {}

QString IOSAppGroups::sharedContainerPath()
{
    NSURL *url = [[NSFileManager defaultManager]
        containerURLForSecurityApplicationGroupIdentifier:
            [NSString stringWithUTF8String:kGroupIdentifier]];
    if (url)
        return QString::fromNSString(url.path) + "/";
    return QString();
}

void IOSAppGroups::writeJSON(const QString &fileName, const QByteArray &data)
{
    QString dir = sharedContainerPath();
    if (dir.isEmpty()) return;
    QDir().mkpath(dir);
    QFile f(dir + fileName);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        f.write(data);
}

void IOSAppGroups::reloadTimelines()
{
    if (@available(iOS 14.0, *)) {
        [[WidgetCenter shared] reloadAllTimelines];
    }
}

void IOSAppGroups::onPlatformFinished(const UsageData &data)
{
    QJsonObject obj;
    obj["name"] = data.platformName;
    obj["type"] = data.platformType;
    obj["valid"] = data.isValid;

    auto &desc = PlatformRegistry::instance().descriptor(data.platformType);
    QJsonArray slots;
    for (const auto &slot : desc.widgetSlots) {
        QJsonObject s;
        s["label"] = slot.label;
        s["dataKey"] = slot.dataKey;
        s["defaultOn"] = slot.defaultOn;

        QString value;
        if (slot.dataKey == "token")
            value = QString::number(data.tokenPercentage());
        else if (slot.dataKey == "mcp")
            value = QString::number(data.mcpPercentage());
        else if (slot.dataKey == "time")
            value = data.tokenResetTime();
        else if (slot.dataKey == "balance") {
            for (const auto &q : data.quotaLimits) {
                if (q.type.contains("BALANCE") && value.isEmpty()) {
                    QString currency = q.type.contains("CNY") ? "CNY" : "USD";
                    value = currency + " " + QString::number(q.total / 100.0, 'f', 2);
                }
            }
        } else if (slot.dataKey == "granted") {
            for (const auto &q : data.quotaLimits) {
                if (q.type.contains("BALANCE") && value.isEmpty()) {
                    value = QString::number(q.remaining / 100.0, 'f', 2);
                }
            }
        }
        s["value"] = value.isEmpty() ? "-1.0" : value;
        slots.append(s);
    }
    obj["slots"] = slots;

    writeJSON(QString("platform_%1.json").arg(m_completedCount),
              QJsonDocument(obj).toJson(QJsonDocument::Compact));
    m_completedCount++;
}

void IOSAppGroups::onAllFinished()
{
    QJsonObject meta;
    meta["platformCount"] = m_completedCount;
    meta["timestamp"] = static_cast<double>(QDateTime::currentMSecsSinceEpoch());
    writeJSON("widget_meta.json", QJsonDocument(meta).toJson(QJsonDocument::Compact));

    m_completedCount = 0;
    reloadTimelines();
}

void IOSAppGroups::syncWidgetConfig(AppSettings *s)
{
    QJsonObject config;
    config["widgetShowToken"] = s->widgetShowToken();
    config["widgetShowMcp"] = s->widgetShowMcp();
    config["widgetShowTime"] = s->widgetShowTime();
    config["widgetShowBalance"] = s->widgetShowBalance();
    config["widgetShowGranted"] = s->widgetShowGranted();
    config["widgetFontSize"] = s->widgetFontSize();
    config["refreshInterval"] = s->autoRefreshInterval();

    QJsonArray platforms;
    for (int i = 0; i < s->platformCount(); i++) {
        if (s->platformEnabled(i) && !s->platformAuthToken(i).isEmpty()) {
            QJsonObject p;
            p["name"] = s->platformName(i);
            p["baseUrl"] = s->platformBaseUrl(i);
            p["type"] = s->platformType(i);
            platforms.append(p);
        }
    }
    config["platforms"] = platforms;

    writeJSON("widget_config.json", QJsonDocument(config).toJson(QJsonDocument::Compact));
}
