//
// Created by oreste on 28/10/24.
//

#include "DatabaseManager.h"
DatabaseManager::DatabaseManager(
        const QString& dbName,
        const QString& user,
        const QString& password,
        const QString& fileName)
{
    initialiseDatabase(dbName, user, password);
    QSqlQuery query;

    if(!query.prepare("SELECT COUNT(*) FROM ways")) {
        parseData(fileName);
        ConfigManager cf = ConfigManager();
        calculateAndSaveBoundsToConfig(cf);
    }
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

                // Flags to check for address tags
                bool hasCity = false;
                bool hasPostcode = false;
                bool hasHousenumber = false;
                bool hasStreet = false;

                // Variables to store address data
                QString city, postcode, housenumber, street;

                // Read node tags
                while (!(xml.isEndElement() && xml.name() == "node")) {
                    xml.readNext();
                    if (xml.isStartElement() && xml.name() == "tag") {
                        QString key = xml.attributes().value("k").toString();
                        QString value = xml.attributes().value("v").toString();

                        // Check for address-related tags
                        if (key == "addr:city") {
                            city=value;
                            hasCity = true;
                        } else if (key == "addr:postcode") {
                            postcode=value;
                            hasPostcode = true;
                        } else if (key == "addr:housenumber") {
                            housenumber=value;
                            hasHousenumber = true;
                        } else if (key == "addr:street") {
                            street=value;
                            hasStreet = true;
                        }

                        // Insert the tag into the tags table
                        query.prepare("INSERT INTO tags (element_id, element_type, tag_key, value) "
                                      "VALUES (:id, 'node', :key, :value)");
                        query.bindValue(":id", id);
                        query.bindValue(":key", key);
                        query.bindValue(":value", value);
                        if (!query.exec()) {
                            qDebug() << "Failed to insert tag:" << query.lastError().text();
                        }
                    }
                }

                // Determine if the node is important
                bool isImportant = hasCity && hasPostcode && hasHousenumber && hasStreet;

                // Insert the node into the nodes table
                query.prepare("INSERT INTO nodes (id, lat, lon, isImportant) "
                              "VALUES (:id, :lat, :lon, :isImportant)");
                query.bindValue(":id", id);
                query.bindValue(":lat", lat);
                query.bindValue(":lon", lon);
                query.bindValue(":isImportant", isImportant ? 1 : 0);
                if (!query.exec()) {
                    qDebug() << "Failed to insert node:" << query.lastError().text();
                }

                // If the node has address information, insert it into the address table
                if (isImportant) {
                    query.prepare("INSERT INTO address (node_id, housenumber, street, postcode, city) "
                                  "VALUES (:node_id, :housenumber, :street, :postcode, :city)");

                    query.bindValue(":node_id", id);
                    query.bindValue(":housenumber", housenumber);
                    query.bindValue(":street", street);
                    query.bindValue(":postcode", postcode);
                    query.bindValue(":city", city);

                    if (!query.exec()) {
                        qDebug() << "Failed to insert address:" << query.lastError().text();
                    }
                }
            }
        } else if (xml.isEndElement() && xml.name() == "osm") {
            break; // End of parsing when reaching the end of the OSM element
        }
    }
    return true;
}

// Step 2b: Parse ways and their relationships from the OSM file into the database
bool DatabaseManager::parseWays(QXmlStreamReader &xml, QSqlQuery &query) {

    while (!xml.atEnd()) {
        xml.readNext();
        //qDebug() << "Parse ways started";
        if (xml.isStartElement()) {
            if (xml.name() == "way") {
                //qDebug()<<"Way detected";
                QString wayId = xml.attributes().value("id").toString();

                // Retrieve attributes from the way element
                bool visible = xml.attributes().value("visible").toString() == "true"; // or "false"
                auto uid = xml.attributes().value("uid").toString();

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
                query.prepare("INSERT INTO ways (id, visible, uid) VALUES (:id, :visible, :uid)");
                query.bindValue(":id", wayId);
                query.bindValue(":visible", visible);
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
                for (const auto &ref : nodeRefs) {
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

    // Critical section: Database operations
    truncateAllTables();
    createTables();

    // Parse nodes
    parseNodes(xml, query);

    // Reset the XML reader before parsing ways
    file.seek(0);
    xml.setDevice(&file);

    parseWays(xml,query);


    if (xml.hasError()) {
        qDebug() << "XML Parsing Error:" << xml.errorString();
        return false;
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
               "lon DOUBLE NOT NULL,"
               "isImportant TINYINT(1) DEFAULT 0"  // New column for importance
               ") ENGINE=InnoDB;");

    // Query for creating ways table
    query.exec("CREATE TABLE IF NOT EXISTS ways ("
               "id VARCHAR(255) PRIMARY KEY,"
               "visible BOOLEAN,"
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

    if (!query.exec("CREATE TABLE IF NOT EXISTS address ("
                    "node_id VARCHAR(255) PRIMARY KEY, "
                    "housenumber VARCHAR(20), "
                    "street VARCHAR(255), "
                    "postcode VARCHAR(255), "
                    "city VARCHAR(255), "
                    "FOREIGN KEY (node_id) REFERENCES nodes(id) ON DELETE CASCADE"
                    ") ENGINE=InnoDB;")) {
        qDebug() << "Failed to create address table: " << query.lastError().text();
        db.rollback();
        return; // Or handle the error appropriately, e.g., by notifying the user
    }




    db.commit();
}

QMap<QString, QString> DatabaseManager::getWayTags(const QString &wayId) {
    QMap<QString, QString> tags;
    QSqlQuery query;

    // Query to get all tags for the specified way
    query.prepare("SELECT tag_key, value FROM tags WHERE element_id = :wayId AND element_type = 'way';");
    query.bindValue(":wayId", wayId);

    if (query.exec()) {
        while (query.next()) {
            auto key = query.value(0).toString();
            auto value = query.value(1).toString();
            tags.insert(key, value);
        }
    } else {
        qDebug() << "Failed to retrieve tags for way" << wayId << ":" << query.lastError();
    }

    return tags;
}