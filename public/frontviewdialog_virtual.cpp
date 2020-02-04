#include "frontviewdialog_virtual.h"
#include <QtDebug>
#include <QItemSelectionModel>
#include <QtDebug>
#include "lds_query_hddpos.h"
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "lds_messagebox.h"
#include <QProcess>
#include "n_func.h"
#include "backheader.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "public_logindialog.h"
#include "w_bt_dish_kitchenprinter_printerset.h"

frontViewDialog_virtual::frontViewDialog_virtual(QObject *parent) :
    QObject(parent)
{
}

frontViewDialog_virtual::~frontViewDialog_virtual()
{
}

void frontViewDialog_virtual::static_display_dish(const QString &dishname, double num, double price, double total)
{
    QString type = lds::sysconf->value("system_setting/kexianleixing").toString();
    switch(ClientDisplay::creat_clientdisplay_ledtypes().indexOf(type)) {
    case 3://LED-8-WIN
    case 0://LED-8
        static_display_price(price, dishname);
        break;
    case 1://VFD220
        static_display_price(price, dishname);
        break;
    case 2://DingHeng]
    {
        cJSON_path path(cJSON_Array);
        cJSON_AddItemToArray(path.path_root(), cJSON_help::mapTocJSON(ldsVariantMap("sqr", 5, "value", num, "name", "")));
        cJSON_AddItemToArray(path.path_root(), cJSON_help::mapTocJSON(ldsVariantMap("sqr", 1, "value", price, "name", "")));
        cJSON_AddItemToArray(path.path_root(), cJSON_help::mapTocJSON(ldsVariantMap("sqr", 2, "value", total, "name", "")));

        public_sql::save_login->qt_display_print_polling->emitsignalTrigger(path.toByteArray());
    }
        break;
    }
}

void frontViewDialog_virtual::static_display_num(double value)
{
    public_sql::save_login->qt_display_print_polling->emitsignalTrigger(5, value);
}

void frontViewDialog_virtual::static_display_price(double value, const QString &vch_dishname)
{
    public_sql::save_login->qt_display_print_polling->emitsignalTrigger(1, value, vch_dishname);
}

void frontViewDialog_virtual::static_display_total(double value)
{
    public_sql::save_login->qt_display_print_polling->emitsignalTrigger(2, value);
}

void frontViewDialog_virtual::static_display_cash(double value)
{
    public_sql::save_login->qt_display_print_polling->emitsignalTrigger(3, value);
}

void frontViewDialog_virtual::static_display_change(double value)
{
    public_sql::save_login->qt_display_print_polling->emitsignalTrigger(4, value);
}

void frontViewDialog_virtual::static_display_clear()
{
    if(public_sql::display_led.getType() == lds::sysconf->value("system_setting/kexianleixing").toString()) {
        public_sql::display_led.setType(lds::sysconf->value("system_setting/kexianleixing").toString());
    }
    public_sql::display_led.startShowinfo(lds::sysconf->value("system_setting/kexian_vfd_start").toString(),
                                          lds::sysconf->value("system_setting/kexianduankou").toString());
}
