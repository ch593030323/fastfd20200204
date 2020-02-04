#include "bank_card.h"
#include "ui_bank_card.h"
#include "lds_messagebox.h"
#include <QDateTime>
#include "backheader.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "n_func.h"
#include <QTimer>
#include "w_sys_manage_cloudsync.h"

bank_card::bank_card(const QString &ch_paymodeno, double num_pay, double num_cost, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::bank_card)
{
    ui->setupUi(this);
    this->ch_paymodeno = ch_paymodeno;
    this->num_pay = num_pay;
    this->num_cost = num_cost;
    init_control();

    ui->lineEdit_pay->setValue(num_pay);
    ui->lineEdit_real_pay->setValue(num_pay);
    lds::hideWidget(ui->pushButton_online_coupon);

    if(ch_paymodeno == "04") {
        lds::showWidget(ui->pushButton_online_coupon);
    }
    //
    QTimer::singleShot(0, this, SLOT(todelayfocus()));
}

bank_card::~bank_card()
{
    delete ui;
}

void bank_card::init_control()
{
    ui->lineEdit_pay->setEnabled(false);
}

double bank_card::getRealPay()
{
    return ui->lineEdit_real_pay->value();
}

QString bank_card::getVoucher()
{
    return ui->lineEdit_voucher->text();
}

QString bank_card::getMemo()
{
    return ui->lineEdit_vch_memo->text();
}

void bank_card::on_pushButton_4_clicked()//确定
{
    this->accept();
}

void bank_card::on_pushButton_5_clicked()//取消
{
    this->reject();
}

void bank_card::todelayfocus()
{
    ui->lineEdit_real_pay->setFocus();
    ui->lineEdit_real_pay->selectAll();
}

void bank_card::on_pushButton_online_coupon_clicked()
{
    lds_dialog_input dialog(this);
    dialog.setWindowTitle(ui->pushButton_online_coupon->text());
    dialog.ui->label->setText("");
    if(QDialog::Accepted != transparentCenterDialog(&dialog).exec()) {
        return;
    }

    QString orderSn = dialog.ui->lineEdit->text().trimmed().replace(" ", "");
    QString errstring;
    QDate useDate;
    double money;
    double use;
    lds_messagebox_loading_show loading(this, "", "LOADING");
    loading.show();

    if(false == w_sys_manage_cloudsync::get_yun_coupon(useDate, money, use, orderSn, errstring) ) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }

    QString memo = QObject::tr("使用限制") + ":" +  (use <= 0 ? QObject::tr("任意金额") : (QObject::tr("消费满") + QString::number(use)));
    QString msg;
    msg += QString("<p  style=\" margin:0px\">%1</p>").arg(
                QObject::tr("优惠券面额") + ":" +  lds::septNumber(money, 2));
    if(use > num_cost)
        msg += QString("<p  style=\" margin:0px\"><span style=\" color:#ff546b;\">%1</span></p>").arg(
                    memo + "(" +  QObject::tr("消费金额") + ":" +  lds::septNumber(num_cost, 2) + ")");
    else
        msg += QString("<p  style=\" margin:0px\">%1</p>").arg(
                    memo );
    msg += QString("<p  style=\" margin:0px\">%1</p>").arg(
                QObject::tr("截止日期") + ":" +  useDate.toString("yyyy-MM-dd"));

    if(use > num_cost) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, msg, QObject::tr("无法使用"));
        return;
    }
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, msg, QObject::tr("确定使用"), QObject::tr("取消"))) {
        return;
    }
    ui->lineEdit_real_pay->setValue(money);
    ui->lineEdit_voucher->setText(orderSn);
    ui->lineEdit_vch_memo->setText(memo);

    if(!w_sys_manage_cloudsync::verify_yun_coupon(orderSn, errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
    this->accept();
}
