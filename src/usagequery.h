#ifndef USAGEQUERY_H
#define USAGEQUERY_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>
#include "platformconfig.h"

struct ModelUsageItem { Q_GADGET public:
    QString model; QString provider; qint64 inputTokens; qint64 outputTokens; qint64 totalTokens; qint64 requestCount;
};
struct ToolUsageItem { Q_GADGET public:
    QString toolName; qint64 callCount;
};
struct QuotaLimitItem { Q_GADGET public:
    QString type; double percentage; qint64 currentUsage; qint64 total; qint64 remaining;
    QString usageDetails; qint64 nextResetTime; QString resetTime;
};
struct UsageData { Q_GADGET public:
    QList<ModelUsageItem> modelUsage; QList<ToolUsageItem> toolUsage; QList<QuotaLimitItem> quotaLimits;
    Q_PROPERTY(bool isValid MEMBER isValid) Q_PROPERTY(QString errorMsg MEMBER errorMsg)
    Q_PROPERTY(QString platformName MEMBER platformName)
    bool isValid = false; QString errorMsg; QString platformName;
    Q_INVOKABLE double tokenPercentage() const;
    Q_INVOKABLE double mcpPercentage() const;
    Q_INVOKABLE QString expiryInfo() const;
    Q_INVOKABLE QString tokenResetTime() const;
    Q_INVOKABLE int modelCount() const { return modelUsage.size(); }
    Q_INVOKABLE int toolCount() const { return toolUsage.size(); }
    Q_INVOKABLE QStringList modelNames() const {
        QStringList l; for (auto &m : modelUsage) l << m.model; return l;
    }
    Q_INVOKABLE qint64 modelTokens(int i) const { return (i>=0 && i<modelUsage.size()) ? modelUsage[i].totalTokens : 0; }
    Q_INVOKABLE qint64 modelRequests(int i) const { return (i>=0 && i<modelUsage.size()) ? modelUsage[i].requestCount : 0; }
    Q_INVOKABLE QStringList toolNames() const {
        QStringList l; for (auto &t : toolUsage) l << t.toolName; return l;
    }
    Q_INVOKABLE qint64 toolCalls(int i) const { return (i>=0 && i<toolUsage.size()) ? toolUsage[i].callCount : 0; }
};

class UsageQuery : public QObject
{
    Q_OBJECT
public:
    explicit UsageQuery(QObject *parent = nullptr);
    Q_INVOKABLE void query();
    Q_INVOKABLE void setAutoRefresh(int minutes);
    Q_INVOKABLE void stopAutoRefresh();

signals:
    void queryFinished(const UsageData &data);
    void queryAllFinished();
    void queryFailed(const QString &error);

private:
    struct PlatformQuery {
        PlatformConfig config;
        int pending = 3;
        int success = 0;
        UsageData data;
    };
    void queryAllPlatforms();
    void sendRequest(PlatformQuery *pq, const QString &path, const QString &query,
                     std::function<void(QNetworkReply *)> callback, int retry = 0);
    void onModelUsageReply(PlatformQuery *pq, QNetworkReply *reply);
    void onToolUsageReply(PlatformQuery *pq, QNetworkReply *reply);
    void onQuotaLimitReply(PlatformQuery *pq, QNetworkReply *reply);
    void platformDone(PlatformQuery *pq);
    QString parseUsageDetails(const QJsonValue &val);

    QNetworkAccessManager *m_manager;
    QTimer *m_timer;
    QList<PlatformQuery *> m_active;
    int m_completed = 0;
};

#endif
