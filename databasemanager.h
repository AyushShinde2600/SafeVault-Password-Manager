#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QVector>
#include "passwordentry.h"

class DatabaseManager {
private:
    QSqlDatabase m_db;

public:
    DatabaseManager();
    ~DatabaseManager();

    bool initDatabase();
    bool setMasterPasswordHash(const QString &hash);
    QString getMasterPasswordHash();

    bool addEntry(const PasswordEntry &entry);
    bool deleteEntry(int id);
    QVector<PasswordEntry> getAllEntries();
};

#endif // DATABASEMANAGER_H