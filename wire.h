#ifndef WIRE_H
#define WIRE_H

#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>
#include <QList>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>
#include "junction.h"

typedef QAction* QAction_ptr;


class Wire : public QGraphicsItemGroup
{
public:
    Wire();
    ~Wire();
    void addVertex(QPointF vertex);
    void addJunction(QPointF pos);
    const QList<QPointF> vertexes() const;
    const QList<QPointF> junctions() const;
    void handleWireSegmentRemoving();

private:
    class WireSegment;
    QList<QPointF> _vertexes;
    QList<QPointF> _junctions;

    QMenu* contextMenu;
    QAction_ptr actionRemoveWire;

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
};


class Wire::WireSegment : public QGraphicsLineItem
{
public:
    WireSegment(const QPointF& p1, const QPointF& p2, QGraphicsItem* parent = nullptr);
    ~WireSegment();
    QPainterPath shape() const;
    bool isToBeRemoved() const;

private:
    //QList<Junction*> junctions;
    QMenu* contextMenu;
    QAction_ptr actionRemoveWireSegment, actionRemoveWire;
    bool _toBeRemoved;

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
    //void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
};


#endif // WIRE_H
