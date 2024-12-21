#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QPushButton>
#include "geojsonviewer.h"
#include "geojsoncontrols.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create the main window container
    QWidget window;
    window.setWindowTitle("GeoJSON Viewer");

    // Create the main viewer (GeoJSONViewer) with default zoom factor 100
    GeoJSONViewer *viewer = new GeoJSONViewer(nullptr, 900000.0);  // Pass 100 for automatic zoom-in

    // Load GeoJSON file
    viewer->loadGeoJSON("map.geojson");

    // Create controls for zoom and pan
    GeoJSONControls *controls = new GeoJSONControls(viewer);

    // Use a QSplitter to divide the space
    QSplitter *splitter = new QSplitter(Qt::Horizontal, &window);

    // Add viewer and controls to the splitter
    splitter->addWidget(viewer);
    splitter->addWidget(controls);

    // Set proportional sizes: 80% for the viewer, 20% for controls
    splitter->setStretchFactor(0, 8); // Viewer gets 80% (8 parts out of 10)
    splitter->setStretchFactor(1, 2); // Controls get 20% (2 parts out of 10)

    // Make the window maximized
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(splitter);
    window.setLayout(mainLayout);

    window.showMaximized();

    return app.exec();
}
