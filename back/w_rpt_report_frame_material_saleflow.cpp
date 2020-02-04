#include "w_rpt_report_frame_material_saleflow.h"
#include "ui_w_rpt_report_frame_material_saleflow.h"
#include "ui_w_rpt_report_frame.h"
#include "n_func.h"
#include "backheader.h"
#include <QtDebug>
#include "lds_model_sqltablemodel_delegate_com.h"
#include <QTableWidget>
#include <QScrollBar>

w_rpt_report_frame_material_saleflow::w_rpt_report_frame_material_saleflow(QWidget *parent) :
    w_rpt_report_frame(QStringList() << QObject::tr("流水") << QObject::tr("利润") , parent)
{

    ui_help = new Ui::w_rpt_report_frame_material_saleflow;
    ui_help->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->widget->setup(QStringList() << QObject::tr("查询")<< QObject::tr("导出")<< QObject::tr("打印")<< QObject::tr("退出"));
    //
    connect(ui->widget->index_widget(QObject::tr("查询")), SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(QObject::tr("导出")), SIGNAL(clicked()),this,SLOT(toexport()));
    connect(ui->widget->index_widget(QObject::tr("打印")), SIGNAL(clicked()),this,SLOT(toprint()));
}

w_rpt_report_frame_material_saleflow::~w_rpt_report_frame_material_saleflow()
{
    delete ui_help;
}

void w_rpt_report_frame_material_saleflow::toselect()
{
    pushbutton_auto_enable dd(ui->widget->index_widget(QObject::tr("查询")));
    lds_query_hddpos  query;
    int index = ui->stackedWidget->currentIndex();
    QTableView *tableview = static_cast<QTableView *>(ui->stackedWidget->widget(index));
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
    QString from_where =
            " FROM"
            "    material_u_orderdish b,"
            "    cey_u_orderdish a,"
            "    cey_u_checkout_master c "
            "       where"
            "       b.int_order_flowID = a.int_flowid"
            "       and a.ch_payno = c.ch_payno"
            "       and c.ch_state = 'Y'"
            "       and a.dt_operdate >= '%1' "
            "       and a.dt_operdate <= '%2' "
            ;
    switch(index) {
    case 0://原料销售流水
    {
        query.execSelect(QString("select count(0) "+ from_where)
                   //开始时间、结束时间、会员3、卡状态4
                   .arg(ui->dateTimeEdit->dateTimeStr())
                   .arg(ui->dateTimeEdit_2->dateTimeStr())
                   );
        query.next();
        int rowcount = query.recordValue(0).toInt();
        ui->pushButton_step->init(index, rowcount, onepagestep,
                                  QString(
                                      "    SELECT"
                                      "          a.ch_payno as `"+QObject::tr("付款单号") + "`,"
                                      "          a.int_flowid as `"+QObject::tr("流水号") + "`,"
                                      "    (select vch_dishname from cey_bt_dish where cey_bt_dish.ch_dishno = a.ch_dishno) as `"+QObject::tr("商品")+"`,"
                                      "        a.num_num - a.num_back  as `"+/*QObject::tr("商品数量")*/"num_dish" + "`,"
                                      "    (select vch_dishname from cey_bt_dish where cey_bt_dish.ch_dishno = b.ch_dishno) as `"+QObject::tr("原料")+"`,"
                                      "        b.num_num - b.num_back  as `"+/*QObject::tr("原料数量")*/"num_material" + "`,"
                                      "        a.dt_operdate  as `"+QObject::tr("下单时间") + "`,"
                                      "           a.vch_operid  as `"+QObject::tr("操作员") +"`"
                                      + from_where
                                      + " order by b.int_order_flowID  desc ")
                                  .arg(ui->dateTimeEdit->dateTimeStr())
                                  .arg(ui->dateTimeEdit_2->dateTimeStr())
                                  );
        //
        toselect_page();
        //
        ui->pushButton_step->sum_clear(index);
        ui->pushButton_step->sum_append(index,  "num_dish");
        ui->pushButton_step->sum_append(index,  "num_material");
        ui->pushButton_step->sum_genrate(index);
    }
        break;
    case 1://原料毛利报表
    {
        query.execSelect(QString("select count(0) "+ from_where)
                   //开始时间、结束时间、会员3、卡状态4
                   .arg(ui->dateTimeEdit->dateTimeStr())
                   .arg(ui->dateTimeEdit_2->dateTimeStr())
                   );
        query.next();
        int rowcount = query.recordValue(0).toInt();

        ui->pushButton_step->init(index, rowcount, onepagestep,
                                  "select b.int_flowid,"
                                  "a.ch_payno,"
                                  "a.int_flowid,"
                                  "a.ch_dishno,"
                                  "a.num_num - a.num_back as num_dish,"
                                  "b.ch_dishno as ch_materialno,"
                                  "b.num_num - b.num_back as num_material,"
                                  "a.dt_operdate,"
                                  "a.vch_operid " + from_where.arg(ui->dateTimeEdit->dateTimeStr()).arg(ui->dateTimeEdit_2->dateTimeStr())
                                  );
        //
        toselect_page();
        //
        ui->pushButton_step->sum_clear(index);
        ui->pushButton_step->sum_append(index,  "num_dish");
        ui->pushButton_step->sum_append(index,  "num_material");
        ui->pushButton_step->sum_genrate(index);
    }
        break;
    }


    sum_filldata(ui->pushButton_step->get_sum_list_total_data(index));
    resize_column_to_content_t();
}

void w_rpt_report_frame_material_saleflow::toprint()
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

void w_rpt_report_frame_material_saleflow::select_page_after()
{
    tableview_setpan(QObject::tr("流水号"), QStringList()
                     << QObject::tr("流水号")
                     << QObject::tr("商品")
                     );
    tableview_setpan(QObject::tr("付款单号"), QStringList()
                     << QObject::tr("付款单号")
                     );
}
