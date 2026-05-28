#include "usagequery.h"
#include "appsettings.h"
#include "androidprefs.h"
#include <QDateTime>
#include <QJsonDocument>
#include <QRegularExpression>

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
    if (m_querying) {
        abortActiveRequests();
    }

    auto platforms = AppSettings::instance().allPlatforms();
    QList<PlatformConfig> enabled;
    for (const auto &p : platforms)
        if (p.enabled && !p.authToken.isEmpty())
            enabled.append(p);

    if (enabled.isEmpty()) {
        emit queryFailed("No platform configured");
        return;
    }

    m_querying = true;

    for (const auto &cfg : enabled) {
        m_active.append(new PlatformQuery{cfg, 3, 0, UsageData()});
        m_active.last()->data.platformName = cfg.name;
    }
    queryAllPlatforms();
}

void UsageQuery::queryAllPlatforms()
{
    QDateTime now = QDateTime::currentDateTime();
    QDateTime start = now.addSecs(-24 * 3600);
    QString timeQuery = QString("?startTime=%1&endTime=%2")
                                .arg(QUrl::toPercentEncoding(start.toString("yyyy-MM-dd HH:mm:ss")),
                                     QUrl::toPercentEncoding(now.toString("yyyy-MM-dd HH:mm:ss")));

    for (auto *pq : m_active) {
        sendRequest(pq, pq->config.apiPrefix + "/model-usage", timeQuery,
                    [this](PlatformQuery *p, QNetworkReply *r) { onModelUsageReply(p, r); });
        sendRequest(pq, pq->config.apiPrefix + "/tool-usage", timeQuery,
                    [this](PlatformQuery *p, QNetworkReply *r) { onToolUsageReply(p, r); });
        sendRequest(pq, pq->config.apiPrefix + "/quota/limit", "",
                    [this](PlatformQuery *p, QNetworkReply *r) { onQuotaLimitReply(p, r); });
    }
}

void UsageQuery::sendRequest(PlatformQuery *pq, const QString &path, const QString &query,
                              std::function<void(PlatformQuery *, QNetworkReply *)> callback,
                              int retry)
{
    QUrl url(pq->config.baseUrl + path + query);
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", pq->config.authToken.toUtf8());
    request.setRawHeader("Accept-Language", "en-US,en");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setTransferTimeout(15000);

    QNetworkReply *reply = m_manager->get(request);
    m_pendingReplies.append(reply);

    connect(reply, &QNetworkReply::finished, this,
            [this, reply, pq, path, query, callback, retry]() {
                m_pendingReplies.removeOne(reply);

                if (!m_active.contains(pq)) {
                    reply->deleteLater();
                    return;
                }

                if (reply->error() != QNetworkReply::NoError) {
                    if (reply->error() == QNetworkReply::TimeoutError) {
                        qWarning() << "Request timeout:" << path;
                    }
                    if (retry < 2) {
                        reply->deleteLater();
                        sendRequest(pq, path, query, callback, retry + 1);
                        return;
                    }
                }
                callback(pq, reply);
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

void UsageQuery::onModelUsageReply(PlatformQuery *pq, QNetworkReply *reply)
{
    QJsonObject root = parseJsonReply(reply, "model-usage");
    if (!root.isEmpty() && root.contains("data")) {
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
            pq->data.modelUsage.append(item);
        }
        pq->success++;
    }
    platformDone(pq);
}

void UsageQuery::onToolUsageReply(PlatformQuery *pq, QNetworkReply *reply)
{
    QJsonObject root = parseJsonReply(reply, "tool-usage");
    if (!root.isEmpty() && root.contains("data")) {
        QJsonArray arr = root.value("data").toArray();
        for (const QJsonValue &v : arr) {
            QJsonObject o = v.toObject();
            ToolUsageItem item;
            item.toolName = o["toolName"].toString();
            item.callCount = o["callCount"].toVariant().toLongLong();
            pq->data.toolUsage.append(item);
        }
        pq->success++;
    }
    platformDone(pq);
}

QString UsageQuery::parseUsageDetails(const QJsonValue &val)
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
        return (s.isEmpty() || s == "/" || s.replace(QRegularExpression("[/|\\s]"), "").isEmpty())
               ? QString() : s;
    }
    return QString();
}

void UsageQuery::onQuotaLimitReply(PlatformQuery *pq, QNetworkReply *reply)
{
    QJsonObject root = parseJsonReply(reply, "quota-limit");
    if (!root.isEmpty() && root.contains("data")) {
        QJsonObject dataObj = root.value("data").toObject();
        if (dataObj.contains("limits")) {
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
                pq->data.quotaLimits.append(item);
            }
            pq->success++;
        }
    }
    platformDone(pq);
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
