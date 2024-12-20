#include "MainWidget.h"

//#include "DatabaseManager.h"

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
    runButton = new QPushButton("Start", this);

    connect(runButton, &QPushButton::clicked, this, &MainWidget::onRunButtonClicked);

    changeDataButton = new QPushButton("Change Data");
    addCarButton = new QPushButton("Add Car");

    displayInfo = new QPushButton("Display infor");
    connect(displayInfo, &QPushButton::clicked, this, &MainWidget::onDisplayInfo);




    // Connect button slots
    connect(clearButton, &QPushButton::clicked, this, &MainWidget::clearDebugText);
    //connect(runButton, &QPushButton::clicked, this, &MainWidget::onRun);
    connect(changeDataButton, &QPushButton::clicked, this, &MainWidget::changeData);
    connect(addCarButton, &QPushButton::clicked, this, &MainWidget::addCarDialog);

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

    debugLayout->addWidget(runButton);
    debugLayout->addWidget(displayInfo);
    debugLayout->addLayout(buttonsLayout, 0);  // Stretch factor set to 0 for buttons layout

    // Main layout
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(graphicsView, 3);  // 3:7 ratio for graphicsView vs debug area
    mainLayout->addLayout(debugLayout, 1);   // Adjust ratios as needed







    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &MainWidget::updateAnimation);


    setLayout(mainLayout);
    setFixedSize(width-50,height-100);
}

void MainWidget::clearDebugText() {
    debugTextArea->clear();

}

void MainWidget::onDisplayInfo() {
    for(auto car:cars){
        //debugTextArea->append( "Pos("+QString::number(car->getCurrentPosition().x())+" "+QString::number(car->getCurrentPosition().x())+");");
        debugTextArea->append(""+QString::number(car->currentPosition->x())+","+QString::number(car->currentPosition->y())+";");
        debugTextArea->append("Speed: " + QString::number(car->speed) + "; Frequency: " + QString::number(car->frequence) +";");

    }
}
void MainWidget::changeData() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open OSM File", "", "OSM Files (*.osm)");
    if (!filePath.isEmpty()) {
        // Implement database loading logic here
        debugTextArea->append("Loaded file: " + filePath);
    }
}

void MainWidget::addCarDialog() {
    auto dialog = new AddCarDialog(graphicsView->scene());

    // Connect the signal from AddCarDialog with the selected node IDs
    connect(dialog, &AddCarDialog::carAdded, [this]( Path* path, int speed, int frequency) {
        // Create a new Car with the generated Path
        QString carId = QString::number(cars.size() + 1);
        auto newCar = new Car(carId, path, graphicsView->scene());
        newCar->nextDestinationNode=newCar->path->head;
        if(newCar->nextDestinationNode->next!=nullptr){
            newCar->nextDestinationNode=newCar->nextDestinationNode->next;
        }

        newCar->display(debugTextArea);
        // Add the new Car to the list of cars
        cars.push_back(newCar);
    });

    dialog->exec();
}

void MainWidget::addCar(const QString& initialNodeId, const QString& destinationNodeId) {
    // Create adjacency list from the database
    AdjacencyList adjList = DatabaseManager::buildAdjacencyList();
    Path *pathFinder=new Path();

    // Generate the path
    if (pathFinder->generatePath(initialNodeId, destinationNodeId, adjList)) {
        pathFinder->printPath();

        // Create a new car with the generated path
        QString carId = QString::number(cars.size() + 1);
        Car* newCar = new Car(carId, pathFinder, graphicsView->scene());
        cars.push_back(newCar);
    } else {
        qDebug() << "Failed to generate path for the car.";
    }
}
/*
void MainWidget::addCar() {
    // Create adjacency list from the database
    AdjacencyList adjList = DatabaseManager::buildAdjacencyList();

    // Get random nodes from the adjacency list
    QStringList nodes = adjList.keys();
    if (nodes.size() < 2) {
        qDebug() << "Not enough nodes to select random locations.";
        return;
    }

    int initialIndex = QRandomGenerator::global()->bounded(nodes.size());
    int destinationIndex;
    do {
        destinationIndex = QRandomGenerator::global()->bounded(nodes.size());
    } while (initialIndex == destinationIndex);

    QString initialNodeId = nodes[initialIndex];
    QString destinationNodeId = nodes[destinationIndex];

    qDebug() << "Randomly selected nodes:" << initialNodeId << "to" << destinationNodeId;

    // Use the overloaded addCar function with the selected nodes
    addCar(initialNodeId, destinationNodeId);
}
*/
void MainWidget::onRunButtonClicked() {
    /*debugTextArea->append("Starting car movement...");
    movementTimer->start(50); // Adjust the interval as needed
    for(auto car:cars){
        car->moveAlongPath();
    }*/
    if (animationTimer->isActive()) {
        // If the animation is active, stop it
        animationTimer->stop();
        runButton->setText("Lancer Simulation");
        //logMessage("\t>Simulation arretée");
    } else {
        // If the animation is not active, start it
        animationTimer->start(16);  // Update approximately every 16 milliseconds (60 frames per second)
        runButton->setText("Stop Simulation");
        //logMessage("\t>Simulation lancée");
    }
}
void MainWidget::updateAnimation() {
    // Update the animation progress based on the elapsed time
    qreal elapsedTime = animationTimer->interval() / qreal(animationDuration);

    updateCarPositions(elapsedTime);

    if (animationTimer->isActive()) {
        update();
    } else {
        debugTextArea->setText("Lancer Simulation");
    }
}


/*
void MainWidget::toggleSimulation() {
    if (animationTimer->isActive()) {
        // If the animation is active, stop it
        animationTimer->stop();
        debugTextArea->setText("Lancer Simulation");
        //logMessage("\t>Simulation arretée");
    } else {
        // If the animation is not active, start it
        animationTimer->start(16);  // Update approximately every 16 milliseconds (60 frames per second)
        debugTextArea->setText("Stop Simulation");
        //logMessage("\t>Simulation lancée");
    }
}
*/

//From App class
void MainWidget::updateCarPositions(qreal elapsedTime) {
    for (Car* car : cars) {
        car->updatePosition(elapsedTime,cars);
    }
    updateConnectedCars();
}

void MainWidget::updateConnectedCars() {
    for(Car* car:cars){
        car->updateConnectedCars(cars);
        //qDebug()<<"Connected cars size is "<<car->getConnectedCars().size();
    }
}
