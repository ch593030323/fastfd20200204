#include "w_scr_dish_main_qr_code_over_view_set.h"
#include "ui_w_scr_dish_main_qr_code_over_view_set.h"
#include "lds.h"

w_scr_dish_main_qr_code_over_view_set::w_scr_dish_main_qr_code_over_view_set(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_main_qr_code_over_view_set)
{
    ui->setupUi(this);
    ui->checkBox_auto_refresh->setChecked(lds::sysconf->value("w_scr_dish_main_qr_code_over_view_set/auto_refresh", false).toBool());
    ui->spinBox->setValue(lds::sysconf->value("w_scr_dish_main_qr_code_over_view_set/during_refresh", 10).toInt());
    ui->checkBox_auto_accept->setChecked(lds::sysconf->value("w_scr_dish_main_qr_code_over_view_set/auto_accept", false).toBool());
//    ui->checkBox_auto_paid->setChecked(lds::sysconf->value("w_scr_dish_main_qr_code_over_view_set/auto_paid", false).toBool());


    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->checkBox_auto_refresh, SIGNAL(clicked(bool)), this, SLOT(toupdate(bool)));
}

w_scr_dish_main_qr_code_over_view_set::~w_scr_dish_main_qr_code_over_view_set()
{
    delete ui;
}

void w_scr_dish_main_qr_code_over_view_set::tocancel()
{
    this->reject();
}

void w_scr_dish_main_qr_code_over_view_set::took()
{
    lds::sysconf->setValue("w_scr_dish_main_qr_code_over_view_set/auto_refresh", ui->checkBox_auto_refresh->isChecked());
    lds::sysconf->setValue("w_scr_dish_main_qr_code_over_view_set/during_refresh", ui->spinBox->value());
    lds::sysconf->setValue("w_scr_dish_main_qr_code_over_view_set/auto_accept", ui->checkBox_auto_accept->isChecked());
//    lds::sysconf->setValue("w_scr_dish_main_qr_code_over_view_set/auto_paid", ui->checkBox_auto_paid->isChecked());
    this->accept();
}

void w_scr_dish_main_qr_code_over_view_set::toupdate(bool checked)
{
    ui->checkBox_auto_accept->setEnabled(checked);
//    ui->checkBox_auto_paid->setEnabled(checked);
    if(false == checked) {
        ui->checkBox_auto_accept->setChecked(false);
//        ui->checkBox_auto_paid->setChecked(false);
    }
}
