#include "w_sys_manage_cloudsync_transfer.h"
#include <QtDebug>
#include <QTimer>
#include "w_sys_manage_cloudsync.h"
#include "networkhttprequest.h"
#include "saas_pay_detail.h"
#include "cjson_help.h"
#include <QFile>
#include <QEventLoop>
#include "lds.h"

w_sys_manage_cloudsync_transfer::w_sys_manage_cloudsync_transfer(QObject *parent) :
    QObject(parent)
{
}

httpDataAutoDel w_sys_manage_cloudsync_transfer::sendPost2(const QString &url, const QByteArray &param, const QMap<QString, QString> &headInfo, QString &retparam, int mesc, QString *errstring)
{
    httpDataAutoDel httpd;
    QByteArray  strResponse;
    QList<networkHttpRequest::httpHeader> httpdList;

    if(!headInfo.contains("Content-Type"))
        httpdList  << networkHttpRequest::httpHeader("Content-Type", "application/json;charset=utf-8");
    if(!headInfo.contains("terminalCode"))
        httpdList  << networkHttpRequest::httpHeader("terminalCode", lds::terminalCode.toLocal8Bit());
    for(QMap<QString, QString>::const_iterator k = headInfo.begin(); k != headInfo.end(); k++) {
        if(k.key() == "orgCode"
                || k.key() == "userName"
                || k.key() == "password"
                ) {
        } else {
            httpdList << networkHttpRequest::httpHeader(k.key().toLocal8Bit(), k.value().toLocal8Bit());
        }
    }

    httpd.httpStatus = networkHttpRequest::post(url, httpdList, param, strResponse, mesc, errstring);
    switch(httpd.httpStatus) {
    case 200:
        retparam = strResponse;
        httpd.json= cJSON_help::createSharedPtr(retparam.toLocal8Bit());
        break;
    case 550:
        retparam = strResponse;
        httpd.json = cJSON_help::createSharedPtr(retparam.toLocal8Bit());
        httpd.message = cJSON_help::getcJSONvalue(httpd.json, "message").toString();
        httpd.code = cJSON_help::getcJSONvalue(httpd.json, "code").toString();

        if(errstring)
            *errstring = httpd.message;
        break;
    default:
        httpd.message=strResponse;
    }
    return httpd;
}
/**
若是错误，则返回retparam，包含错误信息
*/
httpDataAutoDel w_sys_manage_cloudsync_transfer::sendGet(const QString &url, const QMap<QString, QString> &headInfo, QString &retparam, int mesc, QString *errstring)
{
    httpDataAutoDel httpd;
    QByteArray  strResponse;
    QList<networkHttpRequest::httpHeader> httpdList;

    if(!headInfo.contains("Content-Type"))
        httpdList  << networkHttpRequest::httpHeader("Content-Type", "application/json;charset=utf-8");
    if(!headInfo.contains("terminalCode"))
        httpdList  << networkHttpRequest::httpHeader("terminalCode", lds::terminalCode.toLocal8Bit());
    for(QMap<QString, QString>::const_iterator k = headInfo.begin(); k != headInfo.end(); k++) {
        if(k.key() == "orgCode"
                || k.key() == "userName"
                || k.key() == "password"
                ) {
        } else {
            httpdList << networkHttpRequest::httpHeader(k.key().toLocal8Bit(), k.value().toLocal8Bit());
        }
    }
    httpd.httpStatus = networkHttpRequest::get(url, httpdList, strResponse, mesc, errstring);
    switch(httpd.httpStatus) {
    case 200:
        retparam =strResponse;
        httpd.json=cJSON_help::createSharedPtr(retparam.toLocal8Bit());
        break;
    case 550:
        retparam =strResponse;
        httpd.json=cJSON_help::createSharedPtr(retparam.toLocal8Bit());
        httpd.message= cJSON_help::getcJSONvalue(httpd.json, "message").toString();
        httpd.code = cJSON_help::getcJSONvalue(httpd.json, "code").toString();

        if(errstring)
            *errstring = httpd.message;
        break;
    default:
        httpd.message=strResponse;
    }

    return httpd;
}

QNetworkReply *w_sys_manage_cloudsync_transfer::directGet(QNetworkAccessManager *networkAccessManager, const QUrl &url)
{
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/java;charset=utf-8");
    request.setUrl(url);
    int mesc = 60 * 1000;
    QEventLoop eloop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer,SIGNAL(timeout()),&eloop,SLOT(quit()));
    QObject::connect(networkAccessManager,SIGNAL(finished(QNetworkReply*)),&eloop,SLOT(quit()));
    QNetworkReply *reply = networkAccessManager->get(request);
    timer.start(mesc);
    eloop.exec();
    return reply;
}
