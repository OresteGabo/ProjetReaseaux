#include "MainWidget.h"
#include "CustomGraphicsView.h"
#include "ConfigManager.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QGuiApplication>
#include "CustomScene.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent) {

    QJsonObject jsonObj= ConfigManager::loadJsonFile();
    QJsonObject boundObj = jsonObj.value("Bound").toObject();
    QJsonObject screenObj = jsonObj.value("MainWindow").toObject();

    int width = screenObj.value("width").toInt();
    int height = screenObj.value("height").toInt();

    // Initialize the scene and view
    auto scene = new CustomScene(width, height);
    graphicsView = new CustomGraphicsView(scene, this);

    // Initialize the debug text area
    debugTextArea = new QTextEdit();
    debugTextArea->setReadOnly(true);

    // Initialize buttons
    clearButton = new QPushButton("Clear");
    changeDataButton = new QPushButton("Change Data");
    addCarButton = new QPushButton("Add Car");

    // Connect button slots
    connect(clearButton, &QPushButton::clicked, this, &MainWidget::clearDebugText);
    connect(changeDataButton, &QPushButton::clicked, this, &MainWidget::changeData);
    connect(addCarButton, &QPushButton::clicked, this, &MainWidget::addCar);

    // Layout for the buttons
    auto buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(clearButton);
    buttonLayout->addWidget(changeDataButton);

    auto buttonsLayout = new QVBoxLayout;
    buttonsLayout->addStretch(1);  // Add spacer above buttons to push them down
    buttonsLayout->addLayout(buttonLayout);
    buttonsLayout->addWidget(addCarButton);

    // Layout for debug area and buttons
    auto debugLayout = new QVBoxLayout();
    debugLayout->addWidget(debugTextArea, 1);  // Stretch factor set to 1 for text area
    debugLayout->addLayout(buttonsLayout, 0);  // Stretch factor set to 0 for buttons layout

    // Main layout
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(graphicsView, 3);  // 3:7 ratio for graphicsView vs debug area
    mainLayout->addLayout(debugLayout, 1);   // Adjust ratios as needed

    setLayout(mainLayout);
    setFixedSize(width-50,height-100);
}

void MainWidget::clearDebugText() {
    debugTextArea->clear();
}

void MainWidget::changeData() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open OSM File", "", "OSM Files (*.osm)");
    if (!filePath.isEmpty()) {
        // Implement database loading logic here
        debugTextArea->append("Loaded file: " + filePath);
    }
}

void MainWidget::addCar() {
    // Open a custom dialog to add a car
    debugTextArea->append("Add Car button clicked");
    // Create and show the dialog here
}
