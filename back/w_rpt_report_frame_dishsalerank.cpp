#include "w_rpt_report_frame_dishsalerank.h"
#include "ui_w_rpt_report_frame_dishsalerank.h"
#include "ui_w_rpt_report_frame.h"
#include "n_func.h"
#include "backheader.h"
#include <QtDebug>
#include "lds_model_sqltablemodel_delegate_com.h"
#include <QTableWidget>
#include <QScrollBar>

w_rpt_report_frame_dishsalerank::w_rpt_report_frame_dishsalerank(QWidget *parent) :
    w_rpt_report_frame(QStringList() << QObject::tr("流水"), parent)
{
    ui_help = new Ui::w_rpt_report_frame_dishsalerank;
    ui_help->setupUi(ui->frame_other);
    resize(lds::MAIN_WINDOW_SIZE);

    ui_help->comboBox_group->addItem(QObject::tr("按商品明细"), "a.ch_dishno, vch_dishname,  cey_bt_dish a");
    ui_help->comboBox_group->addItem(QObject::tr("按商品类型"), "c.ch_dish_typeno, vch_dish_typename,  cey_bt_dish_type c");
    ui_help->comboBox_group->addItem(QObject::tr("按小类"), "c.ch_sub_typeno, vch_sub_typename,  cey_bt_dish_subtype c");
    ui_help->comboBox_group->addItem(QObject::tr("按餐桌号"), "d.ch_tableno, vch_tablename, cey_bt_table d");
    ui_help->comboBox_group->addItem(QObject::tr("按餐桌类型"), "e.ch_typeno, vch_typename, cey_bt_table_type e");

    ui->widget->setup(QStringList() << QObject::tr("查询")<< QObject::tr("导出")<< QObject::tr("打印")<< QObject::tr("退出"));
    //
    connect(ui->widget->index_widget(QObject::tr("查询")), SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(QObject::tr("导出")), SIGNAL(clicked()),this,SLOT(toexport()));
    connect(ui->widget->index_widget(QObject::tr("打印")), SIGNAL(clicked()),this,SLOT(toprint()));


    disconnect(ui->pushButton_up, SIGNAL(clicked()),this, SLOT(toup()));
    disconnect(ui->pushButton_down, SIGNAL(clicked()),this, SLOT(todown()));
    connect(ui->pushButton_up, SIGNAL(clicked()),this,SLOT(toPageUp()));
    connect(ui->pushButton_down, SIGNAL(clicked()),this,SLOT(toPageDown()));

    ui->pushButton_step->hide();
    ui->tableView_sum->hide();
}

w_rpt_report_frame_dishsalerank::~w_rpt_report_frame_dishsalerank()
{
    delete ui_help;
}

void w_rpt_report_frame_dishsalerank::toselect()
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

    QStringList filter_key_list = ui_help->comboBox_group->itemData(ui_help->comboBox_group->currentIndex()).toString().split(",", QString::SkipEmptyParts);
    QString filter_key = filter_key_list.value(0);
    sql_keyname = filter_key_list.value(0);
    sql_valuename = filter_key_list.value(1);
    tablename = filter_key_list.value(2);

    QString sql_where =
            QString(
                " from cey_u_orderdish a, cey_u_checkout_master b, cey_bt_dish c, cey_u_table d, "
                " (select  ch_typeno, ch_tableno  from  cey_bt_table union all select 'Q0' as ch_typeno, 'Q000' as ch_tableno) e"
                " where  a.ch_payno = b.ch_payno and a.ch_dishno = c.ch_dishno and b.ch_payno = d.ch_payno and a.ch_tableno = e.ch_tableno"
                " and b.ch_state = 'Y'"
                " and b.dt_operdate >= '%1' "
                " and b.dt_operdate <= '%2' "
                " Group by " + filter_key
                )
            .arg(ui->dateTimeEdit->dateTimeStr())
            .arg(ui->dateTimeEdit_2->dateTimeStr())
            ;
    model->setQuery(
                " SELECT  "+filter_key+
                ","
                " sum(a.num_num - a.num_back) as num_dish,"
                " sum((a.num_num - a.num_back) * a.num_price ) as num_total,"
                " sum(if(a.ch_presentflag = 'Y',  0,  (a.num_num - a.num_back) * a.num_price * 0.01 * a.int_discount)) as num_total2,"
                " sum(a.num_price_add) as num_price_add,"
                " sum(if(a.ch_presentflag = 'Y',  0,  (a.num_num - a.num_back) * a.num_price * 0.01 * a.int_discount) + a.num_price_add) as num_total3"

                + sql_where
                + " order by num_dish desc ");
    if(tableview->itemDelegateForColumn(0) == 0) {
        lds_model_sqltablemodel_delegate_com *d = new lds_model_sqltablemodel_delegate_com(this, tablename, sql_keyname, sql_valuename);
        d->enable_empty_return_key();
        tableview->setItemDelegateForColumn(0, d);
    } else {
        lds_model_sqltablemodel_delegate_com *d = static_cast<lds_model_sqltablemodel_delegate_com *>(tableview->itemDelegateForColumn(0));
        d->update_show_value(tablename, sql_keyname, sql_valuename);
    }
    //汇总
    ui->pushButton_step->init(index, 0, onepagestep, "");

    tableview->resizeColumnsToContents();
}

void w_rpt_report_frame_dishsalerank::toexport()
{
    exportModel();
}

void w_rpt_report_frame_dishsalerank::toprint()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());

    int index = ui->stackedWidget->currentIndex();
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->widget(index));

    w_rpt_report_printFormat *dialog
            = new w_rpt_report_printFormat(w_rpt_report_printFormat::show_model_quick,  this->windowTitle(),
                lds::gs_operid,
                n_func::f_get_sysdatetime_str(),
                QString("%1_%2").arg(this->metaObject()->className()).arg(index),
                ui->dateTimeEdit->dateTimeStr(),
                ui->dateTimeEdit_2->dateTimeStr(),
                tableview,
                ui->tableView_sum,
                0 );
    dialog->pop(b, this);
}
