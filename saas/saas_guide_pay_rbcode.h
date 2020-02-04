#ifndef SAAS_GUIDE_PAY_RBCODE_H
#define SAAS_GUIDE_PAY_RBCODE_H

#include <QDialog>
#include <QTimer>
#include "alipay_help.h"

namespace Ui {
class saas_guide_pay_rbcode;
}

class saas_guide_pay_rbcode : public QDialog
{
    Q_OBJECT

public:
    explicit saas_guide_pay_rbcode(double fee, QWidget *parent = 0);
    ~saas_guide_pay_rbcode();

    void generRbCode();
    QString out_trade_no;
private slots:
    void check_pay();
private:
    Ui::saas_guide_pay_rbcode *ui;
    double fee;
    QTimer *timer;
    alipay_help::alipay_public_request_argument arg0;
};

#endif // SAAS_GUIDE_PAY_RBCODE_H
