#include "platform_registry.h"
#include "platform_handler.h"
#include "platforms/glm/glm_handler.h"
#include "platforms/deepseek/deepseek_handler.h"

PlatformRegistry::PlatformRegistry(QObject *parent) : QObject(parent) {}

PlatformRegistry &PlatformRegistry::instance()
{
    static PlatformRegistry s;
    return s;
}

void PlatformRegistry::init()
{
    auto &r = instance();
    if (!r.m_entries.isEmpty())
        return;

    r.registerPlatform(
        { "glm", "智谱 GLM", "https://open.bigmodel.cn", "/api/monitor/usage",
          { {"Token","token",true}, {"MCP","mcp",true}, {"重置","time",true} } },
        new GlmHandler());

    r.registerPlatform(
        { "deepseek", "DeepSeek", "https://api.deepseek.com", "",
          { {"余额","balance",true}, {"赠金","granted",true} } },
        new DeepSeekHandler());
}

void PlatformRegistry::registerPlatform(const PlatformDescriptor &desc,
                                         PlatformHandler *h)
{
    m_entries[desc.type] = { desc, h };
}

QStringList PlatformRegistry::allTypes() const
{
    return m_entries.keys();
}

QString PlatformRegistry::displayName(const QString &type) const
{
    return m_entries.value(type).descriptor.displayName;
}

QString PlatformRegistry::defaultBaseUrl(const QString &type) const
{
    return m_entries.value(type).descriptor.defaultBaseUrl;
}

QString PlatformRegistry::defaultApiPrefix(const QString &type) const
{
    return m_entries.value(type).descriptor.defaultApiPrefix;
}

const PlatformDescriptor &PlatformRegistry::descriptor(const QString &type) const
{
    static PlatformDescriptor empty;
    auto it = m_entries.constFind(type);
    return it != m_entries.constEnd() ? it->descriptor : empty;
}

PlatformHandler *PlatformRegistry::handler(const QString &type) const
{
    auto it = m_entries.constFind(type);
    return it != m_entries.constEnd() ? it->handler : nullptr;
}
