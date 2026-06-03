#include "usagequery.h"
#include "appsettings.h"
#include "platform_registry.h"
#include "platform_handler.h"
#include <QDateTime>
#include <QJsonDocument>
#include <QUrl>

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

UsageQuery::UsageQuery(QObject *parent)
    : QObject(parent), m_manager(new QNetworkAccessManager(this)), m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &UsageQuery::query);
}

UsageQuery::~UsageQuery()
{
    abortActiveRequests();
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

void UsageQuery::query()
{
    if (m_querying)
        abortActiveRequests();

    PlatformRegistry::init();

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
        auto ptype = cfg.platformType;
        auto *handler = PlatformRegistry::instance().handler(ptype);
        if (!handler)
            continue;

        auto *pq = new PlatformQuery{cfg, handler, 0, 0, UsageData()};
        pq->data.platformName = cfg.name;
        pq->data.platformType = ptype;
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
        auto endpoints = pq->handler->endpoints(pq->config.apiPrefix, timeQuery);
        pq->pending = endpoints.size();
        for (int i = 0; i < endpoints.size(); i++) {
            sendRequest(pq, endpoints[i].path, endpoints[i].query,
                        pq->handler, i);
        }
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

    connect(reply, &QNetworkReply::finished, this,
            [this, reply, pq, path, query, handler, endpointIndex, retry]() {
                m_pendingReplies.removeOne(reply);

                if (!m_active.contains(pq)) {
                    reply->deleteLater();
                    return;
                }

                if (reply->error() != QNetworkReply::NoError) {
                    if (reply->error() == QNetworkReply::TimeoutError)
                        qWarning() << "Request timeout:" << path;
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
        pq->data.errorMsg = pq->config.name + " API failed";
    emit queryFinished(pq->data);

    m_completed++;
    if (m_completed >= m_active.size()) {
        m_querying = false;

        QJsonArray cacheArr;
        for (auto *aq : m_active)
            cacheArr.append(usageDataToJson(aq->data));
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
    if (!cached.isEmpty())
        emit cachedDataAvailable(cached);
}

QJsonObject UsageQuery::usageDataToJson(const UsageData &data)
{
    QJsonObject obj;
    obj["platformName"] = data.platformName;
    obj["platformType"] = data.platformType;
    obj["isValid"] = data.isValid;
    obj["errorMsg"] = data.errorMsg;

    QJsonArray models;
    for (int i = 0; i < data.modelCount(); i++) {
        QJsonObject m;
        m["name"] = data.modelNames()[i];
        m["provider"] = data.modelProvider(i);
        m["inputTokens"] = static_cast<qint64>(data.modelInputTokens(i));
        m["outputTokens"] = static_cast<qint64>(data.modelOutputTokens(i));
        m["tokens"] = static_cast<qint64>(data.modelTokens(i));
        m["requests"] = static_cast<qint64>(data.modelRequests(i));
        models.append(m);
    }
    obj["models"] = models;

    QJsonArray tools;
    for (int i = 0; i < data.toolCount(); i++) {
        QJsonObject t;
        t["name"] = data.toolNames()[i];
        t["calls"] = static_cast<qint64>(data.toolCalls(i));
        tools.append(t);
    }
    obj["tools"] = tools;

    QJsonArray quotas;
    for (int i = 0; i < data.quotaCount(); i++) {
        QJsonObject q;
        q["type"] = data.quotaType(i);
        q["percentage"] = data.quotaPercentage(i);
        q["currentUsage"] = static_cast<qint64>(data.quotaCurrentUsage(i));
        q["total"] = static_cast<qint64>(data.quotaTotal(i));
        q["remaining"] = static_cast<qint64>(data.quotaRemaining(i));
        q["unit"] = static_cast<qint64>(data.quotaUnit(i));
        q["number"] = static_cast<qint64>(data.quotaNumber(i));
        q["resetTime"] = data.quotaResetTime(i);
        q["usageDetails"] = data.quotaUsageDetails(i);
        quotas.append(q);
    }
    obj["quotas"] = quotas;

    obj["tokenPct"] = data.tokenPercentage();
    obj["mcpPct"] = data.mcpPercentage();
    obj["resetTime"] = data.tokenResetTime();

    return obj;
}
