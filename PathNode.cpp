//
// Created by oreste on 29/10/24.
//

#include "PathNode.h"
PathNode::PathNode(Node* node,PathNode* n) : node(node), next(n) {
}

const Node* PathNode::getNode() const {
    return node;
}

void PathNode::logMessage(const QString &message, QPlainTextEdit *debugOutput) const {
    debugOutput->appendPlainText(message);
}
