#ifndef PAY_HELPER_PAY_H
#define PAY_HELPER_PAY_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class pay_helper_pay;
}

class pay_helper_pay : public QDialog
{
    Q_OBJECT

public:
    explicit pay_helper_pay(const QString &fee, const QString &payment, QWidget *parent = 0);
    ~pay_helper_pay();

private slots:
    void tocheckpay();
    void tocodepay();
    void tocancel();

    void tomakeUrl();
private:
    Ui::pay_helper_pay *ui;
    const QString payment;
    const QString fee;
    QString out_trade_no_a;
    QString out_trade_no_b_a;
    QString out_trade_no_ma;
    QString out_trade_no_b_ma;
    QString out_trade_no_mw;
    QString out_trade_no_b_mw;
    QSettings *sysconf;
};

#endif // PAY_HELPER_PAY_H
