#ifndef CCB_PAY_HELP_H
#define CCB_PAY_HELP_H

#include <QObject>
#include <QDomDocument>
#include <QPair>

class QNetworkReply;
class ccb_pay_help_pair;

class ccb_pay_help : public QObject
{
    Q_OBJECT
public:
    ccb_pay_help(QObject *parent = 0);
    static bool getPayUrl(
            const QString &MERCHANTID,
            const QString &POSID,
            const QString &BRANCHID,
            const QString &MERCHKEY,
            const QString &PAYMENT, //0.01

            const QString &ORDERID,

            QByteArray &ret_code_url,
            QString &errstring
            );
    static bool getPayUrlTest(QWidget *parent, const QString &ORDERID, const QString &PAYMENT, QByteArray &ret_code_url);

    //微信、订单查询
    static bool orderQuery(
            const QString &MERCHANTID,
            const QString &POSID,
            const QString &BRANCHID,
            const QString &QUPWD,
            const QString &ORDERID,
            QString &errstring
            );
    static bool orderQueryTest(QWidget *parent, const QString &ORDERID);

    static QString docTagNameValue(QDomDocument &doc, const QString &tagName);
    static QString replyHttpCode(QNetworkReply *reply);
};

class ccb_pay_help_pair : public QObject
{
    Q_OBJECT
public:
    ccb_pay_help_pair(QObject *parent = 0) : QObject(parent) {}
    QString join();
    void append(const QString &key, const QString &value);
    bool update(const QString &key, const QString &value);
    void clear();
private:
    QList< QPair<QString, QString> >str_pair_list;
};

#endif // CCB_PAY_HELP_H
