//
// Created by oreste on 29/10/24.
//

#include "CustomGraphicsView.h"
#include "ConfigManager.h"
#include <QPoint>
#include <QPlainTextEdit>

CustomGraphicsView::CustomGraphicsView(QWidget *parent)
        : QGraphicsView(parent), isDragging(false) {
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);

    ConfigManager configManager("config.json");
    int height=configManager.getMainWindowSize().height();
    int width=configManager.getMainWindowSize().width();
    setFixedHeight(height);
    setFixedWidth(width);
    auto debugOutput=new QPlainTextEdit();
    debugOutput->appendPlainText("Hello debugger");


}

void CustomGraphicsView::wheelEvent(QWheelEvent *event) {
    // Zoom in/out
    if (event->angleDelta().y() > 0) {
        scale(1.1, 1.1); // Zoom in
    } else {
        scale(0.9, 0.9); // Zoom out
    }
    event->accept();
}

void CustomGraphicsView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        lastMousePos = event->pos();
        isDragging = true;
        setCursor(Qt::ClosedHandCursor);
    }
    QGraphicsView::mousePressEvent(event);
}

void CustomGraphicsView::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging) {
        // Pan the view
        QPointF delta = event->pos() - lastMousePos;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        lastMousePos = event->pos();
    }
    QGraphicsView::mouseMoveEvent(event);
}


void CustomGraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        unsetCursor();
    }
    QGraphicsView::mouseReleaseEvent(event);
}

