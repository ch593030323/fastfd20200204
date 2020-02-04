#ifndef W_RPT_REPORT_FRAME_DISHPROMOTION_H
#define W_RPT_REPORT_FRAME_DISHPROMOTION_H

#include <QWidget>
#include "w_rpt_report_frame.h"

namespace Ui {
class w_rpt_report_frame_dishpromotion;
}

class w_rpt_report_frame_dishpromotion : public w_rpt_report_frame
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_dishpromotion(QWidget *parent = 0);
     ~w_rpt_report_frame_dishpromotion();
private slots:
    void toselect();

private:
    Ui::w_rpt_report_frame_dishpromotion *ui_help;
};

#endif // W_RPT_REPORT_FRAME_DISHPROMOTION_H
