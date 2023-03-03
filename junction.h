#ifndef JUNCTION_H
#define JUNCTION_H

#include <QGraphicsEllipseItem>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneMouseEvent>

typedef QAction* QAction_ptr;


class Junction : public QGraphicsEllipseItem
{
public:
    Junction(QPointF pos, QGraphicsItem* parent = nullptr);

private:
    QMenu* contextMenu;
    QAction_ptr actionRemove;

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
};

#endif // JUNCTION_H
