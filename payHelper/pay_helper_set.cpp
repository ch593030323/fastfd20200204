#include "pay_helper_set.h"
#include "ui_pay_helper_set.h"
#include "alipay_help.h"
#include "mobile_pay_help.h"
#include <QMessageBox>
#include <QDateTime>
#include <QTimer>
#include <QApplication>
#include <QFileDialog>
#include <QClipboard>
#include "pay_helper_loading.h"

pay_helper_set::pay_helper_set(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::pay_helper_set)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("设置"));
    sysconf = new QSettings(qApp->applicationDirPath() + "/conf.ini", QSettings::IniFormat, this);

    ui->comboBox->addItems(mobile_pay_help::mobile_pay_alipay_keys());
    ui->comboBox->setCurrentIndex(ui->comboBox->findText(sysconf->value("pay_helper_set/mobilepay_sign_type", "RSA2").toString()));
    ui->lineEdit_alipay_appid->setText(sysconf->value("pay_helper_set/alipay_appid").toString());
    ui->lineEdit_alipay_privatekey->setText(sysconf->value("pay_helper_set/alipay_privatekey").toString());
    ui->lineEdit_alipay_storeid->setText(sysconf->value("pay_helper_set/alipay_storeid").toString());
    ui->lineEdit_mobilepay_alipay_appid->setText(sysconf->value("pay_helper_set/mobilepay_alipay_appid").toString());
    ui->lineEdit_mobilepay_alipay_public_key->setText(sysconf->value("pay_helper_set/mobilepay_alipay_public_key").toString());
    ui->lineEdit_mobilepay_alipay_kbstore_id->setText(sysconf->value("pay_helper_set/mobilepay_alipay_kbstore_id").toString());
    ui->lineEdit_mobilepay_wechat_mch_id->setText(sysconf->value("pay_helper_set/mobilepay_wechat_mch_id").toString());

    ui->toolBox->setCurrentIndex(sysconf->value("pay_helper_set/pay_helper_set_index", 0).toInt());

    ui->comboBox_2->addItem(QObject::tr("所有"), "");
    ui->comboBox_2->addItem(QObject::tr("支付宝"), "a");
    ui->comboBox_2->addItem(QObject::tr("摇钱树支付"), "m");
    ui->comboBox_2->addItem(QObject::tr("摇钱树支付") + "/" + QObject::tr("支付宝"), "ma");
    ui->comboBox_2->addItem(QObject::tr("摇钱树支付") + "/" + QObject::tr("微信"), "mw");
    ui->comboBox_2->setCurrentIndex(ui->comboBox_2->findData(sysconf->value("pay_helper_set/quick_pay_type", "")));
    //
    connect(ui->pushButton_alipay_demo,SIGNAL(clicked()), this, SLOT(toalipay_demo()));
    connect(ui->pushButton_alipay_test,SIGNAL(clicked()), this, SLOT(toalipay_test()));
    connect(ui->pushButton_mobilepay_alipay_get_appkey,SIGNAL(clicked()), this, SLOT(tomobilepay_alipay_get_appkey()));
    connect(ui->pushButton_mobilepay_demo,SIGNAL(clicked()), this, SLOT(tomobilepay_demo()));
    connect(ui->pushButton_mobilepay_alipay_get_publickey,SIGNAL(clicked()), this, SLOT(tomobilepay_alipay_get_publickey()));
    connect(ui->pushButton_mobilepay_test,SIGNAL(clicked()), this, SLOT(tomobilepay_test()));
    connect(ui->pushButton_ok,SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_cancel,SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_alipay_get_privatepath,SIGNAL(clicked()), this, SLOT(toalipay_get_privatepath()));
}

pay_helper_set::~pay_helper_set()
{
    delete ui;
}

void pay_helper_set::toalipay_demo()
{
    ui->lineEdit_alipay_appid->setText("2015092200312405");
    ui->lineEdit_alipay_privatekey->setText("://rec/2015092200312405(RSA).txt");
}

void pay_helper_set::toalipay_test()
{
    pay_helper_loading loading(this, "LOADING");
    loading.showdelay();

    alipay_help::alipay_public_request_argument arg0;
    alipay_help::alipay_trade_precreate_argument arg1;
    alipay_help::alipay_trade_precreate_return_argument arg2;
    QString rsa_private_key_path = ui->lineEdit_alipay_privatekey->text();

    arg0.app_id_not_null = ui->lineEdit_alipay_appid->text();
    arg0.charset_not_null="utf-8";
    arg0.sign_type= sysconf->value("pay_helper_set/" + arg0.app_id_not_null).toString();//alipay_help::sign_type_SHA256;
    arg0.timestamp_not_null="2015-12-17 03:07:50";
    arg0.version_not_null="1.0";
    arg0.rsa_private_key_data_not_null = alipay_help::alipay_private_key_add_start_line(rsa_private_key_path, 0);
    arg1.out_trade_no_not_null = "201503022001";
    arg1.subject_not_null = "apple";
    arg1.total_amount_not_null="21.3";

    if(alipay_help::alipay_trade_precreate(arg0, arg1, arg2)) {
        sysconf->setValue("pay_helper_set/" + arg0.app_id_not_null, arg2.sign_type_not_null);
        QMessageBox::information(this, this->windowTitle(),  arg2.sign_type_not_null + QObject::tr("测试成功") + ":"
                                    +"\nout_trade_no"+(arg2.out_trade_no_not_null.isEmpty()?QObject::tr("不正常"):QObject::tr("正常"))
                                    +"\nqr_code"+(arg2.qr_code_not_null.isEmpty()?QObject::tr("不正常"):QObject::tr("正常")));
    } else {
        QMessageBox::warning(this, this->windowTitle(),  QObject::tr("操作失败") + alipay_help::errstring);
    }

}

void pay_helper_set::tomobilepay_alipay_get_appkey()
{
    QString filepath = QFileDialog::getOpenFileName(this, QObject::tr("文件内容需要去头去尾、去空格去回车、才可以正常使用"), qApp->applicationDirPath());
    if(filepath.length() > 0) {
        QFile file(filepath);
        file.open(QFile::ReadOnly);
        ui->lineEdit_mobilepay_alipay_public_key->setText(file.readAll());
    }
}

void pay_helper_set::tomobilepay_demo()
{
    ui->lineEdit_mobilepay_wechat_mch_id->setText("1431014702");
    ui->lineEdit_mobilepay_alipay_appid->setText("2015121000954514");
    ui->lineEdit_mobilepay_alipay_public_key->setText("MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAvprAybgqWuLiL1IYG/nT13oRtR782HXzapBfORVNEo+iGVBLpvH9XSxI1JzQ3/TMjiN4HNB5DNLoepssEO3WAlD2MUa9KWbhWpfRR/ps3aEPPIvSfENXQeJF2re/CbX4jNvsFStAgEgd23/ch6bkH/yilKdMTr8UFtlL4RQ/ECO8gmpo7j+LDwsP83qdOm/NHcdQNOmqPom/ByGVWl9C4ZDEfkjbMpM9MZuMaB6zjQ18KbLrrN4+9DxsA7/HCq33s7BP6gptfMfhq3UoM67GDUAgX4D9PsjMx4cWAu6t9lF64ZeWj51/8YKd2j8+OVSx+W1JMnye3rJnjpBt5PEBRwIDAQAB");
    ui->lineEdit_mobilepay_alipay_kbstore_id->setText("");
    ui->comboBox->setCurrentIndex(ui->comboBox->findText(mobile_pay_help_sign_type::RSA));
}

void pay_helper_set::tomobilepay_alipay_get_publickey()
{
    pay_helper_loading loading(this, "LOADING");
    loading.showdelay();

    QByteArray ret_rsa_public_key;
    QString errstring;
    bool f = false;
    {
        f = mobile_pay_help::getzfbpublic_key(ret_rsa_public_key, errstring, ui->comboBox->currentText());
    }
    if(f) {
        QApplication::clipboard()->setText(ret_rsa_public_key);
        QMessageBox::information(this, this->windowTitle(), QObject::tr("已复制到剪切板") +"\n" + QObject::tr("请打开记事本，新建，粘贴"));
        return;
    }
    QMessageBox::warning(this, this->windowTitle(), QObject::tr("操作失败") + "\n" + errstring);

}

void pay_helper_set::tomobilepay_test()
{
    int type = -1;
    switch(QMessageBox::question(this, this->windowTitle(), QObject::tr("测试"), QObject::tr("支付宝测试"), QObject::tr("微信测试"), QObject::tr("取消"))) {
    case 0:
        type = 0;
        break;
    case 1:
        type = 1;
        break;
    }

    pay_helper_loading loading(this, "LOADING");
    loading.showdelay();

    //金额为整数，单位分，如12.34元，实际传入1234
    //auth_code 自动替换清除空格
    QString appid = ui->lineEdit_mobilepay_alipay_appid->text();//arg2
    QString sub_mch_id = ui->lineEdit_mobilepay_wechat_mch_id->text();//arg1
    QString alipay_public_key = ui->lineEdit_mobilepay_alipay_public_key->text();
    QString auth_code;
    QString total_amount = "1";

    QString out_trade_no;
    QString body = QObject::tr("摇钱树支付");
    QString kbstore_id = ui->lineEdit_mobilepay_alipay_kbstore_id->text();
    QString sub_appid = "";
    QString device_info = "";
    out_trade_no = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
    auth_code = "1300 6767 7289 989988";

    QByteArray code_url;
    QString errstring;
    bool flag = false; {
        if(type == 1) {
            flag = mobile_pay_help::getwxpayurl(code_url,
                                                errstring,
                                                sub_mch_id,
                                                total_amount,
                                                out_trade_no,
                                                body);
        }
        if(type == 0) {
            flag = mobile_pay_help::getzfbpayurl(code_url,
                                                 errstring,
                                                 ui->comboBox->currentText(),
                                                 appid,
                                                 total_amount,
                                                 out_trade_no,
                                                 body,
                                                 kbstore_id,
                                                 alipay_public_key.toLocal8Bit());
        }
    }
    if(flag) {
        QMessageBox::information(this, this->windowTitle(),  QObject::tr("测试成功"));
        return;
    }
    QMessageBox::warning(this, "", QObject::tr("测试失败")+"\n"+errstring);
}

void pay_helper_set::took()
{
    qDebug() << __LINE__;
    sysconf->setValue("pay_helper_set/mobilepay_sign_type", ui->comboBox->currentText());

    sysconf->setValue("pay_helper_set/alipay_appid", ui->lineEdit_alipay_appid->text());
    sysconf->setValue("pay_helper_set/alipay_privatekey", ui->lineEdit_alipay_privatekey->text());
    sysconf->setValue("pay_helper_set/alipay_storeid", ui->lineEdit_alipay_storeid->text());

    sysconf->setValue("pay_helper_set/mobilepay_alipay_appid", ui->lineEdit_mobilepay_alipay_appid->text());
    sysconf->setValue("pay_helper_set/mobilepay_alipay_public_key", ui->lineEdit_mobilepay_alipay_public_key->text());
    sysconf->setValue("pay_helper_set/mobilepay_alipay_kbstore_id", ui->lineEdit_mobilepay_alipay_kbstore_id->text());
    sysconf->setValue("pay_helper_set/mobilepay_wechat_mch_id", ui->lineEdit_mobilepay_wechat_mch_id->text());

    sysconf->setValue("pay_helper_set/pay_helper_set_index", ui->toolBox->currentIndex());

    sysconf->setValue("pay_helper_set/quick_pay_type", ui->comboBox_2->itemData(ui->comboBox_2->currentIndex()));
    this->accept();
    qDebug() << __LINE__;
}

void pay_helper_set::tocancel()
{
    this->reject();
}

void pay_helper_set::toalipay_get_privatepath()
{
    QString filepath = QFileDialog::getOpenFileName(this, "", qApp->applicationDirPath());
    if(filepath.length() > 0) {
        ui->lineEdit_alipay_privatekey->setText(filepath);
    }
}
