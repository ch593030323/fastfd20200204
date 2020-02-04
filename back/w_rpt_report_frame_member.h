#ifndef W_RPT_REPORT_FRAME_MEMBER_H
#define W_RPT_REPORT_FRAME_MEMBER_H

#include <QWidget>
#include "w_rpt_report_frame.h"

namespace Ui {
class w_rpt_report_frame_member;
}

class w_rpt_report_frame_member : public w_rpt_report_frame
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_member(QWidget *parent = 0);
     ~w_rpt_report_frame_member();
private slots:
    void toget_m();
    void toselect();
    void todetail();
    void toprint();
private:
    Ui::w_rpt_report_frame_member *ui_help;
};

#endif // W_RPT_REPORT_FRAME_MEMBER_H
