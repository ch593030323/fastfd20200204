#ifndef W_SCR_DISH_RESTAURANT_PAYSOMEDISH_H
#define W_SCR_DISH_RESTAURANT_PAYSOMEDISH_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"
#include "fexpandmain_2.h"

class Ui_w_bt_dish_unitset_Dialog;

class w_scr_dish_restaurant_paysomedish : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_restaurant_paysomedish(QWidget *parent = 0);
    ~w_scr_dish_restaurant_paysomedish();
    Ui_w_bt_dish_unitset_Dialog *ui;
private slots:
    void topaysomedish_preprint();
};

#endif // W_SCR_DISH_RESTAURANT_PAYSOMEDISH_H
