#include "w_rpt_report_servicefeelowpinquery.h"
//服务费与低消查询
#include "ui_w_rpt_report_cashreport_dialog.h"
#include "ui_w_rpt_report_servicefeelowpinquery_dialog.h"
#include "n_func.h"
#include "backheader.h"
#include "lds_tableview.h"
#include <QtDebug>
#include "lds_model_sqltablemodel_delegate_com.h"

w_rpt_report_servicefeelowpinquery::w_rpt_report_servicefeelowpinquery(QWidget *parent) :
    QDialog(parent),ui(new Ui_w_rpt_report_servicefeelowpinquery_dialog)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->frame->setFixedWidth(w_rpt_report::frame_left_fixed_width);
    w_rpt_report::reportBeginDtSet(ui->dateTimeEdit);
    ui->dateTimeEdit_2->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(), QTime(23,59,59)));
    ui->comboBox->addItem(QObject::tr("全部"), "%");
    ui->comboBox->addItemsBySql("select ch_typeno, vch_typename from cey_bt_table_type;");
    ui->comboBox_2->addItem(QObject::tr("全部"), "%");
    ui->comboBox_2->addItem(QObject::tr("免服务费"), 1);
    ui->comboBox_2->addItem(QObject::tr("低消"), 2);
    querymodel=new report_querymodel(this);
    querymodel->arg_query =
            "            SELECT "
            "                a.ch_payno, "
            "                c.ch_typeno, "
            "                b.ch_tableno, "
            "                a.ch_type, "
            "                (case a.ch_state when '1' then a.num_amount else 0 end) as `num_get`,"
            "                (case a.ch_state when '1' then 0 else a.num_amount end) as `num_free`,"
            "                a.vch_operID, "
            "                a.dt_operdate"
            "            FROM"
            "                cey_u_service_low a,"
            "                cey_u_checkout_master b,"
            "                cey_bt_table c"
            "            WHERE"
            "                a.ch_payno = b.ch_payno"
            "                    and b.ch_tableno = c.ch_tableno"
            "                    and b.ch_state = 'Y'"
            "                    and b.dt_operdate >= '%1' "
            "                    and b.dt_operdate <= '%2' "
            "                    and c.ch_typeno like '%3' "
            "                    and a.ch_type like '%4' "
            "            order by c.ch_typeno,  b.ch_tableno,  a.ch_payno"
            ;

    ui->tableView->setTransferHheader();
    ui->tableView->setModel(querymodel);

    ui->widget->setup(QStringList()<<QObject::tr("查询") <<QObject::tr("退出"), Qt::AlignTop);
    connect(ui->widget->index_widget(QObject::tr("查询")),SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
}

w_rpt_report_servicefeelowpinquery::~w_rpt_report_servicefeelowpinquery()
{

}

void w_rpt_report_servicefeelowpinquery::toselect()
{
    querymodel->setQuery(querymodel->arg_query
                         .arg(ui->dateTimeEdit->dateTimeStr())
                         .arg(ui->dateTimeEdit_2->dateTimeStr())
                         .arg(ui->comboBox->curusrdata().toString())
                         .arg(ui->comboBox_2->curusrdata().toString())
                         );

    if(ui->tableView->itemDelegateForColumn(querymodel->fieldIndex("ch_tableno")) == 0)
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("ch_tableno"), new lds_model_sqltablemodel_delegate_com(this, "cey_bt_table", "ch_tableno", "vch_tablename"));
    if(ui->tableView->itemDelegateForColumn(querymodel->fieldIndex("ch_typeno")) == 0)
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("ch_typeno"), new lds_model_sqltablemodel_delegate_com(this, ui->comboBox));
    if(ui->tableView->itemDelegateForColumn(querymodel->fieldIndex("ch_type")) == 0)
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("ch_type"), new lds_model_sqltablemodel_delegate_com(this, ui->comboBox_2));
    ui->tableView->updateSumRange(4, 5);
    ui->tableView->resizeColumnsToContents();
}

void w_rpt_report_servicefeelowpinquery::toexit()
{
    this->reject();
}

void w_rpt_report_servicefeelowpinquery::toprint()
{

}
