#include "networkhttprequest.h"
#include <QtNetwork>
#include <string>
#include <qdebug.h>

networkHttpRequest::networkHttpRequest(QObject *parent) :
    QObject(parent)
{
}

int networkHttpRequest::post(const QString &url, const QList<networkHttpRequest::httpHeader> &httpd_list, const QByteArray &data, QByteArray &retdata, int mesc, QString *errstring)
{
    QNetworkRequest request;
    foreach (const networkHttpRequest::httpHeader &d, httpd_list) {
        if(d.key == "Content-Type") {
            request.setHeader(QNetworkRequest::ContentTypeHeader, d.value);
        } else if(d.key == "ContentLength") {
            request.setHeader(QNetworkRequest::ContentLengthHeader, d.value.toInt());
        } else {
            request.setRawHeader(d.key, d.value);
        }
    }
    qDebug() << __FUNCTION__ << url;
    foreach(const QByteArray &k, request.rawHeaderList())
        qDebug() <<  "head " <<k << request.rawHeader(k);
    qDebug() << data;
    request.setUrl(url);
    QEventLoop eloop;
    QNetworkAccessManager networkAccessManager;
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer,SIGNAL(timeout()),&eloop,SLOT(quit()));
    connect(&networkAccessManager,SIGNAL(finished(QNetworkReply*)),&eloop,SLOT(quit()));
    QNetworkReply *reply = networkAccessManager.post(request,data);
    timer.start(mesc);
    eloop.exec();

    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    switch(httpStatus) {
    case 200:
        retdata = reply->readAll();
        break;
    case 550:
        retdata = reply->readAll();
        break;
    default:
        if(false == timer.isActive())retdata = "HTTP :" "timeOut";
        else  retdata = "HTTP :" + QByteArray::number(httpStatus);
        if(errstring) *errstring = retdata;
    }
    return httpStatus;
}

int networkHttpRequest::get(const QString &url, const QList<networkHttpRequest::httpHeader> &httpd_list, QByteArray &retdata, int mesc, QString *errstring)
{
    QNetworkRequest request;
    foreach (const networkHttpRequest::httpHeader &d, httpd_list) {
        if(d.key == "Content-Type") {
            request.setHeader(QNetworkRequest::ContentTypeHeader, d.value);
        } else if(d.key == "ContentLength") {
            request.setHeader(QNetworkRequest::ContentLengthHeader, d.value.toInt());
        } else {
            request.setRawHeader(d.key, d.value);
        }
    }

    request.setUrl(url);
    QEventLoop eloop;
    QNetworkAccessManager networkAccessManager;
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer,SIGNAL(timeout()),&eloop,SLOT(quit()));
    connect(&networkAccessManager,SIGNAL(finished(QNetworkReply*)),&eloop,SLOT(quit()));
    QNetworkReply *reply = networkAccessManager.get(request);
    timer.start(mesc);
    eloop.exec();

    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    switch(httpStatus) {
    case 200:
        retdata = reply->readAll();
        break;
    case 550:
        retdata = reply->readAll();
        break;
    default:
        if(false == timer.isActive()) retdata = "HTTP :""time out";
        else  retdata = "HTTP :" + QByteArray::number(httpStatus)+"    "+reply->errorString().toLocal8Bit();
        if(errstring) *errstring = retdata;
    }

    return httpStatus;
}


networkhttpMangerLoop::networkhttpMangerLoop(QObject *parent)
    : QNetworkAccessManager(parent)
    , _timeout(-1)
{
    connect(this, SIGNAL(finished(QNetworkReply*)), &_loop, SLOT(quit()));
}

void networkhttpMangerLoop::postMessage(const QString &url, const QList<networkHttpRequest::httpHeader> &list, const QByteArray data)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    for(int k = 0; k < list.count(); k ++) {
        request.setRawHeader(list[k].key, list[k].value);
    }
    _reply = QSharedPointer<QNetworkReply>(this->post(request, data));
}

int networkhttpMangerLoop::httpStatus()
{
    if(_reply.isNull())
        return 0;
    return _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
}

const QByteArray &networkhttpMangerLoop::readAll()
{
    if(_reply.isNull())
        return "";
    _readall = _reply->readAll();
    return _readall;
}

QSharedPointer<cJSON> networkhttpMangerLoop::readAll2JSON()
{
    return QSharedPointer<cJSON>(cJSON_Parse(readAll()), cJSON_Delete);
}

int networkhttpMangerLoop::exec(QEventLoop::ProcessEventsFlags flags)
{
    if(-1 != _timeout)
        QTimer::singleShot(_timeout, &_loop, SLOT(quit()));
    return _loop.exec(flags);
}

void networkhttpMangerLoop::setTimeOut(int msec)
{
    _timeout = msec;
}
