#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QPoint>
#include <QList>
#include "platformconfig.h"

enum class ThemeId;

class AppSettings : public QObject
{
    Q_OBJECT

public:
    static AppSettings &instance();

    int platformCount() const;
    PlatformConfig platformAt(int index) const;
    QList<PlatformConfig> allPlatforms() const;
    void setPlatform(int index, const PlatformConfig &config);
    void addPlatform(const PlatformConfig &config);
    void removePlatform(int index);

    int autoRefreshInterval() const;
    void setAutoRefreshInterval(int minutes);

    int ballSize() const;
    void setBallSize(int size);

    QPoint ballPosition() const;
    void setBallPosition(const QPoint &pos);

    int pctFontSize() const;
    void setPctFontSize(int size);

    QColor pctColor() const;
    void setPctColor(const QColor &color);

    int timeFontSize() const;
    void setTimeFontSize(int size);

    QColor timeColor() const;
    void setTimeColor(const QColor &color);

    bool autoStart() const;
    void setAutoStart(bool enabled);

    int notifyThreshold() const;
    void setNotifyThreshold(int threshold);

    QString hotkeyToggle() const;
    void setHotkeyToggle(const QString &key);

    QString hotkeyRefresh() const;
    void setHotkeyRefresh(const QString &key);

    int proxyType() const;
    void setProxyType(int type);

    QString proxyHost() const;
    void setProxyHost(const QString &host);

    int proxyPort() const;
    void setProxyPort(int port);

    ThemeId themeId() const;
    void setThemeId(ThemeId id);

    QString language() const;
    void setLanguage(const QString &lang);

    bool isConfigured() const;

    void cacheUsageData(const QString &json);
    QString loadCachedUsageData() const;

signals:
    void platformsChanged();

private:
    explicit AppSettings(QObject *parent = nullptr);
    void load();
    void save();
    void applyAutoStart(bool enabled);

    QList<PlatformConfig> m_platforms;
    int m_autoRefreshInterval = 20;
    int m_ballSize = 90;
    QPoint m_ballPos = QPoint(100, 100);
    int m_pctFontSize = 0;
    QColor m_pctColor;
    int m_timeFontSize = 0;
    QColor m_timeColor;
    bool m_autoStart = false;
    int m_notifyThreshold = 20;
    QString m_hotkeyToggle = "Ctrl+Shift+G";
    QString m_hotkeyRefresh = "Ctrl+Shift+R";
    int m_proxyType = 0;
    QString m_proxyHost;
    int m_proxyPort = 0;
    int m_themeId = 0;
    QString m_language;
    QString m_filePath;
};

#endif
