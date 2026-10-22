#include "passwordentry.h"

PasswordEntry::PasswordEntry()
    : m_id(-1), m_serviceName(""), m_username(""), m_encryptedPassword(""), m_category("General") {}

PasswordEntry::PasswordEntry(int id, const QString &service, const QString &user,
                             const QString &encryptedPass, const QString &category)
    : m_id(id), m_serviceName(service), m_username(user),
    m_encryptedPassword(encryptedPass), m_category(category) {}

int PasswordEntry::id() const { return m_id; }
void PasswordEntry::setId(int id) { m_id = id; }

QString PasswordEntry::serviceName() const { return m_serviceName; }
void PasswordEntry::setServiceName(const QString &service) { m_serviceName = service; }

QString PasswordEntry::username() const { return m_username; }
void PasswordEntry::setUsername(const QString &username) { m_username = username; }

QString PasswordEntry::encryptedPassword() const { return m_encryptedPassword; }
void PasswordEntry::setEncryptedPassword(const QString &encryptedPass) { m_encryptedPassword = encryptedPass; }

QString PasswordEntry::category() const { return m_category; }
void PasswordEntry::setCategory(const QString &category) { m_category = category; }