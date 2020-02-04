#include "lds_roundeddialog_rect_pop.h"
#include <QPainter>
#include <QVariant>
#include "lds.h"
#include <QMouseEvent>
#include <QApplication>


void lds_roundeddialog_rect_pop::setbgColor(const QColor &color)
{
    if(bgColor.isValid()) return;

    bgColor = color;
}

QColor lds_roundeddialog_rect_pop::getbgColor() const
{
    return bgColor;
}

void lds_roundeddialog_rect_pop::settitleColor(const QColor &color)
{
    titleColor = color;
}

QColor lds_roundeddialog_rect_pop::gettitleColor() const
{
    return titleColor;
}

lds_roundeddialog_rect_pop::lds_roundeddialog_rect_pop(QDialog *buddy, QWidget *parent, Qt::WindowFlags flags) :
    QDialog(parent)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(flags);
    this->setProperty("outer_stylesheet", "lds_roundeddialog_rect_align");
    this->setGeometry(QRect(QPoint(qApp->desktop()->availableGeometry().topLeft()),  lds::MAIN_WINDOW_SIZE));

    this->buddy = buddy;
    this->buddy->setParent(this);
    this->buddy->setWindowFlags(Qt::Widget);

    basewidget = 0;

    connect(this->buddy,SIGNAL(finished(int)),this,SLOT(todone(int)));

}

bool lds_roundeddialog_rect_pop::posIsValid()
{
    return buddy_pos_is_valid;
}

void lds_roundeddialog_rect_pop::posDisable()
{
    buddy_pos_is_valid = false;
}

void lds_roundeddialog_rect_pop::updatePos(const QPoint &pos)
{
    buddy_pos_is_valid = true;
    buddy->move(pos - qApp->desktop()->availableGeometry().topLeft());
}

void lds_roundeddialog_rect_pop::updatePos(QWidget *basewidget)
{
    this->basewidget = basewidget;
    updatePos(lds::getPopPoint_size(basewidget, buddy));
}

QDialog *lds_roundeddialog_rect_pop::get_buddy()
{
    return buddy;
}

void lds_roundeddialog_rect_pop::toUpdatePos()
{
    if(basewidget) {
        updatePos(lds::getPopPoint_size(basewidget, buddy));
    }
}

void lds_roundeddialog_rect_pop::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.fillRect(this->rect(), bgColor);
}

void lds_roundeddialog_rect_pop::mousePressEvent(QMouseEvent *e)
{
    QRect rect(buddy->pos(), buddy->size());
    if(rect.contains(e->pos())) {

    } else {
        buddy->reject();
    }
    QDialog::mousePressEvent(e);
}

void lds_roundeddialog_rect_pop::todone(int result)
{
    if(this != this->sender()) {
        this->done(result);
    }
    buddy->show();
}
