#ifndef PATHNODE_H
#define PATHNODE_H

#include <QString>
#include "Node.h"
class PathNode {
public:
    QString nodeId;      // The ID of the node
    PathNode* next;      // Pointer to the next node in the path

    // Constructor
    PathNode(const QString& nodeId, PathNode* nextNode = nullptr)
            : nodeId(nodeId), next(nextNode) {}
};

#endif // PATHNODE_H
