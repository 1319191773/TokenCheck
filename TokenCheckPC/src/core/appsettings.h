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

    int ballBgOpacity() const;
    void setBallBgOpacity(int opacity);

    int ringWidth() const;
    void setRingWidth(int w);

    QColor ballBgColor() const;
    void setBallBgColor(const QColor &c);

    int glmGreenThreshold() const;
    void setGlmGreenThreshold(int v);
    int glmYellowThreshold() const;
    void setGlmYellowThreshold(int v);
    QColor glmGreenColor() const;
    void setGlmGreenColor(const QColor &c);
    QColor glmYellowColor() const;
    void setGlmYellowColor(const QColor &c);
    QColor glmRedColor() const;
    void setGlmRedColor(const QColor &c);

    double dsGreenThreshold() const;
    void setDsGreenThreshold(double v);
    double dsYellowThreshold() const;
    void setDsYellowThreshold(double v);
    QColor dsGreenColor() const;
    void setDsGreenColor(const QColor &c);
    QColor dsYellowColor() const;
    void setDsYellowColor(const QColor &c);
    QColor dsRedColor() const;
    void setDsRedColor(const QColor &c);

    double dsTotalBalance() const;
    void setDsTotalBalance(double v);

    double dsUsdTotalBalance() const;
    void setDsUsdTotalBalance(double v);
    bool dsShowUSD() const;
    void setDsShowUSD(bool v);

    double dsUsdGreenThreshold() const;
    void setDsUsdGreenThreshold(double v);
    double dsUsdYellowThreshold() const;
    void setDsUsdYellowThreshold(double v);
    QColor dsUsdGreenColor() const;
    void setDsUsdGreenColor(const QColor &c);
    QColor dsUsdYellowColor() const;
    void setDsUsdYellowColor(const QColor &c);
    QColor dsUsdRedColor() const;
    void setDsUsdRedColor(const QColor &c);

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

    int m_ballBgOpacity = 80;
    int m_ringWidth = 4;
    QColor m_ballBgColor;
    int m_glmGreenThreshold = 50;
    int m_glmYellowThreshold = 80;
    QColor m_glmGreenColor = QColor("#2ECC71");
    QColor m_glmYellowColor = QColor("#F1C40F");
    QColor m_glmRedColor = QColor("#E74C3C");
    double m_dsGreenThreshold = 10.0;
    double m_dsYellowThreshold = 1.0;
    QColor m_dsGreenColor = QColor("#2ECC71");
    QColor m_dsYellowColor = QColor("#F1C40F");
    QColor m_dsRedColor = QColor("#E74C3C");
    double m_dsTotalBalance = 100.0;
    bool m_dsShowUSD = true;
    double m_dsUsdTotalBalance = 10.0;
    double m_dsUsdGreenThreshold = 5.0;
    double m_dsUsdYellowThreshold = 1.0;
    QColor m_dsUsdGreenColor = QColor("#2ECC71");
    QColor m_dsUsdYellowColor = QColor("#F1C40F");
    QColor m_dsUsdRedColor = QColor("#E74C3C");

    QString m_filePath;
};

#endif
