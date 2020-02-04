#ifndef W_SYS_MANAGE_OUTER_PAY_SET_FENGZE_H
#define W_SYS_MANAGE_OUTER_PAY_SET_FENGZE_H

#include <QDialog>
#include "lds_thread.h"
#include "lds_bool.h"
#include <QLabel>

namespace Ui {
class w_sys_manage_outer_pay_set_fengze;
}

class w_sys_manage_outer_pay_set_fengze : public QDialog
{
    Q_OBJECT
public:
    static void uploadBill();
    static void genrateBill(const QString &ch_payno, double dish_num, double dish_total);

public:
    explicit w_sys_manage_outer_pay_set_fengze(QWidget *parent = 0);
    ~w_sys_manage_outer_pay_set_fengze();

private slots:
    void took();
    void tocancel();
    void lookError();
    void test();
private:
    Ui::w_sys_manage_outer_pay_set_fengze *ui;
};

class w_sys_manage_outer_pay_set_fengze_thread : public lds_thread{
    Q_OBJECT
public:
    w_sys_manage_outer_pay_set_fengze_thread(QObject *parent):lds_thread("fengze", parent) {}
protected:
    //from QThread
    virtual void run();
};

class w_sys_manage_outer_pay_set_fengze_thread_event : public lds_thread_event {
    Q_OBJECT
public:
    w_sys_manage_outer_pay_set_fengze_thread_event(QObject *parent = 0)
        : lds_thread_event(parent) {}
public slots:
    //from lds_thread_event
    virtual void toExec();
};



#endif // W_SYS_MANAGE_OUTER_PAY_SET_FENGZE_H
