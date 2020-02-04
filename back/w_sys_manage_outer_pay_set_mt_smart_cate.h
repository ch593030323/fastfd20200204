#ifndef W_SYS_MANAGE_OUTER_PAY_SET_MT_SMART_CATE_H
#define W_SYS_MANAGE_OUTER_PAY_SET_MT_SMART_CATE_H

#include <QDialog>

namespace Ui {
class w_sys_manage_outer_pay_set_mt_smart_cate;
}

class w_sys_manage_outer_pay_set_mt_smart_cate : public QDialog
{
    Q_OBJECT
public:
    explicit w_sys_manage_outer_pay_set_mt_smart_cate(QWidget *parent = 0);
    ~w_sys_manage_outer_pay_set_mt_smart_cate();

private slots:
    void took();
    void tocancel();
    void totest();
    void todemo();
private:
    Ui::w_sys_manage_outer_pay_set_mt_smart_cate *ui;
};

#endif // W_SYS_MANAGE_OUTER_PAY_SET_MT_SMART_CATE_H
