#ifndef W_RPT_REPORT_FRAME_DIRECTSALEGROSS_H
#define W_RPT_REPORT_FRAME_DIRECTSALEGROSS_H

#include <QWidget>
#include "w_rpt_report_frame.h"

namespace Ui {
class w_rpt_report_frame_directsalegross;
}

class w_rpt_report_frame_directsalegross : public w_rpt_report_frame
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_directsalegross(QWidget *parent = 0);
     ~w_rpt_report_frame_directsalegross();
private slots:
    void toselect();

private:
    Ui::w_rpt_report_frame_directsalegross *ui_help;
};

#endif // W_RPT_REPORT_FRAME_DIRECTSALEGROSS_H
