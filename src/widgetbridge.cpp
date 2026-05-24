#include "widgetbridge.h"
#include "appsettings.h"
#include "androidprefs.h"

WidgetBridge::WidgetBridge(QObject *parent) : QObject(parent) {}

void WidgetBridge::onPlatformFinished(const UsageData &data)
{
    int idx = m_platformIndex;
    QString prefix = QString("p%1_").arg(idx);
    AndroidPrefs::write(prefix + "name", data.platformName);
    AndroidPrefs::write(prefix + "token", QString::number(data.tokenPercentage()));
    AndroidPrefs::write(prefix + "mcp", QString::number(data.mcpPercentage()));
    AndroidPrefs::write(prefix + "time", data.tokenResetTime());
    m_platformIndex++;
}

void WidgetBridge::onAllFinished()
{
    AndroidPrefs::writeInt("platformCount", m_platformIndex);

    auto &s = AppSettings::instance();
    AndroidPrefs::writeInt("widgetShowToken", s.widgetShowToken());
    AndroidPrefs::writeInt("widgetShowMcp", s.widgetShowMcp());
    AndroidPrefs::writeInt("widgetShowTime", s.widgetShowTime());
    AndroidPrefs::writeInt("widgetFontSize", s.widgetFontSize());

    m_platformIndex = 0;
    AndroidPrefs::notifyWidgetUpdate();
}
