#include "w_bt_dish_paymode_liantuofu.h"
#include "ui_w_bt_dish_paymode_liantuofu.h"
#include "lds_query_hddpos.h"
#include "lds_messagebox.h"
#include "liantuofu_pay.h"
#include "lds.h"


w_bt_dish_paymode_liantuofu::w_bt_dish_paymode_liantuofu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_bt_dish_paymode_liantuofu)
{
    ui->setupUi(this);

    lds_query_hddpos  query;
    query.execSelect("select * FROM cey_bt_paymode where ch_paymodeno = 'EE' ");
    query.next();
    ui->lineEdit_appId->setText(query.recordValue("vch_arg1").toString());
    ui->lineEdit_key->setText(query.recordValue("vch_arg2").toString());
    ui->lineEdit_merchantCode->setText(query.recordValue("vch_arg3").toString());
    this->setWindowTitle(QObject::tr("联拓富"));

    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_demo, SIGNAL(clicked()), this, SLOT(todemo()));
    connect(ui->pushButton_test, SIGNAL(clicked()), this, SLOT(totest()));

#ifdef QT_DEBUG
#else
    lds::hideWidget(ui->pushButton_demo, true);
#endif
}

w_bt_dish_paymode_liantuofu::~w_bt_dish_paymode_liantuofu()
{
    delete ui;
}

void w_bt_dish_paymode_liantuofu::took()
{
    lds_query_hddpos::tran_saction();
    lds_query_hddpos  query;
    if(false == query.execUpdate("cey_bt_paymode", qrtVariantPairList()
                                 << qrtVariantPair("vch_arg1", ui->lineEdit_appId->text().trimmed())
                                 << qrtVariantPair("vch_arg2", ui->lineEdit_key->text().trimmed())
                                 << qrtVariantPair("vch_arg3", ui->lineEdit_merchantCode->text().trimmed()),
                                 "ch_paymodeno = 'EE'"
                                 )) {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
        return;
    }
    lds_query_hddpos::com_mit();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
    this->accept();
}

void w_bt_dish_paymode_liantuofu::tocancel()
{
    this->reject();
}

void w_bt_dish_paymode_liantuofu::todemo()
{
    ui->lineEdit_appId->setText("EW_N5946005323");
    ui->lineEdit_merchantCode->setText("EW_N4130797151");
    ui->lineEdit_key->setText("f0329e22fb506a4e26ccb29b0a6c5af3");
}

void w_bt_dish_paymode_liantuofu::totest()
{
    QString errstring;
    QByteArray url;
    liantuofu_pay payObj;
    if(payObj.jspay(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"),
                    ui->lineEdit_appId->text().trimmed(),
                    ui->lineEdit_merchantCode->text().trimmed(),
                    ui->lineEdit_key->text().trimmed(),
                    "0.01",
                    url,
                    errstring)) {
        lds_messagebox::information(this,MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        return;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
}
