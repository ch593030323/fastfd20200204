#ifndef W_SYS_MANAGE_CLOUDSYNC_WITH_TIME_H
#define W_SYS_MANAGE_CLOUDSYNC_WITH_TIME_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"
#include "lds_query_hddpos.h"
#include "cjson_help.h"
#include "lds_thread.h"
#include "w_sys_manage_cloudsync.h"
#include "lds_messagebox.h"
#include "w_sys_manage_cloudsync_cloud2.h"
#include <QEventLoop>

class Ui_w_sys_manage_cloudsync_with_time;
class Ui_w_sys_manage_cloudsync_with_time_running;


class w_sys_manage_cloudsync_with_time : public QDialog
{
    Q_OBJECT

public:
    static QString bt_trigger_update_sql_arg;
    static QString bt_trigger_delete_sql_arg;
    static QString bt_trigger_insert_sql_arg;

public:
    explicit w_sys_manage_cloudsync_with_time(QWidget *parent = 0);
    ~w_sys_manage_cloudsync_with_time();
private slots:
    void took();
    void toexit();
    void tousercheck();
    void todatekeep();
    void toupload();
    void todownload();
    void totimecheck();
    void toflowupload();
    void tosaasNot2AA();
    void toshowSaaSBtn();
    void tosaasVerfy();
private:
    void saveData();
private:
    Ui_w_sys_manage_cloudsync_with_time *ui;
};

/**
实时同步
*/
/**
head version:{"dishInfoVersion":0,"dishSubTypeVersion":0,"dishTypeVersion":0,
"dishSuitVersion":0,"dishSuitChangeVersion":0,"dishUnitVersion":0,
"dishTableVersion":0,"dishTableTypeVersion":0,"dishTableAreaVersion":0,
"dishPracticeVersion":0,"dishPracticeMemoVersion":0,"dishPracticeTypeVersion":0}
dishGuQingVersion":0,"payModeVersion

基础资料
"dishInfoList": [],
"dishSubTypeList": [],
"dishTypeList": [],
"dishSuitList": [],
"dishSuitChangeList": [],

"dishUnitList": [],
"dishTableList": [],
"dishTableTypeList": [],
"dishTableAreaList": [],
"dishPracticeList": [],

"dishPracticeMemoList": [],
"dishPracticeTypeList": []
dishGuQingList":0,"
payModeList

//    cey_bt_dish;
//    cey_bt_dish_subtype;
//    cey_bt_dish_type;
//    cey_bt_dish_suit
//    cey_bt_suit_change;

//    cey_bt_unit;
//    cey_bt_table;
//    cey_bt_table_type;
//    cey_bt_table_area;
//    cey_bt_cp_dish;

//    cey_bt_cp_memo;
//    cey_bt_cp_type;
//    cey_bt_dish_warn;
//    cey_bt_paymode;
*/
/*有回收站一说
软件将删除的数据，插入 del_，
上传时，回收站未同步数据：若是匹配原数据则上传正常数据，若不匹配上传删除数据
下载时，若删除，则直接delete，更新 del_，

修改、删除的可以实时同步，但是插入不能实时同步
*/
class w_sys_manage_cloudsync_with_time_running : public QDialog
{
    Q_OBJECT
public:
    enum cloudFlag{
        cloud_none = 0,
        cloud_down = 1,
        cloud_up = 2,
        cloud_flow_up = 4,

        cloud_downup = cloud_down|cloud_up,
    };
    Q_DECLARE_FLAGS(cloudFlags, cloudFlag)

public:
    explicit w_sys_manage_cloudsync_with_time_running(cloudFlags flag, QWidget *parent = 0);
    ~w_sys_manage_cloudsync_with_time_running();

    static bool sql_update_trigger();
    static void sql_update();
    static void sql_check();

    static QString q_bt_get_del_sql(const QString &tablename, bool skip_ch_yun_already_keep_Y = false);
    static int q_bt_check_state_and_del(QObject *parent_nozero_trigger_slot, const QString &tablename, cJSON *pItem, int sqr, QString *where_sql = 0);
    //基础资料  会员 流水 系统设置
    static bool q_bt_get_ready_total_byconf();
    static int q_bt_get_ready_total();
    static int q_m_parameter_get_ready_total();
    static int q_bt_material_num_get_ready_total();
    static int q_bt_promotion_get_ready_total();
    static bool q_bt_promotion_put_ch_yun_already_keep_Y();

    static cJSON *q_bt_get_piece_(const QString &tablename, lds_query_hddpos  &query, const QString &orgCode, int state);//-1删除、 0正常使用
    static void q_bt_put_piece_(const QString &tablename, qrtVariantPairList &key_value_list, cJSON *pItem);

    //-3数据库和获取size不匹配、-2数据库连接失败、 -1云端连接失败、 0需要匹配、 1已经匹配
    static int get_state_local_dish_base_keepdata();
    static void exec_keepdata_byconf(QWidget *parent);
    static void exec_keepdata(QWidget *parent);
    static void exec_no_up_then_down_byconf(QWidget *parent);
    static void yun_version_diff_byconf_trigger_func(QWidget *parent);//函数:void to_yun_version_diff(int d)
private:
    static void exec_keepdata_down_byconf(QWidget *parent);
public:

    //table vesion
    static cJSON *q_bt_get_table_version_array();
    static bool q_bt_put_table_version(cJSON *root);
    static bool q_bt_put_table_version(const cJSON_help &root);
    static bool q_bt_put_table_version(QSharedPointer<cJSON> root);

    static bool q_bt_put_table_cey(QObject *parent_nozero_trigger_slot, cJSON *root);
    static bool q_bt_put_table_cey(QObject *parent_nozero_trigger_slot, QSharedPointer<cJSON> root);

    static cJSON *q_bt_get_table_cey(QObject *parent_nozero_trigger_slot, bool skip_ch_yun_already_keep_Y = false);

    static bool q_bt_put_ch_yun_already_keep_Y();

    //只上传
    static bool q_st_upload_material_num(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap = QMap<QString, QVariant>());
    ///流水需要先开事务
    //    //x7
    //    SELECT ch_payno FROM cey_u_checkout_detail;
    //    SELECT ch_payno FROM cey_u_checkout_master;
    //    SELECT ch_payno FROM cey_u_orderdish;
    //    SELECT ch_payno FROM cey_u_service_low;
    //    SELECT ch_payno FROM cey_u_orderdish_log;
    //    SELECT ch_payno FROM cey_u_togo;
    //    SELECT ch_payno FROM cey_u_table;

    //    //x3
    //    SELECT int_flowID FROM t_m_deposit;
    //    SELECT int_flowID FROM t_m_point;
    //    SELECT int_flowID FROM t_m_pay;
    /*
     * x7
    SELECT ch_payno FROM cey_u_checkout_detail;
    SELECT ch_payno FROM cey_u_checkout_master;
    SELECT ch_payno FROM cey_u_orderdish;
    SELECT ch_payno FROM cey_u_service_low;
    SELECT ch_payno FROM cey_u_orderdish_log;
    SELECT ch_payno FROM cey_u_togo;
    SELECT ch_payno FROM cey_u_table;

    //x3
    SELECT int_flowID FROM t_m_deposit;
    SELECT int_flowID FROM t_m_point;
    SELECT int_flowID FROM t_m_pay;




    */
    /*
//x8
    cey_u_checkout_detail-----------------------checkOutDetail-----------int_flowID-----------codIntFlowId
    cey_u_checkout_master-----------------------checkOutMaster-----------ch_payno-----------comChPayNo
    cey_u_orderdish-----------------------orderDish-----------int_flowID-----------bdIntFlowId
    cey_u_service_low-----------------------serviceLow-----------ch_payno-----------slChPayNo
    cey_u_orderdish_log-----------------------orderDishLog-----------int_flowID-----------odlIntFlowId
    cey_u_togo-----------------------toGo-----------ch_togono-----------chTogono
    cey_u_table-----------------------tableRecordsConsumption-----------ch_billno-----------chBillno
    cey_u_master-----------------------orderMaster-----------ch_billno-----------omChBillNo
    //x3
   t_m_deposit-----------------------memberDeposit-----------int_flowID-----------mdIntFlowId
   t_m_point-----------------------memberPointList-----------int_flowID-----------mpIntFlowId
   t_m_pay-----------------------memberPay-----------int_flowID-----------intFlowid
    */
    static void query_filter_yun_keep(const QString &sql);
    static cJSON *q_u_get_void_flow_root();
    //x8
    static cJSON *q_u_get_piece_cey_u_checkout_detail(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap = QMap<QString, QVariant>());//checkOutDetail
    static cJSON *q_u_get_piece_cey_u_checkout_master(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap = QMap<QString, QVariant>());//checkOutMaster
    static cJSON *q_u_get_piece_cey_u_orderdish(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap = QMap<QString, QVariant>());//orderDish
    static cJSON *q_u_get_piece_cey_u_service_low(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap = QMap<QString, QVariant>());//serviceLow
    static cJSON *q_u_get_piece_cey_u_orderdish_log(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap = QMap<QString, QVariant>());//orderDishLog
    static cJSON *q_u_get_piece_cey_u_togo(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap = QMap<QString, QVariant>());//toGo
    static cJSON *q_u_get_piece_cey_u_table(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap = QMap<QString, QVariant>());//tableRecordsConsumption
    static cJSON *q_u_get_piece_cey_u_master(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap = QMap<QString, QVariant>());//orderMaster
    //x3
    static cJSON *q_u_get_piece_t_m_deposit(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap = QMap<QString, QVariant>());//memberDeposit
    static cJSON *q_u_get_piece_t_m_point(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap = QMap<QString, QVariant>());//memberPointList
    static cJSON *q_u_get_piece_t_m_pay(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap = QMap<QString, QVariant>());//memberPay

    static cJSON *q_u_get_piece_(const QString &tablename, lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap = QMap<QString, QVariant>());//memberPay

    static void q_u_get_by(cJSON *root, const QString &ch_payno, const QString &orgCode);
    static bool q_u_put_by(const QString &ch_payno);

    static void q_u_get_limit500_in_transaction(cJSON *root, const QString &orgCode, QString &dt_operdate);
    static void q_u_get_limit500_in_transaction(QSharedPointer<cJSON> root, const QString &orgCode, QString &dt_operdate);
    static bool q_u_put_limit1000(cJSON *root);
    static bool q_u_put_limit1000(QSharedPointer<cJSON> root);
    static bool q_u_get_yun_already_keep();

    static cJSON *q_u_get_by_ch_payno(const QString &ch_payno, const QDateTime &pay_dt, const QString &orgCode);
    static bool q_u_put_by_ch_payno(const QString &ch_payno, const QDateTime &pay_dt);

private slots:
    void took();
    void tocancel();
public:

    void data_run(cloudFlags f);
    static void base_data_upload_thread();

    void flow_data_upload();
    static bool flow_data_clearAll_Y_transaction(QWidget *parent, lds_messagebox_loading_show *loading);
    static bool flow_pay_data_upload(const QString &ch_payno, const QDateTime &pay_dt, const QString &orgCode, QString *errstring);

    bool upload_data(bool skip_ch_yun_already_keep_Y);
    bool upload_sql(QNetworkAccessManager &m, QEventLoop &loop, const QString &tablename, const QString& sql_where);

public slots:
    void updateGoing(const QString &text);
    void updateGoing(const QString &text, int step);
    void updateGoing(const QString &text, const QDateTime &dt);
    void goingFinished();
private:
    static QVariant queryValueMapFilter(lds_query_hddpos  &query, const QMap<QString, QVariant> &otherValueMap, const QString &field_name);
private:
    Ui_w_sys_manage_cloudsync_with_time_running *ui;
    cloudFlags _flag;
};

///======================================
class w_sys_manage_cloudsync_with_time_check_state_thread : public lds_thread {
    Q_OBJECT
public:
    w_sys_manage_cloudsync_with_time_check_state_thread(QObject *parent);
    ~w_sys_manage_cloudsync_with_time_check_state_thread();
protected:
    //from QThread
    virtual void run();
public:
    void emitsignalTrigger(QObject *parent);
signals:
    void signalTrigger(QObject *parent);//父类不是save_log
};
//======================================
//指定函数，判断是否有数据需要上传
class w_sys_manage_cloudsync_with_time_check_state_thread_event : public QObject {
    Q_OBJECT
public:
    w_sys_manage_cloudsync_with_time_check_state_thread_event(QObject *parent = 0);
    ~w_sys_manage_cloudsync_with_time_check_state_thread_event();
signals:
    void signal_state(int state);
public slots:
    void to_check_state(QObject *parent);
};
///～======================================


///======================================
class w_sys_manage_cloudsync_with_time_flow_upload_thread : public lds_thread {
    Q_OBJECT
public:
    w_sys_manage_cloudsync_with_time_flow_upload_thread(QObject *parent);
    ~w_sys_manage_cloudsync_with_time_flow_upload_thread();

    void manual_stop();
signals:
    void signalgoing(const QString &text);
    void signalgoingFinished();
protected:
    virtual void run();
private:
    bool isbreak;
};
//======================================


///======================================
class w_sys_manage_cloudsync_with_time_basedata_thread : public lds_thread {
    Q_OBJECT
public:
    w_sys_manage_cloudsync_with_time_basedata_thread(QObject *parent);
    ~w_sys_manage_cloudsync_with_time_basedata_thread();
    //from QThread
protected:
    virtual void run();
};
//======================================
//指定函数，判断是否有数据需要上传
class w_sys_manage_cloudsync_with_time_basedata_thread_event : public lds_thread_event {
    Q_OBJECT
public:
    w_sys_manage_cloudsync_with_time_basedata_thread_event(QObject *parent = 0)
        : lds_thread_event(parent) {}
public slots:
    //from lds_thread_event
    virtual void toExec();
};
///～======================================









#endif // W_SYS_MANAGE_CLOUDSYNC_WITH_TIME_H
