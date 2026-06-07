#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>

struct ModelUsageItem {
    QString model;
    QString provider;
    qint64 inputTokens;
    qint64 outputTokens;
    qint64 totalTokens;
    qint64 requestCount;
};

struct ToolUsageItem {
    QString toolName;
    qint64 callCount;
};

struct QuotaLimitItem {
    QString type;
    double percentage;
    qint64 currentUsage;
    qint64 total;
    qint64 remaining;
    qint64 unit;
    qint64 number;
    QString usageDetails;
    qint64 nextResetTime;
    QString resetTime;
};

struct UsageData {
    QList<ModelUsageItem> modelUsage;
    QList<ToolUsageItem> toolUsage;
    QList<QuotaLimitItem> quotaLimits;
    bool isValid = false;
    QString errorMsg;
    QString platformName;
    QString platformType;

    double tokenPercentage() const;
    double mcpPercentage() const;
    QString expiryInfo() const;
    QString tokenResetTime() const;
    int modelCount() const { return modelUsage.size(); }
    int toolCount() const { return toolUsage.size(); }
    int quotaCount() const { return quotaLimits.size(); }

    qint64 totalTokens() const
    {
        qint64 sum = 0;
        for (const auto &m : modelUsage)
            sum += m.totalTokens;
        return sum;
    }

    qint64 totalRequests() const
    {
        qint64 sum = 0;
        for (const auto &m : modelUsage)
            sum += m.requestCount;
        return sum;
    }

    qint64 totalToolCalls() const
    {
        qint64 sum = 0;
        for (const auto &t : toolUsage)
            sum += t.callCount;
        return sum;
    }

    double balanceTotal(const QString &currency = QString()) const
    {
        for (const auto &q : quotaLimits) {
            if (q.type == "BALANCE_" + (currency.isEmpty() ? "CNY" : currency.toUpper()))
                return q.total / 100.0;
        }
        for (const auto &q : quotaLimits) {
            if (q.type.startsWith("BALANCE_"))
                return q.total / 100.0;
        }
        return -1.0;
    }

    QString balanceCurrency() const
    {
        for (const auto &q : quotaLimits) {
            if (q.type.startsWith("BALANCE_"))
                return q.type.mid(8);
        }
        return QString();
    }

    QJsonObject toJson() const;
    static UsageData fromJson(const QJsonObject &obj);
};

#endif
