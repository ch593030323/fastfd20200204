#include "w_scr_dish_pay_set_ogg.h"
#include "ui_w_scr_dish_pay_set_ogg.h"
#include "lds.h"
#include "public_sql.h"
#include "public_logindialog.h"
#include "lds_ogg_play.h"

w_scr_dish_pay_set_ogg::w_scr_dish_pay_set_ogg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_pay_set_ogg)
{
    ui->setupUi(this);
    connect(ui->pushButton_start, SIGNAL(clicked()), this, SLOT(tostart()));
    connect(ui->pushButton_stop, SIGNAL(clicked()), this, SLOT(tostop()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_test, SIGNAL(clicked()), this, SLOT(totest()));
}

w_scr_dish_pay_set_ogg::~w_scr_dish_pay_set_ogg()
{
    delete ui;
}

void w_scr_dish_pay_set_ogg::tostart()
{
    public_sql::save_login->ogg_polling = true;
}

void w_scr_dish_pay_set_ogg::tostop()
{
    public_sql::save_login->ogg_polling = false;
}

void w_scr_dish_pay_set_ogg::tocancel()
{
    this->reject();
}

void w_scr_dish_pay_set_ogg::totest()
{
    double v = qrand() % 100000000;
    double value = v / 100.00 + 0.11;
    public_sql::save_login->ogg_play_polling->play("userdata/settings/ogg/", value);
}
