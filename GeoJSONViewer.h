#ifndef GEOJSONVIEWER_H
#define GEOJSONVIEWER_H

#include <QWidget>
#include <QJsonObject>
#include <QPointF>
#include <QMouseEvent>

class GeoJSONViewer : public QWidget {
    Q_OBJECT

public:
    explicit GeoJSONViewer(QWidget *parent = nullptr, double zoomFactor = 100.0);
    void setZoomFactor(double zoomFactor);

    void loadGeoJSON(const QString &filePath);


protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    public slots:
        void adjustZoom(bool zoomIn);
    void adjustPan(double deltaX, double deltaY);

    //void setZoomFactor(double zoomFactor);  // Add this slot


private:
    QPointF normalizeCoordinates(double lon, double lat, double width, double height);

    QJsonObject geoJSON;
    double minLon, maxLon, minLat, maxLat;
    double zoomFactor;
    double offsetX, offsetY;

    // Variables for mouse interaction
    bool isPanning;
    QPoint lastMousePosition;
};

#endif // GEOJSONVIEWER_H
