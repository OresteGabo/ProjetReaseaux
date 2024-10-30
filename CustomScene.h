#ifndef CUSTOMSCENE_H
#define CUSTOMSCENE_H

#include <QGraphicsScene>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <QPolygonF>
#include <QGraphicsPolygonItem>
#include <QGraphicsLineItem>
#include <QGraphicsItem>
#include <QVector>
#include <QDebug>

class CustomScene : public QGraphicsScene {
Q_OBJECT

public:
    CustomScene(int width, int height, QObject *parent = nullptr);
    static QPointF latLonToXY(double lat, double lon);

private:

    static QJsonObject loadJsonFile(const QString &configFileName="config.json");
    void loadNodesFromDatabase();
    void loadWaysFromDatabase();
};

#endif // CUSTOMSCENE_H

