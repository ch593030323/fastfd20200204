#include "w_rpt_report_frame_select.h"
#include "ui_w_rpt_report_frame_select.h"
#include <QTimer>
#include <QSqlRecord>

w_rpt_report_frame_select::w_rpt_report_frame_select(const QString &sql, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_rpt_report_frame_select)
{
    _sql = sql;
    ui->setupUi(this);
    querymodel = new QSqlQueryModel(this);
    ui->tableView->setModel(querymodel);
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView->setSelectionMode(QTableView::SingleSelection);
    QTimer::singleShot(0, this, SLOT(tosetq()));
}

w_rpt_report_frame_select::~w_rpt_report_frame_select()
{
    delete ui;
}

void w_rpt_report_frame_select::took()
{
    retv = "";
    int row = ui->tableView->currentIndex().row();
    if(row >= 0) {
        retv = querymodel->record(row).value(1).toString();
    }
    this->accept();
}

void w_rpt_report_frame_select::tocancel()
{
    this->reject();
}

void w_rpt_report_frame_select::tosetq()
{
    querymodel->setQuery(_sql);
}
