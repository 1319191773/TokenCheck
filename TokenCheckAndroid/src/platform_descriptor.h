#ifndef PLATFORM_DESCRIPTOR_H
#define PLATFORM_DESCRIPTOR_H

#include <QString>
#include <QList>

struct WidgetSlotDef {
    QString label;       // display label: "Token" / "余额"
    QString dataKey;     // SharedPrefs key suffix: "token" / "balance"
    bool defaultOn = true;
};

struct PlatformDescriptor {
    QString type;              // "glm" / "deepseek"
    QString displayName;       // "智谱 GLM" / "DeepSeek"
    QString defaultBaseUrl;
    QString defaultApiPrefix;
    QList<WidgetSlotDef> widgetSlots;
};

#endif
