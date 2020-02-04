#include "lds_scr_table.h"
#include <QObject>
#include <QColor>
#include "lds.h"
#include "lds_query_hddpos.h"
#include "w_sys_manage_outer_pay_set_tax_type.h"
#include "lds_messagebox.h"

QList<tableSTATE> lds_scr_table::tablestatelist;

const QString lds_scr_table::Q000  = "Q000";

const QVariant lds_scr_table::cey_sys_parameter_value(const QString &vch_parameter, const QVariant &defValue)
{
    lds_query_hddpos  q;
    return cey_sys_parameter_value(q, vch_parameter, defValue);
}

const QVariant lds_scr_table::cey_sys_parameter_value(lds_query_hddpos  &query, const QString &vch_parameter, const QVariant &defValue)
{
    return lds_query_hddpos::selectValue(query, QString(" select vch_value from cey_sys_parameter where vch_parameter = '%1' ").arg(vch_parameter), 0, defValue);
}

lds_scr_table::lds_scr_table()
{
}

QList<tableSTATE> &lds_scr_table::getTablestatelist()
{
    if(tablestatelist.isEmpty()) {
        tablestatelist.append((tableSTATE) {QObject::tr("正使用"), tableState_KAITAI, lds::static_color_light(QString("#ff546b"), -1).name()});//c94b01"});
        tablestatelist.append((tableSTATE) {QObject::tr("已预订"), tableState_YUDING,  "mediumpurple"});
        tablestatelist.append((tableSTATE) {QObject::tr("未使用"),  tableState_KONGXIAN,  "#50c600"});
        tablestatelist.append((tableSTATE) {QObject::tr("已预打"),  tableState_YUDA,  "#0070a1"});
        tablestatelist.append((tableSTATE) {QObject::tr("未清台"),  tableState_BAR,  "white"});
        tablestatelist.append((tableSTATE) {QObject::tr("二维码"),  tableState_QR_CODE,  lds::static_color_light(QString("#ff650b"), -1).name()});//"#ff00ff"});
        //        tablestatelist.append(tableSTATE(QObject::tr("不可选"), tableState_DISABLE,  "#686868"));
    }
    return tablestatelist;
}

tableSTATE lds_scr_table::gettableState(tableState state)
{
    foreach(const tableSTATE &t, lds_scr_table::getTablestatelist()) {
        if(t.state == state) {
            return t;
            break;
        }
    }
    return tableSTATE();
}


bool lds_scr_table::get_max_ch_billno_add1_Insert_cey_u_master_Create_serial_no(const QString &k, QString &ret_ch_billno)
{
    ret_ch_billno = get_max_ch_billno_add1(k);
    return insert_cey_u_master_Create_serial_no(ret_ch_billno);
}

QString lds_scr_table::get_max_ch_billno_add1(const QString &k)
{
    lds_query_hddpos  query;
    QDateTime dt = query.curDateTime();
    QString kyyMMddFF = k + dt.toString("yyMMdd") + lds::LOCAL_TERMINAL_INDEX_FF;

    query.execSelect(QString("select max(ch_billno) from cey_u_master where ch_billno like '%1%' ").arg(kyyMMddFF));
    query.next();
    int id = query.recordValue(0).toString().mid(9).toInt() + 1;
    QString ch_billno = kyyMMddFF + QString().sprintf("%05d",id);

    return ch_billno;
}

QString lds_scr_table::get_max_ch_payno_add1(const QDateTime &dt)
{
    lds_query_hddpos  query;
    QString kyyMMddFF = "P" + dt.toString("yyMMdd") + lds::LOCAL_TERMINAL_INDEX_FF;
    query.execSelect(QString("select Max(ch_payno) from cey_u_checkout_master where   ch_payno like '%1%' ").arg(kyyMMddFF));
    query.next();
    int id = query.recordValue(0).toString().mid(9).toInt() + 1;
    QString ch_payno = kyyMMddFF + QString().sprintf("%05d",id);

    return  ch_payno;
}

QString lds_scr_table::get_max_ch_payno_add1()
{
    lds_query_hddpos  query;
    return get_max_ch_payno_add1(query.curDateTime());
}

bool lds_scr_table::insert_cey_u_master_Create_serial_no(const QString &ch_billno, QString *errstring)
{
    lds_query_hddpos  query;
    if(false == insert_cey_u_master_Create_serial_no(query, ch_billno)) {
        if(errstring)
            *errstring = query.recordError();
        return false;
    }
    return true;
}

bool lds_scr_table::insert_cey_u_master_Create_serial_no(lds_query_hddpos  &query, const QString &ch_billno)
{
    QDateTime cur_dt = query.selectNow();

    if(query.execInsertDuplicateUpdate("cey_u_master_help",
                                       "ch_billno",
                                       qrtVariantPairList()
                                       << qrtVariantPair("ch_billno", ch_billno)<< qrtVariantPair("date", cur_dt)
                                       )) {
        QString serial_no_cur = lds_scr_table::create_serial_no(ch_billno);
        if(query.execInsertDuplicateUpdate("cey_u_master",
                                           "ch_billno",
                                           qrtVariantPairList()
                                           << qrtVariantPair("ch_billno", ch_billno)
                                           << qrtVariantPair("ch_state", 1)
                                           << qrtVariantPair("dt_operdate", cur_dt)
                                           << qrtVariantPair("vch_operID", lds::gs_operid)
                                           << qrtVariantPair("vch_memo", serial_no_cur))) {
            return true;
        }
    }
    return false;
}

QString lds_scr_table::create_serial_no(const QString &ch_billno)
{
    lds_query_hddpos  query;
    query.execSelect(QString(" select (select int_flowID from  cey_u_master_help where ch_billno = '%1') - min(int_flowID) as orderNo from cey_u_master_help where date = '%2' ; ")
                     .arg(ch_billno)
                     .arg(QDateTime::fromString(ch_billno.mid(1, 6), "yyMMdd").toString("20yy-MM-dd"))
                     );
    query.next();

    int serial = query.recordValue("orderNo").toInt() + 1;//排除serial为0的情况
    int begin = cey_sys_parameter_value(query, "serial_no_begin").toInt();
    QString end = cey_sys_parameter_value(query, "serial_no_end").toString();

    return QString::number(lds::min_max_value_add1_loop(begin, end.toInt(), serial)).rightJustified(end.length(), '0');
}



bool lds_scr_table::cey_u_taxsale_master_insert_table(lds_query_hddpos  &query, const QString &ch_billno, QString *errstring)
{
    qrtVariantPairList bind_value_list;
    cey_u_taxsale_master_make_map(bind_value_list, ch_billno);

    if(query.execInsert("cey_u_taxsale_master", bind_value_list)) {
        return true;
    }
    if(errstring)
        *errstring = query.recordError();
    return false;
}

bool lds_scr_table::cey_u_taxsale_master_insert_table(lds_query_hddpos  &query, const QString &ch_billno, const lds_model_sqltablemodel *values, QString *errstring)
{
    qrtVariantPairList bind_value_list;
    bind_value_list
            << values->tablename_kvmap_pair("cey_u_taxsale_master", ch_billno, "ch_billno")

            << values->tablename_kvmap_pair("cey_u_taxsale_master", ch_billno, "ch_fix_flag")
            << values->tablename_kvmap_pair("cey_u_taxsale_master", ch_billno, "int_fix_tax")
            << values->tablename_kvmap_pair("cey_u_taxsale_master", ch_billno, "int_fix_tax_2")
            << values->tablename_kvmap_pair("cey_u_taxsale_master", ch_billno, "int_fix_tax_3")

            << values->tablename_kvmap_pair("cey_u_taxsale_master", ch_billno, "ch_fix_service")
            << values->tablename_kvmap_pair("cey_u_taxsale_master", ch_billno, "int_fix_service")

            << values->tablename_kvmap_pair("cey_u_taxsale_master", ch_billno, "ch_tax_mode")
            << values->tablename_kvmap_pair("cey_u_taxsale_master", ch_billno, "ch_tax_type")
            << values->tablename_kvmap_pair("cey_u_taxsale_master", ch_billno, "dt_operdate")
               ;

    if(query.execInsert("cey_u_taxsale_master", bind_value_list)) {
        return true;
    }
    if(errstring)
        *errstring = query.recordError();
    return false;
}

void lds_scr_table::cey_u_taxsale_master_insert_model(lds_model_sqltablemodel *model, const QString &ch_billno)
{
    qrtVariantPairList bind_value_list;
    cey_u_taxsale_master_make_map(bind_value_list, ch_billno);
    model->tablename_kvmap_insert_field("cey_u_taxsale_master", ch_billno, bind_value_list);
}

void lds_scr_table::cey_u_taxsale_master_make_map(qrtVariantPairList &bind_value_list, const QString &ch_billno)
{
    int tax_price_include_tax = f_get_sysparm("tax_price_include_tax",0).toInt();
    int tax_tax_include_service = f_get_sysparm("tax_tax_include_service",0).toInt();
    int ch_tax_mode = tax_tax_include_service * 2 + tax_price_include_tax;
    lds_model_sqltablemodel m;
    bind_value_list
            << qrtVariantPair("ch_billno", ch_billno)

            << qrtVariantPair("ch_fix_flag", (f_get_sysparm("pay_fixed_tax") == "1" ? "Y" : "N") )
            << qrtVariantPair("int_fix_tax", w_sys_manage_outer_pay_set_tax_type::int_rate_worked(&m, 1))
            << qrtVariantPair("int_fix_tax_2", w_sys_manage_outer_pay_set_tax_type::int_rate_worked(&m, 2))
            << qrtVariantPair("int_fix_tax_3", w_sys_manage_outer_pay_set_tax_type::int_rate_worked(&m, 3))

            << qrtVariantPair("ch_fix_service", f_get_sysparm("pay_fixed_service") == "1" ? "Y" : "N")
            << qrtVariantPair("int_fix_service", f_get_sysparm("pay_fixed_service_value"))

            << qrtVariantPair("ch_tax_mode", ch_tax_mode)
            << qrtVariantPair("ch_tax_type", f_get_sysparm("tax_type", 0))
            << qrtVariantPair("dt_operdate", lds_query_hddpos::selectNow())
               ;
}

bool lds_scr_table::cey_bt_table_update(const QString &ch_tableno, const QString &ch_billno, int sqr, QString *errstring)
{
    lds_query_hddpos  query;
    bool f = cey_bt_table_update(query, ch_tableno, ch_billno, sqr);
    if(false == f)
        if(errstring)
            *errstring = query.recordError();
    return f;
}

bool lds_scr_table::cey_bt_table_update(lds_query_hddpos  &query, const QString &ch_tableno, const QString &ch_billno, int sqr)
{
    bool f = false;
    if(lds_scr_table::Q000 == ch_tableno) {
        f = query.execUpdate("cey_sys_lan_terminal", "ch_billno", ch_billno, qrtEqual("vch_terminal", lds::terminalCode));

        return f;
    }
    //中餐
    if(sqr == 0) {
        f = query.execUpdate("cey_bt_table","ch_billno", ch_billno, qrtEqual("ch_tableno", ch_tableno));
    } else {
        f = query.execUpdate("cey_bt_table","ch_billno_"+QString::number(sqr), ch_billno, qrtEqual("ch_tableno", ch_tableno));
    }
    return f;
}

bool lds_scr_table::cey_bt_table_lock(const QString &ch_tableno, int sqr)
{
    lds_query_hddpos  query;
    bool f = false;
    if(sqr == 0) {
        f = query.execUpdate("cey_bt_table","ch_lockflag", "Y",qrtEqual("ch_tableno", ch_tableno));
    } else {
        f = query.execUpdate("cey_bt_table","ch_lockflag_"+QString::number(sqr), "Y",qrtEqual("ch_tableno", ch_tableno));
    }

    return f;
}

bool lds_scr_table::cey_bt_table_unlock(const QString &ch_tableno, int sqr, QString *errstring)//0 - 8
{
    lds_query_hddpos  query;
    if(sqr == 0) {
        return query.execUpdate("cey_bt_table", "ch_lockflag",
                                "N",qrtEqual("ch_tableno", ch_tableno), errstring);
    }

    return query.execUpdate("cey_bt_table", "ch_lockflag_"+QString::number(sqr),
                            "N",qrtEqual("ch_tableno", ch_tableno), errstring);
}

//该餐桌已经被锁定,请确定
bool lds_scr_table::cey_bt_table_has_locked(const QString &ch_tableno, int sqr)
{
    lds_query_hddpos  query;
    bool f = false;
    if(sqr == 0) {
        query.execSelect(QString("SELECT ch_lockflag from cey_bt_table where ch_tableno = '%1' ")
                         .arg(ch_tableno));
        //
        query.next();
        f = ("Y" == query.recordValue(0).toString());
    } else {
        query.execSelect(QString("SELECT ch_lockflag_%2 from cey_bt_table  where ch_tableno = '%1' ")
                         .arg(ch_tableno)
                         .arg(sqr));
        //
        query.next();
        f = ("Y" == query.recordValue(0).toString());
    }
    return f;
}

bool lds_scr_table::cey_bt_table_has_locked(QWidget *parent, const QString &ch_tableno, int sqr)
{
#ifdef QT_DEBUG
    return false;
#endif
    if(cey_bt_table_has_locked(ch_tableno, sqr)) {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("该餐桌已经被锁定,请确定"));
        return true;
    }
    return false;
}

int lds_scr_table::cey_bt_table_div_sqr(const QString &ch_tableno, const QString &ch_billno)
{
    if(lds_scr_table::Q000 == ch_tableno) {
        return 0;
    }

    lds_query_hddpos  query;
    query.execSelect(QString("SELECT "
                             " ch_billno ,  ch_billno_1,ch_billno_2, "
                             " ch_billno_3, ch_billno_4,ch_billno_5, "
                             " ch_billno_6, ch_billno_7,ch_billno_8  "
                             " from cey_bt_table where ch_tableno = '%1' ")
                     .arg(ch_tableno)
                     );
    if(query.next()) {
        for(int k = 0; k < 9; k++) {
            if(query.recordValue(k).toString() == ch_billno) {
                return k;
            }
        }
    }
    return -1;
}

QString lds_scr_table::cey_bt_table_ch_billno_get(const QString &ch_tableno, int sqr)
{
    QString div;
    if(sqr == 0) {
        div = "";
    } else {
        div = "_"+QString::number(sqr);
    }
    lds_query_hddpos  query;
    query.execSelect(QString("SELECT ch_billno%2 from cey_bt_table where ch_tableno = '%1' ")
                     .arg(ch_tableno)
                     .arg(div));
    query.next();

    return query.recordValue(0).toString();
}

bool lds_scr_table::cey_u_orderdish_dish_existed(const QString &ch_billno)
{
    lds_query_hddpos  query;
    query.execSelect(QString("SELECT ch_billno from cey_u_orderdish where ch_billno = '%1' limit 1")
                     .arg(ch_billno));
    return query.next();
}

bool lds_scr_table::cey_bt_table_bar_ch_billno_existed(const QString &ch_tableno, int sqr)
{
    lds_query_hddpos  query;
    query.execSelect(QString("select ch_billno from cey_bt_table_bar where ch_tableno = '%1' and int_div_id = %2 ")
                     .arg(ch_tableno)
                     .arg(sqr));
    return query.next();
}


QString lds_scr_table::int_rate_name(const lds_model_sqltablemodel *m, int index)
{
    return w_sys_manage_outer_pay_set_tax_type::int_rate_name(m, index);
}

double lds_scr_table::int_rate_worked(const lds_model_sqltablemodel *m, int index)
{
    return w_sys_manage_outer_pay_set_tax_type::int_rate_worked(m, index);
}

QString lds_scr_table::f_get_sysparm(const QString &as_parameter, const QVariant &as_default)
{
    lds_query_hddpos query;
    query.execSelect(QString("select vch_value from cey_sys_parameter where vch_parameter='%1' ").arg(as_parameter));
    if(query.next()) {
        return query.recordValue("vch_value").toString();
    }
    return as_default.toString();
}
