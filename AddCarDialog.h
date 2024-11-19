#ifndef ADDCAR_DIALOG_H
#define ADDCAR_DIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QPointF>
#include <QTextEdit>
#include <QDebug>
#include <QSqlError>
#include <QRandomGenerator>
#include "Path.h"
//#include "AdjacencyList.h"
///#include "DatabaseManager.h"

class AddCarDialog : public QDialog {
Q_OBJECT

public:
    explicit AddCarDialog(QGraphicsScene *scene, QWidget *parent = nullptr);
    ~AddCarDialog();

signals:
    void carAdded(const Path& path, int speed, int frequency);

private slots:
    void onValidate();
    void onCancel();
    void onGeneratePath();

private:


    QGraphicsScene *scene;
    QComboBox *initialAddressComboBox;
    QComboBox *destinationComboBox;
    QLineEdit *speedLineEdit;
    QLineEdit *frequencyLineEdit;
    QPushButton *validateButton;
    QPushButton *cancelButton;
    QTextEdit* pathInfo;
    QPushButton* generatePathButton;
    //DatabaseManager* databaseManager; // Assuming you have access to this in your project
};

#endif // ADDCAR_DIALOG_H
