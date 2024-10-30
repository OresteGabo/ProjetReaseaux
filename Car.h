//
// Created by oreste on 29/10/24.
//

#ifndef GRAPHICSINQT_CAR_H
#define GRAPHICSINQT_CAR_H


#include <QGraphicsItem>
#include <QPixmap>
#include <QRandomGenerator>
#include  <cmath>
#include "PathNode.h"

class PathNode;

class Car:public QGraphicsItem {
public:
    Car(int id=0,PathNode* destinationPathHead = nullptr,
        double speed = 100.0,
        const QPixmap& car=QPixmap("car.png"),
        double frequence=QRandomGenerator::global()->bounded(10, 300)
    );

    void updatePosition(qreal elapsedTime,QVector<Car*> allCars);
    void draw(QPainter& painter) const;
    void logMessage(const QString &message, QPlainTextEdit *debugOutput )const;
    QString toString()const;
    QPointF getPosition()const{return position;}
    void updateConnectedCars(QVector<Car*> allCars);
    bool connectedTo(const Car*)const ;
    int getRadius()const{return frequence;}
    void resetPosition() {
        position=QPointF(destinationPathHead->getNode()->x(),destinationPathHead->getNode()->y());
        nextDestinationNode=destinationPathHead->next;
    }

    double getFrequence() const{return frequence;}




private:
    QPointF position;
    qreal speed;
    const PathNode* destinationPathHead;
    PathNode* nextDestinationNode;
    QPixmap carImage;
    double frequence;
    QVector<Car*> connectedCars;
public:
    const QVector<Car *> &getConnectedCars() const;



public:
};


#endif //GRAPHICSINQT_CAR_H
