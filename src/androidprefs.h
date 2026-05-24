#ifndef ANDROIDPREFS_H
#define ANDROIDPREFS_H

#include <QString>

class AndroidPrefs
{
public:
    static void write(const QString &key, const QString &value);
    static void writeInt(const QString &key, int value);
    static QString read(const QString &key, const QString &defaultValue = QString());
    static void notifyWidgetUpdate();
};

#endif
