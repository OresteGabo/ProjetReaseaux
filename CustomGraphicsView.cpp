//
// Created by oreste on 29/10/24.
//

#include "CustomGraphicsView.h"
#include "ConfigManager.h"
#include <QPoint>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
CustomGraphicsView::CustomGraphicsView(QGraphicsScene* scene,QWidget *parent)
        : QGraphicsView(parent), isDragging(false) {
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);



    ConfigManager configManager("config.json");
    auto height=configManager.getMainWindowSize().height();
    auto width=configManager.getMainWindowSize().width();
    setFixedHeight(height);
    setFixedWidth(width);


    // Create the QTextEdit for the debugger text area
    auto debugTextArea = new QTextEdit();
    debugTextArea->setReadOnly(true);
    debugTextArea->setFixedSize(100, 100);  // Set fixed width and height as needed


    // Add buttons in a layout
    auto buttonContainer = new QWidget();
    auto buttonLayout = new QVBoxLayout();
    auto button1 = new QPushButton("Button 1");
    auto button2 = new QPushButton("Button 2");
    buttonLayout->addWidget(button1);
    buttonLayout->addWidget(button2);
    buttonContainer->setLayout(buttonLayout);

    // Main widget to hold both the text area and buttons
    auto debuggerWidget = new QWidget();
    auto debuggerLayout = new QVBoxLayout(debuggerWidget);
    debuggerLayout->addWidget(debugTextArea);
    debuggerLayout->addWidget(buttonContainer);

    // Use a proxy widget to embed the debugger widget in the graphics view
    QGraphicsProxyWidget *proxyWidget = scene->addWidget(debuggerWidget);

    // Position on the right side of the view
    proxyWidget->setPos( 320, 10);  // Adjust position as needed

    // Ensure the debugger widget doesn’t zoom or move with the scene
    proxyWidget->setFlag(QGraphicsItem::ItemIgnoresTransformations);


    setScene(scene);
    fitInView(scene->sceneRect(), Qt::KeepAspectRatio); // Fit scene into view


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

