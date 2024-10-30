//
// Created by oreste on 28/10/24.
//

#ifndef NODE_H
#define NODE_H

#include <QGraphicsItem>
#include <QPainter>
#include <QString>
#include<QDebug>
#include "ConfigManager.h"
#include "CustomScene.h"
#include <QRandomGenerator>

class Node : public QGraphicsItem {
public:
    QString id;
    double lat;
    double lon;
    bool isJunction;
    QPointF position;

    Node(const QString &id, double lat, double lon, const QPointF &pos)
            : id(id), lat(lat), lon(lon), position(pos) {
        qDebug() << "Node constructor called with parameters - Id:" << id
                 << ", Latitude:" << lat
                 << ", Longitude:" << lon
                 << ", Position:" << pos;
        QRandomGenerator rd= QRandomGenerator();
        rd.bounded(1);
        isJunction= rd.bounded(1000) % 2 == 0;



    }


    [[nodiscard]] QPointF toPoint()const{
        return CustomScene::latLonToXY(lat,lon);
    }
    QRectF boundingRect() const override {
        return QRectF(position.x() - 5, position.y() - 5, 10, 10); // Node size
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        //painter->setBrush(Qt::blue);
        painter->drawEllipse(boundingRect());
    }
};

#endif // NODE_H

