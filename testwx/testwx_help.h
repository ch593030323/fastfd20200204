#ifndef TESTWX_HELP_H
#define TESTWX_HELP_H

#include <QObject>
#include<QNetworkRequest>
#include<QNetworkReply>
#include<QEventLoop>
#include<QUuid>
#include<QRegExp>
#include<QCryptographicHash>
#include<QXmlStreamReader>
#include<QMap>
#include "liberweima/erweima.h"
#include "cJSON.h"

enum OnlinePayType{
    OnlinePayTypeNone = 0,
    OnlinePayTypeWeChat,
    OnlinePayTypeAliPay
};


class testwx_help : public QObject
{
    Q_OBJECT
public:
    typedef struct  {
        QString return_code;
        QString return_msg;
        QString appid;
        QString mch_id;
        QString nonce_str;
        QString sign;
        QString result_code;
        QString prepay_id;
        QString code_url;
        QString trade_type;
        QString err_code;
        QString err_code_des;
        QString time_end;
        QString trade_state;
        QString total_fee;
        QString transaction_id;

    }retDataStruct;
public:
    explicit testwx_help(QObject *parent = 0);

    static bool AmountQRcodeRequest(const QString &appid,
                                    const QString &tradedesc,
                                    int total_fee,
                                    const QString &paykey,
                                    const QString &mch_id,
                                    const QString &out_trade_no,
                                    QByteArray &qrcode);
    static bool WeixinPost(const QString &url, const QByteArray &content, QByteArray&responseData, QString *errstring = 0);
    static QString getuuid();

    static retDataStruct getPayBillInfo(const QString &out_trade_no,
                                        const QString &appid,
                                        const QString &paykey,
                                        const QString &mch_id);

    static retDataStruct reslovexml(QXmlStreamReader &streamxml);

    static QPixmap get_rbcode(const char *string);
    static QPixmap get_rbcode(const char *string, int width);
    static QPixmap get_rbcode(const char *string, int width, const QString &rb_title, const QString &rb_desc, const QColor & rb_color);
    static QPixmap get_rbcode(const char *string, int width, const QString &rb_title, const QString &rb_desc, const QColor & rb_color, int border);
    static QPixmap get_rbcode(
            const QString &title,
            const char *rb_wechat, const QString &title_wechat,
            const char *rb_alipay, const QString &title_alipay,
            const QString &ch_paymodeno);

    static QString OnlinePayTypeDesc(int type);

    static QVariant getcJSONvalue(cJSON *root, const char *string);
    static QVariant getcJSONvalue(const QByteArray &json, const char *string);
};

#endif // TESTWX_HELP_H
