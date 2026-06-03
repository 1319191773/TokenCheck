#ifndef GLOBALHOTKEY_H
#define GLOBALHOTKEY_H

#include <QObject>
#include <QAbstractNativeEventFilter>
#include <QHash>

class GlobalHotkey : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit GlobalHotkey(QObject *parent = nullptr);
    ~GlobalHotkey();

    bool registerHotkey(int id, const QString &keySequence);
    void unregisterHotkey(int id);
    void unregisterAll();

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool nativeEventFilter(const QByteArray &eventType, void *message,
                           qintptr *result) override;
#else
    bool nativeEventFilter(const QByteArray &eventType, void *message,
                           long *result) override;
#endif

signals:
    void hotkeyPressed(int id);

private:
    QHash<int, quint32> m_registeredIds;
    int m_nextAtomBase = 0xC000;

    static bool parseKeySequence(const QString &seq, quint32 &mods, quint32 &vk);
    static QString keyToString(quint32 vk);
};

#endif
