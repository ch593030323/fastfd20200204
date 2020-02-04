#include "back_anti_checkout.h"
#include "ui_back_anti_checkout.h"
#include <QSqlRecord>
#include "lds_tableview_delegate_check.h"
#include <QDesktopWidget>
#include <QApplication>
#include "n_func.h"
#include <QtDebug>
#include "w_rpt_report.h"


back_anti_checkout::back_anti_checkout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::back_anti_checkout)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    model_top = new QStandardItemModel(this);
    model_top->setHorizontalHeaderLabels(QStringList() <<QObject::tr("序号")<<QObject::tr("单号")<<QObject::tr("结帐号")<<QObject::tr("消费总额")<<QObject::tr("操作员")<<QObject::tr("时间")<<QObject::tr("新结帐号")<<QObject::tr("桌号")<<QObject::tr("操作员"));
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setModel(model_top);
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView->setSelectionMode(QTableView::SingleSelection);

    model_left = new QStandardItemModel(this);
    model_left->setHorizontalHeaderLabels(QStringList() <<QObject::tr("行号")<<QObject::tr("品号")<<QObject::tr("品名")<<QObject::tr("单位")<<QObject::tr("价格")<<QObject::tr("加价")<<QObject::tr("数量")<<QObject::tr("折扣")<<QObject::tr("税率")<< QObject::tr("税率") << QObject::tr("金额") << QObject::tr("赠送")<<QObject::tr("特价"));
    ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_2->setModel(model_left);
    ui->tableView_2->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView_2->setSelectionMode(QTableView::SingleSelection);

    model_right = new QStandardItemModel(this);
    model_right->setHorizontalHeaderLabels(QStringList() <<QObject::tr("付款方式")<<QObject::tr("付款金额")<<QObject::tr("找零")<<QObject::tr("实收")<<QObject::tr("面额")<<QObject::tr("凭证"));
    ui->tableView_3->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_3->setModel(model_right);
    ui->tableView_3->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView_3->setSelectionMode(QTableView::SingleSelection);

    //

    QDateTime dt =n_func::f_get_sysdatetime();
//    w_rpt_report::reportBeginDtSet(ui->dateTimeEdit);
//    ui->dateTimeEdit->setDateTime(QDateTime(dt.date(), QTime(0,0,0)));
//    ui->dateTimeEdit_2->setDateTime(QDateTime(dt.date(), QTime(23,59,59)));
    //
    init_control();
    w_rpt_report::reportBeginDtSet(ui->dateTimeEdit);
//    ui->dateTimeEdit->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(), QTime(0,0,0)));
    ui->dateTimeEdit_2->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(), QTime(23,59,59)));
    ui->pushButton_export->hide();
}

back_anti_checkout::~back_anti_checkout()
{
    delete ui;
}

void back_anti_checkout::init_control()
{
    QSqlQueryModel model;
    QString date1 = ui->dateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString date2 = ui->dateTimeEdit_2->dateTime().toString("yyyy-MM-dd hh:mm:ss");


    model_top->removeRows(0, model_top->rowCount());
    model.setQuery(QString("SELECT DISTINCT b.ch_billno,b.ch_payno_un,b.num_cost_un,b.vch_operid,b.dt_operdate,a.ch_payno,a.ch_tableno,a.vch_operID FROM cey_u_orderdish a,cey_u_uncheckout_log b WHERE(b.ch_billno = a.ch_billno)and(b.dt_operdate >= '%1')AND(b.dt_operdate < '%2' )").arg(date1).arg(date2));

    for(int i = 0; i < model.rowCount(); i++)
    {
        int row = model_top->rowCount();
        model_top->setRowCount(row + 1);
        model_top->setItem(row,0,new QStandardItem(QString::number(row+1))); //序号
        model_top->setItem(row,1,new QStandardItem(model.record(i).value(0).toString())); //单号
        model_top->setItem(row,2,new QStandardItem(model.record(i).value(1).toString()));//结帐号
        model_top->setItem(row,3,new QStandardItem(QString().sprintf("%0.2f",model.record(i).value(2).toFloat())));//消费总额
        model_top->setItem(row,4,new QStandardItem(model.record(i).value(3).toString()));//操作员员
        model_top->setItem(row,5,new QStandardItem(model.record(i).value(4).toDateTime().toString("yyyy-MM-dd hh:mm:ss")));//时间
        model_top->setItem(row,6,new QStandardItem(model.record(i).value(5).toString()));//新结帐号
        model_top->setItem(row,7,new QStandardItem(model.record(i).value(6).toString()));//桌号
        model_top->setItem(row,8,new QStandardItem(model.record(i).value(7).toString()));//点菜人
    }


    if(ui->tableView->verticalHeader()->count() > 0) {
        ui->tableView->resizeColumnsToContents();
        ui->tableView->setCurrentIndex(model_top->index(0, 0));
    }
    on_tableView_clicked(ui->tableView->currentIndex());
}

void back_anti_checkout::on_pushButton_12_clicked()
{
    this->close();
}

void back_anti_checkout::on_tableView_clicked(const QModelIndex &index)
{
    Q_UNUSED(index)

    QSqlQueryModel model;
    int row = ui->tableView->currentIndex().row();
    if(row <= -1)
    {
        return;
    }
    if(model_top->rowCount() > 0)
    {
        model_left->removeRows(0, model_left->rowCount());
        model.setQuery(QString("SELECT a.ch_dishno,IFNULL(b.vch_dishname,a.ch_dishno),(select d.vch_unitname from cey_bt_unit d where d.ch_unitno=b.ch_unitno)as k,a.num_num,a.num_price,a.num_price_add,a.int_discount,a.ch_presentflag,a.ch_specialflag,b.int_rate,a.num_num*(b.int_rate*b.num_price*0.01) FROM cey_u_orderdish a,cey_bt_dish b,cey_u_uncheckout_log c WHERE  a.ch_dishno = b.ch_dishno  and  a.ch_billno = c.ch_billno and a.ch_billno = '%1'").arg(model_top->item(row,  1)->text()));

        for(int i = 0; i < model.rowCount(); i++)
        {
            int row = model_left->rowCount();
            model_left->setRowCount(row + 1);
            model_left->setItem(row,0,new QStandardItem(QString::number(row+1))); //序号
            model_left->setItem(row,1,new QStandardItem(model.record(i).value(0).toString())); //品码
            model_left->setItem(row,2,new QStandardItem(model.record(i).value(1).toString())); //品名
            model_left->setItem(row,3,new QStandardItem(model.record(i).value(2).toString())); //单位
            model_left->setItem(row,4,new QStandardItem(QString().sprintf("%0.2f",model.record(i).value(4).toFloat()))); //价格
            model_left->setItem(row,5,new QStandardItem(QString().sprintf("%0.2f",model.record(i).value(5).toFloat()))); //加价
            model_left->setItem(row,6,new QStandardItem(QString().sprintf("%0.2f",model.record(i).value(3).toFloat()))); //数量
            QString zhe = QObject::tr("折扣") +":" +model.record(i).value(6).toString() + "%";
            model_left->setItem(row,7,new QStandardItem(zhe)); //折扣
            model_left->setItem(row,8,new QStandardItem(model.record(i).value(9).toString())); //税率
            model_left->setItem(row,9,new QStandardItem(QString().sprintf("%0.2f",model.record(i).value(10).toFloat()))); //税率
            float jine = 0;

            //            jine = model.record(i).value(3).toFloat() * model.record(i).value(4).toFloat() * model.record(i).value(6).toFloat() * 0.01 + model.record(i).value(5).toFloat() + model.record(i).value(10).toFloat();

            jine = model.record(i).value("num_num").toFloat() * model.record(i).value("num_price").toFloat() * model.record(i).value("int_discount").toFloat() * 0.01 + model.record(i).value("num_price").toFloat() + model.record(i).value("int_rate").toFloat();

            model_left->setItem(row,10,new QStandardItem(QString().sprintf("%0.2f",jine))); //金额
            model_left->setItem(row,11,new QStandardItem(model.record(i).value(7).toString())); //赠送
            model_left->setItem(row,12,new QStandardItem(model.record(i).value(8).toString())); //特价
        }
    }

    ui->tableView_2->selectRow(0);
    if(ui->tableView_2->verticalHeader()->count() > 0) {
        ui->tableView_2->resizeColumnsToContents();
    }

    if(model_top->rowCount() > 0)
    {
        model_right->removeRows(0, model_right->rowCount());
        model.setQuery(QString("SELECT IFNULL((select b.vch_paymodename from cey_bt_paymode b where b.ch_paymodeno=a.ch_paymodeno),a.ch_paymodeno),a.num_payamount, a.num_change, a.num_realamount, a.num_face, a.vch_voucherno  FROM cey_u_checkout_detail a  WHERE  a.ch_payno ='%1'").arg(model_top->item(0,2)->text()));
        for(int i = 0; i < model.rowCount(); i++)
        {
            int row = model_right->rowCount();
            model_right->setRowCount(row + 1);
            model_right->setItem(row,0,new QStandardItem(model.record(i).value(0).toString())); //付款方式
            model_right->setItem(row,1,new QStandardItem(QString().sprintf("%0.2f",model.record(i).value(1).toFloat())));//付款金额
            model_right->setItem(row,2,new QStandardItem(QString().sprintf("%0.2f",model.record(i).value(2).toFloat())));//找零
            model_right->setItem(row,3,new QStandardItem(QString().sprintf("%0.2f",model.record(i).value(3).toFloat())));//实收金额
            model_right->setItem(row,4,new QStandardItem(QString().sprintf("%0.2f",model.record(i).value(4).toFloat())));//面额
            QString str;
            if(model.record(i).value(5).toString() != "null")
            {
                str = model.record(i).value(5).toString();
            }
            model_right->setItem(row,5,new QStandardItem(str));//凭证
        }
    }
    ui->tableView_3->selectRow(0);
    if(ui->tableView_3->verticalHeader()->count() > 0) {
        ui->tableView_3->resizeColumnsToContents();
    }
}

void back_anti_checkout::on_pushButton_10_clicked()
{
    init_control();
    return;
}

void back_anti_checkout::on_pushButton_11_clicked()//导出
{

}
