#ifndef ENCRYPTIONMANAGER_H
#define ENCRYPTIONMANAGER_H

#include <QString>
#include <QByteArray>

class EncryptionManager {
public:
    static QString hashMasterPassword(const QString &masterPassword, const QString &salt = "SafeVaultSalt123");
    static QString encryptPassword(const QString &plainText, const QString &masterKey);
    static QString decryptPassword(const QString &cipherText, const QString &masterKey);
    static QString generateRandomPassword(int length = 16, bool includeSpecial = true);
};

#endif // ENCRYPTIONMANAGER_H