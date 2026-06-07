#include "appsettings.h"
#include "platform_registry.h"
#include "theme.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>
#include <QCoreApplication>
#include <QSettings>

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

PlatformConfig AppSettings::platformAt(int i) const
{
    if (i >= 0 && i < m_platforms.size())
        return m_platforms[i];
    return PlatformConfig();
}

QList<PlatformConfig> AppSettings::allPlatforms() const { return m_platforms; }

void AppSettings::setPlatform(int i, const PlatformConfig &config)
{
    if (i >= 0 && i < m_platforms.size()) {
        m_platforms[i] = config;
        save();
        emit platformsChanged();
    }
}

void AppSettings::addPlatform(const PlatformConfig &config)
{
    PlatformConfig pc = config;
    if (pc.platformType.isEmpty()) {
        QString lower = pc.baseUrl.toLower();
        pc.platformType = lower.contains("deepseek") ? "deepseek" : "glm";
    }
    if (pc.apiPrefix.isEmpty()) {
        PlatformRegistry::init();
        pc.apiPrefix = PlatformRegistry::instance().defaultApiPrefix(pc.platformType);
    }
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

int AppSettings::autoRefreshInterval() const { return m_autoRefreshInterval; }
void AppSettings::setAutoRefreshInterval(int minutes)
{
    m_autoRefreshInterval = minutes;
    save();
}

int AppSettings::ballSize() const { return m_ballSize; }
void AppSettings::setBallSize(int size)
{
    m_ballSize = size;
    save();
}

QPoint AppSettings::ballPosition() const { return m_ballPos; }
void AppSettings::setBallPosition(const QPoint &pos)
{
    m_ballPos = pos;
    save();
}

int AppSettings::pctFontSize() const { return m_pctFontSize; }
void AppSettings::setPctFontSize(int size)
{
    m_pctFontSize = size;
    save();
}

QColor AppSettings::pctColor() const { return m_pctColor; }
void AppSettings::setPctColor(const QColor &color)
{
    m_pctColor = color;
    save();
}

int AppSettings::timeFontSize() const { return m_timeFontSize; }
void AppSettings::setTimeFontSize(int size)
{
    m_timeFontSize = size;
    save();
}

QColor AppSettings::timeColor() const { return m_timeColor; }
void AppSettings::setTimeColor(const QColor &color)
{
    m_timeColor = color;
    save();
}

bool AppSettings::autoStart() const { return m_autoStart; }
void AppSettings::setAutoStart(bool enabled)
{
    m_autoStart = enabled;
    save();
    applyAutoStart(enabled);
}

int AppSettings::notifyThreshold() const { return m_notifyThreshold; }
void AppSettings::setNotifyThreshold(int threshold)
{
    m_notifyThreshold = threshold;
    save();
}

QString AppSettings::hotkeyToggle() const { return m_hotkeyToggle; }
void AppSettings::setHotkeyToggle(const QString &key)
{
    m_hotkeyToggle = key;
    save();
}

QString AppSettings::hotkeyRefresh() const { return m_hotkeyRefresh; }
void AppSettings::setHotkeyRefresh(const QString &key)
{
    m_hotkeyRefresh = key;
    save();
}

int AppSettings::proxyType() const { return m_proxyType; }
void AppSettings::setProxyType(int type)
{
    m_proxyType = type;
    save();
}

QString AppSettings::proxyHost() const { return m_proxyHost; }
void AppSettings::setProxyHost(const QString &host)
{
    m_proxyHost = host;
    save();
}

int AppSettings::proxyPort() const { return m_proxyPort; }
void AppSettings::setProxyPort(int port)
{
    m_proxyPort = port;
    save();
}

ThemeId AppSettings::themeId() const
{
    return m_themeId == 1 ? ThemeId::Light : ThemeId::Dark;
}

void AppSettings::setThemeId(ThemeId id)
{
    m_themeId = (id == ThemeId::Light) ? 1 : 0;
    save();
}

QString AppSettings::language() const
{
    return m_language;
}

void AppSettings::setLanguage(const QString &lang)
{
    m_language = lang;
    save();
}

int AppSettings::ballBgOpacity() const { return m_ballBgOpacity; }
void AppSettings::setBallBgOpacity(int o) { m_ballBgOpacity = o; save(); }

int AppSettings::ringWidth() const { return m_ringWidth; }
void AppSettings::setRingWidth(int w) { m_ringWidth = w; save(); }

QColor AppSettings::ballBgColor() const { return m_ballBgColor; }
void AppSettings::setBallBgColor(const QColor &c) { m_ballBgColor = c; save(); }

int AppSettings::glmGreenThreshold() const { return m_glmGreenThreshold; }
void AppSettings::setGlmGreenThreshold(int v) { m_glmGreenThreshold = v; save(); }
int AppSettings::glmYellowThreshold() const { return m_glmYellowThreshold; }
void AppSettings::setGlmYellowThreshold(int v) { m_glmYellowThreshold = v; save(); }
QColor AppSettings::glmGreenColor() const { return m_glmGreenColor; }
void AppSettings::setGlmGreenColor(const QColor &c) { m_glmGreenColor = c; save(); }
QColor AppSettings::glmYellowColor() const { return m_glmYellowColor; }
void AppSettings::setGlmYellowColor(const QColor &c) { m_glmYellowColor = c; save(); }
QColor AppSettings::glmRedColor() const { return m_glmRedColor; }
void AppSettings::setGlmRedColor(const QColor &c) { m_glmRedColor = c; save(); }

double AppSettings::dsGreenThreshold() const { return m_dsGreenThreshold; }
void AppSettings::setDsGreenThreshold(double v) { m_dsGreenThreshold = v; save(); }
double AppSettings::dsYellowThreshold() const { return m_dsYellowThreshold; }
void AppSettings::setDsYellowThreshold(double v) { m_dsYellowThreshold = v; save(); }
QColor AppSettings::dsGreenColor() const { return m_dsGreenColor; }
void AppSettings::setDsGreenColor(const QColor &c) { m_dsGreenColor = c; save(); }
QColor AppSettings::dsYellowColor() const { return m_dsYellowColor; }
void AppSettings::setDsYellowColor(const QColor &c) { m_dsYellowColor = c; save(); }
QColor AppSettings::dsRedColor() const { return m_dsRedColor; }
void AppSettings::setDsRedColor(const QColor &c) { m_dsRedColor = c; save(); }

double AppSettings::dsTotalBalance() const { return m_dsTotalBalance; }
void AppSettings::setDsTotalBalance(double v) { m_dsTotalBalance = v; save(); }

bool AppSettings::dsShowUSD() const { return m_dsShowUSD; }
void AppSettings::setDsShowUSD(bool v) { m_dsShowUSD = v; save(); }

double AppSettings::dsUsdTotalBalance() const { return m_dsUsdTotalBalance; }
void AppSettings::setDsUsdTotalBalance(double v) { m_dsUsdTotalBalance = v; save(); }

double AppSettings::dsUsdGreenThreshold() const { return m_dsUsdGreenThreshold; }
void AppSettings::setDsUsdGreenThreshold(double v) { m_dsUsdGreenThreshold = v; save(); }
double AppSettings::dsUsdYellowThreshold() const { return m_dsUsdYellowThreshold; }
void AppSettings::setDsUsdYellowThreshold(double v) { m_dsUsdYellowThreshold = v; save(); }
QColor AppSettings::dsUsdGreenColor() const { return m_dsUsdGreenColor; }
void AppSettings::setDsUsdGreenColor(const QColor &c) { m_dsUsdGreenColor = c; save(); }
QColor AppSettings::dsUsdYellowColor() const { return m_dsUsdYellowColor; }
void AppSettings::setDsUsdYellowColor(const QColor &c) { m_dsUsdYellowColor = c; save(); }
QColor AppSettings::dsUsdRedColor() const { return m_dsUsdRedColor; }
void AppSettings::setDsUsdRedColor(const QColor &c) { m_dsUsdRedColor = c; save(); }

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
        root["timestamp"] = static_cast<qint64>(QDateTime::currentMSecsSinceEpoch());
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

    for (auto &p : m_platforms) {
        if (p.platformType.isEmpty()) {
            QString lower = p.baseUrl.toLower();
            p.platformType = lower.contains("deepseek") ? "deepseek" : "glm";
        }
    }

    m_autoRefreshInterval = root["autoRefreshInterval"].toInt(20);
    m_ballSize = root["ballSize"].toInt(90);

    QJsonArray posArr = root["ballPos"].toArray();
    if (posArr.size() == 2)
        m_ballPos = QPoint(posArr[0].toInt(100), posArr[1].toInt(100));

    m_pctFontSize = root["pctFontSize"].toInt(0);
    QString pc = root["pctColor"].toString();
    if (!pc.isEmpty())
        m_pctColor = QColor(pc);

    m_timeFontSize = root["timeFontSize"].toInt(0);
    QString tc = root["timeColor"].toString();
    if (!tc.isEmpty())
        m_timeColor = QColor(tc);

    m_autoStart = root["autoStart"].toBool(false);
    m_notifyThreshold = root["notifyThreshold"].toInt(20);
    m_hotkeyToggle = root["hotkeyToggle"].toString("Ctrl+Shift+G");
    m_hotkeyRefresh = root["hotkeyRefresh"].toString("Ctrl+Shift+R");
    m_proxyType = root["proxyType"].toInt(0);
    m_proxyHost = root["proxyHost"].toString();
    m_proxyPort = root["proxyPort"].toInt(0);
    m_themeId = root["themeId"].toInt(0);
    m_language = root["language"].toString();

    m_ballBgOpacity = root["ballBgOpacity"].toInt(80);
    m_ringWidth = root["ringWidth"].toInt(4);
    QString bbgc = root["ballBgColor"].toString();
    if (!bbgc.isEmpty()) m_ballBgColor = QColor(bbgc);
    m_glmGreenThreshold = root["glmGreenThreshold"].toInt(50);
    m_glmYellowThreshold = root["glmYellowThreshold"].toInt(80);
    m_glmGreenColor = QColor(root["glmGreenColor"].toString("#2ECC71"));
    m_glmYellowColor = QColor(root["glmYellowColor"].toString("#F1C40F"));
    m_glmRedColor = QColor(root["glmRedColor"].toString("#E74C3C"));
    m_dsGreenThreshold = root["dsGreenThreshold"].toDouble(10.0);
    m_dsYellowThreshold = root["dsYellowThreshold"].toDouble(1.0);
    m_dsGreenColor = QColor(root["dsGreenColor"].toString("#2ECC71"));
    m_dsYellowColor = QColor(root["dsYellowColor"].toString("#F1C40F"));
    m_dsRedColor = QColor(root["dsRedColor"].toString("#E74C3C"));
    m_dsTotalBalance = root["dsTotalBalance"].toDouble(100.0);
    m_dsShowUSD = root["dsShowUSD"].toBool(true);
    m_dsUsdTotalBalance = root["dsUsdTotalBalance"].toDouble(10.0);
    m_dsUsdGreenThreshold = root["dsUsdGreenThreshold"].toDouble(5.0);
    m_dsUsdYellowThreshold = root["dsUsdYellowThreshold"].toDouble(1.0);
    m_dsUsdGreenColor = QColor(root["dsUsdGreenColor"].toString("#2ECC71"));
    m_dsUsdYellowColor = QColor(root["dsUsdYellowColor"].toString("#F1C40F"));
    m_dsUsdRedColor = QColor(root["dsUsdRedColor"].toString("#E74C3C"));
}

void AppSettings::save()
{
    QJsonObject root;

    QJsonArray arr;
    for (const auto &p : m_platforms)
        arr.append(p.toJson());
    root["platforms"] = arr;

    root["autoRefreshInterval"] = m_autoRefreshInterval;
    root["ballSize"] = m_ballSize;

    QJsonArray posArr;
    posArr.append(m_ballPos.x());
    posArr.append(m_ballPos.y());
    root["ballPos"] = posArr;

    root["pctFontSize"] = m_pctFontSize;
    root["pctColor"] = m_pctColor.isValid() ? m_pctColor.name() : QString();
    root["timeFontSize"] = m_timeFontSize;
    root["timeColor"] = m_timeColor.isValid() ? m_timeColor.name() : QString();
    root["autoStart"] = m_autoStart;
    root["notifyThreshold"] = m_notifyThreshold;
    root["hotkeyToggle"] = m_hotkeyToggle;
    root["hotkeyRefresh"] = m_hotkeyRefresh;
    root["proxyType"] = m_proxyType;
    root["proxyHost"] = m_proxyHost;
    root["proxyPort"] = m_proxyPort;
    root["themeId"] = m_themeId;
    root["language"] = m_language;

    root["ballBgOpacity"] = m_ballBgOpacity;
    root["ringWidth"] = m_ringWidth;
    root["ballBgColor"] = m_ballBgColor.isValid() ? m_ballBgColor.name() : QString();
    root["glmGreenThreshold"] = m_glmGreenThreshold;
    root["glmYellowThreshold"] = m_glmYellowThreshold;
    root["glmGreenColor"] = m_glmGreenColor.isValid() ? m_glmGreenColor.name() : QString();
    root["glmYellowColor"] = m_glmYellowColor.isValid() ? m_glmYellowColor.name() : QString();
    root["glmRedColor"] = m_glmRedColor.isValid() ? m_glmRedColor.name() : QString();
    root["dsGreenThreshold"] = m_dsGreenThreshold;
    root["dsYellowThreshold"] = m_dsYellowThreshold;
    root["dsGreenColor"] = m_dsGreenColor.isValid() ? m_dsGreenColor.name() : QString();
    root["dsYellowColor"] = m_dsYellowColor.isValid() ? m_dsYellowColor.name() : QString();
    root["dsRedColor"] = m_dsRedColor.isValid() ? m_dsRedColor.name() : QString();
    root["dsTotalBalance"] = m_dsTotalBalance;
    root["dsShowUSD"] = m_dsShowUSD;
    root["dsUsdTotalBalance"] = m_dsUsdTotalBalance;
    root["dsUsdGreenThreshold"] = m_dsUsdGreenThreshold;
    root["dsUsdYellowThreshold"] = m_dsUsdYellowThreshold;
    root["dsUsdGreenColor"] = m_dsUsdGreenColor.isValid() ? m_dsUsdGreenColor.name() : QString();
    root["dsUsdYellowColor"] = m_dsUsdYellowColor.isValid() ? m_dsUsdYellowColor.name() : QString();
    root["dsUsdRedColor"] = m_dsUsdRedColor.isValid() ? m_dsUsdRedColor.name() : QString();

    QFile f(m_filePath);
    if (f.open(QIODevice::WriteOnly))
        f.write(QJsonDocument(root).toJson());
}

void AppSettings::applyAutoStart(bool enabled)
{
#ifdef Q_OS_WIN
    QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                  QSettings::NativeFormat);
    if (enabled) {
        QString appPath = QCoreApplication::applicationFilePath();
        appPath.replace("/", "\\");
        reg.setValue("TokenCheckPC", "\"" + appPath + "\"");
    } else {
        reg.remove("TokenCheckPC");
    }
#endif

#ifdef Q_OS_MAC
    QString launchDir = QDir::homePath() + "/Library/LaunchAgents";
    QString plistPath = launchDir + "/com.zaitech.TokenCheckPC.plist";
    if (enabled) {
        QDir().mkpath(launchDir);
        QString execPath = QCoreApplication::applicationFilePath();
        QString plist = QString(
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" "
            "\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
            "<plist version=\"1.0\">\n"
            "<dict>\n"
            "    <key>Label</key>\n"
            "    <string>com.zaitech.TokenCheckPC</string>\n"
            "    <key>ProgramArguments</key>\n"
            "    <array>\n"
            "        <string>%1</string>\n"
            "    </array>\n"
            "    <key>RunAtLoad</key>\n"
            "    <true/>\n"
            "    <key>KeepAlive</key>\n"
            "    <false/>\n"
            "</dict>\n"
            "</plist>\n"
        ).arg(execPath);
        QFile f(plistPath);
        if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
            f.write(plist.toUtf8());
    } else {
        QFile::remove(plistPath);
    }
#endif
}
