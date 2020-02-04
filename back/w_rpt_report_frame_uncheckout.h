#ifndef W_RPT_REPORT_FRAME_UNCHECKOUT_H
#define W_RPT_REPORT_FRAME_UNCHECKOUT_H

#include <QWidget>
#include "w_rpt_report_frame.h"

namespace Ui {
class w_rpt_report_frame_uncheckout;
}

class w_rpt_report_frame_uncheckout : public w_rpt_report_frame
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_uncheckout(QWidget *parent = 0);
    ~w_rpt_report_frame_uncheckout();
private slots:
    void toselect();

private:
    Ui::w_rpt_report_frame_uncheckout *ui_help;
};

#endif // W_RPT_REPORT_FRAME_UNCHECKOUT_H
