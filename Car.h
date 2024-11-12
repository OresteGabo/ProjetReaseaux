#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QString>
#include <QPointF>
#include "CustomScene.h"
#include <QSqlError>
class Car {
public:
    Car(const QString &id, const QString &initialNodeId, const QString &destinationNodeId, QGraphicsScene *scene)
            : carId(id), initialNode(initialNodeId), destinationNode(destinationNodeId), scene(scene) {
        // Initialize car's graphical representation (a simple ellipse for now)
        carItem = new QGraphicsEllipseItem(0, 0, 1, 1);  // Create a circle for the car

        carItem->setBrush(Qt::red);  // Set the car color

        // Get latitude and longitude from the database
        double initialLat, initialLon, destLat, destLon;
        if (!getNodeCoordinates(initialNodeId, initialLat, initialLon) ||
            !getNodeCoordinates(destinationNodeId, destLat, destLon)) {
            qDebug() << "Error: Failed to get node coordinates from database.";
            return;
        }

        // Convert lat/lon to scene coordinates using CustomScene::latLonToXY
        QPointF initialPos = CustomScene::latLonToXY(initialLat, initialLon);
        qDebug()<<"Initial position is ("+QString::number(initialPos.x())+","+QString::number(initialPos.y())+")";
        QPointF destinationPos = CustomScene::latLonToXY(destLat, destLon);
        qDebug()<<"destinationPos position is ("+QString::number(destinationPos.x())+","+QString::number(destinationPos.y())+")";

        // Set the initial position of the car item
        carItem->setPos(initialPos);

        // Add car item to the scene
        scene->addItem(carItem);

        qDebug() << "Car created with ID:" << carId << "Initial Position:" << initialPos << "Destination:" << destinationPos;
    }


    // Method to set position on the scene (using QPointF)
    void setPosition(const QPointF &position) {
        carItem->setPos(position);
    }

    // Method to update the destination (e.g., show a red circle for the destination)
    void updateDestination(const QPointF &destinationPos) {
        // Update or draw the destination marker (simple red circle for example)
        if (destinationItem) {
            scene->removeItem(destinationItem);
            delete destinationItem;
        }
        destinationItem = new QGraphicsEllipseItem(destinationPos.x() - 5, destinationPos.y() - 5, 10, 10);
        destinationItem->setBrush(Qt::red);  // Red circle for the destination
        scene->addItem(destinationItem);
    }
    bool getNodeCoordinates(const QString &nodeId, double &lat, double &lon) {
        QSqlQuery query;
        query.prepare("SELECT lat, lon FROM nodes WHERE id = :nodeId");
        query.bindValue(":nodeId", nodeId);

        if (!query.exec()) {
            qDebug() << "Database query error:" << query.lastError().text();
            return false;
        }

        if (query.next()) {
            lat = query.value(0).toDouble();
            lon = query.value(1).toDouble();
            return true;
        } else {
            qDebug() << "Node ID not found in database:" << nodeId;
            return false;
        }
    }

private:
    QString carId;
    QString initialNode;
    QString destinationNode;
    QGraphicsScene *scene;
    QGraphicsEllipseItem *carItem;  // Visual representation of the car
    QGraphicsEllipseItem *destinationItem = nullptr;  // Destination marker
};
