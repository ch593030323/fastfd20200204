#include "ccb_pay_help.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDateTime>
#include <QEventLoop>
#include <QCryptographicHash>
#include <QDomDocument>
#include <QMessageBox>
#include "testwx_help.h"

ccb_pay_help::ccb_pay_help(QObject *parent)
    : QObject(parent)
{
}

bool ccb_pay_help::getPayUrl(const QString &MERCHANTID, const QString &POSID, const QString &BRANCHID, const QString &MERCHKEY, const QString &PAYMENT, const QString &ORDERID, QByteArray &ret_code_url, QString &errstring)
{
    QNetworkAccessManager m;
    QEventLoop loop;
    QNetworkRequest request;
    QNetworkReply *reply = 0;
    QByteArray readall;

    QString PAYURL;
    QByteArray QRURL;

    QString CURCODE = "01";//人民币
    QString TXCODE = "530550";//固定
    QString REMARK1;
    QString REMARK2;
    QString RETURNTYPE = "3";//0或空：返回页面二维码、1：返回JSON格式【二维码信息串】、2：返回聚合扫码页面二维码、3：返回聚合扫码JSON格式【二维码信息串】
    QString TIMEOUT;
    QString PUB = MERCHKEY;
    ccb_pay_help_pair msg;
    connect(&m, SIGNAL(finished(QNetworkReply*)),&loop, SLOT(quit()));

    msg.clear();
    msg.append("MERCHANTID", MERCHANTID);
    msg.append("POSID", POSID);
    msg.append("BRANCHID", BRANCHID);
    msg.append("ORDERID", ORDERID);
    msg.append("PAYMENT", PAYMENT);

    msg.append("CURCODE", CURCODE);
    msg.append("TXCODE", TXCODE);
    msg.append("REMARK1", REMARK1);
    msg.append("REMARK2", REMARK2);
    msg.append("RETURNTYPE", RETURNTYPE);

    msg.append("TIMEOUT", TIMEOUT);
    //encrypted
    msg.append("MAC", QCryptographicHash::hash ( (msg.join() +"&" + "PUB="+PUB).toLocal8Bit(), QCryptographicHash::Md5 ).toHex());
    //url
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/html; charset=utf-8");
    request.setUrl(QString("https://ibsbjstar.ccb.com.cn/CCBIS/ccbMain?CCB_IBSVersion=V6&") + msg.join());

    reply = m.post(request, "");
    loop.exec();

    readall = reply->readAll();
    if("200" == replyHttpCode(reply)) {
        qDebug() << __FILE__ << __LINE__ << readall;
        if("true" == testwx_help::getcJSONvalue(readall, "SUCCESS").toString()) {
            PAYURL = testwx_help::getcJSONvalue(readall, "PAYURL").toString();

            request.setUrl(PAYURL);
            reply = m.post(request, "");
            loop.exec();
            readall = reply->readAll();
            if("200" == replyHttpCode(reply)) {
                if("true" == testwx_help::getcJSONvalue(readall, "SUCCESS").toString()) {
                    QRURL = testwx_help::getcJSONvalue(readall, "QRURL").toByteArray();
                    QRURL = QByteArray::fromPercentEncoding(QRURL);
                    ret_code_url = QRURL;
                    return true;
                }
            }
        }
    }
    errstring = readall;
    if(errstring.length() == 0)
        errstring = reply->errorString();
    if(errstring.length() == 0)
        errstring = "UNKNOW ERROR";
    return false;
}

bool ccb_pay_help::getPayUrlTest(QWidget *parent, const QString &ORDERID, const QString &PAYMENT, QByteArray &ret_code_url)
{
    QString MERCHANTID = "105303000004292";
    QString POSID = "008980292";
    QString BRANCHID = "320000000";
    QString MERCHKEY = "db78d479a09597376265f30d020111";
    //
    QString errstring;

    if(ccb_pay_help::getPayUrl(MERCHANTID, POSID, BRANCHID, MERCHKEY, PAYMENT, ORDERID, ret_code_url, errstring)) {
        QMessageBox::information(parent, "", ret_code_url);
        return true;
    }
    QMessageBox::warning(parent, "", errstring);
    return false;
}

bool ccb_pay_help::orderQuery(const QString &MERCHANTID, const QString &POSID, const QString &BRANCHID, const QString &QUPWD, const QString &ORDERID, QString &errstring)
{
    QNetworkAccessManager m;
    QEventLoop loop;
    QNetworkRequest request;
    QNetworkReply *reply = 0;
    QByteArray readall;

    QString TXCODE = "410408";//固定
    ccb_pay_help_pair msg;
    ///
    //ORDERDATE与ORDERID必须有一个输入
    QString ORDERDATE;// = "20180614";
    QString BEGORDERTIME;// = "00:00:00";
    QString ENDORDERTIME;// = "23:59:59";
    QString TYPE = "0";//0支付流水  1退款流水
    QString KIND = "1";//0 未结算流水1 已结算流水
    QString STATUS = "3";//0失败1成功2不确定3全部
    QString SEL_TYPE = "3";//1页面形式、2文件返回形式 (提供TXT和XML格式文件的下载)、3 XML页面形式
    QString PAGE = "1";
    QString OPERATOR;//非必输项
    QString CHANNEL;//现值为空

    QDomDocument doc;
    bool docValid;
    connect(&m, SIGNAL(finished(QNetworkReply*)),&loop, SLOT(quit()));

    msg.clear();
    msg.append("MERCHANTID", MERCHANTID);
    msg.append("BRANCHID", BRANCHID);
    msg.append("POSID", POSID);
    msg.append("ORDERDATE", ORDERDATE);//非必输项
    msg.append("BEGORDERTIME", BEGORDERTIME);//非必输项

    msg.append("ENDORDERTIME", ENDORDERTIME);//非必输项
    msg.append("ORDERID", ORDERID);//ORDERDATE与ORDERID必须有一个输入
    msg.append("QUPWD", "");//必输项 不参加MAC计算
    msg.append("TXCODE", TXCODE);
    msg.append("TYPE", TYPE);

    msg.append("KIND", KIND);
    msg.append("STATUS", STATUS);
    msg.append("SEL_TYPE", SEL_TYPE);
    msg.append("PAGE", PAGE);
    msg.append("OPERATOR", OPERATOR);

    msg.append("CHANNEL", CHANNEL);
    //
    msg.append("MAC", QCryptographicHash::hash(msg.join().toLocal8Bit(), QCryptographicHash::Md5).toHex());
    //
    msg.update("QUPWD", QUPWD);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain;charset=UTF-8");
    //商朝的摇钱树云，中转
    request.setUrl(QString("http://115.28.155.165/qryqs/bankREST/batchQueryBankPay-rest"));
    reply = m.post(request, (QString("https://ibsbjstar.ccb.com.cn/CCBIS/ccbMain?") + msg.join()).toLocal8Bit());
    loop.exec();
    readall = reply->readAll();
    docValid = doc.setContent(readall.trimmed());
    if("200" == replyHttpCode(reply)) {
        if("1" == docTagNameValue(doc, "STATUSCODE")) {
            return true;
        }
    }
    //errstring
    errstring = "";
    if(docValid) {
        errstring +=  "HttpCode:" + replyHttpCode(reply) + "\n";
        errstring +=  "RETURN_CODE:" + docTagNameValue(doc, "RETURN_CODE") + "\n";
        errstring +=  "RETURN_MSG:" + docTagNameValue(doc, "RETURN_MSG") + "\n";
        errstring +=  "STATUSCODE:" + docTagNameValue(doc, "STATUSCODE") + "\n";
        errstring +=  "STATUS:" + docTagNameValue(doc, "STATUS");
    }
    if(errstring.length() == 0)
        errstring = readall;
    if(errstring.length() == 0)
        errstring = reply->errorString();
    if(errstring.length() == 0)
        errstring = "UNKNOW ERROR";

    return false;
}

bool ccb_pay_help::orderQueryTest(QWidget *parent, const QString &ORDERID)
{
    QString MERCHANTID = "105303000004292";
    QString POSID = "008980292";
    QString BRANCHID = "320000000";
    QString QUPWD = "888888";
    //
    QString errstring;
    if(ccb_pay_help::orderQuery(MERCHANTID, POSID, BRANCHID, QUPWD, ORDERID, errstring)) {
        QMessageBox::information(parent, "", "pay success");
        return true;
    }
//    QMessageBox::warning(parent, "", errstring);
    return false;
}

QString ccb_pay_help::docTagNameValue(QDomDocument &doc, const QString &tagName)
{
    QDomNodeList nodelist;
    nodelist = doc.elementsByTagName(tagName);
    if(nodelist.count() > 0) {
        return nodelist.at(0).toElement().text();
    }
    return "";
}

QString ccb_pay_help::replyHttpCode(QNetworkReply *reply)
{
    return reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString();
}


QString ccb_pay_help_pair::join()
{
    QString ret;
    for(int k = 0; k < str_pair_list.count(); k++) {
        ret += str_pair_list[k].first + "=" + str_pair_list[k].second + "&";
    }
    ret.chop(1);
    return ret;
}

void ccb_pay_help_pair::append(const QString &key, const QString &value)
{
    QPair<QString, QString> p;
    p.first = key;
    p.second = value;
    str_pair_list.append(p);
}

bool ccb_pay_help_pair::update(const QString &key, const QString &value)
{
    for(int k = 0; k < str_pair_list.count(); k++) {
        if(key == str_pair_list[k].first) {
            str_pair_list[k].second = value;
            return true;
        }
    }
    return false;
}

void ccb_pay_help_pair::clear()
{
    str_pair_list.clear();
}
