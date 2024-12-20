#include "CustomScene.h"
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QGraphicsEllipseItem>
#include <QFile>
#include<QJsonDocument>
#include <QJsonObject>
#include <QPlainTextEdit>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>

CustomScene::CustomScene(int width, int height, QObject *parent)
        : QGraphicsScene(parent) {
    setSceneRect(0, 0, width, height);
    setBackgroundBrush(QBrush(Qt::lightGray));
    loadWaysFromDatabase();
    //loadNodesFromDatabase();

}

void CustomScene::loadNodesFromDatabase() {
    QSqlQuery query;
    query.exec("SELECT id, lat, lon, isImportant FROM nodes");

    while (query.next()) {
        QString nodeId = query.value(0).toString();
        double lat = query.value(1).toDouble();
        double lon = query.value(2).toDouble();
        bool isImportant = query.value(3).toBool();
        if(isImportant) {
            QPointF position = latLonToXY(lat, lon);
            // Create a visual representation of the node
            QGraphicsEllipseItem *nodeItem = addEllipse(position.x() - 0.2, position.y() - 0.2, 0.4, 0.4, QPen(Qt::blue),
                                                        QBrush(Qt::blue));
            nodeItem->setData(0, nodeId); // Store the node ID in the item's data for later reference
        }
    }
}



void CustomScene::loadWaysFromDatabase() {
    // Define QFutureWatcher objects
    auto highwayWatcher = new QFutureWatcher<void>(this);
    auto waterWatcher = new QFutureWatcher<void>(this);
    auto buildingWatcher = new QFutureWatcher<void>(this);
    auto landuseWatcher = new QFutureWatcher<void>(this);

    // Helper lambda for updating the scene
    auto updateScene = [this]() {
        qDebug() << "Scene updated after a task completed.";
        this->update(); // Trigger a repaint of the scene
    };

    // Connect watchers to update the scene when each task finishes loading
    connect(highwayWatcher, &QFutureWatcher<void>::finished, this, updateScene);
    connect(waterWatcher, &QFutureWatcher<void>::finished, this, updateScene);
    connect(buildingWatcher, &QFutureWatcher<void>::finished, this, updateScene);
    connect(landuseWatcher, &QFutureWatcher<void>::finished, this, updateScene);

    // Use QtConcurrent to run loadSpecificWays in separate threads
    highwayWatcher->setFuture(QtConcurrent::run([this]() {
        loadSpecificWays("highway", Qt::darkGray);
    }));

    waterWatcher->setFuture(QtConcurrent::run([this]() {
        loadSpecificWays("waterway", QColor(0, 150, 255, 120));
    }));

    buildingWatcher->setFuture(QtConcurrent::run([this]() {
        loadSpecificWays("building", QColor(150, 75, 0, 150));
    }));

    landuseWatcher->setFuture(QtConcurrent::run([this]() {
        loadSpecificWays("landuse", QColor(34, 139, 34, 150));
    }));
}


void CustomScene::loadSpecificWays(const QString &type, const QColor &color) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", type); // Unique connection name
    db.setDatabaseName("OSMData");
    db.setHostName("127.0.0.1");
    db.setUserName("oreste");
    db.setPassword("Muhirehonore@1*");

    if (!db.open()) {
        qDebug() << "Database connection failed for" << type << ":" << db.lastError().text();
        return;
    }

    emit debugMessage("Started loading " + type); // Emit a debug message
    QSqlQuery query(db);
    query.prepare("SELECT id FROM ways WHERE id IN (SELECT element_id FROM tags WHERE tag_key = :type)");
    query.bindValue(":type", type);
    query.exec();

    QVector<QPolygonF> polygons;
    QVector<QPainterPath> paths;

    while (query.next()) {
        QString wayId = query.value(0).toString();
        QVector<QPointF> wayPoints;

        // Get the node references for this way
        QSqlQuery wayQuery(db);
        wayQuery.prepare("SELECT node_id FROM ways_nodes WHERE way_id = :wayId ORDER BY node_order");
        wayQuery.bindValue(":wayId", wayId);
        wayQuery.exec();

        while (wayQuery.next()) {
            QString nodeId = wayQuery.value(0).toString();

            QSqlQuery nodeQuery(db);
            nodeQuery.prepare("SELECT lat, lon FROM nodes WHERE id = :id");
            nodeQuery.bindValue(":id", nodeId);

            if (!nodeQuery.exec()) {
                qDebug() << "Node query execution failed:" << nodeQuery.lastError().text();
                continue;
            }

            if (nodeQuery.next()) {
                double lat = nodeQuery.value(0).toDouble();
                double lon = nodeQuery.value(1).toDouble();
                QPointF pos = latLonToXY(lat, lon);
                wayPoints.append(pos);
            }
        }

        if (!wayPoints.isEmpty()) {
            if (wayPoints.first() == wayPoints.last()) {
                // Closed polygon (e.g., buildings)
                polygons.append(QPolygonF(wayPoints));
            } else {
                // Open polyline (e.g., roads, rivers)
                QPainterPath path;
                path.moveTo(wayPoints.first());
                for (const QPointF &point : wayPoints) {
                    path.lineTo(point);
                }
                paths.append(path);
            }
        }
    }

    db.close();
    QSqlDatabase::removeDatabase(type);

    // Schedule GUI updates in the main thread
    QMetaObject::invokeMethod(this, [this, type, color, polygons, paths]() {
        QPen pen(color);
        if (type != "highway") {
            pen.setCosmetic(true);
        }

        pen.setJoinStyle(Qt::RoundJoin);
        pen.setCapStyle(Qt::RoundCap);

        for (const auto &polygon : polygons) {
            addPolygon(polygon, pen, QBrush(color));
        }
        for (const auto &path : paths) {
            addPath(path, pen, QBrush(Qt::NoBrush));
        }

        qDebug() << "Finished loading" << type << "ways.";
        this->update();
    }, Qt::QueuedConnection);

    emit debugMessage("Finished loading " + type); // Emit another debug message
}


QPointF CustomScene::latLonToXY(double lat, double lon) {
    // Here you would implement the conversion using the previously established bounds
    // This is just a placeholder, please adjust according to your bounds calculations

    QJsonObject jsonObj= loadJsonFile();
    QJsonObject boundObj = jsonObj.value("Bound").toObject();
    QJsonObject screenObj = jsonObj.value("MainWindow").toObject();

    int width = screenObj.value("width").toInt();
    int height = screenObj.value("height").toInt();
    double minLat = boundObj.value("minLat").toDouble();
    double maxLat = boundObj.value("maxLat").toDouble();
    double minLon = boundObj.value("minLon").toDouble();
    double maxLon = boundObj.value("maxLon").toDouble();

    double x = (lon - minLon) / (maxLon - minLon) * width;
    double y = height - (lat - minLat) / (maxLat - minLat) * height;

    return QPointF(x, y);
}

QJsonObject CustomScene::loadJsonFile(const QString &configFileName) {
    QFile file(configFileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open JSON file:" << configFileName;
        return QJsonObject();
    }
    QByteArray jsonData = file.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonData));
    return  jsonDoc.object();
}

