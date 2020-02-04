#ifndef BANK_CARD_H
#define BANK_CARD_H

#include <QDialog>
//其他收银操作
namespace Ui {
class bank_card;
}

class bank_card : public QDialog
{
    Q_OBJECT
    
public:
    explicit bank_card(const QString &ch_paymodeno, double num_pay, double num_cost, QWidget *parent);
    ~bank_card();
    void init_control();

    double getRealPay();
    QString getVoucher();
    QString getMemo();

private slots:
    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void todelayfocus();
    void on_pushButton_online_coupon_clicked();

private:
    Ui::bank_card *ui;
    double num_pay;
    double num_cost;
    QString ch_paymodeno;
};

#endif // BANK_CARD_H
