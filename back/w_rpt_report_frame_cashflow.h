#ifndef W_RPT_REPORT_FRAME_CASHFLOW_H
#define W_RPT_REPORT_FRAME_CASHFLOW_H

#include <QWidget>
#include "w_rpt_report_frame.h"

namespace Ui {
class w_rpt_report_frame_cashflow;
}

class w_rpt_report_frame_cashflow : public w_rpt_report_frame
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_cashflow(QWidget *parent = 0);
     ~w_rpt_report_frame_cashflow();
public slots:
    void toselect();
    void toprint();

protected:
    virtual void select_page_after();
private:
    Ui::w_rpt_report_frame_cashflow *ui_help;
};

#endif // W_RPT_REPORT_FRAME_CASHFLOW_H
