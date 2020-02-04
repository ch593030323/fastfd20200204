#include "liantuofu_pay.h"
#include <QMap>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtDebug>
#include <QEventLoop>
#include <QCryptographicHash>
#include <QTimer>

liantuofu_pay::liantuofu_pay(QObject *parent) :
    QObject(parent)
{
}

bool liantuofu_pay::jspay_test(const QString &outTradeNo, const QString &totalAmount, QByteArray &payUrl, QString &errstring)
{
    QString appId = "EW_N5946005323";
    QString merchantCode = "EW_N4130797151";
    QString key = "f0329e22fb506a4e26ccb29b0a6c5af3";

    return jspay(outTradeNo, appId, merchantCode, key, totalAmount, payUrl, errstring);
}

bool liantuofu_pay::query_test(const QString &outTradeNo, QString &errstring)
{
    QString appId = "EW_N5946005323";
    QString merchantCode = "EW_N4130797151";
    QString key = "f0329e22fb506a4e26ccb29b0a6c5af3";

    return query(outTradeNo, appId, merchantCode, key, errstring);
}

bool liantuofu_pay::codepay_test(const QString &authCode, const QString &outTradeNo, QString &errstring)
{
    return true;
}

bool liantuofu_pay::jspay(const QString &outTradeNo, const QString &appId, const QString &merchantCode, const QString &key, const QString &totalAmount, QByteArray &payUrl, QString &errstring)
{
    QString random = QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + QString::number(rand() % 1000);

    //        QString totalAmount = "0.01";
    //        QString expireSeconds;
    //        QString operatorId;
    //        QString notifyUrl;

    QByteArray readall;

    QMap<QString, QString> map;
    map.insert("appId", appId);
    map.insert("random", random);
    map.insert("merchantCode", merchantCode);
    map.insert("outTradeNo", outTradeNo);

    map.insert("totalAmount", totalAmount);
    map.insert("notifyUrl", "");

    map.insert("sign", createSign(map, key));
    //
    QString postd = mapJoin(map);
    //
    QNetworkAccessManager manager;
    QNetworkRequest request;
    QEventLoop loop;
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    request.setUrl(QString("http://api.liantuofu.com/open/jspay"));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    QNetworkReply *reply = manager.post(request, postd.toLocal8Bit());
    loop.exec();
    readall = reply->readAll();

    qDebug() << postd;
    qDebug() <<readall;

    cJSONPointer obj(cJSON_Parse(readall));
    if(0 != obj.json) {
        if(QString("SUCCESS") == getcJSONString(obj.json, "code")) {
            payUrl = getcJSONString(obj.json, "url").toLocal8Bit();
            return true;
        }
        errstring = jsonTrimmd(obj.json);
        return false;
    }
    errstring = reply->errorString();
    return false;
}

bool liantuofu_pay::query(const QString &outTradeNo, const QString &appId, const QString &merchantCode, const QString &key, QString &errstring)
{
    QByteArray readall;
    QString random = QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + QString::number(rand() % 1000);

    QMap<QString, QString> map;
    map.insert("appId", appId);
    map.insert("random", random);
    map.insert("merchantCode", merchantCode);
    map.insert("outTradeNo", outTradeNo);

    map.insert("sign", createSign(map, key));
    //
    QString postd = mapJoin(map);
    //
    QNetworkAccessManager manager;
    QNetworkRequest request;
    QEventLoop loop;
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    request.setUrl(QString("http://api.liantuofu.com/open/pay/query"));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    QNetworkReply *reply = manager.post(request, postd.toLocal8Bit());
    loop.exec();
    readall = reply->readAll();

    qDebug() << __LINE__ << readall;

    cJSONPointer obj(cJSON_Parse(readall));
    if(0 != obj.json) {
        if(QString("SUCCESS") == getcJSONString(obj.json, "code")) {
            return true;
        }
        errstring = jsonTrimmd(obj.json);
        return false;
    }
    errstring = reply->errorString();
    return false;
}

//输入密码时，会直接报错
bool liantuofu_pay::codepay(const QString &authCode, const QString &outTradeNo, const QString &appId, const QString &merchantCode, const QString &key, const QString &totalAmount, QString &payType, QString &errstring)
{
    QString random = QString::number(QDateTime::currentMSecsSinceEpoch());

    //        QString totalAmount = "0.01";
    //        QString expireSeconds;
    //        QString operatorId;
    //        QString notifyUrl;

    QByteArray readall;

    QMap<QString, QString> map;
    map.insert("appId", appId);
    map.insert("random", random);
    map.insert("merchantCode", merchantCode);
    map.insert("outTradeNo", outTradeNo);

    map.insert("totalAmount", totalAmount);
    map.insert("authCode", authCode.trimmed().replace(" ", ""));

    map.insert("sign", createSign(map, key));
    //
    QString postd = mapJoin(map);
    //
    QNetworkAccessManager manager;
    QNetworkRequest request;
    QEventLoop loop;
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    request.setUrl(QString("http://api.liantuofu.com/open/pay"));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    QNetworkReply *reply = manager.post(request, postd.toLocal8Bit());
    loop.exec();
    readall = reply->readAll();

    qDebug() << postd;
    qDebug() <<readall;

    cJSONPointer obj(cJSON_Parse(readall));
    if(0 != obj.json) {
        payType = getcJSONString(obj.json, "payType");

        if(QString("SUCCESS") == getcJSONString(obj.json, "code")) {
            return true;
        }
        //~SUCCESS

        if(QString("USER_PAYING") == getcJSONString(obj.json, "subCode")) {
            QString subMsg = getcJSONString(obj.json, "subMsg");

            for(int k = 0, count = 30; k < count; k ++) {
                if(query(outTradeNo, appId, merchantCode, key, errstring)) {
                    return true;
                }
                emit signal_code_query(k + 1, count, subMsg);
                emit signal_code_query(k + 1, count);
#ifdef QT_DEBUG
                QTimer::singleShot(100, &loop, SLOT(quit()));
#else
                QTimer::singleShot(1000, &loop, SLOT(quit()));
#endif
                loop.exec();
            }
            errstring = QObject::tr("支付失败");
            return false;
        }
        //~USER_PAYING

        errstring = jsonTrimmd(obj.json);
        return false;
    }
    errstring = reply->errorString();
    return false;
}

QString liantuofu_pay::createSign(const QMap<QString, QString> &map, const QString &key)
{
    QString toBeSigned;
    QString sign;

    for(QMap<QString, QString>::const_iterator k = map.begin(); k != map.end(); k++) {
        if(k.value() == "" || k.key() == "sign" || k.key() == "sign_type") {

        } else {
            toBeSigned += k.key() + "=" + k.value() + "&";
        }
    }
    toBeSigned += "key=" + key + "&";
    toBeSigned.chop(1);
    //md5 转小写
    sign = QCryptographicHash::hash(toBeSigned.toLocal8Bit(), QCryptographicHash::Md5).
            toHex().
            toLower();

    return sign;
}

QString liantuofu_pay::mapJoin(const QMap<QString, QString> &map)
{
    QString d;
    for(QMap<QString, QString>::const_iterator k = map.begin();
        k != map.end();
        k++) {
        d += k.key() + "=" + k.value() + "&";
    }
    d.chop(1);

    return d;
}

QString liantuofu_pay::jsonTrimmd(cJSON *json)
{
    QString msg = cJSON_Print(json);
    msg.replace(" ", "").replace("\t", "");
    msg.remove(0, 1);
    msg.chop(1);

    return msg;
}

QString liantuofu_pay::getcJSONString(cJSON *item, const char *string)
{
    cJSON *json = cJSON_GetObjectItem(item, string);
    if(json)
        return json->valuestring;
    return "";
}
