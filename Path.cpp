#include <QTextEdit>
#include "Path.h"
#include "DatabaseManager.h"


Path::Path() : head(nullptr), tail(nullptr) {
}

Path::~Path() {
    clear();
}

// Helper function to clear the linked list
void Path::clear() {
    auto current = head;
    while (current != nullptr) {
        auto toDelete = current;
        current = current->next;
        delete toDelete;
    }
    head = nullptr;
    tail = nullptr;
}

// Function to append a node to the end of the path
void Path::append(const QString& nodeId) {
    PathNode* newNode = new PathNode(nodeId);
    if (tail == nullptr) {
        head = newNode;
        //nextDestinationNode = newNode; // Update `nextDestinationNode`
    } else {
        tail->next = newNode;
    }
    tail = newNode;
}

// Function to generate a path using BFS
bool Path::generatePath(const QString& startNode, const QString& destinationNode, const AdjacencyList& adjList) {
    if (startNode == destinationNode) {
        append(startNode);
        return true;
    }

    // Map to store the parent of each node in the shortest path
    QMap<QString, QString> parentMap;
    QSet<QString> visited;
    QQueue<QString> queue;

    queue.enqueue(startNode);
    visited.insert(startNode);

    bool pathFound = false;

    // Perform Breadth-First Search (BFS) to find the shortest path
    while (!queue.isEmpty()) {
        QString currentNode = queue.dequeue();

        // Check if we have reached the destination
        if (currentNode == destinationNode) {
            pathFound = true;
            break;
        }

        // Get the neighbors of the current node
        if (!adjList.contains(currentNode)) {
            continue;
        }

        for (const QString& neighbor : adjList[currentNode]) {
            if (!visited.contains(neighbor)) {
                queue.enqueue(neighbor);
                visited.insert(neighbor);
                parentMap[neighbor] = currentNode;
            }
        }
    }

    if (!pathFound) {
        qDebug() << "Path not found between" << startNode << "and" << destinationNode;
        return false;
    }

    // Backtrack from destinationNode to startNode using parentMap
    QString currentNode = destinationNode;
    QVector<QString> pathStack;

    while (currentNode != startNode) {
        pathStack.push_back(currentNode);
        currentNode = parentMap[currentNode];
    }
    pathStack.push_back(startNode);

    // Add the nodes to the Path in reverse order (from startNode to destinationNode)
    for (auto it = pathStack.rbegin(); it != pathStack.rend(); ++it) {
        append(*it);
    }

    //nextDestinationNode = head; // Initialize the next destination node
    return true;
}
// Function to print the path for debugging
void Path::printPath() const {
    auto current = head;
    qDebug()<<"Path:";
    while (current != nullptr) {
        qDebug()<< current->nodeId;
        current = current->next;
    }
}

int Path::size() const {
    auto current=head;
    int counter=0;
    while(current!= nullptr){
        counter++;
        current = current->next;
    }
    return counter;
}

PathNode* Path::getNodeAt(int index) const {
    if (index < 0) {
        qDebug() << "Invalid index:" << index;
        return nullptr;
    }

    PathNode* current = head;
    int currentIndex = 0;

    // Traverse the list to find the node at the specified index
    while (current != nullptr && currentIndex < index) {
        current = current->next;
        currentIndex++;
    }

    // If the node is found, return it; otherwise, return nullptr
    if (currentIndex == index) {
        return current;
    } else {
        qDebug() << "Index out of bounds:" << index;
        return nullptr;
    }
}
