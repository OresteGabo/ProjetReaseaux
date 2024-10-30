//
// Created by oreste on 29/10/24.
//

#include "Car.h"
#include "PathNode.h"
#include<QRandomGenerator>
#include <QBrush>
#include <QtMath>

Car::Car(int id,PathNode* destinationPathHead, double speed,const QPixmap& car,double frequence) :

        speed(speed), destinationPathHead(destinationPathHead),
        position(destinationPathHead->getNode()->toPoint()),
        carImage(car),
        frequence(frequence)
{
    carImage = carImage.scaledToWidth(30);
    // Set the initial position based on the starting destination node
    if (destinationPathHead) {
        position = QPointF(destinationPathHead->getNode()->x(), destinationPathHead->getNode()->y());
        nextDestinationNode = destinationPathHead->next;
        qDebug()<<"The car initialised with path adress (" << destinationPathHead->getNode()->x()<<","<<destinationPathHead->getNode()->y()<<")";
    } else {
        // If no starting node is provided, set an initial position at (0, 0)
        position = QPointF(0, 0);
        qDebug()<<"The car initialised with 0,0";
        nextDestinationNode = nullptr;
    }

}
void Car::updatePosition(qreal elapsedTime,QVector<Car*> allCars) {
    if (!nextDestinationNode) {
        return;  // No destination path set, do nothing
    }

    // Update the position based on the elapsed time and speed
    qreal deltaX = speed * elapsedTime * qCos(qAtan2(nextDestinationNode->getNode()->y() - position.y(), nextDestinationNode->getNode()->x() - position.x()));
    qreal deltaY = speed * elapsedTime * qSin(qAtan2(nextDestinationNode->getNode()->y() - position.y(), nextDestinationNode->getNode()->x() - position.x()));

    position += QPointF(deltaX, deltaY);

    // Check if the car has reached the current destination node
    qreal distance =0.1; // (position - *(nextDestinationNode->getNode())).manhattanLength();
    if (distance <= 1.0) {
        // If the distance is small enough, consider it reached and set the next destination node
        nextDestinationNode = nextDestinationNode->next;
        if (!nextDestinationNode) {
            // If there's no next destination, stop the car
            speed = 0.0;
        }
    }

    updateConnectedCars(allCars);
}
bool Car::connectedTo(const Car *car) const {
    QPointF position2 = car->getPosition();
    double radius1=frequence;
    double radius2 = car->getFrequence();

    // Calculate the distance between the centers of the circles
    double distance = std::hypot(position.x() - position2.x(), position.y() - position2.y());

    // Determine if the circles intersect based on their radii and distance
    //return distance < (getRadius() + radius2);
    return radius1 > distance || radius2> distance;
}
void Car::updateConnectedCars(QVector<Car*> allCars) {
    connectedCars.clear();
    for (const Car* otherCar : allCars) {
        if (otherCar != this && connectedTo( otherCar)) {
            connectedCars.push_back(const_cast<Car*>(otherCar));
        }
    }
}