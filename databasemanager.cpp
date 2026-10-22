#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager() {}

DatabaseManager::~DatabaseManager() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::initDatabase() {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("safevault.db");

    if (!m_db.open()) {
        qDebug() << "Database Error:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS vault_settings ("
               "key TEXT PRIMARY KEY, "
               "value TEXT)");

    query.exec("CREATE TABLE IF NOT EXISTS entries ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "service TEXT, "
               "username TEXT, "
               "encrypted_password TEXT, "
               "category TEXT)");

    return true;
}

bool DatabaseManager::setMasterPasswordHash(const QString &hash) {
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO vault_settings (key, value) VALUES ('master_hash', :hash)");
    query.bindValue(":hash", hash);
    return query.exec();
}

QString DatabaseManager::getMasterPasswordHash() {
    QSqlQuery query("SELECT value FROM vault_settings WHERE key = 'master_hash'");
    if (query.next()) {
        return query.value(0).toString();
    }
    return "";
}

bool DatabaseManager::addEntry(const PasswordEntry &entry) {
    QSqlQuery query;
    query.prepare("INSERT INTO entries (service, username, encrypted_password, category) "
                  "VALUES (:service, :username, :enc_pass, :category)");
    query.bindValue(":service", entry.serviceName());
    query.bindValue(":username", entry.username());
    query.bindValue(":enc_pass", entry.encryptedPassword());
    query.bindValue(":category", entry.category());
    return query.exec();
}

bool DatabaseManager::deleteEntry(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM entries WHERE id = :id");
    query.bindValue(":id", id);
    return query.exec();
}

QVector<PasswordEntry> DatabaseManager::getAllEntries() {
    QVector<PasswordEntry> list;
    QSqlQuery query("SELECT id, service, username, encrypted_password, category FROM entries ORDER BY id DESC");

    while (query.next()) {
        int id = query.value(0).toInt();
        QString service = query.value(1).toString();
        QString user = query.value(2).toString();
        QString encPass = query.value(3).toString();
        QString cat = query.value(4).toString();

        list.append(PasswordEntry(id, service, user, encPass, cat));
    }
    return list;
}