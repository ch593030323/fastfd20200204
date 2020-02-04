#include "w_scr_dish_main_table.h"
#include "ui_w_scr_dish_main_table.h"

w_scr_dish_main_table::w_scr_dish_main_table(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_main_table)
{
    ui->setupUi(this);
}

w_scr_dish_main_table::~w_scr_dish_main_table()
{
    delete ui;
}
