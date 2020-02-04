#include "lds_roundeddialog_rect.h"
#include <QPainter>
#include <QVariant>
#include "lds.h"

void lds_roundeddialog_rect0::setbgColor(const QColor &color)
{
    bgColor = color;
}

QColor lds_roundeddialog_rect0::getbgColor() const
{
    return bgColor;
}

void lds_roundeddialog_rect0::settitleColor(const QColor &color)
{
    titleColor = color;
}

QColor lds_roundeddialog_rect0::gettitleColor() const
{
    return titleColor;
}

lds_roundeddialog_rect0::lds_roundeddialog_rect0(QWidget *parent) :
    QDialog(parent)
{
    lds::setwflagFrameless(this);
    this->setProperty("outer_stylesheet", "lds_roundeddialog_rect");
}

void lds_roundeddialog_rect0::enable_click_empty_reject()
{
    click_empty_reject = true;
}

void lds_roundeddialog_rect0::enable_transparent_pay()
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    transparent_paint = true;
}

void lds_roundeddialog_rect0::paintEvent(QPaintEvent *e)
{
    if(true == transparent_paint) {
        QPainter p(this);
        p.setPen(Qt::NoPen);
        p.fillRect(this->rect(), bgColor);
    }
}
void lds_roundeddialog_rect0::mouseReleaseEvent(QMouseEvent *e)
{
    QDialog::mouseReleaseEvent(e);
    if(click_empty_reject) {
        this->reject();
    }
}




lds_roundeddialog_rect::lds_roundeddialog_rect(QWidget *parent)
    : lds_roundeddialog_rect0(parent)
{
    enable_transparent_pay();
}
