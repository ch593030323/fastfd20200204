#include "w_rpt_report_frame_dishsale.h"
#include "ui_w_rpt_report_frame_dishsale.h"
#include "ui_w_rpt_report_frame.h"
#include "n_func.h"
#include "backheader.h"
#include <QtDebug>
#include "lds_model_sqltablemodel_delegate_com.h"
#include <QTableWidget>
#include <QScrollBar>
#include "w_rpt_report_dataexport.h"

w_rpt_report_frame_dishsale::w_rpt_report_frame_dishsale(QWidget *parent) :
    w_rpt_report_frame(QStringList() << QObject::tr("流水") << QObject::tr("汇总")  ,  parent)
{
    ui_help = new Ui::w_rpt_report_frame_dishsale;
    ui_help->setupUi(ui->frame_other);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->widget->setup(QStringList() << QObject::tr("查询")<< QObject::tr("导出")<< QObject::tr("打印")<< QObject::tr("退出"));
    ui_help->comboBox_group->addItem(QObject::tr("付款单号"), QStringList() << "b.ch_payno");
    ui_help->comboBox_group->addItem(QObject::tr("操作员"),  QStringList() << "a.vch_operID" << "vch_operator" << "("+backheader::SELECT_OPERID_NAME +")  a");
    ui_help->comboBox_group->addItem(QObject::tr("大类"), QStringList() << "c.ch_dish_typeno" << "vch_dish_typename" << "cey_bt_dish_type c");
    ui_help->comboBox_group->addItem(QObject::tr("小类"), QStringList() << "c.ch_sub_typeno" << "vch_sub_typename" << "cey_bt_dish_subtype c");
    ui_help->comboBox_group->addItem(QObject::tr("餐桌"), QStringList() << "a.ch_tableno" << "vch_tablename" << "cey_bt_table a");
    ui_help->comboBox_group->addItem(QObject::tr("餐桌类型"), QStringList() << "d.ch_typeno" << "vch_typename" << "cey_bt_table_type d");

    ///
    int idx = 0;
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_group_2, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_group_2, 1);
    idx++;
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_group_2, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_group_2, 0);
    ///~
    //1
    sqlarg2 =
            "     sum(a.num_num - a.num_back) as num_dish, "
            "     sum((a.num_num - a.num_back) * a.num_price ) as num_total, "
            "     sum((a.num_num - a.num_back) * a.num_price  * (1 - a.int_discount * 0.01)) as num_discount, "
            "     sum(if(a.ch_presentflag = 'Y',  (a.num_num - a.num_back) * a.num_price,   0)) as num_present, "
            "     sum(if(a.ch_presentflag = 'Y',  0,  (a.num_num - a.num_back) * a.num_price * 0.01 * a.int_discount)) as num_total2, "
            "     sum(a.num_price_add) as num_price_add, "
            "     sum(if(a.ch_presentflag = 'Y',  0,  (a.num_num - a.num_back) * a.num_price * 0.01 * a.int_discount) + a.num_price_add) as num_total3, "
            "     sum((a.num_price - c.num_sale_price) * a.num_num-a.num_back)  as num_profit "
            ;
    sqltable1 =
            "                 cey_u_orderdish a, "
            "                 cey_u_checkout_master b, "
            "                 cey_bt_dish c, "
            "             (select "
            "                 ch_typeno, ch_tableno "
            "             from "
            "                 cey_bt_table union all select 'Q0' as ch_typeno, 'Q000' as ch_tableno) d "
            ;
    sqlfilter =
            "                 (a.ch_tableno = d.ch_tableno and a.ch_payno = b.ch_payno) "
            "                     and (b.ch_state = 'Y') "
            "                     and (a.ch_dishno = c.ch_dishno) "
            "                     and (a.num_num - a.num_back <> 0) "
            "                     and (b.dt_operdate > '%1' "
            "                     and b.dt_operdate <= '%2') "
            ;
    //
    connect(ui->widget->index_widget(QObject::tr("查询")), SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(QObject::tr("导出")), SIGNAL(clicked()),this,SLOT(toexport()));
    connect(ui->widget->index_widget(QObject::tr("打印")), SIGNAL(clicked()),this,SLOT(toprint()));
    //    ui->pushButton_down->hide();
    ui->pushButton_step->hide();
    //    ui->pushButton_up->hide();
    disconnect(ui->pushButton_up, SIGNAL(clicked()),this, SLOT(toup()));
    disconnect(ui->pushButton_down, SIGNAL(clicked()),this, SLOT(todown()));
    connect(ui->pushButton_up, SIGNAL(clicked()),this,SLOT(toPageUp()));
    connect(ui->pushButton_down, SIGNAL(clicked()),this,SLOT(toPageDown()));
    connect(ui_help->comboBox_group, SIGNAL(currentIndexChanged(int)), this,  SLOT(toupdate_group2(int)));
    toupdate_group2(0);
}

w_rpt_report_frame_dishsale::~w_rpt_report_frame_dishsale()
{
    delete ui_help;
}

void w_rpt_report_frame_dishsale::toselect()
{
    pushbutton_auto_enable dd(ui->widget->index_widget(QObject::tr("查询")));

    QStringList group_list = ui_help->comboBox_group->itemData(ui_help->comboBox_group->currentIndex()).toStringList();
    QString group2 = ui_help->comboBox_group_2->itemData(ui_help->comboBox_group_2->currentIndex()).toString();
    int index = ui->stackedWidget->currentIndex();
    QTableView *tableview = static_cast<QTableView *>(ui->stackedWidget->widget(index));
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
    switch(index) {
    case 0:///商品销售明细
    {
        QString sqlarg =
                "select "
                + group_list.value(mapKey) + " as `group_key`,"
                "a.ch_dishno,  "
                "c.vch_dishname,  "
                "c.ch_unitno ,  "
                + sqlarg2+","
                "a.vch_operID, "
                "a.dt_operdate "
                ;
        QString sql_where =
                " FROM "
                + sqltable1 + " where " + sqlfilter ;
        ;
        if(!group2.isEmpty()) {
            sql_where += QString(" and %1 = '%2' ").arg(group_list.value(mapKey)).arg(group2);
        }
        sql_where = sql_where
                .arg(ui->dateTimeEdit->dateTimeStr())
                .arg(ui->dateTimeEdit_2->dateTimeStr())
                ;
        model->setQuery(
                    sqlarg + sql_where +" group by  a.ch_dishno, "+group_list.value(mapKey)+" order by "+group_list.value(mapKey)+" desc "
                    );
        //
        if(tableview->itemDelegateForColumn(model->fieldIndex("ch_unitno")) == 0) {
            tableview->setItemDelegateForColumn(model->fieldIndex("ch_unitno"), new lds_model_sqltablemodel_delegate_com(this, "cey_bt_unit", "ch_unitno", "vch_unitname"));
        }
        if(tableview->itemDelegateForColumn(model->fieldIndex("vch_operid")) == 0) {
            tableview->setItemDelegateForColumn(model->fieldIndex("vch_operid"), new lds_model_sqltablemodel_delegate_com(this, "("+backheader::SELECT_OPERID_NAME +") t", "vch_operID", "vch_operator"));
        }
        if(tableview->itemDelegateForColumn(0) == 0) {
            lds_model_sqltablemodel_delegate_com *d = new lds_model_sqltablemodel_delegate_com(this, ui_help->comboBox_group_2);
            d->enable_empty_return_key();
            tableview->setItemDelegateForColumn(0, d);
        }
        //汇总
        ui->pushButton_step->init(index, 0, onepagestep, sqlarg + sql_where);
    }
        break;///~商品销售明细
    case 1:///商品销售汇总
    {
        QString sqlarg =
                "select "
                + group_list.value(mapKey)+ ","
                + sqlarg2
                ;
        QString sql_where =
                " FROM "
                + sqltable1 + " where " + sqlfilter ;
        sql_where = sql_where
                .arg(ui->dateTimeEdit->dateTimeStr())
                .arg(ui->dateTimeEdit_2->dateTimeStr())
                ;
        model->setQuery(
                    sqlarg + sql_where +" group by "+group_list.value(mapKey)+" order by "+group_list.value(mapKey)+" desc "
                    );
        //
        if(tableview->itemDelegateForColumn(0) == 0) {
            lds_model_sqltablemodel_delegate_com *d = new lds_model_sqltablemodel_delegate_com(this, ui_help->comboBox_group_2);
            d->enable_empty_return_key();
            tableview->setItemDelegateForColumn(0, d);
        }
        ui->pushButton_step->init(index, 0, onepagestep, sqlarg + sql_where);
    }
        break;///~商品销售汇总
    }
    //
    if(tableview->itemDelegateForColumn(0) == 0) {
        lds_model_sqltablemodel_delegate_com *d = new lds_model_sqltablemodel_delegate_com(this, ui_help->comboBox_group_2);
        d->enable_empty_return_key();
        tableview->setItemDelegateForColumn(0, d);
    }
    ui->pushButton_step->sum_clear(index);
    ui->pushButton_step->sum_append(index,  "num_total");
    ui->pushButton_step->sum_append(index,  "num_discount");
    ui->pushButton_step->sum_append(index,  "num_present");
    ui->pushButton_step->sum_append(index,  "num_total2");
    ui->pushButton_step->sum_append(index,  "num_price_add");
    ui->pushButton_step->sum_append(index,  "num_total3");
    ui->pushButton_step->sum_append(index,  "num_profit");
    ui->pushButton_step->sum_genrate(index);
    sum_filldata(ui->pushButton_step->get_sum_list_total_data(index));
    resize_column_to_content_t();

}

void w_rpt_report_frame_dishsale::toupdate_group2(int index)
{
    QStringList group_list = ui_help->comboBox_group->itemData(index).toStringList();
     ui_help->comboBox_group_2->clear();
     ui_help->comboBox_group_2->addItem(QObject::tr("全部"), "");
     ui_help->comboBox_group_2->addItemsBySql(QString("select %1, %2 from %3")
                                              .arg(group_list.value(mapKey))
                                              .arg(group_list.value(mapValue))
                                              .arg(group_list.value(mapTablename)));
}

void w_rpt_report_frame_dishsale::toexport()
{
    exportModel();
}

void w_rpt_report_frame_dishsale::toprint()
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
