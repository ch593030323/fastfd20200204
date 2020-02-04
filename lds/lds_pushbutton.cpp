#include "lds_pushbutton.h"
#include <QEvent>
#include <QApplication>
#include <QtDebug>
#include "lds.h"
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QMouseEvent>

lds_pushbutton *lds_pushbutton::createButton(const QIcon &icon, const QSize &icon_size)
{
    lds_pushbutton *b = new lds_pushbutton;
    b->setIcon(icon);
    b->setIconSize(icon_size);
    return b;
}

lds_pushbutton::lds_pushbutton(QWidget *parent) :
    QPushButton(parent)
{
    do_longpress = new lds_record_do_longpress(this);
    connect(do_longpress, SIGNAL(timeout()), this, SIGNAL(timeout()));
}

void lds_pushbutton::enablePaintMenuPopup(bool enable)
{
    p_menu_popup = enable;
    update();
}

void lds_pushbutton::setData(const QString &role, const QVariant &value)
{
    str_map.insert(role, value);
}

QVariant lds_pushbutton::getData(const QString &role)
{
    return str_map.value(role);
}

void lds_pushbutton::setData(int role, const QVariant &value)
{
    int_map.insert(role, value);
}

QVariant lds_pushbutton::getData(int role)
{
    return int_map.value(role);
}

void lds_pushbutton::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)

    QStylePainter p(this);
    QStyleOptionButton option;
    initStyleOption(&option);
    paintBackground(&p, option);
    p.drawControl(QStyle::CE_PushButtonLabel, option);
}

void lds_pushbutton::paintBackground(QStylePainter *painter, const QStyleOptionButton &option)
{
    QStyleOptionButton option1 = option;
    //
    option1.icon = QIcon();
    option1.text = "";

    QBrush background = option1.palette.background();
    if(p_menu_popup
            && option1.state & QStyle::State_Raised) {
        if(Qt::LinearGradientPattern == background.style()) {
            QLinearGradient *liner = (QLinearGradient*)background.gradient();

            painter->save();
            //fill path
            QPainterPath path;
            path.moveTo(0, 0);
            path.lineTo(this->width() - 1, 0);
            path.lineTo(this->width() - 1, this->height() - 20);
            path.lineTo(this->width() - 20, this->height() - 1);
            path.lineTo(0, this->height() - 1);
            path.closeSubpath();
            painter->fillPath(path, *liner);

            //draw broad
            painter->setPen(QPen(liner->stops().value(0).second, 1));
            painter->drawPath(path);

            //fill triangle
            path = QPainterPath();
            path.moveTo(this->width(), this->height() - 16);
            path.lineTo(this->width(),this->height());
            path.lineTo(this->width() - 16, this->height());
            path.closeSubpath();
            painter->fillPath(path, liner->stops().value(0).second);
            //
            painter->restore();
            return;
        }
    }
    painter->drawControl(QStyle::CE_PushButton, option1);
}

void lds_pushbutton::toUpdateText(const QString &text)
{
    this->setText(text);
}

void lds_pushbutton::enableLongPress(bool flag, QWidget *parent)
{
    do_longpress->setEnable(flag);
    if(parent != 0)
        do_longpress->loading_label->setParent(parent);
}

void lds_pushbutton::mousePressEvent(QMouseEvent *event)
{
    QPushButton::mousePressEvent(event);
    do_longpress->start(event->pos());
}

void lds_pushbutton::mouseReleaseEvent(QMouseEvent *event)
{
    if(do_longpress->isprepared_stop()) {
        QPushButton::mouseReleaseEvent(event);
    } else {
        this->setDown(false);
    }
}
