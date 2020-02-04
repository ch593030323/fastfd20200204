#ifndef W_SYS_MANAGE_SELECT_H
#define W_SYS_MANAGE_SELECT_H

#include <QDialog>
#include "ftableview_standmodel_sql_none.h"

namespace Ui {
class w_sys_manage_select;
}

class w_sys_manage_select : public QDialog
{
    Q_OBJECT

public:
    explicit w_sys_manage_select(QWidget *parent = 0);
    ~w_sys_manage_select();

private:
    Ui::w_sys_manage_select *ui;
    ftableview_standmodel_sql_none *cook_model;
};

#endif // W_SYS_MANAGE_SELECT_H
