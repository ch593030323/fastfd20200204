#ifndef W_RPT_REPORT_MATERIAL_SALE_H
#define W_RPT_REPORT_MATERIAL_SALE_H

#include <QDialog>

namespace Ui {
class w_rpt_report_material_sale;
}

class w_rpt_report_material_sale : public QDialog
{
    Q_OBJECT

public:
    explicit w_rpt_report_material_sale(QWidget *parent = 0);
    ~w_rpt_report_material_sale();

private:
    Ui::w_rpt_report_material_sale *ui;
};

#endif // W_RPT_REPORT_MATERIAL_SALE_H
