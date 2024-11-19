//
// Created by oreste on 28/10/24.
//



#include "DatabaseManager.h"
//#include <QDebug>
DatabaseManager::DatabaseManager(
        const QString& dbName,
        const QString& user,
        const QString& password,
        const QString& fileName)
{
    initialiseDatabase(dbName, user, password);
    QSqlQuery query;
    AdjacencyList adjList = buildAdjacencyList();

// Print the adjacency list
    for (auto it = adjList.begin(); it != adjList.end(); ++it) {
        QString node = it.key();

        QSet<QString> neighbors = it.value();

        //qDebug() << "Node" << node << "is connected to:" << neighbors;
    }




    //if(!query.prepare("SELECT COUNT(*) FROM ways")) {
        //parseData(fileName);
        //ConfigManager cf = ConfigManager();
        //calculateAndSaveBoundsToConfig(cf);
    //}
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



QVector<QString> DatabaseManager::getDrivableWaysIds() {
    QVector<QString> wayIds;

    // Define the query to get drivable ways that also have a name
    QSqlQuery query;
    QString sql = R"(
        SELECT DISTINCT t1.element_id
        FROM tags t1
        JOIN tags t2 ON t1.element_id = t2.element_id
        WHERE t1.element_type = 'way'
          AND t1.tag_key = 'highway'
          AND t1.value IN ('service', 'primary', 'residential', 'tertiary',
                           'unclassified', 'secondary', 'trunk', 'trunk_link',
                           'living_street', 'motorway_link')
          AND t2.tag_key = 'name'
    )";

    // Execute the query
    if (query.exec(sql)) {
        // Fetch the result and store the way IDs
        while (query.next()) {
            QString wayId = query.value(0).toString();
            wayIds.push_back(wayId);
        }
    } else {
        qDebug() << "Query execution failed:" << query.lastError().text();
    }

    return wayIds;
}



QString DatabaseManager::getWayNameById(const QString& wayId) {
    QString wayName;


    // Prepare the SQL query
    QSqlQuery query;
    query.prepare(R"(
        SELECT DISTINCT tags.value
        FROM tags
        JOIN ways ON tags.element_id = ways.id
        WHERE tags.tag_key = 'name'
          AND ways.id = :wayId
    )");

    // Bind the way ID parameter
    query.bindValue(":wayId", wayId);

    // Execute the query and fetch the result
    if (query.exec()) {
        if (query.next()) {
            wayName = query.value(0).toString();
        } else {
            qDebug() << "No name found for way ID:" << wayId;
        }
    } else {
        qDebug() << "Query execution failed:" << query.lastError().text();
    }

    return wayName;
}
QVector<QString> DatabaseManager::getNodesOfWay(const QString& wayId) {
    QVector<QString> nodeIds;

    // SQL query to fetch the node IDs of the given way, ordered by node_order
    QSqlQuery query;
    QString sql = R"(
        SELECT node_id
        FROM ways_nodes
        WHERE way_id = :wayId
        ORDER BY node_order
    )";

    // Prepare and bind the way ID parameter
    query.prepare(sql);
    query.bindValue(":wayId", wayId);

    // Execute the query and fetch the node IDs
    if (query.exec()) {
        while (query.next()) {
            QString nodeId = query.value(0).toString();
            nodeIds.push_back(nodeId);
        }
    } else {
        qDebug() << "Failed to retrieve nodes for way ID" << wayId << ":" << query.lastError().text();
    }

    return nodeIds;
}


AdjacencyList DatabaseManager::buildAdjacencyList() {
    AdjacencyList adjList;

    // Step 1: Get the list of drivable way IDs
    QVector<QString> drivableWayIds = getDrivableWaysIds();

    // Step 2: Iterate through each drivable way
    for (const auto& wayId : drivableWayIds) {
        // Get the list of nodes for this way
        QVector<QString> nodes = getNodesOfWay(wayId);

        // Step 3: Add edges between consecutive nodes
        for (int i = 0; i < nodes.size() - 1; ++i) {
            const QString& currentNode = nodes[i];
            const QString& nextNode = nodes[i + 1];

            // Add the edge in both directions (undirected graph)
            adjList[currentNode].insert(nextNode);
            adjList[nextNode].insert(currentNode);
        }
    }

    return adjList;
}

QVector<QString> DatabaseManager::getNodesOfWaysWithName() {
    QVector<QString> nodeIds;
    QSqlQuery query;
    QString queryString=R"(
SELECT element_id
FROM tags
WHERE element_type = 'way'
  AND tag_key = 'name'
  AND value LIKE '%rue%'
   OR element_type = 'way'
  AND tag_key = 'name'
  AND value LIKE '%avenue%'
   OR element_type = 'way'
  AND tag_key = 'name'
  AND value LIKE '%boulevard%';
)";
    // Query to get nodes with the 'addr:street' tag
    //QString queryString = "SELECT element_id FROM tags WHERE element_type = 'node' AND tag_key = 'addr:street'";

    if (!query.exec(queryString)) {
        qDebug() << "Failed to execute query:" << query.lastError().text();
        return nodeIds; // Return an empty vector if the query fails
    }

    // Fetch and store the node IDs
    while (query.next()) {
        QString nodeId = query.value(0).toString();
        nodeIds.append(nodeId);
    }

    return nodeIds;
}

QString DatabaseManager::getWayNameForNode(const QString &nodeId) {
    QSqlQuery query;
    query.prepare(R"(
        SELECT t.value AS way_name
        FROM ways_nodes wn
        JOIN tags t ON wn.way_id = t.element_id
        WHERE wn.node_id = :nodeId
          AND t.element_type = 'way'
          AND t.tag_key = 'name'
          AND (t.value LIKE '%rue%' OR t.value LIKE '%avenue%' OR t.value LIKE '%boulevard%')
        LIMIT 1;
    )");

    query.bindValue(":nodeId", nodeId);

    if (!query.exec()) {
        qDebug() << "Database query failed:" << query.lastError().text();
        return QString();
    }

    if (query.next()) {
        return query.value(0).toString(); // Return the way name
    }

    return QString(); // Return an empty string if no result is found
}

QPointF DatabaseManager::getPositionByNodeId(QString nodeId){
    QSqlQuery sql;
    QString quertString=R"(
        select lat,lon from nodes where id = :nodeId
    )";
    sql.prepare(quertString);
    sql.bindValue(":nodeId", nodeId);
    if(sql.exec()){
        if (sql.next()) {
            double lat = sql.value(0).toDouble();
            double lon = sql.value(1).toDouble();
            return CustomScene::latLonToXY(lat, lon);
        }
    }
    qDebug()<<"The pos of node "+nodeId+" is not found";
    return {5,5};
}

/*
QString DatabaseManager::getStreetNameByNode(const QString &nodeId) {
    //select value from tags where element_type ='node' and tag_key ='addr:street' and element_id='10121250337' ;

    QVector<QString> nodeIds;

    // SQL query to fetch the node IDs of the given way, ordered by node_order
    QSqlQuery query;
    QString sql = R"(
        SELECT value
        FROM tags
        WHERE element_type ='node'
        AND tag_key ='addr:street'
        AND element_id = :nodeId
    )";

    query.prepare(sql);
    query.bindValue(":wayId", nodeId);


    if(query.exec()){
        return query.value(0).toString();
    }else {
        qDebug() << "Failed to retrieve the street name for node ID" << nodeId << ":" << query.lastError().text();
    }


    return "#";
}
*/