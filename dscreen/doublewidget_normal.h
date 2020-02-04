#ifndef DOUBLEWIDGET_NORMAL_H
#define DOUBLEWIDGET_NORMAL_H

#include "doublewidget.h"

class doublewidget_normal : public doublewidget
{
public:
    doublewidget_normal(QSize dsize ,QWidget *parent = 0);

    void doublewidget_layout();

protected slots:
    void adc_timer_start();
    void adc_timer_out();
    void standmodel_setdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4);
    void standmodel_insetdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4);
    void qr_code_adv_hold();

};

#endif // DOUBLEWIDGET_NORMAL_H
