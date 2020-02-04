#ifndef W_SYS_MANAGE_OUTER_PAY_SET_SQL_H
#define W_SYS_MANAGE_OUTER_PAY_SET_SQL_H

#include <QDialog>

namespace Ui {
class w_sys_manage_outer_pay_set_sql;
}

class w_sys_manage_outer_pay_set_sql : public QDialog
{
    Q_OBJECT

public:
    explicit w_sys_manage_outer_pay_set_sql(QWidget *parent = 0);
    ~w_sys_manage_outer_pay_set_sql();

private slots:
    void took();
    void tocancel();
    void updateEdit(int index);
    void on_pushButton_sql_run_clicked();

private:
    Ui::w_sys_manage_outer_pay_set_sql *ui;
};

#endif // W_SYS_MANAGE_OUTER_PAY_SET_SQL_H
