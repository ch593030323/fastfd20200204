#include "w_rpt_report_material_sale.h"
#include "ui_w_rpt_report_material_sale.h"

w_rpt_report_material_sale::w_rpt_report_material_sale(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_rpt_report_material_sale)
{
    ui->setupUi(this);
}

w_rpt_report_material_sale::~w_rpt_report_material_sale()
{
    delete ui;
}
