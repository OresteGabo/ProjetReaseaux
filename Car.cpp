#include "Car.h"
#include "DatabaseManager.h"
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QTimer>
#include <QtMath>
Car::Car(const QString& id, Path* path, QGraphicsScene* scene)
        : carId(id), path(path), scene(scene) ,nextDestinationNode(path->head->next),currentPosition(new QPointF(path->head->getPosition())){
    // Initialize car's graphical representation (a simple ellipse for now)
    carItem = new QGraphicsEllipseItem(0, 0, 2, 2);
    carItem->setBrush(Qt::red);
initialiseDesinationNodes();


    // Get initial and destination node coordinates
    double initialLat, initialLon, destLat, destLon;
    auto initialNode = path->head;
    auto destinationNode = initialNode;
    qDebug()<<"Current position after ini t is "<<path->head->getPosition().x()<<" , "<<path->head->getPosition().y();

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

    nextDestinationNode=path->head->next;


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
    destinationItem = new QGraphicsEllipseItem(destinationPos.x() - 2, destinationPos.y() - 2, 4, 4);
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
    auto currentNode = path->head;
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
        pathItem->setPen(QPen(Qt::blue, 0.5));
        scene->addItem(pathItem);
    }
}
/*
void Car::updatePosition(qreal elapsedTime,QVector<Car*> allCars) {
    if(!path.nextDestinationNode){
        qDebug()<<"No destination";
        return;
    }


    // Update the position based on the elapsed time and speed
    double nextY=DatabaseManager::getPositionByNodeId(path.nextDestinationNode->nodeId).y();
    double nextX=DatabaseManager::getPositionByNodeId(path.nextDestinationNode->nodeId).x();

    double x=path.currentPosition.x();
    double y=path.currentPosition.y();
    qreal deltaX = speed * elapsedTime * qCos(qAtan2(nextY - y, nextX - x));
    qreal deltaY = speed * elapsedTime * qSin(qAtan2(nextY - y, nextX - x));


    DatabaseManager::getPositionByNodeId("");
    path.currentPosition += QPointF(deltaX, deltaY);
    qDebug()<<"Current position is ";
    qDebug()<<path.currentPosition;

    // Check if the car has reached the current destination node
    qreal distance = (path.currentPosition - DatabaseManager::getPositionByNodeId(path.nextDestinationNode->nodeId)).manhattanLength();
    if (distance <= 1.0) {
        // If the distance is small enough, consider it reached and set the next destination node
        path.nextDestinationNode=path.nextDestinationNode->next;
        if (!path.nextDestinationNode) {
            // If there's no next destination, stop the car
            speed = 0.0;
        }
    }

    updateConnectedCars(allCars);
}
*/

void Car::updatePosition(qreal elapsedTime, QVector<Car*> allCars) {
    speed=100;
    frequence=30;
    if (!nextDestinationNode) {
        return;  // No destination path set, do nothing
    }

    // Update the position based on the elapsed time and speed
    auto pos=DatabaseManager::getPositionByNodeId(nextDestinationNode->nodeId);
    qreal deltaX = speed * elapsedTime * qCos(qAtan2(pos.y() - currentPosition->y(), pos.x() - currentPosition->x()));
    qreal deltaY = speed * elapsedTime * qSin(qAtan2(pos.y() - currentPosition->y(), pos.x() - currentPosition->x()));
    qDebug()<<"delta x:"<<deltaX<<" deltaY: "<<deltaY;
    qDebug()<<"currentPosition is "<<*currentPosition;
    //*currentPosition += QPointF(deltaX, deltaY);
    currentPosition=new QPointF(currentPosition->x()+(deltaX*10000),currentPosition->y()+(deltaY*10000));
    qDebug()<<"currentPosition is "<<*currentPosition;

    // Check if the car has reached the current destination node
    qreal distance = (*currentPosition - (DatabaseManager::getPositionByNodeId(nextDestinationNode->nodeId))).manhattanLength();
    if (distance <= 1.0) {
        // If the distance is small enough, consider it reached and set the next destination node
        nextDestinationNode = nextDestinationNode->next;
        if (!nextDestinationNode) {
            // If there's no next destination, stop the car
            speed = 0.0;
        }
    }

    setPosition(*currentPosition);
    updateConnectedCars(allCars);

}



void Car::updateConnectedCars(QVector<Car*> allCars) {
    connectedCars.clear();
    for (const Car* otherCar : allCars) {
        if (otherCar != this && connectedTo( otherCar)) {
            connectedCars.push_back(const_cast<Car*>(otherCar));
        }
    }
}
bool Car::connectedTo(const Car *car) const {
    QPointF *position2 = car->currentPosition;//car->getPosition();
    double radius1=frequence;
    double radius2 = car->frequence;

    // Calculate the distance between the centers of the circles
    double distance= std::hypot(currentPosition->x()-position2->x(),currentPosition->y()-position2->y());
    // Determine if the circles intersect based on their radii and distance
    //return distance < (getRadius() + radius2);
    return radius1 > distance || radius2> distance;
}