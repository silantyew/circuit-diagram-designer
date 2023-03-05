#include "wire.h"
#include <QPen>
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include "parameters.h"
#include "circuitdiagrameditor.h"


Wire::Wire()
{
    this->setHandlesChildEvents(false);
}

Wire::~Wire()
{
    for(auto& i: this->childItems())
    {
        delete i;
    }
}

void Wire::addVertex(QPointF vertex)
{
    if(!this->_vertexes.isEmpty())
    {
        WireSegment* wireSegment = new WireSegment(this->_vertexes.last(), vertex, this);
        wireSegment->setPen(QPen(QBrush(Qt::black), 2));
        this->addToGroup(wireSegment);
    }

    this->_vertexes.append(vertex);
}

void Wire::handleWireSegmentRemoving()
{
    auto segments = this->childItems();
    for(auto& i: segments)
    {
        if(!dynamic_cast<WireSegment*>(i)) // if child item is not a wire segment
        {
            segments.removeAll(i);
        }
    }

    if(segments.length() == 1 || \
       segments.length() <= 3 && \
           (dynamic_cast<WireSegment*>(segments.first())->line().length() <= 4 * junctionPointRadius || \
            dynamic_cast<WireSegment*>(segments.last())->line().length() <= 4 * junctionPointRadius))
    {
        delete this;
        return;
    }


    for(auto i = segments.begin(); i < segments.end(); ++i)
    {
        if(dynamic_cast<WireSegment*>(*i)->isToBeRemoved()) // segment does not exist anymore
        {
            Wire *newWire1 = new Wire, *newWire2 = new Wire;

            for(auto j = segments.begin(); j <= i; ++j)
            {
                QPointF p = dynamic_cast<WireSegment*>(*j)->line().p1();

                if(j == segments.begin() || j == i)
                {
                    newWire1->addJunction(p);
                }

                newWire1->addVertex(p);
            }

            for(auto j = i; j < segments.end(); ++j)
            {
                QPointF p = dynamic_cast<WireSegment*>(*j)->line().p2();

                if(j == i || j == segments.end() - 1)
                {
                    newWire2->addJunction(p);
                }

                newWire2->addVertex(p);
            }

            if(newWire1->vertexes().length() > 1)
            {
                dynamic_cast<CircuitDiagramEditor*>(this->scene()->views().first())->placeWire(newWire1);
            }

            if(newWire2->vertexes().length() > 1)
            {
                dynamic_cast<CircuitDiagramEditor*>(this->scene()->views().first())->placeWire(newWire2);
            }

            delete this;
            return;
        }
    }
}

const QList<QPointF> Wire::vertexes() const
{
    return this->_vertexes;
}

const QList<QPointF> Wire::junctions() const
{
    return this->_junctions;
}

void Wire::addJunction(QPointF pos)
{
    new Junction(pos, this);
    this->_junctions.append(pos);
}

Wire::WireSegment::WireSegment(const QPointF& p1, const QPointF& p2, QGraphicsItem* parent) : \
    QGraphicsLineItem(QLine(p1.toPoint(), p2.toPoint()), parent), _toBeRemoved(false)
{
    this->setFlags(QGraphicsItem::ItemIsSelectable);

    contextMenu = new QMenu("Edit wire");
    this->actionRemoveWireSegment = this->contextMenu->addAction("Remove wire segment");
    this->actionRemoveWire = this->contextMenu->addAction("Remove wire");
}

Wire::WireSegment::~WireSegment()
{
    delete this->contextMenu;

    /*for(auto i: this->junctions)
    {
        delete i;
    }*/
}

void Wire::WireSegment::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    QAction* selectedAction = this->contextMenu->exec(event->screenPos());

    if(selectedAction == this->actionRemoveWireSegment)
    {
        this->_toBeRemoved = true;
        dynamic_cast<Wire*>(this->parentItem())->handleWireSegmentRemoving();
    }
    else if(selectedAction == this->actionRemoveWire)
    {
        delete this->parentItem();
    }
}

bool Wire::WireSegment::isToBeRemoved() const
{
    return this->_toBeRemoved;
}

QPainterPath Wire::WireSegment::shape() const
{
    return getShapeWithMargins(QGraphicsLineItem::shape());
}

/*void Wire::WireSegment::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    this->junctions.append(new Junction(snapToGrid(event->pos()), this));
}*/
