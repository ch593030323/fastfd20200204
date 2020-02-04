#include "public_pushbutton_pay.h"
#include <QStylePainter>
#include <QStyleOptionButton>
#include "lds_scr_table.h"
#include "lds.h"
#include <QtDebug>
#include <QElapsedTimer>

public_pushbutton_pay::public_pushbutton_pay(QWidget *parent) :
    lds_pushbutton_wrap(parent)
{
    QString c0 = lds::static_color_light(
                lds_scr_table::gettableState(tableState_KAITAI).colorname1,
                0).name();
    QString c3 = lds::static_color_light(
                lds_scr_table::gettableState(tableState_KAITAI).colorname1,
                3).name();


    QString dis_c0 = lds::static_color_light(
                lds_scr_table::gettableState(tableState_DISABLE).colorname1,
                0).name();
    QString dis_c3 = lds::static_color_light(
                lds_scr_table::gettableState(tableState_DISABLE).colorname1,
                3).name();

    setStyleSheet(
                QString(
                    "QPushButton{"
                    "background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 %1, stop:1 %2);"
                    "color:white;border:1px solid %2;padding-top:3px;padding-left:3px;}"
                    "QPushButton:pressed{color:transparent;background-color:black;border:0px;}"


                    "QPushButton:disabled{"
                    "background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 %3, stop:1 %4);"
                    "color:white;border:1px solid %4;padding-top:3px;padding-left:3px;}"
                    )
                .arg(c3)
                .arg(c0)
                .arg(dis_c3)
                .arg(dis_c0)
                );
}

void public_pushbutton_pay::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    QString t;
    QRect r;
    QStyleOptionButton o;
    initStyleOption(&o);
    t = o.text;
    r = o.rect;

    p.drawControl(QStyle::CE_PushButton, o);
}
