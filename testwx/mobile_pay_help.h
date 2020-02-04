#ifndef MOBILE_PAY_HELP_H
#define MOBILE_PAY_HELP_H

#include <QVariant>
#include "cJSON.h"
#include <QStringList>
#include <QSharedPointer>
#include <memory>

/// 本支付、微信支付宝的金额都是整数、以分为单位
///  微信支付宝的 的 out_trade_no 作为退款用
///
/// errstring用于返回错误信息
///
/// paytype用于返回付款类型，例如扫码支付时，若是手机出示的是支付宝，则返回pay_zfb
/// 用于 codepay、wxOrderQuery、zfbOrderQuery
///
/// 扫码支付时，条码需要做清除空格操作

/// 微信支付：
/// 1.只需要获取【微信子商户号】即可，向售后申请
/// 支付宝支付：
/// *****************
/// 1.已有的账户、根据软件提供的【公钥】(getzfbpublic_key)重新设置【公钥】
/// *****************
/// 2.【appid】必填
/// *****************
/// 3.支付宝支付分RSA和RSA2，RSA2必须填公钥
/// *****************
/// 4.RSA的公钥从指定网址下载即可，RSA除了指定网址下载外，还需要从支付平台下载商户公钥，填入软件

/// 微信支付，两个参数，appid和公钥(RSA不填，RSA2必填)
/// 支付宝支付，两个参数，sub_mch_id和sub_appid(选填)
/// 退款：支付ok，微信退款测试用例会返回：特约子商户号未授权服务商的产品权限
/// 支付宝：软件需要功能：获取商户公钥的方法和添加服务商公钥的路径
/// 两种扫码支付，二维码支付都支持
class mobile_pay_help_sign_type{
public:
    mobile_pay_help_sign_type() {
        map.insert("RSA", "");
        map.insert("RSA2", "_new");
    }
    QStringList keys() {return map.keys();}
    QString value(const QString &key) {return map.value(key);}

    static const QString RSA;
    static const QString RSA2;
private:
    QMap<QString, QString> map;
};

class mobile_pay_help
{
public:
    enum payType {
        pay_none,
        pay_zfb,
        pay_wx
    };
public:
    mobile_pay_help();

    //测试微信部分
    static void test();
    //测试支付部分
    static void test1();
    static QStringList mobile_pay_alipay_keys();

    ////////////////////微信、支付宝
    //微信、支付宝，扫码支付
    static bool codepay(
            payType &pay_type,
            QString &errstring,

            const QString &sign_type,       //RSA\RSA2
            const QString &appid,           //支付宝appid
            const QString &sub_mch_id,      //微信子商户号
            const QString &auth_code,       //条形码
            const QString &total_amount,    //金额 单位为分

            const QString &out_trade_no,    //订单号
            const QString &body,            //商品描述
            const QString &kbstore_id, //口碑门店
            const QByteArray &new_alipay_public_key = "",            //商户支付宝公钥,RSA2必填

            const QString &sub_appid = "",  //微信子appid 选填 ，子商户有公众账 号时填写
            const QString &device_info = ""//设备信息
            );
    ////////////////////微信
    //微信、二维码支付
    static bool getwxpayurl(
            QByteArray &code_url,
            QString &errstring,

            const QString &sub_mch_id,      //微信子商户号
            const QString &total_amount,    //金额 单位为分
            const QString &out_trade_no,    //订单号
            const QString &body,            //商品描述

            const QString &sub_appid = "",  //微信子appid
            const QString &device_info = "" //设备信息
            );

    //微信、订单查询
    static bool wxOrderQuery(
            QString &errstring,

            const QString &sub_mch_id,      //微信子商户号
            const QString &out_trade_no,    //订单号
            const QString &sub_appid = ""   //微信子appid
            );

    //微信、退款
    static bool wxrefund(
            QString &errstring,

            const QString &out_trade_no,    //商户系统内部的订单号
            const QString &total_fee,      //订单总金额,单位为分,只能为整数
            const QString &refund_fee,      //退款总金额,单位为分,只能为整数,可部分退款
            const QString &sub_mch_id,    //微信支付分配的子商户号
            const QString &sub_appid = ""       //微信分配的子商户公众账号ID
            );


    ////////////////////支付宝
    //支付宝、获取支付url
    static bool getzfbpayurl(
            QByteArray &code_url,
            QString &errstring,

            const QString &sign_type,
            const QString &appid,           //支付宝appid
            const QString &total_amount,    //金额 单位为分
            const QString &out_trade_no,    //订单号
            const QString &body,            //商品描述

            const QString &kbstore_id, //口碑门店

            const QByteArray &new_alipay_public_key = "",            //商户支付宝公钥,RSA2必填
            const QString &device_info = "" //设备信息
            );

    //支付宝、订单查询
    static bool zfbOrderQuery(
            QString &errstring,

            const QString &sign_type,
            const QString &appid,           //支付宝appid
            const QString &out_trade_no,    //订单号
            const QByteArray &new_alipay_public_key = ""            //商户支付宝公钥,RSA2必填
            );

    //支付宝、获取公钥
    static bool getzfbpublic_key(
            QByteArray &ret_rsa_public_key,
            QString &errstring,

            const QString &sign_type);
    //支付宝、退款
    static bool zfbrefund(
            QString &errstring,

            const QString &sign_type,
            const QString &trade_no,       //支付宝系统交易号
            const QString &appid,          //支付宝appid
            const QString &refund_reason, //退单原因
            const QString &refund_amount,   //退款金额
            const QByteArray &new_alipay_public_key = ""            //商户支付宝公钥,RSA2必填
            );

private:
    static bool do_post(const QString &url, const QByteArray &content, QByteArray&responseData, QString *errstring);
    static bool do_get(const QString &url, QByteArray&responseData, QString *errstring);

    static QString get_error(const QByteArray &message,
                             const QByteArray &data,
                             const char *json_zfb_string,
                             const char *json_wx_string);
    static QString get_error_zfb(const QByteArray &message, const QByteArray &data, const char *json_string);
    static QString get_error_wx(const QByteArray &message, const QByteArray &data, const char *json_string);

    ///cjson
    static QVariant getcJSONvalue(cJSON *root, const char *string);
    static QVariant getcJSONvalue(const QByteArray &d, const char *string);
    static QVariant getcJSONvalue(QSharedPointer<cJSON> d, const char *string);
    static QVariant getItemValue(cJSON *item);
    ///~cjson
    static mobile_pay_help_sign_type sign_type_map;
};

#endif // MOBILE_PAY_HELP_H
