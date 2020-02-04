#ifndef LDS_NETWORK_H
#define LDS_NETWORK_H

#include <QMap>
#include <QNetworkReply>
#include <cJSON.h>
#include <QProgressBar>
#include <QDateTime>
#include "lds_bool.h"
#include "cjson_help.h"
#include "lds_query_hddpos.h"

#define http_www_yqspos_com (QString("http://") + lds_query_hddpos::selectValue("select vch_value from cey_sys_parameter where vch_parameter='cloud_host' ", 0, "www.yqspos.com").toString())

class lds_network
{
public:
    lds_network();
    static bool managerPost(const QNetworkRequest &request, const QByteArray &body,
                            QByteArray &readall, QString &errstring, int &httpcode);
    static bool managerPut(const QNetworkRequest &request, const QByteArray &body,
                            QByteArray &readall, QString &errstring, int &httpcode);
    static bool managerGet(const QNetworkRequest &request,
                           QByteArray &readall, QString &errstring, int &httpcode);
};

class lds_network_ras_head{
public:
    lds_network_ras_head &append(const QString &key, const QVariant &value);
    QMap<QByteArray, QByteArray> d;
};

#endif // LDS_NETWORK_H
