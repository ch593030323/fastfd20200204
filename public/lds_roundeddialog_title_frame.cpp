#include "lds_roundeddialog_title_frame.h"
#include <QPainter>
#include "lds.h"
#include "saas_pay_detail.h"

lds_roundeddialog_title_frame::lds_roundeddialog_title_frame(QWidget *parent)
    : QFrame(parent) {
    titleColor = 0;
    isbottomBorder =false;
}

void lds_roundeddialog_title_frame::setTitleColor(QColor *titleColor)
{
    this->titleColor = titleColor;
    update();
}

void lds_roundeddialog_title_frame::enableBottomBorder()
{
    isbottomBorder = true;
}

void lds_roundeddialog_title_frame::paintEvent(QPaintEvent *e)
{
    QFrame::paintEvent(e);

    QPainter p(this);
    QRect rect0 = this->rect();
    //bg RIGHT
    if(titleColor) {
        lds::paintPolygon(&p, rect0, *titleColor);
    }
    //top left
    saas_pay_detail::paintLeftTop(rect0, &p);

    //isbottomBorder
    if(isbottomBorder) {
        p.fillRect(rect0.x(),  rect0.y() + rect0.height() - 2, rect0.width(), 2,
                   "obsidian" == lds::sysconf->value("system_setting/skin_name", "obsidian").toString() ?  QColor("black") :  QColor("darkgrey")
                                                                                                           );
    }
}
