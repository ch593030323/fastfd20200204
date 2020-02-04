#ifndef W_SYS_MANAGE_OUTER_BILL_TXT_SET_H
#define W_SYS_MANAGE_OUTER_BILL_TXT_SET_H

#include <QDialog>

namespace Ui {
class w_sys_manage_outer_bill_txt_set;
}

class w_sys_manage_outer_bill_txt_set : public QDialog
{
    Q_OBJECT

public:
    explicit w_sys_manage_outer_bill_txt_set(QWidget *parent = 0);
    ~w_sys_manage_outer_bill_txt_set();

private slots:
    void toexit();
    void tosave();
    void toopen();

private:
    Ui::w_sys_manage_outer_bill_txt_set *ui;
    QString filepath;
};

#endif // W_SYS_MANAGE_OUTER_BILL_TXT_SET_H
