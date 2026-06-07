#include "types.h"

double UsageData::tokenPercentage() const
{
    for (const auto &item : quotaLimits) {
        QString t = item.type.toLower();
        if (t.contains("token") || t == "tokens_limit")
            return item.percentage;
    }
    return -1.0;
}

double UsageData::mcpPercentage() const
{
    for (const auto &item : quotaLimits) {
        QString t = item.type.toLower();
        if (t.contains("mcp") || t.contains("time_limit") || t.contains("time"))
            return item.percentage;
    }
    return -1.0;
}

QString UsageData::expiryInfo() const
{
    for (const auto &item : quotaLimits) {
        QString t = item.type.toLower();
        if ((t.contains("mcp") || t.contains("time_limit") || t.contains("time"))
            && !item.usageDetails.isEmpty())
            return item.usageDetails;
    }
    for (const auto &item : quotaLimits)
        if (!item.usageDetails.isEmpty())
            return item.usageDetails;
    return QString();
}

QString UsageData::tokenResetTime() const
{
    for (const auto &item : quotaLimits) {
        QString t = item.type.toLower();
        if ((t.contains("token") || t == "tokens_limit") && !item.resetTime.isEmpty())
            return item.resetTime;
    }
    return QString();
}

QJsonObject UsageData::toJson() const
{
    QJsonObject obj;
    obj["platformName"] = platformName;
    obj["platformType"] = platformType;
    obj["isValid"] = isValid;
    obj["errorMsg"] = errorMsg;

    QJsonArray models;
    for (const auto &m : modelUsage) {
        QJsonObject o;
        o["model"] = m.model;
        o["provider"] = m.provider;
        o["inputTokens"] = m.inputTokens;
        o["outputTokens"] = m.outputTokens;
        o["totalTokens"] = m.totalTokens;
        o["requestCount"] = m.requestCount;
        models.append(o);
    }
    obj["models"] = models;

    QJsonArray tools;
    for (const auto &t : toolUsage) {
        QJsonObject o;
        o["toolName"] = t.toolName;
        o["callCount"] = t.callCount;
        tools.append(o);
    }
    obj["tools"] = tools;

    QJsonArray quotas;
    for (const auto &q : quotaLimits) {
        QJsonObject o;
        o["type"] = q.type;
        o["percentage"] = q.percentage;
        o["currentUsage"] = q.currentUsage;
        o["total"] = q.total;
        o["remaining"] = q.remaining;
        o["unit"] = q.unit;
        o["number"] = q.number;
        o["resetTime"] = q.resetTime;
        o["usageDetails"] = q.usageDetails;
        quotas.append(o);
    }
    obj["quotas"] = quotas;
    return obj;
}

UsageData UsageData::fromJson(const QJsonObject &obj)
{
    UsageData d;
    d.platformName = obj["platformName"].toString();
    d.platformType = obj["platformType"].toString();
    d.isValid = obj["isValid"].toBool();
    d.errorMsg = obj["errorMsg"].toString();

    for (const QJsonValue &v : obj["models"].toArray()) {
        QJsonObject o = v.toObject();
        ModelUsageItem m;
        m.model = o["model"].toString();
        m.provider = o["provider"].toString();
        m.inputTokens = o["inputTokens"].toVariant().toLongLong();
        m.outputTokens = o["outputTokens"].toVariant().toLongLong();
        m.totalTokens = o["totalTokens"].toVariant().toLongLong();
        m.requestCount = o["requestCount"].toVariant().toLongLong();
        d.modelUsage.append(m);
    }

    for (const QJsonValue &v : obj["tools"].toArray()) {
        QJsonObject o = v.toObject();
        ToolUsageItem t;
        t.toolName = o["toolName"].toString();
        t.callCount = o["callCount"].toVariant().toLongLong();
        d.toolUsage.append(t);
    }

    for (const QJsonValue &v : obj["quotas"].toArray()) {
        QJsonObject o = v.toObject();
        QuotaLimitItem q;
        q.type = o["type"].toString();
        q.percentage = o["percentage"].toDouble();
        q.currentUsage = o["currentUsage"].toVariant().toLongLong();
        q.total = o["total"].toVariant().toLongLong();
        q.remaining = o["remaining"].toVariant().toLongLong();
        q.unit = o["unit"].toVariant().toLongLong();
        q.number = o["number"].toVariant().toLongLong();
        q.resetTime = o["resetTime"].toString();
        q.usageDetails = o["usageDetails"].toString();
        d.quotaLimits.append(q);
    }
    return d;
}
