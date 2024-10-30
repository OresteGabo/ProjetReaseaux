//
// Created by oreste on 29/10/24.
//

#ifndef GRAPHICSINQT_PATH_H
#define GRAPHICSINQT_PATH_H


#include "PathNode.h"

class Path {

public:
    explicit Path(PathNode* h=nullptr);
    ~Path();
    Path* addNode(PathNode*);
    Path* addNode(Node*);
    [[nodiscard]] PathNode* getFinalPath()const;
    [[nodiscard]] int getSize()const;
    [[nodiscard]] PathNode* getHead()const;
    void draw(QPainter& painter) const;
    void logMessage(const QString &message, QPlainTextEdit *debugOutput )const;
private:
    PathNode* head;
};


#endif //GRAPHICSINQT_PATH_H
