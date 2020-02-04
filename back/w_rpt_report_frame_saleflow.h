#ifndef W_RPT_REPORT_FRAME_SALEFLOW_H
#define W_RPT_REPORT_FRAME_SALEFLOW_H

#include <QWidget>
#include "w_rpt_report_frame.h"

namespace Ui {
class w_rpt_report_frame_saleflow;
}

class w_rpt_report_frame_saleflow : public w_rpt_report_frame
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_saleflow(QWidget *parent = 0);
    ~w_rpt_report_frame_saleflow();
public slots:
    void toselect();
    void toprint();
protected:
    virtual void select_page_after();

private:
    Ui::w_rpt_report_frame_saleflow *ui_help;
};

#endif // W_RPT_REPORT_FRAME_SALEFLOW_H
