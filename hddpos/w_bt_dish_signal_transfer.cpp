#include "w_bt_dish_signal_transfer.h"
#include <QtDebug>
QObject *w_bt_dish_signal_transfer::wxpolling = 0;
QObject *w_bt_dish_signal_transfer::ttpolling = 0;
QObject *w_bt_dish_signal_transfer::kitchenpolling = 0;
QObject *w_bt_dish_signal_transfer::kitchen_print_polling = 0;
QObject *w_bt_dish_signal_transfer::yun_version_diff_polling = 0;
QObject *w_bt_dish_signal_transfer::yun_flow_upload_polling = 0;
QObject *w_bt_dish_signal_transfer::isoft_upload_polling = 0;
QObject *w_bt_dish_signal_transfer::fengze_upload_polling = 0;
QObject *w_bt_dish_signal_transfer::restful_upload_polling = 0;
QObject *w_bt_dish_signal_transfer::liaoning_inventory_keep_polling = 0;
QObject *w_bt_dish_signal_transfer::keepdata_time_basedata_polling = 0;
QObject *w_bt_dish_signal_transfer::qt_recipt_print_polling = 0;
QObject *w_bt_dish_signal_transfer::qt_display_print_polling = 0;
QObject *w_bt_dish_signal_transfer::tel_box_polling = 0;
QObject *w_bt_dish_signal_transfer::ogg_play_polling = 0;
QObject *w_bt_dish_signal_transfer::api_malaysia_hourly_polling = 0;

void w_bt_dish_signal_transfer::check()
{
    if(wxpolling == 0) { qDebug() << __FILE__ << __LINE__ ; QObject *o = 0; o->children();}
    if(ttpolling == 0) { qDebug() << __FILE__ << __LINE__ ; QObject *o = 0; o->children();}
    if(kitchenpolling == 0) { qDebug() << __FILE__ << __LINE__ ; QObject *o = 0; o->children();}
    if(kitchen_print_polling == 0) { qDebug() << __FILE__ << __LINE__ ; QObject *o = 0; o->children();}
    if(yun_version_diff_polling == 0) { qDebug() << __FILE__ << __LINE__ ; QObject *o = 0; o->children();}
    if(yun_flow_upload_polling == 0) { qDebug() << __FILE__ << __LINE__ ; QObject *o = 0; o->children();}
    if(isoft_upload_polling == 0) { qDebug() << __FILE__ << __LINE__ ; QObject *o = 0; o->children();}
    if(fengze_upload_polling == 0) { qDebug() << __FILE__ << __LINE__ ; QObject *o = 0; o->children();}
    if(restful_upload_polling == 0) { qDebug() << __FILE__ << __LINE__ ; QObject *o = 0; o->children();}
    if(liaoning_inventory_keep_polling == 0) { qDebug() << __FILE__ << __LINE__ ; QObject *o = 0; o->children();}
    if(keepdata_time_basedata_polling == 0) { qDebug() << __FILE__ << __LINE__ ; QObject *o = 0; o->children();}
    if(qt_recipt_print_polling == 0) { qDebug() << __FILE__ << __LINE__ ; QObject *o = 0; o->children();}
    if(qt_display_print_polling == 0) { qDebug() << __FILE__ << __LINE__ ; QObject *o = 0; o->children();}
    if(tel_box_polling == 0) { qDebug() << __FILE__ << __LINE__ ; QObject *o = 0; o->children();}
    if(ogg_play_polling == 0) { qDebug() << __FILE__ << __LINE__ ; QObject *o = 0; o->children();}
    if(api_malaysia_hourly_polling == 0) { qDebug() << __FILE__ << __LINE__ ; QObject *o = 0; o->children();}
}

w_bt_dish_signal_transfer::w_bt_dish_signal_transfer(QObject *parent) :
    QObject(parent)
{
}
