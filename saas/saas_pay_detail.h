#ifndef SAAS_PAY_DETAIL_H
#define SAAS_PAY_DETAIL_H

#include <QDialog>
#include <QStandardItemModel>
#include "networkhttprequest.h"
#include "cjson_help.h"
#include "lds_messagebox.h"
#include "lds_network.h"

namespace Ui {
class saas_pay_detail;
}

class saas_pay_detail : public QDialog
{
    Q_OBJECT

public:
    explicit saas_pay_detail(QWidget *parent = 0);
    ~saas_pay_detail();

    static bool isSaaSAA();//普通版
    static bool isSaaSAA21();//专业版
    static bool isSaaSFA20();//saas标准版
    static bool isSaaSFA21();//saas专业版

    static bool isSaaS();
    static bool btnlanguageswicthCan();
    static void mobile_connect_max_value_set_zero();
    static bool SaaSFuncCheck(QWidget *parent, const QString &code);
    static QString terminalTypeGet();
    static QString terminalSytemGet();
    static const QMap<QString, QString> &codeSaaSMapGet();
    static QString saasDesc();
    static bool saaslimitcheck(QWidget *parent = 0);

    static QStandardItemModel *saasModel();

    static QVariant saasValue(const QString &key, const QVariant &defaultValue = QVariant());
    static void saasSetValue(const QString &key, const QVariant &value);

    static int saas_lan_num();
    static QString saas_type();
    static QDateTime saas_dueDate();
    static QString saas_dueDate_str();
    static QDateTime saas_online_last_run();

    static QString appName();

    static bool saas_update_adv(const QString &dir, lds_messagebox_loading_show *loading);

    static void paintLeftTop(const QRect &rect0, QPainter *painter);
    static void paintLeftTopAA(const QRect &rect0, QPainter *painter);
    static void signWidgetUpdate();
private slots:
    void took();
    void toexit();
private:
    Ui::saas_pay_detail *ui;
    static int signWidgetLight;
    //
};
#endif // SAAS_PAY_DETAIL_H
