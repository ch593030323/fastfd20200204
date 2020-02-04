#ifndef W_RPT_REPORT_FRAME_DAYBUSINESS_H
#define W_RPT_REPORT_FRAME_DAYBUSINESS_H

#include <QWidget>
#include "w_rpt_report_frame.h"

namespace Ui {
class w_rpt_report_frame_daybusiness;
}

class w_rpt_report_frame_daybusiness : public w_rpt_report_frame
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_daybusiness(QWidget *parent = 0);
     ~w_rpt_report_frame_daybusiness();
private slots:
    void toselect();
private:
    Ui::w_rpt_report_frame_daybusiness *ui_help;
};

#endif // W_RPT_REPORT_FRAME_DAYBUSINESS_H
