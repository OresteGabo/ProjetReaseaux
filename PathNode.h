//
// Created by oreste on 29/10/24.
//

#ifndef GRAPHICSINQT_PATHNODE_H
#define GRAPHICSINQT_PATHNODE_H

#include <QPlainTextEdit>
#include "Node.h"

//Chainon d'une liste chainéee Path
class PathNode {
    friend class Car;
    friend class Path;
public:
    PathNode(Node* node,PathNode* next=nullptr);
    const Node* getNode() const;
    void logMessage(const QString &message, QPlainTextEdit *debugOutput )const;
private:
    Node* node;
    PathNode* next;
};


#endif //GRAPHICSINQT_PATHNODE_H
