#include "w_rpt_report_frame_saleflow.h"
#include "w_rpt_report_frame_saleflow.h"
#include "ui_w_rpt_report_frame_saleflow.h"
#include "ui_w_rpt_report_frame.h"
#include "n_func.h"
#include "backheader.h"
#include <QtDebug>
#include "lds_model_sqltablemodel_delegate_com.h"
#include <QTableWidget>
#include <QScrollBar>
#include "lds_tableview.h"
#include "fexpandmain_model_sqltablemodel_data.h"
#include "fexpandmain_delegate_vch_printmemo.h"

w_rpt_report_frame_saleflow::w_rpt_report_frame_saleflow(QWidget *parent) :
    w_rpt_report_frame(QStringList() << QObject::tr("流水"), parent)
{
    ui_help = new Ui::w_rpt_report_frame_saleflow;
    ui_help->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->widget->setup(QStringList() << QObject::tr("查询")<< QObject::tr("导出")<< QObject::tr("打印")<< QObject::tr("退出"));
    //
    connect(ui->widget->index_widget(QObject::tr("查询")), SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(QObject::tr("导出")), SIGNAL(clicked()),this,SLOT(toexport()));
    connect(ui->widget->index_widget(QObject::tr("打印")), SIGNAL(clicked()),this,SLOT(toprint()));
}

w_rpt_report_frame_saleflow::~w_rpt_report_frame_saleflow()
{
    delete ui_help;
}

void w_rpt_report_frame_saleflow::toselect()
{
    pushbutton_auto_enable dd(ui->widget->index_widget(QObject::tr("查询")));

    lds_query_hddpos  query;
    int index = ui->stackedWidget->currentIndex();
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->widget(index));
    report_querymodel *model = static_cast<report_querymodel *>(tableview->model());
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
            "     cey_u_orderdish a, "
            "     cey_u_checkout_master b, "
            "     cey_bt_dish c "
            " Where "
            "     (a.ch_dishno = c.ch_dishno) "
            "         and (a.ch_payno = b.ch_payno) "
            "         and (b.ch_state = 'Y') "
            "         and b.dt_operdate >= '%1' "
            "         and b.dt_operdate <= '%2' "
            ;
    query.execSelect(QString("select count(0) "+sql_where)
               //开始时间、结束时间、会员3、卡状态4
               .arg(ui->dateTimeEdit->dateTimeStr())
               .arg(ui->dateTimeEdit_2->dateTimeStr())
               );
    query.next();
    int rowcount = query.recordValue(0).toInt();
    ui->pushButton_step->init(index, rowcount, onepagestep,
                              QString(
                                  " SELECT  "
                                  "     a.ch_payno, "
                                  "     c.vch_dishname, "
                                  "     a.num_num - a.num_back as num_dish,"
                                  "     a.num_price, "
                                  "     if(a.ch_presentflag = 'Y', 0, (a.num_num - a.num_back) * a.num_price ) as num_total,"
                                  "     if(a.ch_presentflag = 'Y', 0, (a.num_num - a.num_back) * a.num_price * 0.01 * a.int_discount) as num_total2,"
                                  "     a.num_price_add, "
                                  "     if(a.ch_presentflag = 'Y', 0, (a.num_num - a.num_back) * a.num_price * 0.01 * a.int_discount) + a.num_price_add as num_total3,"
                                  "     a.ch_dishno, "
                                  "     a.vch_print_memo, "
                                  "     a.num_num, "
                                  "     a.num_back, "
                                  "     a.vch_back_operID, "
                                  "     a.dt_back_operdate, "
                                  "     a.int_discount, "
                                  "     a.vch_dis_operID, "
                                  "     a.dt_dis_operdate, "
                                  "     a.ch_presentflag, "
                                  "     a.vch_pre_operID, "
                                  "     a.dt_pre_operdate, "
                                  "     a.ch_suitflag, "
                                  "     a.ch_specialflag, "
                                  "     a.ch_tableno, "
                                  "     a.vch_operID, "
                                  "     a.dt_operdate  "
                                  + sql_where
                                  + " order by b.ch_payno desc ")
                              //开始时间、结束时间、会员、卡状态
                              .arg(ui->dateTimeEdit->dateTimeStr())
                              .arg(ui->dateTimeEdit_2->dateTimeStr())
                              );
    //
    toselect_page();
    if(tableview->itemDelegateForColumn(model->fieldIndex("vch_print_memo")) == 0) {
        tableview->setItemDelegateForColumn(model->fieldIndex("vch_print_memo"), new fexpandmain_delegate_vch_printmemo(this));
    }
    //
    ui->pushButton_step->sum_clear(index);
    ui->pushButton_step->sum_append(index,  "num_dish");
    ui->pushButton_step->sum_append(index,  "num_total");
    ui->pushButton_step->sum_append(index,  "num_total2");
    ui->pushButton_step->sum_append(index,  "num_price_add");
    ui->pushButton_step->sum_append(index,  "num_total3");
    ui->pushButton_step->sum_genrate(index);

    sum_filldata(ui->pushButton_step->get_sum_list_total_data(index));
    resize_column_to_content_t();
}

void w_rpt_report_frame_saleflow::toprint()
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

void w_rpt_report_frame_saleflow::select_page_after()
{

    tableview_setpan("ch_payno", QStringList()
                     << "ch_payno"
                     );
}
