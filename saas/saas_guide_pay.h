#ifndef SAAS_GUIDE_PAY_H
#define SAAS_GUIDE_PAY_H

#include <QFrame>
#include "lds_bool.h"

namespace Ui {
class saas_guide_pay;
}

class saas_guide_pay : public QFrame
{
    Q_OBJECT

public:
    explicit saas_guide_pay(QWidget *parent = 0);

    ~saas_guide_pay();

    bool pay();

protected slots:
    virtual void getPurchaseInfo();
    void getUpgradeService();
private:
    void layoutUpdate();

    void layoutUpdate(const QString &saas_version);

private slots:
    void updatePayNum();

    void on_toolButton_num_key_clicked();

    void on_radioButton_extend_clicked(bool checked);

    void on_radioButton_update_clicked(bool checked);

protected:
    Ui::saas_guide_pay *ui;

    struct getSelectServiceSubtotal{
        lds_bool_default_false valid;
        QString extra_common_param;
        double totalAmount;
    } payInfo;
};

class saas_guide_pay_update : public saas_guide_pay{
    Q_OBJECT

public:
    explicit saas_guide_pay_update(QWidget *parent = 0)
        : saas_guide_pay(parent) {}

protected slots:
    //from saas_guide_pay
    virtual void getPurchaseInfo();

};

#endif // SAAS_GUIDE_PAY_H
