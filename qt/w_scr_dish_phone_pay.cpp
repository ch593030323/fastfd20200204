#include "w_scr_dish_phone_pay.h"
#include "ui_w_scr_dish_phone_pay.h"
#include "frontviewdialog_virtual.h"
#include "lds_dialog_input_double.h"
#include "ui_lds_dialog_input_dialog_double.h"
#include "lds_roundeddialog_rect_align.h"
#include "lds_messagebox.h"
#include "mobile_pay_help.h"
#include "lds_query_hddpos.h"
#include "public_sql.h"
#include "n_func.h"
#include "lds.h"
#include <QMovie>
#include "backheader.h"
#include "lds_ogg_play.h"
#include "public_logindialog.h"
#include "ccb_pay_help.h"
#include "fupay_help.h"
#include "qmenu.h"
#include "liantuofu_pay.h"


const QString w_scr_dish_phone_pay::tradeQRCode = "tradeQRCode";
const QString w_scr_dish_phone_pay::tradeBarCode = "tradeBarCode";

w_scr_dish_phone_pay::w_scr_dish_phone_pay(const QString &ch_paymodeno, double fee, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_phone_pay),
    ch_paymodeno(ch_paymodeno),
    fee(fee)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(1000);
    queryCount = 0;

#ifdef QT_PHONON_SUPPORT
    lds::showWidget(ui->checkBox);
#else
    lds::hideWidget(ui->checkBox);
#endif

    ui->pushButton_err->setMenu(new QMenu(this));
    ui->pushButton_err->menu()->addAction("");
    lds::hideWidget(ui->pushButton_err);
    lds::hideWidget(ui->pushButton_ok);
    ui->label_alipay->setMovie(new QMovie(":/image/loading.gif", "", this));
    ui->label_wechat->setMovie(new QMovie(":/image/loading.gif", "", this));
    if("05" == ch_paymodeno//微信
            || "CC" == ch_paymodeno//龙支付
            ) {
        ui->label_alipay->hide();
        ui->label_wechat_desc->hide();
        ui->label_alipay_desc->hide();
        ui->lineEdit_payno->hide();
        ui->label_payno->hide();
    }
    if("CC" == ch_paymodeno) {//龙支付
        lds::showWidget(ui->pushButton_ok);
        lds::showWidget(ui->pushButton_err);
    }
    if("06" == ch_paymodeno//支付宝
            || "EE" == ch_paymodeno//联拓富
            ) {
        ui->label_wechat->hide();
        ui->label_alipay_desc->hide();
        ui->label_wechat_desc->hide();
    }
    if("DD" == ch_paymodeno) {//微支付只有条码支付
        lds::hideWidget(ui->label_alipay_desc);
        lds::hideWidget(ui->label_alipay);
        lds::hideWidget(ui->label_wechat_desc);
        lds::hideWidget(ui->label_wechat);
        lds::hideWidget(ui->pushButton_print);
        this->setFixedHeight(100);
    }
    //
    lds_query_hddpos  query;
    query.execSelect(QString("select  *  from cey_bt_paymode where ch_paymodeno = '%1' ")
                     .arg(ch_paymodeno));
    query.next();
    vch_arg1 = query.recordValue("vch_arg1");
    vch_arg2 = query.recordValue("vch_arg2");
    vch_arg3 = query.recordValue("vch_arg3");
    vch_arg5 = query.recordValue("vch_arg5");
    vch_arg6 = query.recordValue("vch_arg6");
    vch_arg7 = query.recordValue("vch_arg7");

    //connect
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toDDexit()));
    connect(ui->lineEdit_payno, SIGNAL(returnPressed()), this, SLOT(tocodePay()));
    connect(ui->pushButton_print, SIGNAL(clicked()), this, SLOT(toprint()));
    connect(ui->checkBox, SIGNAL(clicked(bool)), this, SLOT(to_save_audio(bool)));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(tocheckOk()));

    ui->checkBox->setChecked(lds::sysconf->value("w_scr_dish_phone_pay_" + ch_paymodeno, false).toBool());

    setWindowTitle(title(ch_paymodeno) + ":" + QString::number(fee, 'f', 2));
    if("DD" == ch_paymodeno) {//微支付
    } else {
        connect(timer, SIGNAL(timeout()), this, SLOT(topolling()));
        connect(ui->lineEdit_payno, SIGNAL(textChanged(QString)), timer, SLOT(start()));//时间器会立刻stop and start
        QTimer::singleShot(10, this, SLOT(makeUrl()));
    }
}

w_scr_dish_phone_pay::~w_scr_dish_phone_pay()
{
    delete ui;
}

bool w_scr_dish_phone_pay::getInputPayFee(double &ret_fee, const QString &ch_paymodeno, double num_pay, QWidget *parent)
{
    lds_dialog_input_double inputdialog(parent);
    inputdialog.ui->label->setText(QObject::tr("支付金额"));
    inputdialog.setWindowTitle(title(ch_paymodeno));
    inputdialog.ui->lineEdit->setValue(num_pay);
    if(QDialog::Accepted == transparentCenterDialog(&inputdialog).exec()) {
        if(inputdialog.ui->lineEdit->value() > num_pay) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("输入金额不能大于应收金额"));
            return false;
        }
        ret_fee = inputdialog.ui->lineEdit->value();
        return true;
    }
    return false;
}

QString w_scr_dish_phone_pay::title(const QString &ch_paymodeno)
{
    if(ch_paymodeno == "AA") {//摇钱树支付
        return QObject::tr("摇钱树支付");
    }
    if(ch_paymodeno == "05") {//微信支付
        return QObject::tr("微信支付");
    }
    if(ch_paymodeno == "06") {//支付宝支付
        return QObject::tr("支付宝支付");
    }
    if(ch_paymodeno == "CC") {//龙支付
        return QObject::tr("龙支付");
    }
    if(ch_paymodeno == "DD") {//微支付
        return QObject::tr("微支付");
    }
    if(ch_paymodeno == "EE") {//微支付
        return QObject::tr("联拓富");
    }
    return"";
}

bool w_scr_dish_phone_pay::isPayRecord(const QString &ch_paymodeno)
{
    lds_query_hddpos  query;
    query.execSelect(QString("select  vch_arg6  from cey_bt_paymode where ch_paymodeno = '%1'  ")
                     .arg(ch_paymodeno));
    query.next();
    return "Y" == query.recordValue("vch_arg6").toString();
}

void w_scr_dish_phone_pay::makeUrl()
{
    ui->label_alipay->movie()->start();
    ui->label_wechat->movie()->start();

    QString errstring;
    QByteArray url_wechat;
    QByteArray url_alipay;

    if(ch_paymodeno == "AA") {//摇钱树支付
        QString sub_mch_id = vch_arg1.toString().trimmed();//arg1
        QString appid = vch_arg2.toString().trimmed();//arg2
        QByteArray rsa_public_key_data = vch_arg3.toByteArray();//arg3
        QString total_amount_x100 = QString::number(qRound(fee* 100));
        QString body = QObject::tr("摇钱树支付") +"," + QObject::tr("机器号") + lds::terminalCode;
        QString kbstore_id = vch_arg5.toString();
        //
        if(appid.length() != 0
                && false == mobile_pay_help::getzfbpayurl(url_alipay,
                                                          errstring,
                                                          n_func::f_get_sysparm("mobile_alipay_sign_type", "RSA2"),
                                                          appid,
                                                          total_amount_x100,
                                                          get_out_trade_no(tradeQRCode),
                                                          body,
                                                          kbstore_id,
                                                          rsa_public_key_data)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        }
        ui->label_alipay->setPixmap(testwx_help::get_rbcode(url_alipay.data(), ui->label_alipay->width()));
        //
        if(sub_mch_id.length() != 0
                && false == mobile_pay_help::getwxpayurl(url_wechat,
                                                         errstring,
                                                         sub_mch_id,
                                                         total_amount_x100,
                                                         get_out_trade_no(tradeQRCode),
                                                         body)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        }
        ui->label_wechat->setPixmap(testwx_help::get_rbcode(url_wechat.data(), ui->label_wechat->width()));
    }
    if(ch_paymodeno == "05") {//微信
        QString appid = vch_arg1.toString();//微信APPID
        QString mch_id = vch_arg2.toString();//商户号
        QString paykey = vch_arg3.toString();//API密钥
        int pay_x100 = qRound(fee * 100);
        if(false == testwx_help::AmountQRcodeRequest(appid,
                                                     QObject::tr("微信支付"),
                                                     pay_x100,
                                                     paykey,
                                                     mch_id,
                                                     get_out_trade_no(tradeQRCode),
                                                     url_wechat)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QObject::tr("操作失败"));
        }
        ui->label_wechat->setPixmap(testwx_help::get_rbcode(url_wechat.data(), ui->label_wechat->width()));
    }
    if(ch_paymodeno == "CC") {//龙支付
        if(!ccb_pay_help::getPayUrl(vch_arg2.toString(), vch_arg3.toString(), vch_arg1.toString(), vch_arg5.toString(),
                                    QString().sprintf("%.02f", fee),
                                    get_out_trade_no(tradeQRCode),
                                    url_wechat,
                                    errstring)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        }
        ui->label_wechat->setPixmap(testwx_help::get_rbcode(url_wechat.data(), ui->label_wechat->width()));
    }
    if(ch_paymodeno == "06") {//支付宝
        QString app_id = vch_arg1.toString();
        QString store_id = vch_arg2.toString();
        QString rsa_private_key_path = vch_arg3.toString();

        alipay_help::alipay_trade_precreate_argument arg1;
        arg1.out_trade_no_not_null = get_out_trade_no(tradeQRCode);
        arg1.subject_not_null = QObject::tr("餐饮消费") + "," + QObject::tr("机器号") + lds::terminalCode;
        arg1.total_amount_not_null=QString::number(fee, 'f', 2);
        arg1.store_id = store_id;
        //
        alipay_help::alipay_trade_precreate_return_argument arg2;
        if(false == alipay_help::alipay_trade_precreate(get_alipay_arg0(app_id, rsa_private_key_path),arg1,arg2)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, alipay_help::errstring);
        } else {
            lds::sysconf->setValue("alipay_help/" + app_id, arg2.sign_type_not_null);
            url_alipay = arg2.qr_code_not_null.toLocal8Bit();
        }
        ui->label_alipay->setPixmap(testwx_help::get_rbcode(url_alipay.data(), ui->label_alipay->width()));
    }
    if(ch_paymodeno == "EE") {
        QString appid = vch_arg1.toString();
        QString key = vch_arg2.toString();
        QString merchantCode = vch_arg3.toString();
        liantuofu_pay payObj;

        if(false == payObj.jspay(get_out_trade_no(tradeQRCode),
                                 appid,
                                 merchantCode,
                                 key,
                                 QString().sprintf("%.02f", fee),
                                 url_wechat,
                                 errstring)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        }
        ui->label_alipay->setPixmap(testwx_help::get_rbcode(url_wechat.data(), ui->label_wechat->width()));
    }
    //副屏
    rb_ddata.udpKey = lds::udp_rb_code_request_begin;
    rb_ddata.rb_title = this->windowTitle();
    rb_ddata.rb_title_alipay = ui->label_alipay_desc->text();
    rb_ddata.rb_title_wechat = ui->label_wechat_desc->text();
    rb_ddata.rb_alipay = url_alipay;
    rb_ddata.rb_wechat = url_wechat;
    rb_ddata.rb_ch_paymodeno = ch_paymodeno;

    if("CC" == ch_paymodeno//龙支付、：和微信副屏模式一样
            ||  "EE" == ch_paymodeno//联拓富 ：和微信副屏模式一样
            ) {
        rb_ddata.rb_ch_paymodeno = "05";
    }

    public_sql::doublescreensenddataByconf(rb_ddata);

    //开始循环
    timer->start();
}

void w_scr_dish_phone_pay::topolling()
{
    queryCount ++;

    QString errstring;
    QWidget *w = qApp->activeWindow();
    if(this->parent() == w) {
    } else {
        goto end;
    }

    qDebug() << __FUNCTION__ << __LINE__;
    if("AA" == ch_paymodeno) {//移动
        QString sub_mch_id = vch_arg1.toString().trimmed();//arg1
        QString appid = vch_arg2.toString().trimmed();//arg2
        QByteArray rsa_public_key_data = vch_arg3.toByteArray();//arg3

        if(mobile_pay_help::wxOrderQuery(errstring,
                                         sub_mch_id,
                                         get_out_trade_no(tradeQRCode)
                                         )) {
            ret_out_trade_no = get_out_trade_no(tradeQRCode);
            ret_pay_name = QObject::tr("微信支付");
            toDDaccept();
            goto end;
        }
        if(mobile_pay_help::zfbOrderQuery(errstring,
                                          n_func::f_get_sysparm("mobile_alipay_sign_type", "RSA2"),
                                          appid,
                                          get_out_trade_no(tradeQRCode),
                                          rsa_public_key_data)) {
            ret_out_trade_no = get_out_trade_no(tradeQRCode);
            ret_pay_name = QObject::tr("支付宝支付");
            toDDaccept();
            goto end;
        }
    }

    if("05" == ch_paymodeno) {//微信
        QString appid = vch_arg1.toString();//微信APPID
        QString mch_id = vch_arg2.toString();//商户号
        QString paykey = vch_arg3.toString();//API密钥

        testwx_help::retDataStruct ret = testwx_help::getPayBillInfo(get_out_trade_no(tradeQRCode),
                                                                     appid,
                                                                     paykey,
                                                                     mch_id);
        if(!ret.transaction_id.trimmed().isEmpty()
                &&ret.trade_state.trimmed() == "SUCCESS") {
            ret_out_trade_no = get_out_trade_no(tradeQRCode);
            ret_pay_name = QObject::tr("微信支付");

            this->toDDaccept();
            goto end;
        }
    }

    if("06" == ch_paymodeno) {//支付宝
        QString app_id = vch_arg1.toString();
        QString rsa_private_key_path = vch_arg3.toString();

        alipay_help::alipay_public_request_argument arg0 = get_alipay_arg0(app_id, rsa_private_key_path);
        alipay_help::alipay_trade_query_argument arg1;
        alipay_help::alipay_trade_query_return_argument arg2;
        arg1.out_trade_no = get_out_trade_no(tradeQRCode);
        if(alipay_help::alipay_trade_query(arg0, arg1,arg2)) {
            if(!arg2.trade_no_not_null.isEmpty()
                    &&arg2.trade_status_not_null.trimmed() == "TRADE_SUCCESS") {
                ret_out_trade_no = get_out_trade_no(tradeQRCode);
                ret_pay_name = QObject::tr("支付宝支付");
                this->toDDaccept();
                goto end;
            }
        }
    }

    if("CC" == ch_paymodeno) {//龙支付
        if(ccb_pay_help::orderQuery(vch_arg2.toString(), vch_arg3.toString(), vch_arg1.toString(), vch_arg5.toString(), get_out_trade_no(tradeQRCode), errstring)) {
            ret_out_trade_no = get_out_trade_no(tradeQRCode);
            ret_pay_name = QObject::tr("龙支付");

            this->toDDaccept();
            goto end;
        }
        ui->pushButton_err->menu()->actions().at(0)->setText(QString::number(queryCount) + ":" + errstring);
    }

    if("EE" == ch_paymodeno) {
        QString appid = vch_arg1.toString();
        QString key = vch_arg2.toString();
        QString merchantCode = vch_arg3.toString();
        liantuofu_pay payObj;

        if(payObj.query(get_out_trade_no(tradeQRCode),
                                appid,
                                merchantCode,
                                key,
                                errstring)) {
            ret_out_trade_no = get_out_trade_no(tradeQRCode);
            ret_pay_name = QObject::tr("联拓富");

            this->toDDaccept();
            goto end;
        }
    }
    //
end:
    timer->start();
}

void w_scr_dish_phone_pay::tocodePay()
{
    lds_messagebox_loading_show loading(this, "", "LOADING");
    loading.show_delay();
    QString errstring;
    QString auth_code = ui->lineEdit_payno->text().trimmed().replace(" ", "");

    if("AA" == ch_paymodeno) {//移动
        QString sub_mch_id = vch_arg1.toString().trimmed();//arg1
        QString appid = vch_arg2.toString().trimmed();//arg2
        QByteArray rsa_public_key_data = vch_arg3.toByteArray();//arg3
        QString total_amount_x100 = QString::number(qRound(fee* 100));
        QString body = QObject::tr("摇钱树支付") +"," + QObject::tr("机器号") + lds::terminalCode;
        QString kbstore_id = vch_arg5.toString();

        mobile_pay_help::payType pay_type;
        if(mobile_pay_help::codepay(
                    pay_type,
                    errstring,

                    n_func::f_get_sysparm("mobile_alipay_sign_type", "RSA2"),
                    appid,
                    sub_mch_id,
                    auth_code,
                    total_amount_x100,

                    get_out_trade_no(tradeBarCode),
                    body,
                    kbstore_id,
                    rsa_public_key_data
                    )) {
            if(mobile_pay_help::pay_wx == pay_type) {
                ret_out_trade_no = get_out_trade_no(tradeBarCode);
                ret_pay_name = QObject::tr("微信支付");
            }

            if(mobile_pay_help::pay_zfb == pay_type) {
                ret_out_trade_no = get_out_trade_no(tradeBarCode);
                ret_pay_name = QObject::tr("支付宝支付");
            }

            toDDaccept();
            return;
        }
    }
    if("05" == ch_paymodeno) {//微信暂时没有条码支付

    }
    if("CC" == ch_paymodeno) {//龙支付

    }
    if("EE" == ch_paymodeno) {//联拓富
        QString appid = vch_arg1.toString();
        QString key = vch_arg2.toString();
        QString merchantCode = vch_arg3.toString();
        QString payType;
        liantuofu_pay payObj;

        connect(&payObj, SIGNAL(signal_code_query(qint64, qint64, QString)), &loading, SLOT(updateProgress(qint64, qint64, QString)));
        if(payObj.codepay(auth_code,
                                  get_out_trade_no(tradeBarCode),
                                  appid,
                                  merchantCode,
                                  key,
                                  QString().sprintf("%.02f", fee),
                                  payType,
                                  errstring)) {
            ret_out_trade_no = get_out_trade_no(tradeBarCode);
            ret_pay_name = payType;

            toDDaccept();
            return;
        }
    }
    if("06" == ch_paymodeno) {//支付宝
        QString app_id = vch_arg1.toString();
        QString store_id = vch_arg2.toString();
        QString rsa_private_key_path = vch_arg3.toString();

        alipay_help::alipay_public_request_argument arg0 = get_alipay_arg0(app_id, rsa_private_key_path);
        alipay_help::alipay_trade_pay_argument arg1;
        alipay_help::alipay_trade_pay_return_argument arg2;
        arg1.auth_code_not_null = auth_code;
        arg1.out_trade_no_not_null = get_out_trade_no(tradeBarCode);
        arg1.subject_not_null = QObject::tr("餐饮消费") + "," + QObject::tr("机器号") + lds::terminalCode;
        arg1.total_amount_not_null = QString::number(fee, 'f', 2);
        arg1.store_id = store_id;

        //本质上， 条码支付和二维码支付是独立的，这个同用一个arg0
        if(alipay_help::alipay_trade_pay(arg0, arg1, arg2)) {
            ret_out_trade_no = get_out_trade_no(tradeBarCode);
            ret_pay_name = QObject::tr("支付宝支付");
            toDDaccept();
            return;
        }
        errstring = alipay_help::errstring;
    }
    if("DD" == ch_paymodeno) {//微支付
        QString merchant_no = vch_arg1.toString();
        QString terminal_id = vch_arg2.toString();
        QString access_token = vch_arg3.toString();
        QString total_amount_x100 = QString::number(qRound(fee* 100));

        fupay_help::payType pay_type;
        fupay_help payObj;
        connect(&payObj, SIGNAL(signal_code_query(qint64,qint64)), &loading, SLOT(updateProgress(qint64,qint64)));
        if(payObj.codepay(pay_type, errstring,
                          auth_code,
                          total_amount_x100,
                          get_out_trade_no(tradeBarCode),

                          merchant_no,
                          terminal_id,
                          access_token
                          )) {
            ret_out_trade_no = get_out_trade_no(tradeBarCode);
            ret_pay_name = payObj.payTypeName(pay_type);
            toDDaccept();
            return;
        }
    }

    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败") +"\n"+ errstring);
}

void w_scr_dish_phone_pay::toprint()
{
    lds_messagebox_loading_show loading(this, "", "LOADING");
    loading.show_delay();

    QString bottom_desc;
    OnlinePayType pay_type = OnlinePayTypeNone;
    if(ch_paymodeno == "05") {//微信
        pay_type = OnlinePayTypeWeChat;
    }

    if(ch_paymodeno == "CC") {//龙支付
        pay_type = OnlinePayTypeWeChat;
    }
    if(ch_paymodeno == "EE") {//联拓富
        pay_type = OnlinePayTypeWeChat;
    }

    if(ch_paymodeno == "06") {//支付宝
        pay_type = OnlinePayTypeAliPay;
    }

    if(ch_paymodeno == "AA") {//支付宝
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("打印二维码"), QObject::tr("微信"), QObject::tr("支付宝"), QObject::tr("取消"))) {
        case 0:
            pay_type = OnlinePayTypeWeChat;
            bottom_desc = ui->label_wechat_desc->text();
            break;
        case 1:
            pay_type = OnlinePayTypeAliPay;
            bottom_desc = ui->label_alipay_desc->text();
            break;
        default:
            break;
        }
    }
    if(ch_paymodeno == "DD") {//支付宝
        return;
    }
    if(pay_type != OnlinePayTypeNone) {
        return;
    }
    //打印小票
    blockIns printins;
    printins.append(backheader::printer.drawText("a00,s00", rb_ddata.rb_title+"\n"));//居中不放大
    if(bottom_desc.length() > 0)
        printins.append(backheader::printer.drawText("", bottom_desc + "\n"));//清除缓存 居中 pixmap路径
    printins.append(backheader::printer.drawQR_Code("", (pay_type == OnlinePayTypeAliPay ? rb_ddata.rb_alipay : rb_ddata.rb_wechat) ));//清除缓存 居中 pixmap路径
    printins.append(backheader::printer.drawTextEnd("1B6405 1D5601", ""));//走纸5x， 切纸
    backheader::reslove_write(printins);
}

void w_scr_dish_phone_pay::to_save_audio(bool f)
{
    lds::sysconf->setValue("w_scr_dish_phone_pay_" + ch_paymodeno, f);
}

void w_scr_dish_phone_pay::tocheckOk()
{
    if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("支付成功"), QObject::tr("确定"), QObject::tr("取消"))) {
        this->accept();
    }
}

QString w_scr_dish_phone_pay::get_out_trade_no(const QString &type)
{
    if(type_out_trade_no_map.find(ch_paymodeno+type) != type_out_trade_no_map.end())
        return type_out_trade_no_map.value(ch_paymodeno+type);

    QDateTime cur_dt = n_func::f_get_sysdatetime();
    QString k  = lds::LOCAL_TERMINAL_INDEX_FF + lds::dt_str(cur_dt, "yyyyMMddhhmmss") + QString::number(type_out_trade_no_map.count());
    type_out_trade_no_map.insert(ch_paymodeno + type, k);

    return k;
}

alipay_help::alipay_public_request_argument w_scr_dish_phone_pay::get_alipay_arg0(const QString &app_id, const QString &rsa_private_key_path)
{
    alipay_help::alipay_public_request_argument arg0;
    arg0.app_id_not_null = app_id;
    arg0.charset_not_null="utf-8";
    arg0.sign_type=lds::sysconf->value("alipay_help/" + app_id).toString(); //alipay_help::sign_type_SHA1;
    arg0.timestamp_not_null= lds::cur_machine_dt_str();
    arg0.version_not_null="1.0";
    arg0.rsa_private_key_data_not_null = alipay_help::alipay_private_key_add_start_line(rsa_private_key_path, 0);

    return arg0;
}

void w_scr_dish_phone_pay::toDDexit()
{
    lds::DOUBLE_DATA ddata;
    ddata.udpKey = lds::udp_rb_code_request_end;
    ddata.rb_title = QObject::tr("支付取消");
    public_sql::doublescreensenddataByconf(ddata);
    this->reject();
}

void w_scr_dish_phone_pay::toDDaccept()
{
    lds::DOUBLE_DATA ddata;
    ddata.udpKey = lds::udp_rb_code_request_end;
    ddata.rb_title = QObject::tr("支付成功");
    public_sql::doublescreensenddataByconf(ddata);

    //
    if(ui->checkBox->isChecked()) {
        public_sql::save_login->ogg_play_polling->play("userdata/settings/ogg/", fee);
    }
    //
    this->accept();
}

