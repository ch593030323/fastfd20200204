#ifndef W_RPT_REPORT_FRAME_CATEGORYBUSINESS_H
#define W_RPT_REPORT_FRAME_CATEGORYBUSINESS_H

#include <QWidget>
#include "w_rpt_report_frame.h"

namespace Ui {
class w_rpt_report_frame_categorybusiness;
}

class w_rpt_report_frame_categorybusiness : public w_rpt_report_frame
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_categorybusiness(QWidget *parent = 0);
    ~w_rpt_report_frame_categorybusiness();
private slots:
    void toselect();
private:
    Ui::w_rpt_report_frame_categorybusiness *ui_help;
};

#endif // W_RPT_REPORT_FRAME_CATEGORYBUSINESS_H
