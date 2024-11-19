#ifndef CAR_H
#define CAR_H

#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QPointF>
#include <QString>
#include <QGraphicsPathItem>
#include "CustomScene.h"
#include "Path.h"
#include <QSqlError>

class Car: public QObject  {
    Q_OBJECT
public:
    Car(const QString& id, const Path& path, QGraphicsScene* scene);

    // Set position of the car on the scene
    void setPosition(const QPointF& position);

    // Update the destination marker
    void updateDestination(const QPointF& destinationPos);

    // Get coordinates of a node from the database
    bool getNodeCoordinates(const QString& nodeId, double& lat, double& lon);

    // Draw the path for the car
    void drawPath();

    // Move the car along its path
    void moveAlongPath();



private:
    QString carId;
    Path path;  // The path the car follows
    QGraphicsScene* scene;
    QGraphicsEllipseItem* carItem;       // Visual representation of the car
    QGraphicsEllipseItem* destinationItem = nullptr;  // Destination marker
    QGraphicsPathItem* pathItem = nullptr;  // Visual representation of the path


    int currentNodeIndex = 0; // Current position in the path
};

#endif // CAR_H
