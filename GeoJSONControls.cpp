#include "geojsoncontrols.h"
#include <QPushButton>
#include <QVBoxLayout>

GeoJSONControls::GeoJSONControls(GeoJSONViewer *viewer, QWidget *parent)
    : QWidget(parent), viewer(viewer) {

    // Create buttons
    QPushButton *zoomInButton = new QPushButton("Zoom In");
    QPushButton *zoomOutButton = new QPushButton("Zoom Out");

    // Connect buttons to actions
    connect(zoomInButton, &QPushButton::clicked, this, &GeoJSONControls::zoomIn);
    connect(zoomOutButton, &QPushButton::clicked, this, &GeoJSONControls::zoomOut);

    // Layout for buttons - QVBoxLayout will stack the buttons vertically
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(zoomInButton);
    layout->addWidget(zoomOutButton);
    layout->addStretch(1); // Push the buttons to the top
    layout->setSpacing(5); // Add space between buttons

    // Set the layout for the control panel
    setLayout(layout);
}

void GeoJSONControls::zoomIn() {
    viewer->adjustZoom(true);  // This will zoom in more if the button is clicked
}

void GeoJSONControls::zoomOut() {
    viewer->adjustZoom(false);  // This will zoom out if the button is clicked
}
