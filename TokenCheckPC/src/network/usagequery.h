#ifndef USAGEQUERY_H
#define USAGEQUERY_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include "platformconfig.h"
#include "types.h"

class PlatformHandler;

class UsageQuery : public QObject
{
    Q_OBJECT
public:
    explicit UsageQuery(QObject *parent = nullptr);
    ~UsageQuery();

    void query();
    void setAutoRefresh(int minutes);
    void stopAutoRefresh();
    bool isQuerying() const { return m_querying; }
    void loadCache();

signals:
    void queryFinished(const UsageData &data);
    void queryAllFinished();
    void queryFailed(const QString &error);
    void cachedDataAvailable(const QList<UsageData> &dataList);

private:
    struct PlatformQuery {
        PlatformConfig config;
        PlatformHandler *handler = nullptr;
        int pending = 0;
        int success = 0;
        UsageData data;
        QNetworkReply::NetworkError lastError = QNetworkReply::NoError;
    };

    void abortActiveRequests();
    void applyProxy();
    void queryAllPlatforms(const QString &timeQuery);
    void sendRequest(PlatformQuery *pq, const QString &path, const QString &query,
                     PlatformHandler *handler, int endpointIndex, int retry = 0);
    void platformDone(PlatformQuery *pq);
    QJsonObject parseJsonReply(QNetworkReply *reply, const QString &context);
    static QString errorToDisplay(QNetworkReply::NetworkError err);

    QNetworkAccessManager *m_manager;
    QTimer *m_timer;
    QList<PlatformQuery *> m_active;
    QList<QNetworkReply *> m_pendingReplies;
    int m_completed = 0;
    bool m_querying = false;
};

#endif
