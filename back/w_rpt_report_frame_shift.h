#ifndef W_RPT_REPORT_FRAME_SHIFT_H
#define W_RPT_REPORT_FRAME_SHIFT_H

#include <QWidget>
#include "w_rpt_report_frame.h"

namespace Ui {
class w_rpt_report_frame_shift;
}

class w_rpt_report_frame_shift : public w_rpt_report_frame
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_shift(QWidget *parent = 0);
     ~w_rpt_report_frame_shift();

public slots:
    void toselect();

    void toreprint();

    void toprint();
protected:
    //from w_rpt_report_frame
     void select_page_after();
private:
    Ui::w_rpt_report_frame_shift *ui_help;
};

#endif // W_RPT_REPORT_FRAME_SHIFT_H
