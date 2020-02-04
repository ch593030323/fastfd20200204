#include "testwx_help.h"
#include <QTimer>
#include <QDateTime>
#include <QHostInfo>
#include <QSslConfiguration>
/*
wxd9ccc742b1c39110
10036785
wwwscposcomhdd802600000000000000
*/

testwx_help::testwx_help(QObject *parent) : QObject(parent)
{

}

bool testwx_help::AmountQRcodeRequest(const QString &appid, const QString &tradedesc,  int total_fee, const QString &paykey, const QString &mch_id, const QString &out_trade_no, QByteArray &qrcode)
{
    QString errstring;
    QByteArray post;
    QByteArray recv;
    retDataStruct ret;

    //    QString appid="wxd9ccc742b1c39110";
    QString body=tradedesc;//"qt测试商品";//商品描述
    //    QString total_fee="1";//分
    //    QString paykey="wwwscposcomhdd802600000000000000";
    QString randomString = getuuid();
    /*string(32)*/QString newOrderNo = out_trade_no;
    //    QString mch_id="10036785";

    post.append("appid=" + appid + "&");
    post.append("body=" + body + "&");
    post.append("mch_id="+mch_id+"&");
    post.append("nonce_str=" + randomString + "&");
    post.append("notify_url=http://www.scpos.com&");
    post.append("out_trade_no=" + newOrderNo + "&");//商户订单号，最多32位
    post.append("product_id=" + newOrderNo + "&");
    post.append("total_fee=" + QString::number(total_fee) + "&");
    post.append("trade_type=NATIVE&");
    post.append("key=" + paykey + "");

    QString md5;
    QByteArray bb;
    bb = QCryptographicHash::hash ( post, QCryptographicHash::Md5 );
    md5.append(bb.toHex());
    QString signString =md5.toUpper();//
    post.clear();
    post.append("<xml>");
    post.append("<appid>" + appid + "</appid>");
    post.append("<mch_id>"+mch_id+"</mch_id>");
    post.append("<nonce_str>" + randomString + "</nonce_str>");
    post.append("<body>" + body + "</body>");
    post.append("<out_trade_no>" + newOrderNo + "</out_trade_no>");
    post.append("<total_fee>" + QString::number(total_fee) + "</total_fee>");
    post.append("<notify_url>http://www.scpos.com</notify_url>");
    post.append("<trade_type>NATIVE</trade_type>");
    post.append("<product_id>" + newOrderNo + "</product_id>");
    post.append("<sign>" + signString + "</sign>");
    post.append("</xml>");

    if(WeixinPost(QString("https://api.mch.weixin.qq.com/pay/unifiedorder"),post,recv, &errstring)) {
        QXmlStreamReader streamxml(recv);
        ret = reslovexml(streamxml);
        if(ret.code_url!="") {
            qrcode = ret.code_url.toLocal8Bit();
            return true;
        } else  {
            qDebug() << "return_code:" <<   ret.return_code;
            qDebug() << "return_msg:" <<    ret.return_msg;
            //            qDebug() << "appid     :" <<    ret.appid;
            //            qDebug() << "mch_id     :" <<   ret.mch_id;
            qDebug() << "nonce_str:" <<     ret.nonce_str;
            qDebug() << "sign     :" <<     ret.sign;
            qDebug() << "result_code:" <<   ret.result_code;
            qDebug() << "prepay_id:" <<     ret.prepay_id;
            qDebug() << "code_url:" <<      ret.code_url;
            qDebug() << "trade_type:" <<    ret.trade_type;
            qDebug() << "err_code:" <<      ret.err_code;
            qDebug() << "err_code_des:" <<  ret.err_code_des;
            qDebug() << "time_end:" <<      ret.time_end;
            qDebug() << "trade_state:" <<   ret.trade_state;
            qDebug() << "total_fee:" <<     ret.total_fee;
            qDebug() << "transaction_id:" <<ret.transaction_id;
        }
    } else {
        qDebug() << __LINE__ << errstring;
    }//! if

    return false;
}
#include "stdlib.h"
#include "stdio.h"

bool testwx_help::WeixinPost(const QString &url, const QByteArray &content, QByteArray &responseData, QString *errstring)
{
    int contentLength = content.length();
    QNetworkRequest req;
    QNetworkAccessManager manager;
    QEventLoop eloop;
    QTimer timer;
    req.setUrl(QUrl(url));//支付宝 美团 大众点评 糯米
    //application/octet-stream
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    req.setHeader(QNetworkRequest::ContentLengthHeader,contentLength);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#else
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1);
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    req.setSslConfiguration(config);
#endif

    timer.setSingleShot(true);
    connect(&timer,SIGNAL(timeout()),&eloop,SLOT(quit()));
    connect(&manager,SIGNAL(finished(QNetworkReply*)),&eloop,SLOT(quit()));
    QNetworkReply *reply=manager.post(req,content);
    timer.start(10*1000);//10s的超时eply*)),&eloop,SLOT(quit()));
    eloop.exec(QEventLoop::ExcludeUserInputEvents);

    if(timer.isActive()) {
        if(reply->error()==QNetworkReply::NoError) {
            responseData = reply->readAll();
            return true;
        } else {
            if(errstring) {
                errstring->clear();
                *errstring += reply->errorString();
                *errstring += reply->readAll();
            }
        }
    } else {
        if(errstring)
            *errstring = "timeout";
    }
    return false;
}

QString testwx_help::getuuid()
{
    QUuid id = QUuid::createUuid();
    QString strId = id.toString();

    QString newuuid=strId.replace(QRegExp("\\-"), "").replace(QRegExp("\\{"), "").replace(QRegExp("\\}"), "");

    return newuuid;
}

testwx_help::retDataStruct testwx_help::getPayBillInfo(const QString &out_trade_no, const QString &appid,const QString &paykey, const QString &mch_id)
{
    QByteArray post;
    QByteArray recv;
    retDataStruct ret;

    //    QString appid="wxd9ccc742b1c39110";
    //    QString paykey="wwwscposcomhdd802600000000000000";
    QString randomString = getuuid();
    QString newOrderNo = out_trade_no;//"888889";
    //    QString mch_id = "10036785";
    post.append("appid=" + appid + "&");
    post.append("mch_id="+mch_id+"&");
    post.append("nonce_str=" + randomString + "&");
    post.append("out_trade_no=" + newOrderNo + "&");
    post.append("key=" + paykey + "");

    QString md5;
    QByteArray bb;
    bb = QCryptographicHash::hash ( post, QCryptographicHash::Md5 );
    md5.append(bb.toHex());
    QString signString =md5.toUpper();//

    post.clear();
    post.append("<xml>");
    post.append("<appid>" + appid + "</appid>");
    post.append("<mch_id>"+mch_id+"</mch_id>");
    post.append("<nonce_str>" + randomString + "</nonce_str>");
    post.append("<out_trade_no>" + newOrderNo + "</out_trade_no>");
    post.append("<sign>" + signString + "</sign>");
    post.append("</xml>");
    if(WeixinPost(QString("https://api.mch.weixin.qq.com/pay/orderquery"),post,recv)) {
        QXmlStreamReader streamxml(recv);
        ret = reslovexml(streamxml);
    }

    return ret;
}

testwx_help::retDataStruct testwx_help::reslovexml(QXmlStreamReader &streamxml)
{
    testwx_help::retDataStruct ret;
    // QMap person;
    while(!streamxml.atEnd())
    {
        streamxml.readNext();
        if(streamxml.isStartElement()) {
            if(streamxml.name() == "return_code") {
                ret.result_code= streamxml.readElementText();
            }
            else if(streamxml.name() == "return_msg")  {
                ret.return_msg= streamxml.readElementText();
            }
            else if(streamxml.name() == "appid")  {
                ret.appid= streamxml.readElementText();
            }
            else if(streamxml.name() == "mch_id")  {
                ret.mch_id= streamxml.readElementText();
            }
            else if(streamxml.name() == "nonce_str")  {
                ret.nonce_str= streamxml.readElementText();
            }
            else if(streamxml.name() == "sign")    {
                ret.sign= streamxml.readElementText();
            }
            else if(streamxml.name() == "code_url")   {
                ret.code_url= streamxml.readElementText();
            }
            else if(streamxml.name() == "err_code")   {
                ret.err_code= streamxml.readElementText();
            }
            else if(streamxml.name() == "err_code_des") {
                ret.err_code_des= streamxml.readElementText();
            }
            else if(streamxml.name() == "time_end")  {
                ret.time_end = streamxml.readElementText();
            }
            else if(streamxml.name() == "trade_state")    {
                ret.trade_state = streamxml.readElementText();
            }
            else if(streamxml.name() == "total_fee")  {
                ret.total_fee = streamxml.readElementText();
            }
            else if(streamxml.name() == "transaction_id") {
                ret.transaction_id = streamxml.readElementText();
            }
        }
    }//! while

    return ret;
}

QPixmap testwx_help::get_rbcode(const char *string)
{
    //1
    QPixmap pixmap_rb;
    QRcode *qr = QRcode_encodeString(string,3, QR_ECLEVEL_L, QR_MODE_8, 1);
    if(0 != qr) {
        pixmap_rb = QPixmap(qr->width, qr->width);

        QPainter painter(&pixmap_rb);
        painter.fillRect(QRect(0, 0, pixmap_rb.width(), pixmap_rb.height()), QColor("white"));
        for(int y = 0; y < pixmap_rb.height(); y++) {
            for(int x = 0; x < pixmap_rb.width(); x++) {
                const unsigned char b = qr->data[y * pixmap_rb.height() + x];
                if(b &0x01) {
                    painter.fillRect(QRect(x, y, 1, 1), QColor("black"));
                }
            }
        }

        //-1
        QRcode_free(qr);
    }
    return pixmap_rb;
}

QPixmap testwx_help::get_rbcode(const char *string, int width)
{
    QPixmap pixmap2 = get_rbcode(string);
    int rb_width_org = pixmap2.width();
    if(rb_width_org == 0) return QPixmap();

    pixmap2 = pixmap2.scaledToWidth(qMax(rb_width_org, rb_width_org * (width/rb_width_org)));
    return pixmap2;
}

QPixmap testwx_help::get_rbcode(const char *string, int width, const QString &rb_title, const QString &rb_desc, const QColor &rb_color)
{
    QPixmap pixmap1;
    QPixmap pixmap2 = get_rbcode(string);
    int rb_width_org = pixmap2.width();
    if(rb_width_org == 0) return QPixmap();

    pixmap2 = pixmap2.scaledToWidth(qMax(rb_width_org, rb_width_org * (width/rb_width_org)));

    int title_h = 25;
    int desc_h = 20;
    int border_height = 10;

    pixmap1 = QPixmap(width,  title_h + desc_h + border_height*2 + pixmap2.height());

    QPainter p(&pixmap1);
    p.fillRect(QRect(0, 0, pixmap1.width(), pixmap1.height()), QColor("white"));

    //1
    p.setPen(rb_color);
    QFont f = p.font();

    //2
    f.setPixelSize(title_h);
    p.setFont(f);
    p.drawText(QRect(0, 0, width, title_h), Qt::AlignCenter, rb_title);

    //3
    f.setPixelSize(desc_h);
    p.setFont(f);
    p.drawText(QRect(0, title_h, width, desc_h), Qt::AlignCenter, rb_desc);

    //4
    p.drawPixmap(QPoint((pixmap1.width() - pixmap2.width())/2, title_h +desc_h + border_height), pixmap2);

    return pixmap1;
}

QPixmap testwx_help::get_rbcode(const char *string, int width, const QString &rb_title, const QString &rb_desc, const QColor &rb_color, int border)
{
    QPixmap p = get_rbcode(string, width, rb_title, rb_desc, rb_color);
    QPixmap p2(p.size() + QSize(border*2, border*2));
    QPainter painter(&p2);
    painter.fillRect(QRect(0, 0,  p2.width(), p2.height()), QColor("white"));
    painter.drawPixmap(border, border, p);

    return p2;
}

QPixmap testwx_help::get_rbcode(const QString &title, const char *rb_wechat, const QString &title_wechat, const char *rb_alipay, const QString &title_alipay, const QString &ch_paymodeno)
{
    int yoff = 0;
    int xoff = 0;
    if(ch_paymodeno == "05") {//wechat
        xoff = 120 /2 + 50 / 2;
        yoff = -21 - 5;
    }
    if(ch_paymodeno == "06") {//alipay
        xoff = 120 /2 + 50 / 2;
        xoff = -1 * xoff;
        yoff = -21 - 5;
    }
    QPixmap pixmap2 = get_rbcode(rb_wechat);
    pixmap2 = pixmap2.scaledToWidth(120);
    pixmap2.save("pixmap2.jpg");

    QPixmap pixmap3 = get_rbcode(rb_alipay);
    pixmap3 = pixmap3.scaledToWidth(120);
    pixmap3.save("pixmap3.jpg");


    QPixmap pixmap1(5 + 120 + 50 + 120 + 5, 5 + 31 + 5 + 21 + 5 +120 + 5 + yoff);
    QPainter p(&pixmap1);
    p.fillRect(QRect(0, 0, pixmap1.width(), pixmap1.height()), QColor("white"));
    QFont font = p.font();

    p.setPen(QColor("#ff546b"));
    font.setPixelSize(31);p.setFont(font);
    p.drawText(QRect(0                  ,  5,                      pixmap1.width(), 31), Qt::AlignCenter, title);
    font.setPixelSize(21);p.setFont(font);

    p.translate(xoff, yoff);
    if(yoff >= 0) p.drawText(QRect(5                  ,  5 + 31 + 5,      120,                      21), Qt::AlignCenter, title_wechat);
    if(yoff >= 0) p.drawText(QRect(5 + 120 + 50, 5 + 31 + 5,      120,                       21), Qt::AlignCenter, title_alipay);

    p.setPen(QColor("black"));
    if(xoff >= 0) p.drawPixmap(5,                    5 + 31 + 5 + 21 + 5, pixmap2);
    if(xoff <= 0) p.drawPixmap(5 + 120 + 50, 5 + 31 + 5 + 21 + 5, pixmap3);

    pixmap1.save("pixmap1.jpg");
    return pixmap1;
}

QString testwx_help::OnlinePayTypeDesc(int type)
{
    if(type == OnlinePayTypeWeChat) {
        return QObject::tr("微信支付");
    }
    if(type == OnlinePayTypeAliPay) {
        return QObject::tr("支付宝支付");
    }
    return "";
}

QVariant testwx_help::getcJSONvalue(cJSON *root, const char *string)
{
    QVariant ret;
    if(root == 0) {
        return ret;
    }
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

QVariant testwx_help::getcJSONvalue(const QByteArray &json, const char *string)
{
    cJSON *p = cJSON_Parse(json);
    QVariant d;
    if(p) {
        d =  getcJSONvalue(p, string);
        cJSON_Delete(p);
    }
    return d;
}

