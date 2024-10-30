//
// Created by oreste on 28/10/24.
//

#ifndef GRAPHICSINQT_DATABASEMANAGER_H
#define GRAPHICSINQT_DATABASEMANAGER_H


#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QString>

class DatabaseManager {
public:
    DatabaseManager(const QString &dbName) {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(dbName);

        if (!db.open()) {
            qDebug() << "Error: Unable to open database:" << db.lastError().text();
        } else {
            qDebug() << "Database opened successfully.";
            createTables();
        }
    }

    ~DatabaseManager() {
        db.close();
    }

private:
    QSqlDatabase db;

    void createTables() {
        QSqlQuery query;
        // Create nodes table
        query.exec("CREATE TABLE IF NOT EXISTS nodes ("
                   "id TEXT PRIMARY KEY,"
                   "lat REAL,"
                   "lon REAL);");

        // Create ways table
        query.exec("CREATE TABLE IF NOT EXISTS ways ("
                   "id TEXT PRIMARY KEY,"
                   "type TEXT,"
                   "nodes TEXT);");

        // Additional tables can be created as needed
    }
};



#endif //GRAPHICSINQT_DATABASEMANAGER_H
