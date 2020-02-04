#include "w_rpt_report_ui.h"
#include "ui_w_rpt_report_ui.h"

w_rpt_report_ui::w_rpt_report_ui(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_rpt_report_ui)
{
    ui->setupUi(this);
}

w_rpt_report_ui::~w_rpt_report_ui()
{
    delete ui;
}
