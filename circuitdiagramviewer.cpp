#include "circuitdiagramviewer.h"
#include <QVector>
#include <QKeyEvent>
#include <QDebug> // TODO: remove
#include "parameters.h"
#include "wire.h"


CircuitDiagramViewer::Scene::Scene() : wireDrawingInProgress(false), \
                                       previewWire(new PreviewWire)
{
    this->addItem(previewWire);
    this->setBackgroundBrush(QBrush(Qt::gray));
    // draw border and grid
    this->_workplace = this->addRect(0, 0, \
                                     workplaceSize.width(), workplaceSize.height(), \
                                     QPen(Qt::black), QBrush(Qt::white, Qt::Dense1Pattern));
}

QGraphicsRectItem* CircuitDiagramViewer::Scene::workplace()
{
    return this->_workplace;
}

CircuitDiagramViewer::Scene::~Scene()
{
    /*if(this->newWire)
    {
        delete this->newWire;
    }
    delete this->previewWire;*/
}

void CircuitDiagramViewer::clear()
{
    delete this->_scene;
    this->setScene(this->_scene = new Scene);
}

void CircuitDiagramViewer::Scene::copyPreviewWireToNewWire()
{
    switch(this->previewWire->direction())
    {
        case PreviewWire::Direction::Horizontal:
        case PreviewWire::Direction::Vertical:
            this->newWire->addVertex(this->previewWire->endPoint());
            break;

        case PreviewWire::Direction::HorizontalToVertical:
        case PreviewWire::Direction::VerticalToHorizontal:
            this->newWire->addVertex(this->previewWire->vertexPoint());
            this->newWire->addVertex(this->previewWire->endPoint());
            break;
    }
}

void CircuitDiagramViewer::Scene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if(event->button() == Qt::MouseButton::LeftButton)
    {
        this->changeWireDrawingState(event->scenePos());
    }
}

void CircuitDiagramViewer::Scene::changeWireDrawingState(QPointF vertexPos)
{
    if(!this->wireDrawingInProgress)
    {
        this->newWire = new Wire;
        this->newWire->addVertex(vertexPos);
        this->addItem(this->newWire);

        this->previewWire->setStartPoint(vertexPos);
        this->previewWire->setEndPoint(vertexPos);
        this->previewWire->show();
    }
    else
    {
        this->previewWire->setEndPoint(vertexPos); // snap to junction if clicked on junction
        this->copyPreviewWireToNewWire();
        this->previewWire->hide();
    }

    this->newWire->addJunction(this->newWire->vertexes().last());

    this->wireDrawingInProgress = !this->wireDrawingInProgress;
}

void CircuitDiagramViewer::Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseReleaseEvent(event);

    if(event->button() == Qt::MouseButton::LeftButton)
    {
        if(this->wireDrawingInProgress && \
           this->newWire->vertexes().last() != event->scenePos()) // compare due to inability of qt to distinguish single and double clicks
        {
            this->copyPreviewWireToNewWire();
        }
    }
}

void CircuitDiagramViewer::Scene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if(this->wireDrawingInProgress)
    {
        this->previewWire->setStartPoint(this->newWire->vertexes().last());
        this->previewWire->setEndPoint(event->scenePos());
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void CircuitDiagramViewer::Scene::keyPressEvent(QKeyEvent* event)
{
    if(this->wireDrawingInProgress)
    {
        if(event->key() == Qt::Key_Escape)
        {
            this->wireDrawingInProgress = false;
            this->previewWire->hide();

            if(this->newWire->vertexes().length() <= 2)
            {
                delete this->newWire;
            }
            else
            {
                newWire->addJunction(newWire->vertexes().last());
            }
        }
        else if(event->key() == Qt::Key_Shift)
        {
            this->previewWire->invertDirection();
            this->previewWire->update();
        }
    }
}

CircuitDiagramViewer::CircuitDiagramViewer(QSize viewerSize, QWidget* parent) : \
    QGraphicsView(this->_scene = new Scene, parent), \
    currentScaleFactor(1)
{
    this->setMinimumSize(viewerSize);
    this->setMaximumSize(viewerSize);
    this->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->_scene->workplace()->setZValue(-1);
}

CircuitDiagramViewer::~CircuitDiagramViewer()
{
    delete this->_scene;
    //delete this->workplace;
}

void CircuitDiagramViewer::placeComponent(ElectronicComponent *device, QPoint pos)
{
    device->setPos(pos);
    this->_scene->addItem(device);
}

void CircuitDiagramViewer::placeWire(Wire* wire)
{
    this->_scene->addItem(wire);
}

void CircuitDiagramViewer::zoomIn()
{
    if(this->currentScaleFactor <= 8)
    {
        this->currentScaleFactor *= viewerScaleStepFactor;
        this->scale(viewerScaleStepFactor, viewerScaleStepFactor);
    }
}

void CircuitDiagramViewer::zoomOut()
{
    if(this->currentScaleFactor >= 0.125)
    {
        this->currentScaleFactor /= viewerScaleStepFactor;
        this->scale(1 / viewerScaleStepFactor, 1 / viewerScaleStepFactor);
    }
}

QRectF CircuitDiagramViewer::getWorkplaceRect()
{
    return this->_scene->workplace()->rect();
}

void CircuitDiagramViewer::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers().testFlag(Qt::ControlModifier))
    {
        if(event->angleDelta().y() > 0)
        {
            this->zoomIn();
        }
        else if(event->angleDelta().y() < 0)
        {
            this->zoomOut();
        }

        return; // forbid scrolling
    }

    QGraphicsView::wheelEvent(event);
}

QImage CircuitDiagramViewer::toImage()
{
    QBrush brush = this->_scene->workplace()->brush();
    this->_scene->workplace()->setBrush(QBrush(Qt::white));

    this->_scene->clearSelection(); // Selections would also render to the file
    this->_scene->setSceneRect(this->_scene->itemsBoundingRect()); // Re-shrink the scene to its bounding contents
    QImage image(this->_scene->sceneRect().size().toSize(), QImage::Format_ARGB32); // Create the image with the exact size of the shrunk scene
    image.fill(Qt::transparent);

    QPainter painter(&image);
    this->_scene->render(&painter);
    this->_scene->workplace()->setBrush(brush);
    return image;
}
