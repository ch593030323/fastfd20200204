#include "w_rpt_report_frame_tax.h"
#include "ui_w_rpt_report_frame_tax.h"
#include "ui_w_rpt_report_frame.h"
#include "n_func.h"
#include "backheader.h"
#include <QtDebug>
#include "lds_model_sqltablemodel_delegate_com.h"
#include <QTableWidget>
#include <QScrollBar>
#include "w_rpt_report_dataexport.h"
#include "w_rpt_report_printformat.h"
#include "lds_tableview.h"
#include "lds_menu.h"

w_rpt_report_frame_tax::w_rpt_report_frame_tax(QWidget *parent) :
    w_rpt_report_frame(QStringList() << QObject::tr("销售")  << QObject::tr("采购") << QObject::tr("汇总") ,  parent)
{
    ui_help = new Ui::w_rpt_report_frame_tax;
    ui_help->setupUi(ui->frame_other);
    resize(lds::MAIN_WINDOW_SIZE);
    lds::hideWidget(ui->pushButton_step, true);

    ui->widget->setup(QStringList() << QObject::tr("查询")<< QObject::tr("导出")<< QObject::tr("打印")<< QObject::tr("退出"));
    connect(ui->widget->index_widget(QObject::tr("查询")), SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(QObject::tr("导出")), SIGNAL(clicked()),this,SLOT(toexport()));
    connect(ui->widget->index_widget(QObject::tr("打印")), SIGNAL(clicked()),this,SLOT(toprint()));
}

w_rpt_report_frame_tax::~w_rpt_report_frame_tax()
{
    delete ui_help;
}

void w_rpt_report_frame_tax::toselect()
{
    pushbutton_auto_enable dd(ui->widget->index_widget(QObject::tr("查询")));
    int index = ui->stackedWidget->currentIndex();
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->widget(index));
    report_querymodel *model = static_cast<report_querymodel *>(tableview->model());
    static int onepagestep = tableviewPageCells(tableview);
    lds_query_hddpos query;

    ui->pushButton_step->init(index);
    switch(index) {//收银流水
    case 0://销售
    {
        QString vch_tax_number = lds_query_hddpos::selectValue(QString(" select vch_tax_number from cey_sys_company")).toString();
        QString sql =
                " SELECT "
                + QString(" '%1'  as vch_tax_number,").arg(vch_tax_number) +
                " ch_payno,"
                " num_cash - num_rate as tx_cost,"//商品金额
                " num_rate as tx_tax,"//税额
                " num_cash as tx_cash,"//含税金额
                " dt_operdate"
                " FROM cey_u_checkout_master"
                " where ch_state = 'Y' "
                "and dt_operdate >= '%1' and dt_operdate <= '%2'  "
                ;
        sql = sql.arg(ui->dateTimeEdit->dateTimeStr()).arg(ui->dateTimeEdit_2->dateTimeStr());
        model->setQuery(sql);
        ui->pushButton_step->init(index, 0, onepagestep, sql);
        //
        ui->pushButton_step->sum_clear(index);
        ui->pushButton_step->sum_append(index,  "tx_cost");
        ui->pushButton_step->sum_append(index,  "tx_tax");
        ui->pushButton_step->sum_append(index,  "tx_cash");
        ui->pushButton_step->sum_genrate(index);
    }
        break;
    case 1://采购
    {
        QString sql =
                " select "
                " a.ch_providerNo,"
                " a.vch_providerName,"
                " a.vch_tax_number,"
                " sum(b.num_cost) as tx_cost,"//商品金额
                " sum(b.num_tax) as tx_tax,"//税额
                " sum(b.num_cash) as tx_cash "//含税金额
                " from cey_st_provider a, cey_st_provider_pay b"
                " where"
                " a.ch_providerNo = b.ch_providerNo "
                "and b.dt_operdate >= '%1' and b.dt_operdate <= '%2'  "
                " group by a.ch_providerNo;"
                ;
        sql = sql.arg(ui->dateTimeEdit->dateTimeStr()).arg(ui->dateTimeEdit_2->dateTimeStr());

        model->setQuery(sql);
        ui->pushButton_step->init(index, 0, onepagestep, sql);
        ui->pushButton_step->sum_clear(index);
        ui->pushButton_step->sum_append(index,  "tx_cost");
        ui->pushButton_step->sum_append(index,  "tx_tax");
        ui->pushButton_step->sum_append(index,  "tx_cash");
        ui->pushButton_step->sum_genrate(index);
    }
        break;
    case 2://汇总
    {
        query.execSelect(
                    QString(" select sum(num_cash), sum(num_rate) from cey_u_checkout_master where ch_state = 'Y'  "
                            "and dt_operdate >= '%1' and dt_operdate <= '%2'  ")
                    .arg(ui->dateTimeEdit->dateTimeStr()).arg(ui->dateTimeEdit_2->dateTimeStr())
                    );
        query.next();
        qDebug() << query.lastQuery();
        double sale_tx_cost = query.recordValue(0).toDouble();
        double sale_tx_tax = query.recordValue(1).toDouble();

        query.execSelect(
                    QString(" select sum(num_cash), sum(num_tax) from cey_st_provider_pay  "
                            " where dt_operdate >= '%1' and dt_operdate <= '%2'  ")
                    .arg(ui->dateTimeEdit->dateTimeStr()).arg(ui->dateTimeEdit_2->dateTimeStr())
                    );
        query.next();
        double invt_tx_cost = query.recordValue(0).toDouble();
        double invt_tx_tax = query.recordValue(1).toDouble();

        QString sql =
                " select "
                " %1 as sale_tx_cost,  %2 as sale_tx_tax,"
                " %3 as invt_tx_cost,  %4 as invt_tx_tax,  %5 as invt_tx_net"
                ;
        sql = sql
                .arg(sale_tx_cost).arg(sale_tx_tax)
                .arg(invt_tx_cost).arg(invt_tx_tax).arg(sale_tx_tax - invt_tx_tax );

        model->setQuery(sql);
        ui->pushButton_step->init(index, 0, onepagestep, sql);}
        break;
    }

    sum_filldata(ui->pushButton_step->get_sum_list_total_data(index));
    tableview->resizeColumnsToContentsDelay();
}

void w_rpt_report_frame_tax::toexport()
{
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->currentWidget());
    w_rpt_report_dataExport dialog(tableview, ui->tableView_sum, this);
    dialog.setWindowTitle(this->windowTitle());
    dialog.defFilepath(this->windowTitle());
    transparentCenterDialog(&dialog).exec();
}

void w_rpt_report_frame_tax::toprint()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());

    int index = ui->stackedWidget->currentIndex();
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->widget(index));

    w_rpt_report_printFormat *dialog
            = new w_rpt_report_printFormat(w_rpt_report_printFormat::show_model_quick, this->windowTitle(), lds::gs_operid, n_func::f_get_sysdatetime_str(), QString("%1_%2").arg(this->metaObject()->className()).arg(index),
                                           ui->dateTimeEdit->dateTimeStr(), ui->dateTimeEdit_2->dateTimeStr(),
                                           tableview, ui->tableView_sum,
                                           0 );
    dialog->pop(b, this);
}
