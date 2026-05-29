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
    AndroidPrefs::write(prefix + "type", data.platformType);

    if (data.platformType == "deepseek") {
        QString balanceStr;
        QString grantedStr;
        auto quotas = data.quotaLimits;
        for (const auto &q : quotas) {
            if (q.type.contains("BALANCE") && balanceStr.isEmpty()) {
                QString currency = q.type.contains("CNY") ? "CNY" : "USD";
                balanceStr = currency + " " + QString::number(q.total / 100.0, 'f', 2);
                grantedStr = QString::number(q.remaining / 100.0, 'f', 2);
            }
        }
        AndroidPrefs::write(prefix + "balance", balanceStr);
        AndroidPrefs::write(prefix + "granted", grantedStr);
        AndroidPrefs::write(prefix + "token", "-1.0");
        AndroidPrefs::write(prefix + "mcp", "-1.0");
        AndroidPrefs::write(prefix + "time", "");
    } else {
        AndroidPrefs::write(prefix + "token", QString::number(data.tokenPercentage()));
        AndroidPrefs::write(prefix + "mcp", QString::number(data.mcpPercentage()));
        AndroidPrefs::write(prefix + "time", data.tokenResetTime());
        AndroidPrefs::write(prefix + "balance", "");
        AndroidPrefs::write(prefix + "granted", "");
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

    m_completedCount = 0;
    AndroidPrefs::notifyWidgetUpdate();
}
