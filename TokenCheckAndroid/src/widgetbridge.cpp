#include "widgetbridge.h"
#include "appsettings.h"
#include "androidprefs.h"
#include "platform_registry.h"

WidgetBridge::WidgetBridge(QObject *parent) : QObject(parent) {}

int WidgetBridge::resolvePlatformIndex(const QString &platformName) const
{
    auto platforms = AppSettings::instance().allPlatforms();
    for (int i = 0; i < platforms.size(); ++i) {
        if (platforms[i].name == platformName)
            return i;
    }
    return m_completedCount;
}

void WidgetBridge::onPlatformFinished(const UsageData &data)
{
    int idx = resolvePlatformIndex(data.platformName);
    QString prefix = QString("p%1_").arg(idx);
    AndroidPrefs::write(prefix + "name", data.platformName);
    AndroidPrefs::write(prefix + "type", data.platformType);

    auto &desc = PlatformRegistry::instance().descriptor(data.platformType);
    for (const auto &slot : desc.widgetSlots) {
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
        AndroidPrefs::write(prefix + slot.dataKey, value.isEmpty() ? "-1.0" : value);
    }

    m_completedCount++;
}

void WidgetBridge::onAllFinished()
{
    AndroidPrefs::writeInt("platformCount", m_completedCount);

    auto &s = AppSettings::instance();
    AndroidPrefs::writeInt("widgetShowToken", s.widgetShowToken());
    AndroidPrefs::writeInt("widgetShowMcp", s.widgetShowMcp());
    AndroidPrefs::writeInt("widgetShowTime", s.widgetShowTime());
    AndroidPrefs::writeInt("widgetShowBalance", s.widgetShowBalance());
    AndroidPrefs::writeInt("widgetShowGranted", s.widgetShowGranted());
    AndroidPrefs::writeInt("widgetFontSize", s.widgetFontSize());
    AndroidPrefs::writeInt("widgetRefreshInterval", s.autoRefreshInterval());

    m_completedCount = 0;
    AndroidPrefs::notifyWidgetUpdate();
}
