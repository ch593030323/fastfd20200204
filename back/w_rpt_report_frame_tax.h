#ifndef W_RPT_REPORT_FRAME_TAX_H
#define W_RPT_REPORT_FRAME_TAX_H

#include <QDialog>
#include "w_rpt_report_frame.h"

namespace Ui {
class w_rpt_report_frame_tax;
}

class w_rpt_report_frame_tax : public w_rpt_report_frame
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_tax(QWidget *parent = 0);
    ~w_rpt_report_frame_tax();

private slots:
    void toselect();
public slots:
    virtual void toexport();
    virtual void toprint();
private:
    Ui::w_rpt_report_frame_tax *ui_help;
};

#endif // W_RPT_REPORT_FRAME_TAX_H
