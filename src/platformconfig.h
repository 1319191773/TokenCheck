#ifndef PLATFORMCONFIG_H
#define PLATFORMCONFIG_H

#include <QObject>
#include <QString>
#include <QJsonObject>

struct PlatformConfig {
    Q_GADGET
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString baseUrl MEMBER baseUrl)
    Q_PROPERTY(QString authToken MEMBER authToken)
    Q_PROPERTY(QString apiPrefix MEMBER apiPrefix)
    Q_PROPERTY(bool enabled MEMBER enabled)
public:
    QString name;
    QString baseUrl;
    QString authToken;
    QString apiPrefix = "/api/monitor/usage";
    bool enabled = true;

    QJsonObject toJson() const {
        QJsonObject o;
        o["name"] = name;
        o["baseUrl"] = baseUrl;
        o["authToken"] = authToken;
        o["apiPrefix"] = apiPrefix;
        o["enabled"] = enabled;
        return o;
    }

    static PlatformConfig fromJson(const QJsonObject &o) {
        PlatformConfig pc;
        pc.name = o["name"].toString();
        pc.baseUrl = o["baseUrl"].toString();
        pc.authToken = o["authToken"].toString();
        pc.apiPrefix = o["apiPrefix"].toString("/api/monitor/usage");
        pc.enabled = o["enabled"].toBool(true);
        return pc;
    }
};

#endif
