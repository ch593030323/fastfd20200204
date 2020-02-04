#include "lds_datetimeedit.h"
#include <QMouseEvent>
#include <QCalendarWidget>
#include <QtDebug>
#include <QStyleOptionComboBox>
#include <QPainter>
#include <QStyle>
#include <QTimer>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include "lds_popup_pos.h"
#include "lds_calendarwidget.h"
#include <QStyleOptionComboBox>
#include <QApplication>



lds_datetimeEdit::lds_datetimeEdit(QWidget *parent):
    QDateTimeEdit(parent)
{
    this->setCalendarPopup(true);
    this->setDisplayFormat("yyyy-MM-dd h:m:s");
    calendar = 0;
    is_pop_adjust_postion = false;
}

void lds_datetimeEdit::setDisplayFormat(const QString &format)
{
    QDateTimeEdit::setDisplayFormat(format);
}

QString lds_datetimeEdit::dateTimeStr(const QString &format)
{
    return this->dateTime().toString(format);
}

void lds_datetimeEdit::enablePopAdjustPostion()
{
    is_pop_adjust_postion = true;
}

QDateTime lds_datetimeEdit::dateTime()
{
    return QDateTime::fromString(
                QDateTimeEdit::dateTime().toString(this->displayFormat()), this->displayFormat());
}

void lds_datetimeEdit::mousePressEvent(QMouseEvent *event)
{
    if(!this->calendarPopup()) {
        QAbstractSpinBox::mousePressEvent(event);
        return;
    }

    QStyleOptionComboBox optCombo;
    optCombo.init(this);
    optCombo.editable = true;
    optCombo.subControls = QStyle::SC_All;
    if(QStyle::SC_ComboBoxArrow == this->style()->hitTestComplexControl(QStyle::CC_ComboBox, &optCombo, event->pos(), this)) {
        if(calendar == 0) {
            calendar = new lds_calendarwidget(this, this);
            calendar->updat_geometry();
            calendar->setWindowFlags(Qt::Popup);
            calendar->move(lds_popup_pos::getPopPoint(this, calendar));
        } else {
            if(is_pop_adjust_postion) {
                calendar->move(lds_popup_pos::getPopPoint(this, calendar));
            }
        }
        calendar->setDateTime(dateTime());
        calendar->resetFocus();
        calendar->show();
    }
}

void lds_datetimeEdit::paintEvent(QPaintEvent *event)
{
    if(!this->calendarPopup()) {
        QAbstractSpinBox::paintEvent(event);
        return;
    }
    QStyleOptionSpinBox opt;
    initStyleOption(&opt);

    QStyleOptionComboBox optCombo;
    optCombo.init(this);
    optCombo.editable = true;
    optCombo.frame = opt.frame;
    optCombo.subControls = opt.subControls;
    optCombo.activeSubControls = opt.activeSubControls;
    optCombo.state = opt.state;

    QPainter p(this);
    style()->drawComplexControl(QStyle::CC_ComboBox, &optCombo, &p, this);
}
