//
// Created by oreste on 29/10/24.
//

#ifndef GRAPHICSINQT_CONFIGMANAGER_H
#define GRAPHICSINQT_CONFIGMANAGER_H
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRect>
class ConfigManager {
    QJsonObject jsonObj;
    QString configFileName;

public:
    ConfigManager(const QString &configFileName) :configFileName(configFileName){
        QFile file(configFileName);
        // If file doesn't exist, create a new file
        if (!file.exists()) {
            if (file.open(QIODevice::ReadWrite)) {
                file.close();
            }
        }

        // Read file
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray fileData = file.readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
            jsonObj = jsonDoc.object();
            file.close();
        }
    }

    void setMainWindowSize(const QRect &screenGeometry) {
        QJsonObject mainWindowObj;
        mainWindowObj["width"] = screenGeometry.width();
        mainWindowObj["height"] = screenGeometry.height();
        jsonObj["MainWindow"] = mainWindowObj;
    }

    QRect getMainWindowSize() {
        if (jsonObj.contains("MainWindow")) {
            QJsonObject mainWindowObj = jsonObj["MainWindow"].toObject();
            int width = mainWindowObj["width"].toInt();
            int height = mainWindowObj["height"].toInt();
            return {0, 0, width-100, height-50};
        }
        return {0, 0, 800, 600};  // return default size
    }

    void setBounds(double minLat, double maxLat, double minLon, double maxLon) {
        QJsonObject boundsObj;
        boundsObj["minLat"] = minLat;
        boundsObj["maxLat"] = maxLat;
        boundsObj["minLon"] = minLon;
        boundsObj["maxLon"] = maxLon;
        jsonObj["Bound"] = boundsObj;

        writeToFile();
    }

private:
    bool writeToFile() {
        QFile file(configFileName);
        if (file.open(QIODevice::ReadWrite)) {
            QJsonDocument jsonDoc(jsonObj);
            file.resize(0); // Clear the file
            file.write(jsonDoc.toJson());
            file.close();
            return true;
        }
        return false;
    }
};


#endif //GRAPHICSINQT_CONFIGMANAGER_H
