#ifndef W_RPT_REPORT_FRAME_DISHQUIT_H
#define W_RPT_REPORT_FRAME_DISHQUIT_H

#include <QWidget>
#include "w_rpt_report_frame.h"
namespace Ui {
class w_rpt_report_frame_dishquit;
}

class w_rpt_report_frame_dishquit : public w_rpt_report_frame
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_dishquit(QWidget *parent = 0);
    ~w_rpt_report_frame_dishquit();
public slots:
    void toselect();
    void toprint();

private:
    Ui::w_rpt_report_frame_dishquit *ui_help;
};

#endif // W_RPT_REPORT_FRAME_DISHQUIT_H
