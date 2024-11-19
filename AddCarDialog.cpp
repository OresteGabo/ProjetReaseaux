#include "AddCarDialog.h"
#include "DatabaseManager.h"
#include "Car.h"
#include <QLinearGradient>
AddCarDialog::AddCarDialog(QGraphicsScene *scene, QWidget *parent)
        : QDialog(parent), scene(scene) {

    // Initialize ComboBoxes
    initialAddressComboBox = new QComboBox(this);
    destinationComboBox = new QComboBox(this);


    auto drivableWays=DatabaseManager::getDrivableWaysIds();

    auto adjList = DatabaseManager::buildAdjacencyList();
    auto nodesOfWaysWithName=DatabaseManager::getDrivableWaysNodesId();

// Print the adjacency list
    for (QString node:nodesOfWaysWithName){
        QString streetName=DatabaseManager::getWayNameForNode(node);

        initialAddressComboBox->addItem(node+": "+streetName,node);
        destinationComboBox->addItem(node+": "+streetName,node);

        //qDebug() << "Node" << node << "is connected to:" << neighbors;
    }



    for(auto i=0;i<drivableWays.size();i++){
        initialAddressComboBox->addItem(drivableWays[i],drivableWays[i]);
        destinationComboBox->addItem(drivableWays[i],drivableWays[i]);
    }

    // Initialize input fields
    speedLineEdit = new QLineEdit(this);
    frequencyLineEdit = new QLineEdit(this);
    speedLineEdit->setValidator(new QIntValidator(10, 100, this));
    frequencyLineEdit->setValidator(new QIntValidator(10, 20, this));

    // Initialize buttons
    generatePathButton = new QPushButton("Generate Path", this);
    validateButton = new QPushButton("Valider", this);
    cancelButton = new QPushButton("Annuler", this);

    // Initialize pathInfo
    pathInfo = new QTextEdit(this);
    pathInfo->setReadOnly(true);

    // Layout setup
    auto layout = new QVBoxLayout(this);

    auto initialAdressLayout = new QVBoxLayout(this);
    initialAdressLayout->addWidget(new QLabel("Initial Address:", this));
    initialAdressLayout->addWidget(initialAddressComboBox);

    auto destinationAdressLayout = new QVBoxLayout(this);
    destinationAdressLayout->addWidget(new QLabel("Destination:", this));
    destinationAdressLayout->addWidget(destinationComboBox);

    auto adress = new QHBoxLayout(this);
    adress->addLayout(initialAdressLayout);
    adress->addLayout(destinationAdressLayout);

    auto speedLayout = new QVBoxLayout(this);
    speedLayout->addWidget(new QLabel("Speed (10-100):", this));
    speedLayout->addWidget(speedLineEdit);

    auto frequencyLayout = new QVBoxLayout(this);
    frequencyLayout->addWidget(new QLabel("Frequency (10-20):", this));
    frequencyLayout->addWidget(frequencyLineEdit);

    auto paramLayout = new QHBoxLayout(this);
    paramLayout->addLayout(speedLayout);
    paramLayout->addLayout(frequencyLayout);

    layout->addLayout(adress);
    layout->addLayout(paramLayout);
    layout->addWidget(pathInfo);
    layout->addWidget(generatePathButton);
    layout->addWidget(validateButton);
    layout->addWidget(cancelButton);

    // Connections
    connect(generatePathButton, &QPushButton::clicked, this, &AddCarDialog::onGeneratePath);
    connect(validateButton, &QPushButton::clicked, this, &AddCarDialog::onValidate);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

AddCarDialog::~AddCarDialog() = default;

void AddCarDialog::onGeneratePath() {
    QString initialNodeId = initialAddressComboBox->currentData().toString();
    QString destinationNodeId = destinationComboBox->currentData().toString();

    if (initialNodeId.isEmpty() || destinationNodeId.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please select both initial and destination addresses.");
        return;
    }

    // Build the adjacency list from the database
    AdjacencyList adjList = DatabaseManager::buildAdjacencyList();
    qDebug()<<"Debug text1";
    //Create the Path object
    Path pathFinder;
    qDebug()<<"Debug text2";
    bool pathExists = pathFinder.generatePath(initialNodeId, destinationNodeId, adjList);

    if (pathExists) {
        pathInfo->setText("Path generated successfully:\n" +
                          QString("Initial Node: %1\nDestination Node: %2\nPath Length: %3")
                                  .arg(initialNodeId)
                                  .arg(destinationNodeId)
                                  .arg(pathFinder.size()));
        qDebug() << "Path generated successfully from" << initialNodeId << "to" << destinationNodeId;
    } else {
        pathInfo->setText( "Failed to generate path from " + initialNodeId + " to " +destinationNodeId);
    }
}

void AddCarDialog::onValidate() {
    if (pathInfo->toPlainText().contains("Error")) {
        QMessageBox::warning(this, "Validation Error", "Please generate a valid path before validation.");
        return;
    }

    QString initialNodeId = initialAddressComboBox->currentData().toString();
    QString destinationNodeId = destinationComboBox->currentData().toString();
    Path path=Path();
    path.generatePath(initialNodeId,destinationNodeId,DatabaseManager::buildAdjacencyList());
    int speed = speedLineEdit->text().toInt();
    int frequency = frequencyLineEdit->text().toInt();

    qDebug()<<"Car emit";
    emit carAdded(path, speed, frequency);
    accept();
}

void AddCarDialog::onCancel() {
    reject();
}

