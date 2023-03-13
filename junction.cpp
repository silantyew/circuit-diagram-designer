#include <QBrush>
#include <QCursor>
#include <QCoreApplication>
#include <QGraphicsSceneContextMenuEvent>
#include "circuitdiagrameditor.h"
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
    this->actionRemove = this->contextMenu->addAction("Remove junction");
}

void Junction::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    if(this->parentItem())
    {
        QMenu itemSelectionContextMenu;
        itemSelectionContextMenu.addAction("Junction"); // select this junction item

        QString parentName;
        if(dynamic_cast<Wire*>(this->parentItem()))
        {
            parentName = "Wire";
        }
        else if(ElectronicComponent* c = dynamic_cast<ElectronicComponent*>(this->parentItem()))
        {
            parentName = "Component: " + c->label()->toPlainText();
        }
        QAction_ptr selectParentAction = itemSelectionContextMenu.addAction(parentName);


        QAction* selectedAction = itemSelectionContextMenu.exec(event->screenPos());
        if(selectedAction == selectParentAction)
        {
            //this->parentItem()-> OPEN CONTEXT MENU OF PARENT ITEM
            QEvent graphicsSceneContextMenuEvent(QEvent::GraphicsSceneContextMenu);
            this->scene()->sendEvent(this->parentItem(), &graphicsSceneContextMenuEvent);
            return;
        }
    }


    QAction* selectedAction = this->contextMenu->exec(event->screenPos());

    if(selectedAction == this->actionRemove)
    {
        delete this;
    }
}

void Junction::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);
    dynamic_cast<CircuitDiagramEditor::Scene*>(\
                this->scene())->changeWireDrawingState(this->scenePos());
}
