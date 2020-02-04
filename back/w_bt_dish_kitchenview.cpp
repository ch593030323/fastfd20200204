#include "w_bt_dish_kitchenview.h"
#include "ui_w_bt_dish_kitchenview.h"

w_bt_dish_kitchenview::w_bt_dish_kitchenview(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_bt_dish_kitchenview)
{
    ui->setupUi(this);
}

w_bt_dish_kitchenview::~w_bt_dish_kitchenview()
{
    delete ui;
}
