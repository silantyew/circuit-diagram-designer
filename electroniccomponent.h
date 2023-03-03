#ifndef ELECTRONICCOMPONENT_H
#define ELECTRONICCOMPONENT_H

#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include "junction.h"

typedef QAction* QAction_ptr;

class ElectronicComponent : public QGraphicsPixmapItem
{
public:
    ElectronicComponent(const QString& id, const QString& iconFilename, const QSize& size);
    ~ElectronicComponent();
    QString id() const;
    QPainterPath shape() const;
    QGraphicsTextItem* label() const;
    void addJunction(QPointF pos);
    const QVector<QGraphicsEllipseItem*>* junctions() const;
    ElectronicComponent* copy() const;
    int rotation(Qt::Axis axis);
    void rotate(qreal angle, Qt::Axis axis);

private:
    class Label;

    QString _id, _iconFilename;
    Label* _label;
    QVector<QGraphicsEllipseItem*> _junctions;

    QMenu* contextMenu;
    QAction_ptr actionRotateClockwise, actionRotateAnticlockwise, actionIsMovable, \
            actionRename, actionRemove, actionMirrorHorizontally, actionMirrorVertically;
    int _rotationAngles[3];

    void setUpContextMenu();
    void placeLabel();

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
};


class ElectronicComponent::Label : public QGraphicsTextItem
{
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

};

#endif // ELECTRONICCOMPONENT_H
