#include "usagequery.h"
#include "appsettings.h"
#include "androidprefs.h"
#include <QDateTime>
#include <QUrlQuery>
#include <QJsonDocument>

double UsageData::tokenPercentage() const {
    for (const auto &item : quotaLimits) {
        QString t = item.type.toLower();
        if (t.contains("token") || t == "tokens_limit") return item.percentage;
    }
    return -1.0;
}
double UsageData::mcpPercentage() const {
    for (const auto &item : quotaLimits) {
        QString t = item.type.toLower();
        if (t.contains("mcp") || t.contains("time_limit") || t.contains("time")) return item.percentage;
    }
    return -1.0;
}
QString UsageData::expiryInfo() const {
    for (const auto &item : quotaLimits) {
        QString t = item.type.toLower();
        if ((t.contains("mcp") || t.contains("time_limit") || t.contains("time")) && !item.usageDetails.isEmpty())
            return item.usageDetails;
    }
    for (const auto &item : quotaLimits)
        if (!item.usageDetails.isEmpty()) return item.usageDetails;
    return QString();
}
QString UsageData::tokenResetTime() const {
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

void UsageQuery::query()
{
    auto platforms = AppSettings::instance().allPlatforms();
    QList<PlatformConfig> enabled;
    for (const auto &p : platforms)
        if (p.enabled && !p.authToken.isEmpty()) enabled.append(p);

    if (enabled.isEmpty()) {
        emit queryFailed("No platform configured");
        return;
    }

    qDeleteAll(m_active); m_active.clear(); m_completed = 0;

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
            [this, pq](QNetworkReply *r) { onModelUsageReply(pq, r); });
        sendRequest(pq, pq->config.apiPrefix + "/tool-usage", timeQuery,
            [this, pq](QNetworkReply *r) { onToolUsageReply(pq, r); });
        sendRequest(pq, pq->config.apiPrefix + "/quota/limit", "",
            [this, pq](QNetworkReply *r) { onQuotaLimitReply(pq, r); });
    }
}

void UsageQuery::sendRequest(PlatformQuery *pq, const QString &path, const QString &query,
                              std::function<void(QNetworkReply *)> callback, int retry)
{
    QUrl url(pq->config.baseUrl + path + query);
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", pq->config.authToken.toUtf8());
    request.setRawHeader("Accept-Language", "en-US,en");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setTransferTimeout(15000);

    QNetworkReply *reply = m_manager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, pq, path, query, callback, retry]() {
        if (reply->error() != QNetworkReply::NoError && retry < 1) {
            reply->deleteLater();
            sendRequest(pq, path, query, callback, retry + 1);
            return;
        }
        callback(reply);
        reply->deleteLater();
    });
}

void UsageQuery::onModelUsageReply(PlatformQuery *pq, QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonArray arr = QJsonDocument::fromJson(reply->readAll()).object().value("data").toArray();
        for (const QJsonValue &v : arr) {
            QJsonObject o = v.toObject();
            ModelUsageItem item;
            item.model = o["model"].toString(); item.provider = o["provider"].toString();
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
    if (reply->error() == QNetworkReply::NoError) {
        QJsonArray arr = QJsonDocument::fromJson(reply->readAll()).object().value("data").toArray();
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
            QString label = o["label"].toString(), used = o["used"].toString(), total = o["total"].toString();
            parts.append(label.isEmpty() ? (used + "/" + total) : (label + ": " + used + "/" + total));
        }
        return parts.join(" | ");
    }
    return val.isString() ? val.toString() : QString();
}

void UsageQuery::onQuotaLimitReply(PlatformQuery *pq, QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonArray arr = QJsonDocument::fromJson(reply->readAll()).object().value("data").toObject().value("limits").toArray();
        for (const QJsonValue &v : arr) {
            QJsonObject o = v.toObject();
            QuotaLimitItem item;
            item.type = o["type"].toString();
            item.percentage = o["percentage"].toDouble();
            item.currentUsage = o["currentValue"].toVariant().toLongLong();
            item.total = o["usage"].toVariant().toLongLong();
            item.remaining = o["remaining"].toVariant().toLongLong();
            item.nextResetTime = o["nextResetTime"].toVariant().toLongLong();
            item.usageDetails = parseUsageDetails(o["usageDetails"]);
            if (item.nextResetTime > 0)
                item.resetTime = QDateTime::fromMSecsSinceEpoch(item.nextResetTime).toString("HH:mm");
            pq->data.quotaLimits.append(item);
        }
        pq->success++;
    }
    platformDone(pq);
}

void UsageQuery::platformDone(PlatformQuery *pq)
{
    pq->pending--;
    if (pq->pending > 0) return;

    pq->data.isValid = (pq->success > 0);
    if (!pq->data.isValid) pq->data.errorMsg = pq->config.name + " API failed";
    emit queryFinished(pq->data);

    m_completed++;
    if (m_completed >= m_active.size()) {
        qDeleteAll(m_active); m_active.clear();
        emit queryAllFinished();
    }
}

void UsageQuery::setAutoRefresh(int minutes) { m_timer->start(minutes * 60 * 1000); }
void UsageQuery::stopAutoRefresh() { m_timer->stop(); }
