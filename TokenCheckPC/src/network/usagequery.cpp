#include "usagequery.h"
#include "appsettings.h"
#include "platform_registry.h"
#include "platform_handler.h"
#include <QDateTime>
#include <QJsonDocument>
#include <QUrl>
#include <QNetworkProxy>
#include <QSslSocket>
#include <QSslConfiguration>

UsageQuery::UsageQuery(QObject *parent)
    : QObject(parent), m_manager(new QNetworkAccessManager(this)), m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &UsageQuery::query);
}

UsageQuery::~UsageQuery()
{
    abortActiveRequests();
}

void UsageQuery::applyProxy()
{
    int type = AppSettings::instance().proxyType();
    if (type == 0) {
        m_manager->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
    } else {
        QNetworkProxy proxy;
        proxy.setType(type == 1 ? QNetworkProxy::HttpProxy : QNetworkProxy::Socks5Proxy);
        proxy.setHostName(AppSettings::instance().proxyHost());
        proxy.setPort(static_cast<quint16>(AppSettings::instance().proxyPort()));
        m_manager->setProxy(proxy);
    }
}

void UsageQuery::abortActiveRequests()
{
    for (auto *reply : m_pendingReplies) {
        reply->abort();
        reply->deleteLater();
    }
    m_pendingReplies.clear();
    qDeleteAll(m_active);
    m_active.clear();
    m_completed = 0;
    m_querying = false;
}

QString UsageQuery::errorToDisplay(QNetworkReply::NetworkError err)
{
    switch (err) {
    case QNetworkReply::AuthenticationRequiredError:
        return "Auth Failed";
    case QNetworkReply::TimeoutError:
        return "Timeout";
    case QNetworkReply::HostNotFoundError:
        return "No Network";
    case QNetworkReply::ConnectionRefusedError:
        return "Connection Refused";
    case QNetworkReply::SslHandshakeFailedError:
        return "SSL Error";
    default:
        return "Error";
    }
}

void UsageQuery::query()
{
    if (m_querying)
        abortActiveRequests();

    PlatformRegistry::init();
    applyProxy();

    auto platforms = AppSettings::instance().allPlatforms();
    QList<PlatformConfig> enabled;
    for (const auto &p : platforms) {
        if (p.enabled && !p.authToken.isEmpty())
            enabled.append(p);
    }

    if (enabled.isEmpty()) {
        emit queryFailed("No platform configured");
        return;
    }

    m_querying = true;

    for (const auto &cfg : enabled) {
        auto *handler = PlatformRegistry::instance().handler(cfg.platformType);
        if (!handler)
            continue;
        auto *pq = new PlatformQuery{cfg, handler, 0, 0, UsageData(), QNetworkReply::NoError};
        pq->data.platformName = cfg.name;
        pq->data.platformType = cfg.platformType;
        m_active.append(pq);
    }

    QDateTime now = QDateTime::currentDateTime();
    QDateTime start = now.addSecs(-24 * 3600);
    QString timeQuery = QString("?startTime=%1&endTime=%2")
                                .arg(QUrl::toPercentEncoding(start.toString("yyyy-MM-dd HH:mm:ss")),
                                     QUrl::toPercentEncoding(now.toString("yyyy-MM-dd HH:mm:ss")));
    queryAllPlatforms(timeQuery);
}

void UsageQuery::queryAllPlatforms(const QString &timeQuery)
{
    for (auto *pq : m_active) {
        auto eps = pq->handler->endpoints(pq->config.apiPrefix, timeQuery);
        pq->pending = eps.size();
        for (int i = 0; i < eps.size(); i++)
            sendRequest(pq, eps[i].path, eps[i].query, pq->handler, i);
    }
}

void UsageQuery::sendRequest(PlatformQuery *pq, const QString &path, const QString &query,
                              PlatformHandler *handler, int endpointIndex, int retry)
{
    QUrl url(pq->config.baseUrl + path + query);
    QNetworkRequest request(url);
    QByteArray authHeader = pq->config.authToken.toUtf8();
    QString prefix = handler->authHeaderPrefix();
    if (!prefix.isEmpty() && !authHeader.startsWith(prefix.toUtf8()))
        authHeader = prefix.toUtf8() + authHeader;
    request.setRawHeader("Authorization", authHeader);
    request.setRawHeader("Accept-Language", "en-US,en");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setTransferTimeout(15000);

    QNetworkReply *reply = m_manager->get(request);
    m_pendingReplies.append(reply);

    connect(reply, &QNetworkReply::sslErrors, this,
            [reply](const QList<QSslError> &errors) {
                qWarning() << "SSL Errors:";
                for (const auto &e : errors)
                    qWarning() << "  " << e.errorString();
                reply->ignoreSslErrors();
            });

    connect(reply, &QNetworkReply::finished, this,
            [this, reply, pq, path, query, handler, endpointIndex, retry]() {
                m_pendingReplies.removeOne(reply);
                if (!m_active.contains(pq)) {
                    reply->deleteLater();
                    return;
                }
                if (reply->error() != QNetworkReply::NoError) {
                    pq->lastError = reply->error();
                    if (retry < 2) {
                        reply->deleteLater();
                        sendRequest(pq, path, query, handler, endpointIndex, retry + 1);
                        return;
                    }
                }
                QJsonObject root = parseJsonReply(reply, "api");
                if (handler->parse(endpointIndex, root, pq->data))
                    pq->success++;
                platformDone(pq);
                reply->deleteLater();
            });
}

QJsonObject UsageQuery::parseJsonReply(QNetworkReply *reply, const QString &context)
{
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Network error in" << context << ":" << reply->errorString();
        return QJsonObject();
    }
    QByteArray data = reply->readAll();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error in" << context << ":" << err.errorString();
        return QJsonObject();
    }
    return doc.object();
}

void UsageQuery::platformDone(PlatformQuery *pq)
{
    pq->pending--;
    if (pq->pending > 0)
        return;

    pq->data.isValid = (pq->success > 0);
    if (!pq->data.isValid)
        pq->data.errorMsg = pq->config.name + ": " + errorToDisplay(pq->lastError);
    emit queryFinished(pq->data);

    m_completed++;
    if (m_completed >= m_active.size()) {
        m_querying = false;

        QJsonArray cacheArr;
        for (auto *aq : m_active)
            cacheArr.append(aq->data.toJson());
        AppSettings::instance().cacheUsageData(
            QString::fromUtf8(QJsonDocument(cacheArr).toJson(QJsonDocument::Compact)));

        qDeleteAll(m_active);
        m_active.clear();
        m_completed = 0;
        emit queryAllFinished();
    }
}

void UsageQuery::setAutoRefresh(int minutes)
{
    m_timer->start(minutes * 60 * 1000);
}

void UsageQuery::stopAutoRefresh()
{
    m_timer->stop();
}

void UsageQuery::loadCache()
{
    QString cached = AppSettings::instance().loadCachedUsageData();
    if (cached.isEmpty())
        return;
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(cached.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError)
        return;
    QJsonObject root = doc.object();
    QJsonArray arr = root["data"].toArray();
    QList<UsageData> dataList;
    for (const QJsonValue &v : arr)
        dataList.append(UsageData::fromJson(v.toObject()));
    if (!dataList.isEmpty())
        emit cachedDataAvailable(dataList);
}
