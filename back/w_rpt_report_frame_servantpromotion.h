#ifndef W_RPT_REPORT_FRAME_SERVANTPROMOTION_H
#define W_RPT_REPORT_FRAME_SERVANTPROMOTION_H

#include <QWidget>
#include "w_rpt_report_frame.h"

namespace Ui {
class w_rpt_report_frame_servantpromotion;
}

class w_rpt_report_frame_servantpromotion : public w_rpt_report_frame
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_servantpromotion(QWidget *parent = 0);
    ~w_rpt_report_frame_servantpromotion();
private slots:
    void toselect();

private:
    Ui::w_rpt_report_frame_servantpromotion *ui_help;
};

#endif // W_RPT_REPORT_FRAME_SERVANTPROMOTION_H
