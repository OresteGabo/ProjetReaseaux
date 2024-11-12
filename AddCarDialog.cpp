#include "AddCarDialog.h"
#include <QLinearGradient>
AddCarDialog::AddCarDialog(QGraphicsScene *scene, QWidget *parent)
        : QDialog(parent), scene(scene) {



    // Initialize ComboBoxes
    initialAddressComboBox = new QComboBox(this);
    destinationComboBox = new QComboBox(this);

    // Load important nodes into the ComboBoxes
    QSqlQuery query("SELECT nodes.id, address.housenumber, address.street, address.postcode, address.city "
                    "FROM nodes "
                    "JOIN address ON nodes.id = address.node_id "
                    "WHERE nodes.isImportant = 1");

    if (!query.exec()) {
        qDebug() << "Failed to execute query:" << query.lastError().text();
    }

    while (query.next()) {
        QString nodeId = query.value(0).toString();
        QString address = QString("%1 %2, %3 %4")
                .arg(query.value(1).toString()) // house number
                .arg(query.value(2).toString()) // street
                .arg(query.value(3).toString()) // postal code
                .arg(query.value(4).toString()); // city
        initialAddressComboBox->addItem(address, nodeId);
        destinationComboBox->addItem(address, nodeId);
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

    // Layout setup
    QVBoxLayout *layout = new QVBoxLayout(this);

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

    layout->addWidget(generatePathButton);
    layout->addWidget(validateButton);
    layout->addWidget(cancelButton);

    // Connections
    connect(validateButton, &QPushButton::clicked, this, &AddCarDialog::onValidate);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

}

AddCarDialog::~AddCarDialog() = default;

void AddCarDialog::loadNodes() {
    QSqlQuery query("SELECT id, name FROM nodes");
    if (query.exec()) {
        while (query.next()) {
            QString nodeId = query.value(0).toString();
            QString nodeName = query.value(1).toString();
            initialAddressComboBox->addItem(nodeName, nodeId);
            destinationComboBox->addItem(nodeName, nodeId);
        }
    } else {
        QMessageBox::warning(this, "Database Error", "Failed to load nodes from the database.");
    }
}


void AddCarDialog::onValidate() {
    QString initialAddress = initialAddressComboBox->currentText();
    QString destination = destinationComboBox->currentText();
    int speed = speedLineEdit->text().toInt();
    int frequency = frequencyLineEdit->text().toInt();

    if (initialAddress.isEmpty() || destination.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please select both initial and destination addresses.");
        return;
    }
    if (speed <= 0) {
        QMessageBox::warning(this, "Input Error", "Please enter a valid speed.");
        return;
    }
    if (frequency <= 0) {
        QMessageBox::warning(this, "Input Error", "Please enter a valid frequency.");
        return;
    }

    QString initialNodeId = initialAddressComboBox->currentData().toString();
    QString destinationNodeId = destinationComboBox->currentData().toString();


    emit carAdded(initialNodeId, destinationNodeId, speed, frequency);
    accept();
}

void AddCarDialog::onCancel() {
    reject();
}
