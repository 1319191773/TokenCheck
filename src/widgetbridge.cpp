#include "widgetbridge.h"
#include "appsettings.h"
#include "androidprefs.h"

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
    AndroidPrefs::write(prefix + "token", QString::number(data.tokenPercentage()));
    AndroidPrefs::write(prefix + "mcp", QString::number(data.mcpPercentage()));
    AndroidPrefs::write(prefix + "time", data.tokenResetTime());
    m_completedCount++;
}

void WidgetBridge::onAllFinished()
{
    AndroidPrefs::writeInt("platformCount", m_completedCount);

    auto &s = AppSettings::instance();
    AndroidPrefs::writeInt("widgetShowToken", s.widgetShowToken());
    AndroidPrefs::writeInt("widgetShowMcp", s.widgetShowMcp());
    AndroidPrefs::writeInt("widgetShowTime", s.widgetShowTime());
    AndroidPrefs::writeInt("widgetFontSize", s.widgetFontSize());

    m_completedCount = 0;
    AndroidPrefs::notifyWidgetUpdate();
}
