#include "lds_label_circle_num.h"
#include <QPainter>
#include <QTimerEvent>

lds_label_circle_num::lds_label_circle_num(QWidget *parent) :
    label_click(parent)
{
}

void lds_label_circle_num::setup(QWidget *buddy, const QSize &size, const QColor &color, int xoff, int yoff)
{
    this->setFixedSize(size);
    this->setColor(color);
    this->setGeometry(buddy->geometry().right() - this->width() + xoff, buddy->geometry().top() - yoff, this->width(), this->height());
}

void lds_label_circle_num::setColor(const QColor &color)
{
    this->color = color;
}

void lds_label_circle_num::stop()
{
    timer.stop();
    update();
}

void lds_label_circle_num::start()
{
    timer.start(60, this);
    update();
}

void lds_label_circle_num::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timer.timerId()) {
        ++step;
        step = step % 360;
        update();
    } else {
        QWidget::timerEvent(event);
    }
}

void lds_label_circle_num::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    QRectF r = this->rect();
    qreal off = 1;
    path.addEllipse(r);
    p.fillPath(path,color);
    r.adjust(off, off, -off, -off);
    QConicalGradient conical;
    if(timer.isActive()) {
        conical.setColorAt(0, QColor(255, 255, 255, 0));
        conical.setColorAt(1, QColor(255, 255, 255, 255));
    } else {
        conical.setColorAt(0, QColor(255, 255, 255, 120));
        conical.setColorAt(1, QColor(255, 255, 255, 120));
    }
    conical.setCenter(r.center());
    conical.setAngle(step * 10);

    p.setPen(QPen(conical, 2 * off));
    p.drawEllipse(r);

    QFont f = p.font();
    f.setPointSize(20);
    p.setFont(f);
    p.setPen(QPen(QColor("white")));
    p.drawText(this->rect(), Qt::AlignCenter, this->text());
}
