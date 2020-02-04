#ifndef W_SCR_DISH_CUICAIPIECE_CHOOSE_H
#define W_SCR_DISH_CUICAIPIECE_CHOOSE_H

#include <QDialog>
#include "w_bt_dish.h"

class lds_model_sqltablemodel;
class Ui_w_bt_dish_unitset_Dialog;
class w_scr_dish_cuicaipiece_choose : public QDialog
{
    Q_OBJECT
public:
    explicit w_scr_dish_cuicaipiece_choose(const QString &ch_billno, w_bt_dish::kitchenState kstate, QWidget *parent);
    ~w_scr_dish_cuicaipiece_choose();
private slots:
    void toall();
    void tonoall();
    void took();
    void tocanecel();
    void torefresh();
private:
     Ui_w_bt_dish_unitset_Dialog *ui;
     lds_model_sqltablemodel *tablemodel;
     QString _ch_billno;
     w_bt_dish::kitchenState _kstate;

};

#endif // W_SCR_DISH_CUICAIPIECE_CHOOSE_H
