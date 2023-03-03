#include "previewwire.h"

PreviewWire::PreviewWire() : \
    segment1(new QGraphicsLineItem), \
    segment2(new QGraphicsLineItem), \
    invertedDirection(false)
{
    this->addToGroup(this->segment1);
    this->addToGroup(this->segment2);
}


void PreviewWire::invertDirection()
{
    this->invertedDirection = !this->invertedDirection;
}


void PreviewWire::setStartPoint(const QPointF &point)
{
    this->_startPoint = point;
    this->updateSegments();
}


void PreviewWire::setEndPoint(const QPointF &point)
{
    this->_endPoint = point;
    this->updateSegments();
}


QPointF PreviewWire::startPoint()
{
    return this->_startPoint;
}


QPointF PreviewWire::endPoint()
{
    return this->_endPoint;
}


QPointF PreviewWire::vertexPoint()
{
   switch(this->direction())
   {
   case Direction::HorizontalToVertical:
       return QPointF(this->endPoint().x(), this->startPoint().y());

   case Direction::VerticalToHorizontal:
       return QPointF(this->startPoint().x(), this->endPoint().y());

   default:
       return QPointF(); // there is no vertex point
   }
}


PreviewWire::Direction PreviewWire::direction()
{
    if(this->startPoint().x() == this->endPoint().x())
    {
        return Direction::Horizontal;
    }
    else if(this->startPoint().y() == this->endPoint().y())
    {
        return Direction::Vertical;
    }
    else
    {
        return this->invertedDirection ? \
                    Direction::VerticalToHorizontal : \
                    Direction::HorizontalToVertical;
    }
}


void PreviewWire::updateSegments()
{
    switch(this->direction())
    {
    case Direction::Horizontal:
    case Direction::Vertical:
        this->segment2->hide();
        this->segment1->setLine(QLine(this->startPoint().toPoint(), this->endPoint().toPoint()));
        break;

    case Direction::HorizontalToVertical:
    case Direction::VerticalToHorizontal:
        this->segment2->show();
        this->segment1->setLine(QLine(this->startPoint().toPoint(), this->vertexPoint().toPoint()));
        this->segment2->setLine(QLine(this->vertexPoint().toPoint(), this->endPoint().toPoint()));
        break;
    }
}


void PreviewWire::update()
{
    this->updateSegments();
    QGraphicsItemGroup::update();
}
