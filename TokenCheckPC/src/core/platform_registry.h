#ifndef PLATFORM_REGISTRY_H
#define PLATFORM_REGISTRY_H

#include <QObject>
#include <QHash>
#include "platform_descriptor.h"

class PlatformHandler;

class PlatformRegistry : public QObject
{
    Q_OBJECT
public:
    static PlatformRegistry &instance();
    static void init();
    ~PlatformRegistry();

    QStringList allTypes() const;
    QString displayName(const QString &type) const;
    QString defaultBaseUrl(const QString &type) const;
    QString defaultApiPrefix(const QString &type) const;

    const PlatformDescriptor &descriptor(const QString &type) const;
    PlatformHandler *handler(const QString &type) const;

private:
    explicit PlatformRegistry(QObject *parent = nullptr);
    void registerPlatform(const PlatformDescriptor &desc, PlatformHandler *h);

    struct Entry {
        PlatformDescriptor descriptor;
        PlatformHandler *handler = nullptr;
    };
    QHash<QString, Entry> m_entries;
    QList<PlatformHandler *> m_ownedHandlers;
};

#endif
