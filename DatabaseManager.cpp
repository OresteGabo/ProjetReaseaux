//
// Created by oreste on 28/10/24.
//

#include "DatabaseManager.h"
DatabaseManager::DatabaseManager(const QString& dbName,
                const QString& user,
                const QString& password,
                const QString& fileName)
{
    initialiseDatabase(dbName, user, password);
    parseData(fileName);
    ConfigManager cf=ConfigManager();
    calculateAndSaveBoundsToConfig(cf);
}

// Step 2a: Parse nodes from the OSM file into the database
bool DatabaseManager::parseNodes(QXmlStreamReader &xml, QSqlQuery &query) {
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
            }else if(xml.name()=="way"){
                qDebug()<<"Way detected";
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
            break; // End of nodes when we reach the end of the OSM element
        }
    }
    return true;
}

// Step 2b: Parse ways and their relationships from the OSM file into the database
bool DatabaseManager::parseWays(QXmlStreamReader &xml, QSqlQuery &query) {

    while (!xml.atEnd()) {
        xml.readNext();
        qDebug() << "Parse ways started";
        if (xml.isStartElement()) {
            if (xml.name() == "way") {
                qDebug()<<"Way detected";
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
bool DatabaseManager::parseData(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file:" << fileName;
        return false;
    }
    QXmlStreamReader xml(&file);
    QSqlQuery query;
    //clean the database for previous data
    truncateAllTables();
    createTables();

    parseNodes(xml, query);

    if (xml.hasError()) {
        qDebug() << "XML Parsing Error:" << xml.errorString();
        return false;
    }else{

    }

    file.close();
    return true;
}

void DatabaseManager::createTables() {
    QSqlQuery query;

    db.transaction();

    // Query for creating nodes table
    query.exec("CREATE TABLE IF NOT EXISTS nodes ("
               "id VARCHAR(255) PRIMARY KEY,"
               "lat DOUBLE NOT NULL,"
               "lon DOUBLE NOT NULL"
               ") ENGINE=InnoDB;");

    // Query for creating ways table
    query.exec("CREATE TABLE IF NOT EXISTS ways ("
               "id VARCHAR(255) PRIMARY KEY,"
               "visible BOOLEAN,"
               "version VARCHAR(255),"
               "timestamp DATETIME,"
               "user VARCHAR(255),"
               "uid VARCHAR(255)"
               ") ENGINE=InnoDB;");

    // Query for creating ways_nodes table
    query.exec("CREATE TABLE IF NOT EXISTS ways_nodes ("
               "way_id VARCHAR(255),"
               "node_id VARCHAR(255),"
               "node_order INT,"
               "PRIMARY KEY (way_id, node_id, node_order),"
               "FOREIGN KEY (way_id) REFERENCES ways(id) ON DELETE CASCADE,"
               "FOREIGN KEY (node_id) REFERENCES nodes(id) ON DELETE CASCADE"
               ") ENGINE=InnoDB;");

    // Query for creating relations table
    query.exec("CREATE TABLE IF NOT EXISTS relations ("
               "id VARCHAR(255) PRIMARY KEY,"
               "visible BOOLEAN,"
               "version VARCHAR(255),"
               "timestamp DATETIME,"
               "user VARCHAR(255),"
               "uid VARCHAR(255)"
               ") ENGINE=InnoDB;");

    // Query for creating relations_members table
    query.exec("CREATE TABLE IF NOT EXISTS relations_members ("
               "relation_id VARCHAR(255),"
               "member_id VARCHAR(255),"
               "member_type ENUM('node', 'way', 'relation'),"
               "role VARCHAR(255),"
               "PRIMARY KEY (relation_id, member_id),"
               "FOREIGN KEY (relation_id) REFERENCES relations(id) ON DELETE CASCADE"
               ") ENGINE=InnoDB;");

    query.exec("CREATE TABLE IF NOT EXISTS tags ("
               "element_id VARCHAR(255),"
               "element_type ENUM('node', 'way', 'relation'),"
               "tag_key VARCHAR(255),"
               "value TEXT,"
               "PRIMARY KEY (element_id, tag_key, element_type)"
               ") ENGINE=InnoDB;");

    db.commit();
}
