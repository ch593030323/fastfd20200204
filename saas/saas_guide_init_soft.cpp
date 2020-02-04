#include "saas_guide_init_soft.h"
#include "ui_saas_guide_init_soft.h"
#include "public_sql.h"
#include "w_scr_dish_pay_set.h"
#include "lds.h"
#include "saas_guide_label.h"
#include "saas_guide_screen.h"


saas_guide_init_soft::saas_guide_init_soft(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::saas_guide_init_soft)
{
    ui->setupUi(this);

    ui->pushButton_connect_MySQL->setCheckable1(true);
    ui->pushButton_demo->setCheckable1(true);
    ui->pushButton_labelprint->setCheckable1(true);
    ui->pushButton_reciept->setCheckable1(true);
    ui->pushButton_screen->setCheckable1(true);

//    ui->pushButton_connect_MySQL->setChecked1(lds::sysconf->value("saas_guide_init_soft/connect_MySQL", false).toBool());
    ui->pushButton_demo->setChecked1(lds::sysconf->value("saas_guide_init_soft/demo", false).toBool());
    ui->pushButton_labelprint->setChecked1(lds::sysconf->value("saas_guide_init_soft/labelprint", false).toBool());
    ui->pushButton_reciept->setChecked1(lds::sysconf->value("saas_guide_init_soft/reciept", false).toBool());
    ui->pushButton_screen->setChecked1(lds::sysconf->value("saas_guide_init_soft/screen", false).toBool());
}

saas_guide_init_soft::~saas_guide_init_soft()
{
    delete ui;
}

void saas_guide_init_soft::on_pushButton_connect_MySQL_clicked()
{
    //已连接
    if(ui->pushButton_connect_MySQL->isChecked1()) {
        return;
    }
    lds_messagebox_loading_show loading(0, "");
    loading.show_delay();
    bool sql_is_connected =false;
    while(false == (sql_is_connected = public_sql::CONNECT_MySQL(&loading)) ) {
        if(QDialog::Rejected == public_sql::MySQL_PARAME_SET_EXEC(this)) {
            break;
        }
    }

    if(sql_is_connected) {
        ui->pushButton_connect_MySQL->setChecked1(true);
    }
}

void saas_guide_init_soft::on_pushButton_demo_clicked()
{
    //MySQL_isConnected
    if(false == MySQL_isConnected()) {
        return;
    }
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, ui->pushButton_demo->text(), QObject::tr("确认"), QObject::tr("取消"))) {
        return;
    }
    lds_messagebox_loading_show loading(this, "");
    loading.show_delay();
    loading.setText(ui->pushButton_demo->text());
    if(!public_sql::MySQL_restore(public_sql::show_chinese_sql_file_dir + "/" + public_sql::show_chinese_sql_file)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, public_sql::MySQL_info);
        return;
    }
    ui->pushButton_demo->setChecked1(true);
}

void saas_guide_init_soft::on_pushButton_reciept_clicked()
{
    //MySQL_isConnected
    if(false == MySQL_isConnected())  return;

    if(ui->pushButton_reciept->isChecked1()) {
        return;
    }
    w_scr_dish_pay_set dialog(this);
    dialog.setWindowTitle(ui->pushButton_reciept->text());
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    dialog.move(0,0);
    if(QDialog::Accepted == dialog.exec()) {
        ui->pushButton_reciept->setChecked1(true);
    }
}

void saas_guide_init_soft::on_pushButton_labelprint_clicked()
{
    //MySQL_isConnected
    if(false == MySQL_isConnected())  return;

    if(ui->pushButton_labelprint->isChecked1()) {
        return;
    }

    saas_guide_label dialog(this);
    dialog.setWindowTitle(ui->pushButton_labelprint->text());
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    dialog.move(0,0);
    if(QDialog::Accepted == dialog.exec()) {
        ui->pushButton_labelprint->setChecked1(true);
    }
}

void saas_guide_init_soft::on_pushButton_screen_clicked()
{
    //MySQL_isConnected
    if(false == MySQL_isConnected())  return;

    //pushButton_screen
    if(ui->pushButton_screen->isChecked1()) {
        return;
    }

    saas_guide_screen dialog(this);
    dialog.setWindowTitle(ui->pushButton_screen->text());
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    dialog.move(0,0);
    if(QDialog::Accepted == dialog.exec()) {
        ui->pushButton_screen->setChecked1(true);
    }
}

void saas_guide_init_soft::on_pushButton_done_clicked()
{
    //MySQL_isConnected
    if(false == MySQL_isConnected())  return;

    lds::sysconf->setValue("saas_guide_init_soft/connect_MySQL", ui->pushButton_connect_MySQL->isChecked1());
    lds::sysconf->setValue("saas_guide_init_soft/demo", ui->pushButton_demo->isChecked1());
    lds::sysconf->setValue("saas_guide_init_soft/labelprint", ui->pushButton_labelprint->isChecked1());
    lds::sysconf->setValue("saas_guide_init_soft/reciept", ui->pushButton_reciept->isChecked1());
    lds::sysconf->setValue("saas_guide_init_soft/screen", ui->pushButton_screen->isChecked1());
}

bool saas_guide_init_soft::MySQL_isConnected()
{
    if(ui->pushButton_connect_MySQL->isChecked1()) {
        return true;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("请先连接数据库"));
    return false;
}
