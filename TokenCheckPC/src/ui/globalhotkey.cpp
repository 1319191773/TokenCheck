#include "globalhotkey.h"
#include <QHotkey>

GlobalHotkey::GlobalHotkey(QObject *parent) : QObject(parent) {}

GlobalHotkey::~GlobalHotkey() { unregisterAll(); }

bool GlobalHotkey::registerHotkey(int id, const QString &keySequence)
{
    unregisterHotkey(id);

    if (keySequence.trimmed().isEmpty())
        return true;

    auto *hk = new QHotkey(QKeySequence(keySequence), true, this);
    if (!hk->isRegistered()) {
        qWarning() << "Failed to register hotkey:" << keySequence;
        delete hk;
        return false;
    }
    connect(hk, &QHotkey::activated, this, [this, id]() {
        emit hotkeyPressed(id);
    });
    m_hotkeys[id] = hk;
    return true;
}

void GlobalHotkey::unregisterHotkey(int id)
{
    auto it = m_hotkeys.find(id);
    if (it != m_hotkeys.end()) {
        delete it.value();
        m_hotkeys.erase(it);
    }
}

void GlobalHotkey::unregisterAll()
{
    qDeleteAll(m_hotkeys);
    m_hotkeys.clear();
}
