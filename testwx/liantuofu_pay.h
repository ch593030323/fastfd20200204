#ifndef LIANTUOFU_PAY_H
#define LIANTUOFU_PAY_H

#include <QObject>
#include <QMap>
#include <cJSON.h>
//联拓富
//https://www.showdoc.cc/liantuofu?page_id=721259087608792
class liantuofu_pay : public QObject
{
    Q_OBJECT
public:
    explicit liantuofu_pay(QObject *parent = 0);

    bool jspay_test(const QString &outTradeNo, //订单号
                           const QString &totalAmount, //0.01
                           QByteArray &payUrl, //qr_code
                           QString &errstring
                           );
    bool query_test(const QString &outTradeNo,//订单号
                           QString &errstring
                           );
    bool codepay_test(const QString &authCode, //支付授权码
                             const QString &outTradeNo,//订单号
                             QString &errstring);

    //聚合支付生产二维码
    bool jspay(
            const QString &outTradeNo,//订单号
            const QString &appId,//接入标识
            const QString &merchantCode,//门店编号
            const QString &key,//密钥

            const QString &totalAmount,//0.01
            QByteArray &payUrl,//qr_code
            QString &errstring
            );
    //订单查询
    bool query(
            const QString &outTradeNo,//订单号
            const QString &appId,//接入标识
            const QString &merchantCode,//门店编号
            const QString &key,//密钥,

            QString &errstring);
    //扫码支付
    bool codepay(
            const QString &authCode, //支付授权码
            const QString &outTradeNo,//订单号
            const QString &appId,//接入标识
            const QString &merchantCode,//门店编号
            const QString &key,//密钥

            const QString &totalAmount,//0.01

            QString &payType,
            QString &errstring
            );
signals:
    void signal_code_query(qint64 cur, qint64 total, const QString &msg);
    void signal_code_query(qint64 cur, qint64 total);
private:
    static QString createSign(const QMap<QString, QString> &map, const QString &key);
    static QString mapJoin(const QMap<QString, QString> &map);
    static QString jsonTrimmd(cJSON *json);
    static QString getcJSONString(cJSON *item, const char *string);
    class cJSONPointer {
    public:
        cJSONPointer(cJSON *json) : json(json) {}
        ~cJSONPointer() {if(json) cJSON_Delete(json);}
        cJSON *json;
    };
};

#endif // LIANTUOFU_PAY_H
