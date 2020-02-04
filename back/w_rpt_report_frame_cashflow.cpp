#include "w_rpt_report_frame_cashflow.h"
#include "ui_w_rpt_report_frame_cashflow.h"
#include "ui_w_rpt_report_frame.h"
#include "n_func.h"
#include "backheader.h"
#include <QtDebug>
#include "lds_model_sqltablemodel_delegate_com.h"
#include <QTableWidget>
#include <QScrollBar>

w_rpt_report_frame_cashflow::w_rpt_report_frame_cashflow(QWidget *parent) :
    w_rpt_report_frame(QStringList() << QObject::tr("流水"), parent)
{
    ui_help = new Ui::w_rpt_report_frame_cashflow;
    ui_help->setupUi(ui->frame_other);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->widget->setup(QStringList() << QObject::tr("查询")<< QObject::tr("导出")<< QObject::tr("打印")<< QObject::tr("退出"));
    //
    connect(ui->widget->index_widget(QObject::tr("查询")), SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(QObject::tr("导出")), SIGNAL(clicked()),this,SLOT(toexport()));
    connect(ui->widget->index_widget(QObject::tr("打印")), SIGNAL(clicked()),this,SLOT(toprint()));
}

w_rpt_report_frame_cashflow::~w_rpt_report_frame_cashflow()
{
    delete ui_help;
}

void w_rpt_report_frame_cashflow::toselect()
{
    pushbutton_auto_enable dd(ui->widget->index_widget(QObject::tr("查询")));

    lds_query_hddpos  query;
    int index = ui->stackedWidget->currentIndex();
    QTableView *tableview = static_cast<QTableView *>(ui->stackedWidget->widget(index));
    QString cey_u_checkout_master_ch_state = "Y";
    if(ui_help->checkBox_stateP->isChecked())
        cey_u_checkout_master_ch_state = "P";
    //
    static int onepagestep = -1;
    if(onepagestep == -1) {
        QTableWidget t;
        t.setColumnCount(1);
        onepagestep = ( tableview->height() - tableview->horizontalScrollBar()->height()- t.horizontalHeader()->height() ) / tableview->verticalHeader()->defaultSectionSize();
    }
    //
    ui->pushButton_step->init(index);
    //
    QString sql_where =
            " FROM "
            "     cey_u_checkout_detail a, "
            "     cey_u_checkout_master b "
            " WHERE "
            "     (a.ch_payno = b.ch_payno) "
            "         and (b.ch_state = '%1') "
            "         and b.dt_operdate >= '%2' "
            "         and b.dt_operdate <= '%3' "
            ;
    query.execSelect(QString("select count(0) "+sql_where)
               //开始时间、结束时间、会员3、卡状态4
               .arg(cey_u_checkout_master_ch_state)
               .arg(ui->dateTimeEdit->dateTimeStr())
               .arg(ui->dateTimeEdit_2->dateTimeStr())
               );
    query.next();
    int rowcount = query.recordValue(0).toInt();
    ui->pushButton_step->init(index, rowcount, onepagestep,
                              QString(
                                  " SELECT  "
                                  "     a.ch_payno, "
                                  "     b.ch_billno, "
                                  "     ifnull((select  "
                                  "                     cey_bt_table.vch_tablename "
                                  "                 from "
                                  "                     cey_bt_table "
                                  "                 where "
                                  "                     b.ch_tableno = cey_bt_table.ch_tableno), "
                                  "             'Q000') as vch_tablename,"
                                  "     a.ch_paymodeno, "
                                  "     (select  "
                                  "             cey_bt_paymode.vch_paymodename "
                                  "         from "
                                  "             cey_bt_paymode "
                                  "         where "
                                  "             a.ch_paymodeno = cey_bt_paymode.ch_paymodeno) as vch_paymodename,"
                                  "     a.num_payamount, "
                                  "     a.num_realamount, "
                                  "     a.num_change, "
                                  "     a.vch_voucherno, "
                                  "     a.vch_memo, "
                                  "     b.vch_operID, "
                                  "     b.dt_operdate "
                                  + sql_where
                                  + " order by b.ch_payno desc ")
                              //开始时间、结束时间、会员、卡状态
                              .arg(cey_u_checkout_master_ch_state)
                              .arg(ui->dateTimeEdit->dateTimeStr())
                              .arg(ui->dateTimeEdit_2->dateTimeStr())
                              );
    //
    toselect_page();
    //
    ui->pushButton_step->sum_clear(index);
    ui->pushButton_step->sum_append(index,  "num_payamount");
    ui->pushButton_step->sum_append(index,  "num_realamount");
    ui->pushButton_step->sum_append(index,  "num_change");
    ui->pushButton_step->sum_genrate(index);
    sum_filldata(ui->pushButton_step->get_sum_list_total_data(index));
}

void w_rpt_report_frame_cashflow::toprint()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());

    int index = ui->stackedWidget->currentIndex();
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->widget(index));

    w_rpt_report_printFormat *dialog
            = new w_rpt_report_printFormat(w_rpt_report_printFormat::show_model_quick, this->windowTitle(), lds::gs_operid, n_func::f_get_sysdatetime_str(),
                                    QString("%1_%2").arg(this->metaObject()->className()).arg(index),
                                    ui->dateTimeEdit->dateTimeStr(), ui->dateTimeEdit_2->dateTimeStr(),
                                    tableview, ui->pushButton_step->get_limit_sql(index), ui->tableView_sum,
                                    0);
    dialog->pop(b, this);
}

void w_rpt_report_frame_cashflow::select_page_after()
{
    tableview_setpan("ch_payno", QStringList()
                     << "ch_payno"
                     );
}
