#ifndef FEXPANDMAIN_MODEL_SQLTABLEMODEL_H
#define FEXPANDMAIN_MODEL_SQLTABLEMODEL_H
#include "lds_model_sqltablemodel.h"
#include <QDateTime>
#include "lds.h"
#include <QElapsedTimer>
#include "lds_thread.h"
#include "lds_query_hddpos.h"
#include <QSqlRecord>
#include "cjson_help.h"
#include "vch_print_memodata.h"
#include "w_bt_dish_back.h"

class fexpandmain_model_sqltablemodel_data;
class fexpandmain_model_sqltablemodel;
class QTableView;
class  vch_membernoData{
public:
    enum chooseFlag{
        choose_switch = 0,
        choose_local,
        choose_yun,
    };
public:
    vch_membernoData();
    void init();
    void init0(const QString &vch_memberno, const QString &vch_member, const QString &ch_member_belong_state, const QString &ch_favourable, int int_discount, const QString &ch_pricekind, const QString &ch_attribute, const QString &vch_typename);
    bool init_by_memberno(const QString &memberno, const QString &ch_member_belong_state, QString &errstring);
    QString desc();
    QString color();
    QString paymodeno();
    chooseFlag mutil_choose();
    void print();
    bool isyun() const;
    bool is_m_price() const;
    bool is_m_discount() const;
    double mprice(double org_price, double m_price) const;
    QString vch_memberno;
    QString vch_member;
    QString ch_member_belong_state;
    QString ch_favourable;//1会员价, 2固定折扣
    int int_discount;
    QString ch_pricekind;
    QString ch_attribute;//1：优惠卡、2充值卡
    QString vch_typename;
};

struct int_discountData {
    int int_discount;
    QString vch_dis_operid;
    QDateTime dt_dis_operdate;
};

class fexpandmain_keepData {
public:
    fexpandmain_keepData();

    void print() const;

    //销售金额
    double dish_total() const;

    //可以用于直接支付
    double pay_total(const fexpandmain_model_sqltablemodel_data *m) const;

    double dish_num() const;

    double pay_service(const QString &ch_billno) const ;
    QString pay_service_desc(const QString &ch_billno) const ;

    double num_rate() const;///这边会厨显1-1=-1.19209e-07的情况，以后注意

    ///变化
    double consum;//                    (num_num-num_back)*num_price + num_price_add, num_price_add)
    double discount;//                  (num_num-num_back)*num_price*(1-int_discount*0.01)
    double present;//                   if(ch_presentflag = 'Y', (num_num-num_back)*num_price, 0)
    double num;//                  num_num-num_back
    double num_service;

    double num_rate_before;//那些金额用于计算税率
    double num_rate1;
    double num_rate2;
    double num_rate3;
    double num_rate();

    double num_price_add;//num_price_add
    ///～变化

    ///固定值
    double num_pay_low;//低消
    int ch_service_mode;//4:商品消费服务率,3:餐桌定额
    double num_ration;//定额服务费
    ///～固定值

    mutable QString pay_msg;
    ///支付时生效
public:
    fexpandmain_keepData operator-(const fexpandmain_keepData &other);
    fexpandmain_keepData operator+(const fexpandmain_keepData &other);
    inline void operator+=(const fexpandmain_keepData &other) {
        *this = *this + other;
    }
    inline void operator-=(const fexpandmain_keepData &other) {
        *this = *this - other;
    }
};

class fexpandmain_model_sqltablemodel : public lds_model_sqltablemodel{
    Q_OBJECT
public:
    enum roleType{
        role_children_count = Qt::UserRole + 1,
    };
public:
    fexpandmain_model_sqltablemodel(QObject *parent = 0);
    virtual ~fexpandmain_model_sqltablemodel();

    static bool can_m_price(const fexpandmain_model_sqltablemodel *m, int row, QString *errstring = 0);
    //商品描述
    QString get_dish_desc(int row) const;

    //cey_bt_cp_memo
    QString get_vch_cp_memo_by_map(const QString &ch_no) const;
    QString get_num_add_price_by_map(const QString &ch_no) const ;

    //cey_bt_dish
    QString get_vch_dishname_by_map(const QString &ch_dishno) const;
    QString get_ch_curprice_by_map(const QString &ch_dishno) const;
    QString get_ch_discount_by_map(const QString &ch_dishno) const;
    double get_num_m_price_by_map(const QString &ch_dishno) const;
    QString get_ch_serviceflag_by_map(const QString &ch_dishno) const;
    QString get_item_flag_by_map(const QString &ch_dishno) const;
    QVariant get_cey_bt_dish_value(const QString &ch_dishno, const QString &valuename) const;

    //cey_u_taxsale_master
    QVariant get_cey_u_taxsale_master_value(const QString &ch_billno, const QString &valuename) const;
    double get_ch_dishno_rate(const QString &ch_billno, const QString &ch_dishno) const;
    QString get_cey_u_taxsale_master_desc(const QString &ch_billno);

    //cey_bt_unit
    QVariant get_cey_bt_unit_value(const QString &ch_unitno, const QString &valuename) const;
    //cey_bt_table_type
    QString get_sevice_mode_by_map(const QString &ch_tableno) const;
    double get_sevice_int_rate_by_map(const QString &ch_tableno) const;
    QVariant get_cey_bt_table_type_value(const QString &ch_typeno, const QString &valuename) const;
    //cey_bt_table
    QVariant get_cey_bt_table_value(const QString &ch_tableno, const QString &valuename) const;
    //cey_u_table
    QVariant get_cey_u_table_value(const QString &ch_billno, const QString &valuename) const;
    //cey_bt_paymode
    QVariant get_cey_bt_paymode_value(const QString &ch_paymodeno, const QString &valuename) const;
    //cey_sys_parameter
    QVariant get_cey_sys_parameter_value(const QString &vch_parameter, const QVariant &def_value = QVariant()) const;

    double num_dec(double value) const;
    double price_dec(double value) const;
    double amount_dec(double value) const;

    //判断
    bool can_discount(int row, QString *errstring = 0) const;
    bool can_m_price(int row, QString *errstring = 0) const;
    bool can_change_price(int row, QString *errstring = 0) const;
    bool can_change_num(int row, QString *errstring = 0) const;
    bool can_change_dish(int row, QString *errstring = 0) const;//落单前
    bool can_org_dish_append(int row, QString *errstring = 0) const;
    bool can_present(int row, QString *errstring = 0) const;
    bool can_cook(int row, QString *errstring = 0) const;
    bool can_cook_pop(const QString &ch_dishno)const;
    bool can_addon(int row, QString *errstring = 0) const;
    bool can_qt_Y_dish_fold(int row) const;
    bool can_qt_P_dish_fold(int row) const;
    //
    void update_num_price_add_2_P(const int &row);
    void update_num_num_Y(float num_num_P_old, const int &rowP);
    void update_vch_print_memo_Y(float num_num_old_P, const int &rowP);
    void update_vch_print_memo_N(float num_num_old_N, const int &row_N);

    //
    static bool discount_off_mode();
    //

    double pay_low(const QString &ch_tableno, int int_person) const;

    static double pay_without_low_service(int ch_tax_mode, double consum, double present, double discount, double num_rate);

    static double roundBySystem(const fexpandmain_model_sqltablemodel *cache, double value);
    double roundBySystem(double value) const;
protected slots:
    bool select();
};

#endif // FEXPANDMAIN_MODEL_SQLTABLEMODEL_H
