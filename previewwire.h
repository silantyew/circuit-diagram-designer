#ifndef PREVIEWWIRE_H
#define PREVIEWWIRE_H

#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>


class PreviewWire : public QGraphicsItemGroup
{
public:
    PreviewWire();
    void invertDirection();
    void setStartPoint(const QPointF& point);
    void setEndPoint(const QPointF& point);
    QPointF startPoint(), vertexPoint(), endPoint();
    void update();

    enum class Direction { Horizontal, Vertical, HorizontalToVertical, VerticalToHorizontal };
    Direction direction();

private:
    QGraphicsLineItem *segment1, *segment2;
    QPointF _startPoint, _endPoint;
    bool invertedDirection;

    void updateSegments();
};

#endif // PREVIEWWIRE_H
