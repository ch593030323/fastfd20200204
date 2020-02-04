#include "saas_guide_pay_rbcode.h"
#include "ui_saas_guide_pay_rbcode.h"
#include "alipay_help.h"
#include "testwx_help.h"
#include "lds.h"
#include <QDateTime>
#include "lds_messagebox.h"

saas_guide_pay_rbcode::saas_guide_pay_rbcode(double fee, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::saas_guide_pay_rbcode)
{
    ui->setupUi(this);
    this->fee = fee;
#ifdef QT_DEBUG
    this->fee = 0.01;
#endif
    timer =  new QTimer(this);
    timer->setInterval(1000);

    QDateTime sqldt = QDateTime::currentDateTime();
    out_trade_no = lds::LOCAL_TERMINAL_INDEX_FF + sqldt.toString("yyyyMMddhhmmsszzz");

    ///
    QString timestamp_not_null= sqldt.toString("yyyy-MM-dd hh:mm:ss");
    arg0.app_id_not_null = "2015121000954514";
    arg0.charset_not_null="utf-8";
    arg0.sign_type=alipay_help::sign_type_SHA1;//固定
    arg0.timestamp_not_null= timestamp_not_null;
    arg0.version_not_null="1.0";
    arg0.rsa_private_key_data_not_null = alipay_help::alipay_private_key_add_start_line(":/image/2015121000954514.pem", 0);



    generRbCode();
    connect(ui->pushButton_exit, SIGNAL(clicked()), this,SLOT(reject()));
    connect(timer, SIGNAL(timeout()), this, SLOT(check_pay()));
    setWindowTitle(QObject::tr("支付") + ":" + QString::number(fee, 'f', 2));

    timer->start();
}

saas_guide_pay_rbcode::~saas_guide_pay_rbcode()
{
    delete ui;
}

void saas_guide_pay_rbcode::generRbCode()
{
    ///
    alipay_help::alipay_trade_precreate_argument arg1;
    alipay_help::alipay_trade_precreate_return_argument arg2;
    arg1.out_trade_no_not_null = out_trade_no;
    arg1.subject_not_null = "saas Ali pay";
    arg1.total_amount_not_null=QString::number(fee, 'f',  2);
    if(alipay_help::alipay_trade_precreate(arg0,arg1,arg2)) {
        ui->label_alipay->setPixmap(
                    testwx_help::get_rbcode(arg2.qr_code_not_null.toLocal8Bit(), ui->label_alipay->width()));
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QObject::tr("支付宝") + QObject::tr("操作失败") + "\n" + alipay_help::errstring);
    }

    ///
    QByteArray qrcodecontent;
    if(testwx_help::AmountQRcodeRequest(
                "wxd9ccc742b1c39110",
                "SaaS WeChat Pay",
                int(fee*100),
                "wwwscposcomhdd802600000000000000",
                "10036785",
                out_trade_no,
                qrcodecontent
                )) {
        ui->label_wechat->setPixmap(
                    testwx_help::get_rbcode(qrcodecontent.data(), ui->label_wechat->width()));
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QObject::tr("微信") + QObject::tr("操作失败"));
    }
}

void saas_guide_pay_rbcode::check_pay()
{
    timer->stop();
    ///
    alipay_help::alipay_trade_query_argument arg1;
    alipay_help::alipay_trade_query_return_argument arg2;
    arg1.out_trade_no = out_trade_no;
    if(alipay_help::alipay_trade_query(arg0, arg1, arg2)) {
        if(!arg2.trade_no_not_null.isEmpty()
                &&arg2.trade_status_not_null.trimmed() == "TRADE_SUCCESS") {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("支付宝") + "," + QObject::tr("支付成功"));
            this->accept();
            return;
        }
    }
    ///
    testwx_help::retDataStruct wechatd = testwx_help::getPayBillInfo(out_trade_no,
                                                                     "wxd9ccc742b1c39110",
                                                                     "wwwscposcomhdd802600000000000000",
                                                                     "10036785");
    if(!wechatd.transaction_id.trimmed().isEmpty()
            &&wechatd.trade_state.trimmed() == "SUCCESS") {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("微信") + "," + QObject::tr("支付成功"));
        this->accept();
        return;
    }

    timer->start();
}
