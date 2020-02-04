#include "w_i_inventory_check.h"
//盘点查询
#include "ui_w_i_inventory_check_dialog.h"
#include "backheader.h"

#include <QStandardItemModel>
#include "n_func.h"
#include "lds_tableview_delegate_check.h"



QVariant w_i_inventory_check::QueryModel::data(const QModelIndex &item, int role) const
{
    if(role==Qt::DisplayRole) {
        if(item.column()==1) {
            return QSqlQueryModel::data(item, role).toDateTime().toString("yyyy-MM-dd hh:mm:ss");
        }
    }
    return QSqlQueryModel::data(item, role);
}

w_i_inventory_check::w_i_inventory_check(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_i_inventory_check_Dialog)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->frame->setFixedWidth(w_rpt_report::frame_left_fixed_width);

    selectsql =
            " SELECT a.ch_sheetNo as tabname0,"
            " a.dt_audit as tabname1,"
            " b.ch_materialno as tabname2,"
            " c.vch_dishname as tabname3,"
            " (select vch_unitname"
            "  from cey_bt_unit"
            "  where cey_bt_unit.ch_unitno = c.ch_unitno) as tabname4,"
            "  b.num_sys as tabname5,"
            "  b.num_check as tabname6,"
            "  b.num_pro_loss as tabname7,"
            "   b.chg_flag as tabname8,"
            "  b.cost_price as tabname9,"
            " round(num_sys * cost_price, 2) as tabname10,"
            " round(num_check * cost_price, 2) as tabname11"
            " FROM cey_st_check_master a, cey_st_check_detail b, cey_bt_dish c"
            "  Where a.ch_sheetNo = b.ch_sheetNo"
            " and b.ch_materialno = c.ch_dishno"
            " and a.ch_state = 'Y'"
            " and b.chg_flag = 'Y'"
            " and c.ch_sub_typeNo like '%1%'"
            " and c.ch_dishno like '%2%'"
            " and a.dt_audit >= '%3'"
            " and a.dt_audit <= '%4'"
            " ORDER BY tabname%5";


    //时间改成年月日
    ui->dateTimeEdit->setDisplayFormat("yyyy-MM-dd");
    ui->dateTimeEdit_2->setDisplayFormat("yyyy-MM-dd");
    ui->dateTimeEdit->setDate(n_func::f_get_sysdatetime().date());
    ui->dateTimeEdit_2->setDate(n_func::f_get_sysdatetime().date());

    //商品大类
    ui->comboBox_series->addItems(w_rpt_report::querymodelsetfilters("select * from cey_bt_dish_type"));

    //model
    querymodel=new QueryModel;
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);ui->tableView->setModel(querymodel);

    ui->tableView->setItemDelegate(new lds_tableview_delegate_check(QVector<int>()
                                                                    << 8,
            ui->tableView));
    //! toolbar
    ui->widget->setup(QStringList() << QObject::tr("查询") << QObject::tr("退出"), Qt::AlignTop);
    connect(ui->widget->index_widget(QObject::tr("查询")),SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->tableView->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(toselect()));

}

void w_i_inventory_check::toselect()
{
    //数据库sql
    querymodel->setQuery(
                selectsql
                .arg(backheader::getinterdata(ui->comboBox_series->currentText()))//获取过滤信息
                .arg(ui->selectgoods->gettext())
                .arg(QDateTime(ui->dateTimeEdit->date()).toString("yyyy-MM-dd hh:mm:ss"))
                .arg(QDateTime(ui->dateTimeEdit_2->date(), QTime(23,59,59)).toString("yyyy-MM-dd hh:mm:ss"))
                .arg(w_rpt_report::querymodelordersort(ui->tableView->tableView))
                );

    //更新表头
    backheader::modelSetHeader(querymodel, QStringList()<<QObject::tr("单据号码")<<QObject::tr("日期时间")<<QObject::tr("商品编号")<<QObject::tr("商品名称")<<QObject::tr("单位")<<QObject::tr("系统库存")<<QObject::tr("盘点数量")<<QObject::tr("盈亏数量")<<QObject::tr("调整库存")<<QObject::tr("单价")<<QObject::tr("系统金额")<<QObject::tr("盘点金额"));

    //更新合计
    QString sumsql=querymodel->query().lastQuery().trimmed();
    if(sumsql.endsWith("DESC")||sumsql.endsWith("ASC")) {
        //5 6 7 10 11
        lds_query_hddpos  query;
        query.execSelect(QString("select sum(tabname5),sum(tabname6),sum(tabname7),sum(tabname10),sum(tabname11)  from (%1)t1 ").arg(sumsql.left(sumsql.lastIndexOf("ORDER BY"))));
        query.next();

        ui->tableView->updateSum(5,query.recordValue(0));
        ui->tableView->updateSum(6,query.recordValue(1));
        ui->tableView->updateSum(7,query.recordValue(2));
        ui->tableView->updateSum(10,query.recordValue(3));
        ui->tableView->updateSum(11,query.recordValue(4));
    }
    ui->tableView->resizeColumnsToContents();
}

void w_i_inventory_check::toexit()
{
    this->reject();
}
