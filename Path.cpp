//
// Created by oreste on 29/10/24.
//

#include "Path.h"
Path::Path(PathNode* head):head(head){}
Path* Path::addNode(PathNode* node){
    PathNode* s=head;

    if(head==nullptr){
        head=node;
        return this;
    }
    while(s->next!=nullptr){
        s=s->next;
    }
    s->next=node;
    return this;

}
Path* Path::addNode( Node* a){
    auto s=head;
    auto node=new PathNode(a);

    if(head==nullptr){
        head=node;
        return this;
    }
    while(s->next!=nullptr){
        s=s->next;
    }
    s->next=node;
    return this;
}
PathNode* Path::getFinalPath()const{
    auto s=head;
    if(head==nullptr){
        return nullptr;
    }
    while(s->next!= nullptr){
        s=s->next;
    }
    return s;
}
int Path::getSize()const{
    int counter=0;
    PathNode* s=head;
    while(s!=nullptr){
        counter++;
        s=s->next;
    }
    return counter;
}
PathNode* Path::getHead()const{
    return head;
}
void Path::draw(CustomScene* scene)  {
    if (!head) return;

    PathNode *current = head;
    while (current && current->next) {
        qDebug()<<current->getNode()->toPoint();
        auto startNode = current->getNode();
        auto endNode = current->next->getNode();

        // Convert latitude/longitude to scene coordinates
        QPointF startPoint = CustomScene::latLonToXY(startNode->lat, startNode->lon);
        QPointF endPoint = CustomScene::latLonToXY(endNode->lat, endNode->lon);

        // Draw the line on the scene
        scene->addLine(QLineF(startPoint, endPoint), QPen(Qt::red, 2));
        current = current->next;
    }
}

void Path::logMessage(const QString &message, QPlainTextEdit *debugOutput) const {
    debugOutput->appendPlainText(message);
}

Path::~Path() {

}
