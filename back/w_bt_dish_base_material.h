#ifndef W_BT_DISH_BASE_MATERIAL_H
#define W_BT_DISH_BASE_MATERIAL_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"
namespace Ui {
class w_bt_dish_base_material;
}

class w_bt_dish_base_material : public QDialog
{
    Q_OBJECT

public:
    explicit w_bt_dish_base_material(QWidget *parent = 0);
    ~w_bt_dish_base_material();
private slots:
    void tonew();
    void tochange();
    void todel();
    void toexit();

    void torefresh();
private:
    Ui::w_bt_dish_base_material *ui;
    lds_model_sqltablemodel *tablemodel;
};

#endif // W_BT_DISH_BASE_MATERIAL_H
