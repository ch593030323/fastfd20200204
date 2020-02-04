#ifndef lds_scr_table_H
#define lds_scr_table_H

#include "lds_bool.h"
#include <QString>
#include <QVariant>
#include "lds_query_hddpos.h"
class lds_model_sqltablemodel;

enum tableState{
    tableState_NONE = 1,           //none
    tableState_KONGXIAN=2,         //空闲
    tableState_YUDING = 4,         //预订
    tableState_KAITAI=8,           //开台
    tableState_YUDA = 16,          //预打
    tableState_DISABLE = 32,       //不可选
    tableState_BAR = 64,          //酒吧
    tableState_QR_CODE = 128,          //二维码点餐
};

struct tableSTATE{
    QString key;
    tableState state;
    QString colorname1;
};

struct cell_content{
    QString showed_color;
    lds_bool_default_false ispressed;
    lds_bool_default_false ischecked;
    lds_bool_default_false isdisabled;
    lds_bool_default_false isDensePattern;
};

class lds_scr_table
{
public:
    static const  QString Q000;
    static const QVariant cey_sys_parameter_value(const QString &vch_parameter, const QVariant &defValue = QVariant());
    static const QVariant cey_sys_parameter_value(lds_query_hddpos  &query, const QString &vch_parameter, const QVariant &defValue = QVariant());
public:
    lds_scr_table();
    static QList<tableSTATE> &getTablestatelist();
    static tableSTATE gettableState(tableState state);

    //cey_u_master
    static bool get_max_ch_billno_add1_Insert_cey_u_master_Create_serial_no(const QString &k, QString &ret_ch_billno);
    static QString get_max_ch_billno_add1(const QString &k);
    static QString get_max_ch_payno_add1(const QDateTime &dt);
    static QString get_max_ch_payno_add1();

    static bool insert_cey_u_master_Create_serial_no(const QString &ch_billno, QString *errstring = 0);
    static bool insert_cey_u_master_Create_serial_no(lds_query_hddpos  &query, const QString &ch_billno);
    static QString create_serial_no(const QString &ch_billno);
    //~cey_u_master

    //cey_u_taxsale_master
    static bool cey_u_taxsale_master_insert_table(lds_query_hddpos  &query, const QString &ch_billno, QString *errstring = 0);
    static bool cey_u_taxsale_master_insert_table(lds_query_hddpos  &query, const QString &ch_billno, const lds_model_sqltablemodel *values, QString *errstring = 0);

    static void cey_u_taxsale_master_insert_model(lds_model_sqltablemodel *model, const QString &ch_billno);
    static void cey_u_taxsale_master_make_map(qrtVariantPairList &bind_value_list, const QString &ch_billno);

    static bool cey_bt_table_update(const QString &ch_tableno, const QString &ch_billno, int sqr, QString *errstring = 0);
    static bool cey_bt_table_update(lds_query_hddpos  &query, const QString &ch_tableno, const QString &ch_billno, int sqr);

    static bool cey_bt_table_lock(const QString &ch_tableno, int sqr);
    static bool cey_bt_table_unlock(const QString &ch_tableno, int sqr, QString *errstring = 0);
    static bool cey_bt_table_has_locked(const QString &ch_tableno, int sqr);
    static bool cey_bt_table_has_locked(QWidget *parent, const QString &ch_tableno, int sqr);
    static int cey_bt_table_div_sqr(const QString &ch_tableno, const QString &ch_billno);
    static QString cey_bt_table_ch_billno_get(const QString &ch_tableno, int sqr);
    static bool cey_bt_table_bar_ch_billno_existed(const QString &ch_tableno, int sqr);
    static bool cey_u_orderdish_dish_existed(const QString &ch_billno);
    //~cey_u_taxsale_master

    static QString int_rate_name(const lds_model_sqltablemodel *m, int index);//税率名称
    static double int_rate_worked(const lds_model_sqltablemodel *m, int index);

    static QString f_get_sysparm(const QString &as_parameter, const QVariant &as_default=QVariant());

public:
    static QList<tableSTATE> tablestatelist;
};


#endif // lds_scr_table_H
