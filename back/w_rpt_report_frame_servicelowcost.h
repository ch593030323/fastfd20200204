#ifndef W_RPT_REPORT_FRAME_SERVICELOWCOST_H
#define W_RPT_REPORT_FRAME_SERVICELOWCOST_H

#include <QWidget>
#include "w_rpt_report_frame.h"

namespace Ui {
class w_rpt_report_frame_servicelowcost;
}

class w_rpt_report_frame_servicelowcost : public w_rpt_report_frame
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_servicelowcost(QWidget *parent = 0);
    ~w_rpt_report_frame_servicelowcost();
private slots:
    void toselect();

private:
    Ui::w_rpt_report_frame_servicelowcost *ui_help;
};

#endif // W_RPT_REPORT_FRAME_SERVICELOWCOST_H
