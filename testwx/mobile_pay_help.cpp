#include "mobile_pay_help.h"
#include <QDateTime>
#include <QTimer>
#include <QSslConfiguration>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QNetworkReply>
#include <QtDebug>

const QString mobile_pay_help_sign_type::RSA = "RSA";
const QString mobile_pay_help_sign_type::RSA2 = "RSA2";

mobile_pay_help_sign_type mobile_pay_help::sign_type_map;

mobile_pay_help::mobile_pay_help()
{
}
void mobile_pay_help::test()
{
    //    //金额为整数，单位分，如12.34元，实际传入1234
    //    //auth_code 自动替换清除空格
    //    QString appid;// = "2015121000954514";
    //    QString sub_mch_id = "1431014702";
    //    QString kbstore_id;// = "0000000100010005";
    //    QString auth_code;
    //    QString total_amount = "1";

    //    QString out_trade_no;
    //    QString body = QObject::tr("摇钱树支付");
    //    QString sub_appid = "";
    //    QString device_info = "";
    //    QString transaction_id;

    //    out_trade_no = "01"+QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
    //    auth_code = "1300 5768 2925 712750";

    //    ///微信扫码支付，测试可行
    //    //    if(codepay(
    //    //                transaction_id,

    //    //                appid,
    //    //                sub_mch_id,
    //    //                kbstore_id,
    //    //                auth_code,
    //    //                total_amount,

    //    //                out_trade_no,
    //    //                body
    //    //                )) {
    //    //        qDebug() << "codepay:" << "支付成功";
    //    //        qDebug() << "transaction_id" << transaction_id << total_amount;
    //    //        qDebug() << "out_trade_no" << out_trade_no << total_amount;
    //    //    } else {
    //    //        qDebug() << "codepay:" << "支付失败" << errstring;
    //    //    }

    //    ///微信二维码支付，测试可行
    //    QByteArray code_url;
    //    if(getwxpayurl(code_url, sub_mch_id, total_amount, out_trade_no, body)) {
    //        QString transaction_id;
    //        if(wxOrderQuery(transaction_id, sub_mch_id, out_trade_no, sub_appid)) {
    //        } else {
    //        }
    //    } else {
    //    }
    //    ///微信退款，暂时不使用，需要门店签合同授权
}

void mobile_pay_help::test1()
{
    //    QString appid = "2015121000954514";
    //    QString sub_mch_id;// = "1431014702";
    //    QString kbstore_id = "0000000100010005";
    //    QString auth_code;
    //    QString total_amount = "1";//支付宝的金额也是以分为单位

    //    QString out_trade_no;
    //    QString body = QObject::tr("摇钱树支付");
    //    QString sub_appid = "";
    //    QString device_info = "";
    //    QString transaction_id;

    //    out_trade_no = "01"+QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
    //    auth_code = "2846 46994981 978715";

    //    ///支付宝扫码支付，测试可行
    //    //    if(codepay(
    //    //                transaction_id,

    //    //                appid,
    //    //                sub_mch_id,
    //    //                kbstore_id,
    //    //                auth_code,
    //    //                total_amount,

    //    //                out_trade_no,
    //    //                body
    //    //                )) {
    //    //        qDebug() << "codepay:" << "支付成功";
    //    //        qDebug() << "transaction_id" << transaction_id << total_amount;
    //    //        qDebug() << "out_trade_no" << out_trade_no << total_amount;

    //    //    } else {
    //    //        qDebug() << "codepay:" << "支付失败" << errstring;
    //    //    }
    //    ///支付宝二维码支付，测试可行
    //    QByteArray code_url;
    //    if(getzfbpayurl(mobile_pay_help_sign_type::RSA,
    //                    code_url, appid,
    //                    total_amount, out_trade_no, body,
    //                    kbstore_id,
    //                    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAvprAybgqWuLiL1IYG/nT13oRtR782HXzapBfORVNEo+iGVBLpvH9XSxI1JzQ3/TMjiN4HNB5DNLoepssEO3WAlD2MUa9KWbhWpfRR/ps3aEPPIvSfENXQeJF2re/CbX4jNvsFStAgEgd23/ch6bkH/yilKdMTr8UFtlL4RQ/ECO8gmpo7j+LDwsP83qdOm/NHcdQNOmqPom/ByGVWl9C4ZDEfkjbMpM9MZuMaB6zjQ18KbLrrN4+9DxsA7/HCq33s7BP6gptfMfhq3UoM67GDUAgX4D9PsjMx4cWAu6t9lF64ZeWj51/8YKd2j8+OVSx+W1JMnye3rJnjpBt5PEBRwIDAQAB")) {
    //        qDebug() << "getwxpayurl" << __LINE__ << code_url;
    //        //        QString trade_no;
    //        //        if(zfbOrderQuery("RSA", trade_no, appid, out_trade_no)) {
    //        //                        qDebug() << "zfbOrderQuery" << trade_no;
    //        //        } else {
    //        //                        qDebug() << __LINE__ << errstring;
    //        //        }
    //    } else {
    //        qDebug() << __LINE__ << errstring;
    //    }
    //    ///支付宝退款操作，测试可行
    //    zfbrefund("RSA", "cz-desktop20170121150514781", appid, "", "1");
}

QStringList mobile_pay_help::mobile_pay_alipay_keys()
{
    return sign_type_map.keys();
}

bool mobile_pay_help::codepay(mobile_pay_help::payType &pay_type, QString &errstring, const QString &sign_type, const QString &appid, const QString &sub_mch_id, const QString &auth_code, const QString &total_amount, const QString &out_trade_no, const QString &body, const QString &kbstore_id, const QByteArray &new_alipay_public_key, const QString &sub_appid, const QString &device_info)
{
    QByteArray recv;
    QByteArray data;

    data += "out_trade_no="+out_trade_no+"&";
    data += "total_amount="+total_amount+"&";
    data += "appid="+appid+"&";
    data += "auth_code="+auth_code.trimmed().replace(" ", "")+"&";

    data += "body="+body+"&";
    data += "kbstore_id="+kbstore_id+"&";
    data += "sub_mch_id="+sub_mch_id+"&";
    data += "sub_appid="+sub_appid+"&";
    data += "device_info="+device_info;
    if(sign_type == mobile_pay_help_sign_type::RSA2)
        data += "&new_alipay_public_key="+new_alipay_public_key.toPercentEncoding();
    if(auth_code.length() == 0) {
        errstring = QObject::tr("收款码为空");
        return false;
    }

    if( do_post("http://pay.scpos.com/SystemManage/wxzfb/codepay" + sign_type_map.value(sign_type), data,recv, &errstring)) {

        QSharedPointer<cJSON> p(cJSON_Parse(recv.data()));
        const QByteArray state = getcJSONvalue(p, "state").toByteArray();
        const QByteArray message = getcJSONvalue(p, "message").toByteArray();
        const QByteArray data = getcJSONvalue(p, "data").toByteArray();
        QString transaction_id;

        if("success" == state) {
            if(data == "1") {//zfb
                QByteArray d = getcJSONvalue(message,"alipay_trade_pay_response").toByteArray();
                transaction_id = getcJSONvalue(d, "trade_no").toString();
                pay_type = pay_zfb;
            }
            if(data == "2") {//wx
                transaction_id = getcJSONvalue(message, "transaction_id").toString();
                pay_type = pay_wx;
            }
            if(transaction_id.length() > 0) {
                return true;
            }
        }
        errstring = get_error(message, data, "alipay_trade_pay_response", "err_code_des");//err_code_des
    }
    return false;
}

bool mobile_pay_help::getwxpayurl(QByteArray &code_url, QString &errstring, const QString &sub_mch_id, const QString &total_amount, const QString &out_trade_no, const QString &body, const QString &sub_appid, const QString &device_info)
{
    QByteArray recv;
    QByteArray data;

    data += "out_trade_no="+out_trade_no+"&";
    data += "total_amount="+total_amount+"&";
    data += "body="+body+"&";
    data += "sub_mch_id="+sub_mch_id+"&";

    data += "sub_appid="+sub_appid+"&";
    data += "device_info="+device_info;
    if(sub_mch_id.length() == 0) {
        errstring = "sub_mch_id_no_null";
        return false;
    }

    if(do_post("http://pay.scpos.com/SystemManage/wxzfb/getwxpayurl",data,recv, &errstring)) {
        QSharedPointer<cJSON> p(cJSON_Parse(recv.data()));

        const QByteArray state = getcJSONvalue(p, "state").toByteArray();
        const QByteArray message = getcJSONvalue(p, "message").toByteArray();
        const QByteArray data = getcJSONvalue(p, "data").toByteArray();
        //tate = "success" 成功还要继续判断 里面的json  除 请求二维码网址外
        if("success" == state) {
            code_url = message;
            return true;
        }
        errstring = get_error_wx(message, data, "err_code_des");
    }
    return false;
}

bool mobile_pay_help::wxOrderQuery(QString &errstring, const QString &sub_mch_id, const QString &out_trade_no, const QString &sub_appid)
{
    QByteArray recv;
    QByteArray data;

    data += "out_trade_no="+out_trade_no+"&";
    data += "sub_mch_id="+sub_mch_id+"&";
    data += "sub_appid="+sub_appid;

    if( do_post("http://pay.scpos.com/SystemManage/wxzfb/wxOrderQuery",data,recv, &errstring)) {
        QSharedPointer<cJSON> p(cJSON_Parse(recv.data()));
        const QByteArray state = getcJSONvalue(p, "state").toByteArray();
        const QByteArray message = getcJSONvalue(p, "message").toByteArray();
        const QByteArray data = getcJSONvalue(p, "data").toByteArray();
        if("success" == state) {
            //微信判断
            QString trade_no = getcJSONvalue(message, "transaction_id").toString();
            if(trade_no.length() > 0) {
                return true;
            }
        }
        errstring = get_error_wx(message, data, "err_code_des");
    }
    return false;
}

bool mobile_pay_help::wxrefund(QString &errstring, const QString &out_trade_no, const QString &total_fee, const QString &refund_fee, const QString &sub_mch_id, const QString &sub_appid)
{
    QByteArray recv;
    QByteArray data;

    data += "out_trade_no="+out_trade_no+"&";
    data += "total_fee="+total_fee+"&";
    data += "refund_fee="+refund_fee+"&";
    data += "sub_appid="+sub_appid+"&";
    data += "sub_mch_id="+sub_mch_id;

    if( do_post("http://pay.scpos.com/SystemManage/wxzfb/wxrefund",data,recv, &errstring)) {
        QSharedPointer<cJSON>p(cJSON_Parse(recv.data()));

        const QByteArray state = getcJSONvalue(p, "state").toByteArray();
        const QByteArray message = getcJSONvalue(p, "message").toByteArray();
        const QByteArray data = getcJSONvalue(p, "data").toByteArray();
        if("success" == state) {
            return true;
        }
        errstring = get_error_wx(message, data, "return_msg");
    }
    return false;
}

bool mobile_pay_help::getzfbpayurl(QByteArray &code_url, QString &errstring, const QString &sign_type, const QString &appid, const QString &total_amount, const QString &out_trade_no, const QString &body, const QString &kbstore_id, const QByteArray &new_alipay_public_key, const QString &device_info)
{
    QByteArray recv;
    QByteArray data;

    data += "appid="+appid+"&";
    data += "total_amount="+total_amount+"&";
    data += "out_trade_no="+out_trade_no+"&";
    data += "body="+body+"&";
    data += "kbstore_id="+kbstore_id+"&";

    data += "device_info="+device_info;
    if(sign_type == mobile_pay_help_sign_type::RSA2)
        data += "&new_alipay_public_key="+new_alipay_public_key.toPercentEncoding();

    if(do_post("http://pay.scpos.com/SystemManage/wxzfb/getzfbpayurl" + sign_type_map.value(sign_type) ,data,recv, &errstring)) {
        QSharedPointer<cJSON> p(cJSON_Parse(recv.data()));

        const QByteArray state = getcJSONvalue(p, "state").toByteArray();
        const QByteArray message = getcJSONvalue(p, "message").toByteArray();
        const QByteArray data = getcJSONvalue(p, "data").toByteArray();
        if("success" == state) {
            code_url = message;
            return true;
        }
        errstring = get_error_zfb(message, data, "alipay_trade_precreate_response");
    }
    return false;
}

bool mobile_pay_help::zfbOrderQuery(QString &errstring, const QString &sign_type, const QString &appid, const QString &out_trade_no, const QByteArray &new_alipay_public_key)
{
    QByteArray recv;
    QByteArray data;

    data += "appid="+appid+"&";
    data += "out_trade_no="+out_trade_no;
    if(sign_type == mobile_pay_help_sign_type::RSA2)
        data += "&new_alipay_public_key="+new_alipay_public_key.toPercentEncoding();

    if( do_post("http://pay.scpos.com/SystemManage/wxzfb/zfbOrderQuery" + sign_type_map.value(sign_type) ,data,recv, &errstring)) {
        QSharedPointer<cJSON> p(cJSON_Parse(recv.data()));

        const QByteArray state = getcJSONvalue(p, "state").toByteArray();
        const QByteArray message = getcJSONvalue(p, "message").toByteArray();
        const QByteArray data = getcJSONvalue(p, "data").toByteArray();
        errstring = message;
        if("success" == state) {
            cJSON *root = cJSON_Parse(message.data());
            if(root) {
                QSharedPointer<cJSON> d(root);
                cJSON *item = cJSON_GetObjectItem(root,"alipay_trade_query_response");
                QString trade_no = getcJSONvalue(item, "trade_no").toString();
                if(trade_no.length() > 0) {
                    return true;
                }
            }
        }
        errstring = get_error_zfb(message, data, "alipay_trade_query_response");
    }

    return false;
}

bool mobile_pay_help::getzfbpublic_key(QByteArray &ret_rsa_public_key, QString &errstring, const QString &sign_type)
{
    QByteArray recv;
    if( do_get("http://pay.scpos.com/SystemManage/wxzfb/getzfbpublic_key" + sign_type_map.value(sign_type),recv, &errstring)) {
        QSharedPointer<cJSON>p(cJSON_Parse(recv.data()));

        const QByteArray state = getcJSONvalue(p, "state").toByteArray();
        const QByteArray message = getcJSONvalue(p, "message").toByteArray();
        if("success" == state) {
            ret_rsa_public_key = message;
            return true;
        }
        errstring = "unknow error";
    }
    return false;
}

bool mobile_pay_help::zfbrefund(QString &errstring, const QString &sign_type, const QString &trade_no, const QString &appid, const QString &refund_reason, const QString &refund_amount, const QByteArray &new_alipay_public_key)
{
    QByteArray recv;
    QByteArray data;

    data += "out_trade_no="+trade_no+"&";
    data += "appid="+appid+"&";
    data += "refund_reason="+refund_reason+"&";
    data += "refund_amount="+refund_amount;
    if(sign_type == mobile_pay_help_sign_type::RSA2)
        data += "&new_alipay_public_key="+new_alipay_public_key.toPercentEncoding();

    if( do_post("http://pay.scpos.com/SystemManage/wxzfb/zfbrefund" + sign_type_map.value(sign_type) ,data,recv, &errstring)) {
        QSharedPointer<cJSON> p(cJSON_Parse(recv.data()));

        const QByteArray state = getcJSONvalue(p, "state").toByteArray();
        const QByteArray message = getcJSONvalue(p, "message").toByteArray();
        const QByteArray data = getcJSONvalue(p, "data").toByteArray();
        errstring = message;
        if("success" == state) {
            return true;
        }
        errstring = get_error_zfb(message, data, "alipay_trade_refund_response");
    }
    return false;
}

bool mobile_pay_help::do_post(const QString &url, const QByteArray &content, QByteArray &responseData, QString *errstring)
{
    int contentLength = content.length();
    QNetworkRequest req;
    QNetworkAccessManager manager;
    QEventLoop eloop;
    QTimer timer;
    req.setUrl(QUrl(url));
    //application/octet-stream
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    req.setHeader(QNetworkRequest::ContentLengthHeader,contentLength);

    qDebug() << "url:" << url ;
    qDebug() << "body" << content;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#else
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1);
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    req.setSslConfiguration(config);
#endif

    timer.setSingleShot(true);
    QObject::connect(&timer,SIGNAL(timeout()),&eloop,SLOT(quit()));
    QObject::connect(&manager,SIGNAL(finished(QNetworkReply*)),&eloop,SLOT(quit()));
    QNetworkReply *reply=manager.post(req,content);
    timer.start(60*1000);
    eloop.exec(QEventLoop::ExcludeUserInputEvents);
    responseData = reply->readAll();
    qDebug() << "responseData" << responseData.data();

    if(timer.isActive()) {
        if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200) {
            return true;
        }
        *errstring = reply->errorString();
        return false;
    }
    *errstring = "timeout";
    return false;
}

bool mobile_pay_help::do_get(const QString &url, QByteArray &responseData, QString *errstring)
{
    QNetworkRequest req;
    QNetworkAccessManager manager;
    QEventLoop eloop;
    QTimer timer;
    req.setUrl(QUrl(url));
    //application/octet-stream
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    qDebug() << "url:" << url ;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#else
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1);
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    req.setSslConfiguration(config);
#endif

    timer.setSingleShot(true);
    QObject::connect(&timer,SIGNAL(timeout()),&eloop,SLOT(quit()));
    QObject::connect(&manager,SIGNAL(finished(QNetworkReply*)),&eloop,SLOT(quit()));
    QNetworkReply *reply=manager.get(req);
    timer.start(60*1000);
    eloop.exec(QEventLoop::ExcludeUserInputEvents);

    if(timer.isActive()) {
        if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200) {
            responseData = reply->readAll();
            return true;
        }
        *errstring = reply->errorString();
        return false;
    }
    *errstring = "timeout";
    return false;
}

QString mobile_pay_help::get_error(const QByteArray &message, const QByteArray &data, const char *json_zfb_string, const char *json_wx_string)
{
    QString err;
    if(data == "0") {//云端返回
        err = message;
    }
    if(data == "1") {//支付宝返回
        QByteArray d = getcJSONvalue(message, json_zfb_string).toByteArray();
        err = getcJSONvalue(d, "sub_msg").toString();
    }
    if(data == "2") {//微信返回
        err = getcJSONvalue(message, json_wx_string).toString();
    }
    if(err.isEmpty()) {
        err = message;
    }
    if(err.isEmpty()) {
        err = "unknow error";
    }

    return err;
}

QString mobile_pay_help::get_error_zfb(const QByteArray &message, const QByteArray &data, const char *json_string)
{
    if(data == "0")
        return message;

    return get_error(message, "1", json_string, 0);
}

QString mobile_pay_help::get_error_wx(const QByteArray &message, const QByteArray &data, const char *json_string)
{
    if(data == "0")
        return message;

    return get_error(message, "2", 0, json_string);
}

QVariant mobile_pay_help::getcJSONvalue(cJSON *root, const char *string)
{
    if(0 == root)
        return QVariant();

    cJSON *item = cJSON_GetObjectItem(root,string);
    return getItemValue(item);
}

QVariant mobile_pay_help::getcJSONvalue(const QByteArray &d, const char *string)
{
    QSharedPointer<cJSON> p(cJSON_Parse(d.data()));
    return getcJSONvalue(p.data(), string);
}

QVariant mobile_pay_help::getcJSONvalue(QSharedPointer<cJSON> d, const char *string)
{
    return getcJSONvalue(d.data(), string);
}

QVariant mobile_pay_help::getItemValue(cJSON *item)
{
    QVariant ret;
    if(0 == item) {
        qDebug("json return item is null , please check ");
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
