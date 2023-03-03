#include <QBrush>
#include <QCursor>
#include "circuitdiagramviewer.h"
#include "junction.h"
#include "parameters.h"

Junction::Junction(QPointF pos, QGraphicsItem* parent)
{
    this->setPos(pos);
    this->setParentItem(parent);
    this->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
    this->setRect(-junctionPointRadius / 2, -junctionPointRadius / 2, \
                      junctionPointRadius, junctionPointRadius);
    this->setBrush(QBrush(Qt::black));
    this->setCursor(QCursor(Qt::PointingHandCursor));
    this->setZValue(10);

    this->contextMenu = new QMenu("Edit junction");
    this->actionRemove = new QAction("Remove junction");
}

void Junction::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    QAction* selectedAction = this->contextMenu->exec(event->screenPos()); // TODO: not shown

    if(selectedAction == this->actionRemove)
    {
        delete this;
    }
}

void Junction::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);
    dynamic_cast<CircuitDiagramViewer::Scene*>(\
                this->scene())->changeWireDrawingState(this->scenePos());
}
