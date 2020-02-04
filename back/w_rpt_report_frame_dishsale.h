#ifndef W_RPT_REPORT_FRAME_DISHSALE_H
#define W_RPT_REPORT_FRAME_DISHSALE_H

#include <QWidget>
#include "w_rpt_report_frame.h"

namespace Ui {
class w_rpt_report_frame_dishsale;
}

class w_rpt_report_frame_dishsale : public w_rpt_report_frame
{
    Q_OBJECT
public:
    explicit w_rpt_report_frame_dishsale(QWidget *parent = 0);
     ~w_rpt_report_frame_dishsale();
private slots:
    void toselect();
    void toupdate_group2(int index);
public slots:
    virtual void toexport();
    virtual void toprint();
private:
    Ui::w_rpt_report_frame_dishsale *ui_help;
    QString sqlarg2;
    QString sqltable1;
    QString sqltable2;
    QString sqlfilter;
};

#endif // W_RPT_REPORT_FRAME_DISHSALE_H
