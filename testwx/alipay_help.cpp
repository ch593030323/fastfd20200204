#include "alipay_help.h"
#include "cJSON.h"
#include <QtDebug>
#include <QSslKey>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QNetworkReply>
#include <QTimer>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/rand.h>
#include <openssl/objects.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <QCoreApplication>
#include <QSslConfiguration>
#include <QtDebug>
#include <QHostAddress>
#include <QStringList>
#include <QTextCodec>
#include <QDataStream>
#include <QNetworkInterface>
#include <stdio.h>

const QString alipay_help::sign_type_SHA1 = "RSA";
const QString alipay_help::sign_type_SHA256 = "RSA2";

QString alipay_help::errstring;
static QString trans_GBK2UFT8(char *data)
{
    QTextCodec *c = QTextCodec::codecForName("GBK");//GBK->uft-8
    return c->toUnicode(data);
}

bool generate_key()
{
    int             ret = 0;
    RSA             *r = NULL;
    BIGNUM          *bne = NULL;
    BIO             *bp_public = NULL, *bp_private = NULL;

    int             bits = 2048;
    unsigned long   e = RSA_F4;

    // 1. generate rsa key
    bne = BN_new();
    ret = BN_set_word(bne,e);
    if(ret != 1) {
        goto free_all;
    }

    r = RSA_new();
    ret = RSA_generate_key_ex(r, bits, bne, NULL);
    if(ret != 1) {
        goto free_all;
    }
    // 2. save public key
    bp_public = BIO_new_file("rsa_public_key.pem", "w+");
    ret = PEM_write_bio_RSA_PUBKEY(bp_public, r);
    if(ret != 1) {
        goto free_all;
    }

    // 3. save rsa private key
    bp_private = BIO_new_file("rsa_private_key.pem", "w+");
    ret = PEM_write_bio_RSAPrivateKey(bp_private, r, NULL, NULL, 0, NULL, NULL);

    // 4. free
free_all:

    BIO_free_all(bp_public);
    BIO_free_all(bp_private);
    RSA_free(r);
    BN_free(bne);

    return (ret == 1);
}
bool alipay_help::check(const alipay_help::alipay_public_request_argument &d)
{
    errstring.clear();
    if(d.app_id_not_null.isEmpty()) { errstring = "app_id_not_null"; return false; }
    if(d.charset_not_null.isEmpty()) { errstring = "charset_not_null"; return false; }
    if(d.sign_not_null.isEmpty()) { errstring = "sign_not_null"; return false; }

    if(d.timestamp_not_null.isEmpty()) { errstring = "timestamp_not_null"; return false; }
    if(d.version_not_null.isEmpty()) { errstring = "version_not_null"; return false; }
    if(d.biz_content_not_null.isEmpty()) { errstring = "biz_content_not_null"; return false; }
    if(d.rsa_private_key_data_not_null.isEmpty()) { errstring = "rsa_private_key_data_not_null"; return false; }
    return true;
}

bool alipay_help::check(const alipay_help::alipay_goods_detail_argument &d)
{
    errstring.clear();
    if(d.goods_id_not_null.isEmpty()) { errstring = "goods_id_not_null"; return false; }
    if(d.goods_name_not_null.isEmpty()) { errstring = "goods_name_not_null"; return false; }
    if(d.quantity_not_null.isEmpty()) { errstring = "quantity_not_null"; return false; }
    if(d.price_not_null.isEmpty()) { errstring = "price_not_null"; return false; }
    return true;
}

bool alipay_help::check(const alipay_help::alipay_trade_precreate_argument &d)
{
    errstring.clear();
    if(d.out_trade_no_not_null.isEmpty()) { errstring = "out_trade_no_not_null"; return false; }
    if(d.total_amount_not_null.isEmpty()) { errstring = "total_amount_not_null"; return false; }
    if(d.subject_not_null.isEmpty()) { errstring = "subject_not_null"; return false; }
    return true;
}

bool alipay_help::check(const alipay_help::alipay_trade_query_argument &d)
{
    errstring.clear();
    if(d.trade_no.isEmpty()
            &&d.out_trade_no.isEmpty()) {
        errstring = "trade_no and out_trade_no all are empty";
        return false;
    }
    return true;

}

bool alipay_help::check(const alipay_help::alipay_trade_pay_argument &d)
{
    errstring.clear();
    if(d.out_trade_no_not_null.isEmpty()) { errstring = "out_trade_no_not_null"; return false; }
    if(d.scene_not_null.isEmpty()) { errstring = "scene_not_null"; return false; }
    if(d.auth_code_not_null.isEmpty()) { errstring = "auth_code_not_null"; return false; }
    if(d.total_amount_not_null.isEmpty()) { errstring = "total_amount_not_null"; return false; }
    if(d.subject_not_null.isEmpty()) { errstring = "subject_not_null"; return false; }
    return true;
}

bool alipay_help::check(const alipay_help::alipay_trade_cancel_argument &d)
{
    errstring.clear();
    if(d.out_trade_no_not_null.isEmpty()) { errstring = "out_trade_no_not_null"; return false; }
    return true;
}

bool alipay_help::check(const alipay_help::alipay_trade_refund_argument &d)
{
    errstring.clear();
    if(d.refund_amount_not_null.isEmpty()) { errstring = "refund_amount_not_null"; return false; }
    if(d.trade_no_not_null.isEmpty()) { errstring = "trade_no_not_null"; return false; }
    return true;
}

bool alipay_help::resolve_data(const QByteArray &data, alipay_help::alipay_trade_precreate_return_argument &retvalue)
{
    /*
"{"alipay_trade_precreate_response":{"code":"40002","msg":"Invalid Arguments","sub_code":"isv.invalid-signature","sub_msg":"��Чǩ��"},"sign":"pKAZjddvi+mJDIJnopTjVuwG3yoNc8JKW6HvjZ9v5GQ551NAhuIIJjL1cvAm6Llxxbjm9bYRNWRR0LJsXLaxYKzpymJNOZ0WcZtqcHmTaBzdII/G5boGLQaSl347pywft04Vb/0oeKBuEekqzPXQIma+iBXbK9GP0i5qghxTGHg="}" */
    cJSON *root = cJSON_Parse(data.data());
    cJSON *item = cJSON_GetObjectItem(root,"alipay_trade_precreate_response");
    bool flag;
    QString info;
    errstring.clear();

    flag = true;
    retvalue.public_response.code_not_null = getcJSONvalue(item, "code").toString();
    retvalue.public_response.msg_not_null = getcJSONvalue(item, "msg").toString();
    retvalue.public_response.sign = getcJSONvalue(item, "sign").toString();
    retvalue.public_response.sub_code = getcJSONvalue(item, "sub_code").toString();
    retvalue.public_response.sub_msg = getcJSONvalue(item, "sub_msg").toString();

    retvalue.out_trade_no_not_null = getcJSONvalue(item, "out_trade_no").toString();
    retvalue.qr_code_not_null = getcJSONvalue(item, "qr_code").toString();

    if(retvalue.out_trade_no_not_null.isEmpty()
            ||retvalue.qr_code_not_null.isEmpty()) {
        info =
                "code:" + getcJSONvalue(item, "code").toString() +"\n" +
                "sub_code:" + getcJSONvalue(item, "sub_code").toString() +"\n" +
                "sub_msg:" + getcJSONvalue(item, "sub_msg").toString() ;
        flag = false;
    }
    cJSON_Delete(root);
    if(!flag) {
        errstring = info;
        return false;
    }
    return true;
}

bool alipay_help::resolve_data(const QByteArray &data, alipay_help::alipay_trade_query_return_argument &retvalue)
{

    cJSON *root = cJSON_Parse(data.data());
    cJSON *item = cJSON_GetObjectItem(root,"alipay_trade_query_response");
    bool flag;
    QString info;
    errstring.clear();

    flag = true;
    retvalue.public_response.code_not_null = getcJSONvalue(item, "code").toString();
    retvalue.public_response.msg_not_null = getcJSONvalue(item, "msg").toString();
    retvalue.public_response.sign = getcJSONvalue(item, "sign").toString();
    retvalue.public_response.sub_code = getcJSONvalue(item, "sub_code").toString();
    retvalue.public_response.sub_msg = getcJSONvalue(item, "sub_msg").toString();

    retvalue.alipay_store_id = getcJSONvalue(item, "alipay_store_id").toString();
    retvalue.buyer_logon_id_not_null = getcJSONvalue(item, "buyer_logon_id").toString();
    retvalue.buyer_pay_amount = getcJSONvalue(item, "buyer_pay_amount").toString();
    {
        cJSON *fund_bill_list = cJSON_GetObjectItem(item, "fund_bill_list");
        if(fund_bill_list) {
            for(int index = 0, indexcount = cJSON_GetArraySize(fund_bill_list); index < indexcount; index++) {
                cJSON *fund_bill = cJSON_GetArrayItem(fund_bill_list,index);
                retvalue.fund_bill_list.append(alipay_trade_fund_bill(getcJSONvalue(fund_bill, "fund_channel").toString(),
                                                                      getcJSONvalue(fund_bill, "amount").toString())
                                               );
            }
        }
    }
    retvalue.invoice_amount = getcJSONvalue(item, "invoice_amount").toString();

    retvalue.open_id_not_null = getcJSONvalue(item, "open_id").toString();
    retvalue.out_trade_no_not_null = getcJSONvalue(item, "out_trade_no").toString();
    retvalue.point_amount = getcJSONvalue(item, "point_amount").toString();
    retvalue.receipt_amount = getcJSONvalue(item, "receipt_amount").toString();
    retvalue.send_pay_dateterminal_id = getcJSONvalue(item, "send_pay_dateterminal_id").toString();

    retvalue.store_id = getcJSONvalue(item, "store_id").toString();
    retvalue.terminal_id = getcJSONvalue(item, "terminal_id").toString();
    retvalue.total_amount_not_null = getcJSONvalue(item, "total_amount").toString();
    retvalue.trade_no_not_null = getcJSONvalue(item, "trade_no").toString();
    retvalue.trade_status_not_null = getcJSONvalue(item, "trade_status").toString();

    if(retvalue.trade_no_not_null.isEmpty()
            ||retvalue.trade_status_not_null.trimmed() != "TRADE_SUCCESS") {
        info = "error code:"+getcJSONvalue(item, "code").toString();

        //获取错误描述
        cJSON *item0 = cJSON_GetObjectItem(item,"sub_msg");
        if(item0) {

            info =  trans_GBK2UFT8(item0->valuestring);
        }
        //
        flag = false;
    }
    cJSON_Delete(root);

    if(!flag) {
        errstring = info;
        return false;
    }
    return true;
}

bool alipay_help::resolve_data(const QByteArray &data, alipay_help::alipay_trade_pay_return_argument &retvalue)
{
    cJSON *root = cJSON_Parse(data.data());
    cJSON *item = cJSON_GetObjectItem(root,"alipay_trade_pay_response");
    bool flag;
    QString info;
    errstring.clear();

    flag = true;
    retvalue.public_response.code_not_null = getcJSONvalue(item, "code").toString();
    retvalue.public_response.msg_not_null = getcJSONvalue(item, "msg").toString();
    retvalue.public_response.sign = getcJSONvalue(item, "sign").toString();
    retvalue.public_response.sub_code = getcJSONvalue(item, "sub_code").toString();
    retvalue.public_response.sub_msg = getcJSONvalue(item, "sub_msg").toString();

    retvalue.buyer_logon_id_not_null = getcJSONvalue(item, "buyer_logon_id").toString();
    retvalue.buyer_pay_amount_not_null = getcJSONvalue(item, "buyer_pay_amount").toString();
    {
        cJSON *fund_bill_list = cJSON_GetObjectItem(item, "fund_bill_list");
        if(fund_bill_list) {
            for(int index = 0, indexcount = cJSON_GetArraySize(fund_bill_list); index < indexcount; index++) {
                cJSON *fund_bill = cJSON_GetArrayItem(fund_bill_list,index);
                retvalue.fund_bill_list_not_null.append(alipay_trade_fund_bill(getcJSONvalue(fund_bill, "fund_channel").toString(),
                                                                               getcJSONvalue(fund_bill, "amount").toString())
                                                        );
            }
        }
    }
    retvalue.gmt_payment_not_null = getcJSONvalue(item, "gmt_payment").toString();
    retvalue.invoice_amount_not_null = getcJSONvalue(item, "invoice_amount").toString();

    retvalue.open_id_not_null = getcJSONvalue(item, "open_id").toString();
    retvalue.out_trade_no_not_null = getcJSONvalue(item, "out_trade_no").toString();
    retvalue.point_amount_not_null = getcJSONvalue(item, "point_amount").toString();
    retvalue.receipt_amount_not_null = getcJSONvalue(item, "receipt_amount").toString();
    retvalue.store_name = getcJSONvalue(item, "store_name").toString();

    retvalue.total_amount_not_null = getcJSONvalue(item, "total_amount").toString();
    retvalue.trade_no_not_null = getcJSONvalue(item, "trade_no").toString();

    if(retvalue.public_response.code_not_null != "10000") {
        info = "error code:"+retvalue.public_response.code_not_null;

        //获取错误描述
        cJSON *item0 = cJSON_GetObjectItem(item,"sub_msg");
        if(item0) {

            info =  trans_GBK2UFT8(item0->valuestring);
        }
        //
        flag = false;
    }
    cJSON_Delete(root);

    if(!flag) {
        errstring = info;
        return false;
    }
    return true;
}

bool alipay_help::resolve_data(const QByteArray &data, alipay_help::alipay_trade_refund_return_argument &retvalue)
{
    cJSON *root = cJSON_Parse(data.data());
    cJSON *item = cJSON_GetObjectItem(root,"alipay_trade_refund_response");
    bool flag;
    QString info;
    errstring.clear();

    flag = true;
    retvalue.public_response.code_not_null = getcJSONvalue(item, "code").toString();
    retvalue.public_response.msg_not_null = getcJSONvalue(item, "msg").toString();
    retvalue.public_response.sign = getcJSONvalue(item, "sign").toString();
    retvalue.public_response.sub_code = getcJSONvalue(item, "sub_code").toString();
    retvalue.public_response.sub_msg = getcJSONvalue(item, "sub_msg").toString();

    retvalue.buyer_logon_d = getcJSONvalue(item, "buyer_logon_d").toString();
    retvalue.fund_change_not_null = getcJSONvalue(item, "fund_change").toString();
    retvalue.gmt_refund_pay = getcJSONvalue(item, "gmt_refund_pay").toString();
    retvalue.open_id = getcJSONvalue(item, "open_id").toString();
    retvalue.out_trade_no_not_null = getcJSONvalue(item, "out_trade_no").toString();

    {
        cJSON *refund_detail_item_list = cJSON_GetObjectItem(item, "refund_detail_item_list");
        if(refund_detail_item_list) {
            for(int index = 0, indexcount = cJSON_GetArraySize(refund_detail_item_list); index < indexcount; index++) {
                cJSON *refund_detail_item = cJSON_GetArrayItem(refund_detail_item_list,index);
                retvalue.refund_detail_item_list_not_null.append(alipay_trade_refund_detail_item(getcJSONvalue(refund_detail_item, "fund_channel").toString(),
                                                                                                 getcJSONvalue(refund_detail_item, "amount").toString())
                                                                 );
            }
        }
    }
    retvalue.refund_fee = getcJSONvalue(item, "refund_fee").toString();
    retvalue.store_name = getcJSONvalue(item, "store_name").toString();
    retvalue.trade_no_not_null = getcJSONvalue(item, "trade_no").toString();

    if(retvalue.public_response.code_not_null != "10000") {
        info = "error code:"+retvalue.public_response.code_not_null;

        //获取错误描述
        cJSON *item0 = cJSON_GetObjectItem(item,"sub_msg");
        if(item0) {

            info =  trans_GBK2UFT8(item0->valuestring);
        }
        //
        flag = false;
    }
    cJSON_Delete(root);

    if(!flag) {
        errstring = info;
        return false;
    }
    return true;
}

bool alipay_help::getsignature_bypkey(const QString &sign_type, const QByteArray &rsa_private_key_data, const QByteArray &preparedata, QByteArray &retvalue)
{
    qDebug() << preparedata << sign_type;
    errstring.clear();
    //获得摘要字符串
    QByteArray sigret;
    if(sign_type == sign_type_SHA1) {
        sigret.resize(20);
        SHA1((unsigned char*)preparedata.data(), preparedata.length(), (unsigned char*)sigret.data());
    }
    if(sign_type == sign_type_SHA256) {
        sigret.resize(32);
        SHA256((unsigned char*)preparedata.data(), preparedata.length(), (unsigned char*)sigret.data());
    }
    //~获得摘要字符串

    //获取私钥
    RSA *rsa = NULL;
    BIO* memBIO = NULL;
    memBIO = BIO_new(BIO_s_mem());
    int bioWriteLen = BIO_write(memBIO, rsa_private_key_data.data(), rsa_private_key_data.length());
    rsa =PEM_read_bio_RSAPrivateKey(memBIO, NULL, NULL, NULL); //PEM_read_bio_RSA_PUBKEY(memBIO, NULL, NULL, NULL);
    if (rsa == NULL) {
        errstring = "rsa is null";

        QByteArray d;
        d.resize(256);
        d.fill(0);
        BIO *err = BIO_new(BIO_s_mem());
        ERR_print_errors(err);
        BIO_read(err, d.data(), d.length());
        delete err;
        err = 0;

        errstring = d;
        return false;
    }
    //～获取私钥

    //签名
    unsigned int sigLen = 0;
    unsigned char *sigBuff =(unsigned char *) malloc(RSA_size(rsa));
    int result;
    if(sign_type == sign_type_SHA1) {
        result = RSA_sign(NID_sha1, (unsigned char*)sigret.data(),sigret.length(), sigBuff, &sigLen, rsa);
    }
    if(sign_type == sign_type_SHA256) {
        result = RSA_sign(NID_sha256, (unsigned char*)sigret.data(), sigret.length(), sigBuff, &sigLen, rsa);
    }
    retvalue = QByteArray((char *)sigBuff, sigLen).toBase64();
    //～签名
    return true;
}

bool alipay_help::alipay_post(const QByteArray &data,QByteArray &retvalue)
{
    qDebug() << "data" <<  data;
    errstring.clear();
    QNetworkAccessManager manager;
    QNetworkRequest req;
    QEventLoop eloop;
    QTimer timer;

    req.setUrl(QUrl("https://openapi.alipay.com/gateway.do?"));
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded;charset=utf-8");
    req.setHeader(QNetworkRequest::ContentLengthHeader,data.length());
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#else
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1);
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    req.setSslConfiguration(config);
#endif
    timer.setSingleShot(true);
    connect(&timer,SIGNAL(timeout()),&eloop,SLOT(quit()));
    connect(&manager,SIGNAL(finished(QNetworkReply*)),&eloop,SLOT(quit()));//89774293
    QNetworkReply *reply=manager.post(req,data);
    timer.start(10*1000);//10s的超时
    eloop.exec(QEventLoop::ExcludeUserInputEvents);

    if(!timer.isActive()) {
        errstring =  "timeout";
        return false;
    }
    if(reply->error()!=QNetworkReply::NoError) {
        errstring = reply->errorString();
        return false;
    }
    retvalue = reply->readAll();
    return true;
}

bool alipay_help::generate_RSA_key()
{
    errstring.clear();
    if(!generate_key()) {
        errstring = "generate_key failure!";
        return false;
    }
    return true;
}

bool alipay_help::alipay_sign(const alipay_help::alipay_public_request_argument &arg0, QMap<QString, QString> &m, QByteArray &retvalue)
{
    //参与签名的数据
    QByteArray step0;
    QByteArray signature;
    for(QMap<QString, QString>::const_iterator mitr = m.begin();
        mitr != m.end();
        mitr++) {
        //sign不参与签名
        if(mitr.key() == ("sign")) continue;
        QString t=mitr.value();
        step0 += mitr.key()+"="+t.simplified()+"&";
    }
    if(!step0.isEmpty()) {
        step0.chop(1);//去除最后&
    }
    if(!getsignature_bypkey(m.value("sign_type"), arg0.rsa_private_key_data_not_null, step0, signature)) {
        return false;
    }
    //通过服务器来签名
    m["sign"] = signature;
    //rsa 和base64 和url encoding 之后生成上传数据
    step0.clear();
    for(QMap<QString, QString>::const_iterator mitr = m.begin();
        mitr != m.end();
        mitr++) {
        if(!step0.isEmpty()) step0 += '&';
        step0 += mitr.key()+"="+mitr.value().simplified().toLocal8Bit().toPercentEncoding();
    }
    retvalue = step0;
    return true;
}

QVariant alipay_help::getcJSONvalue(cJSON *root, const char *string)
{
    QVariant ret;
    cJSON *item = cJSON_GetObjectItem(root,string);
    if(!item) {
        qDebug("alipay_help:json return item is null,  please check ");
        return ret;
    }
    if(item) {
        if(item->type == cJSON_String) {
            ret = item->valuestring;
        } else if(item->type == cJSON_Number) {
            if(item->valuedouble != double(item->valueint)) {
                ret = item->valuedouble;
            } else {
                ret = item->valueint;
            }
        }
    }
    return ret;
}

alipay_help::alipay_help(QObject *parent) :
    QObject(parent)
{
}

QByteArray alipay_help::alipay_private_key_add_start_line(const QString &filepath, bool *ok)
{
    if(ok) *ok = true;
    QByteArray retd;
    QFile file(filepath);
    if(false == file.open(QFile::ReadOnly)) {
        if(ok) *ok = false;
        return retd;
    }
    retd = file.readAll();
    alipay_private_key_add_start_line(retd);

    return retd;
}



void alipay_help::alipay_private_key_add_start_line(QByteArray &private_key_data)
{
    if(private_key_data.startsWith("-----BEGIN RSA PRIVATE KEY-----")) {
        qDebug() << "private_key_data already has start line";
        return;
    }
    if(private_key_data.startsWith("-----BEGIN PRIVATE KEY-----")) {
        private_key_data.replace("-----BEGIN PRIVATE KEY-----", "-----BEGIN RSA PRIVATE KEY-----");
        private_key_data.replace("-----END PRIVATE KEY-----", "-----END RSA PRIVATE KEY-----");
        return;
    }
    int pos = 64;
    while(pos < private_key_data.count()) {
        private_key_data.insert(pos, "\n");
        pos += 65;
    }
    private_key_data.prepend("-----BEGIN RSA PRIVATE KEY-----\n");
    if(!private_key_data.endsWith("\n"))
        private_key_data.append("\n");
    private_key_data.append("-----END RSA PRIVATE KEY-----");
}

void alipay_help::alipay_test()
{
    //预支付
    alipay_help::alipay_public_request_argument arg0;
    alipay_help::alipay_trade_precreate_argument arg1;
    alipay_help::alipay_trade_precreate_return_argument arg2;
    arg0.app_id_not_null = "2015121000954514";
    arg0.charset_not_null="utf-8";
    arg0.sign_type=sign_type_SHA1;
    arg0.timestamp_not_null="2015-12-17 03:07:50";
    arg0.version_not_null="1.0";

    //121.41.82.112:9100
    arg1.out_trade_no_not_null = "201503022001";
    arg1.subject_not_null = "apple";
    arg1.total_amount_not_null="21.3";

    //    {   alipay_help::alipay_goods_detail_argument goodsdetail;
    //        goodsdetail.goods_id_not_null = "app-02";
    //        goodsdetail.goods_name_not_null  = "apple";
    //        goodsdetail.quantity_not_null="25.5";
    //        goodsdetail.price_not_null="12.3";
    //        arg1.goods_detail.append(goodsdetail);
    //    } {   alipay_help::alipay_goods_detail_argument goodsdetail;
    //        goodsdetail.goods_id_not_null = "app-02";
    //        goodsdetail.goods_name_not_null  = "apple";
    //        goodsdetail.quantity_not_null="25.5";
    //        goodsdetail.price_not_null="12.3";
    //        arg1.goods_detail.append(goodsdetail);
    //    }
    if(alipay_help::alipay_trade_precreate(arg0, arg1, arg2)) {
        qDebug() << alipay_help::errstring;
    }

    //查询
    {
        alipay_help::alipay_public_request_argument arg0;
        alipay_help::alipay_trade_query_argument arg1;
        alipay_help::alipay_trade_query_return_argument arg2;

        arg0.app_id_not_null = "2015121000954514";
        arg0.charset_not_null="utf-8";
        arg0.sign_type=sign_type_SHA1;
        arg0.timestamp_not_null="2015-12-17 03:07:50";
        arg0.version_not_null="1.0";

        arg1.out_trade_no = "201503022001";
        arg1.trade_no = "201503022001";
        if(alipay_help::alipay_trade_query(arg0, arg1,arg2)) {
            //            qDebug() << arg2.alipay_store_id;
            //            qDebug() << arg2.buyer_logon_id_not_null;
            //            qDebug() << arg2.buyer_pay_amount;
            //            qDebug() << arg2.fund_bill_list;
            //            qDebug() << arg2.invoice_amount;

            //            qDebug() << arg2.open_id_not_null;
            //            qDebug() << arg2.out_trade_no_not_null;
            //            qDebug() << arg2.point_amount;
            //            qDebug() << arg2.receipt_amount;
            //            qDebug() << arg2.send_pay_dateterminal_id;


            //            qDebug() << arg2.store_id;
            //            qDebug() << arg2.terminal_id;
            //            qDebug() << arg2.total_amount_not_null;
            //            qDebug() << arg2.trade_no_not_null;
            //            qDebug() << arg2.trade_status_not_null;
        }
    }

    //条码 支付
    {
        alipay_help::alipay_public_request_argument arg0;
        alipay_help::alipay_trade_pay_argument arg1;
        alipay_help::alipay_trade_pay_return_argument arg2;

        arg0.app_id_not_null = "2015121000954514";
        arg0.charset_not_null="utf-8";
        arg0.sign_type=sign_type_SHA1;
        arg0.timestamp_not_null="2015-12-17 03:07:50";
        arg0.version_not_null="1.0";

        arg1.auth_code_not_null = "284676279936654496";
        arg1.out_trade_no_not_null = "201503022001";
        arg1.subject_not_null = "apple";
        arg1.total_amount_not_null="21.3";

        if(alipay_help::alipay_trade_pay(arg0, arg1, arg2)) {
        }
        qDebug() << arg2.public_response.code_not_null;

    }

    //退钱
    {
        alipay_help::alipay_public_request_argument arg0;
        alipay_help::alipay_trade_refund_argument arg1;
        alipay_help::alipay_trade_refund_return_argument arg2;

        arg0.app_id_not_null = "2015121000954514";
        arg0.charset_not_null="utf-8";
        arg0.sign_type=sign_type_SHA1;
        arg0.timestamp_not_null="2015-12-17 03:07:50";
        arg0.version_not_null="1.0";

        arg1.refund_amount_not_null = "21.30";
        arg1.trade_no_not_null ="2016030321001004560295269147";

        if(alipay_help::alipay_trade_refund(arg0, arg1, arg2)) {
        }
        qDebug() << arg2.public_response.code_not_null;
    }
}

bool alipay_help::alipay_trade_pay(const alipay_help::alipay_public_request_argument &arg0, const alipay_help::alipay_trade_pay_argument &arg1, alipay_help::alipay_trade_pay_return_argument &arg2)
{
    //判断arg0和arg1是否符合规范
    if(check(arg0)
            &&check(arg1)) {
        //判断arg1.goods_detail是否符合规范
        foreach(const alipay_goods_detail_argument &goods, arg1.goods_detail) {
            if(!check(goods)) return false;
        }

        QMap<QString, QString> content;
        content.insert("method", "alipay.trade.pay");

        if(!arg0.app_id_not_null.isEmpty())content.insert("app_id", arg0.app_id_not_null);
        if(!arg0.charset_not_null.isEmpty())content.insert("charset", arg0.charset_not_null);
        if(!arg0.notify_url.isEmpty())content.insert("notify_url", arg0.notify_url);
        if(!arg0.sign_not_null.isEmpty())content.insert("sign", arg0.sign_not_null);

        if(!arg0.sign_type.isEmpty())content.insert("sign_type", arg0.sign_type);
        if(!arg0.timestamp_not_null.isEmpty())content.insert("timestamp", arg0.timestamp_not_null);
        if(!arg0.version_not_null.isEmpty())content.insert("version", arg0.version_not_null);
        cJSON *biz_content=cJSON_CreateObject();
        {
            if(!arg1.auth_code_not_null.isEmpty())cJSON_AddItemToObject(biz_content, "auth_code", cJSON_CreateString(arg1.auth_code_not_null.toLocal8Bit()));
            if(!arg1.body.isEmpty())cJSON_AddItemToObject(biz_content, "body", cJSON_CreateString(arg1.body.toLocal8Bit()));
            if(!arg1.discountable_amount.isEmpty())cJSON_AddItemToObject(biz_content, "discountable_amount", cJSON_CreateString(arg1.discountable_amount.toLocal8Bit()));
            {//extend_params
                cJSON *extend_params=cJSON_CreateObject();
                cJSON_AddItemToObject(biz_content, "extend_params", extend_params);
                if(!arg1.extend_params.hb_fq_num.isEmpty()) cJSON_AddItemToObject(extend_params, "hb_fq_num", cJSON_CreateString(arg1.extend_params.hb_fq_num.toLocal8Bit()));
                if(!arg1.extend_params.hb_fq_seller_percent.isEmpty()) cJSON_AddItemToObject(extend_params, "hb_fq_seller_percent", cJSON_CreateString(arg1.extend_params.hb_fq_seller_percent.toLocal8Bit()));
                if(!arg1.extend_params.sys_service_provider_id.isEmpty()) cJSON_AddItemToObject(extend_params, "sys_service_provider_id", cJSON_CreateString(arg1.extend_params.sys_service_provider_id.toLocal8Bit()));
            }
            if(!arg1.goods_detail.isEmpty()) {
                cJSON *goods_detail=cJSON_CreateArray();
                cJSON_AddItemToObject(biz_content, "goods_detail", goods_detail);
                foreach(const alipay_goods_detail_argument &goods, arg1.goods_detail) {
                    cJSON *goods_detail0=cJSON_CreateObject();
                    cJSON_AddItemToObject(goods_detail, "", goods_detail0);
                    if(!goods.alipay_goods_id.isEmpty())cJSON_AddStringToObject(goods_detail0, "alipay_goods_id", goods.alipay_goods_id.toLocal8Bit());
                    if(!goods.body.isEmpty())cJSON_AddStringToObject(goods_detail0, "body", goods.body.toLocal8Bit());
                    if(!goods.goods_category.isEmpty())cJSON_AddStringToObject(goods_detail0, "goods_category", goods.goods_category.toLocal8Bit());

                    if(!goods.goods_id_not_null.isEmpty())cJSON_AddStringToObject(goods_detail0, "goods_id", goods.goods_id_not_null.toLocal8Bit());
                    if(!goods.goods_name_not_null.isEmpty())cJSON_AddStringToObject(goods_detail0, "goods_name", goods.goods_name_not_null.toLocal8Bit());
                    if(!goods.price_not_null.isEmpty())cJSON_AddStringToObject(goods_detail0, "price", goods.price_not_null.toLocal8Bit());
                    if(!goods.quantity_not_null.isEmpty())cJSON_AddStringToObject(goods_detail0, "quantity", goods.quantity_not_null.toLocal8Bit());
                }
            }

            if(!arg1.operator_id.isEmpty())cJSON_AddItemToObject(biz_content, "operator_id", cJSON_CreateString(arg1.operator_id.toLocal8Bit()));
            if(!arg1.out_trade_no_not_null.isEmpty())cJSON_AddItemToObject(biz_content, "out_trade_no", cJSON_CreateString(arg1.out_trade_no_not_null.toLocal8Bit()));
            if(!arg1.royalty_info.isEmpty())cJSON_AddItemToObject(biz_content, "royalty_info", cJSON_CreateString(arg1.royalty_info.toLocal8Bit()));
            if(!arg1.scene_not_null.isEmpty())cJSON_AddItemToObject(biz_content, "scene", cJSON_CreateString(arg1.scene_not_null.toLocal8Bit()));
            if(!arg1.seller_id.isEmpty())cJSON_AddItemToObject(biz_content, "seller_id", cJSON_CreateString(arg1.seller_id.toLocal8Bit()));

            if(!arg1.store_id.isEmpty())cJSON_AddItemToObject(biz_content, "store_id", cJSON_CreateString(arg1.store_id.toLocal8Bit()));
            if(!arg1.subject_not_null.isEmpty())cJSON_AddItemToObject(biz_content, "subject", cJSON_CreateString(arg1.subject_not_null.toLocal8Bit()));
            if(!arg1.terminal_id.isEmpty())cJSON_AddItemToObject(biz_content, "terminal_id", cJSON_CreateString(arg1.terminal_id.toLocal8Bit()));
            if(!arg1.time_expire.isEmpty())cJSON_AddItemToObject(biz_content, "time_expire", cJSON_CreateString(arg1.time_expire.toLocal8Bit()));
            if(!arg1.total_amount_not_null.isEmpty())cJSON_AddItemToObject(biz_content, "total_amount", cJSON_CreateString(arg1.total_amount_not_null.toLocal8Bit()));

            if(!arg1.undiscountable_amount.isEmpty())cJSON_AddItemToObject(biz_content, "undiscountable_amount", cJSON_CreateString(arg1.undiscountable_amount.toLocal8Bit()));
        }
        if(!arg0.biz_content_not_null.isEmpty())content.insert("biz_content", QString(cJSON_Print(biz_content)));
        cJSON_Delete(biz_content);
        //准备上传
        QByteArray data;
        QByteArray retvalue;
        return ALIPAY_COMMIT(arg0, content, data, retvalue, arg2);
    }
    return false;
}

bool alipay_help::alipay_trade_precreate(const alipay_help::alipay_public_request_argument &arg0, const alipay_help::alipay_trade_precreate_argument &arg1, alipay_trade_precreate_return_argument &arg2)
{
    //判断arg0和arg1是否符合规范
    if(check(arg0)
            &&check(arg1)) {
        //判断arg1.goods_detail是否符合规范
        foreach(const alipay_goods_detail_argument &goods, arg1.goods_detail) {
            if(!check(goods)) return false;
        }

        QMap<QString, QString> content;
        content.insert("method", "alipay.trade.precreate");

        if(!arg0.app_id_not_null.isEmpty())content.insert("app_id", arg0.app_id_not_null);
        if(!arg0.charset_not_null.isEmpty())content.insert("charset", arg0.charset_not_null);
        if(!arg0.notify_url.isEmpty())content.insert("notify_url", arg0.notify_url);
        if(!arg0.sign_not_null.isEmpty())content.insert("sign", arg0.sign_not_null);

        content.insert("sign_type", arg0.sign_type);
        if(!arg0.timestamp_not_null.isEmpty())content.insert("timestamp", arg0.timestamp_not_null);
        if(!arg0.version_not_null.isEmpty())content.insert("version", arg0.version_not_null);

        cJSON *biz_content=cJSON_CreateObject();
        {
            if(!arg1.body.isEmpty())cJSON_AddItemToObject(biz_content, "body", cJSON_CreateString(arg1.body.toLocal8Bit()));
            if(!arg1.discountable_amount.isEmpty())cJSON_AddItemToObject(biz_content, "discountable_amount", cJSON_CreateString(arg1.discountable_amount.toLocal8Bit()));
            {//extend_params
                cJSON *extend_params=cJSON_CreateObject();
                cJSON_AddItemToObject(biz_content, "extend_params", extend_params);
                if(!arg1.extend_params.hb_fq_num.isEmpty()) cJSON_AddItemToObject(extend_params, "hb_fq_num", cJSON_CreateString(arg1.extend_params.hb_fq_num.toLocal8Bit()));
                if(!arg1.extend_params.hb_fq_seller_percent.isEmpty()) cJSON_AddItemToObject(extend_params, "hb_fq_seller_percent", cJSON_CreateString(arg1.extend_params.hb_fq_seller_percent.toLocal8Bit()));
                if(!arg1.extend_params.sys_service_provider_id.isEmpty()) cJSON_AddItemToObject(extend_params, "sys_service_provider_id", cJSON_CreateString(arg1.extend_params.sys_service_provider_id.toLocal8Bit()));
            }
            if(!arg1.goods_detail.isEmpty()) {
                cJSON *goods_detail=cJSON_CreateArray();
                cJSON_AddItemToObject(biz_content, "goods_detail", goods_detail);
                foreach(const alipay_goods_detail_argument &goods, arg1.goods_detail) {
                    cJSON *goods_detail0=cJSON_CreateObject();
                    cJSON_AddItemToObject(goods_detail, "", goods_detail0);
                    if(!goods.alipay_goods_id.isEmpty())cJSON_AddStringToObject(goods_detail0, "alipay_goods_id", goods.alipay_goods_id.toLocal8Bit());
                    if(!goods.body.isEmpty())cJSON_AddStringToObject(goods_detail0, "body", goods.body.toLocal8Bit());
                    if(!goods.goods_category.isEmpty())cJSON_AddStringToObject(goods_detail0, "goods_category", goods.goods_category.toLocal8Bit());

                    if(!goods.goods_id_not_null.isEmpty())cJSON_AddStringToObject(goods_detail0, "goods_id", goods.goods_id_not_null.toLocal8Bit());
                    if(!goods.goods_name_not_null.isEmpty())cJSON_AddStringToObject(goods_detail0, "goods_name", goods.goods_name_not_null.toLocal8Bit());
                    if(!goods.price_not_null.isEmpty())cJSON_AddStringToObject(goods_detail0, "price", goods.price_not_null.toLocal8Bit());
                    if(!goods.quantity_not_null.isEmpty())cJSON_AddStringToObject(goods_detail0, "quantity", goods.quantity_not_null.toLocal8Bit());
                }
            }
            if(!arg1.operator_id.isEmpty())cJSON_AddItemToObject(biz_content, "operator_id", cJSON_CreateString(arg1.operator_id.toLocal8Bit()));

            if(!arg1.out_trade_no_not_null.isEmpty())cJSON_AddItemToObject(biz_content, "out_trade_no", cJSON_CreateString(arg1.out_trade_no_not_null.toLocal8Bit()));
            if(!arg1.seller_id.isEmpty())cJSON_AddItemToObject(biz_content, "seller_id", cJSON_CreateString(arg1.seller_id.toLocal8Bit()));
            if(!arg1.store_id.isEmpty())cJSON_AddItemToObject(biz_content, "store_id", cJSON_CreateString(arg1.store_id.toLocal8Bit()));
            if(!arg1.subject_not_null.isEmpty())cJSON_AddItemToObject(biz_content, "subject", cJSON_CreateString(arg1.subject_not_null.toLocal8Bit()));
            if(!arg1.terminal_id.isEmpty())cJSON_AddItemToObject(biz_content, "terminal_id", cJSON_CreateString(arg1.terminal_id.toLocal8Bit()));

            if(!arg1.time_expire.isEmpty())cJSON_AddItemToObject(biz_content, "time_expire", cJSON_CreateString(arg1.time_expire.toLocal8Bit()));
            if(!arg1.total_amount_not_null.isEmpty())cJSON_AddItemToObject(biz_content, "total_amount", cJSON_CreateString(arg1.total_amount_not_null.toLocal8Bit()));
            if(!arg1.undiscountable_amount.isEmpty())cJSON_AddItemToObject(biz_content, "undiscountable_amount", cJSON_CreateString(arg1.undiscountable_amount.toLocal8Bit()));
        }
        if(!arg0.biz_content_not_null.isEmpty())content.insert("biz_content", QString(cJSON_Print(biz_content)));

        cJSON_Delete(biz_content);

        //准备上传
        QByteArray data;
        QByteArray retvalue;
        return ALIPAY_COMMIT(arg0, content, data, retvalue, arg2);
    }
    return false;
}

bool alipay_help::alipay_trade_query(const alipay_help::alipay_public_request_argument &arg0, const alipay_help::alipay_trade_query_argument &arg1, alipay_trade_query_return_argument &arg2)
{
    //判断arg0和arg1是否符合规范
    if(check(arg1)
            &&check(arg0)) {
        QMap<QString, QString> content;
        content.insert("method", "alipay.trade.query");

        if(!arg0.app_id_not_null.isEmpty())content.insert("app_id", arg0.app_id_not_null);
        if(!arg0.charset_not_null.isEmpty())content.insert("charset", arg0.charset_not_null);
        if(!arg0.notify_url.isEmpty())content.insert("notify_url", arg0.notify_url);
        if(!arg0.sign_not_null.isEmpty())content.insert("sign", arg0.sign_not_null);

        if(!arg0.sign_type.isEmpty())content.insert("sign_type", arg0.sign_type);
        if(!arg0.timestamp_not_null.isEmpty())content.insert("timestamp", arg0.timestamp_not_null);
        if(!arg0.version_not_null.isEmpty())content.insert("version", arg0.version_not_null);
        cJSON *biz_content=cJSON_CreateObject();
        {
            if(!arg1.trade_no.isEmpty())cJSON_AddItemToObject(biz_content, "trade_no", cJSON_CreateString(arg1.trade_no.toLocal8Bit()));
            if(!arg1.out_trade_no.isEmpty())cJSON_AddItemToObject(biz_content, "out_trade_no", cJSON_CreateString(arg1.out_trade_no.toLocal8Bit()));
        }
        if(!arg0.biz_content_not_null.isEmpty())content.insert("biz_content", QString(cJSON_Print(biz_content)));
        cJSON_Delete(biz_content);
        //准备上传
        QByteArray data;
        QByteArray retvalue;
        return ALIPAY_COMMIT(arg0, content, data, retvalue, arg2);
    }

    return false;
}

bool alipay_help::alipay_trade_cancel()
{
    return true;
}

bool alipay_help::alipay_trade_refund(const alipay_help::alipay_public_request_argument &arg0, const alipay_help::alipay_trade_refund_argument &arg1, alipay_help::alipay_trade_refund_return_argument &arg2)
{
    //判断arg0和arg1是否符合规范
    if(check(arg0)
            &&check(arg1)) {
        QMap<QString, QString> content;
        content.insert("method", "alipay.trade.refund");
        if(!arg0.app_id_not_null.isEmpty())content.insert("app_id", arg0.app_id_not_null);
        if(!arg0.charset_not_null.isEmpty())content.insert("charset", arg0.charset_not_null);
        if(!arg0.notify_url.isEmpty())content.insert("notify_url", arg0.notify_url);
        if(!arg0.sign_not_null.isEmpty())content.insert("sign", arg0.sign_not_null);

        if(!arg0.sign_type.isEmpty())content.insert("sign_type", arg0.sign_type);
        if(!arg0.timestamp_not_null.isEmpty())content.insert("timestamp", arg0.timestamp_not_null);
        if(!arg0.version_not_null.isEmpty())content.insert("version", arg0.version_not_null);

        cJSON *biz_content=cJSON_CreateObject();
        {
            if(!arg1.out_request_no.isEmpty())cJSON_AddItemToObject(biz_content, "out_request_no", cJSON_CreateString(arg1.out_request_no.toLocal8Bit()));
            if(!arg1.refund_amount_not_null.isEmpty())cJSON_AddItemToObject(biz_content, "refund_amount", cJSON_CreateString(arg1.refund_amount_not_null.toLocal8Bit()));
            if(!arg1.refund_reason.isEmpty())cJSON_AddItemToObject(biz_content, "refund_reason", cJSON_CreateString(arg1.refund_reason.toLocal8Bit()));
            if(!arg1.store_id.isEmpty())cJSON_AddItemToObject(biz_content, "store_id", cJSON_CreateString(arg1.store_id.toLocal8Bit()));
            if(!arg1.terminal_id.isEmpty())cJSON_AddItemToObject(biz_content, "terminal_id", cJSON_CreateString(arg1.terminal_id.toLocal8Bit()));
            if(!arg1.trade_no_not_null.isEmpty())cJSON_AddItemToObject(biz_content, "trade_no", cJSON_CreateString(arg1.trade_no_not_null.toLocal8Bit()));
        }
        if(!arg0.biz_content_not_null.isEmpty())content.insert("biz_content", QString(cJSON_Print(biz_content)));
        cJSON_Delete(biz_content);

        //准备上传
        QByteArray data;
        QByteArray retvalue;
        return ALIPAY_COMMIT(arg0, content, data, retvalue, arg2);
    }
    return false;
}
