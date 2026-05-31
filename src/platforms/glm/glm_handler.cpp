#include "glm_handler.h"
#include "usagequery.h"
#include <QDateTime>
#include <QJsonArray>
#include <QRegularExpression>

QList<PlatformEndpoint> GlmHandler::endpoints(const QString &apiPrefix,
                                               const QString &timeQuery) const
{
    return {
        { apiPrefix + "/model-usage", timeQuery },
        { apiPrefix + "/tool-usage", timeQuery },
        { apiPrefix + "/quota/limit", "" }
    };
}

bool GlmHandler::parse(int endpointIndex, const QJsonObject &root, UsageData &data)
{
    if (!root.contains("data"))
        return false;
    switch (endpointIndex) {
    case 0: return parseModelUsage(root, data);
    case 1: return parseToolUsage(root, data);
    case 2: return parseQuotaLimit(root, data);
    default: return false;
    }
}

bool GlmHandler::parseModelUsage(const QJsonObject &root, UsageData &data)
{
    QJsonArray arr = root.value("data").toArray();
    for (const QJsonValue &v : arr) {
        QJsonObject o = v.toObject();
        ModelUsageItem item;
        item.model = o["model"].toString();
        item.provider = o["provider"].toString();
        item.inputTokens = o["inputTokens"].toVariant().toLongLong();
        item.outputTokens = o["outputTokens"].toVariant().toLongLong();
        item.totalTokens = o["totalTokens"].toVariant().toLongLong();
        item.requestCount = o["requestCount"].toVariant().toLongLong();
        data.modelUsage.append(item);
    }
    return true;
}

bool GlmHandler::parseToolUsage(const QJsonObject &root, UsageData &data)
{
    QJsonArray arr = root.value("data").toArray();
    for (const QJsonValue &v : arr) {
        QJsonObject o = v.toObject();
        ToolUsageItem item;
        item.toolName = o["toolName"].toString();
        item.callCount = o["callCount"].toVariant().toLongLong();
        data.toolUsage.append(item);
    }
    return true;
}

bool GlmHandler::parseQuotaLimit(const QJsonObject &root, UsageData &data)
{
    QJsonObject dataObj = root.value("data").toObject();
    if (!dataObj.contains("limits"))
        return false;

    QJsonArray arr = dataObj.value("limits").toArray();
    for (const QJsonValue &v : arr) {
        QJsonObject o = v.toObject();
        QuotaLimitItem item;
        item.type = o["type"].toString();
        item.percentage = o["percentage"].toDouble();
        item.currentUsage = o["currentValue"].toVariant().toLongLong();
        item.total = o["usage"].toVariant().toLongLong();
        item.remaining = o["remaining"].toVariant().toLongLong();
        item.unit = o["unit"].toVariant().toLongLong();
        item.number = o["number"].toVariant().toLongLong();
        item.nextResetTime = o["nextResetTime"].toVariant().toLongLong();
        item.usageDetails = parseUsageDetails(o["usageDetails"]);
        if (item.nextResetTime > 0)
            item.resetTime = QDateTime::fromMSecsSinceEpoch(item.nextResetTime).toString("HH:mm");
        data.quotaLimits.append(item);
    }
    return true;
}

QString GlmHandler::parseUsageDetails(const QJsonValue &val)
{
    if (val.isArray()) {
        QStringList parts;
        for (const QJsonValue &v : val.toArray()) {
            QJsonObject o = v.toObject();
            QStringList fields;
            for (const QString &key : o.keys()) {
                QString value = o.value(key).toString();
                if (!value.isEmpty())
                    fields.append(key + ": " + value);
            }
            if (!fields.isEmpty())
                parts.append(fields.join(", "));
        }
        return parts.join(" | ");
    }
    if (val.isString()) {
        QString s = val.toString().trimmed();
        return (s.isEmpty() || s == "/"
               || s.replace(QRegularExpression("[/|\\s]"), "").isEmpty())
               ? QString() : s;
    }
    return QString();
}
