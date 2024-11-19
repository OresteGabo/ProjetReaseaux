#include <QTextEdit>
#include "Path.h"

Path::Path() : head(nullptr), tail(nullptr) {
    //nextDestinationNode=head->next;
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
        tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
}

// Function to generate a path using BFS
bool Path::generatePath(const QString& startNode, const QString& destinationNode, const AdjacencyList& adjList) {
    clear(); // Clear any existing path
    qDebug()<<"Generate path called";
    QMap<QString, QString> previousNode; // To track the path
    QQueue<QString> queue;
    QSet<QString> visited;

    // Initialize BFS
    queue.enqueue(startNode);
    visited.insert(startNode);

    bool pathFound = false;
    qDebug()<<"Generate path called before the loop";
    while (!queue.isEmpty()) {
        QString currentNode = queue.dequeue();

        // Check if we reached the destination
        if (currentNode == destinationNode) {
            pathFound = true;
            break;
        }

        // Explore neighbors
        for (const QString& neighbor : adjList[currentNode]) {
            if (!visited.contains(neighbor)) {
                queue.enqueue(neighbor);
                visited.insert(neighbor);
                previousNode[neighbor] = currentNode;
            }
        }
    }
    qDebug()<<"After the loop";
    // Reconstruct the path if it was found
    if (pathFound) {
        QString currentNode = destinationNode;
        while (currentNode != startNode) {
            append(currentNode);
            currentNode = previousNode[currentNode];
        }
        append(startNode); // Add the start node at the end
        return true;
    } else {
        qDebug() << "No path found between" << startNode << "and" << destinationNode;
        return false;
    }
}

// Function to print the path for debugging
void Path::printPath() const {
    auto current = head;
    qDebug()<<"Path:";
    while (current != nullptr) {
        qDebug()<< current->nodeId;
        //if()
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
