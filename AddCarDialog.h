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

class AddCarDialog : public QDialog {
Q_OBJECT

public:
    explicit AddCarDialog(QGraphicsScene *scene, QWidget *parent = nullptr);
    ~AddCarDialog();

signals:
    void carAdded(const QString &initialNodeId, const QString &destinationNodeId, int speed, int frequency);

private slots:
    void onValidate();
    void onCancel();

private:
    void loadNodes();

    QGraphicsScene *scene;
    QComboBox *initialAddressComboBox;
    QComboBox *destinationComboBox;
    QLineEdit *speedLineEdit;
    QLineEdit *frequencyLineEdit;
    QPushButton *validateButton;
    QPushButton *cancelButton;
    QLabel *pathFingerLabel=new QLabel("NO PATH FOUND");
    QPushButton* generatePathButton;
};

#endif // ADDCAR_DIALOG_H
