#include "w_sys_manage_select.h"
#include "ui_w_sys_manage_select.h"

w_sys_manage_select::w_sys_manage_select(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_select)
{
    ui->setupUi(this);
}

w_sys_manage_select::~w_sys_manage_select()
{
    delete ui;
}
