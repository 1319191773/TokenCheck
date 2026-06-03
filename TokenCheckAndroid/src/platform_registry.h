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

    Q_INVOKABLE QStringList allTypes() const;
    Q_INVOKABLE QString displayName(const QString &type) const;
    Q_INVOKABLE QString defaultBaseUrl(const QString &type) const;
    Q_INVOKABLE QString defaultApiPrefix(const QString &type) const;

    const PlatformDescriptor &descriptor(const QString &type) const;
    PlatformHandler *handler(const QString &type) const;

private:
    explicit PlatformRegistry(QObject *parent = nullptr);
    void registerPlatform(const PlatformDescriptor &desc, PlatformHandler *h);

    struct Entry {
        PlatformDescriptor descriptor;
        PlatformHandler *handler;
    };
    QHash<QString, Entry> m_entries;
};

#endif
