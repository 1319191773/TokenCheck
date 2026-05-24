#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QList>
#include "platformconfig.h"

class AppSettings : public QObject
{
    Q_OBJECT

public:
    static AppSettings &instance();

    Q_INVOKABLE int platformCount() const;
    Q_INVOKABLE QString platformName(int index) const;
    Q_INVOKABLE QString platformBaseUrl(int index) const;
    Q_INVOKABLE QString platformAuthToken(int index) const;
    Q_INVOKABLE QString platformApiPrefix(int index) const;
    Q_INVOKABLE bool platformEnabled(int index) const;
    Q_INVOKABLE void setPlatform(int index, const QString &name, const QString &baseUrl,
                                  const QString &token, const QString &apiPrefix, bool enabled);
    Q_INVOKABLE void addPlatform(const QString &name, const QString &baseUrl,
                                  const QString &token, const QString &apiPrefix);
    Q_INVOKABLE void removePlatform(int index);
    PlatformConfig platformAt(int index) const;
    QList<PlatformConfig> allPlatforms() const;

    Q_INVOKABLE int autoRefreshInterval() const;
    Q_INVOKABLE void setAutoRefreshInterval(int minutes);

    Q_INVOKABLE int widgetFontSize() const;
    Q_INVOKABLE void setWidgetFontSize(int size);

    Q_INVOKABLE int widgetShowToken() const;
    Q_INVOKABLE void setWidgetShowToken(int show);
    Q_INVOKABLE int widgetShowMcp() const;
    Q_INVOKABLE void setWidgetShowMcp(int show);
    Q_INVOKABLE int widgetShowTime() const;
    Q_INVOKABLE void setWidgetShowTime(int show);

    Q_INVOKABLE void syncWidgetConfig();

    Q_INVOKABLE bool isConfigured() const;

private:
    explicit AppSettings(QObject *parent = nullptr);
    void load();
    void save();

    QList<PlatformConfig> m_platforms;
    int m_autoRefreshInterval = 5;
    int m_widgetFontSize = 14;
    int m_widgetShowToken = 1;
    int m_widgetShowMcp = 1;
    int m_widgetShowTime = 1;
    QString m_filePath;
};

#endif
