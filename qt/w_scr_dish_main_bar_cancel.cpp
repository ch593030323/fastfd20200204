#include "w_scr_dish_main_bar_cancel.h"
#include "ui_w_scr_dish_main_bar_cancel.h"
#include "public_printby_ch_billno.h"
#include "lds_messagebox.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include <QtDebug>
#include "n_func.h"
#include "w_rpt_report_frame.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "lds_scr_table.h"
#include <QTimer>


bool w_scr_dish_main_bar_cancel::cey_bt_table_bar_dish_existed(const QString &ch_tableno, int int_div_id)
{
    lds_query_hddpos  query;
    query.execSelect(QString("SELECT ch_billno from cey_bt_table_bar where ch_tableno = '%1'  and int_div_id = %2 ")
                     .arg(ch_tableno)
                     .arg(int_div_id));
    while(query.next()) {
        if(lds_scr_table::cey_u_orderdish_dish_existed(query.recordValue("ch_billno").toString())) {
            return true;
        }
    }
    return false;
}

bool w_scr_dish_main_bar_cancel::cey_bt_table_bar_clear_transaction(const QString &ch_tableno, int int_div_id, QString *errstring)
{
    lds_query_hddpos  query;
    lds_query_hddpos::tran_saction();
    if(query.execDelete("cey_bt_table_bar", QString(" ch_tableno = '%1' and int_div_id = %2 ")
                        .arg(ch_tableno)
                        .arg(int_div_id))) {
        lds_query_hddpos::com_mit();
        return true;
    }
    lds_query_hddpos::roll_back();
    if(errstring) *errstring = query.recordError();
    return false;
}

void w_scr_dish_main_bar_cancel::cey_bt_table_bar_print(QWidget *parent, const QString &ch_tableno, int int_div_id)
{
    QStringList ch_billno_list;
    lds_query_hddpos  query;
    query.execSelect(QString("SELECT ch_billno from cey_bt_table_bar where ch_tableno = '%1' and int_div_id = %2 ")
                     .arg(ch_tableno)
                     .arg(int_div_id));
    while(query.next()) {
        if(lds_scr_table::cey_u_orderdish_dish_existed(query.recordValue("ch_billno").toString())) {
            ch_billno_list.append(query.recordValue("ch_billno").toString());
        }
    }
    //
    if(ch_billno_list.count() == 0) {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("没有需要打印的数据"));
        return;
    }
    //
    if(0 == lds_messagebox::question(parent, MESSAGE_TITLE_VOID, QObject::tr("打印消费单"), QObject::tr("是"), QObject::tr("否"))) {
        QString ch_billno = ch_billno_list.first();
        public_ins_maker print_maker;
        print_maker.print_ins += public_printby_ch_billno(ch_billno).toprint_preprint_div_title(print_maker.defpath(), QObject::tr("消费单"));

        double subtotal = 0;
        double total;
        int idvcount = 0;
        foreach(const QString &billno, ch_billno_list) {
            print_maker.drawN();
            print_maker.print_ins += public_printby_ch_billno(billno).toprint_preprint_div(print_maker.defpath(), total);
            subtotal += total;
            idvcount ++;
        }
        print_maker.drawN();
        print_maker.print_ins += public_printby_ch_billno(ch_billno).toprint_preprint_div_tail(print_maker.defpath(), subtotal, idvcount);
        print_maker.resolve();
        print_maker.write_bylanguage();
    }
}

w_scr_dish_main_bar_cancel::w_scr_dish_main_bar_cancel(const QString &ch_tableno, int int_div_id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_main_bar_cancel)
{
    ui->setupUi(this);
    this->ch_tableno = ch_tableno;
    this->int_div_id = int_div_id;

    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_u_orderdish");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);

    ui->tableView->setModel(tablemodel);
    ui->tableView->setTransferHheader();
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_suitflag"), new lds_model_sqltablemodel_delegate_com(
                                                this, ldsVariantMap("P", QObject::tr("主商品"), "Y", QObject::tr("子商品"))
                                                ));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_dishno"), new lds_model_sqltablemodel_delegate_com(
                                                this, tablemodel, "cey_bt_dish","ch_dishno","vch_dishname"
                                                ));
    ui->tableView->rePecifyHeader(QStringList() << "ch_tableno" << "ch_billno"<< "ch_dishno"<< "num_num"<< "num_back"<< "ch_suitflag");

    //connect
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this,SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this,SLOT(took()));
    connect(ui->pushButton_print, SIGNAL(clicked()), this,SLOT(toprint()));

    this->setWindowTitle(QObject::tr("清台"));
    QTimer::singleShot(0, this, SLOT(torefresh()));
}

w_scr_dish_main_bar_cancel::~w_scr_dish_main_bar_cancel()
{
    delete ui;
}

void w_scr_dish_main_bar_cancel::tocancel()
{
    this->reject();
}

void w_scr_dish_main_bar_cancel::took()
{
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认清台"), QObject::tr("确定"), QObject::tr("取消"))) {
        return;
    }
    QString errstring;
    if(cey_bt_table_bar_clear_transaction(ch_tableno, int_div_id, &errstring)) {
        this->accept();
        return;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
}

void w_scr_dish_main_bar_cancel::toprint()
{
    cey_bt_table_bar_print(this, ch_tableno, int_div_id);
}

void w_scr_dish_main_bar_cancel::torefresh()
{
    lds_query_hddpos  query;
    QStringList inch_billnos;
    query.execSelect(QString("select ch_billno from cey_bt_table_bar where ch_tableno = '%1' and int_div_id = %2 ")
                     .arg(ch_tableno)
                     .arg(int_div_id));
    while(query.next()) {
        inch_billnos << query.recordValue("ch_billno").toString();
    }

    //select
    tablemodel->setFilter(QString("ch_billno in ('%1')")
                          .arg(inch_billnos.join("','")));
    ui->tableView->resizeColumnsToContents();
    lds_tableview_virtual::tableviewSetpan(ui->tableView, tablemodel->fieldIndex("ch_tableno"),
                                           QList<int>() << tablemodel->fieldIndex("ch_tableno"));
    lds_tableview_virtual::tableviewSetpan(ui->tableView, tablemodel->fieldIndex("ch_billno"),
                                           QList<int>() << tablemodel->fieldIndex("ch_billno"), false);
}

w_scr_dish_main_bar_cancel_print::w_scr_dish_main_bar_cancel_print(const QString &ch_tableno, int int_div_id, QWidget *parent)
    : w_scr_dish_main_bar_cancel(ch_tableno, int_div_id, parent)
{
    ui->pushButton_ok->hide();
}

w_scr_dish_main_bar_cancel_print::~w_scr_dish_main_bar_cancel_print()
{

}
