#include "MainWidget.h"
#include "CustomGraphicsView.h"
#include "ConfigManager.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QGuiApplication>
#include "CustomScene.h"
#include "AddCarDialog.h"
#include "Node.h"
#include "Path.h"
#include "djkstra.h"
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
/*
void MainWidget::addCar() {
    auto dialog = new AddCarDialog(graphicsView->scene());

    connect(dialog, &AddCarDialog::carAdded, [this](const QPointF &initial, const QPointF &destination, int speed, int frequency) {
        debugTextArea->append(QString("Car added: From (%1, %2) to (%3, %4) with speed %5 and frequency %6")
                                      .arg(initial.x()).arg(initial.y())
                                      .arg(destination.x()).arg(destination.y())
                                      .arg(speed).arg(frequency));

        // Create a new Car and add it to the vector
        QString initialNodeId = QString::number(initial.x()) + "," + QString::number(initial.y());
        QString destinationNodeId = QString::number(destination.x()) + "," + QString::number(destination.y());

        // Create new car, pass the scene
        auto newCar = new Car(QString::number(qrand()), initialNodeId, destinationNodeId, graphicsView->scene());
        qDebug()<<"Initial node used is "<<initialNodeId << " and destination node used is " <<destinationNodeId;
        cars.push_back(newCar);
    });

    dialog->exec();
}
*/
void MainWidget::addCar() {
    auto dialog = new AddCarDialog(graphicsView->scene());

    // Connect the signal from AddCarDialog with the selected node IDs
    connect(dialog, &AddCarDialog::carAdded, [this](const QString &initialNodeId, const QString &destinationNodeId, int speed, int frequency){
        debugTextArea->append(QString("Car added: From node %1 to node %2 with speed %3 and frequency %4")
                                      .arg(initialNodeId)
                                      .arg(destinationNodeId)
                                      .arg(speed)
                                      .arg(frequency));

        // Create a new Car using the node IDs from the database
        auto newCar = new Car(QString::number(qrand()), initialNodeId, destinationNodeId, graphicsView->scene());
        qDebug() << "Initial node ID used:" << initialNodeId << ", Destination node ID used:" << destinationNodeId;

        cars.push_back(newCar);
    });

    dialog->exec();
}
void MainWidget::generatePath(Node *initialNode, Node *destinationNode) {
    debugTextArea->append("Generating shortest path...");

    // Call the Dijkstra's function
    Path *path = generateShortestPath(initialNode, destinationNode, nodes, adjacencyList);

    if (path) {
        debugTextArea->append("Path generated successfully.");
        paths.push_back(path); // Store the path

        // Draw the path using CustomScene
        path->draw(dynamic_cast<CustomScene *>(graphicsView->scene()));
    } else {
        debugTextArea->append("No path found.");
    }
}