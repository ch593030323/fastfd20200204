#include "w_scr_dish_restaurant_parameter.h"
#include "ui_w_scr_dish_restaurant_parameter.h"
#include "lds.h"
#include "lds_messagebox.h"
#include "w_sys_manage_outer_pay_set.h"

w_scr_dish_restaurant_parameter::w_scr_dish_restaurant_parameter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_restaurant_parameter)
{
    ui->setupUi(this);
    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_save, SIGNAL(clicked()),this,SLOT(tosave()));

    ui->label->setText(QObject::tr("显示模式"));
    ui->comboBox_map_mode->addItem(QObject::tr("显示餐桌编号"), "show_table_code");
    ui->comboBox_map_mode->addItem(QObject::tr("显示餐桌名称"), "show_table_name");
    ui->comboBox_map_mode->addItem(QObject::tr("显示餐桌编号") + "+" + QObject::tr("显示餐桌名称"), "show_table_code_name");

    old_map_show = lds::sysconf->value("w_scr_dish_restaurant_parameter/map_show", "show_table_name").toString();
    ui->comboBox_map_mode->setCurrentIndex(ui->comboBox_map_mode->findData(old_map_show));
}

w_scr_dish_restaurant_parameter::~w_scr_dish_restaurant_parameter()
{
    delete ui;
}

bool w_scr_dish_restaurant_parameter::map_showChanged()
{
    return old_map_show != lds::sysconf->value("w_scr_dish_restaurant_parameter/map_show", "show_table_name").toString();
}

void w_scr_dish_restaurant_parameter::tosave()
{
    lds::sysconf->setValue("w_scr_dish_restaurant_parameter/map_show", ui->comboBox_map_mode->itemData(ui->comboBox_map_mode->currentIndex()).toString());
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
    this->accept();
}

void w_scr_dish_restaurant_parameter::toexit()
{
    this->reject();
}
