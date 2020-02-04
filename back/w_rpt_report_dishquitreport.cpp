#include "w_rpt_report_dishquitreport.h"
//采购退货报表
#include "ui_w_rpt_report_cashreport_dialog.h"
#include "n_func.h"
#include "backheader.h"
#include "lds_tableview.h"
#include <QtDebug>

w_rpt_report_dishquitreport::w_rpt_report_dishquitreport(QWidget *parent) :
    QDialog(parent),ui(new Ui_w_rpt_report_cashreport_Dialog)
{
    //! init
    ui->setupUi(this);
    ui->frame->setFixedWidth(w_rpt_report::frame_left_fixed_width);
    w_rpt_report::reportBeginDtSet(ui->dateTimeEdit);
    ui->dateTimeEdit_2->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(), QTime(23,59,59)));
    tab1sqlinit();

    querymodel=new datetimeModel;

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);ui->tableView->setModel(querymodel);

    ui->comboBox_11->addItems(tab1info.keys());
    //! toolbar
    ui->widget->setup(QStringList()<<QObject::tr("查询")<<QObject::tr("退出"), Qt::AlignTop);
    connect(ui->widget->index_widget(QObject::tr("查询")),SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->comboBox_11,SIGNAL(currentIndexChanged(QString)),this,SLOT(tocom12(QString)));
    connect(ui->tableView->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(toselect()));

    //! after
    tocom12(ui->comboBox_11->currentText());

    //tabwidget header
    //采购退货报表
}

void w_rpt_report_dishquitreport::tab1sqlinit()
{
    tab1info.insert(QObject::tr("按商品类型"),
                    w_rpt_report::TABTYPE(QStringList()<<QObject::tr("商品类型")<<QObject::tr("品码")<<QObject::tr("品名")<<QObject::tr("单位")<<QObject::tr("均价")<<QObject::tr("数量")<<QObject::tr("金额")<<QObject::tr("折扣")<<QObject::tr("赠送额")<<QObject::tr("实际金额"),
                                          QString("select "
                                                  " (select vch_dish_typename from cey_bt_dish_type where cey_bt_dish_type.ch_dish_typeno=dish_typeno) as 'tabname0',"
                                                  " dishno as 'tabname1',"
                                                  " dishname as 'tabname2',"
                                                  " (select vch_unitname from cey_bt_unit where cey_bt_unit.ch_unitno=dish_unitno) as 'tabname3',"
                                                  " (case t.num when 0 then 0 else t.amount/t.num end) as 'tabname4',"
                                                  " t.num as 'tabname5',"
                                                  " t.amount as 'tabname6',"
                                                  " t.discount as 'tabname7',"
                                                  " t.present as 'tabname8',"
                                                  " t.amount-t.discount-t.present as 'tabname9'"
                                                  " from (%1)t  ORDER BY tabname%4").arg(
                                              QString (
                                                  "Select dish_typeno,"

                                                  " dish_typeno as 'dish_typeno1',"

                                                  " dishno,"

                                                  " dishname,"

                                                  " dish_unitno,"

                                                  " sum(num) as 'num',"

                                                  " sum(amount) as 'amount',"

                                                  " sum(discount) as 'discount',"

                                                  " sum(present) as 'present',"

                                                  " typeno"

                                                  " from (SELECT c.ch_dish_typeno as 'dish_typeno',"

                                                  " a.ch_dishno as 'dishno',"

                                                  " c.vch_dishname as 'dishname',"

                                                  " c.ch_unitno as 'dish_unitno',"

                                                  " a.num_back as 'num',"

                                                  " (a.num_back * a.num_price) as 'amount',"

                                                  " (a.num_back * a.num_price) *"

                                                  " (100 - a.int_discount) * 0.01 as 'discount',"

                                                  " (case a.ch_presentflag"

                                                  " when 'Y' then"

                                                  " (a.num_back * a.num_price) *"

                                                  " a.int_discount * 0.01"

                                                  " else"

                                                  " 0"

                                                  " end) as 'present',"

                                                  " c.ch_dish_typeno as 'typeno'"

                                                  " FROM cey_u_orderdish a, cey_u_checkout_master b, cey_bt_dish c"

                                                  " where (a.ch_payno = b.ch_payno)"

                                                  " and (b.ch_state = 'Y')"

                                                  " and (a.num_back <> 0)"

                                                  " and (a.ch_dishno = c.ch_dishno)"

                                                  " and (c.ch_dish_typeno like '%3%')"

                                                  " and (b.dt_operdate >= '%1')"

                                                  " and (b.dt_operdate <= '%2')"

                                                  ""

                                                  " union all"

                                                  " SELECT c.ch_dish_typeno as 'dish_typeno',"

                                                  " a.ch_dishno as 'dishno',"

                                                  " c.vch_dishname as 'dishname',"

                                                  " c.ch_unitno as 'dish_unitno',"

                                                  " a.num_num - a.num_back as 'num',"

                                                  " ((a.num_num - a.num_back) * a.num_price ) as 'amount',"

                                                  " ((a.num_num - a.num_back) * a.num_price ) *"

                                                  " (100 - a.int_discount) * 0.01 as 'discount',"

                                                  " (case a.ch_presentflag"

                                                  " when 'Y' then"

                                                  " ((a.num_num - a.num_back) * a.num_price ) *"

                                                  " a.int_discount * 0.01"

                                                  " else"

                                                  " 0"

                                                  " end) as 'present',"

                                                  " c.ch_dish_typeno as 'typeno'"

                                                  " FROM cey_u_orderdish a, cey_u_checkout_master b, cey_bt_dish c"

                                                  " where (a.ch_payno = b.ch_payno)"

                                                  " and (b.ch_state = 'P')"

                                                  " and (a.num_num - a.num_back <> 0)"

                                                  " and (a.ch_dishno = c.ch_dishno)"

                                                  " and (c.ch_dish_typeno like '%3%')"

                                                  " and (b.dt_operdate2 >= '%1')"

                                                  " and (b.dt_operdate2 <= '%2')"

                                                  ""

                                                  " union all"

                                                  " SELECT c.ch_dish_typeno as 'dish_typeno',"

                                                  " a.ch_dishno as 'dishno',"

                                                  " c.vch_dishname as 'dishname',"

                                                  " c.ch_unitno as 'dish_unitno',"

                                                  " a.num_num - a.num_back as 'num',"

                                                  " ((a.num_num - a.num_back) * a.num_price ) as 'amount',"

                                                  " ((a.num_num - a.num_back) * a.num_price ) *"

                                                  " (100 - a.int_discount) * 0.01 as 'discount',"

                                                  " (case a.ch_presentflag"

                                                  " when 'Y' then"

                                                  " ((a.num_num - a.num_back) * a.num_price ) *"

                                                  " a.int_discount * 0.01"

                                                  " else"

                                                  " 0"

                                                  " end) as 'present',"

                                                  " c.ch_dish_typeno as 'typeno'"

                                                  " FROM cey_u_orderdish a, cey_u_togo b, cey_bt_dish c"

                                                  " where (a.ch_togono = b.ch_togono)"

                                                  " and (b.ch_state = 'P')"

                                                  " and (a.num_num - a.num_back <> 0)"

                                                  " and (a.ch_dishno = c.ch_dishno)"

                                                  " and (c.ch_dish_typeno like '%3%')"

                                                  " and (b.dt_operdate >= '%1')"

                                                  " and (b.dt_operdate <= '%2')"

                                                  ")cc"

                                                  " Group by dish_typeno, dishno, dishname, dish_unitno, typeno"
                                                  )),
                                          w_rpt_report::querymodelsetfilters("select * from cey_bt_dish_type")
                                          ));

    tab1info.insert(QObject::tr("按餐桌"),
                    w_rpt_report::TABTYPE(QStringList()<<QObject::tr("餐桌")<<QObject::tr("品码")<<QObject::tr("品名")<<QObject::tr("单位")<<QObject::tr("均价")<<QObject::tr("数量")<<QObject::tr("金额")<<QObject::tr("折扣")<<QObject::tr("赠送额")<<QObject::tr("实际金额"),

                                          QString("select "
                                                  " (select vch_tablename  from cey_bt_table where cey_bt_table.ch_tableno=dish_typeno) as 'tabname0',"
                                                  " dishno as 'tabname1',"
                                                  " dishname as 'tabname2',"
                                                  " (select vch_unitname from cey_bt_unit where cey_bt_unit.ch_unitno=dish_unitno) as 'tabname3',"
                                                  " (case t.num when 0 then 0 else t.amount/t.num end) as 'tabname4',"
                                                  " t.num as 'tabname5',"
                                                  " t.amount as 'tabname6',"
                                                  " t.discount as 'tabname7',"
                                                  " t.present as 'tabname8',"
                                                  " t.amount-t.discount-t.present as 'tabname9'"
                                                  " from (%1)t  ORDER BY tabname%4").arg(
                                              QString (
                                                  "Select dish_typeno,"

                                                  " dish_typeno as 'dish_typeno1',"

                                                  " dishno,"

                                                  " dishname,"

                                                  " dish_unitno,"

                                                  " sum(num) as 'num',"

                                                  " sum(amount) as 'amount',"

                                                  " sum(discount) as 'discount',"

                                                  " sum(present) as 'present',"

                                                  " typeno"

                                                  " from (SELECT a.ch_tableno as 'dish_typeno',"

                                                  " a.ch_dishno as 'dishno',"

                                                  " c.vch_dishname as 'dishname',"

                                                  " c.ch_unitno as 'dish_unitno',"

                                                  " a.num_back as 'num',"

                                                  " (a.num_back * a.num_price ) as 'amount',"

                                                  " (a.num_back * a.num_price ) *"

                                                  " (100 - a.int_discount) * 0.01 as 'discount',"

                                                  " (case a.ch_presentflag"

                                                  " when 'Y' then"

                                                  " (a.num_back * a.num_price ) *"

                                                  " a.int_discount * 0.01"

                                                  " else"

                                                  " 0"

                                                  " end) as 'present',"

                                                  " c.ch_dish_typeno as 'typeno'"

                                                  " FROM cey_u_orderdish a, cey_u_checkout_master b, cey_bt_dish c"

                                                  " where (a.ch_payno = b.ch_payno)"

                                                  " and (b.ch_state = 'Y')"

                                                  " and (a.num_back <> 0)"

                                                  " and (a.ch_dishno = c.ch_dishno)"

                                                  " and (a.ch_tableno like '%3%')"

                                                  " and (b.dt_operdate >= '%1')"

                                                  " and (b.dt_operdate <= '%2')"

                                                  " union all"

                                                  " SELECT a.ch_tableno as 'dish_typeno',"

                                                  " a.ch_dishno as 'dishno',"

                                                  " c.vch_dishname as 'dishname',"

                                                  " c.ch_unitno as 'dish_unitno',"

                                                  " a.num_num - a.num_back as 'num',"

                                                  " ((a.num_num - a.num_back) * a.num_price ) as 'amount',"

                                                  " ((a.num_num - a.num_back) * a.num_price ) *"

                                                  " (100 - a.int_discount) * 0.01 as 'discount',"

                                                  " (case a.ch_presentflag"

                                                  " when 'Y' then"

                                                  " ((a.num_num - a.num_back) * a.num_price ) *"

                                                  " a.int_discount * 0.01"

                                                  " else"

                                                  " 0"

                                                  " end) as 'present',"

                                                  " c.ch_dish_typeno as 'typeno'"

                                                  " FROM cey_u_orderdish a, cey_u_checkout_master b, cey_bt_dish c"

                                                  " where (a.ch_payno = b.ch_payno)"

                                                  " and (b.ch_state = 'P')"

                                                  " and (a.num_num - a.num_back <> 0)"

                                                  " and (a.ch_dishno = c.ch_dishno)"

                                                  " and (a.ch_tableno like '%3%')"

                                                  " and (b.dt_operdate >= '%1')"

                                                  " and (b.dt_operdate <= '%2')"

                                                  " union all"

                                                  " SELECT a.ch_tableno as 'dish_typeno',"

                                                  " a.ch_dishno as 'dishno',"

                                                  " c.vch_dishname as 'dishname',"

                                                  " c.ch_unitno as 'dish_unitno',"

                                                  " a.num_num - a.num_back as 'num',"

                                                  " ((a.num_num - a.num_back) * a.num_price ) as 'amount',"

                                                  " ((a.num_num - a.num_back) * a.num_price ) *"

                                                  " (100 - a.int_discount) * 0.01 as 'discount',"

                                                  " (case a.ch_presentflag"

                                                  " when 'Y' then"

                                                  " ((a.num_num - a.num_back) * a.num_price ) *"

                                                  " a.int_discount * 0.01"

                                                  " else"

                                                  " 0"

                                                  " end) as 'present',"

                                                  " c.ch_dish_typeno as 'typeno'"

                                                  " FROM cey_u_orderdish a, cey_u_togo b, cey_bt_dish c"

                                                  " where (a.ch_togono = b.ch_togono)"

                                                  " and (b.ch_state = 'P')"

                                                  " and (a.num_num - a.num_back <> 0)"

                                                  " and (a.ch_dishno = c.ch_dishno)"

                                                  " and (a.ch_tableno like '%3%')"

                                                  " and (b.dt_operdate >= '%1')"

                                                  " and (b.dt_operdate <= '%2')"

                                                  " Group by dish_typeno, dishno, dishname, dish_unitno, typeno"
                                                  )),

                                          w_rpt_report::querymodelsetfilters("select * from cey_bt_table")
                                          ));

    tab1info.insert(QObject::tr("按餐桌类型"),
                    w_rpt_report::TABTYPE(QStringList()<<QObject::tr("餐桌类型")<<QObject::tr("品码")<<QObject::tr("品名")<<QObject::tr("单位")<<QObject::tr("均价")<<QObject::tr("数量")<<QObject::tr("金额")<<QObject::tr("折扣")<<QObject::tr("赠送额")<<QObject::tr("实际金额"),
                                          QString("select "
                                                  " (select vch_typename  from cey_bt_table_type where cey_bt_table_type.ch_typeno=dish_typeno) as 'tabname0',"
                                                  " dishno as 'tabname1',"
                                                  " dishname as 'tabname2',"
                                                  " (select vch_unitname from cey_bt_unit where cey_bt_unit.ch_unitno=dish_unitno) as 'tabname3',"
                                                  " (case t.num when 0 then 0 else t.amount/t.num end) as 'tabname4',"
                                                  " t.num as 'tabname5',"
                                                  " t.amount as 'tabname6',"
                                                  " t.discount as 'tabname7',"
                                                  " t.present as 'tabname8',"
                                                  " t.amount-t.discount-t.present as 'tabname9'"
                                                  " from (%1)t  ORDER BY tabname%4").arg(
                                              QString (
                                                  "Select dish_typeno,"

                                                  " dish_typeno as 'dish_typeno1',"

                                                  " dishno,"

                                                  " dishname,"

                                                  " dish_unitno,"

                                                  " sum(num) as 'num',"

                                                  " sum(amount) as 'amount',"

                                                  " sum(discount) as 'discount',"

                                                  " sum(present) as 'present',"

                                                  " typeno"

                                                  " from (SELECT d.ch_typeno as 'dish_typeno',"

                                                  " a.ch_dishno as 'dishno',"

                                                  " c.vch_dishname as 'dishname',"

                                                  " c.ch_unitno as 'dish_unitno',"

                                                  " a.num_back as 'num',"

                                                  " (a.num_back * a.num_price ) as 'amount',"

                                                  " (a.num_back * a.num_price ) *"

                                                  " (100 - a.int_discount) * 0.01 as 'discount',"

                                                  " (case a.ch_presentflag"

                                                  " when 'Y' then"

                                                  " (a.num_back * a.num_price ) *"

                                                  " a.int_discount * 0.01"

                                                  " else"

                                                  " 0"

                                                  " end) as 'present',"

                                                  " c.ch_dish_typeno as 'typeno'"

                                                  " FROM cey_u_orderdish a,"

                                                  " cey_u_checkout_master b,"

                                                  " cey_bt_dish c,"

                                                  " (select ch_tableno, ch_typeno"

                                                  " from cey_bt_table"

                                                  " union"

                                                  " select 'Q000', 'Q0') d"

                                                  " where a.ch_tableno = d.ch_tableno"

                                                  " and (a.ch_payno = b.ch_payno)"

                                                  " and (b.ch_state = 'Y')"

                                                  " and (a.num_back <> 0)"

                                                  " and (a.ch_dishno = c.ch_dishno)"

                                                  " and (d.ch_typeno like '%3%')"

                                                  " and (b.dt_operdate >= '%1')"

                                                  " and (b.dt_operdate <= '%2')"

                                                  " union all"

                                                  " SELECT d.ch_typeno as 'dish_typeno',"

                                                  " a.ch_dishno as 'dishno',"

                                                  " c.vch_dishname as 'dishname',"

                                                  " c.ch_unitno as 'dish_unitno',"

                                                  " a.num_num - a.num_back as 'num',"

                                                  " ((a.num_num - a.num_back) * a.num_price ) as 'amount',"

                                                  " ((a.num_num - a.num_back) * a.num_price ) *"

                                                  " (100 - a.int_discount) * 0.01 as 'discount',  (case a.ch_presentflag  when 'Y' then  ((a.num_num - a.num_back) *a.num_price ) * a.int_discount * 0.01"

                                                  " else"

                                                  " 0"

                                                  " end) as 'present',"

                                                  " c.ch_dish_typeno as 'typeno'"

                                                  " FROM cey_u_orderdish a,"

                                                  " cey_u_checkout_master b,"

                                                  " cey_bt_dish c,"

                                                  " (select ch_tableno, ch_typeno"

                                                  " from cey_bt_table"

                                                  " union"

                                                  " select 'Q000', 'Q0') d"

                                                  " where a.ch_tableno = d.ch_tableno"

                                                  " and (a.ch_payno = b.ch_payno)"

                                                  " and (b.ch_state = 'P')"

                                                  " and (a.num_num - a.num_back <> 0)"

                                                  " and (a.ch_dishno = c.ch_dishno)"

                                                  " and (d.ch_typeno like '%3%')"

                                                  " and (b.dt_operdate >= '%1')"

                                                  " and (b.dt_operdate <= '%2')"

                                                  " union all"

                                                  " SELECT d.ch_typeno as 'dish_typeno',"

                                                  " a.ch_dishno as 'dishno',"

                                                  " c.vch_dishname as 'dishname',"

                                                  " c.ch_unitno as 'dish_unitno',"

                                                  " a.num_num - a.num_back as 'num',"

                                                  " ((a.num_num - a.num_back) * a.num_price ) as 'amount',"

                                                  " ((a.num_num - a.num_back) * a.num_price ) *"

                                                  " (100 - a.int_discount) * 0.01 as 'discount',"

                                                  " (case a.ch_presentflag  when 'Y' then ((a.num_num - a.num_back) *a.num_price ) * a.int_discount * 0.01"

                                                  " else"

                                                  " 0"

                                                  " end) as 'present',"

                                                  " c.ch_dish_typeno as 'typeno'"

                                                  " FROM cey_u_orderdish a,"

                                                  " cey_u_togo b,"

                                                  " cey_bt_dish c,"

                                                  " (select ch_tableno, ch_typeno"

                                                  " from cey_bt_table"

                                                  " union"

                                                  " select 'Q000', 'Q0') d"

                                                  " where a.ch_tableno = d.ch_tableno"

                                                  " and (a.ch_togono = b.ch_togono)"

                                                  " and (b.ch_state = 'P')"

                                                  " and (a.num_num - a.num_back <> 0)"

                                                  " and (a.ch_dishno = c.ch_dishno)"

                                                  " and (d.ch_typeno like '%3%')"

                                                  " and (b.dt_operdate >= '%1')"

                                                  " and (b.dt_operdate <= '%2')"

                                                  ") cc"

                                                  " Group by dish_typeno, dishno, dishname, dish_unitno, typeno"
                                                  )),
                                          w_rpt_report::querymodelsetfilters("select * from cey_bt_table_area")
                                          ));

    tab1info.insert(QObject::tr("按退货人"),
                    w_rpt_report::TABTYPE(QStringList()<<QObject::tr("退货人")<<QObject::tr("品码")<<QObject::tr("品名")<<QObject::tr("单位")<<QObject::tr("均价")<<QObject::tr("数量")<<QObject::tr("金额")<<QObject::tr("折扣")<<QObject::tr("赠送额")<<QObject::tr("实际金额"),
                                          QString("select "
                                                  " dish_typeno as 'tabname0',"
                                                  " dishno as 'tabname1',"
                                                  " dishname as 'tabname2',"
                                                  " (select vch_unitname from cey_bt_unit where cey_bt_unit.ch_unitno=dish_unitno) as 'tabname3',"
                                                  " (case t.num when 0 then 0 else t.amount/t.num end) as 'tabname4',"
                                                  " t.num as 'tabname5',"
                                                  " t.amount as 'tabname6',"
                                                  " t.discount as 'tabname7',"
                                                  " t.present as 'tabname8',"
                                                  " t.amount-t.discount-t.present as 'tabname9'"
                                                  " from (%1)t  ORDER BY tabname%4").arg(
                                              QString (
                                                  "Select dish_typeno,"

                                                  " dish_typeno as 'dish_typeno1',"

                                                  " dishno,"

                                                  " dishname,"

                                                  " dish_unitno,"

                                                  " sum(num) as 'num',"

                                                  " sum(amount) as 'amount',"

                                                  " sum(discount) as 'discount',"

                                                  " sum(present) as 'present',"

                                                  " typeno"

                                                  " from (SELECT ifnull(a.vch_back_operID, '') as 'dish_typeno',"

                                                  " a.ch_dishno as 'dishno',"

                                                  " c.vch_dishname as 'dishname',"

                                                  " c.ch_unitno as 'dish_unitno',"

                                                  " a.num_back as 'num',"

                                                  " (a.num_back * a.num_price ) as 'amount',"

                                                  " (a.num_back * a.num_price ) *"

                                                  " (100 - a.int_discount) * 0.01 as 'discount',"

                                                  " (case a.ch_presentflag"

                                                  " when 'Y' then"

                                                  " (a.num_back * a.num_price ) *"

                                                  " a.int_discount * 0.01"

                                                  " else"

                                                  " 0"

                                                  " end) as 'present',"

                                                  " c.ch_dish_typeno as 'typeno'"

                                                  " FROM cey_u_orderdish a, cey_u_checkout_master b, cey_bt_dish c"

                                                  " where (a.ch_payno = b.ch_payno)"

                                                  " and (b.ch_state = 'Y')"

                                                  " and (a.num_back <> 0)"

                                                  " and (a.ch_dishno = c.ch_dishno)"

                                                  " and (ifnull(a.vch_back_operID, '') like '%3%')"

                                                  " and (b.dt_operdate >= '%1')"

                                                  " and (b.dt_operdate <= '%2')"

                                                  " union all"

                                                  " SELECT ifnull(b.vch_operID2, '') as 'dish_typeno',"

                                                  " a.ch_dishno as 'dishno',"

                                                  " c.vch_dishname as 'dishname',"

                                                  " c.ch_unitno as 'dish_unitno',"

                                                  " a.num_num - a.num_back as 'num',"

                                                  " ((a.num_num - a.num_back) * a.num_price ) as 'amount',"

                                                  " ((a.num_num - a.num_back) * a.num_price ) *"

                                                  " (100 - a.int_discount) * 0.01 as 'discount',"

                                                  " (case a.ch_presentflag"

                                                  " when 'Y' then"

                                                  " ((a.num_num - a.num_back) * a.num_price ) *"

                                                  " a.int_discount * 0.01"

                                                  " else"

                                                  " 0"

                                                  " end) as 'present',"

                                                  " c.ch_dish_typeno as 'typeno'"

                                                  " FROM cey_u_orderdish a, cey_u_checkout_master b, cey_bt_dish c"

                                                  " where (a.ch_payno = b.ch_payno)"

                                                  " and (b.ch_state = 'P')"

                                                  " and (a.num_num - a.num_back <> 0)"

                                                  " and (a.ch_dishno = c.ch_dishno)"

                                                  " and (ifnull(b.vch_operID2, '') like '%3%')"

                                                  " and (b.dt_operdate >= '%1')"

                                                  " and (b.dt_operdate <= '%2')"

                                                  " union all"

                                                  " SELECT ifnull(b.vch_operID, '') as 'dish_typeno',"

                                                  " a.ch_dishno as 'dishno',"

                                                  " c.vch_dishname as 'dishname',"

                                                  " c.ch_unitno as 'dish_unitno',"

                                                  " a.num_num - a.num_back as 'num',"

                                                  " ((a.num_num - a.num_back) * a.num_price ) as 'amount',"

                                                  " ((a.num_num - a.num_back) * a.num_price ) *"

                                                  " (100 - a.int_discount) * 0.01 as 'discount',"

                                                  " (case a.ch_presentflag"

                                                  " when 'Y' then"

                                                  " ((a.num_num - a.num_back) * a.num_price ) *"

                                                  " a.int_discount * 0.01"

                                                  " else"

                                                  " 0"

                                                  " end) as 'present',"

                                                  " c.ch_dish_typeno as 'typeno'"

                                                  " FROM cey_u_orderdish a, cey_u_togo b, cey_bt_dish c"

                                                  " where (a.ch_togono = b.ch_togono)"

                                                  " and (b.ch_state = 'P')"

                                                  " and (a.num_num - a.num_back <> 0)"

                                                  " and (a.ch_dishno = c.ch_dishno)"

                                                  " and (ifnull(b.vch_operID, '') like '%3%')"

                                                  " and (b.dt_operdate >= '%1')"

                                                  " and (b.dt_operdate <= '%2')"

                                                  ") cc"

                                                  " Group by dish_typeno, dishno, dishname, dish_unitno, typeno"

                                                  )),
                                          w_rpt_report::querymodelsetfilters(backheader::SELECT_OPERID_NAME)
                                          ));

    tab1info.insert(QObject::tr("按退货明细查询"),
                    w_rpt_report::TABTYPE(QStringList()<<QObject::tr("退货类型")<<QObject::tr("品码")<<QObject::tr("品名")<<QObject::tr("单位")<<QObject::tr("均价")<<QObject::tr("数量")<<QObject::tr("金额")<<QObject::tr("折扣")<<QObject::tr("赠送额")<<QObject::tr("实际金额")<<QObject::tr("退货人")<<QObject::tr("退货时间")<<QObject::tr("退货原因"),
                                          QString (
                                              "SELECT '"+QObject::tr("点单退货")+"' as 'dish_typeno',"

                                              " a.ch_dishno as 'dishno',"

                                              " c.vch_dishname as 'dishname',"

                                              " c.ch_unitno as 'dish_unitno',"

                                              " a.num_back as 'num',"

                                              " (a.num_back * a.num_price ) as 'amount',"

                                              " (a.num_back * a.num_price ) *"

                                              " (100 - a.int_discount) * 0.01 as 'discount',"

                                              " (case a.ch_presentflag"

                                              " when 'Y' then"

                                              " (a.num_back * a.num_price ) * a.int_discount * 0.01"

                                              " else"

                                              " 0"

                                              " end) as 'present',"

                                              " c.ch_dish_typeno as 'typeno',"

                                              " a.vch_back_operID as 'operID',"

                                              " a.dt_back_operdate as 'operdate',"

                                              " (select vch_reason"

                                              " from cey_u_orderdish_log"

                                              " where a.int_flowid = int_orderflow"

                                              " and ch_type = '1' LIMIT 1) as 'reason'"

                                              " FROM cey_u_orderdish a, cey_u_checkout_master b, cey_bt_dish c"

                                              " WHERE (a.ch_payno = b.ch_payno)"

                                              " and (b.ch_state = 'Y')"

                                              " and (a.num_back <> 0)"

                                              " and (a.ch_dishno = c.ch_dishno)"

                                              " and (b.dt_operdate >= '%1')"

                                              " and (b.dt_operdate <= '%2')"

                                              "union all"

                                              ""

                                              " SELECT '"+QObject::tr("快餐反结账")+"' as 'dish_typeno',"

                                              " a.ch_dishno as 'dishno',"

                                              " c.vch_dishname as 'dishname',"

                                              " c.ch_unitno as 'dish_unitno',"

                                              " a.num_num - a.num_back as 'num',"

                                              " ((a.num_num - a.num_back) * a.num_price ) as 'amount',"

                                              " ((a.num_num - a.num_back) * a.num_price ) *"

                                              " (100 - a.int_discount) * 0.01 as 'discount',"

                                              " (case a.ch_presentflag  when 'Y' then  ((a.num_num - a.num_back) *  a.num_price ) * a.int_discount * 0.01"

                                              " else"

                                              " 0"

                                              " end) as 'present',"

                                              " c.ch_dish_typeno as 'typeno',"

                                              " b.vch_operID2 as 'operID',"

                                              " b.dt_operdate2 as 'operdate',"

                                              " (select vch_reason"

                                              " from cey_u_orderdish_log"

                                              " where a.int_flowid = int_orderflow"

                                              " and ch_type = '1' LIMIT 1) as 'reason'"

                                              " FROM cey_u_orderdish a, cey_u_checkout_master b, cey_bt_dish c"

                                              " WHERE (a.ch_payno = b.ch_payno)"

                                              " and (b.ch_state = 'P')"

                                              " and (a.num_num - a.num_back <> 0)"

                                              " and (a.ch_dishno = c.ch_dishno)"

                                              " and (b.dt_operdate >= '%1')"

                                              " and (b.dt_operdate <= '%2')"

                                              " union all"

                                              " SELECT '"+QObject::tr("外卖退单")+"' as 'dish_typeno',"

                                              " a.ch_dishno as 'dishno',"

                                              " c.vch_dishname as 'dishname',"

                                              " c.ch_unitno as 'dish_unitno',"

                                              " a.num_num - a.num_back as 'num',"

                                              " ((a.num_num - a.num_back) * a.num_price ) as 'amount',"

                                              " ((a.num_num - a.num_back) * a.num_price ) *"

                                              " (100 - a.int_discount) * 0.01 as 'discount',"

                                              " (case a.ch_presentflag  when 'Y' then  ((a.num_num - a.num_back) * a.num_price ) * a.int_discount * 0.01"

                                              " else"

                                              " 0"

                                              " end) as 'present',"

                                              " c.ch_dish_typeno as 'typeno',"

                                              " b.vch_operID as 'operID',"

                                              " b.dt_operdate as 'operdate',"

                                              " (select vch_reason"

                                              " from cey_u_orderdish_log"

                                              " where a.int_flowid = int_orderflow"

                                              " and ch_type = '1' LIMIT 1) as 'reason'"

                                              " FROM cey_u_orderdish a, cey_u_togo b, cey_bt_dish c"

                                              " WHERE (a.ch_togono = b.ch_togono)"

                                              " and (b.ch_state = 'P')"

                                              " and (a.num_num - a.num_back <> 0)"

                                              " and (a.ch_dishno = c.ch_dishno)"

                                              " and (b.dt_operdate >= '%1')"

                                              " and (b.dt_operdate <= '%2')"
                                              ),
                                          w_rpt_report::querymodelsetfilters("")
                                          ));
}

void w_rpt_report_dishquitreport::toselect()
{
    //
    QString ls_begin;
    QString ls_end;
    ls_begin = ui->dateTimeEdit->dateTimeStr();
    ls_end = ui->dateTimeEdit_2->dateTimeStr();

    //model select
    querymodel->setQuery(tab1info[ui->comboBox_11->currentText()].sql
            .arg(ls_begin)
            .arg(ls_end)
            .arg(backheader::getinterdata(ui->comboBox_12->currentText()))//获取过滤信息
            .arg(w_rpt_report::querymodelordersort(ui->tableView->tableView))
            );

    //更新表头
    backheader::modelSetHeader(querymodel, tab1info[ui->comboBox_11->currentText()].headers);
    if(querymodel->rowCount() > 0) {
        ui->tableView->resizeColumnsToContents();
    }

    //更新合计
    QString sumsql=querymodel->query().lastQuery().trimmed();
    if(sumsql.endsWith("DESC")||sumsql.endsWith("ASC")) {
        //5 6 7  9 10
        lds_query_hddpos  query;

        if(tab1info[ui->comboBox_11->currentText()].except_c.isEmpty()) {
            query.execSelect(QString("select sum(tabname3),sum(tabname4),sum(tabname5),sum(tabname6),sum(tabname7),sum(tabname8),sum(tabname9) from (%1)t1 ").arg(sumsql.left(sumsql.lastIndexOf("ORDER BY"))));
            query.next();

            ui->tableView->updateSum(3,query.recordValue(0));
            ui->tableView->updateSum(4,query.recordValue(1));
            ui->tableView->updateSum(5,query.recordValue(2));
            ui->tableView->updateSum(6,query.recordValue(3));
            ui->tableView->updateSum(7,query.recordValue(4));
            ui->tableView->updateSum(8,query.recordValue(5));
            ui->tableView->updateSum(9,query.recordValue(6));

        } else {
            query.execSelect(QString("select sum(tabname3),sum(tabname4),sum(tabname5),sum(tabname6),sum(tabname7) from (%1)t1 ").arg(sumsql.left(sumsql.lastIndexOf("ORDER BY"))));
            query.next();

            ui->tableView->updateSum(3,query.recordValue(0));
            ui->tableView->updateSum(4,query.recordValue(1));
            ui->tableView->updateSum(5,query.recordValue(2));
            ui->tableView->updateSum(6,query.recordValue(3));
            ui->tableView->updateSum(7,query.recordValue(4));
        }
        ui->tableView->resizeColumnsToContents();
    }

}

void w_rpt_report_dishquitreport::toexit()
{
    this->reject();
}

void w_rpt_report_dishquitreport::tocom12(const QString &text)
{
    ui->comboBox_12->clear();
    ui->comboBox_12->addItems(tab1info[text].filters);
}
