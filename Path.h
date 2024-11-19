#ifndef PATH_H
#define PATH_H

#include "PathNode.h"
#include <QString>
#include <QMap>
#include <QSet>
#include <QQueue>
#include <QDebug>

// Adjacency list type: a map where each node ID maps to a set of connected node IDs
using AdjacencyList = QMap<QString, QSet<QString>>;

class Path {

public:
    Path();
    ~Path();

    // Build the path from startNode to destinationNode using the adjacency list
    bool generatePath(const QString& startNode, const QString& destinationNode, const AdjacencyList& adjList);

    // Add a node to the end of the path
    void append(const QString& nodeId);

    // Print the path for debugging
    void printPath() const;

    // Get the head of the path (start node)
    auto getHead() const { return head; }
    auto getTail()const { return tail; }
    PathNode* getNodeAt(int index) const;


    int size()const;
    PathNode* nextDestinationNode;

private:
    PathNode* head;   // Pointer to the head of the linked list
    PathNode* tail;   // Pointer to the tail of the linked list

    // Helper function to clear the linked list
    void clear();
};

#endif // PATH_H
