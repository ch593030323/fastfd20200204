#ifndef W_BT_DISH_SUIT_H
#define W_BT_DISH_SUIT_H

//商品套菜资料
#include <QDialog>
#include "lds_model_sqltablemodel.h"
#include "lds_query_hddpos.h"
#include "w_bt_dish.h"

class Ui_w_bt_dish_suit_Dialog;
class lds_model_sqlstandardmodel;

class w_bt_dish_suit : public QDialog
{
    Q_OBJECT
public:
    explicit w_bt_dish_suit(QWidget *parent = 0);

public slots:
    void toRefresh();//刷新
    void toUpdateDishSuit(const QModelIndex &index);
    void toUpdateSuitChange(const QModelIndex &index);
    void toNewDishSuit();
    void toDelDishSuit();
    void toNewSuitChange();
    void toDelSuitChange();
    void toExit();

    void tosuitnew();
    void tosuitdel();

public:
    Ui_w_bt_dish_suit_Dialog *ui;
    lds_model_sqlstandardmodel *model_bt_dish;
    lds_model_sqlstandardmodel *model_dish_suit;
    lds_model_sqlstandardmodel *model_suit_change;
};

#endif // W_BT_DISH_SUIT_H
