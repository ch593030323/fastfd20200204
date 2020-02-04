#include "lds_query_hddpos.h"
#include "lds_thread.h"
#include "lds.h"
#include <QtDebug>
#include <QApplication>
#include <QSqlError>
#include <QMap>
#include <QSqlRecord>
#include <QSqlField>
#include "lds_query_thread.h"


QMap<QString, int> lds_query_hddpos::dbname_state_map;

void lds_query_hddpos::interface_malayisa_hourly_pos_check(QList<lds_query_db::TablePairlistData> &list)
{
    const lds_query_db::TablePairlistData *seek_data = 0;
    for(int k = 0; k < list.count(); k ++) {
        const lds_query_db::TablePairlistData &d = list[k];
        if(d.table.toLower().trimmed() == "cey_u_checkout_master") {
            seek_data = &list[k];
            break;
        }
    }
    if(seek_data == 0)
        return;

    int index = qrtVariant::pairListIndex(seek_data->list, "dt_operdate");
    if(index < 0)
        return;
    QDate dt_operdate = seek_data->list[index].second.getValue().toDate();
    //no exist (interface_malayisa_hourly_pos.dt_operdate)
    if(!lds_query_hddpos::selectHasNext(QString(" select 1 from interface_malayisa_hourly_pos where dt_operdate = '%1' ").arg(dt_operdate.toString(yyyyMMdd)))) {
        int max_int_batchid;
        int day_off;
        if(lds_query_hddpos::selectValue(" select count(0) from interface_malayisa_hourly_pos ").toInt() == 0) {
            day_off = lds_query_hddpos::selectValue(" select ifnull(min(dt_operdate), now()) from cey_u_checkout_master").toDate().daysTo(dt_operdate) + 1;
            max_int_batchid = 0;
        } else {
            day_off = lds_query_hddpos::selectValue(" select max(dt_operdate) from interface_malayisa_hourly_pos ").toDate().daysTo(dt_operdate);
            max_int_batchid = lds_query_hddpos::selectValue(" select max(int_batchid) from interface_malayisa_hourly_pos ").toInt();
        }
        for(;day_off > 0; day_off --) {
            max_int_batchid ++;
            qrtVariantPairList binds;
            binds << qrtVariantPair("int_batchid",   max_int_batchid);
            binds << qrtVariantPair("dt_operdate", dt_operdate.addDays(-(day_off - 1)));
            binds << qrtVariantPair("ch_state", "N");
            list << (lds_query_db::TablePairlistData){1, "interface_malayisa_hourly_pos", binds, ""};
        }
        return;
    }

    //insert and Y = interface_malayisa_hourly_pos.ch_state
    if(1 == seek_data->type && "Y" == lds_query_hddpos::selectValue(QString(" select ch_state from interface_malayisa_hourly_pos where dt_operdate = '%1' ").arg(dt_operdate.toString(yyyyMMdd))).toString()) {
        qrtVariantPairList binds;
        binds << qrtVariantPair("ch_state", "N");
        list << (lds_query_db::TablePairlistData){0, "interface_malayisa_hourly_pos", binds, QString(" dt_operdate = '%1' ").arg(dt_operdate.toString(yyyyMMdd))};
        return;
    }
    //update and P = cey_u_checkout_master.ch_state
    if(0 == seek_data->type && "P" == qrtVariant::pairListValue(seek_data->list, "ch_state").toString()) {
        qrtVariantPairList binds;
        binds << qrtVariantPair("ch_state", "N");
        list << (lds_query_db::TablePairlistData){0, "interface_malayisa_hourly_pos", binds, QString(" dt_operdate = '%1' ").arg(dt_operdate.toString(yyyyMMdd))};
        return;
    }
}

void lds_query_hddpos::cey_u_orderdish_print_tr_check(QList<lds_query_db::TablePairlistData> &list)
{
    const lds_query_db::TablePairlistData *seek_data = 0;
    for(int k = 0; k < list.count(); k ++) {
        const lds_query_db::TablePairlistData &d = list[k];
        if(d.table.toLower().trimmed() == "cey_u_orderdish_print_tr") {
            seek_data = &list[k];
            break;
        }
    }
    if(seek_data == 0)
        return;
    //update
    if(seek_data->type != 0)
        return;
    int index = qrtVariant::pairListIndex(seek_data->list, "vch_plan_k3o3_state");
    if(index < 0)
        return;

    qrtVariantPairList binds;
    binds << qrtVariantPair("vch_plan_k3o3_state", qrtVariant::pairListValue(seek_data->list, "vch_plan_k3o3_state"));
    list << (lds_query_db::TablePairlistData){0, "cey_u_orderdish", binds, seek_data->where_sql};
}

void lds_query_hddpos::ch_yun_already_keep_check(const QString &table, qrtVariantPairList &key_value_list){
    //ch_yun_already_keep 插入包含时，一律都是N
    bool flag = true;
    if(db.record(table).field("ch_yun_already_keep").isValid()) {
        flag = true;
        for(int k = 0; k < key_value_list.count(); k++) {
            if(key_value_list[k].first == "ch_yun_already_keep")  {
                flag = false;
                break;
            }
        }
        if(flag)
            key_value_list << qrtVariantPair("ch_yun_already_keep", "N");
    }
}

void lds_query_hddpos::terminal_code_check(const QString &table, qrtVariantPairList &key_value_list){
    bool flag = true;
    //terminal_code 插入时，一律都是本机
    if(db.record(table).field("terminal_code").isValid()) {
        flag = true;
        int k = 0;
        for(; k < key_value_list.count(); k++) {
            if(key_value_list[k].first == "terminal_code")  {
                flag = false;
                break;
            }
        }
        if(flag)
            key_value_list << qrtVariantPair("terminal_code", lds::terminalCode);
        else
            key_value_list[k].second = lds::terminalCode;
    }
}

QVariant lds_query_hddpos::selectValue(lds_query_hddpos &query, const QString &sql, int i, const QVariant &defValue)
{
    query.execSelect(sql);
    if(query.next())
        return query.recordValue(i);

    return defValue;
}

QVariant lds_query_hddpos::selectValue(const QString &sql, int i, const QVariant &defValue)
{
    lds_query_hddpos q;
    return selectValue(q, sql, i, defValue);
}

QVariant lds_query_hddpos::selectValue(const QString &sql, const QString &arg0, int i, const QVariant &defValue)
{
    lds_query_hddpos q;
    return selectValue(q, sql.arg(arg0), i, defValue);
}

QDateTime lds_query_hddpos::selectNow()
{
    lds_query_hddpos q;
    return selectValue(q, "select now() ", 0).toDateTime();
}

bool lds_query_hddpos::selectHasNext(const QString &sql)
{
    lds_query_hddpos q;
    q.execSelect(sql);
    return q.next();
}

bool lds_query_hddpos::updateTable(const QString &tablename, const qrtVariantPairList &key_value_list, const QString &where_sql, QString *errstring)
{
    lds_query_hddpos q;
    return q.execUpdate(tablename, key_value_list, where_sql, errstring);
}

bool lds_query_hddpos::insertTable(const QString &tablename, const qrtVariantPairList &key_value_list, QString *errstring)
{
    lds_query_hddpos q;
    return q.execInsert(tablename, key_value_list, errstring);
}

bool lds_query_hddpos::deleteTable(const QString &tablename, const QString &where_sql, QString *errstring)
{
    lds_query_hddpos q;
    return q.execDelete(tablename, where_sql, errstring);
}

QString lds_query_hddpos::cey_sys_parameter_get(const QString &key, const QString &def)
{
    this->execSelect(QString(" select vch_value from cey_sys_parameter where vch_parameter = '%1' ").arg(key));
    if(this->next()) {
        return this->recordValue(0).toString();
    }
    return def;
}

void lds_query_hddpos::cey_sys_parameter_set(const QString &key, const QString &value, const QString &vch_explain)
{
    this->execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList()
                                    << qrtVariantPair("vch_parameter", key)
                                    << qrtVariantPair("vch_value", value)
                                    << qrtVariantPair("vch_explain", vch_explain)
                                    );
}

lds_query_hddpos::lds_query_hddpos()
    :lds_query_db(lds_query_thread::getThreadDbAndOpen())
{
}

void lds_query_hddpos::execUpdateTrigger(QList<lds_query_db::TablePairlistData> &list)
{
    for(int k = 0; k < list.count(); k ++) {
        TablePairlistData &d = list[k];
        ch_yun_already_keep_check(d.table, d.list);
    }
    //interface_malayisa_hourly_pos
    interface_malayisa_hourly_pos_check(list);
}

void lds_query_hddpos::execInsertTrigger(QList<lds_query_db::TablePairlistData> &list)
{
    for(int k = 0; k < list.count(); k ++) {
        TablePairlistData &d = list[k];
        ch_yun_already_keep_check(d.table, d.list);
        terminal_code_check(d.table, d.list);
    }
    //interface_malayisa_hourly_pos
    interface_malayisa_hourly_pos_check(list);
}

int &lds_query_hddpos::get_db_state()
{
    QString dbname = lds_query_thread::getThreadDbname();
    if(dbname_state_map.find(dbname) == dbname_state_map.end()) {
        dbname_state_map.insert(dbname, 0);
    }
    return dbname_state_map[dbname];
}

bool lds_query_hddpos::tran_saction()
{
#ifdef QT_DEBUG
    qDebug() << __FUNCTION__ << get_db_state();
    if(get_db_state() != 0) {
        qDebug("transaction,  state != 0 ");
        qDebug() << get_db_state();
        QObject *o = 0;
        o->children();
    }
#endif
    get_db_state()++;
    return lds_query_thread::getThreadDbAndOpen().transaction();
}

bool lds_query_hddpos::com_mit()
{
#ifdef QT_DEBUG
    qDebug() << __FUNCTION__ << get_db_state();
    if(get_db_state() != 1) {
        qDebug("commit,  state != 1 ");
        qDebug() << get_db_state();
        QObject *o = 0;
        o->children();
    }
#endif
    get_db_state()--;
    return lds_query_thread::getThreadDbAndOpen().commit();
}

bool lds_query_hddpos::roll_back()
{
#ifdef QT_DEBUG
    qDebug() << __FUNCTION__ << get_db_state();
    if(get_db_state() != 1) {
        qDebug("rollback,  state != 1 ");
        qDebug() << get_db_state();
        QObject *o = 0;
        o->children();
    }
#endif
    get_db_state()--;
    return lds_query_thread::getThreadDbAndOpen().rollback();
}

void lds_query_hddpos::recordFill(qrtVariantPairList &d)
{
    for(int k = 0, count = this->recordCount(); k < count; k++) {
        d.append(qrtVariantPair(this->record().fieldName(k), this->recordValue(k)));
    }
}

void lds_query_hddpos::replace(qrtVariantPairList &d, const QString &field_name, const QVariant &value)
{
    for(int k = 0; k < d.count(); k++) {
        if(d[k].first == field_name) {
            d[k].second = value;
            break;
        }
    }
}

void lds_query_hddpos::remove(qrtVariantPairList &d, const QString &field_name)
{
    for(int k = 0; k < d.count(); k++) {
        if(d[k].first == field_name) {
            d.removeAt(k);
            break;
        }
    }
}
