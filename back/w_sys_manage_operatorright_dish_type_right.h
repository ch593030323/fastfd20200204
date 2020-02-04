#ifndef W_SYS_MANAGE_OPERATORRIGHT_DISH_TYPE_RIGHT_H
#define W_SYS_MANAGE_OPERATORRIGHT_DISH_TYPE_RIGHT_H

#include "w_rpt_report_frame.h"

//大类权限
class w_sys_manage_operatorright_dish_type_right : public QDialog{
    Q_OBJECT
public:
    explicit w_sys_manage_operatorright_dish_type_right(const QString &vch_operid, QWidget *parent = 0);
    ~w_sys_manage_operatorright_dish_type_right();
private slots:
    void tonew();
    void todel();
    void took();
    void tocancel();
    void refresh();
private:
    lds_tableView *tableview;
    lds_ComboBox *combobox;
    lds_toolbar_button *pushbutton_new;
    lds_toolbar_button *pushbutton_del;
    QStandardItemModel *standmodel;
    QLabel *label_desc;
};

#endif // W_SYS_MANAGE_OPERATORRIGHT_DISH_TYPE_RIGHT_H
