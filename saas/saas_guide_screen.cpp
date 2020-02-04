#include "saas_guide_screen.h"
#include "ui_w_sys_manage_outer_settings.h"
#include "lds.h"

saas_guide_screen::saas_guide_screen(QWidget *parent) :
    w_sys_manage_outer_settings(parent, 3)
{
    lds::setwflagFrameless(this);
    ui->frame->hide();
}

void saas_guide_screen::took()
{
    saveData(3, true);
    this->accept();
}
