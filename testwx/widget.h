#ifndef WIDGET_H
#define WIDGET_H

#include<QWidget>
#include<QNetworkRequest>
#include<QNetworkReply>
#include<QEventLoop>
#include<QUuid>
#include<QRegExp>
#include<QCryptographicHash>
#include<QXmlStreamReader>
#include<QMap>
#include "liberweima/erweima.h"
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::Widget *ui;
    QString getuuid();
    QNetworkAccessManager *netManager;
    QEventLoop eventLoop;
    QNetworkReply *reply;
    bool do_post(QString url, QByteArray _content,QByteArray &recv) ;

    erweima *weimawidget;

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

    }retDataStruct;

};

#endif // WIDGET_H
