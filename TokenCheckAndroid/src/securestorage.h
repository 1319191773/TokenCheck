#ifndef SECURESTORAGE_H
#define SECURESTORAGE_H

#include <QString>

class SecureStorage
{
public:
    static void saveToken(const QString &platformId, const QString &token);
    static QString loadToken(const QString &platformId);
    static void deleteToken(const QString &platformId);
};

#endif
