#ifndef W_RPT_REPORT_FRAME_PERIODBUSINESS_H
#define W_RPT_REPORT_FRAME_PERIODBUSINESS_H

#include <QWidget>
#include "w_rpt_report_frame.h"

namespace Ui {
class w_rpt_report_frame_periodbusiness;
}

class w_rpt_report_frame_periodbusiness : public w_rpt_report_frame
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_periodbusiness(QWidget *parent = 0);
    ~w_rpt_report_frame_periodbusiness();
private slots:
    void toselect();
private:
    Ui::w_rpt_report_frame_periodbusiness *ui_help;
};

#endif // W_RPT_REPORT_FRAME_PERIODBUSINESS_H
