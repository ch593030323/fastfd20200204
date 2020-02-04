#include "w_scr_dish_pay_set.h"
#include "ui_w_sys_manage_outer_settings.h"
#include "lds.h"

w_scr_dish_pay_set::w_scr_dish_pay_set(QWidget *parent) :
    w_sys_manage_outer_settings(parent, 0)
{
    lds::setwflagFrameless(this);
    ui->frame->hide();
}

w_scr_dish_pay_set::~w_scr_dish_pay_set()
{
}

void w_scr_dish_pay_set::took()
{
    saveData(0, true);
    this->accept();
}
