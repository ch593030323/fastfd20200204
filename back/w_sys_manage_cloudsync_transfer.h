#ifndef W_SYS_MANAGE_CLOUDSYNC_TRANSFER_H
#define W_SYS_MANAGE_CLOUDSYNC_TRANSFER_H

#include <QObject>
#include <QMap>
#include <QNetworkReply>
#include <cJSON.h>
#include <QProgressBar>
#include <QDateTime>
#include "lds_bool.h"
#include "cjson_help.h"
#include "w_sys_manage_cloudsync.h"

class w_sys_manage_cloudsync_transfer : public QObject
{
    Q_OBJECT
public:
    explicit w_sys_manage_cloudsync_transfer(QObject *parent = 0);
    static httpDataAutoDel  sendPost2(const QString &url, const QByteArray &param, const QMap<QString, QString> &headInfo, QString &retparam, int mesc, QString *errstring = 0);
    static httpDataAutoDel  sendGet(const QString &url, const QMap<QString,QString> &headInfo, QString &retparam, int mesc, QString *errstring = 0);

    static QNetworkReply *directGet(QNetworkAccessManager *networkAccessManager, const QUrl &url);
};

#endif // W_SYS_MANAGE_CLOUDSYNC_TRANSFER_H
