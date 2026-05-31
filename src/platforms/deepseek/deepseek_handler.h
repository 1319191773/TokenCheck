#ifndef DEEPSEEK_HANDLER_H
#define DEEPSEEK_HANDLER_H

#include "platform_handler.h"

class DeepSeekHandler : public PlatformHandler
{
public:
    QString authHeaderPrefix() const override { return "Bearer "; }
    QList<PlatformEndpoint> endpoints(const QString &apiPrefix,
                                       const QString &timeQuery) const override;
    bool parse(int endpointIndex, const QJsonObject &root,
               UsageData &data) override;
};

#endif
