#include "w_bt_dish_paymode_ccb.h"
#include "ui_w_bt_dish_paymode_ccb.h"
#include "lds_query_hddpos.h"
#include "lds_messagebox.h"
#include "lds.h"

w_bt_dish_paymode_CCB::w_bt_dish_paymode_CCB(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_bt_dish_paymode_CCB)
{
    ui->setupUi(this);

    this->setWindowTitle(QObject::tr("龙支付"));
    lds_query_hddpos  query;
    query.execSelect(" select * from cey_bt_paymode where ch_paymodeno = 'CC' ");
    query.next();
    ui->lineEdit_ccb_BRANCHID->setText(query.recordValue("vch_arg1").toString());
    ui->lineEdit_ccb_MERCHANTID->setText(query.recordValue("vch_arg2").toString());
    ui->lineEdit_ccb_POSID->setText(query.recordValue("vch_arg3").toString());
    ui->lineEdit_ccb_PUB->setText(query.recordValue("vch_arg5").toString());
    ui->lineEdit_ccb_QUPWD->setText(query.recordValue("vch_arg6").toString());


    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_ccb_default, SIGNAL(clicked()), this, SLOT(todefault()));

    lds::hideWidget(ui->pushButton_ccb_default, true);
}

w_bt_dish_paymode_CCB::~w_bt_dish_paymode_CCB()
{
    delete ui;
}

void w_bt_dish_paymode_CCB::todefault()
{
    ui->lineEdit_ccb_BRANCHID->setText("320000000");
    ui->lineEdit_ccb_MERCHANTID->setText("105303000004292");
    ui->lineEdit_ccb_POSID->setText("008980292");
    ui->lineEdit_ccb_PUB->setText("db78d479a09597376265f30d020111");
    ui->lineEdit_ccb_QUPWD->setText("888888");
}

void w_bt_dish_paymode_CCB::took()
{
    lds_query_hddpos  query;
    QString errstring;
    lds_query_hddpos::tran_saction();

    if(!query.execUpdate("cey_bt_paymode", qrtVariantPairList()
                     << qrtVariantPair("vch_arg1", ui->lineEdit_ccb_BRANCHID->text().trimmed())
                     << qrtVariantPair("vch_arg2", ui->lineEdit_ccb_MERCHANTID->text().trimmed())
                     << qrtVariantPair("vch_arg3", ui->lineEdit_ccb_POSID->text().trimmed())
                     << qrtVariantPair("vch_arg5", ui->lineEdit_ccb_PUB->text().trimmed())
                     << qrtVariantPair("vch_arg6", ui->lineEdit_ccb_QUPWD->text().trimmed()),
                     qrtEqual("ch_paymodeno", "CC"),
                        &errstring
                     )) {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    lds_query_hddpos::com_mit();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
    this->accept();
}

void w_bt_dish_paymode_CCB::tocancel()
{
    this->reject();
}
