#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMap>
#include "ConfigManager.h"
#include <QDebug>

class DatabaseManager {
public:

    DatabaseManager(const QString& dbName,
                    const QString& user,
                    const QString& password,
                    const QString& fileName);
    ~DatabaseManager()
    {

        // close database connection if necessary
    }

    bool parseData(const QString &fileName);

    // Get the minimum and maximum latitude and longitude from a query
    void getBounds(QSqlQuery &query, double &minLat, double &maxLat, double &minLon, double &maxLon) {
        while (query.next()) {
            double lat = query.value(0).toDouble();
            double lon = query.value(1).toDouble();

            if (lat < minLat) minLat = lat;
            if (lat > maxLat) maxLat = lat;
            if (lon < minLon) minLon = lon;
            if (lon > maxLon) maxLon = lon;
        }
    }


    bool calculateAndSaveBoundsToConfig(ConfigManager &configManager) {
        QSqlQuery query;
        auto minLat = 90.0, maxLat = -90.0, minLon = 180.0, maxLon = -180.0;

        // Calculate bounds by querying node coordinates
        query.exec("SELECT lat, lon FROM nodes");
        getBounds(query, minLat, maxLat, minLon, maxLon);

        // Debug statement to verify bounds
        qDebug() << "Bounds calculated - MinLat:" << minLat << ", MaxLat:" << maxLat
                 << ", MinLon:" << minLon << ", MaxLon:" << maxLon;

        // Set the bounds in ConfigManager and ensure they are saved
        configManager.setBounds(minLat, maxLat, minLon, maxLon);

        // Check if configManager successfully saved bounds
        qDebug() << "Bounds have been set in ConfigManager";

        return true;
    }

private:

    void truncateAllTables() {
        QSqlQuery query;
        QStringList tables = {"relations_members","ways_nodes", "tags","relations","ways","address","nodes"};

        db.transaction();
        for (const auto &tableName : tables) {
            if(query.exec("DROP TABLE " + tableName)){
                qDebug()<<"Table dropped "<<tableName;
            }
        }
        db.commit();
    }
    void createTables() ;



    QSqlDatabase db;

    void initialiseDatabase(const QString &dbName,
                            const QString &user,
                            const QString &password)
    {
        db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName("127.0.0.1");
        db.setDatabaseName(dbName);
        db.setUserName(user);
        db.setPassword(password);
        if (!db.open()) {
            qDebug() << "#####################Database connection failed:" << db.lastError().text();
        }else{
            qDebug() << "#############################Database connection Success\n" ;
        }
    }

    static bool parseNodes(QXmlStreamReader &xml, QSqlQuery &query);
    static bool parseWays(QXmlStreamReader &xml, QSqlQuery &query);
    void markJunctionNodes();
    QMap<QString, QString> getWayTags(const QString &wayId);

};