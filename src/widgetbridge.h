#ifndef WIDGETBRIDGE_H
#define WIDGETBRIDGE_H

#include <QObject>
#include "usagequery.h"

class WidgetBridge : public QObject
{
    Q_OBJECT
public:
    explicit WidgetBridge(QObject *parent = nullptr);
public slots:
    void onPlatformFinished(const UsageData &data);
    void onAllFinished();
private:
    int m_platformIndex = 0;
};

#endif
