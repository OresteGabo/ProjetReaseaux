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


signals:
    void debugMessage(const QString &message); // Signal to send debug messages
signals:
    //void wayLoaded(const QString &type, const QVector<QPointF> &wayPoints, const QColor &color, bool isClosed);
private slots:

    //void onWayLoaded(const QString &type, const QVector<QPointF> &wayPoints, const QColor &color, bool isClosed);

private:

    static QJsonObject loadJsonFile(const QString &configFileName="config.json");
    void loadNodesFromDatabase();
    void loadWaysFromDatabase();
    void loadSpecificWays(const QString &type, const QColor &color);
    //void updateSceneSize();
   // void drawBuilding3D(const QVector<QPointF> &points, const QColor &baseColor);

};

#endif // CUSTOMSCENE_H

