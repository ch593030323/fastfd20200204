#include "w_sys_manage_cloudsync_flowdatadown.h"
#include "ui_w_sys_manage_cloudsync_flowdatadown.h"

w_sys_manage_cloudsync_flowdatadown::w_sys_manage_cloudsync_flowdatadown(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_cloudsync_flowdatadown)
{
    ui->setupUi(this);
}

w_sys_manage_cloudsync_flowdatadown::~w_sys_manage_cloudsync_flowdatadown()
{
    delete ui;
}
