#ifndef W_RPT_REPORT_FRAME_BUSINESSDATA_H
#define W_RPT_REPORT_FRAME_BUSINESSDATA_H

#include <QWidget>
#include "w_rpt_report_frame.h"

namespace Ui {
class w_rpt_report_frame_businessdata;
}

class w_rpt_report_frame_businessdata : public w_rpt_report_frame
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_businessdata(QWidget *parent = 0);
     ~w_rpt_report_frame_businessdata();
private slots:
    void toselect();
private:
    Ui::w_rpt_report_frame_businessdata *ui_help;
};

#endif // W_RPT_REPORT_FRAME_BUSINESSDATA_H
