#ifndef CIRCUITDIAGRAMVIEWER_H
#define CIRCUITDIAGRAMVIEWER_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QTimer>
#include "electroniccomponent.h"
#include "wire.h"
#include "previewwire.h"


class CircuitDiagramViewer : public QGraphicsView
{
Q_OBJECT

public:
    class Scene;

    CircuitDiagramViewer(QSize viewerSize, QWidget* parent = nullptr);
    ~CircuitDiagramViewer();
    QRectF getWorkplaceRect();
    void placeComponent(ElectronicComponent* device, QPoint pos);
    void placeWire(Wire* wire);
    void clear();
    QImage toImage();

public slots:
    void zoomIn();
    void zoomOut();

private:
    Scene* _scene;
    qreal currentScaleFactor;

protected:
    void wheelEvent(QWheelEvent* event);
};


class CircuitDiagramViewer::Scene : public QGraphicsScene
{
Q_OBJECT

public:
    Scene();
    ~Scene();
    QGraphicsRectItem* workplace();

public slots:
    void changeWireDrawingState(QPointF vertexPos);

private:
    bool wireDrawingInProgress;
    QGraphicsRectItem* _workplace;
    // used in proccess of drawing a new wire
    Wire* newWire;
    PreviewWire* previewWire;

    void copyPreviewWireToNewWire();

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
};

#endif // CIRCUITDIAGRAMVIEWER_H
