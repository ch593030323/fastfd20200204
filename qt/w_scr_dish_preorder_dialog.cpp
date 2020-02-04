#include "w_scr_dish_preorder_dialog.h"
#include "ui_w_scr_dish_preorder_dialog.h"

w_scr_dish_preorder_Dialog::w_scr_dish_preorder_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_preorder_Dialog)
{
    ui->setupUi(this);
}

w_scr_dish_preorder_Dialog::~w_scr_dish_preorder_Dialog()
{
    delete ui;
}
