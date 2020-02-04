#ifndef DOUBLEWIDGET_SCALE_H
#define DOUBLEWIDGET_SCALE_H

#include "doublewidget.h"
class public_scale_led;
class QTableWidget;
class QLabel;

class doublewidget_scale : public doublewidget
{
    Q_OBJECT
public:
    explicit doublewidget_scale(QSize dsize ,QWidget *parent = 0);

    void doublewidget_layout();

protected slots:
    void adc_timer_start();
    void adc_timer_out();
    void standmodel_setdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4);
    void standmodel_insetdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4);
    void qr_code_adv_hold();

    void scale_update(const QString &keys, const QString titles, const QString &suffixs);
    void scale_update_numbers(const QString &numbers);
private:
    public_scale_led *widget_scale_led;
};

#endif // DOUBLEWIDGET_SCALE_H
