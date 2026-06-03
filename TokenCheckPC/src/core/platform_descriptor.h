#ifndef PLATFORM_DESCRIPTOR_H
#define PLATFORM_DESCRIPTOR_H

#include <QString>
#include <QList>

struct BallSlotDef {
    QString label;
    QString dataKey;
    bool defaultOn = true;
};

struct PlatformDescriptor {
    QString type;
    QString displayName;
    QString defaultBaseUrl;
    QString defaultApiPrefix;
    QList<BallSlotDef> ballSlots;
};

#endif
