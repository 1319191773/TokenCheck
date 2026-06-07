#include "securestorage.h"

#ifdef Q_OS_IOS

#import <Foundation/Foundation.h>
#import <Security/Security.h>

static const char *kServiceName = "com.tokencheck.tokens";

void SecureStorage::saveToken(const QString &platformId, const QString &token)
{
    if (platformId.isEmpty())
        return;

    NSString *account = platformId.toNSString();
    NSString *service = [NSString stringWithUTF8String:kServiceName];
    NSDictionary *query = @{
        (__bridge id)kSecClass: (__bridge id)kSecClassGenericPassword,
        (__bridge id)kSecAttrService: service,
        (__bridge id)kSecAttrAccount: account
    };

    if (token.isEmpty()) {
        SecItemDelete((__bridge CFDictionaryRef)query);
        return;
    }

    NSData *tokenData = [token.toNSString() dataUsingEncoding:NSUTF8StringEncoding];
    NSDictionary *attrs = @{
        (__bridge id)kSecClass: (__bridge id)kSecClassGenericPassword,
        (__bridge id)kSecAttrService: service,
        (__bridge id)kSecAttrAccount: account,
        (__bridge id)kSecValueData: tokenData,
        (__bridge id)kSecAttrAccessible: (__bridge id)kSecAttrAccessibleAfterFirstUnlockThisDeviceOnly
    };

    OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)query, NULL);
    if (status == errSecSuccess) {
        NSDictionary *update = @{
            (__bridge id)kSecValueData: tokenData,
            (__bridge id)kSecAttrAccessible: (__bridge id)kSecAttrAccessibleAfterFirstUnlockThisDeviceOnly
        };
        SecItemUpdate((__bridge CFDictionaryRef)query, (__bridge CFDictionaryRef)update);
    } else {
        SecItemAdd((__bridge CFDictionaryRef)attrs, NULL);
    }
}

QString SecureStorage::loadToken(const QString &platformId)
{
    if (platformId.isEmpty())
        return QString();

    NSString *account = platformId.toNSString();
    NSString *service = [NSString stringWithUTF8String:kServiceName];
    NSDictionary *query = @{
        (__bridge id)kSecClass: (__bridge id)kSecClassGenericPassword,
        (__bridge id)kSecAttrService: service,
        (__bridge id)kSecAttrAccount: account,
        (__bridge id)kSecReturnData: @YES,
        (__bridge id)kSecMatchLimit: (__bridge id)kSecMatchLimitOne
    };

    CFDataRef result = NULL;
    OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)query, (CFTypeRef *)&result);
    if (status != errSecSuccess || !result)
        return QString();

    NSData *data = (__bridge_transfer NSData *)result;
    return QString::fromNSString([[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding]);
}

void SecureStorage::deleteToken(const QString &platformId)
{
    if (platformId.isEmpty())
        return;

    NSString *account = platformId.toNSString();
    NSString *service = [NSString stringWithUTF8String:kServiceName];
    NSDictionary *query = @{
        (__bridge id)kSecClass: (__bridge id)kSecClassGenericPassword,
        (__bridge id)kSecAttrService: service,
        (__bridge id)kSecAttrAccount: account
    };
    SecItemDelete((__bridge CFDictionaryRef)query);
}

#endif
