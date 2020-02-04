#include "w_i_inventory_summaryquery.h"
//单据汇总查询
#include "ui_w_i_inventory_summaryquery_dialog.h"
#include "backheader.h"
#include "n_func.h"
#include "lds_tableview.h"

w_i_inventory_summaryquery::w_i_inventory_summaryquery(QWidget *parent) :
    QDialog(parent),ui(new Ui_w_i_inventory_summaryquery_Dialog)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->frame->setFixedWidth(w_rpt_report::frame_left_fixed_width);
    //total queryselect
    selectsqlpart0=
            "SELECT ch_materialno as tabname0,"

            " vch_dishname as tabname1,"

            " unitname as tabname2,"

            " ch_sub_typeno as tabname3,"

            " sum_total/sum_num as tabname4,"

            " sum_num as tabname5,"

            " sum_total as tabname6"

            " from(%1)t "

            " ORDER BY tabname%5";
    ;

    selectsqlpart1=
            "SELECT b.ch_materialno, "

            " c.vch_dishname, "

            " (select vch_unitname from cey_bt_unit where cey_bt_unit.ch_unitno=c.vch_pur_unitno) as unitname,"

            " c.ch_sub_typeno, "

            " c.ch_dish_typeno, "

            " sum(b.num_num) as sum_num,"

            " Round(sum(b.num_num * b.num_price), 2) as sum_total"

            " FROM cey_bt_dish c, cey_st_sheet_detail b, cey_st_sheet_master a"

            " WHERE (c.ch_dishno = b.ch_materialno)"

            " and (b.ch_sheetno = a.ch_sheetno)"

            " and (a.ch_sheettype = '%1')"
            ;
    selectsqlpart2=
            " and a.ch_state = 'Y'"

            " and a.dt_audit >= '%1' "

            " and a.dt_audit <= '%2' "

            " and c.ch_sub_typeNo like '%3%' "

            " and b.ch_materialno like '%4%' "

            " Group by b.ch_materialno,"

            " c.vch_dishname,"

            " c.vch_pur_unitno,"

            " c.ch_dish_typeno,"

            " c.ch_sub_typeno"
            ;
    //时间改成年月日
    ui->dateTimeEdit->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(),  QTime(0,0,0)));
    ui->dateTimeEdit_2->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(),  QTime(23,59,59)));

    //商品大类
    ui->comboBox_series->addItems(w_rpt_report::querymodelsetfilters("select * from cey_bt_dish_type"));

    //报表类型
    tab1sqlinit();
    ui->comboBox_type->addItems(tab1info.keys());
    //model
    querymodel=new QSqlQueryModel;
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);ui->tableView->setModel(querymodel);
//    ui->tableView->setSortingEnabled(true);
    //! toolbar
    ui->widget->setup(QStringList() << QObject::tr("查询") << QObject::tr("退出"), Qt::AlignTop);
    connect(ui->widget->index_widget(QObject::tr("查询")),SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->tableView->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(toselect()));

}

void w_i_inventory_summaryquery::tab1sqlinit()
{
    tab1info.insert(QObject::tr("采购入库汇总查询"),
                    w_rpt_report::TABTYPE(QStringList()<<QObject::tr("商品编号")<<QObject::tr("商品名称")<<QObject::tr("单位")<<QObject::tr("商品小类")<<QObject::tr("入库均价")<<QObject::tr("入库数量")<<QObject::tr("入库金额"),
                                          QString (
                                              selectsqlpart0.arg(
                                                  selectsqlpart1.arg("RK")+
                                                  selectsqlpart2)
                                              )
                                          )
                    );
    tab1info.insert(QObject::tr("采购退货汇总查询"),
                    w_rpt_report::TABTYPE(QStringList()<<QObject::tr("商品编号")<<QObject::tr("商品名称")<<QObject::tr("单位")<<QObject::tr("商品小类")<<QObject::tr("入库均价")<<QObject::tr("入库数量")<<QObject::tr("入库金额"),
                                          QString (
                                              selectsqlpart0.arg(
                                                  selectsqlpart1.arg("TH")+
                                                  selectsqlpart2)
                                              )
                                          )
                    );
    tab1info.insert(QObject::tr("报损单汇总查询"),
                    w_rpt_report::TABTYPE(QStringList()<<QObject::tr("商品编号")<<QObject::tr("商品名称")<<QObject::tr("单位")<<QObject::tr("商品小类")<<QObject::tr("入库均价")<<QObject::tr("入库数量")<<QObject::tr("入库金额"),
                                          QString (
                                              selectsqlpart0.arg(
                                                  selectsqlpart1.arg("CK")+
                                                  selectsqlpart2)
                                              )
                                          )
                    );
}

void w_i_inventory_summaryquery::toselect()
{
    //数据库sql
    querymodel->setQuery(tab1info[ui->comboBox_type->currentText()].sql
            .arg(ui->dateTimeEdit->dateTimeStr())
            .arg(ui->dateTimeEdit_2->dateTimeStr())
            .arg(backheader::getinterdata(ui->comboBox_series->currentText()))//获取过滤信息
            .arg(ui->selectgoods->gettext())
            .arg(w_rpt_report::querymodelordersort(ui->tableView->tableView))
            );

    //更新表头
    backheader::modelSetHeader(querymodel, tab1info[ui->comboBox_type->currentText()].headers);
    //更新合计
    QString sumsql=querymodel->query().lastQuery().trimmed();
    if(sumsql.endsWith("DESC")||sumsql.endsWith("ASC")) {
        //5 6 7  9 10
        lds_query_hddpos  query;
        query.execSelect(QString("select sum(tabname5),sum(tabname6) from (%1)t1 ").arg(sumsql.left(sumsql.lastIndexOf("ORDER BY"))));
        query.next();

        ui->tableView->updateSum(5,query.recordValue(0));
        ui->tableView->updateSum(6,query.recordValue(1));
        ui->tableView->resizeColumnsToContents();
    }
}

void w_i_inventory_summaryquery::toexit()
{
    this->reject();
}
