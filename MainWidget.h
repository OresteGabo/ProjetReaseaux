#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QFileDialog>
#include "CustomGraphicsView.h"

class MainWidget : public QWidget {
Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);

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
};

#endif // MAINWIDGET_H
