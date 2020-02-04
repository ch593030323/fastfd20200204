#include "pay_helper_pay.h"
#include "ui_pay_helper_pay.h"
#include "alipay_help.h"
#include "mobile_pay_help.h"
#include <QSettings>
#include <QMessageBox>
#include "testwx_help.h"
#include <QDateTime>
#include <QClipboard>
#include <QTimer>
#include <pay_helper_loading.h>

pay_helper_pay::pay_helper_pay(const QString &fee, const QString &payment, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::pay_helper_pay),
    payment(payment),
    fee(fee)
{
    ui->setupUi(this);

    qDebug() << __FILE__ << __LINE__ << payment;
    sysconf = new QSettings(qApp->applicationDirPath() + "/conf.ini", QSettings::IniFormat, this);
    QString curdt = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
    out_trade_no_a  = curdt + "1";
    out_trade_no_ma = curdt + "2";
    out_trade_no_mw = curdt + "3";
    out_trade_no_b_a = curdt + "4";
    out_trade_no_b_ma = curdt + "5";
    out_trade_no_b_mw = curdt + "6";
    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(tocodepay()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));

    QTimer::singleShot(100, this, SLOT(tomakeUrl()));
}

pay_helper_pay::~pay_helper_pay()
{
    delete ui;
}

void pay_helper_pay::tocheckpay()
{
    QString errstring;
    if(payment == "a") {
        QString app_id = sysconf->value("pay_helper_set/alipay_appid").toString();
        QString rsa_private_key_path = sysconf->value("pay_helper_set/alipay_privatekey").toString();
        QString store_id = sysconf->value("pay_helper_set/alipay_storeid").toString();

        alipay_help::alipay_public_request_argument arg0;
        alipay_help::alipay_trade_query_argument arg1;
        alipay_help::alipay_trade_query_return_argument arg2;

        arg0.app_id_not_null = app_id;
        arg0.charset_not_null="utf-8";
        arg0.sign_type=sysconf->value("alipay_help/" + app_id).toString(); //alipay_help::sign_type_SHA1;
        arg0.timestamp_not_null = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        arg0.version_not_null="1.0";
        arg0.rsa_private_key_data_not_null = alipay_help::alipay_private_key_add_start_line(rsa_private_key_path, 0);


        arg1.out_trade_no = out_trade_no_a;
        if(alipay_help::alipay_trade_query(arg0, arg1,arg2)) {
            if(!arg2.trade_no_not_null.isEmpty()
                    &&arg2.trade_status_not_null.trimmed() == "TRADE_SUCCESS") {
                QApplication::clipboard()->setText(out_trade_no_a);
                QMessageBox::information(this, this->windowTitle(), QObject::tr("支付成功") + "\n" + QObject::tr("单号已复制到剪切板"));
                this->accept();
                return;
            }
        }
    }
    if(payment == "ma") {
        QString sub_mch_id = sysconf->value("pay_helper_set/mobilepay_wechat_mch_id").toString();
        QString appid = sysconf->value("pay_helper_set/mobilepay_alipay_appid").toString();
        QByteArray mobilepay_alipay_public_key = sysconf->value("pay_helper_set/mobilepay_alipay_public_key").toByteArray();
        QString total_amount_x100 = QString::number(qRound(fee.toDouble()* 100));
        QString mobilepay_sign_type = sysconf->value("pay_helper_set/mobilepay_sign_type", "RSA2").toString();
        QString body = this->windowTitle();

        if(mobile_pay_help::zfbOrderQuery(errstring,
                                          mobilepay_sign_type,
                                          appid,
                                          out_trade_no_ma,
                                          mobilepay_alipay_public_key)) {
            QApplication::clipboard()->setText(out_trade_no_ma);
            QMessageBox::information(this, this->windowTitle(), QObject::tr("支付成功") + "\n" + QObject::tr("单号已复制到剪切板"));
            this->accept();
        }
    }
    if(payment == "mw") {
        QString sub_mch_id = sysconf->value("pay_helper_set/mobilepay_wechat_mch_id").toString();
        QString appid = sysconf->value("pay_helper_set/mobilepay_alipay_appid").toString();
        QByteArray mobilepay_alipay_public_key = sysconf->value("pay_helper_set/mobilepay_alipay_public_key").toByteArray();
        QString total_amount_x100 = QString::number(qRound(fee.toDouble()* 100));
        QString mobilepay_sign_type = sysconf->value("pay_helper_set/mobilepay_sign_type", "RSA2").toString();
        QString body = this->windowTitle();

        if(mobile_pay_help::wxOrderQuery(errstring,
                                         sub_mch_id,
                                         out_trade_no_mw
                                         )) {
            QApplication::clipboard()->setText(out_trade_no_mw);
            QMessageBox::information(this, this->windowTitle(), QObject::tr("支付成功") + "\n" + QObject::tr("单号已复制到剪切板"));
            this->accept();
        }
    }

    QTimer::singleShot(1000, this, SLOT(tocheckpay()));
}

void pay_helper_pay::tocodepay()
{
    pay_helper_loading loading(this, "LOADING");
    loading.showdelay();

    QString auth_code = ui->lineEdit->text().replace(" ", "").trimmed();
    QString errstring;

    if(payment == "a") {
        QString app_id = sysconf->value("pay_helper_set/alipay_appid").toString();
        QString rsa_private_key_path = sysconf->value("pay_helper_set/alipay_privatekey").toString();
        QString store_id = sysconf->value("pay_helper_set/alipay_storeid").toString();

        alipay_help::alipay_public_request_argument arg0;
        alipay_help::alipay_trade_pay_argument arg1;
        alipay_help::alipay_trade_pay_return_argument arg2;

        arg0.app_id_not_null = app_id;
        arg0.charset_not_null="utf-8";
        arg0.sign_type=sysconf->value("alipay_help/" + app_id).toString(); //alipay_help::sign_type_SHA1;
        arg0.timestamp_not_null = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        arg0.version_not_null="1.0";
        arg0.rsa_private_key_data_not_null = alipay_help::alipay_private_key_add_start_line(rsa_private_key_path, 0);

        arg1.auth_code_not_null = auth_code;
        arg1.out_trade_no_not_null = out_trade_no_b_a;
        arg1.subject_not_null = this->windowTitle();
        arg1.total_amount_not_null = fee;
        arg1.store_id = store_id;

        //本质上， 条码支付和二维码支付是独立的，这个同用一个arg0
        if(alipay_help::alipay_trade_pay(arg0, arg1, arg2)) {
            QApplication::clipboard()->setText(out_trade_no_b_a);
            QMessageBox::information(this, this->windowTitle(), QObject::tr("支付成功") + "\n" + QObject::tr("单号已复制到剪切板"));
            this->accept();
            return;
        }
        QMessageBox::warning(this, this->windowTitle(), alipay_help::errstring);
    }
    if(payment == "ma") {
        QString sub_mch_id = sysconf->value("pay_helper_set/mobilepay_wechat_mch_id").toString();
        QString appid = sysconf->value("pay_helper_set/mobilepay_alipay_appid").toString();
        QByteArray mobilepay_alipay_public_key = sysconf->value("pay_helper_set/mobilepay_alipay_public_key").toByteArray();
        QString total_amount_x100 = QString::number(qRound(fee.toDouble()* 100));
        QString mobilepay_sign_type = sysconf->value("pay_helper_set/mobilepay_sign_type", "RSA2").toString();
        QString body = this->windowTitle();
        QString kbstore_id = sysconf->value("pay_helper_set/mobilepay_alipay_kbstore_id", "").toString();

        mobile_pay_help::payType pay_type;
        if(mobile_pay_help::codepay(
                    pay_type,
                    errstring,

                    mobilepay_sign_type,
                    appid,
                    "", //sub_mch_id,
                    auth_code,
                    total_amount_x100,

                    out_trade_no_b_ma,
                    body,
                    kbstore_id,
                    mobilepay_alipay_public_key
                    )) {
            QApplication::clipboard()->setText(out_trade_no_b_ma);
            QMessageBox::information(this, this->windowTitle(), QObject::tr("支付成功") + "\n" + QObject::tr("单号已复制到剪切板"));
            this->accept();
            return;
        }
        QMessageBox::warning(this, this->windowTitle(), errstring);
    }

    if(payment == "mw") {
        QString sub_mch_id = sysconf->value("pay_helper_set/mobilepay_wechat_mch_id").toString();
        QString appid = sysconf->value("pay_helper_set/mobilepay_alipay_appid").toString();
        QByteArray mobilepay_alipay_public_key = sysconf->value("pay_helper_set/mobilepay_alipay_public_key").toByteArray();
        QString total_amount_x100 = QString::number(qRound(fee.toDouble()* 100));
        QString mobilepay_sign_type = sysconf->value("pay_helper_set/mobilepay_sign_type", "RSA2").toString();
        QString body = this->windowTitle();
        QString kbstore_id = sysconf->value("pay_helper_set/mobilepay_alipay_kbstore_id", "").toString();

        mobile_pay_help::payType pay_type;
        if(mobile_pay_help::codepay(
                    pay_type,
                    errstring,

                    mobilepay_sign_type,
                    "",//appid,
                    sub_mch_id,
                    auth_code,
                    total_amount_x100,

                    out_trade_no_b_mw,
                    body,
                    kbstore_id,
                    mobilepay_alipay_public_key
                    )) {
            QApplication::clipboard()->setText(out_trade_no_b_mw);
            QMessageBox::information(this, this->windowTitle(), QObject::tr("支付成功") + "\n" + QObject::tr("单号已复制到剪切板"));
            this->accept();
            return;
        }
        QMessageBox::warning(this, this->windowTitle(), errstring);
    }
}

void pay_helper_pay::tocancel()
{
    this->reject();
}

void pay_helper_pay::tomakeUrl()
{
    pay_helper_loading loading(this, "LOADING");
    loading.showdelay();

    if(payment == "a") {
        this->setWindowTitle(QObject::tr("支付宝") + ":" + fee);
        ui->label_title->setText(this->windowTitle());

        QString app_id = sysconf->value("pay_helper_set/alipay_appid").toString();
        QString rsa_private_key_path = sysconf->value("pay_helper_set/alipay_privatekey").toString();
        QString store_id = sysconf->value("pay_helper_set/alipay_storeid").toString();

        alipay_help::alipay_public_request_argument arg0;
        alipay_help::alipay_trade_precreate_argument arg1;
        alipay_help::alipay_trade_precreate_return_argument arg2;

        arg0.app_id_not_null = app_id;
        arg0.charset_not_null="utf-8";
        arg0.sign_type=sysconf->value("alipay_help/" + app_id).toString(); //alipay_help::sign_type_SHA1;
        arg0.timestamp_not_null = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        arg0.version_not_null="1.0";
        arg0.rsa_private_key_data_not_null = alipay_help::alipay_private_key_add_start_line(rsa_private_key_path, 0);

        arg1.out_trade_no_not_null = out_trade_no_a;
        arg1.subject_not_null = this->windowTitle();
        arg1.total_amount_not_null = fee;
        arg1.store_id = store_id;

        if(alipay_help::alipay_trade_precreate(arg0, arg1, arg2)) {
            ui->label_url->setPixmap(testwx_help::get_rbcode(arg2.qr_code_not_null.toLocal8Bit()));
        } else {
            QMessageBox::warning(this, this->windowTitle(),  QObject::tr("操作失败") + alipay_help::errstring);
        }
    }
    if(payment == "ma") {
        this->setWindowTitle(QObject::tr("摇钱树支付") + "/" + QObject::tr("支付宝") + ":" + fee);
        ui->label_title->setText(this->windowTitle());

        QString sub_mch_id = sysconf->value("pay_helper_set/mobilepay_wechat_mch_id").toString();
        QString appid = sysconf->value("pay_helper_set/mobilepay_alipay_appid").toString();
        QByteArray mobilepay_alipay_public_key = sysconf->value("pay_helper_set/mobilepay_alipay_public_key").toByteArray();
        QString total_amount_x100 = QString::number(qRound(fee.toDouble()* 100));
        QString mobilepay_sign_type = sysconf->value("pay_helper_set/mobilepay_sign_type", "RSA2").toString();
        QString body = this->windowTitle();
        QString kbstore_id = sysconf->value("pay_helper_set/mobilepay_alipay_kbstore_id", "").toString();
        QByteArray url;
        QString errstring;
        //
        if(false == mobile_pay_help::getzfbpayurl(url,
                                                  errstring,
                                                  mobilepay_sign_type,
                                                  appid,
                                                  total_amount_x100,
                                                  out_trade_no_ma,
                                                  body,
                                                  kbstore_id,
                                                  mobilepay_alipay_public_key)) {
            QMessageBox::warning(this, this->windowTitle(), errstring);
        }
        ui->label_url->setPixmap(testwx_help::get_rbcode(url));

    }
    if(payment == "mw") {
        this->setWindowTitle(QObject::tr("摇钱树支付") + "/" + QObject::tr("微信") + ":" + fee);
        ui->label_title->setText(this->windowTitle());

        QString sub_mch_id = sysconf->value("pay_helper_set/mobilepay_wechat_mch_id").toString();
        QString appid = sysconf->value("pay_helper_set/mobilepay_alipay_appid").toString();
        QByteArray mobilepay_alipay_public_key = sysconf->value("pay_helper_set/mobilepay_alipay_public_key").toByteArray();
        QString total_amount_x100 = QString::number(qRound(fee.toDouble()* 100));
        QString mobilepay_sign_type = sysconf->value("pay_helper_set/mobilepay_sign_type", "RSA2").toString();
        QString body = this->windowTitle();
        QByteArray url;
        QString errstring;
        //
        if(false == mobile_pay_help::getwxpayurl(url,
                                                 errstring,
                                                 sub_mch_id,
                                                 total_amount_x100,
                                                 out_trade_no_mw,
                                                 body)) {
            QMessageBox::warning(this, this->windowTitle(), errstring);
        }
        ui->label_url->setPixmap(testwx_help::get_rbcode(url));
    }

    QTimer::singleShot(1000, this, SLOT(tocheckpay()));
}
