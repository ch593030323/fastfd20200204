#ifndef W_SYS_MANAGE_OPERATORRIGHT_DISH_TYPE_RIGHT_NEW_TYPE_H
#define W_SYS_MANAGE_OPERATORRIGHT_DISH_TYPE_RIGHT_NEW_TYPE_H

#include "w_rpt_report_frame.h"
class Ui_w_bt_dish_unitset_Dialog;
class w_sys_manage_operatorright_dish_type_right_new_type : public QDialog{
    Q_OBJECT
public:
    explicit w_sys_manage_operatorright_dish_type_right_new_type(const QString &vch_operid, QWidget *parent = 0);
    ~w_sys_manage_operatorright_dish_type_right_new_type();
    QStringList  get_dish_type_list();
private:
    Ui_w_bt_dish_unitset_Dialog *ui;
    report_querymodel *querymodel;
};

#endif // W_SYS_MANAGE_OPERATORRIGHT_DISH_TYPE_RIGHT_NEW_TYPE_H
