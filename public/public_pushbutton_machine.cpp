#include "public_pushbutton_machine.h"
#include <QStyle>
#include <QStyleOptionButton>
#include <QStylePainter>
#include <QMouseEvent>


public_pushbutton_machine::public_pushbutton_machine(QWidget *parent) :
    QPushButton(parent)
{
    this->setFocusPolicy(Qt::NoFocus);
    this->ischecked1 = false;
    this->ischeckable1 = false;
}

void public_pushbutton_machine::emit_signal_transactioned()
{
    emit signal_transactioned();
}

void public_pushbutton_machine::setCheckable1(bool ischeckable1)
{
    this->ischeckable1 = ischeckable1;
}

void public_pushbutton_machine::setChecked1(bool ischecked1)
{
    this->ischecked1 = ischecked1;
    update();
}

bool public_pushbutton_machine::isChecked1()
{
    return this->ischecked1;
}

void public_pushbutton_machine::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    QPushButton::mouseReleaseEvent(e);
    emit_signal_transactioned();
//    if(this->rect().contains(e->pos()) && ischeckable1) {
//        setChecked1(!ischecked1);
//    }
}

void public_pushbutton_machine::paintEvent(QPaintEvent *e)
{
    QStylePainter p(this);
    p.setRenderHints(QPainter::Antialiasing);
    QString t;
    QStyleOptionButton o;
    initStyleOption(&o);
    t = o.text;
    o.text = "";
    int check_icon_bord = 0;
    QSize check_icon_size = QSize(0, 0);
    int flag = Qt::AlignCenter;
    if(ischeckable1) {
        check_icon_bord = 10;
        check_icon_size = QSize(20, 20);
        flag = Qt::AlignLeft | Qt::AlignVCenter;
    }
    p.drawControl(QStyle::CE_PushButton, o);
    p.drawText(this->rect().adjusted(check_icon_bord+check_icon_size.width()+check_icon_bord, 0, 0, 0), flag, t);
    if(ischeckable1) {
            p.drawPixmap(QRect(check_icon_bord, (this->height() - check_icon_size.height()) / 2, check_icon_size.width(), check_icon_size.height()),
                         ischecked1 ? QPixmap(":/image/widgets_pic/radio_on.png") : QPixmap(":/image/widgets_pic/radio_disable_warn.png") );
    }
}
