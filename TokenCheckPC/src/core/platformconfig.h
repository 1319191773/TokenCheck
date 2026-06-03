#ifndef PLATFORMCONFIG_H
#define PLATFORMCONFIG_H

#include <QString>
#include <QJsonObject>

struct PlatformConfig {
    QString name;
    QString baseUrl;
    QString authToken;
    QString apiPrefix = "/api/monitor/usage";
    QString platformType;
    bool enabled = true;

    QJsonObject toJson() const
    {
        QJsonObject o;
        o["name"] = name;
        o["baseUrl"] = baseUrl;
        o["authToken"] = authToken;
        o["apiPrefix"] = apiPrefix;
        o["platformType"] = platformType;
        o["enabled"] = enabled;
        return o;
    }

    static PlatformConfig fromJson(const QJsonObject &o)
    {
        PlatformConfig pc;
        pc.name = o["name"].toString();
        pc.baseUrl = o["baseUrl"].toString();
        pc.authToken = o["authToken"].toString();
        pc.apiPrefix = o["apiPrefix"].toString("/api/monitor/usage");
        pc.platformType = o["platformType"].toString();
        pc.enabled = o["enabled"].toBool(true);
        return pc;
    }
};

#endif
