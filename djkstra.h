//
// Created by oreste on 12/11/24.
//

#ifndef GRAPHICSINQT_DJKSTRA_H
#define GRAPHICSINQT_DJKSTRA_H
#include <QMap>
#include <QSet>
//#include <QPriorityQueue>
#include <queue>
#include "Path.h"
#include "Node.h"
using namespace std;

struct DijkstraInfo {
    double distance;
    Node* predecessor;
    bool visited;

    DijkstraInfo() : distance(std::numeric_limits<double>::infinity()), predecessor(nullptr), visited(false) {}
};



Path* generateShortestPath(Node* initialNode, Node* destinationNode, const QMap<QString, Node*>& nodes, const QMap<QString, QList<Node*>>& adjacencyList) {
    if (!initialNode || !destinationNode) return nullptr;

    // Min-heap priority queue: pairs of (distance, node)
    using NodePair = std::pair<double, Node*>;
    auto compare = [](NodePair a, NodePair b) { return a.first > b.first; };
    std::priority_queue<NodePair, std::vector<NodePair>, decltype(compare)> priorityQueue(compare);

    // Dijkstra information map
    QMap<Node*, DijkstraInfo> dijkstraMap;

    // Initialize the starting node
    dijkstraMap[initialNode].distance = 0;
    priorityQueue.emplace(0.0, initialNode);

    // Main loop of Dijkstra’s algorithm
    while (!priorityQueue.empty()) {
        auto [currentDistance, currentNode] = priorityQueue.top();
        priorityQueue.pop();

        // Skip visited nodes
        if (dijkstraMap[currentNode].visited) continue;
        dijkstraMap[currentNode].visited = true;

        // Check if we reached the destination
        if (currentNode == destinationNode) break;

        // Explore neighbors
        const auto& neighbors = adjacencyList[currentNode->id];
        for (auto neighbor : neighbors) {
            double edgeWeight = currentNode->distanceTo(neighbor); // Assume you have this method

            double newDistance = currentDistance + edgeWeight;
            if (newDistance < dijkstraMap[neighbor].distance) {
                dijkstraMap[neighbor].distance = newDistance;
                dijkstraMap[neighbor].predecessor = currentNode;
                priorityQueue.emplace(newDistance, neighbor);
            }
        }
    }

    // If no path is found, return nullptr
    if (dijkstraMap[destinationNode].predecessor == nullptr) return nullptr;

    // Reconstruct the path using the predecessors
    PathNode* pathHead = nullptr;
    Node* currentNode = destinationNode;
    while (currentNode != nullptr) {
        pathHead = new PathNode(currentNode, pathHead);
        currentNode = dijkstraMap[currentNode].predecessor;
    }

    return new Path(pathHead);
}

#endif //GRAPHICSINQT_DJKSTRA_H
