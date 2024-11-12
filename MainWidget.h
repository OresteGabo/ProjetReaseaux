#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QFileDialog>
#include "CustomGraphicsView.h"
#include "Car.h"
#include "Node.h"
#include "Path.h"

class MainWidget : public QWidget {
Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
void generatePath(Node* initialNode, Node* destinationNode);
signals:
    emit  void carAdded(const QString &initial, const QString &destination, int speed, int frequency);

private slots:
    void clearDebugText();
    void changeData();
    void addCar();

private:
    QTextEdit *debugTextArea;
    CustomGraphicsView *graphicsView;
    QPushButton *clearButton;
    QPushButton *changeDataButton;
    QPushButton *addCarButton;
    std::vector<Car*> cars;

    QMap<QString, Node*> nodes;              // Stores all nodes by ID
    QMap<QString, QList<Node*>> adjacencyList; // Adjacency list for graph
    QVector<Path*> paths;
};

#endif // MAINWIDGET_H
