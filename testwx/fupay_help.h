#ifndef FUPAY_HELP_H
#define FUPAY_HELP_H

#include <QVariant>
#include "cJSON.h"
#include <QEventLoop>
#include <QStringList>
#include <memory>
#include <QSharedPointer>


/*
关联进度窗口来实现15次查询的动态显示
【进度窗口】的要求：1、没有关闭按钮；2右上角没有关闭按钮；3closeEvent不允许点击退出；4keyPressEvent中屏蔽ALT+F4和ESC

【进度窗口】{
    this->setWindowFlags(Qt::FramelessWindowHint | this->windowFlags());
}

closeEvent(QCloseEvent *)
{

}

keyPressEvent(QKeyEvent *e)
{
    e->ignore();
}
showPaying(int cur, int total)
{
    progressBar->setRange(0, total);
    progressBar->setValue(cur);
    if(cur == total) {
        this->accept();
    }
}

调用方法：
void MainWindow::on_lineEdit_fupay_code_pay_returnPressed()
{
    //    商户号：852100302000008
    //    终端：30052759
    //    access_token:d7393417dc0d4a9dabc54ff5bd1657f6

    fupay_help payObj;
    【进度窗口】 dialog(this);
    connect(&payObj, SIGNAL(signal_code_query(int,int)), &dialog, SLOT(showPaying(int, int)));
    dialog.show();
    if(payObj.codepay(errstring,
                           auth_code,
                           "1",
                           out_trade_no,

                           "852100302000008",
                           "30052759",
                           "d7393417dc0d4a9dabc54ff5bd1657f6")) {
    } else {
        QMessageBox::warning(this, "", errstring);
    }
}
*/
class fupay_help : public QObject
{
    Q_OBJECT
public:
    enum payType{
        pay_NONE,

        pay_WEIXIN,//010微信
        pay_ZHIFUBAO,//020 支付宝
        pay_QQQIANBAO,//060QQ钱包
        pay_JDQIANBAO,//080京东钱包
        pay_KOUBEI,//090口碑

        pay_YIZHIFU,//100翼支付
        pay_YINGLIANERWEIMA,//110银联二维码
    };
    QString payTypeName(payType pay_type);

public:
    fupay_help(QObject *parent = 0);
    bool codepay(
            payType &pay_type,//返回支付类型
            QString &errstring,//返回错误

            const QString &auth_no,//支付码
            const QString &total_fee,//金额，单位分
            const QString &out_trade_no,//订单号，软件唯一

            const QString &merchant_no,//商户号
            const QString &terminal_id,//终端号
            const QString &access_token//令牌
            );
signals:
    void signal_code_query(qint64 cur, qint64 total, const QString &msg);
    void signal_code_query(qint64 cur, qint64 total);
private:

    ////////////////////////////////
    //微支付
    QString fupayCodePay(
            QString &errstring,//返回错误
            QString &out_trade_no,//订单号，平台返回

            const QString &auth_no,//支付码
            const QString &total_fee,//金额，单位分
            const QString &terminal_trace,//订单号，软件唯一

            const QString &merchant_no,//商户号
            const QString &terminal_id,//终端号
            const QString &access_token//令牌
            );
    bool fupayOrderQuery(
            payType &pay_type,//支付类型
            QString &errstring,//返回错误

            const QString &merchant_no,//商户号
            const QString &terminal_id,//终端号
            const QString &terminal_trace,//订单号，软件唯一
            const QString &access_token,//令牌
            const QString &out_trade_no//订单号，平台返回
            );
private:
    QEventLoop eloop;
private:
    bool do_post(const QString &url, const QByteArray &content, QByteArray&responseData, QString &errstring);
    static QString get_error(const QByteArray &message, const QByteArray &data );
    ///cjson
    static QVariant getcJSONvalue(cJSON *root, const char *string);
    static QVariant getcJSONvalue(const QByteArray &d, const char *string);
    static QVariant getcJSONvalue(QSharedPointer<cJSON> d, const char *string);
    static QVariant getItemValue(cJSON *item);

};

#endif // FUPAY_HELP_H
