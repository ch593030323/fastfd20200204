#include "w_rpt_report_frame_servantpromotquery.h"
#include "ui_w_rpt_report_frame_servantpromotquery.h"
#include "ui_w_rpt_report_frame.h"
#include "lds_query_hddpos.h"
#include <QTableWidget>
#include <QScrollBar>
#include <QtDebug>
#include "backheader.h"
#include "lds_model_sqltablemodel_delegate_com.h"

w_rpt_report_frame_servantpromotquery::w_rpt_report_frame_servantpromotquery(QWidget *parent) :
    w_rpt_report_frame(QStringList() << QObject::tr("流水") << QObject::tr("汇总") ,  parent)
{
    ui_help = new Ui::w_rpt_report_frame_servantpromotquery;
    ui_help->setupUi(ui->frame_other);
    ui_help->comboBox_servant->addItemsBySql("Select '' as vch_operID,'" + QObject::tr("全部") + "' as vch_operator union  Select '0000' as vch_operID,'system' as vch_operator union Select vch_operID, ifnull(vch_operator, '') as vch_operator from cey_sys_operator where ch_flag = 'Y' and ch_waiter = 'Y' ");

    ui->widget->setup(QStringList() << QObject::tr("查询")<< QObject::tr("导出")<< QObject::tr("打印")<< QObject::tr("退出"));
    //
    connect(ui->widget->index_widget(QObject::tr("查询")), SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(QObject::tr("导出")), SIGNAL(clicked()),this,SLOT(toexport()));
    connect(ui->widget->index_widget(QObject::tr("打印")), SIGNAL(clicked()),this,SLOT(toprint()));
}

w_rpt_report_frame_servantpromotquery::~w_rpt_report_frame_servantpromotquery()
{
    delete ui_help;
}

void w_rpt_report_frame_servantpromotquery::toselect()
{
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
    ui->pushButton_step->init(index);
    //
    switch(index) {
    case 0://服务员提成明细查询
    {
        QString sql_where =
                QString(
                    "    FROM "
                    "    cey_u_orderdish a, cey_u_table b, cey_u_checkout_master c, cey_bt_dish d "
                    "    WHERE "
                    "    a.ch_tableno = b.ch_tableno "
                    "    and a.ch_payno = b.ch_payno "
                    "    and ifnull(b.vch_waiter, '') <> ''   "
                    "    and a.ch_payno = c.ch_payno "
                    "    and c.ch_state = 'Y' "
                    "    and a.ch_suitflag <> 'Y'  "
                    "    and a.ch_dishno = d.ch_dishno  "
                    "    and a.ch_presentflag <> 'Y' "
                    "    and d.ch_deductflag = 'Y' "
                    "    and b.vch_waiter like '%1%' "
                    "    and c.dt_operdate >= '%2'  "
                    "    and c.dt_operdate <= '%3' "
                    )
                .arg(ui_help->comboBox_servant->curusrdata().toString())
                .arg(ui->dateTimeEdit->dateTimeStr())
                .arg(ui->dateTimeEdit_2->dateTimeStr())
                ;
        query.execSelect(QString("select count(0) "+sql_where));
        query.next();
        int rowcount = query.recordValue(0).toInt();
        ui->pushButton_step->init(index, rowcount, onepagestep,
                                  QString(
                                      "     SELECT "
                                      "     b.vch_waiter, "
                                      "     a.ch_payno, "
                                      "     d.ch_dishno,  "
                                      "     d.vch_dishname,  "
                                      "     d.ch_unitno,  "
                                      "     d.ch_dish_typeno,  "
                                      "     d.ch_sub_typeno,  "
                                      "     (num_num - num_back) as num_dish, "
                                      "     (a.num_num - a.num_back) * a.num_price * 0.01 * a.int_discount  as num_total2, "
                                      "     (case d.ch_deducttype  when '2' then d.num_deduct * (num_num - num_back)   else 0  end) as  num_fdeduct, "
                                      "     (case d.ch_deducttype  when '1' then ((a.num_num - a.num_back) * a.num_price) * int_discount * 0.01 * d.int_deduct * 0.01  else 0   end) as num_rdeduct, "
                                      "     b.dt_operdate  "
                                      +    sql_where
                                      )
                                  );
        //
        toselect_page();

        if(0 == tableview->itemDelegateForColumn(model->fieldIndex("vch_waiter"))) {
            tableview->setItemDelegateForColumn(model->fieldIndex("vch_waiter"), new lds_model_sqltablemodel_delegate_com(this, "(" + backheader::SELECT_OPERID_NAME + ") t", "vch_operID", "vch_operator") );
        }
        if(0 == tableview->itemDelegateForColumn(model->fieldIndex("ch_unitno"))) {
            tableview->setItemDelegateForColumn(model->fieldIndex("ch_unitno"), new lds_model_sqltablemodel_delegate_com(this, "cey_bt_unit", "ch_unitno", "vch_unitname") );
        }
        if(0 == tableview->itemDelegateForColumn(model->fieldIndex("ch_dish_typeno"))) {
            tableview->setItemDelegateForColumn(model->fieldIndex("ch_dish_typeno"), new lds_model_sqltablemodel_delegate_com(this, "cey_bt_dish_type", "ch_dish_typeno", "vch_dish_typename") );
        }
        if(0 == tableview->itemDelegateForColumn(model->fieldIndex("ch_sub_typeno"))) {
            tableview->setItemDelegateForColumn(model->fieldIndex("ch_sub_typeno"), new lds_model_sqltablemodel_delegate_com(this, "cey_bt_dish_subtype", "ch_sub_typeno", "vch_sub_typename") );
        }
        //
        ui->pushButton_step->sum_clear(index);
        ui->pushButton_step->sum_append(index,  "num_dish");
        ui->pushButton_step->sum_append(index,  "num_total2");
        ui->pushButton_step->sum_append(index,  "num_fdeduct");
        ui->pushButton_step->sum_append(index,  "num_rdeduct");
        ui->pushButton_step->sum_genrate(index);
    }
        break;
    case 1://服务员提成收银查询
    {
        QString sql_where =
                QString(
                    "     FROM "
                    "     cey_u_checkout_detail a, cey_u_table b, cey_u_checkout_master c, cey_bt_paymode d "
                    "     WHERE "
                    "     a.ch_payno = c.ch_payno "
                    "     and a.ch_paymodeno = d.ch_paymodeno "
                    "     and c.ch_state = 'Y'  "
                    "     and c.ch_payno = b.ch_payno "
                    "     and c.ch_tableno = b.ch_tableno  "
                    "     and IFNULL(b.vch_waiter, '') <> '' "
                    "     and b.vch_waiter like '%1%' "
                    "     and c.dt_operdate >= '%2'  "
                    "     and c.dt_operdate <= '%3' "
                    )
                .arg(ui_help->comboBox_servant->curusrdata().toString())
                .arg(ui->dateTimeEdit->dateTimeStr())
                .arg(ui->dateTimeEdit_2->dateTimeStr())
                ;
        query.execSelect(QString("select count(0) "+sql_where));
        query.next();
        int rowcount = query.recordValue(0).toInt();
        ui->pushButton_step->init(index, rowcount, onepagestep,
                                  QString(
                                      "     SELECT   "
                                      "     b.vch_waiter,  "
                                      "     a.ch_payno,  "
                                      "     b.ch_tableno,   "
                                      "     a.ch_paymodeno,   "
                                      "     a.num_realamount,   "
                                      "     a.dt_operdate  "
                                      +    sql_where
                                      )
                                  );
        //
        toselect_page();

        if(0 == tableview->itemDelegateForColumn(model->fieldIndex("vch_waiter"))) {
            tableview->setItemDelegateForColumn(model->fieldIndex("vch_waiter"), new lds_model_sqltablemodel_delegate_com(this, "(" + backheader::SELECT_OPERID_NAME + ") t", "vch_operID", "vch_operator") );
        }
        if(0 == tableview->itemDelegateForColumn(model->fieldIndex("ch_tableno"))) {
            tableview->setItemDelegateForColumn(model->fieldIndex("ch_tableno"), new lds_model_sqltablemodel_delegate_com(this, "cey_bt_table", "ch_tableno", "vch_tablename") );
        }
        if(0 == tableview->itemDelegateForColumn(model->fieldIndex("ch_paymodeno"))) {
            tableview->setItemDelegateForColumn(model->fieldIndex("ch_paymodeno"), new lds_model_sqltablemodel_delegate_com(this, "cey_bt_paymode", "ch_paymodeno", "vch_paymodename") );
        }
        //
        ui->pushButton_step->sum_clear(index);
        ui->pushButton_step->sum_append(index,  "num_realamount");
        ui->pushButton_step->sum_genrate(index);
    }
        break;
    }
    sum_filldata(ui->pushButton_step->get_sum_list_total_data(index));
    resize_column_to_content_t();
}

void w_rpt_report_frame_servantpromotquery::select_page_after()
{

}
