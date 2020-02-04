#ifndef LDS_QUERY_HDDPOS_H
#define LDS_QUERY_HDDPOS_H

#include "lds_query_db.h"
#define SQLITE_KITCHEN_DISH_TABLE QString("w_bt_dish_kitchen_dish_thread_event")

class lds_query_hddpos : public lds_query_db
{
public:
    static QVariant selectValue(
            lds_query_hddpos &query,
            const QString &sql,
            int i = 0, const QVariant &defValue = QVariant());
    static QVariant selectValue(
            const QString &sql,
            int i = 0, const QVariant &defValue = QVariant());
    static QVariant selectValue(
            const QString &sql, const QString &arg0,
            int i = 0, const QVariant &defValue = QVariant());
    static QDateTime selectNow();

    static bool selectHasNext(const QString &sql);

    static bool updateTable(const QString &tablename, const qrtVariantPairList &key_value_list, const QString &where_sql, QString *errstring = 0);
    static bool insertTable(const QString &tablename, const qrtVariantPairList &key_value_list, QString *errstring = 0);
    static bool deleteTable(const QString &tablename, const QString &where_sql, QString *errstring = 0);

    QString cey_sys_parameter_get(const QString &key, const QString &def = QString());
    void cey_sys_parameter_set(const QString &key, const QString &value, const QString &vch_explain = QString());
public:
    explicit lds_query_hddpos();

    static bool tran_saction();
    static bool com_mit();
    static bool roll_back();

    void recordFill(qrtVariantPairList &d);
    static void replace(qrtVariantPairList &d, const QString &field_name, const QVariant &value);
    static void remove(qrtVariantPairList &d, const QString &field_name);
protected:
    //更新的触发器
    virtual void execUpdateTrigger(QList<TablePairlistData> &list);
    //插入的触发器
    virtual void execInsertTrigger(QList<TablePairlistData> &list);
private:
    static QMap<QString, int> dbname_state_map;
    static int & get_db_state();

    void terminal_code_check(const QString &table, qrtVariantPairList &key_value_list);
    void ch_yun_already_keep_check(const QString &table, qrtVariantPairList &key_value_list);
    void interface_malayisa_hourly_pos_check(QList<lds_query_db::TablePairlistData> &list);
    void cey_u_orderdish_print_tr_check(QList<lds_query_db::TablePairlistData> &list);
};

#endif // LDS_QUERY_HDDPOS_H
