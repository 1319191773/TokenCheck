#ifndef GLOBALHOTKEY_H
#define GLOBALHOTKEY_H

#include <QObject>
#include <QHash>

class QHotkey;

class GlobalHotkey : public QObject
{
    Q_OBJECT
public:
    explicit GlobalHotkey(QObject *parent = nullptr);
    ~GlobalHotkey();

    bool registerHotkey(int id, const QString &keySequence);
    void unregisterHotkey(int id);
    void unregisterAll();

signals:
    void hotkeyPressed(int id);

private:
    QHash<int, QHotkey *> m_hotkeys;
};

#endif
