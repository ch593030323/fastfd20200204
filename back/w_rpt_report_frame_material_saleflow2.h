#ifndef W_RPT_REPORT_FRAME_MATERIAL_SALEFLOW2_H
#define W_RPT_REPORT_FRAME_MATERIAL_SALEFLOW2_H

#include <QDialog>
#include "w_rpt_report_frame_limit.h"
#include "w_rpt_report_frame.h"

namespace Ui {
class w_rpt_report_frame_material_saleflow2;
}

class w_rpt_report_frame_material_saleflow2 : public QDialog
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_material_saleflow2(QWidget *parent = 0);
    ~w_rpt_report_frame_material_saleflow2();

private slots:
    void toselect();
    void toexit();
private:
    Ui::w_rpt_report_frame_material_saleflow2 *ui;
    w_rpt_report_frame_limit select_limit;
    report_querymodel *querymodel;
};

#endif // W_RPT_REPORT_FRAME_MATERIAL_SALEFLOW2_H
