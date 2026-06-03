#ifndef GLM_HANDLER_H
#define GLM_HANDLER_H

#include "platform_handler.h"

class GlmHandler : public PlatformHandler
{
public:
    QList<PlatformEndpoint> endpoints(const QString &apiPrefix,
                                       const QString &timeQuery) const override;
    bool parse(int endpointIndex, const QJsonObject &root,
               UsageData &data) override;

private:
    static bool parseModelUsage(const QJsonObject &root, UsageData &data);
    static bool parseToolUsage(const QJsonObject &root, UsageData &data);
    static bool parseQuotaLimit(const QJsonObject &root, UsageData &data);
    static QString parseUsageDetails(const QJsonValue &val);
};

#endif
