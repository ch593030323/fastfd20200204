#include "w_rpt_report_frame_cash.h"
#include "ui_w_rpt_report_frame_cash.h"
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


w_rpt_report_frame_cash::w_rpt_report_frame_cash(QWidget *parent) :
    w_rpt_report_frame(QStringList() << QObject::tr("流水")  << QObject::tr("汇总")  ,  parent)
{
    ui_help = new Ui::w_rpt_report_frame_cash;
    ui_help->setupUi(ui->frame_other);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->widget->setup(QStringList() << QObject::tr("查询")<< QObject::tr("导出")<< QObject::tr("打印")<< QObject::tr("退出"));
    ui_help->comboBox_group->addItem(QObject::tr("付款单号"), QStringList() << "b.ch_payno");
    ui_help->comboBox_group->addItem(QObject::tr("付款方式"), QStringList() << "a.ch_paymodeno");
    ui_help->comboBox_group->addItem(QObject::tr("操作员"), QStringList() << "b.vch_operID" << "vch_operator" << "("+backheader::SELECT_OPERID_NAME +")  b");
    ui_help->comboBox_group->addItem(QObject::tr("餐桌"), QStringList() << "b.ch_tableno" << "vch_tablename" << "cey_bt_table b");
    ui_help->comboBox_group->addItem(QObject::tr("餐桌类型"), QStringList() << "d.ch_typeno" << "vch_typename" << "cey_bt_table_type d");

    int idx = 0;
    //0
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_group, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_group, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_group_2, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_group_2, 1);
    //1
    idx++;
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_group, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_group, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_group_2, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_group_2, 0);

    ui->pushButton_step->update_map_widget_visiable(0);
    //
    connect(ui->widget->index_widget(QObject::tr("查询")), SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(QObject::tr("导出")), SIGNAL(clicked()),this,SLOT(toexport()));
    connect(ui->widget->index_widget(QObject::tr("打印")), SIGNAL(clicked()),this,SLOT(toprint()));
    //
    ui->pushButton_step->hide();
    disconnect(ui->pushButton_up, SIGNAL(clicked()),this, SLOT(toup()));
    disconnect(ui->pushButton_down, SIGNAL(clicked()),this, SLOT(todown()));
    connect(ui->pushButton_up, SIGNAL(clicked()),this,SLOT(toPageUp()));
    connect(ui->pushButton_down, SIGNAL(clicked()),this,SLOT(toPageDown()));
    connect(ui_help->comboBox_group, SIGNAL(currentIndexChanged(int)), this,  SLOT(toupdate_group2(int)));
    toupdate_group2(0);
}

w_rpt_report_frame_cash::~w_rpt_report_frame_cash()
{
    delete ui_help;
}

void w_rpt_report_frame_cash::toselect()
{
    pushbutton_auto_enable dd(ui->widget->index_widget(QObject::tr("查询")));

    QStringList group_list = ui_help->comboBox_group->itemData(ui_help->comboBox_group->currentIndex()).toStringList();
    QString group2 = ui_help->comboBox_group_2->itemData(ui_help->comboBox_group_2->currentIndex()).toString();


    //    lds_query_hddpos  query;
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
    switch(index) {//收银流水
    case 0:
    {
        //
        bool paydetail = group_list.value(mapKey).isEmpty();
        QString sqlarg =
                " Select  "
                + (paydetail ? "" : (group_list.value(mapKey) + ", ")) +
                "     c.vch_paymodename, "
                "     count(a.ch_payno)  as num_paymode_count,"
                "     sum(a.num_payamount)  as num_payamount,"
                "     sum(a.num_realamount)  as num_realamount,";
        sqlarg +=

                QString(
                    "        sum(if(c.ch_incomeflag = 'Y',"
                    "                num_realamount,"
                    "                0)) as num_realamount_income,"
                    "            sum(if(c.ch_incomeflag <> 'Y',"
                    "                num_realamount,"
                    "                0)) as num_realamount_unincome"
                    )
                ;
        if(ui_help->comboBox_group->currentIndex() == 2) {//操作员
            sqlarg +=
                    ","
                    "            e.num_deduct_rate as int_deduct,  "
                    "            sum(if(c.ch_incomeflag = 'Y',  num_realamount,  0) * e.num_deduct_rate / 100 ) as num_deduct "
                    ;
        }

        QString sql_where =
                " from "
                "     cey_u_checkout_detail a, "
                "     cey_u_checkout_master b, "
                "     cey_bt_paymode c, "
                "     (select   ch_tableno, ch_typeno    from     cey_bt_table union select 'Q000' as 'ch_tableno', 'Q0' as 'ch_typeno') as d,  "
                "     (select vch_operid, vch_operator, num_deduct_rate from cey_sys_operator union select '0000', 'system', 0) e "
                " where "
                "     b.ch_tableno = d.ch_tableno "
                "         and (a.ch_payno = b.ch_payno) "
                "         and (b.ch_state = 'Y') "
                "         and (a.ch_paymodeno = c.ch_paymodeno) "
                "         and (b.vch_operid = e.vch_operid) "
                "         and b.dt_operdate > '%1' "
                "         and b.dt_operdate <= '%2' "
                ;
        if(!group2.isEmpty()) {
            sql_where += QString(" and %1 = '%2' ").arg(group_list.value(mapKey)).arg(group2);
        }
        sql_where = sql_where
                .arg(ui->dateTimeEdit->dateTimeStr())
                .arg(ui->dateTimeEdit_2->dateTimeStr())
                ;
        model->setQuery(
                    sqlarg + sql_where +" group by  a.ch_paymodeno, "+group_list.value(mapKey)+" order by "+group_list.value(mapKey)+" desc "
                    );
        qDebug() << model->query().lastQuery();

        if(tableview->itemDelegateForColumn(0) == 0) {
            lds_model_sqltablemodel_delegate_com *d = new lds_model_sqltablemodel_delegate_com(this, ui_help->comboBox_group_2);
            d->enable_empty_return_key();
            tableview->setItemDelegateForColumn(0, d);

            tableview->setItemDelegateForColumn(model->fieldIndex("ch_dish_typeno"), new lds_model_sqltablemodel_delegate_com(this, "cey_bt_dish_type", "ch_dish_typeno", "vch_dish_typename"));
        }

        //汇总
        ui->pushButton_step->init(index, 0, onepagestep, sqlarg + sql_where);

        ui->pushButton_step->sum_clear(index);
        if(model->fieldIndex("num_paymode_count") > 0) ui->pushButton_step->sum_append(index,  "num_paymode_count");
        if(model->fieldIndex("num_payamount") > 0) ui->pushButton_step->sum_append(index,  "num_payamount");
        if(model->fieldIndex("num_realamount_income") > 0) ui->pushButton_step->sum_append(index,  "num_realamount_income");
        if(model->fieldIndex("num_realamount_unincome") > 0) ui->pushButton_step->sum_append(index,  "num_realamount_unincome");
        if(model->fieldIndex("num_deduct") > 0) ui->pushButton_step->sum_append(index,  "num_deduct");
        ui->pushButton_step->sum_genrate(index);
    }
        break;
    case 1:
    {
        QString sqlarg =
                " select date(dt_operdate) as date,"
                " sum(num_cost) as num_total,"
                " sum(num_discount) as num_discount,"
                " sum(num_present) as num_present,"
                " sum(num_blotout) as num_blotout,"
                " sum(num_free) as num_free,"
                " sum(num_service) as num_service,"
                " sum(num_lowcost) as num_lowcost";
        QString sql_where =
                " from cey_u_checkout_master where ch_state = 'Y'  and dt_operdate > '%1'  and dt_operdate <= '%2' ";
        sql_where = sql_where
                .arg(ui->dateTimeEdit->dateTime().toString(yyyyMMddhhmmss))
                .arg(ui->dateTimeEdit_2->dateTime().toString(yyyyMMddhhmmss))
                ;
        model->setQuery(
                    sqlarg + sql_where + " group by date(dt_operdate) ;"
                    );

        //汇总
        ui->pushButton_step->init(index, 0, onepagestep, sqlarg + sql_where);

        ui->pushButton_step->sum_clear(index);
        if(model->fieldIndex("num_total") > 0) ui->pushButton_step->sum_append(index,  "num_total");
        if(model->fieldIndex("num_discount") > 0) ui->pushButton_step->sum_append(index,  "num_discount");
        if(model->fieldIndex("num_present") > 0) ui->pushButton_step->sum_append(index,  "num_present");
        if(model->fieldIndex("num_blotout") > 0) ui->pushButton_step->sum_append(index,  "num_blotout");
        if(model->fieldIndex("num_free") > 0) ui->pushButton_step->sum_append(index,  "num_free");
        if(model->fieldIndex("num_service") > 0) ui->pushButton_step->sum_append(index,  "num_service");
        if(model->fieldIndex("num_lowcost") > 0) ui->pushButton_step->sum_append(index,  "num_lowcost");
        ui->pushButton_step->sum_genrate(index);
    }
        break;
    }


    sum_filldata(ui->pushButton_step->get_sum_list_total_data(index));
    tableview->resizeColumnsToContents();

}

void w_rpt_report_frame_cash::toupdate_group2(int index)
{
    QStringList group_list = ui_help->comboBox_group->itemData(index).toStringList();
    ui_help->comboBox_group_2->clear();
    ui_help->comboBox_group_2->addItem(QObject::tr("全部"), "");
    ui_help->comboBox_group_2->addItemsBySql(QString("select %1, %2 from %3")
                                             .arg(group_list.value(mapKey))
                                             .arg(group_list.value(mapValue))
                                             .arg(group_list.value(mapTablename)));
}

void w_rpt_report_frame_cash::toexport()
{
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->currentWidget());
    w_rpt_report_dataExport dialog(tableview, ui->tableView_sum, this);
    dialog.setWindowTitle(ui->comboBox->currentText());
    dialog.defFilepath(ui->comboBox->currentText());
    transparentCenterDialog(&dialog).exec();
}

void w_rpt_report_frame_cash::toprint()
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
