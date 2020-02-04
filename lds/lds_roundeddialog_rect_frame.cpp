#include "lds_roundeddialog_rect_frame.h"
#include <QPainter>

lds_roundeddialog_rect_frame::lds_roundeddialog_rect_frame(QWidget *parent) :
    QFrame(parent)
{
    bgColor = 0;
}

void lds_roundeddialog_rect_frame::paintEvent(QPaintEvent *e)
{
    QFrame::paintEvent(e);
    if(bgColor) {
        QPainter p(this);
        p.fillRect(this->rect(), *bgColor);
    }
}
