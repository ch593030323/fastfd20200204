#ifndef W_SYS_MANAGE_SQLCOPY_SOFT_LIMIT2_H
#define W_SYS_MANAGE_SQLCOPY_SOFT_LIMIT2_H

#include <QDialog>
#include <QDate>

namespace Ui {
class w_sys_manage_sqlcopy_soft_limit2;
}

class w_sys_manage_sqlcopy_soft_limit2 : public QDialog
{
    Q_OBJECT

public:
    static void limit_date(QDate &dt_limit, QDate &dt_cur);
public:
    explicit w_sys_manage_sqlcopy_soft_limit2(QWidget *parent = 0);
    ~w_sys_manage_sqlcopy_soft_limit2();

private slots:
    void took();
    void tocancel();
    void toTerminalCodeCopy();
    void topaste();
    void toread();
protected:
    Ui::w_sys_manage_sqlcopy_soft_limit2 *ui;
};

class w_sys_manage_sqlcopy_soft_limit2_for_customer : public w_sys_manage_sqlcopy_soft_limit2{
    Q_OBJECT
public:
    explicit w_sys_manage_sqlcopy_soft_limit2_for_customer(QWidget *parent = 0);

};

#endif // W_SYS_MANAGE_SQLCOPY_SOFT_LIMIT2_H
