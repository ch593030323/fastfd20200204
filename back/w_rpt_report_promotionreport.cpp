#include "w_rpt_report_promotionreport.h"
//商品促销报表
#include "ui_w_rpt_report_promotionreport.h"
#include "lds_query_hddpos.h"
#include "n_func.h"
#include "lds_tableview.h"
#include <QSqlError>
#include "backheader.h"


w_rpt_report_promotionreport::w_rpt_report_promotionreport(QWidget *parent) :
    QDialog(parent),ui(new Ui_w_rpt_report_promotionreport)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->frame->setFixedWidth(w_rpt_report::frame_left_fixed_width);
    //! init
    w_rpt_report::reportBeginDtSet(ui->dateTimeEdit);
    //    ui->dateTimeEdit->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(), QTime(0,0,0)));
    ui->dateTimeEdit_2->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(), QTime(23,59,59)));

    //    ui->tableView->setSortingEnabled(true);
    tab1sqlinit();
    querymodel=new datetimeModel;
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);ui->tableView->setModel(querymodel);
    ui->comboBox_11->addItems(tab1info.keys());
    ui->comboBox_21->addItems(tab1info.keys());
    //! toolbar
    ui->widget->setup(QStringList()<<QObject::tr("查询")<<QObject::tr("退出"), Qt::AlignTop);
    connect(ui->widget->index_widget(QObject::tr("查询")),SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->comboBox_11,SIGNAL(currentIndexChanged(QString)),this,SLOT(tocom12(QString)));
    connect(ui->tableView->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(toselect()));

    //! after
    tocom12(ui->comboBox_11->currentText());
    ui->stackedWidget->setCurrentIndex(0);
}

w_rpt_report_promotionreport::~w_rpt_report_promotionreport()
{

}

void w_rpt_report_promotionreport::tab1sqlinit()
{
    tab1info.insert(QObject::tr("组合商品消费汇总报表"),
                    w_rpt_report::TABTYPE(QStringList()<<QObject::tr("主商品编号")<<QObject::tr("品名")<<QObject::tr("单位")<<QObject::tr("均价")<<QObject::tr("数量")<<QObject::tr("金额")<<QObject::tr("折扣")<<QObject::tr("折扣率")<<QObject::tr("赠送")<<QObject::tr("赠送率")<<QObject::tr("实际金额"),
                                          QString (
                                              " select "
                                              " t.dishno as 'tabname0',"
                                              " t.dishname as 'tabname1',"
                                              " (select vch_unitname from cey_bt_unit where cey_bt_unit.ch_unitno=t.dish_unitno) as 'tabname2',"
                                              " (case t.amount when 0 then 0 else t.amount/t.num end) as 'tabname3',"
                                              " t.num as 'tabname4',"
                                              " t.amount as 'tabname5',"
                                              " t.discount as 'tabname6',"
                                              " (case t.amount when 0 then 0 else t.discount/t.amount end) as 'tabname7',"
                                              " t.present as 'tabname8',"
                                              " (case t.amount when 0 then 0 else t.present/t.amount end) as 'tabname9',"
                                              " t.amount-t.discount-t.present as 'tabname10' "
                                              " from (%1)t  ORDER BY tabname%4").arg(
                                              QString (

                                                  "SELECT d.ch_dishno as 'dishno', "

                                                  " d.vch_dishname as 'dishname', "

                                                  " d.ch_unitno as 'dish_unitno', "

                                                  " sum((num_num - num_back)) as 'num', "

                                                  " sum(((a.num_num - a.num_back) * a.num_price + a.num_price_add)) as 'amount', "

                                                  " sum((((num_num - num_back) * (a.num_price )) * (100 - int_discount) * 0.01)) as 'discount', "

                                                  " sum((case ch_presentflag"

                                                  " when 'Y' then"

                                                  " ((num_num - num_back) *"

                                                  " (a.num_price )) * int_discount * 0.01"

                                                  " else"

                                                  " 0"

                                                  " end)) as 'present', "

                                                  " replace(d.ch_dishno, ' ', '') as 'suitno', "

                                                  " '0' as 'orders' "

                                                  " FROM cey_u_orderdish a, cey_u_checkout_master c, cey_bt_dish d"

                                                  " WHERE a.ch_payno = c.ch_payno"

                                                  " and c.ch_state = 'Y'"

                                                  " and (a.ch_suitflag = 'P')"

                                                  " and a.ch_dishno = d.ch_dishno"

                                                  " and (num_num - num_back) <> 0"

                                                  " and (c.dt_operdate >='%1')"

                                                  " and (c.dt_operdate <='%2')"

                                                  " and d.ch_dishno like '%3%'"

                                                  " and (DATE_FORMAT(c.dt_operdate, '%Y') >='0')"

                                                  " and (DATE_FORMAT(c.dt_operdate, '%Y') <='3')"

                                                  " group by d.ch_dishno, d.vch_dishname, d.ch_unitno"

                                                  " union"

                                                  " SELECT d.ch_dishno as 'dishno', "

                                                  " space(5) + d.vch_dishname as 'dishname', "

                                                  " d.ch_unitno as 'unit', "

                                                  " sum((num_num - num_back)) as 'num', "

                                                  " sum(num_price_add) as 'amount', "

                                                  " sum((((num_num - num_back) * (a.num_price )) * (100 - int_discount) * 0.01)) as 'discount', "

                                                  " sum((case ch_presentflag"

                                                  " when 'Y' then"

                                                  " int_discount * 0.01"

                                                  " else"

                                                  " 0"

                                                  " end)) as 'present', "

                                                  " replace(a.ch_suitno, ' ', '') as 'suitno', "

                                                  " '1' as 'orders'"

                                                  " FROM cey_u_orderdish a, cey_u_checkout_master c, cey_bt_dish d"

                                                  " WHERE a.ch_payno = c.ch_payno"

                                                  " and c.ch_state = 'Y'"

                                                  " and (a.ch_suitflag = 'Y')"

                                                  " and a.ch_dishno = d.ch_dishno"

                                                  " and (c.dt_operdate >='%1')"

                                                  " and (c.dt_operdate <='%2')"

                                                  " and a.ch_suitno like '%3%'"

                                                  " and (DATE_FORMAT(c.dt_operdate, '%Y') >='0')"

                                                  " and (DATE_FORMAT(c.dt_operdate, '%Y') <='3')"

                                                  " group by d.ch_dishno, d.vch_dishname, d.ch_unitno, a.ch_suitno"
                                                  )),
                                          w_rpt_report::querymodelsetfilters("")
                                          ));


}

void w_rpt_report_promotionreport::tab2sqlinit()
{

}

void w_rpt_report_promotionreport::tab3sqlinit()
{

}

void w_rpt_report_promotionreport::tab4sqlinit()
{

}

void w_rpt_report_promotionreport::tab5sqlinit()
{

}

void w_rpt_report_promotionreport::toselect()
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
    if(querymodel->rowCount() > 0) {
        ui->tableView->resizeColumnsToContents();
    }
    //更新表头
    backheader::modelSetHeader(querymodel, tab1info[ui->comboBox_11->currentText()].headers);

    ui->tableView->resizeColumnsToContents();
}

void w_rpt_report_promotionreport::toexit()
{
    this->reject();
}

void w_rpt_report_promotionreport::tocom12(const QString &/*text*/)
{

}
