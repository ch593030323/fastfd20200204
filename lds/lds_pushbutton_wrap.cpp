#include "lds_pushbutton_wrap.h"
#include <QStylePainter>
#include <QStyleOptionButton>
#include<QPainterPath>
#include <QTimeLine>
#include <QEventLoop>
#include <QTimer>
#include <QLabel>
#include <QMovie>
#include <QMouseEvent>
#include <QtDebug>
#include <QEventLoop>
#include <QMap>
#include "lds.h"


lds_pushbutton_wrap::lds_pushbutton_wrap(QWidget *parent) :
    lds_pushbutton(parent)
{
    xp1 = 1;
    yp1 = 1;
    xp2 = -1;
    yp2 = -1;
    textflags = int(Qt::AlignCenter|Qt::TextWordWrap);
}

lds_pushbutton_wrap::~lds_pushbutton_wrap()
{
}

void lds_pushbutton_wrap::paintEvent(QPaintEvent *e)
{
    QStylePainter p(this);
    QStyleOptionButton o;
    QRect rect0 = this->rect().adjusted(xp1, yp1, xp2, yp2);
    QPixmap iconpix = this->icon().pixmap(iconSize());
    initStyleOption(&o);
    QString t = o.text;
    paintBackground(&p, o);
    //
    if(false == iconpix.isNull()) {
        rect0.adjust(iconpix.width() + 2, 0, 0, 0);
        p.drawPixmap(QRect(xp1 + 2, yp1 + (rect0.height() - iconpix.height()) / 2, iconpix.width(), iconpix.height()), iconpix);
    }
    //

    QFontMetrics fm(p.font());
    if(rect0.width() < fm.size(textflags, t).width()) {
        p.drawText(rect0, textflags | Qt::TextWrapAnywhere, t);
    } else {
        p.drawText(rect0, textflags, t);
    }


    int w = rect0.width();
    int h= rect0.height();
    int off = 15;
    ///
    if(show_top_right) {//三角
        QPainterPath path0;

        path0.addPolygon(QVector<QPointF>()
                         << QPointF(rect0.x() + w - off, rect0.y() + 0)
                         << QPointF(rect0.x() + w, rect0.y() + 0)
                         << QPointF(rect0.x() + w, rect0.y() + off)
                         ) ;
        p.fillPath( path0,
                    QColor("#50c600")
                    );
    }
    ///~

    ///
    if(b_enable_frame && isDefault()&&hasFocus() ) {
        p.save();
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor("crimson"), 3));
        p.drawRect(this->rect().adjusted(0,0,-3.0/2,-3.0/2));
        p.restore();
    }
    ///~
    if(false == pixmap.isNull()) {
        p.drawPixmap(QStyle::alignedRect(this->layoutDirection(), pixmap_flag, pixmap.size(), this->rect()), pixmap);
    }
}

void lds_pushbutton_wrap:: show_flag_top_right(bool f)
{
    show_top_right = f;
    update();
}

void lds_pushbutton_wrap::show_pixmap(Qt::Alignment pixmap_flag, const QPixmap &pixmap)
{
    this->pixmap =pixmap;
    this->pixmap_flag =pixmap_flag;
    update();
}

void lds_pushbutton_wrap::disableFrame()
{
    b_enable_frame = false;
}

void lds_pushbutton_wrap::enableFlash(bool show_flash)
{
    this->show_flash = show_flash;
}


