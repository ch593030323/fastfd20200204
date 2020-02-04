#include "public_pushbuton_rt_check.h"
#include <QStylePainter>
#include <QStyleOptionButton>

public_pushbuton_rt_check::public_pushbuton_rt_check(QWidget *parent) :
    lds_toolbar_button(parent)
{
    rt_state = none;
}

QString public_pushbuton_rt_check::get_m_belong_state()
{
    return m_belong_state;
}

void public_pushbuton_rt_check::set_rt_checked(const vch_membernoData &m)
{
    m_belong_state = m.ch_member_belong_state;
    rt_state = rt_checked(m);
    update();
}

void public_pushbuton_rt_check::set_rt_checked(bool f)
{
    rt_state = rt_checked(f);
    update();
}

void public_pushbuton_rt_check::set_rt_checked_warn(bool f)
{
    rt_state = rt_checked_warn(f);
    update();
}

public_pushbuton_rt_check::checkState public_pushbuton_rt_check::rt_checked(const vch_membernoData &m)
{
    checkState rt_state = none;
    if(!m.vch_memberno.isEmpty()) {
        if("Y" == m.ch_member_belong_state) {
            rt_state = yunm;
        } else {
            rt_state = info;
        }
    }
    return rt_state;
}

public_pushbuton_rt_check::checkState public_pushbuton_rt_check::rt_checked(bool f)
{
    checkState rt_state = none;
    if(f) rt_state = info;
    return rt_state;
}

public_pushbuton_rt_check::checkState public_pushbuton_rt_check::rt_checked_warn(bool f)
{
    checkState rt_state = none;
    if(f) rt_state = warn;
    return rt_state;
}

QPixmap public_pushbuton_rt_check::pixmap_checked(checkState state)
{
    QPixmap pg;
    switch(state) {
    case info:
        pg = QPixmap(":/image/widgets_pic/radio_on.png").scaledToWidth(20, Qt::SmoothTransformation);
        break;
    case yunm:
        pg = QPixmap(":/image/widgets_pic/radio_on_yun.png").scaledToWidth(20, Qt::SmoothTransformation);
        break;
    case warn:
        pg = QPixmap(":/image/widgets_pic/radio_on_warn.png").scaledToWidth(20, Qt::SmoothTransformation);
        break;
    default:
        break;
    }
    return pg;
}

QPixmap public_pushbuton_rt_check::pixmap_checked(const vch_membernoData &m)
{
    return pixmap_checked(rt_checked(m));
}

void public_pushbuton_rt_check::paintEvent(QPaintEvent *e)
{
    QPushButton::paintEvent(e);
    QPainter p(this);
    QPixmap pg = pixmap_checked(rt_state);

    if(!pg.isNull()) {
        p.drawPixmap(QRect(width()-pg.width(), 0, pg.width(), pg.height()), pg);
    }
}

void public_pushbuton_rt_check::resizeEvent(QResizeEvent *event)
{
    lds_toolbar_button::resizeEvent(event);
//    int w = qMin(this->width(), this->height()) * 2 / 3;
//    setIconSize(QSize(w, w));
}
