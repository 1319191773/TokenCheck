#include "globalhotkey.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include <QCoreApplication>
#include <QDebug>

GlobalHotkey::GlobalHotkey(QObject *parent)
    : QObject(parent)
{
    QCoreApplication::instance()->installNativeEventFilter(this);
}

GlobalHotkey::~GlobalHotkey()
{
    unregisterAll();
    QCoreApplication::instance()->removeNativeEventFilter(this);
}

bool GlobalHotkey::parseKeySequence(const QString &seq, quint32 &mods, quint32 &vk)
{
    if (seq.trimmed().isEmpty())
        return false;

    mods = 0;
    vk = 0;

    QStringList parts = seq.split('+');
    for (auto &p : parts) {
        QString s = p.trimmed().toLower();
        if (s == "ctrl" || s == "control") {
            mods |= MOD_CONTROL;
        } else if (s == "shift") {
            mods |= MOD_SHIFT;
        } else if (s == "alt") {
            mods |= MOD_ALT;
        } else if (s.length() == 1) {
            QChar c = s[0].toUpper();
            vk = static_cast<quint32>(c.unicode());
        } else {
            return false;
        }
    }
    return vk != 0;
}

bool GlobalHotkey::registerHotkey(int id, const QString &keySequence)
{
#ifdef Q_OS_WIN
    unregisterHotkey(id);

    if (keySequence.trimmed().isEmpty())
        return true;

    quint32 mods, vk;
    if (!parseKeySequence(keySequence, mods, vk))
        return false;

    if (!RegisterHotKey(nullptr, id, mods, vk)) {
        qWarning() << "Failed to register hotkey:" << keySequence;
        return false;
    }

    m_registeredIds[id] = vk;
    return true;
#else
    Q_UNUSED(id)
    Q_UNUSED(keySequence)
    return false;
#endif
}

void GlobalHotkey::unregisterHotkey(int id)
{
#ifdef Q_OS_WIN
    if (m_registeredIds.contains(id)) {
        UnregisterHotKey(nullptr, id);
        m_registeredIds.remove(id);
    }
#else
    Q_UNUSED(id)
#endif
}

void GlobalHotkey::unregisterAll()
{
#ifdef Q_OS_WIN
    for (auto it = m_registeredIds.begin(); it != m_registeredIds.end(); ++it)
        UnregisterHotKey(nullptr, it.key());
    m_registeredIds.clear();
#endif
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
bool GlobalHotkey::nativeEventFilter(const QByteArray &eventType, void *message,
                                      qintptr *result)
#else
bool GlobalHotkey::nativeEventFilter(const QByteArray &eventType, void *message,
                                      long *result)
#endif
{
    Q_UNUSED(eventType)
    Q_UNUSED(result)

#ifdef Q_OS_WIN
    MSG *msg = static_cast<MSG *>(message);
    if (msg->message == WM_HOTKEY) {
        int id = static_cast<int>(msg->wParam);
        if (m_registeredIds.contains(id)) {
            emit hotkeyPressed(id);
            return true;
        }
    }
#else
    Q_UNUSED(message)
#endif
    return false;
}
