#ifndef GEOJSONCONTROLS_H
#define GEOJSONCONTROLS_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "geojsonviewer.h"

class GeoJSONControls : public QWidget {
    Q_OBJECT

public:
    explicit GeoJSONControls(GeoJSONViewer *viewer, QWidget *parent = nullptr);

    private slots:
        void zoomIn();
    void zoomOut();


private:
    GeoJSONViewer *viewer;
};

#endif // GEOJSONCONTROLS_H
