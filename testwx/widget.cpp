#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->netManager = new QNetworkAccessManager(this);
    // QObject::connect(netManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished(QNetworkReply*)));
    connect(netManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    weimawidget=new erweima();
    ui->verticalLayout_erweima->addWidget(weimawidget);
    weimawidget->setVisible(false);
}

Widget::~Widget()
{
    delete ui;
}

bool Widget::do_post(QString url, QByteArray content, QByteArray &responseData)
{
    int contentLength = content.length();
    QNetworkRequest req;
    req.setUrl(QUrl(url));
    //application/octet-stream
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    req.setHeader(QNetworkRequest::ContentLengthHeader,contentLength);

    QNetworkReply *reply2=this->netManager->post(req,content);
    //connect(reply2,SIGNAL(uploadProgress(qint64,qint64)),SLOT(updateProgress(qint64,qint64)));

    eventLoop.exec();       //block until finish
    // QByteArray responseData;
    QVariant status_code = reply2->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(reply2->error() == QNetworkReply::NoError) {



        //load_dialog->hide();
        responseData = reply2->readAll();
        delete reply2;//移除reply2

        return true;
    }  else{

        qDebug()<<"err"<<reply2->errorString();
        //myHelper2::ShowMessageBoxError2(reply2->errorString());
        delete reply2;//移除reply2
        return false;
    }
}

void Widget::on_pushButton_clicked()
{
    QByteArray post;
    QByteArray recv;
    retDataStruct ret;

    QString appid="wxd9ccc742b1c39110";
    QString body="qt测试商品";//商品描述
    QString total_fee="1";//分
    QString payKey="wwwscposcomhdd802600000000000000";
    QString randomString = getuuid();
    QString newOrderNo = "8888890";//randomString;//orderno+"_"+(System.currentTimeMillis()+"").substring(6)+getRandomString(10);


    post.append("appid=" + appid + "&");
    post.append("body=" + body + "&");
    post.append("mch_id=10036785&");
    post.append("nonce_str=" + randomString + "&");
    post.append("notify_url=http://www.scpos.com&");
    post.append("out_trade_no=" + newOrderNo + "&");//商户订单号，最多32位
    post.append("product_id=" + newOrderNo + "&");
    post.append("total_fee=" + total_fee + "&");
    post.append("trade_type=NATIVE&");
    post.append("key=" + payKey + "");

    QString md5;
    QByteArray bb;
    bb = QCryptographicHash::hash ( post, QCryptographicHash::Md5 );
    md5.append(bb.toHex());
    QString signString =md5.toUpper();//
    post.clear();
    post.append("<xml>");
    post.append("<appid>" + appid + "</appid>");
    post.append("<mch_id>10036785</mch_id>");
    post.append("<nonce_str>" + randomString + "</nonce_str>");
    post.append("<body>" + body + "</body>");
    post.append("<out_trade_no>" + newOrderNo + "</out_trade_no>");
    post.append("<total_fee>" + total_fee + "</total_fee>");
    post.append("<notify_url>http://www.scpos.com</notify_url>");
    post.append("<trade_type>NATIVE</trade_type>");
    post.append("<product_id>" + newOrderNo + "</product_id>");
    post.append("<sign>" + signString + "</sign>");
    post.append("</xml>");


    if( do_post(QString("https://api.mch.weixin.qq.com/pay/unifiedorder"),post,recv)) {

        qDebug()<<QObject::trUtf8( recv.data());
        QXmlStreamReader streamxml(recv);
        // QMap person;

        while(!streamxml.atEnd())
        {
            streamxml.readNext();
            if(streamxml.isStartElement())   {
                if(streamxml.name() == "return_code") {
                    ret.result_code= streamxml.readElementText();
                }
                if(streamxml.name() == "return_msg")   {
                    ret.return_msg= streamxml.readElementText();
                }
                if(streamxml.name() == "appid")  {
                    ret.appid= streamxml.readElementText();
                }
                if(streamxml.name() == "mch_id")   {
                    ret.mch_id= streamxml.readElementText();
                }
                if(streamxml.name() == "nonce_str")  {
                    ret.nonce_str= streamxml.readElementText();
                }
                if(streamxml.name() == "sign")  {
                    ret.sign= streamxml.readElementText();
                }
                if(streamxml.name() == "code_url")    {
                    ret.code_url= streamxml.readElementText();
                    if(ret.code_url!="") {
                        weimawidget->setVisible(true);
                        const QByteArray text =ret.code_url.toLocal8Bit();
                        const char *data = text.data();
                        weimawidget->setstr(data);
                    }
                }
                if(streamxml.name() == "err_code") {
                    ret.err_code= streamxml.readElementText();
                }
                if(streamxml.name() == "err_code_des")  {
                    ret.err_code_des= streamxml.readElementText();
                }
            }
        }
        qDebug() << ret.return_code;
        qDebug() << ret.return_msg;
        qDebug() << ret.appid;
        qDebug() << ret.mch_id;
        qDebug() << ret.nonce_str;
        qDebug() << ret.sign;
        qDebug() << ret.result_code;
        qDebug() << ret.prepay_id;
        qDebug() << ret.code_url;
        qDebug() << ret.trade_type;
        qDebug() << ret.err_code;
        qDebug() << ret.err_code_des;
    }
}

QString Widget::getuuid()
{
    QUuid id = QUuid::createUuid();
    QString strId = id.toString();

    QString newuuid=strId.replace(QRegExp("\\-"), "").replace(QRegExp("\\{"), "").replace(QRegExp("\\}"), "");

    return newuuid;
}

void Widget::on_pushButton_2_clicked()
{
    QByteArray post;
    QByteArray recv;
    retDataStruct ret;

    QString appid="wxd9ccc742b1c39110";
    QString payKey="wwwscposcomhdd802600000000000000";
    QString randomString = getuuid();
    QString newOrderNo = "888889";

    post.append("appid=" + appid + "&");
    post.append("mch_id=10036785&");
    post.append("nonce_str=" + randomString + "&");
    post.append("out_trade_no=" + newOrderNo + "&");
    post.append("key=" + payKey + "");

    QString md5;
    QByteArray bb;
    bb = QCryptographicHash::hash ( post, QCryptographicHash::Md5 );
    md5.append(bb.toHex());
    QString signString =md5.toUpper();//

    post.clear();
    post.append("<xml>");
    post.append("<appid>" + appid + "</appid>");
    post.append("<mch_id>10036785</mch_id>");
    post.append("<nonce_str>" + randomString + "</nonce_str>");
    post.append("<out_trade_no>" + newOrderNo + "</out_trade_no>");
    post.append("<sign>" + signString + "</sign>");
    post.append("</xml>");
    if( do_post(QString("https://api.mch.weixin.qq.com/pay/orderquery"),post,recv)) {
        qDebug()<<QObject::trUtf8( recv.data());
    }

}
