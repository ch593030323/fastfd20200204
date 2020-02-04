#include "lds_roundeddialog_rect_widget.h"
#include <QPainter>
#include <QVariant>
#include "lds.h"


void lds_roundeddialog_rect_widget::setbgColor(const QColor &color)
{
    bgColor = color;
}

QColor lds_roundeddialog_rect_widget::getbgColor() const
{
    return bgColor;
}

void lds_roundeddialog_rect_widget::settitleColor(const QColor &color)
{
    titleColor = color;
}

QColor lds_roundeddialog_rect_widget::gettitleColor() const
{
    return titleColor;
}

lds_roundeddialog_rect_widget::lds_roundeddialog_rect_widget(QWidget *parent) :
    QWidget(parent)
{
    //    setAttribute(Qt::WA_TranslucentBackground, true);
    //    lds::setwflagFrameless(this);
    this->setProperty("outer_stylesheet", "lds_roundeddialog_rect");
}

void lds_roundeddialog_rect_widget::reject()
{
    this->close();
}

void lds_roundeddialog_rect_widget::accept()
{
    this->close();
}

int lds_roundeddialog_rect_widget::exec()
{
    show();
    return 1;
}

void lds_roundeddialog_rect_widget::enable_click_empty_reject()
{
    click_empty_reject = true;
}

void lds_roundeddialog_rect_widget::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.fillRect(this->rect(), bgColor);
}
void lds_roundeddialog_rect_widget::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);
    if(click_empty_reject) {
        this->reject();
    }
}
