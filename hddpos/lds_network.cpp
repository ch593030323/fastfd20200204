#include "lds_network.h"
#include "lds.h"
#include <QEventLoop>
#include <QtDebug>
#include <QTimer>

#ifdef QT_DEBUG
static int requestCount = 0;
#endif
lds_network::lds_network()
{
}

bool lds_network::managerPost(const QNetworkRequest &request, const QByteArray &body, QByteArray &readall, QString &errstring, int &httpcode)
{
    QNetworkAccessManager m;
    QEventLoop loop;
    QObject::connect(&m, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply *reply;
    bool retv = true;
    reply = m.post(request, body);
    loop.exec();
    readall = reply->readAll();
    httpcode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(readall.length() == 0
            && reply->error() != QNetworkReply::NoError) {
        retv =  false;
        errstring = reply->errorString();
    }

#ifdef QT_DEBUG
    //
    qDebug() << __FUNCTION__ << "requestCount:" << requestCount++;
    qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << request.url();
    foreach(const QByteArray &k, request.rawHeaderList())
        qDebug() << k << request.rawHeader(k);
    qDebug() << readall;
    qDebug() << body;
    qDebug() << reply->error();
    qDebug() << reply->errorString();
    //
#endif
    return retv;
}

bool lds_network::managerPut(const QNetworkRequest &request, const QByteArray &body, QByteArray &readall, QString &errstring, int &httpcode)
{
    QNetworkAccessManager m;
    QEventLoop loop;
    QObject::connect(&m, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply *reply;
    bool retv = true;
    reply = m.put(request, body);
//#ifdef QT_DEBUG
//    QTimer::singleShot(2000, &loop, SLOT(quit()));
//#endif
    loop.exec();
    if(reply->isRunning()) {
        reply->abort();
    }
    readall = reply->readAll();
    httpcode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(readall.length() == 0
            && reply->error() != QNetworkReply::NoError) {
        retv =  false;
        errstring = reply->errorString();
    }

#ifdef QT_DEBUG
    //
    qDebug() << __FUNCTION__ << "requestCount:" << requestCount++;
    qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << request.url();
    foreach(const QByteArray &k, request.rawHeaderList())
        qDebug() << k << request.rawHeader(k);
    qDebug() << readall;
    qDebug() << body;
    qDebug() << reply->error();
    qDebug() << reply->errorString();
    //
#endif
    return retv;
}

bool lds_network::managerGet(const QNetworkRequest &request, QByteArray &readall, QString &errstring, int &httpcode)
{
    QNetworkAccessManager m;
    QEventLoop loop;
    QObject::connect(&m, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply *reply;
    bool retv = true;
    reply = m.get(request);
    loop.exec();
    readall = reply->readAll();
    httpcode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(readall.length() == 0
            && reply->error() != QNetworkReply::NoError) {
        retv =  false;
        errstring = reply->errorString();
    }

#ifdef QT_DEBUG
    //
    qDebug() << __FUNCTION__ << "requestCount:" << requestCount++;
    qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << request.url();
    foreach(const QByteArray &k, request.rawHeaderList())
        qDebug() << k << request.rawHeader(k);
    qDebug() << readall;
    qDebug() << reply->error();
    qDebug() << reply->errorString();
    //
#endif
    return retv;
}

lds_network_ras_head &lds_network_ras_head::append(const QString &key, const QVariant &value)
{
    d.insert(key.toLocal8Bit(), value.toByteArray());
    return *this;
}
