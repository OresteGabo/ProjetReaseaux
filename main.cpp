#include <QApplication>
#include <QtSql/QSqlDatabase>
#include <QScreen>
#include "CustomScene.h"
#include "ConfigManager.h"
#include "CustomGraphicsView.h"
#include "DatabaseManager.h"
#include <iostream>
using namespace std;


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    DatabaseManager dbMngr=DatabaseManager("OSMData", "oreste", "Muhirehonore@1*","map.osm");

    ConfigManager configManager("config.json");  // Create a ConfigManager for config.json

    auto screen = QGuiApplication::primaryScreen();
    auto screenGeometry = screen->geometry();
    cout<<" Height:"<<screenGeometry.height()<<" , Width:"<< screenGeometry.width()<<endl;
    configManager.setMainWindowSize(screenGeometry);

    int width=configManager.getMainWindowSize().width();
    int height=configManager.getMainWindowSize().height();

    // Create the scene
    auto scene = new CustomScene(width, height);

    // Create view using CustomGraphicsView
    auto view = new CustomGraphicsView();


    view->setScene(scene);
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio); // Fit scene into view
    view->show();

    return app.exec();



}
