#include "securestorage.h"
#include <QSettings>
#include <QByteArray>
#include <QCryptographicHash>
#include <QSysInfo>

#ifndef Q_OS_IOS

static QByteArray deriveKey()
{
    static const char salt[] = "TokenCheck_SecureStorage_Salt_v1";
    QByteArray uid = QSysInfo::machineUniqueId();
    QByteArray material = uid + QByteArray(salt, sizeof(salt) - 1);
    return QCryptographicHash::hash(material, QCryptographicHash::Sha256);
}

static QByteArray xorBytes(const QByteArray &data, const QByteArray &key)
{
    QByteArray result(data.size(), Qt::Uninitialized);
    for (int i = 0; i < data.size(); ++i)
        result[i] = data[i] ^ key[i % key.size()];
    return result;
}

void SecureStorage::saveToken(const QString &platformId, const QString &token)
{
    if (platformId.isEmpty())
        return;

    QSettings vault("tokencheck", "token_vault");
    if (token.isEmpty()) {
        vault.remove(platformId);
        return;
    }

    QByteArray key = deriveKey();
    QByteArray encrypted = xorBytes(token.toUtf8(), key);
    vault.setValue(platformId, QString::fromLatin1(encrypted.toBase64()));
}

QString SecureStorage::loadToken(const QString &platformId)
{
    if (platformId.isEmpty())
        return QString();

    QSettings vault("tokencheck", "token_vault");
    if (!vault.contains(platformId))
        return QString();

    QByteArray key = deriveKey();
    QByteArray encrypted = QByteArray::fromBase64(vault.value(platformId).toString().toLatin1());
    QByteArray decrypted = xorBytes(encrypted, key);
    return QString::fromUtf8(decrypted);
}

void SecureStorage::deleteToken(const QString &platformId)
{
    QSettings vault("tokencheck", "token_vault");
    vault.remove(platformId);
}

#endif
