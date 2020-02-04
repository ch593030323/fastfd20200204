#ifndef DOUBLEWIDGET_1024X600_H
#define DOUBLEWIDGET_1024X600_H

#include "doublewidget.h"

class doublewidget_1024x600 : public doublewidget
{
    Q_OBJECT
public:
    doublewidget_1024x600(QSize dsize ,QWidget *parent = 0);

    void doublewidget_layout();

protected slots:
    void adc_timer_start();
    void adc_timer_out();
    void standmodel_setdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4);
    void standmodel_insetdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4);
    void qr_code_adv_hold();

    void total_settext(const QString &text);
    void num_settext(const QString &text);
    bool hlabels_will_update();
private:
    QString blank;

};

#endif // DOUBLEWIDGET_1024X600_H
