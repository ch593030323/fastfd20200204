#include "table_management_pad.h"
#include "table_management_desk.h"
#include "table_management_graphicsview.h"
#include <QGraphicsSceneMouseEvent>

table_management_pad::table_management_pad()
{
    setFlags(/*ItemIsMovable |*/ ItemIsSelectable | ItemIsFocusable);
    setZValue(1);

    control = new table_management_pad_control(this);
}

table_management_pad::~table_management_pad()
{

}

QRectF table_management_pad::boundingRect() const
{
    return boundrect;
}

QPainterPath table_management_pad::shape() const
{
    QPainterPath path;
    path.addRect(boundrect.toRect());
    return path;
}

void table_management_pad::paint(QPainter */*painter*/, const QStyleOptionGraphicsItem */*item*/, QWidget */*widget*/)
{
    //    painter->fillPath(shape(), QColor(0,0,255,100));
}

void table_management_pad::focusInEvent(QFocusEvent *event)
{
    QGraphicsItem::focusInEvent(event);
    control->show();
    control->setFocus();
}

void table_management_pad::focusOutEvent(QFocusEvent *event)
{
    QGraphicsItem::focusOutEvent(event);
}

void table_management_pad::setPos(const QPointF &pos)
{
    control->setPos(pos);
}

QSizeF table_management_pad::get_control_size()
{
    return control->boundingRect().size();
}

QPointF table_management_pad::get_control_pos()
{
    return control->pos();
}

void table_management_pad::set_app_item(table_management_desk *item)
{
    control->app_item = item;
}

void table_management_pad::update_boundrect(const QRectF &boundrect)
{
    prepareGeometryChange();
    this->boundrect = boundrect;
}

table_management_pad_control::table_management_pad_control(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    setFlags(/*ItemIsMovable |*/ ItemIsSelectable | ItemIsFocusable);
    setZValue(1);
    app_item = 0;
    pg= QPixmap(":/image/table_pad.png");
}

table_management_pad_control::~table_management_pad_control()
{

}

QRectF table_management_pad_control::boundingRect() const
{
    return QRectF(-pg.width()/2, -pg.height()/2, pg.width(), pg.height());
}

QPainterPath table_management_pad_control::shape() const
{
    QPainterPath path;
    path.addRect(QRect(-pg.width()/2, -pg.height()/2, pg.width(), pg.height()));
    return path;
}

void table_management_pad_control::paint(QPainter *painter, const QStyleOptionGraphicsItem */*item*/, QWidget */*widget*/)
{
    painter->drawPixmap(boundingRect().topLeft(),  pg);
}

void table_management_pad_control::focusInEvent(QFocusEvent *event)
{
    QGraphicsItem::focusInEvent(event);
    app_item->update_textcolor(QColor("#ff546b"));
    this->show();
}

void table_management_pad_control::focusOutEvent(QFocusEvent *event)
{
    QGraphicsItem::focusOutEvent(event);
    app_item->update_textcolor(QColor("black"));
    this->hide();
    this->parentItem()->hide();
}

void table_management_pad_control::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);


    if(QRectF(-180, -130, 90, 65).contains(event->pos()))transform_app_item("V+");
    if(QRectF(-90, -130, 90, 65).contains(event->pos()))transform_app_item("V-");
    if(QRectF(0, -130, 90, 65).contains(event->pos()))transform_app_item("H+V+");
    if(QRectF(90, -130, 90, 65).contains(event->pos()))transform_app_item("H-V-");

    if(QRectF(-180, -65, 90, 65).contains(event->pos()))transform_app_item("H+");
    if(QRectF(-90, -65, 90, 65).contains(event->pos()))transform_app_item("H-");
    if(QRectF(0, -65, 90, 65).contains(event->pos()))transform_app_item("R-");
    if(QRectF(90, -65, 90, 65).contains(event->pos()))transform_app_item("R+");

    if(QRectF(-180, 0, 180, 130).contains(event->pos()))transform_app_item("SHAPE");
    if(QRectF(0, 0, 180, 130).contains(event->pos()))transform_app_item("DEL");

}

void table_management_pad_control::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

void table_management_pad_control::transform_app_item(const QString &text)
{
    if(text.contains("R+")) {
        app_item->setData(table_management_desk::item_rotate, app_item->data(table_management_desk::item_rotate).toInt() + table_management_desk::rotate_add_value);
        app_item->setRotation(app_item->data(table_management_desk::item_rotate).toInt());
    }
    if(text.contains("R-")) {
        app_item->setData(table_management_desk::item_rotate, app_item->data(table_management_desk::item_rotate).toInt() - table_management_desk::rotate_add_value);
        app_item->setRotation(app_item->data(table_management_desk::item_rotate).toInt());
    }
    if(text.contains("V+")) {
        app_item->update_boundrect(table_management_desk::item_vscale, 1);
    }
    if(text.contains("V-")) {
        app_item->update_boundrect(table_management_desk::item_vscale, -1);
    }
    if(text.contains("H+")) {
        app_item->update_boundrect(table_management_desk::item_hscale_min, 1);
    }

    if(text.contains("H-")) {
        app_item->update_boundrect(table_management_desk::item_hscale_min, -1);
    }
    if(text.contains("SHAPE")) {
        app_item->set_shapetype(table_management_desk::shapeType(
                                    (app_item->get_shapetype() + 1) % table_management_desk::shape_max));
    }
    if(text.contains("DEL")) {
        this->clearFocus();
        static_cast<table_management_graphicsview*>(app_item->scene()->views()[0])->delete_item(app_item->data(table_management_desk::item_ch_tableno).toString());
        app_item = 0;
    }
}

///////
table_management_pad_control_btn::table_management_pad_control_btn(const QString &text, table_management_pad_control *parent)
    : QGraphicsItem(parent)
{
    parent_item = parent;
    this->text = text;
    ispressed = false;
    setFlag(ItemIsSelectable);
}

table_management_pad_control_btn::~table_management_pad_control_btn()
{

}

QRectF table_management_pad_control_btn::boundingRect() const
{
    return QRectF(-20, -15, 40, 30);
}

QPainterPath table_management_pad_control_btn::shape() const
{
    QPainterPath path;
    path.addRect(QRect(-20, -15, 40, 30));

    return path;
}

void table_management_pad_control_btn::paint(QPainter *painter, const QStyleOptionGraphicsItem */*item*/, QWidget */*widget*/)
{
    if(ispressed) {
        painter->fillPath(shape(), QColor("navy"));
    } else {
        painter->fillPath(shape(), QColor("purple"));
    }

    painter->drawText(boundingRect(), Qt::AlignCenter, text);
}

void table_management_pad_control_btn::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    ispressed = true;
    update();

    table_management_desk *item = parent_item->app_item;
    if(text == "R+") {
        item->setData(table_management_desk::item_rotate, item->data(table_management_desk::item_rotate).toInt() + table_management_desk::rotate_add_value);
        item->setRotation(item->data(table_management_desk::item_rotate).toInt());
    }
    if(text == "R-") {
        item->setData(table_management_desk::item_rotate, item->data(table_management_desk::item_rotate).toInt() - table_management_desk::rotate_add_value);
        item->setRotation(item->data(table_management_desk::item_rotate).toInt());
    }
    if(text == "V+") {
        item->update_boundrect(table_management_desk::item_vscale, 1);
    }
    if(text == "V-") {
        item->update_boundrect(table_management_desk::item_vscale, -1);
    }
    if(text == "H+") {
        item->update_boundrect(table_management_desk::item_hscale_min, 1);
    }

    if(text == "H-") {
        item->update_boundrect(table_management_desk::item_hscale_min, -1);
    }
    if(text == "SHAPE") {
        item->set_shapetype(table_management_desk::shapeType(
                                (item->get_shapetype() + 1) % table_management_desk::shape_max));
    }
}

void table_management_pad_control_btn::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);

}

void table_management_pad_control_btn::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);

    ispressed = false;
    update();
}
