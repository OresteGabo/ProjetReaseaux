#include "geojsonviewer.h"
#include <QPainter>
#include <QPaintEvent>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QWheelEvent>
#include <QKeyEvent>
#include <iostream>

GeoJSONViewer::GeoJSONViewer(QWidget *parent, double zoomFactor)
    : QWidget(parent), minLon(0), maxLon(0), minLat(0), maxLat(0),
      zoomFactor(zoomFactor), offsetX(0), offsetY(0), isPanning(false) {
    // Set the default zoom factor to 100
}

void GeoJSONViewer::loadGeoJSON(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        std::cerr << "Failed to open file: " << filePath.toStdString() << std::endl;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        std::cerr << "Invalid GeoJSON format." << std::endl;
        return;
    }

    geoJSON = doc.object();

    // Extract bounding box from GeoJSON data
    if (geoJSON.contains("features") && geoJSON["features"].isArray()) {
        QJsonArray features = geoJSON["features"].toArray();
        for (const auto &feature : features) {
            if (feature.isObject()) {
                QJsonObject geometry = feature.toObject()["geometry"].toObject();
                if (geometry["type"].toString() == "Polygon") {
                    QJsonArray coordinates = geometry["coordinates"].toArray();
                    for (const auto &ring : coordinates) {
                        QJsonArray points = ring.toArray();
                        for (const auto &point : points) {
                            QJsonArray coords = point.toArray();
                            double lon = coords[0].toDouble();
                            double lat = coords[1].toDouble();

                            if (minLon == 0 && maxLon == 0) {
                                minLon = maxLon = lon;
                                minLat = maxLat = lat;
                            } else {
                                minLon = std::min(minLon, lon);
                                maxLon = std::max(maxLon, lon);
                                minLat = std::min(minLat, lat);
                                maxLat = std::max(maxLat, lat);
                            }
                        }
                    }
                }
            }
        }
    }

    // Calculate the center of the bounding box

    double centerLon = ((minLon + maxLon) / 2 );
    ; //47.75128049862095, 7.327152019332519
    printf("lon bf: %f", centerLon);
    centerLon = centerLon  + 0.00000045;
  //  centerLon = 7.327152;
    printf("lon af: %f" ,centerLon);

    double centerLat = ((minLat + maxLat) / 2);
   centerLat = centerLat - 0.0000005;

    printf("lat bf : %f", centerLat);

   // centerLat = 47.751280;
    printf("lat af: %f", centerLat);

    // Update the offset to center the map on the widget
    offsetX = width() / 2 - normalizeCoordinates(centerLon, centerLat, width(), height()).x();
    offsetY = height() / 2 - normalizeCoordinates(centerLon, centerLat, width(), height()).y();

    // Set an initial zoom factor (optional)
    zoomFactor = 12.0;

    update(); // Refresh the view
}

void GeoJSONViewer::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Background
    painter.fillRect(rect(), Qt::white);

    // Check if GeoJSON is loaded
    if (geoJSON.isEmpty()) {
        painter.drawText(rect(), Qt::AlignCenter, "No GeoJSON data loaded");
        return;
    }

    // Draw the features
    painter.setPen(Qt::blue);
    painter.setBrush(Qt::NoBrush);

    if (geoJSON.contains("features") && geoJSON["features"].isArray()) {
        QJsonArray features = geoJSON["features"].toArray();
        for (const auto &feature : features) {
            if (feature.isObject()) {
                QJsonObject geometry = feature.toObject()["geometry"].toObject();
                if (geometry["type"].toString() == "Polygon") {
                    QJsonArray coordinates = geometry["coordinates"].toArray();
                    for (const auto &ring : coordinates) {
                        QPolygonF polygon;
                        QJsonArray points = ring.toArray();
                        for (const auto &point : points) {
                            QJsonArray coords = point.toArray();
                            double lon = coords[0].toDouble();
                            double lat = coords[1].toDouble();
                            polygon << normalizeCoordinates(lon, lat, width(), height());
                        }
                        painter.drawPolygon(polygon);
                    }
                }
            }
        }
    }
}

QPointF GeoJSONViewer::normalizeCoordinates(double lon, double lat, double width, double height) {
    // Normalize lon/lat to fit in the widget, considering zoom factor and offsets
    double normalizedX = (lon - minLon) / (maxLon - minLon) * width;
    double normalizedY = (lat - minLat) / (maxLat - minLat) * height;

    // Apply zoom factor and offsets
    double centerX = width / 2;
    double centerY = height / 2;
    normalizedX = centerX + (normalizedX - centerX) * zoomFactor + offsetX;
    normalizedY = centerY + (normalizedY - centerY) * zoomFactor + offsetY;

    return QPointF(normalizedX, normalizedY);
}

void GeoJSONViewer::wheelEvent(QWheelEvent *event) {
    // Zoom in or out depending on the wheel direction
    if (event->angleDelta().y() > 0) {
        adjustZoom(true); // Zoom in
    } else {
        adjustZoom(false); // Zoom out
    }
}

void GeoJSONViewer::mousePressEvent(QMouseEvent *event) {
    // Start panning when left mouse button is pressed
    if (event->button() == Qt::LeftButton) {
        isPanning = true;
        lastMousePosition = event->pos();
    }
}

void GeoJSONViewer::mouseMoveEvent(QMouseEvent *event) {
    // Pan the view if the mouse is dragged
    if (isPanning) {
        QPoint currentMousePosition = event->pos();
        int deltaX = currentMousePosition.x() - lastMousePosition.x();
        int deltaY = currentMousePosition.y() - lastMousePosition.y();

        adjustPan(deltaX, deltaY);

        lastMousePosition = currentMousePosition;
    }
}

void GeoJSONViewer::mouseReleaseEvent(QMouseEvent *event) {
    // Stop panning when left mouse button is released
    if (event->button() == Qt::LeftButton) {
        isPanning = false;
    }
}

void GeoJSONViewer::adjustZoom(bool zoomIn) {
    const double zoomStep = 0.1; // Adjust zoom level by 10% per step
    if (zoomIn) {
        zoomFactor += zoomStep;
    } else {
        zoomFactor = std::max(0.1, zoomFactor - zoomStep); // Prevent zooming out too far
    }
    update(); // Redraw the widget
}

void GeoJSONViewer::adjustPan(double deltaX, double deltaY) {
    offsetX += deltaX;
    offsetY += deltaY;
    update(); // Redraw the widget
}


void GeoJSONViewer::setZoomFactor(double zoomFactor) {
    this->zoomFactor = zoomFactor;
    update();  // Redraw the widget with the new zoom factor
}
