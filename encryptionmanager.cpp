#include "encryptionmanager.h"
#include <QCryptographicHash>
#include <QRandomGenerator>

QString EncryptionManager::hashMasterPassword(const QString &masterPassword, const QString &salt) {
    QByteArray combined = (masterPassword + salt).toUtf8();
    QByteArray hash = QCryptographicHash::hash(combined, QCryptographicHash::Sha256);
    hash = QCryptographicHash::hash(hash + salt.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

QString EncryptionManager::encryptPassword(const QString &plainText, const QString &masterKey) {
    QByteArray keyHash = QCryptographicHash::hash(masterKey.toUtf8(), QCryptographicHash::Sha256);
    QByteArray data = plainText.toUtf8();
    QByteArray encrypted;

    for (int i = 0; i < data.size(); ++i) {
        encrypted.append(data.at(i) ^ keyHash.at(i % keyHash.size()));
    }
    return QString(encrypted.toBase64());
}

QString EncryptionManager::decryptPassword(const QString &cipherText, const QString &masterKey) {
    QByteArray keyHash = QCryptographicHash::hash(masterKey.toUtf8(), QCryptographicHash::Sha256);
    QByteArray data = QByteArray::fromBase64(cipherText.toUtf8());
    QByteArray decrypted;

    for (int i = 0; i < data.size(); ++i) {
        decrypted.append(data.at(i) ^ keyHash.at(i % keyHash.size()));
    }
    return QString::fromUtf8(decrypted);
}

QString EncryptionManager::generateRandomPassword(int length, bool includeSpecial) {
    const QString charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const QString special = "!@#$%^&*()_+-=[]{}|;:,.<>?";
    QString validChars = charset + (includeSpecial ? special : "");

    QString password = "";
    for (int i = 0; i < length; ++i) {
        quint32 index = QRandomGenerator::global()->bounded(validChars.length());
        password.append(validChars.at(index));
    }
    return password;
}