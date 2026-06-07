#ifndef IOSAPPGROUPS_H
#define IOSAPPGROUPS_H

#include <QObject>
#include "usagequery.h"

class AppSettings;

class IOSAppGroups : public QObject
{
    Q_OBJECT
public:
    explicit IOSAppGroups(QObject *parent = nullptr);

    static void syncWidgetConfig(AppSettings *settings);
    static void reloadTimelines();

public slots:
    void onPlatformFinished(const UsageData &data);
    void onAllFinished();

private:
    static QString sharedContainerPath();
    static void writeJSON(const QString &fileName, const QByteArray &data);

    int m_completedCount = 0;
};

#endif
