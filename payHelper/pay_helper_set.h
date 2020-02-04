#ifndef PAY_HELPER_SET_H
#define PAY_HELPER_SET_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class pay_helper_set;
}

class pay_helper_set : public QDialog
{
    Q_OBJECT

public:
    explicit pay_helper_set(QWidget *parent = 0);
    ~pay_helper_set();

private slots:
    void toalipay_demo();
    void toalipay_test();
    void tomobilepay_alipay_get_appkey();
    void tomobilepay_demo();
    void tomobilepay_alipay_get_publickey();
    void tomobilepay_test();
    void took();
    void tocancel();
    void toalipay_get_privatepath();
private:
    Ui::pay_helper_set *ui;
    QSettings *sysconf;
};

#endif // PAY_HELPER_SET_H
