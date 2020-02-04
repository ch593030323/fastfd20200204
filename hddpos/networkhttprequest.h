#ifndef NETWORKHTTPREQUEST_H
#define NETWORKHTTPREQUEST_H

#include <QObject>
#include <QNetworkRequest>
#include <QSharedPointer>
#include <QEventLoop>
#include <QNetworkReply>
#include "cJSON.h"


class networkHttpRequest : public QObject
{
    Q_OBJECT
public:
    struct httpHeader{
        inline httpHeader() {}
        inline httpHeader(const QByteArray &k, const QByteArray &v) {
            this->key = k;
            this->value = v;
        }
        QByteArray key;
        QByteArray value;
    };

public:
    explicit networkHttpRequest(QObject *parent = 0);

public :
    static int post(const QString &url, const QList<httpHeader> &httpd_list, const QByteArray &data, QByteArray &retdata,  int mesc = 20*60*1000, QString *errstring = 0);
//    static int post1(const QString &url, const QList<httpHeader> &httpd_list, const QByteArray &data, QByteArray &retdata,  int mesc = 20*60*1000, QString *errstring = 0);

    static int get(const QString &url, const QList<httpHeader> &httpd_list, QByteArray &retdata,  int mesc = 20*60*1000, QString *errstring = 0);
private:
};

class networkhttpMangerLoop : public QNetworkAccessManager{

    Q_OBJECT
public:
    networkhttpMangerLoop(QObject *parent = 0);

    void postMessage(const QString &url, const QList<networkHttpRequest::httpHeader> &list, const QByteArray data);
    int httpStatus();
    const QByteArray& readAll();
    QSharedPointer<cJSON> readAll2JSON();

    void setTimeOut(int msec);
    int exec(QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents);
private:
    QEventLoop _loop;
    int _timeout;
    QSharedPointer<QNetworkReply> _reply;
    QByteArray _readall;
};

#endif // NETWORKHTTPREQUEST_H
