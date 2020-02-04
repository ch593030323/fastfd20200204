#include "w_scr_dish_main_pay_info.h"
#include "ui_fexpandmain_payover.h"

w_scr_dish_main_pay_info::w_scr_dish_main_pay_info(QWidget *parent) :
    QFrame(parent)
{
    ui = new Ui_fexpandmain_payover;
    ui->setupUi(this);
}

w_scr_dish_main_pay_info::~w_scr_dish_main_pay_info()
{
    delete ui;
}
