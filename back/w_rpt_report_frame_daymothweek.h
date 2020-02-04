#ifndef W_RPT_REPORT_FRAME_DAYMOTHWEEK_H
#define W_RPT_REPORT_FRAME_DAYMOTHWEEK_H

#include <QWidget>
#include "w_rpt_report_frame.h"
namespace Ui {
class w_rpt_report_frame_daymothweek;
}

class w_rpt_report_frame_daymothweek : public w_rpt_report_frame
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_daymothweek(QWidget *parent = 0);
     ~w_rpt_report_frame_daymothweek();
private slots:
    void toselect();

private:
    Ui::w_rpt_report_frame_daymothweek *ui_help;
};

#endif // W_RPT_REPORT_FRAME_DAYMOTHWEEK_H
