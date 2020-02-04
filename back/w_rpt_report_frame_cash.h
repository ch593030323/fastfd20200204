#ifndef W_RPT_REPORT_FRAME_CASH_H
#define W_RPT_REPORT_FRAME_CASH_H

#include <QWidget>
#include "w_rpt_report_frame.h"

namespace Ui {
class w_rpt_report_frame_cash;
}

class w_rpt_report_frame_cash : public w_rpt_report_frame
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_cash(QWidget *parent = 0);
     ~w_rpt_report_frame_cash();
private slots:
    void toselect();

    void toupdate_group2(int index);
public slots:
    virtual void toexport();
    virtual void toprint();

private:
    Ui::w_rpt_report_frame_cash *ui_help;
};

#endif // W_RPT_REPORT_FRAME_CASH_H
