#include "public_pushbutton_lineargradient.h"
#include <QStylePainter>
#include <QStyleOptionButton>
#include "lds.h"
#include <QMouseEvent>

public_pushbutton_LinearGradient::public_pushbutton_LinearGradient(QWidget *parent) :
    QPushButton(parent)
{
    do_longpress = new lds_record_do_longpress(this);
    connect(do_longpress, SIGNAL(timeout()), this, SIGNAL(timeout()));
}

void public_pushbutton_LinearGradient::paintEvent(QPaintEvent *e)
{
    QStylePainter p(this);
    QStyleOptionButton o;
    initStyleOption(&o);

    if(o.state & QStyle::State_Sunken) {
        p.fillRect(o.rect, QColor("#ff546b"));
    } else {
        QColor bgColor;
        if(lds::sysconf->value("system_setting/skin_name", "obsidian").toString() == "obsidian") {
            bgColor = QColor(104, 3, 17);
        } else {
            bgColor = QColor("#08272b");
        }

        QLinearGradient linear;
        linear.setColorAt(0, lds::static_color_light(bgColor, 4));
        linear.setColorAt(1, bgColor);
        linear.setStart(o.rect.topLeft());
        linear.setFinalStop(o.rect.bottomLeft());
        p.fillRect(o.rect, linear);
    }

    QPixmap pixmap = o.icon.pixmap(QSize(35, 35));
    int pixmap_h = o.rect.height() * 0.6;
    p.drawPixmap((this->width() - pixmap.width())/2, (pixmap_h - pixmap.height())/2,  pixmap);

    p.setPen(QColor("white"));
    p.drawText(o.rect.adjusted(0, pixmap_h, 0, 0), Qt::AlignCenter | Qt::TextWordWrap, o.text);
}

void public_pushbutton_LinearGradient::enableLongPress(bool flag)
{
        do_longpress->setEnable(flag);
}

void public_pushbutton_LinearGradient::mousePressEvent(QMouseEvent *event)
{
    QPushButton::mousePressEvent(event);
    do_longpress->start(event->pos());
}

void public_pushbutton_LinearGradient::mouseReleaseEvent(QMouseEvent *event)
{
    if(do_longpress->isprepared_stop()) {
        QPushButton::mouseReleaseEvent(event);
    } else {
        this->setDown(false);
    }
}
