#include "fupay_help.h"
#include <QDateTime>
#include <QTimer>
#include <QSslConfiguration>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QNetworkReply>
#include <QtDebug>

QString fupay_help::payTypeName(fupay_help::payType pay_type)
{
    if(pay_type == pay_NONE) return "";

    if(pay_type == pay_WEIXIN) return QObject::tr("微信");
    if(pay_type == pay_ZHIFUBAO) return QObject::tr("支付宝");
    if(pay_type == pay_QQQIANBAO) return QObject::tr("QQ钱包");
    if(pay_type == pay_JDQIANBAO) return QObject::tr("京东钱包");
    if(pay_type == pay_KOUBEI) return QObject::tr("口碑");

    if(pay_type == pay_YIZHIFU) return QObject::tr("翼支付");
    if(pay_type == pay_YINGLIANERWEIMA) return QObject::tr("银联二维码");

    return "";
}

fupay_help::fupay_help(QObject *parent):QObject(parent)
{

}

bool fupay_help::codepay(payType &pay_type, QString &errstring, const QString &auth_no, const QString &total_fee, const QString &out_trade_no, const QString &merchant_no, const QString &terminal_id, const QString &access_token)
{
    //result_code  业务结果：01成功 02失败 ，03支付中
    QString trade_no;
    QString result_code;

    result_code = fupayCodePay(errstring, trade_no, auth_no, total_fee, out_trade_no, merchant_no, terminal_id, access_token);
    if("01" == result_code) {//成功
        return true;
    }
    if("02" == result_code) {//失败
        return false;
    }
    if("03" == result_code) {//支付中
        //5000 × 15
        //1000 × 30
        for(int k = 0, count = 30; k < count; k++) {
            if(fupayOrderQuery(pay_type, errstring, merchant_no, terminal_id, out_trade_no, access_token, trade_no)) {
                return true;
            }
            emit signal_code_query(k + 1, count, errstring);
            emit signal_code_query(k + 1, count);
#ifdef QT_DEBUG
            QTimer::singleShot(100, &eloop, SLOT(quit()));
#else
            QTimer::singleShot(1000, &eloop, SLOT(quit()));
#endif
            eloop.exec();
        }
        errstring = QObject::tr("支付失败");
    }
    return false;
}

QString fupay_help::fupayCodePay(QString &errstring, QString &out_trade_no, const QString &auth_no, const QString &total_fee, const QString &terminal_trace, const QString &merchant_no, const QString &terminal_id, const QString &access_token)
{
    QByteArray recv;
    QByteArray data;

    data += "merchant_no="+merchant_no+"&";
    data += "terminal_id="+terminal_id+"&";
    data += "terminal_trace="+terminal_trace+"&";
    data += "total_fee="+total_fee+"&";
    data += "auth_no="+auth_no+"&";

    data += "access_token="+access_token;


    if( do_post("http://pay.scpos.com/SystemManage/wxzfb/fupay_barcodepay", data,recv, errstring)) {
        QSharedPointer<cJSON> p(cJSON_Parse(recv.data()));
        const QByteArray state = getcJSONvalue(p, "state").toByteArray();
        const QByteArray message = getcJSONvalue(p, "message").toByteArray();
        const QByteArray data = getcJSONvalue(p, "data").toByteArray();
        errstring = message;
        if("success" == state) {
            //result_code  业务结果：01成功 02失败 ，03支付中
            QString result_code = getcJSONvalue(message, "result_code").toString();
            QString trade_no = getcJSONvalue(message, "out_trade_no").toString();
            if(trade_no.length() > 0) {
                out_trade_no = trade_no;
                return result_code;
            }
        }
        errstring = get_error(message, data);
    }
    return "";
}

bool fupay_help::fupayOrderQuery(fupay_help::payType &pay_type, QString &errstring, const QString &merchant_no, const QString &terminal_id, const QString &terminal_trace, const QString &access_token, const QString &out_trade_no)
{
    QByteArray recv;
    QByteArray data;

    data += "merchant_no="+merchant_no+"&";
    data += "terminal_id="+terminal_id+"&";
    data += "terminal_trace="+terminal_trace+"&";
    data += "access_token="+access_token+"&";
    data += "out_trade_no="+out_trade_no;

    if( do_post("http://pay.scpos.com/SystemManage/wxzfb/fupay_query", data,recv, errstring)) {
        QSharedPointer<cJSON> p(cJSON_Parse(recv.data()));
        const QByteArray state = getcJSONvalue(p, "state").toByteArray();
        const QByteArray message = getcJSONvalue(p, "message").toByteArray();
        const QByteArray data = getcJSONvalue(p, "data").toByteArray();
        errstring = message;
        if("success" == state) {
            const QString result_code = getcJSONvalue(message, "result_code").toString();
            const QString pay_type_str = getcJSONvalue(message, "pay_type").toString();
            if(result_code == "01") {
                pay_type = pay_NONE;

                /*
请求类型，010微信，020 支付宝，060QQ钱包，080京东钱包，090口
碑，100翼支付，110银联二维码
*/
                if(pay_type_str == "010") pay_type = pay_WEIXIN;
                if(pay_type_str == "020") pay_type = pay_ZHIFUBAO;
                if(pay_type_str == "060") pay_type = pay_QQQIANBAO;
                if(pay_type_str == "080") pay_type = pay_JDQIANBAO;
                if(pay_type_str == "090") pay_type = pay_KOUBEI;

                if(pay_type_str == "100") pay_type = pay_YIZHIFU;
                if(pay_type_str == "110") pay_type = pay_YINGLIANERWEIMA;

                qDebug() << "message:" << message << pay_type;
                return true;
            }
        }
        errstring = get_error(message, data);
    }
    return false;
}

bool fupay_help::do_post(const QString &url, const QByteArray &content, QByteArray &responseData, QString &errstring)
{
    int contentLength = content.length();
    QNetworkRequest req;
    QNetworkAccessManager manager;
    req.setUrl(QUrl(url));
    //application/octet-stream
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    req.setHeader(QNetworkRequest::ContentLengthHeader,contentLength);

    qDebug() << "url:" << url ;
    qDebug() << "body" << content;

    QObject::connect(&manager,SIGNAL(finished(QNetworkReply*)),&eloop,SLOT(quit()));
    QNetworkReply *reply=manager.post(req,content);
    eloop.exec();

    if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200) {
        responseData = reply->readAll();
        qDebug() << responseData.data();
        return true;
    }
    errstring = reply->errorString();
    return false;
}

QString fupay_help::get_error(const QByteArray &message, const QByteArray &data)
{
    QString err;
    if(data == "0") {//云端返回
        err = message;
    }
    err = getcJSONvalue(message, "return_msg").toString();

    if(err.isEmpty()) {
        err = message;
    }
    if(err.isEmpty()) {
        err = "unknow error";
    }
    return err;
}

QVariant fupay_help::getcJSONvalue(cJSON *root, const char *string)
{
    if(0 == root)
        return QVariant();

    cJSON *item = cJSON_GetObjectItem(root,string);
    return getItemValue(item);
}

QVariant fupay_help::getcJSONvalue(const QByteArray &d, const char *string)
{
    QSharedPointer<cJSON> p(cJSON_Parse(d.data()));
    return getcJSONvalue(p.data(), string);
}

QVariant fupay_help::getcJSONvalue(QSharedPointer<cJSON> d, const char *string)
{
    return getcJSONvalue(d.data(), string);
}

QVariant fupay_help::getItemValue(cJSON *item)
{
    QVariant ret;
    if(0 == item) {
        qDebug("json return item is null,  please check ");
        return ret;
    }
    if(item->type == cJSON_String) {
        ret = item->valuestring;
    } else if(item->type == cJSON_Number) {
        if(item->valuedouble != double(item->valueint)) {
            ret = item->valuedouble;
        } else {
            ret = item->valueint;
        }
    } else if(item->type == cJSON_Object) {
        char *str = cJSON_Print(item);
        ret = str;
        delete str;
        str = 0;
    }
    return ret;
}
