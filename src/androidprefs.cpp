#include "androidprefs.h"
#include <QJniObject>

static QJniObject getCtx()
{
    return QJniObject::callStaticObjectMethod(
        "org/qtproject/qt/android/QtNative", "activity", "()Landroid/app/Activity;");
}

void AndroidPrefs::write(const QString &key, const QString &value)
{
    QJniObject ctx = getCtx();
    if (!ctx.isValid()) return;

    QJniObject prefs = ctx.callObjectMethod(
        "getSharedPreferences", "(Ljava/lang/String;I)Landroid/content/SharedPreferences;",
        QJniObject::fromString("glm_usage_prefs").object<jstring>(), jint(0));

    if (prefs.isValid()) {
        QJniObject editor = prefs.callObjectMethod("edit",
            "()Landroid/content/SharedPreferences$Editor;");
        if (editor.isValid()) {
            editor.callObjectMethod("putString",
                "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/SharedPreferences$Editor;",
                QJniObject::fromString(key).object<jstring>(),
                QJniObject::fromString(value).object<jstring>());
            editor.callMethod<void>("apply");
        }
    }
}

void AndroidPrefs::writeInt(const QString &key, int value)
{
    write(key, QString::number(value));
}

QString AndroidPrefs::read(const QString &key, const QString &defaultValue)
{
    QJniObject ctx = getCtx();
    if (!ctx.isValid()) return defaultValue;

    QJniObject prefs = ctx.callObjectMethod(
        "getSharedPreferences", "(Ljava/lang/String;I)Landroid/content/SharedPreferences;",
        QJniObject::fromString("glm_usage_prefs").object<jstring>(), jint(0));

    if (prefs.isValid()) {
        QJniObject str = prefs.callObjectMethod("getString",
            "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
            QJniObject::fromString(key).object<jstring>(),
            QJniObject::fromString(defaultValue).object<jstring>());
        if (str.isValid())
            return str.toString();
    }
    return defaultValue;
}

void AndroidPrefs::notifyWidgetUpdate()
{
    QJniObject ctx = getCtx();
    if (!ctx.isValid()) return;

    QJniObject intent("android/content/Intent", "(Ljava/lang/String;)V",
        QJniObject::fromString("com.glm.usage.WIDGET_UPDATE").object<jstring>());
    intent.callObjectMethod("setPackage", "(Ljava/lang/String;)Landroid/content/Intent;",
        QJniObject::fromString("com.glm.usage").object<jstring>());
    ctx.callMethod<void>("sendBroadcast", "(Landroid/content/Intent;)V", intent.object());
}
