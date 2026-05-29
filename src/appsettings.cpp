#include "appsettings.h"
#include "androidprefs.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>

AppSettings::AppSettings(QObject *parent) : QObject(parent)
{
    m_filePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                 + "/settings.json";
    QDir().mkpath(QFileInfo(m_filePath).absolutePath());
    load();
}

AppSettings &AppSettings::instance()
{
    static AppSettings s;
    return s;
}

int AppSettings::platformCount() const { return m_platforms.size(); }

QString AppSettings::platformName(int i) const
{
    return (i >= 0 && i < m_platforms.size()) ? m_platforms[i].name : QString();
}

QString AppSettings::platformBaseUrl(int i) const
{
    return (i >= 0 && i < m_platforms.size()) ? m_platforms[i].baseUrl : QString();
}

QString AppSettings::platformAuthToken(int i) const
{
    return (i >= 0 && i < m_platforms.size()) ? m_platforms[i].authToken : QString();
}

QString AppSettings::platformApiPrefix(int i) const
{
    return (i >= 0 && i < m_platforms.size()) ? m_platforms[i].apiPrefix : QString();
}

bool AppSettings::platformEnabled(int i) const
{
    return (i >= 0 && i < m_platforms.size()) ? m_platforms[i].enabled : false;
}

void AppSettings::setPlatform(int i, const QString &name, const QString &baseUrl,
                               const QString &token, const QString &apiPrefix, bool enabled)
{
    if (i >= 0 && i < m_platforms.size()) {
        m_platforms[i].name = name;
        m_platforms[i].baseUrl = baseUrl;
        m_platforms[i].authToken = token;
        m_platforms[i].apiPrefix = apiPrefix;
        m_platforms[i].enabled = enabled;
        save();
        emit platformsChanged();
    }
}

void AppSettings::addPlatform(const QString &name, const QString &baseUrl,
                               const QString &token, const QString &apiPrefix,
                               bool enabled)
{
    PlatformConfig pc;
    pc.name = name;
    pc.baseUrl = baseUrl;
    pc.authToken = token;
    pc.apiPrefix = apiPrefix.isEmpty() ? "/api/monitor/usage" : apiPrefix;
    pc.enabled = enabled;
    m_platforms.append(pc);
    save();
    emit platformsChanged();
}

void AppSettings::removePlatform(int i)
{
    if (i >= 0 && i < m_platforms.size()) {
        m_platforms.removeAt(i);
        save();
        emit platformsChanged();
    }
}

PlatformConfig AppSettings::platformAt(int i) const
{
    if (i >= 0 && i < m_platforms.size())
        return m_platforms[i];
    return PlatformConfig();
}

QList<PlatformConfig> AppSettings::allPlatforms() const { return m_platforms; }

int AppSettings::autoRefreshInterval() const { return m_autoRefreshInterval; }
void AppSettings::setAutoRefreshInterval(int minutes)
{
    m_autoRefreshInterval = minutes;
    save();
    emit settingsChanged();
}

int AppSettings::widgetFontSize() const { return m_widgetFontSize; }
void AppSettings::setWidgetFontSize(int size)
{
    m_widgetFontSize = size;
    save();
    emit settingsChanged();
}

int AppSettings::widgetShowToken() const { return m_widgetShowToken; }
void AppSettings::setWidgetShowToken(int show)
{
    m_widgetShowToken = show;
    save();
    emit settingsChanged();
}

int AppSettings::widgetShowMcp() const { return m_widgetShowMcp; }
void AppSettings::setWidgetShowMcp(int show)
{
    m_widgetShowMcp = show;
    save();
    emit settingsChanged();
}

int AppSettings::widgetShowTime() const { return m_widgetShowTime; }
void AppSettings::setWidgetShowTime(int show)
{
    m_widgetShowTime = show;
    save();
    emit settingsChanged();
}

int AppSettings::widgetShowBalance() const { return m_widgetShowBalance; }
void AppSettings::setWidgetShowBalance(int show)
{
    m_widgetShowBalance = show;
    save();
    emit settingsChanged();
}

int AppSettings::widgetShowGranted() const { return m_widgetShowGranted; }
void AppSettings::setWidgetShowGranted(int show)
{
    m_widgetShowGranted = show;
    save();
    emit settingsChanged();
}

bool AppSettings::isConfigured() const
{
    for (const auto &p : m_platforms) {
        if (p.enabled && !p.authToken.isEmpty())
            return true;
    }
    return false;
}

void AppSettings::cacheUsageData(const QString &json)
{
    QFile f(QFileInfo(m_filePath).absolutePath() + "/usage_cache.json");
    if (f.open(QIODevice::WriteOnly)) {
        QJsonObject root;
        root["timestamp"] = static_cast<double>(QDateTime::currentMSecsSinceEpoch());
        root["data"] = QJsonDocument::fromJson(json.toUtf8()).array();
        f.write(QJsonDocument(root).toJson(QJsonDocument::Compact));
    }
}

QString AppSettings::loadCachedUsageData() const
{
    QFile f(QFileInfo(m_filePath).absolutePath() + "/usage_cache.json");
    if (!f.open(QIODevice::ReadOnly))
        return QString();
    return QString::fromUtf8(f.readAll());
}

void AppSettings::load()
{
    QFile f(m_filePath);
    if (!f.open(QIODevice::ReadOnly))
        return;

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &err);
    if (err.error != QJsonParseError::NoError)
        return;

    QJsonObject root = doc.object();
    QJsonArray arr = root["platforms"].toArray();
    for (const QJsonValue &v : arr)
        m_platforms.append(PlatformConfig::fromJson(v.toObject()));

    m_autoRefreshInterval = root["autoRefreshInterval"].toInt(5);
    m_widgetFontSize = root["widgetFontSize"].toInt(14);
    m_widgetShowToken = root["widgetShowToken"].toInt(1);
    m_widgetShowMcp = root["widgetShowMcp"].toInt(1);
    m_widgetShowTime = root["widgetShowTime"].toInt(1);
    m_widgetShowBalance = root["widgetShowBalance"].toInt(1);
    m_widgetShowGranted = root["widgetShowGranted"].toInt(1);
}

void AppSettings::save()
{
    QJsonObject root;
    QJsonArray arr;
    for (const auto &p : m_platforms)
        arr.append(p.toJson());
    root["platforms"] = arr;
    root["autoRefreshInterval"] = m_autoRefreshInterval;
    root["widgetFontSize"] = m_widgetFontSize;
    root["widgetShowToken"] = m_widgetShowToken;
    root["widgetShowMcp"] = m_widgetShowMcp;
    root["widgetShowTime"] = m_widgetShowTime;
    root["widgetShowBalance"] = m_widgetShowBalance;
    root["widgetShowGranted"] = m_widgetShowGranted;

    QFile f(m_filePath);
    if (f.open(QIODevice::WriteOnly))
        f.write(QJsonDocument(root).toJson());
}

void AppSettings::syncWidgetConfig()
{
    save();
    AndroidPrefs::writeInt("widgetShowToken", m_widgetShowToken);
    AndroidPrefs::writeInt("widgetShowMcp", m_widgetShowMcp);
    AndroidPrefs::writeInt("widgetShowTime", m_widgetShowTime);
    AndroidPrefs::writeInt("widgetShowBalance", m_widgetShowBalance);
    AndroidPrefs::writeInt("widgetShowGranted", m_widgetShowGranted);
    AndroidPrefs::writeInt("widgetFontSize", m_widgetFontSize);

    QJsonArray arr;
    for (const auto &p : m_platforms)
        if (p.enabled && !p.authToken.isEmpty())
            arr.append(p.toJson());
    AndroidPrefs::write("platformConfigs",
                        QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact)));

    AndroidPrefs::notifyWidgetUpdate();
}
