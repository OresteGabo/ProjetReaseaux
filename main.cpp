#include <QApplication>
#include <QGraphicsView>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include<QXmlStreamReader>
#include <QScreen>
#include "CustomScene.h"
#include "ConfigManager.h"
#include "CustomGraphicsView.h"
#include <iostream>
using namespace std;
// Step 1: Initialize database connection
bool initialiseDatabase(const QString &dbName, const QString &user, const QString &password) {

    //qDebug() << "#####################INITIALIZE DEBUT\n" ;
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");

    //qDebug() << "#####################INITIALIZE FIN DEBUT HOST NAME\n" ;
    db.setHostName("127.0.0.1");
    db.setDatabaseName(dbName);
    db.setUserName(user);
    db.setPassword(password);

    if (!db.open()) {
        qDebug() << "#####################Database connection failed:" << db.lastError().text();
        return false;
    }else{
        qDebug() << "#############################Database connection Success\n" ;
    }
    return true;
}

// Step 2a: Parse nodes from the OSM file into the database
bool parseNodes(QXmlStreamReader &xml, QSqlQuery &query) {
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            if (xml.name() == "node") {
                QString id = xml.attributes().value("id").toString();
                double lat = xml.attributes().value("lat").toDouble();
                double lon = xml.attributes().value("lon").toDouble();

                query.prepare("INSERT INTO nodes (id, lat, lon) VALUES (:id, :lat, :lon)");
                query.bindValue(":id", id);
                query.bindValue(":lat", lat);
                query.bindValue(":lon", lon);
                if (!query.exec()) {
                    qDebug() << "Failed to insert node:" << query.lastError().text();
                }
            }
        } else if (xml.isEndElement() && xml.name() == "osm") {
            break; // End of nodes when we reach the end of the OSM element
        }
    }
    return true;
}

// Step 2b: Parse ways and their relationships from the OSM file into the database
bool parseWays(QXmlStreamReader &xml, QSqlQuery &query) {
    qDebug() << "Parse ways started";
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            if (xml.name() == "way") {
                QString wayId = xml.attributes().value("id").toString();

                // Retrieve attributes from the way element
                bool visible = xml.attributes().value("visible").toString() == "true"; // or "false"
                QString version = xml.attributes().value("version").toString();
                QString timestamp = xml.attributes().value("timestamp").toString();
                QString user = xml.attributes().value("user").toString();
                QString uid = xml.attributes().value("uid").toString();

                QVector<QString> nodeRefs;
                QMap<QString, QString> tags;

                while (!(xml.isEndElement() && xml.name() == "way")) {
                    xml.readNext();
                    if (xml.isStartElement() && xml.name() == "nd") {
                        nodeRefs.append(xml.attributes().value("ref").toString());
                    } else if (xml.isStartElement() && xml.name() == "tag") {
                        tags.insert(xml.attributes().value("k").toString(), xml.attributes().value("v").toString());
                    }
                }

                // Insert way and its tags
                query.prepare("INSERT INTO ways (id, visible, version, timestamp, user, uid) VALUES (:id, :visible, :version, :timestamp, :user, :uid)");
                query.bindValue(":id", wayId);
                query.bindValue(":visible", visible);
                query.bindValue(":version", version);
                query.bindValue(":timestamp", QDateTime::fromString(timestamp, Qt::ISODate)); // Adjust format as needed
                query.bindValue(":user", user);
                query.bindValue(":uid", uid);
                if (!query.exec()) {
                    qDebug() << "Failed to insert way:" << query.lastError().text();
                }

                for (const auto &tagKey : tags.keys()) {
                    query.prepare("INSERT INTO tags (element_id, element_type, tag_key, value) VALUES (:element_id, 'way', :tag_key, :value)");
                    query.bindValue(":element_id", wayId);
                    query.bindValue(":tag_key", tagKey);
                    query.bindValue(":value", tags[tagKey]);
                    if (!query.exec()) {
                        qDebug() << "Failed to insert way tag:" << query.lastError().text();
                    }
                }

                // Insert way-node relationships
                int order = 0;
                for (const QString &ref : nodeRefs) {
                    query.prepare("INSERT INTO ways_nodes (way_id, node_id, node_order) VALUES (:wayId, :nodeId, :order)");
                    query.bindValue(":wayId", wayId);
                    query.bindValue(":nodeId", ref);
                    query.bindValue(":order", order++);
                    if (!query.exec()) {
                        qDebug() << "Failed to insert way-node relationship:" << query.lastError().text();
                    }
                }
            }
        } else if (xml.isEndElement() && xml.name() == "osm") {
            break; // End of ways when we reach the end of the OSM element
        }
    }
    return true;
}



// Step 2: Parse OSM file data into the database
bool parseData(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file:" << fileName;
        return false;
    }

    QXmlStreamReader xml(&file);
    QSqlQuery query;

    // First, parse nodes
    parseNodes(xml, query);

    // Then parse ways
    parseWays(xml, query);

    if (xml.hasError()) {
        qDebug() << "XML Parsing Error:" << xml.errorString();
        return false;
    }

    file.close();
    return true;
}


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

// Get the Json object from a file
QJsonObject getJsonObjFromFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadWrite)) {
        qDebug() << "Failed to open file:" << fileName;
        return QJsonObject();
    }

    QByteArray fileData = file.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
    return jsonDoc.object();
}

// Write the JSON object to a file
bool writeJsonObjToFile(const QString &fileName, QJsonObject jsonObj) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadWrite)) {
        qDebug() << "Failed to open file:" << fileName;
        return false;
    }

    QJsonDocument jsonDoc(jsonObj);
    file.resize(0); // Clear the file
    file.write(jsonDoc.toJson());
    file.close();
    return true;
}


bool calculateAndSaveBoundsToConfig(ConfigManager &configManager) {
    QSqlQuery query;
    auto minLat = 90.0, maxLat = -90.0, minLon = 180.0, maxLon = -180.0;

    // Calculate bounds by querying node coordinates
    query.exec("SELECT lat, lon FROM nodes");
    getBounds(query, minLat, maxLat, minLon, maxLon);

    // Set the bounds in the ConfigManager
    configManager.setBounds(minLat, maxLat, minLon, maxLon);

    return true;
}

bool saveScreenDimensionsToJSON(const QRect& screenGeometry,const QString &configFileName = "config.json") {

    // Get the JSON object from the config file
    QJsonObject jsonObj = getJsonObjFromFile(configFileName);
    if (jsonObj.isEmpty()) return false;

    // Create a new object for the MainWindow dimensions
    QJsonObject mainWindowObj;
    mainWindowObj["width"] = screenGeometry.width();
    mainWindowObj["height"] = screenGeometry.height();

    // Add the MainWindow object to the JSON object
    jsonObj["MainWindow"] = mainWindowObj;
    cout<<screenGeometry.width()<< " "<<screenGeometry.width()<<"endl";
    // Write the updated JSON object to the config file
    return writeJsonObjToFile(configFileName, jsonObj);
}


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    //cerr<<"Error"<<endl;

    // Initialize database
    if (!initialiseDatabase("OSMData", "oreste", "Muhirehonore@1*")) {
        return -1;
    }


    /*
    // Parse data into the database
    if (!parseData("map.osm")) {
        return -1;
    }
    */

    // Get screen dimensions
    auto screen = QGuiApplication::primaryScreen();
    auto screenGeometry = screen->geometry();

    ConfigManager configManager("config.json");  // Create a ConfigManager for config.json
    configManager.setMainWindowSize(screenGeometry);  // Set main window size to screen size

    // Get main window size
    auto mainWindowSize = configManager.getMainWindowSize();
    auto width = mainWindowSize.width();
    auto height = mainWindowSize.height();

    // Calculate and save bounds to config
    calculateAndSaveBoundsToConfig(configManager);

    // Create the scene
    auto scene = new CustomScene(width, height);

    // Create view using CustomGraphicsView
    auto view = new CustomGraphicsView();


    view->setScene(scene);
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio); // Fit scene into view
    view->show();

    return app.exec();



}
