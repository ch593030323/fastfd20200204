#ifndef W_SCR_DISH_PHONE_PAY_H
#define W_SCR_DISH_PHONE_PAY_H

#include <QDialog>
#include <QTimer>
#include <QMutex>
#include "alipay_help.h"
#include "testwx_help.h"
#include "lds.h"
#include "lds_dd.h"

namespace Ui {
class w_scr_dish_phone_pay;
}

class w_scr_dish_phone_pay : public QDialog
{
    Q_OBJECT
public:
    static const QString tradeQRCode;
    static const QString tradeBarCode;
public:
    explicit w_scr_dish_phone_pay(const QString &ch_paymodeno,
                                  double fee,
                                  QWidget *parent = 0);
    ~w_scr_dish_phone_pay();
    QString ret_out_trade_no;
    QString ret_pay_name;

    static bool getInputPayFee(double &ret_fee, const QString &ch_paymodeno, double num_pay, QWidget *parent);
    static QString title(const QString &ch_paymodeno);
    static bool isPayRecord(const QString &ch_paymodeno);
private slots:
    void makeUrl();
    void toDDexit();
    void toDDaccept();
    void topolling();
    void tocodePay();

    void toprint();
    void to_save_audio(bool f);

    void tocheckOk();
private:
    QString get_out_trade_no(const QString &type);

    alipay_help::alipay_public_request_argument get_alipay_arg0(const QString &app_id, const QString &rsa_private_key_path);
private:
    Ui::w_scr_dish_phone_pay *ui;
    const QString ch_paymodeno;
    const double fee;
    QTimer *timer;

    lds::DOUBLE_DATA rb_ddata;

    QMap<QString, QString> type_out_trade_no_map;

    QVariant vch_arg1;
    QVariant vch_arg2;
    QVariant vch_arg3;
    QVariant vch_arg5;
    QVariant vch_arg6;
    QVariant vch_arg7;

    int queryCount;
};

#endif // W_SCR_DISH_PHONE_PAY_H
