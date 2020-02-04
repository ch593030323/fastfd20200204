#include "w_rpt_report_directsalegrossprofitreport.h"
//原料毛利报表
#include "ui_w_rpt_report_directsalegrossprofitreport_dialog.h"
#include "lds_query_hddpos.h"
#include "n_func.h"
#include "lds_tableview.h"
#include <QSqlError>
#include "backheader.h"
#include <QtDebug>

w_rpt_report_directsalegrossprofitreport::w_rpt_report_directsalegrossprofitreport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_rpt_report_directsalegrossprofitreport_Dialog)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->frame->setFixedWidth(w_rpt_report::frame_left_fixed_width);
    //! init
    w_rpt_report::reportBeginDtSet(ui->dateTimeEdit);
    ui->dateTimeEdit_2->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(), QTime(23,59,59)));
    tab1sqlinit();

    querymodel=new report_querymodel(this);

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);ui->tableView->setModel(querymodel);

    //! toolbar
    ui->widget->setup(QStringList()<<QObject::tr("查询")<<QObject::tr("退出"), Qt::AlignTop);
    connect(ui->widget->index_widget(QObject::tr("查询")),SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->tableView->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(toselect()));

    //! after
    ui->comboBox_series->addItems(w_rpt_report::querymodelsetfilters("select * from cey_bt_dish_type"));
    ui->comboBox->addItem(QObject::tr("原料毛利报表"));

}

w_rpt_report_directsalegrossprofitreport::~w_rpt_report_directsalegrossprofitreport()
{

}


void w_rpt_report_directsalegrossprofitreport::tab1sqlinit()
{
    tabinfo.sql = QString (

                "select (select cey_bt_dish_type.vch_dish_typename from cey_bt_dish_type where cey_bt_dish_type.ch_dish_typeno=t.ch_dish_typeno) as 'tabname0', "

                " ch_dishno as 'tabname1', "

                " vch_dishname as 'tabname2', "

                " (select vch_unitname from cey_bt_unit where cey_bt_unit.ch_unitno=t.ch_unitno) as 'tabname3', "

                " num as 'tabname4', "

                " num_amt as 'tabname5', "

                " num_ck_amt as 'tabname6', "

                " num_amt - num_ck_amt as 'tabname7', "

                " (num_amt - num_ck_amt) / num_amt as 'tabname8' "

                " from (select C.ch_dish_typeno,"

                " B.ch_dishno,"

                " C.vch_dishname,"

                " C.ch_unitno,"

                " sum(B.num_num - B.num_back) num,"

                " sum(((B.num_num - B.num_back) *B.num_price) * int_discount * 0.01 + B.num_price_add) num_amt,"

                " sum(num_kc_amt) num_ck_amt"

                " from (select ch_sheetno,"

                " sum(-num_num * int_flag) num_kc,"

                " sum(-num_num * int_flag * cost_price) num_kc_amt"

                " from cey_st_material_flow"

                " group by ch_sheetno) A,"

                " cey_u_orderdish B,"

                " cey_bt_dish C,"

                " cey_u_checkout_master D"

                " where LEFT(A.ch_sheetno,60) = LEFT(B.int_flowid,60)"

                " and B.ch_dishno = C.ch_dishno"

                " and D.ch_payno = B.ch_payno"

                " and ch_state = 'Y'"

                " and D.dt_operdate >= '%1'"

                " and D.dt_operdate <= '%2'"

                " and C.ch_dish_typeno like '%3%'"

                " group by C.ch_dish_typeno, B.ch_dishno, C.vch_dishname, C.ch_unitno) t ORDER BY ch_dishno DESC"
                );
    tabinfo.headers = QStringList()<<QObject::tr("餐桌")<<QObject::tr("品码")<<QObject::tr("品名")<<QObject::tr("单位")<<QObject::tr("数量")<<QObject::tr("消费金额")<<QObject::tr("成本金额")<<QObject::tr("毛利额")<<QObject::tr("毛利率");
}

void w_rpt_report_directsalegrossprofitreport::toselect()
{
    //model select
    querymodel->setQuery(tabinfo.sql
            .arg(ui->dateTimeEdit->dateTimeStr())
            .arg(ui->dateTimeEdit_2->dateTimeStr())
            .arg(backheader::getinterdata(ui->comboBox_series->currentText()))//获取过滤信息
            );
    //更新表头
    backheader::modelSetHeader(querymodel, tabinfo.headers);
    //更新合计
    ui->tableView->updateSum(5);
    ui->tableView->updateSum(6);
    ui->tableView->updateSum(7);
    ui->tableView->updateSum(8);

    ui->tableView->resizeColumnsToContents();
}

void w_rpt_report_directsalegrossprofitreport::toexit()
{
    this->reject();
}
