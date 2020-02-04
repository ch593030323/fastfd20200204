#ifndef PUBLIC_LOGINDIALOG_H
#define PUBLIC_LOGINDIALOG_H

#include <QDialog>
#include <QDateTime>
#include <QTimer>
#include "lds_dialog_drog.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "w_sys_manage_wirelessordermonitoring.h"
#include<QPointer>
#include "public_sql.h"

class fast_order;
class lds_messagebox_loading;
class Ui_login_Dialog;
class w_scr_dish_restaurant_dialog;
class QProcess;
class YUN_POLLING_thread;
class w_scr_dish_TECH_TRANS_thread;
class w_bt_dish_kitchen_dish_thread;
class WX_POLLING_thread;
class w_bt_dish_kitchen_dish_print_state_thread;
class business_data_upload_isoft_thread;
class w_bt_dish_Kitchenprinter_printer_thread;
class w_scr_dish_main_view;
class w_bt_dish_Kitchenprinter_display_thread;
class lds_messagebox_loading_show;
class TelBox_thread;
class w_scr_dish_liaoning_inventory_keep_thread;
class lds_ogg_play_thread;
class w_sys_manage_outer_pay_set_fengze_thread;
class business_data_upload_restful_thread;
class business_interface_malayisa_hourly_pos_thread;

class public_loginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit public_loginDialog(QWidget *parent = 0);
    ~public_loginDialog();
    void setversion(const QString &text);
    void init(const QString &u="", const QString &p="");
    void connectdb_init(lds_messagebox_loading_show *loading);
    bool connectdb_init_check(lds_messagebox_loading_show *loading);

#ifdef Q_OS_WIN
protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
private:
   w_sys_manage_wirelessordermonitoring_wx wx_OS_WIN;
#endif
#ifdef LINPOS_OS7
   void resizeEvent(QResizeEvent *e);
#endif
   void threadInit();
public slots:
    void goto_back();
    void goto_pre();
    bool goto_prerestaurant(const QString &ch_tableno, const QString &ch_billno);

    void hideall_and_back_login(const QString &p, const QString &u);
    void hideall_and_back_login();
    void goto_close();

private slots:
    void togoto_back();//通过登录来验证
    void togoto_pre();//通过登录来验证
    void todbset();//数据库设置
    void toversionCheck();
    void toupload_polling();

    void to_cp_SoftPos();
    void tousr2pwd();
signals:
    void signal_w_sys_manage_cloudsync_basedatadown();
    void signal_everytime_to_print_failed_kitchen_bill();

    void signal_kitchen_print_state(const QString &int_id, uchar state);
    void signal_kitchen_print_error(const QString &int_id);

    void signal_TelNoComing(const QString &telNo);
    void signal_telNoComingState(const QString &telNoState);
public slots:
    void toTelNoComing(const QString &telNo);
    void showprinterr(const QString &errstring, const QString &title, int error_id = public_sql::error_no, const QString &int_print_id = "");

    QWidget* getcur_validparent();
    void to_sql_heart_beat();
    /*厨打*/
public slots:
    void to_everytime_to_print_failed_kitchen_bill();//间断时间补打漏打单
private slots:
    void set_num();
    QString toTryLogin();//验证登录信息
    void updateinfo(const QString &errinfo);
    void toprint_activesize();
    void toupdatelogo();
private:
    Ui_login_Dialog *ui;
    int info_sqr;
    QTimer *sql_heart_beat;
    bool sql_isopen;
    //数据库测试
public:
    WX_POLLING_thread *wxpolling;
    w_scr_dish_TECH_TRANS_thread *ttpolling;
    w_bt_dish_kitchen_dish_thread *kitchenpolling;
    w_bt_dish_kitchen_dish_print_state_thread *kitchen_print_polling;
    w_sys_manage_cloudsync_with_time_check_state_thread *yun_version_diff_polling;
    w_sys_manage_cloudsync_with_time_flow_upload_thread *yun_flow_upload_polling;
    business_data_upload_isoft_thread *isoft_upload_polling;
    w_sys_manage_outer_pay_set_fengze_thread *fengze_upload_polling;
    business_data_upload_restful_thread *restful_upload_polling;
    w_scr_dish_liaoning_inventory_keep_thread *liaoning_inventory_keep_polling;
    w_sys_manage_cloudsync_with_time_basedata_thread *keepdata_time_basedata_polling;
    w_bt_dish_Kitchenprinter_printer_thread *qt_recipt_print_polling;
    w_bt_dish_Kitchenprinter_display_thread *qt_display_print_polling;
    TelBox_thread *tel_box_polling;
    lds_ogg_play_thread *ogg_play_polling;
    business_interface_malayisa_hourly_pos_thread *api_malaysia_hourly_polling;
signals:
    void signal_isoft_upload_polling_upload(const QString &filename);
    void signal_isoft_upload_polling_finished(const QString &success_status);
private:
    void upload_ftp_business(lds_messagebox_loading_show *loading);
    void upload_xiexin_business(lds_messagebox_loading_show *loading);


    ///本地的轮询
public:
    void runStart();
    //厨打5、数据库连接30、中餐刷新10、微信点餐15
    lds_bool_default_false restaurantrun;
    lds_bool_default_false heartrun;
    lds_bool_default_false ogg_polling;


    int runRestaurantTimeout;
    int runKitchenTimeout;
    int runHeartTimeout;
    int runTelBoxTimeout;
    //厨打5、数据库连接30、中餐刷新10、微信点餐15
signals:
    void signalRestaurantUpdate(int step, int total);
    void signalKitchenUpdate();
    void signalTelboxUpdate();//心跳
    void signalTimeout_1s();
public slots:
    void runFrameupdate();
    void restrant_auto_lock_unique();

private:
    int runstep;
    QTimer *runtimer;
    ///~本地的轮询
};

#endif // PUBLIC_LOGINDIALOG_H
