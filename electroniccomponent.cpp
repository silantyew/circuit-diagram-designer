#include "electroniccomponent.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QtMath>
#include <QBrush>
#include <QMessageBox>
#include <QInputDialog>
#include "parameters.h"
#include <QDebug>


/*bool isObjWithinWorkplace(QPointF objPos, QRectF workplace)
{
    return objPos.x() > 0 && objPos.y() > 0 && \
            objPos.x() < workplace.bottomRight().x() && objPos.y() < workplace.bottomRight().y();
}*/


ElectronicComponent::ElectronicComponent(const QString& id, \
                                         const QString& iconFilename, \
                                         const QSize& size) : \
    QGraphicsPixmapItem(QPixmap(iconFilename).scaled(size)), \
    _id(id), \
    _iconFilename(iconFilename), \
    _label(new Label), \
    _rotationAngles {0, 0, 0}
{

    this->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);

    this->_label->setParentItem(this);
    this->_label->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    this->_label->setDefaultTextColor(Qt::black);
    this->placeLabel();

    this->setUpContextMenu();
}

ElectronicComponent* ElectronicComponent::copy() const
{
    ElectronicComponent* _new =  new ElectronicComponent(this->_id, \
                                                         this->_iconFilename, \
                                                         QSize(this->pixmap().size()));

    _new->label()->setPlainText(this->label()->toPlainText());
    for(const auto& j: this->_junctions)
    {
        _new->addJunction(j->pos());
    }
    return _new;
}

ElectronicComponent::~ElectronicComponent()
{
    delete this->_label;

    for(auto i: this->_junctions)
    {
        delete i;
    }
}

QString ElectronicComponent::id() const
{
    return this->_id;
}

QPainterPath ElectronicComponent::shape() const
{
    return getShapeWithMargins(QGraphicsPixmapItem::shape());
}

int ElectronicComponent::rotation(Qt::Axis axis)
{
    return this->_rotationAngles[axis];
}

QGraphicsTextItem* ElectronicComponent::label() const
{
    return this->_label;
}

const QVector<QGraphicsEllipseItem*>* ElectronicComponent::junctions() const
{
    return &this->_junctions;
}

void ElectronicComponent::setUpContextMenu()
{
    contextMenu = new QMenu("Edit component");
    this->actionRotateClockwise = this->contextMenu->addAction("Rotate clockwise");
    this->actionRotateAnticlockwise = contextMenu->addAction("Rotate counterclockwise");
    this->actionMirrorHorizontally = this->contextMenu->addAction("Mirror horizontally");
    this->actionMirrorVertically = contextMenu->addAction("Mirror vertically");
    this->contextMenu->addSeparator();
    this->actionIsMovable = this->contextMenu->addAction("Movable");
    this->actionIsMovable->setCheckable(true);
    this->actionIsMovable->setChecked(true);
    this->actionRename = contextMenu->addAction("Rename");
    this->actionRemove = contextMenu->addAction("Remove");
}

void ElectronicComponent::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    QAction* selectedAction = this->contextMenu->exec(event->screenPos());
    if(selectedAction == actionRemove)
    {
        if(QMessageBox::question(nullptr, \
                                 "Remove item", \
                                 "Do you really want to permanently remove item?", \
                                 QMessageBox::Yes | QMessageBox::No) == \
           QMessageBox::Yes)
        {
            delete this;
        }
    }
    else if(selectedAction == actionRename)
    {
        this->_label->setPlainText(QInputDialog::getText(nullptr, \
                                                         "Change label", \
                                                         "Enter a new label:", \
                                                         QLineEdit::Normal, \
                                                         this->_label->toPlainText()) \
                                   );
    }
    else if(selectedAction == actionIsMovable)
    {
        this->setFlag(GraphicsItemFlag::ItemIsMovable, actionIsMovable->isChecked());
    }
    else if(selectedAction == actionRotateClockwise)
    {
        this->rotate(90, Qt::ZAxis);
    }
    else if(selectedAction == actionRotateAnticlockwise)
    {
        this->rotate(-90, Qt::ZAxis);
    }
    else if(selectedAction == actionMirrorHorizontally)
    {
        this->rotate(180, Qt::YAxis);
    }
    else if(selectedAction == actionMirrorVertically)
    {
        this->rotate(180, Qt::XAxis);
    }
}

void ElectronicComponent::rotate(qreal angle, Qt::Axis axis)
{
    this->setTransform(this->transform().rotate(angle, axis));
    this->_label->setTransform(this->_label->transform().rotate(-angle, axis));

    // XAxis = 0, YAxis = 1, ZAxis = 2
    _rotationAngles[axis] += angle;
    _rotationAngles[axis] %= 360;
}

void ElectronicComponent::placeLabel()
{
    QSize componentSize = this->pixmap().size();
    QPointF newPos = componentSize.width() >= componentSize.height() ? \
                QPointF(componentSize.width() / 2 - 10, -25) : \
                QPointF(componentSize.width() + 5, componentSize.height() / 2 - 12);

    this->_label->setPos(newPos);
}


void ElectronicComponent::Label::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsTextItem::mouseMoveEvent(event);
    this->setPos(this->pos());
}


void ElectronicComponent::addJunction(QPointF pos)
{
    this->_junctions.append(new Junction(pos, this));
}
