#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QFileDialog>
#include <QRandomGenerator>
#include <QTimer>
#include <QSlider>
#include <QPlainTextEdit>
#include "CustomGraphicsView.h"
#include "Car.h"

#include "CustomGraphicsView.h"
#include "ConfigManager.h"
#include <QMessageBox>
#include <QGuiApplication>
#include "CustomScene.h"
#include "AddCarDialog.h"
#include "Node.h"
#include "Path.h"
#include "DatabaseManager.h"

class MainWidget : public QWidget {
Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    void addCar();
    void addCar(const QString& initialNodeId, const QString& destinationNodeId);
    void updateAnimation();
    void toggleSimulation();
    void updateConnectedCars();
    void onDisplayInfo();

    //From App class
    void updateCarPositions(qreal elapsedTime);
signals:
    emit  void carAdded(const Path&, int speed, int frequency);


private slots:
    void clearDebugText();
    void changeData();
    void addCarDialog();

public slots:
    void onRunButtonClicked();


private:
    QTextEdit *debugTextArea;
    CustomGraphicsView *graphicsView;
    QPushButton *clearButton;
    QPushButton *changeDataButton;
    QPushButton *addCarButton;
    QVector<Car*> cars;
    QPushButton *displayInfo;

    int animationDuration;
    QTimer* animationTimer;

    QPushButton* runButton;
    QTimer* movementTimer; // Timer for car movement



};

#endif // MAINWIDGET_H
