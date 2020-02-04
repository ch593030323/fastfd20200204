#include "lds_widgetgoing.h"
#include <QPainter>
#include <QtDebug>

lds_widgetgoing::lds_widgetgoing(QWidget *parent) :
    QWidget(parent)
{
    stepcur = 4;
    steptotal = 7;
}

void lds_widgetgoing::setstep(int cur, int total)
{
    stepcur = cur;
    steptotal = total;
    update();
}

void lds_widgetgoing::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    if(steptotal == 0)
        return;

    p.setPen(Qt::NoPen);
    p.setBrush(QColor("black"));
    float curwidth = this->width() * stepcur / (steptotal*1.0);
    p.drawRect(QRectF(0, 0, curwidth, this->height()));
    p.setBrush(QColor("white"));
    p.drawRect(QRectF(curwidth, 0, this->width() * 1.0 - curwidth, this->height()));
}
