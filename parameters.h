#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QtGlobal>
#include <QPointF>
#include <QPainterPath>
#include <QPaintDevice>

const QString componentsLibraryPath = "./library.xml";
const QSize workplaceSize = QSize(3508, 2480);

constexpr qreal viewerScaleStepFactor = 2;
constexpr qreal gridStep = 10;
constexpr qreal graphicElementMargin = gridStep;
constexpr qreal junctionPointRadius = 10;


inline QPointF snapToGrid(QPointF pos)
{

    return QPoint(qRound(pos.x() / gridStep) * gridStep, \
                  qRound(pos.y() / gridStep) * gridStep);
}

/*inline QPointF snapToAxis(QPointF referencePoint, QPointF endPoint)
{
    // y1 = x1 + b1
    qreal b1 = referencePoint.y() - referencePoint.x();
    qreal referenceLine1_y = endPoint.x() + b1;

    // y2 = x2 + b2
    qreal b2 = referencePoint.y() + referencePoint.x();
    qreal referenceLine2_y = -endPoint.x() + b2;

    if((endPoint.y() >= referenceLine1_y && endPoint.y() >= referenceLine2_y) || \
       (endPoint.y() <= referenceLine1_y && endPoint.y() <= referenceLine2_y))
    {
        return QPointF(referencePoint.x(), endPoint.y());
    }
    else
    {
        return QPointF(endPoint.x(), referencePoint.y());
    }
}*/

inline QPainterPath getShapeWithMargins(const QPainterPath& shape)
{
    QPainterPath newPath;
    newPath.addRect(shape.boundingRect().adjusted(-graphicElementMargin,
                                                  -graphicElementMargin, \
                                                  graphicElementMargin, \
                                                  graphicElementMargin));
    return newPath;
}


#endif // PARAMETERS_H
