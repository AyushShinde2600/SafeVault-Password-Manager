#ifndef PASSWORDENTRY_H
#define PASSWORDENTRY_H

#include <QString>

class PasswordEntry {
private:
    int m_id;
    QString m_serviceName;
    QString m_username;
    QString m_encryptedPassword;
    QString m_category;

public:
    PasswordEntry();
    PasswordEntry(int id, const QString &service, const QString &user,
                  const QString &encryptedPass, const QString &category);

    int id() const;
    void setId(int id);

    QString serviceName() const;
    void setServiceName(const QString &service);

    QString username() const;
    void setUsername(const QString &username);

    QString encryptedPassword() const;
    void setEncryptedPassword(const QString &encryptedPass);

    QString category() const;
    void setCategory(const QString &category);
};

#endif // PASSWORDENTRY_H