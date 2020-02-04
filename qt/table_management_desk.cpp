#include "table_management_desk.h"
#include "math.h"
#include "table_management_pad.h"
#include "table_management_graphicsview.h"
#include <QGraphicsDropShadowEffect>
#include <QStyleOptionGraphicsItem>
#include "n_func.h"
#include "restauranttableview.h"
#include <QtDebug>
#include "lds.h"
#include "lds_scr_table.h"
#include <QGraphicsSceneMouseEvent>
#include "restauranttableview.h"
#include "lds_model_sqltablemodel.h"

static QPointF get_pop_point(QSizeF popsize, QRectF box, QRectF screen)
{
    QPointF pos = box.bottomLeft();
    QPointF pos2 =  box.topRight();

    //        pos.setX(pos.x()-popsize.width());
    pos2.setX(pos2.x()-popsize.width());
    if (pos.x() < screen.left()) {
        pos.setX(qMax(pos.x(), screen.left()));
    } else if (pos.x()+popsize.width() > screen.right()) {
        pos.setX(qMax(pos.x()-popsize.width(), screen.right()-popsize.width()));
    }

    if (pos.y() + popsize.height() > screen.bottom()) {
        pos.setY(pos2.y() - popsize.height());
    } else if (pos.y() < screen.top()) {
        pos.setY(screen.top());
    }
    if (pos.y() < screen.top()) {
        pos.setY(screen.top());
    }
    if (pos.y()+popsize.height() > screen.bottom()) {
        pos.setY(screen.bottom()-popsize.height());
    }
    return pos;
}


int table_management_desk::scale_add_value = 20;
int table_management_desk::rotate_add_value = 10;
table_management_desk::table_management_desk(table_management_pad *pad, shapeType stype)
{
    this->pad = pad;
    setFlags(ItemIsMovable | ItemIsSelectable| ItemIsFocusable);
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
    effect->setYOffset(0);
    effect->setXOffset(0);
    effect->setBlurRadius(20);
    effect->setColor(QColor(0, 0, 0, 180));
#ifdef Q_OS_WIN
    setGraphicsEffect(effect);
#endif

    setData(item_hscale_min, 1);
    setData(item_vscale, 1);

    setData(item_rotate, 0);
    setData(item_boundingRect, QRectF(-50, -50, 100, 100));
    setData(item_ch_areano, QString());
    setData(item_ch_tableno, QString());

    setData(item_shapetype, stype);
    setData(item_pos, QPointF());

    textcolor = QColor("black");
}

table_management_desk::~table_management_desk()
{
}

QRectF table_management_desk::boundingRect() const
{
    return data(item_boundingRect).toRectF();
}

QPainterPath table_management_desk::shape() const
{
    qreal w = boundingRect().width();
    qreal h = boundingRect().height();
    qreal w_2 = w / 2;
    qreal h_2 = h / 2;
    QPainterPath path;
    switch(data(item_shapetype).toInt() ) {
    case shape_rectangle_min:
        path.addRoundedRect(QRectF(-w_2, -h_2, w, h), 5, 5);
        break;
    case shape_triangle:
    {
        qreal sqr30 = pow(3, 0.5);
        path.addPolygon(QPolygonF(
                            QVector<QPointF>()
                            << QPointF(0, -h_2)
                            << QPointF(  h_2/2 * sqr30, h_2/2)
                            << QPointF( -h_2/2 * sqr30, h_2/2)
                            << QPointF(0, -h_2)
                            ));
    }
        break;
    case shape_circle:
        path.addEllipse(QRectF(-w_2, -h_2, w, h));
        break;

    default:
        break;
    }

    return path;
}

void table_management_desk::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget */*widget*/)
{
    const QPainterPath path = shape();
    p_dta.rect = path.boundingRect().toRect();
    p_dta.option_state = item->state;
    restraurantDelegate::paintTable(painter, p_dta);
    return;
}

int table_management_desk::type() const
{
    return Type;
}

void table_management_desk::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    //
    if(deskView->press_event_do_double_clicked(this->data(item_ch_tableno).toString())) {
        deskView->emit_signal_table_double_clicked(this->data(item_ch_tableno).toString());
        return;
    }
    //
    ismove = false;
    pad->hide();
}

void table_management_desk::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
    ismove = true;
}

void table_management_desk::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    if(flags() & ItemIsMovable) {
        if(!ismove) {
            QRectF boundrect = boundingRect().translated(this->pos());//.adjusted(10, 10, -10, -10);
            this->setSelected(false);
            QPointF ppos = get_pop_point(pad->get_control_size(), boundrect, pad->boundingRect());
            pad->setPos(ppos +
                        QPointF(pad->get_control_size().width()/2,
                                pad->get_control_size().height()/2
                                ));
            pad->set_app_item(this);
            pad->setFocus();
            pad->show();
        }
        return;
    }
    if(this->boundingRect().contains(event->pos())) {
        deskView->emit_signal_table_clicked(this->data(item_ch_tableno ).toString());
    }
}

void table_management_desk::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    deskView->emit_signal_table_double_clicked(this->data(item_ch_tableno ).toString());
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void table_management_desk::focusOutEvent(QFocusEvent *event)
{
    QGraphicsItem::focusOutEvent(event);
    deskView->emit_signal_table_clicked("");
}

void table_management_desk::set_shapetype(table_management_desk::shapeType stype)
{
    prepareGeometryChange();
    setData(item_shapetype, stype);
}

table_management_desk::shapeType table_management_desk::get_shapetype()
{
    return (shapeType)data(item_shapetype).toInt();
}

void table_management_desk::update_boundrect(int stype, int add_sub_key)
{
    prepareGeometryChange();

    QRectF rect = data(item_boundingRect).toRectF();
    if(stype == item_hscale_min) {
        rect.adjust(-scale_add_value*add_sub_key, 0, scale_add_value*add_sub_key, 0);
    }
    if(stype == item_vscale) {
        rect.adjust(0, -scale_add_value*add_sub_key, 0, scale_add_value*add_sub_key);
    }
    setData(item_boundingRect, rect);
}

void table_management_desk::update_textcolor(QColor textcolor)
{
    this->textcolor = textcolor;
    update();
}

void table_management_desk::paint_cell(const QSqlRecord &index, const restraurantDelegate *d)
{
    d->paintCellFill(p_dta, index);
}

void table_management_desk::paint_cell(const QModelIndex &index, const restraurantDelegate *d)
{
    d->paintCellFill(p_dta, index);
}

void table_management_desk::paint_path(QPainter *painter, const QPainterPath &path, const QPixmap &pixmap)
{
    painter->setPen(QPen(QColor(0,0,0,100), 1));
    painter->setBrush(pixmap);
    painter->drawPath(path);
}

void table_management_desk::paint_text(QPainter *painter, const QRect &rect, const QStyleOptionGraphicsItem *item, const QString &text)
{
    if(item->state & QStyle::State_Selected) {
        painter->setPen(QColor("#ff546b"));
    } else {
        painter->setPen(textcolor);
    }
    painter->drawText(rect, Qt::AlignCenter, text);
}
