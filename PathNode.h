#ifndef PATHNODE_H
#define PATHNODE_H

#include <QString>
#include "Node.h"
#include "DatabaseManager.h"

//#include "DatabaseManager.h"
class PathNode {
public:
    QString nodeId;      // The ID of the node
    PathNode* next;      // Pointer to the next node in the path

    // Constructor
    PathNode(const QString& nodeId, PathNode* nextNode = nullptr)
            : nodeId(nodeId), next(nextNode) {}
    QPointF getPosition()const{
        //return {};
        return DatabaseManager::getPositionByNodeId(nodeId);
    }
};

#endif // PATHNODE_H
