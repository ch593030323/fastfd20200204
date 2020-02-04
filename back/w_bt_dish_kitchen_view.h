#ifndef W_BT_DISH_KITCHEN_VIEW_H
#define W_BT_DISH_KITCHEN_VIEW_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"

namespace Ui {
class w_bt_dish_kitchen_view;
}

class w_bt_dish_kitchen_view : public QDialog
{
    Q_OBJECT

public:
    explicit w_bt_dish_kitchen_view(QWidget *parent = 0);
    ~w_bt_dish_kitchen_view();

    void initView();
public:
    void refresh();
private slots:
    void torefresh();
    void treeselect();
    void toexit();
    void tokitchenplan();
    void tosave();
    void tomore();
private:
    bool saveData();
    int checkDirty();
private:
    Ui::w_bt_dish_kitchen_view *ui;
public:
    lds_model_sqltablemodel *tablemodel;
};

#endif // W_BT_DISH_KITCHEN_VIEW_H
