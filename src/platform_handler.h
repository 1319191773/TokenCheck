#ifndef PLATFORM_HANDLER_H
#define PLATFORM_HANDLER_H

#include <QString>
#include <QList>
#include <QJsonObject>

struct UsageData;

struct PlatformEndpoint {
    QString path;
    QString query;
};

class PlatformHandler {
public:
    virtual ~PlatformHandler() = default;
    virtual QString authHeaderPrefix() const { return QString(); }
    virtual QList<PlatformEndpoint> endpoints(const QString &apiPrefix,
                                              const QString &timeQuery) const = 0;
    virtual bool parse(int endpointIndex, const QJsonObject &root,
                       UsageData &data) = 0;
};

#endif
