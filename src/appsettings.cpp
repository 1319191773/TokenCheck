#include "appsettings.h"
#include "androidprefs.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>

AppSettings::AppSettings(QObject *parent)
    : QObject(parent)
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

QString AppSettings::platformName(int i) const {
    return (i >= 0 && i < m_platforms.size()) ? m_platforms[i].name : QString();
}
QString AppSettings::platformBaseUrl(int i) const {
    return (i >= 0 && i < m_platforms.size()) ? m_platforms[i].baseUrl : QString();
}
QString AppSettings::platformAuthToken(int i) const {
    return (i >= 0 && i < m_platforms.size()) ? m_platforms[i].authToken : QString();
}
QString AppSettings::platformApiPrefix(int i) const {
    return (i >= 0 && i < m_platforms.size()) ? m_platforms[i].apiPrefix : QString();
}
bool AppSettings::platformEnabled(int i) const {
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
    }
}

void AppSettings::addPlatform(const QString &name, const QString &baseUrl,
                               const QString &token, const QString &apiPrefix)
{
    PlatformConfig pc;
    pc.name = name;
    pc.baseUrl = baseUrl;
    pc.authToken = token;
    pc.apiPrefix = apiPrefix.isEmpty() ? "/api/monitor/usage" : apiPrefix;
    pc.enabled = true;
    m_platforms.append(pc);
    save();
}

void AppSettings::removePlatform(int i)
{
    if (i >= 0 && i < m_platforms.size()) {
        m_platforms.removeAt(i);
        save();
    }
}

PlatformConfig AppSettings::platformAt(int i) const
{
    if (i >= 0 && i < m_platforms.size())
        return m_platforms[i];
    return PlatformConfig();
}

QList<PlatformConfig> AppSettings::allPlatforms() const
{
    return m_platforms;
}

int AppSettings::autoRefreshInterval() const { return m_autoRefreshInterval; }
void AppSettings::setAutoRefreshInterval(int minutes) { m_autoRefreshInterval = minutes; save(); }

int AppSettings::widgetFontSize() const { return m_widgetFontSize; }
void AppSettings::setWidgetFontSize(int size) { m_widgetFontSize = size; save(); }

int AppSettings::widgetShowToken() const { return m_widgetShowToken; }
void AppSettings::setWidgetShowToken(int show) { m_widgetShowToken = show; save(); }

int AppSettings::widgetShowMcp() const { return m_widgetShowMcp; }
void AppSettings::setWidgetShowMcp(int show) { m_widgetShowMcp = show; save(); }

int AppSettings::widgetShowTime() const { return m_widgetShowTime; }
void AppSettings::setWidgetShowTime(int show) { m_widgetShowTime = show; save(); }

bool AppSettings::isConfigured() const
{
    for (const auto &p : m_platforms) {
        if (p.enabled && !p.authToken.isEmpty())
            return true;
    }
    return false;
}

void AppSettings::load()
{
    QFile f(m_filePath);
    if (!f.open(QIODevice::ReadOnly))
        return;

    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    QJsonObject root = doc.object();

    QJsonArray arr = root["platforms"].toArray();
    for (const QJsonValue &v : arr)
        m_platforms.append(PlatformConfig::fromJson(v.toObject()));

    m_autoRefreshInterval = root["autoRefreshInterval"].toInt(5);
    m_widgetFontSize = root["widgetFontSize"].toInt(14);
    m_widgetShowToken = root["widgetShowToken"].toInt(1);
    m_widgetShowMcp = root["widgetShowMcp"].toInt(1);
    m_widgetShowTime = root["widgetShowTime"].toInt(1);
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

    QJsonArray arr;
    for (const auto &p : m_platforms)
        if (p.enabled && !p.authToken.isEmpty()) arr.append(p.toJson());
    AndroidPrefs::write("platformConfigs", QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact)));

    AndroidPrefs::notifyWidgetUpdate();
}
