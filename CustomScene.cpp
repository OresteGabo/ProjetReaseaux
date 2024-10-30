#include "CustomScene.h"
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QGraphicsEllipseItem>
#include <QFile>
#include<QJsonDocument>
#include <QJsonObject>
#include <QPlainTextEdit>

CustomScene::CustomScene(int width, int height, QObject *parent)
        : QGraphicsScene(parent) {
    setSceneRect(0, 0, width, height);
    auto debugOutput=new QPlainTextEdit();
    debugOutput->appendPlainText("Hello debugger");
    addWidget(debugOutput);
    //setBackgroundBrush(Qt::blue);
    loadNodesFromDatabase();
    loadWaysFromDatabase();

}

void CustomScene::loadNodesFromDatabase() {
    QSqlQuery query;
    query.exec("SELECT node_id, latitude, longitude FROM nodes");

    while (query.next()) {
        QString nodeId = query.value(0).toString();
        double lat = query.value(1).toDouble();
        double lon = query.value(2).toDouble();

        QPointF position = latLonToXY(lat, lon);
        // Create a visual representation of the node
        QGraphicsEllipseItem *nodeItem = addEllipse(position.x() - 2, position.y() - 2, 4, 4, QPen(Qt::blue), QBrush(Qt::blue));
        nodeItem->setData(0, nodeId); // Store the node ID in the item's data for later reference
    }
}

void CustomScene::loadWaysFromDatabase() {

    QSqlQuery query;
    query.exec("SELECT id FROM ways");

    while (query.next()) {

        QString wayId = query.value(0).toString();
        QVector<QPointF> wayPoints;

        // Get the node references for this way
        QSqlQuery wayQuery;
        wayQuery.prepare("SELECT node_id FROM ways_nodes WHERE way_id = :wayId ORDER BY node_order");
        wayQuery.bindValue(":wayId", wayId);
        wayQuery.exec();

        while (wayQuery.next()) {
            //qDebug()<<"load ways called  and there is a next";
            QString nodeId = wayQuery.value(0).toString();
            // Fetch node coordinates based on node ID
            QSqlQuery nodeQuery;
            nodeQuery.prepare("SELECT lat, lon FROM nodes WHERE id = :id");
            nodeQuery.bindValue(":id", nodeId);

            if (!nodeQuery.exec()) {
                qDebug() << "Query execution failed:" << nodeQuery.lastError().text();
            }
            ///TODO nodeQuery .next is always false
            if (nodeQuery.next()) {
                double lat = nodeQuery.value(0).toDouble();
                double lon = nodeQuery.value(1).toDouble();
                QPointF pos= latLonToXY(lat, lon);
                wayPoints.append(pos);
                //qDebug() << "QPointF pos: " << pos;
            }else{
               // qDebug() << "ERROR";
            }
        }

        // Draw the way depending on whether it's a closed or open polygon
        if (!wayPoints.isEmpty()) {
            if (wayPoints.first() == wayPoints.last()) {
                // Closed polygon
                QPolygonF polygon(wayPoints);
                addPolygon(polygon, QPen(Qt::green), QBrush(Qt::green));
            } else {
                // Open polyline
                QPainterPath path;
                path.moveTo(wayPoints.first());
                for (const QPointF &point : wayPoints) {
                    path.lineTo(point);
                }
                addPath(path, QPen(Qt::red), QBrush(Qt::NoBrush));
            }
        }
    }
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

