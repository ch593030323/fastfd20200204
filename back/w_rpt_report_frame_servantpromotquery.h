#ifndef W_RPT_REPORT_FRAME_SERVANTPROMOTQUERY_H
#define W_RPT_REPORT_FRAME_SERVANTPROMOTQUERY_H

#include <QDialog>
#include "w_rpt_report_frame.h"

namespace Ui {
class w_rpt_report_frame_servantpromotquery;
}

class w_rpt_report_frame_servantpromotquery : public w_rpt_report_frame
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_servantpromotquery(QWidget *parent = 0);
    ~w_rpt_report_frame_servantpromotquery();


private slots:
    void toselect();
protected:
    virtual void select_page_after();
private:
    Ui::w_rpt_report_frame_servantpromotquery *ui_help;
};

#endif // W_RPT_REPORT_FRAME_SERVANTPROMOTQUERY_H
