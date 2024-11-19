#ifndef CAR_H
#define CAR_H

#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QPointF>
#include <QString>
#include <QGraphicsPathItem>
#include "CustomScene.h"
#include "Path.h"
#include "DatabaseManager.h"
#include <QSqlError>
#include <QTextEdit>

class Car: public QObject  {
Q_OBJECT
public:
    Car(const QString& id, Path* path, QGraphicsScene* scene);

    // Set position of the car on the scene
    void setPosition(const QPointF& position);

    // Update the destination marker
    void updateDestination(const QPointF& destinationPos);

    // Get coordinates of a node from the database
    bool getNodeCoordinates(const QString& nodeId, double& lat, double& lon);

    // Draw the path for the car
    void drawPath();

    // Move the car along its path
    //void moveAlongPath();

    void updatePosition(qreal elapsedTime,QVector<Car*> allCars);
    void updateConnectedCars(QVector<Car*> allCars);
    bool connectedTo(const Car *car) const;
    [[nodiscard]] QVector<Car*> getConnectedCars()const{return connectedCars;}

    void display(QTextEdit* edit){
        edit->append( "Car created with ID:"+ carId);

        auto x=DatabaseManager::getPositionByNodeId(path->head->nodeId).x();
        auto y=DatabaseManager::getPositionByNodeId(path->head->nodeId).y();
        edit->append( " Head node ID:" +path->head->nodeId +" pos ("+ QString::number(x)+","+QString::number(y)+")");
        //currentPosition=QPointF(x,y);
        edit->append( " current pos is:" +QString::number(currentPosition->x())+","+QString::number(currentPosition->y()));

        x=DatabaseManager::getPositionByNodeId(nextDestinationNode->nodeId).x();
        y=DatabaseManager::getPositionByNodeId(nextDestinationNode->nodeId).y();
        edit->append("next node is"+QString::number(x)+","+QString::number(y)+")");

        x=DatabaseManager::getPositionByNodeId(path->tail->nodeId).x();
        y=DatabaseManager::getPositionByNodeId(path->tail->nodeId).y();

        edit->append( " Destination node ID:" + path->tail->nodeId +" Pos("+QString::number(x)+","+QString::number(y)+")");
        edit->append(" Number of nodes: "+QString::number(path->size()));
    }
    PathNode* nextDestinationNode;
    QPointF *currentPosition;


    QString carId;
    Path *path;  // The path the car follows
    QGraphicsScene* scene;
    QGraphicsEllipseItem* carItem;       // Visual representation of the car
    QGraphicsEllipseItem* destinationItem = nullptr;  // Destination marker
    QGraphicsPathItem* pathItem = nullptr;  // Visual representation of the path
    QVector<Car*>connectedCars={};
    double speed=100;
    double frequence=rand()%100;
    void initialiseDesinationNodes(){
        /*auto crt=path->head;
        nextDestinationNode=crt;
        while(nextDestinationNode){
            qDebug()<<nextDestinationNode->nodeId;
            nextDestinationNode=nextDestinationNode->next;
        }
        nextDestinationNode=path->head;*/
        qDebug()<<"&&&&&&&&&&&&&&&&";
    }

    int currentNodeIndex = 0; // Current position in the path
};

#endif // CAR_H
