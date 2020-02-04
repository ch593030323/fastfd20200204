#include "take_cashier.h"
#include "ui_take_cashier.h"
#include <QStandardItem>
#include "lds_tableview_delegate_check.h"
#include "lds_query_hddpos.h"
#include <QDateTime>
#include "lds_messagebox.h"
#include "n_func.h"
#include "lds_messagebox.h"
#include "waimai_pay.h"
#include <QSqlRecord>
#include "public_mainprinter.h"
#include "backheader.h"
#include "lds_messagebox.h"
#include "public_sql.h"
#include <QSqlRecord>
#include "lds_model_sqltablemodel_delegate_com.h"
#include "fexpandmain_model_sqltablemodel_data.h"
#include "fexpandmain_delegate_vch_printmemo.h"

take_cashier::take_cashier(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::take_cashier) {
    ui->setupUi(this);

    init_control();
    row = 0;
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->tableView_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setColumnWidth(1, 200);
    ui->tableView->setColumnHidden(0, true);
}

take_cashier::~take_cashier()
{
    delete ui;
}

void take_cashier::init_control()
{
    QStringList labels;
    labels<<QObject::tr("行号")<<QObject::tr("单号") << QObject::tr("付款方式")<<QObject::tr("消费")<<QObject::tr("税率")<<QObject::tr("折扣")<<QObject::tr("赠送")
         <<QObject::tr("抹零")<<QObject::tr("应收")<<QObject::tr("订餐人")<<QObject::tr("地址")<<QObject::tr("电话") << "sdsf" <<QObject::tr("送餐人");
    model_top = new SqlQueryModel_Linenum1();
    model_top->setQuery(QString("SELECT 'id' = 1, cey_u_togo.ch_togono, cey_u_togo.ch_payno, cey_u_togo.num_cost, cey_u_togo.num_rate,cey_u_togo.num_discount, cey_u_togo.num_present, cey_u_togo.num_blotout, cey_u_togo.num_cost + cey_u_togo.num_rate -cey_u_togo.num_discount  - cey_u_togo.num_present -  cey_u_togo.num_blotout, cey_u_togo.vch_linkman,cey_u_togo.vch_address , cey_u_togo.vch_tel, cey_u_togo.ch_state, cey_u_togo.vch_operID, cey_u_togo.vch_waiter, cey_u_togo.dt_operdate,cey_u_togo.ch_billno  FROM cey_u_togo  WHERE(cey_u_togo.ch_state = 'N')  "));

    model_top->setlist(labels);
    ui->tableView->setModel(model_top);

    ui->tableView->selectRow(0);

    model_bottom = new QSqlQueryModel(this);
    ui->tableView_2->setModel(model_bottom);
    ui->tableView_2->setItemDelegateForColumn(0, new lds_model_sqltablemodel_delegate_com(this,
                                                                                          "cey_bt_dish", "ch_dishno", "vch_dishname"));
    ui->tableView_2->setItemDelegateForColumn(7, new fexpandmain_delegate_vch_printmemo(this));


    QTimer::singleShot(100, this, SLOT(toselectDish()));
}

void take_cashier::on_pushButton_5_clicked()//退出
{
    this->close();
}

void take_cashier::on_tableView_clicked(const QModelIndex &index)
{
    Q_UNUSED(index)
    row = ui->tableView->currentIndex().row();
    int row1 = ui->tableView->currentIndex().row();
    if(row1 <= -1)
    {
        return;
    }
    toselectDish(model_top->record(row1).value(16).toString());

}

void take_cashier::on_pushButton_3_clicked()//
{
    lds_query_hddpos  query;
    lds_query_hddpos::tran_saction();

    int row = ui->tableView->currentIndex().row();
    if(row <= -1)
    {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        
        return;
    }

    QString waimaidanhao=model_top->record(row).value(1).toString();
    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("外卖退货"),QObject::tr("确定"),QObject::tr("取消")))
    {
        QSqlQueryModel model;
        model.setQuery(QString("Select NOW()"));
        QString timer = model.record(0).value(0).toDateTime().toString("yyyy-MM-dd hh:mm:ss");

        if(query.execUpdate("cey_u_togo",
                            qrtVariantPairList()
                            << qrtVariantPair("ch_state", "P")
                            << qrtVariantPair("vch_operID", lds::gs_operid)
                            << qrtVariantPair("dt_operdate", timer),
                            qrtEqual("ch_togono", waimaidanhao)
                            )) {

        }  else  {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
            
            return;
        }

        QList<qrtVariantPair>() << qrtVariantPair("num_num", 0);

        if(query.execUpdate("cey_u_orderdish",qrtVariantPairList()

                            << qrtVariantPair("num_back",  qrtFunc("num_num"))
                            << qrtVariantPair("num_num",  0)
                            << qrtVariantPair("vch_back_operID", lds::gs_operid)
                            << qrtVariantPair("dt_back_operdate", timer),
                            qrtEqual("ch_togono", model_top->record(row).value(1).toString())
                            ))
        {

        }   else
        {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
            
            return;
        }

        lds_query_hddpos::com_mit();

        model_top->clear();
        model_top->setQuery(QString("SELECT 'id' = 1, cey_u_togo.ch_togono, cey_u_togo.ch_payno, cey_u_togo.num_cost, cey_u_togo.num_rate,cey_u_togo.num_discount, cey_u_togo.num_present, cey_u_togo.num_blotout, cey_u_togo.num_cost + cey_u_togo.num_rate -cey_u_togo.num_discount  - cey_u_togo.num_present -  cey_u_togo.num_blotout, cey_u_togo.vch_linkman,cey_u_togo.vch_address , cey_u_togo.vch_tel, cey_u_togo.ch_state, cey_u_togo.vch_operID, cey_u_togo.vch_waiter, cey_u_togo.dt_operdate,cey_u_togo.ch_billno  FROM cey_u_togo  WHERE(cey_u_togo.ch_state = 'N')  "));
        if(row >= model_top->rowCount())
        {
            ui->tableView->selectRow(model_top->rowCount() -1);
        }  else
        {
            ui->tableView->selectRow(row);
        }

        if(model_top->rowCount() > 0) {
            toselectDish(model_top->record(ui->tableView->currentIndex().row()).value(16).toString());
        } else {
            model_bottom->clear();
        }
    }
}

void take_cashier::on_pushButton_4_clicked()//付款
{
    int row = ui->tableView->currentIndex().row();
    if(row <= -1)
    {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        
        return;
    }

    QString str1 = QString().sprintf("%0.2f",model_top->record(row).value(3).toFloat());//消费
    QString str2 = QString().sprintf("%0.2f",model_top->record(row).value(4).toFloat());//税率
    QString str3 = QString().sprintf("%0.2f",model_top->record(row).value(5).toFloat());//折扣
    QString str4 = QString().sprintf("%0.2f",model_top->record(row).value(6).toFloat());//赠送
    QString str5 = QString().sprintf("%0.2f",model_top->record(row).value(7).toFloat());//抹零


    QString str6 = model_top->record(row).value(16).toString();//单号
    QString str7 = model_top->record(row).value(1).toString();//外卖号
    waimai_pay waimaidialog(str1,str3,str4,str5,str2,str6,str7,this);
    waimaidialog.setWindowTitle(QObject::tr("快餐付款"));
    transparentCenterDialog(&waimaidialog).exec();
    if(waimaidialog.btn_t_f == true)
    {
        model_top->clear();
        model_top->setQuery(QString("SELECT 'id' = 1, cey_u_togo.ch_togono, cey_u_togo.ch_payno, cey_u_togo.num_cost, cey_u_togo.num_rate,cey_u_togo.num_discount, cey_u_togo.num_present, cey_u_togo.num_blotout, cey_u_togo.num_cost + cey_u_togo.num_rate -cey_u_togo.num_discount  - cey_u_togo.num_present -  cey_u_togo.num_blotout, cey_u_togo.vch_linkman,cey_u_togo.vch_address , cey_u_togo.vch_tel, cey_u_togo.ch_state, cey_u_togo.vch_operID, cey_u_togo.vch_waiter, cey_u_togo.dt_operdate,cey_u_togo.ch_billno  FROM cey_u_togo  WHERE(cey_u_togo.ch_state = 'N')  "));
        if(row >= model_top->rowCount())
        {
            ui->tableView->selectRow(model_top->rowCount() -1);
        }    else
        {
            ui->tableView->selectRow(row);
        }

        if(model_top->rowCount() > 0) {
            toselectDish(model_top->record(ui->tableView->currentIndex().row()).value(16).toString());
        }  else {
            model_bottom->clear();
        }
    }
}

void take_cashier::toselectDish(const QString &ch_billno)
{
    //model_top->record(0).value(16).toString()
    model_bottom->setQuery(QString("SELECT "
                                   "a.ch_dishno as `"+QObject::tr("品名")+"`"
                                   ",a.num_num - a.num_back as `"+QObject::tr("数量")+"`"
                                   ",a.num_price as `"+QObject::tr("价格")+"`"
                                   ",a.int_discount as `"+QObject::tr("折扣")+"`"
                                   ",a.ch_presentflag as `"+QObject::tr("赠送")+"`"
                                   ",a.num_price_add as `"+QObject::tr("加价")+"`"
                                   ",a.num_price * (a.num_num - a.num_back) * a.int_discount * 0.01 + a.num_price_add as `"+QObject::tr("金额")+"`"
                                   ",a.vch_print_memo as `"+QObject::tr("特殊做法")+"`"
                                   " FROM cey_u_orderdish a WHERE a.ch_billno='%1' ")
                           .arg(ch_billno));
    ui->tableView_2->resizeColumnsToContents();
    ui->tableView_2->selectRow(0);
}

void take_cashier::toselectDish()
{
    int row = ui->tableView->currentIndex().row();
    if(row >= 0) {
        toselectDish(model_top->record(row).value(16).toString());
    }
}
