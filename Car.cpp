#include "Car.h"
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QTimer>
Car::Car(const QString& id, const Path& path, QGraphicsScene* scene)
        : carId(id), path(path), scene(scene) {
    // Initialize car's graphical representation (a simple ellipse for now)
    carItem = new QGraphicsEllipseItem(0, 0, 1, 1);
    carItem->setBrush(Qt::red);

    // Get initial and destination node coordinates
    double initialLat, initialLon, destLat, destLon;
    auto initialNode = path.getHead();
    auto destinationNode = initialNode;

    while (destinationNode && destinationNode->next) {
        destinationNode = destinationNode->next;
    }

    if (!initialNode || !destinationNode ||
        !getNodeCoordinates(initialNode->nodeId, initialLat, initialLon) ||
        !getNodeCoordinates(destinationNode->nodeId, destLat, destLon)) {
        qDebug() << "Error: Failed to get node coordinates from database.";
        return;
    }

    // Convert lat/lon to scene coordinates using CustomScene::latLonToXY
    QPointF initialPos = CustomScene::latLonToXY(initialLat, initialLon);
    QPointF destinationPos = CustomScene::latLonToXY(destLat, destLon);

    // Set the initial position of the car item
    carItem->setPos(initialPos);

    // Add car item to the scene
    scene->addItem(carItem);

    // Update the destination marker
    updateDestination(destinationPos);

    // Draw the path
    drawPath();

    qDebug() << "Car created with ID:" << carId << "Initial Position:" << initialPos << "Destination:" << destinationPos;
}

void Car::setPosition(const QPointF& position) {
    carItem->setPos(position);
}

void Car::updateDestination(const QPointF& destinationPos) {
    if (destinationItem) {
        scene->removeItem(destinationItem);
        delete destinationItem;
    }
    destinationItem = new QGraphicsEllipseItem(destinationPos.x() - 5, destinationPos.y() - 5, 10, 10);
    destinationItem->setBrush(Qt::red);
    scene->addItem(destinationItem);
}

bool Car::getNodeCoordinates(const QString& nodeId, double& lat, double& lon) {
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

void Car::drawPath() {
    if (pathItem) {
        scene->removeItem(pathItem);
        delete pathItem;
    }

    QPainterPath painterPath;
    auto currentNode = path.getHead();
    double lat, lon;

    // Get the coordinates of the first node
    if (currentNode && getNodeCoordinates(currentNode->nodeId, lat, lon)) {
        QPointF startPoint = CustomScene::latLonToXY(lat, lon);
        painterPath.moveTo(startPoint);

        // Iterate through the path and add lines
        while (currentNode) {
            if (getNodeCoordinates(currentNode->nodeId, lat, lon)) {
                QPointF point = CustomScene::latLonToXY(lat, lon);
                painterPath.lineTo(point);
            }
            currentNode = currentNode->next;
        }

        // Create the QGraphicsPathItem and add it to the scene
        pathItem = new QGraphicsPathItem(painterPath);
        pathItem->setPen(QPen(Qt::blue, 2));
        scene->addItem(pathItem);
    }
}
void Car::moveAlongPath() {
    if (!path.getHead()) {
        qDebug() << "Path is empty. Cannot move the car.";
        return;
    }else{
        qDebug()<<"Head is not empty";
    }

    auto currentNode = path.getHead();
    qDebug()<<"current node (head)";
    auto timer = new QTimer(this);
    qDebug()<<"QTimerObject";
    timer->setInterval(1000); // Set the interval for each move (1 second for demonstration)
    qDebug()<<"Set interval to 1000";
    // Lambda function to move the car to the next node
    QObject::connect(timer, &QTimer::timeout, [this, timer, &currentNode]() mutable {
        if (!currentNode) {
            timer->stop();
            timer->deleteLater(); // Clean up the timer
            qDebug() << "Reached the end of the path.";
            return;
        }
        qDebug() << "CurrentNode exist";
        // Get the coordinates of the current node
        double lat, lon;
        if (!getNodeCoordinates(currentNode->nodeId, lat, lon)) {
            qDebug() << "Failed to get coordinates for node:" << currentNode->nodeId;
            timer->stop();
            return;
        }
        qDebug() << "get coordinates for node";
        // Convert lat/lon to scene coordinates
        QPointF pos = CustomScene::latLonToXY(lat, lon);

        // Move the car to the current position
        setPosition(pos);
        qDebug() << "Car moved to node:" << currentNode->nodeId << "Position:" << pos;

        // Move to the next node in the linked list
        currentNode = currentNode->next;

        // Stop if we reach the end of the path
        if (!currentNode) {
            qDebug() << "Car reached the destination.";
            timer->stop();
        }
    });

    // Start the timer
    timer->start();
}
