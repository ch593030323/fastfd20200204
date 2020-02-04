#ifndef W_BT_DISH_KITCHEN_DISH_STATE_H
#define W_BT_DISH_KITCHEN_DISH_STATE_H

#include <QDialog>
#include <QStandardItemModel>
#include "lds_model_sqltablemodel.h"

namespace Ui {
class w_bt_dish_kitchen_dish_state;
}

class w_bt_dish_kitchen_dish_state : public QDialog
{
    Q_OBJECT

public:
    explicit w_bt_dish_kitchen_dish_state(QWidget *parent = 0);
    ~w_bt_dish_kitchen_dish_state();

    static bool kitchen_data_adjust_dbtrans();
private slots:
    void toexit();
    void torefresh();
    void to_kitchen_print();
    void tolostclear();
private:
    QString get_state_desc(const QByteArray &state, int index);
private:
    Ui::w_bt_dish_kitchen_dish_state *ui;
    lds_model_sqltablemodel *tablemodel;
    int pageinfo_curstep;
};

#endif // W_BT_DISH_KITCHEN_DISH_STATE_H
