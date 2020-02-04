#include "saas_guide_label.h"
#include "ui_w_sys_manage_outer_settings.h"
#include "lds.h"

saas_guide_label::saas_guide_label(QWidget *parent) :
    w_sys_manage_outer_settings(parent, 2)
{
    lds::setwflagFrameless(this);
    ui->frame->hide();
}

void saas_guide_label::took()
{
    saveData(2, true);
    this->accept();
}
