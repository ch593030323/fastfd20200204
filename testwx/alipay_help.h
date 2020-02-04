#ifndef ALIPAY_HELP_H
#define ALIPAY_HELP_H

#include <QObject>
#include <QMap>
#include <QUdpSocket>
#include "cJSON.h"

//http://app.alipay.com/market/document.htm?name=saomazhifu#page-11
//2016-10-20
//https://doc.open.alipay.com/docs/api.htm?docType=4&apiId=850

#define ALIPAY_COMMIT_DIRECT(arg0, content, data, retvalue, arg2) (alipay_sign(arg0, content, data) && alipay_post(data, retvalue) && resolve_data(retvalue, arg2))
#define ALIPAY_COMMIT_SHA256(arg0, content, data, retvalue, arg2) (content.insert("sign_type", sign_type_SHA256).value() == sign_type_SHA256 && ALIPAY_COMMIT_DIRECT(arg0, content, data, retvalue, arg2) &&(arg2.sign_type_not_null  = content.value("sign_type")) == sign_type_SHA256)
#define ALIPAY_COMMIT_SHA1(arg0, content, data, retvalue, arg2) (content.insert("sign_type", sign_type_SHA1).value() == sign_type_SHA1 && ALIPAY_COMMIT_DIRECT(arg0, content, data, retvalue, arg2) &&(arg2.sign_type_not_null  = content.value("sign_type")) == sign_type_SHA1)
#define ALIPAY_COMMIT_SHA(arg0, content, data, retvalue, arg2)  (ALIPAY_COMMIT_SHA256(arg0, content, data, retvalue, arg2) || ALIPAY_COMMIT_SHA1(arg0, content, data, retvalue, arg2))
#define ALIPAY_COMMIT(arg0, content, data, retvalue, arg2) ( (content.value("sign_type") == sign_type_SHA256 || content.value("sign_type") == sign_type_SHA1) && (ALIPAY_COMMIT_DIRECT(arg0, content, data, retvalue, arg2)) ? true : ALIPAY_COMMIT_SHA(arg0, content, data, retvalue, arg2) )


class alipay_help : public QObject
{
    Q_OBJECT
public:
    struct datagaramDATA{
        int index;
        QByteArray content0;
        QByteArray content1;
        bool issuccess;
        QString error;
    };
    static const QString sign_type_SHA1;
    static const QString sign_type_SHA256;

public:
    struct alipay_trade_fund_bill{
        inline alipay_trade_fund_bill(const QString &_f, const QString &_a)
            :fund_channel(_f), amount(_a) {}
        QString fund_channel;//支付渠道
        QString amount ;//支付金额
    };
    //公共请求参数
    struct alipay_public_request_argument{
        inline alipay_public_request_argument():
            sign_not_null("anyvalue"),
            biz_content_not_null("anyvalue")
        {}
        QString app_id_not_null;//开发者的AppId
//        QString method_not_null;//口名称          == anyvalue 在请求函数中会赋值
        QString charset_not_null;//接参数字符编号
        QString sign_type;//签名类型,sign_type_SHA256,sign_type_SHA1, 为空时，会先判断sign_type_SHA256， 再判断sign_type_SHA1
        QString sign_not_null;//签名 == anyvalue

        QString notify_url;// 	接口异步通知url
        QString timestamp_not_null;//时间戳 yyyy-MM-dd hh:mm:ss
        QString version_not_null;// 	接口版本号
        QString biz_content_not_null;//业务参数 == anyvalue
        QByteArray rsa_private_key_data_not_null;
    };
    static bool check(const alipay_public_request_argument &d);

    //公共返回参数
    struct alipay_public_response_argument{
        QString sign;//签名
        QString code_not_null;//结果码
        QString msg_not_null;//结果码描述
        QString sub_code;//错误子代码
        QString sub_msg;//错误子代码描述
    };
    //商品明细说明
    struct alipay_goods_detail_argument{
        QString goods_id_not_null;// 	商品编号
        QString alipay_goods_id;//支付宝统一的商品编号
        QString goods_name_not_null;//商品名称
        QString quantity_not_null;//商品数量
        QString price_not_null;// 	商品单价

        QString goods_category;// 	商品类目
        QString body;//商品描述
        QString  show_url;// 	商品的展示地址
    };
    static bool check(const alipay_goods_detail_argument &d);
    //业务扩展参数
    struct alipay_trade_extend_params_argument {
        QString sys_service_provider_id;//系统商编号 该参数作为系统商返佣数据提取的依据，请填写系统商签约协议的PID
        QString hb_fq_num;//使用花呗分期要进行的分期数
        QString hb_fq_seller_percent;//使用花呗分期需要卖家承担的手续费比例的百分值，传入100代表100%
    };
    //预下单请求参数
    struct alipay_trade_precreate_argument{
        QString out_trade_no_not_null;// 	商户订单号
        QString seller_id;// 卖家支付宝用户 ID
        QString total_amount_not_null;// 	订单总金额
        QString discountable_amount;// 	可打折金额
        QString undiscountable_amount;// 	不可打折金额

        QString subject_not_null;// 	订单标题
        QString body;// 	订单描述
        QList<alipay_goods_detail_argument> goods_detail;// 	商品明细列表
        QString operator_id;// 	商户操作员编号
        QString store_id;// 	商户门店编号

        QString terminal_id;// 	机具终端编号
        alipay_trade_extend_params_argument extend_params;// 	扩展参数
        QString time_expire;// 	支付超时时间
    };
    static bool check(const alipay_trade_precreate_argument &d);
    //预下单请求返回参数
    struct alipay_trade_precreate_return_argument{
        alipay_public_response_argument public_response;//公共返回参数
        QString out_trade_no_not_null;
        QString qr_code_not_null;
        QString sign_type_not_null;//签名方式，由软件生成

    };

    //订单查询参数
    //两个参数不能同时为空
    //订单支付时传入的商户订单号。trade_no,out_trade_no 如果同时存在优先取 trade_no
    struct alipay_trade_query_argument{
        inline alipay_trade_query_argument(const QString &t="", const QString &o=""):
            trade_no(t), out_trade_no(o)
        {}
        QString trade_no;
        QString out_trade_no;
    };
    static bool check(const alipay_trade_query_argument &d);

    //订单查询参数返回参数
    struct alipay_trade_query_return_argument{
        alipay_public_response_argument public_response;//公共返回参数
        QString trade_no_not_null;//支付宝交易号
        QString out_trade_no_not_null;// 	商户订单号
        QString open_id_not_null;//买家支付宝用户号
        QString buyer_logon_id_not_null;// 	买家支付宝账号
        QString trade_status_not_null;//交易状态

        QString total_amount_not_null;//订单金额
        QString receipt_amount;// 	实收金额
        QString invoice_amount;// 	开票金额
        QString buyer_pay_amount;//	付款金额
        QString point_amount;//积分宝金额

        QString send_pay_dateterminal_id;//本次交易打款给卖家的时间
        QString terminal_id;//商户机具终端编号
        QString alipay_store_id;//支付宝店铺编号
        QString store_id;//商户门店编号
        QList<alipay_trade_fund_bill> fund_bill_list;// 	资金单据信息的集合
        QString sign_type_not_null;//签名方式，由软件生成

    };

    ////////////////////////////////////////////////////////////////////
    //条码支付请求
    struct alipay_trade_pay_argument{
        inline alipay_trade_pay_argument():scene_not_null("bar_code") {}
        QString out_trade_no_not_null;// 	商户订单号
        QString scene_not_null;// 	支付场景 bar_code
        QString auth_code_not_null;// 	支付授权码 付款码xinxi
        QString seller_id;// 卖家支付宝用户 ID
        QString total_amount_not_null;// 	订单总金额

        QString discountable_amount;// 	可打折金额
        QString undiscountable_amount;// 	不可打折金额
        QString subject_not_null;// 	订单标题
        QString body;// 	订单描述
        QList<alipay_goods_detail_argument> goods_detail;// 	商品明细列表
        alipay_trade_extend_params_argument extend_params;// 	扩展参数

        QString operator_id;// 	商户操作员编号
        QString store_id;// 	商户门店编号
        QString terminal_id;// 	机具终端编号
        QString time_expire;// 	支付超时时间

        QString royalty_info;// 	分账信息
    };
    static bool check(const alipay_trade_pay_argument &d);

    struct alipay_trade_pay_return_argument{//条码支付成功
        alipay_public_response_argument public_response;//公共返回参数

        QString trade_no_not_null;// 	支付宝交易号
        QString out_trade_no_not_null;// 	商户订单号
        QString open_id_not_null;// 	买家支付宝用户号
        QString buyer_logon_id_not_null;// 买家支付宝账号
        QString total_amount_not_null;// 	交易金额

        QString receipt_amount_not_null;// 	实收金额
        QString invoice_amount_not_null;// 	开票金额
        QString buyer_pay_amount_not_null;// 	付款金额
        QString point_amount_not_null;// 	积分宝金额
        QString gmt_payment_not_null;// 	付款时间

        QList<alipay_trade_fund_bill> fund_bill_list_not_null;// 	交易资金明细信息集合
        QString store_name;// 	门店名称
        QString sign_type_not_null;//签名方式，由软件生成
    };

    ///////////////////////////////////////////////////////////////////
    struct alipay_trade_cancel_argument{
        QString out_trade_no_not_null;// 	支付宝交易号
    };
    static bool check(const alipay_trade_cancel_argument &d);

    struct alipay_trade_cancel_return_argument{
        alipay_public_response_argument public_response;//公共返回参数

        QString trade_no_not_null;// 	支付宝交易号
        QString out_trade_no_not_null;// 	商户订单号
        QString retry_flag_no_null;// 	是否可重试标志
        QString action;// 	执行撤销动作
        QString sign_type_not_null;//签名方式，由软件生成

    };
    ///////////////////////////////////////////////////////////////////

    struct alipay_trade_refund_detail_item{
        inline alipay_trade_refund_detail_item(const QString &_f, const QString &_a)
            :fund_channel(_f), amount(_a) {}
        QString fund_channel;//支付渠道
        QString amount ;//支付金额
    };
    struct alipay_trade_refund_argument{
        QString trade_no_not_null;// 	支付宝交易号
        QString refund_amount_not_null;// 	退款金额
        QString out_request_no;// 	商户退款请求号
        QString refund_reason;// 	退款原因
        QString store_id;// 	商户的门店编号

        QString terminal_id;// 	商户的终端编号
    };
    static bool check(const alipay_trade_refund_argument &d);

    struct alipay_trade_refund_return_argument{
        alipay_public_response_argument public_response;//公共返回参数

        QString trade_no_not_null;// 	支付宝交易号
        QString out_trade_no_not_null;// 	商户订单号
        QString open_id;// 	是否可重试标志
        QString buyer_logon_d;// 买家支付宝账号
        QString fund_change_not_null;// 买家支付宝账号

        QString refund_fee;// 买家支付宝账号
        QString gmt_refund_pay;// 买家支付宝账号
        QList<alipay_trade_refund_detail_item> refund_detail_item_list_not_null;// 买家支付宝账号
        QString store_name;// 买家支付宝账号
        QString sign_type_not_null;//签名方式，由软件生成
    };

public:
    static QString errstring;//错误信息
public:
    explicit alipay_help(QObject *parent = 0);

    static QByteArray alipay_private_key_add_start_line(const QString &filepath, bool *ok);
    static void alipay_private_key_add_start_line(QByteArray &private_key_data);
    static void alipay_test();

    //条码 支付
    static bool alipay_trade_pay(const alipay_public_request_argument &arg0, const alipay_trade_pay_argument &arg1, alipay_trade_pay_return_argument &arg2);
    static bool resolve_data(const QByteArray &data, alipay_trade_pay_return_argument &retvalue);

    //扫码 支付
    static bool alipay_trade_precreate(const alipay_public_request_argument &arg0, const alipay_trade_precreate_argument &arg1, alipay_trade_precreate_return_argument &arg2);//预下单请求
    static bool resolve_data(const QByteArray &data, alipay_trade_precreate_return_argument &retvalue);

    //订单 查询
    static bool alipay_trade_query(const alipay_public_request_argument &arg0, const alipay_trade_query_argument &arg1, alipay_trade_query_return_argument &arg2);//查询订单
    static bool resolve_data(const QByteArray &data, alipay_trade_query_return_argument &retvalue);

    static bool alipay_trade_cancel();//撤销订单

    //订单 退货
    static bool alipay_trade_refund(const alipay_public_request_argument &arg0, const alipay_trade_refund_argument &arg1, alipay_trade_refund_return_argument &arg2);//申请退款
    static bool resolve_data(const QByteArray &data, alipay_trade_refund_return_argument &retvalue);

private:
    static bool getsignature_bypkey(const QString &sign_type, const QByteArray & rsa_private_key_data, const QByteArray &preparedata, QByteArray &retvalue);

    static bool alipay_post(const QByteArray &data,QByteArray &retvalue);
    static bool generate_RSA_key();//在当前路径生成私钥和公钥

    //字典排序后，以&连接字段
    static bool alipay_sign(
            const alipay_public_request_argument &arg0,
            QMap<QString, QString> &m/*待上传的map*/,
            QByteArray &retvalue);

    static QVariant getcJSONvalue(cJSON *root, const char *string);

};

#endif // ALIPAY_HELP_H
