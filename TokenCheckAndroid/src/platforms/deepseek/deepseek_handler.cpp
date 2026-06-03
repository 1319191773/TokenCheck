#include "deepseek_handler.h"
#include "usagequery.h"
#include <QJsonArray>

QList<PlatformEndpoint> DeepSeekHandler::endpoints(const QString &,
                                                    const QString &) const
{
    return { { "/user/balance", "" } };
}

bool DeepSeekHandler::parse(int, const QJsonObject &root, UsageData &data)
{
    if (!root.contains("balance_infos"))
        return false;

    QJsonArray infos = root.value("balance_infos").toArray();
    bool isAvailable = root.value("is_available").toBool();
    for (int i = 0; i < infos.size(); i++) {
        QJsonObject info = infos[i].toObject();
        QString currency = info.value("currency").toString("USD");
        QuotaLimitItem item;
        item.type = "BALANCE_" + currency.toUpper();
        item.percentage = -1.0;
        item.total = static_cast<qint64>(info.value("total_balance").toString("0").toDouble() * 100);
        item.remaining = static_cast<qint64>(info.value("granted_balance").toString("0").toDouble() * 100);
        item.currentUsage = static_cast<qint64>(info.value("topped_up_balance").toString("0").toDouble() * 100);
        item.unit = isAvailable ? 1 : 0;
        data.quotaLimits.append(item);
    }
    return true;
}
