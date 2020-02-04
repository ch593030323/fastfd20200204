#include "n_func.h"
#include "lds_query_hddpos.h"
#include <QVariant>

#include <QDebug>
#include <QSqlError>
#include "uo_rw_member.h"
#include "w_m_member_list.h"
#include "w_sys_manage_cloudsync.h"
#include "lds.h"
#include "lds_messagebox.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "public_sql.h"

QByteArray n_func::f_m_defcardpwd = "FFFFFFFFFFFF";
n_func::n_func(QWidget *parent) :
    QDialog(parent)
{
}

QString n_func::f_get_sysdatetime_str(const QString &format)
{
    return f_get_sysdatetime().toString(format);
}

QDateTime n_func::f_get_sysdatetime()
{
    lds_query_hddpos  query;
    query.execSelect("select NOW();");
    if(query.next()) {
        return query.value(0).toDateTime();
    }
    return QDateTime();
}

QString n_func::f_m_get_parameter(QWidget *parent, const QString &as_parameter, const QString &as_default, bool *ok)
{
    if(w_m_member_list::is_yun_member) {
        QString ret=
                w_sys_manage_cloudsync::get_yun_member_parameter(parent, as_parameter, ok);

        return ret.isEmpty()?as_default:ret;
    } else {
        lds_query_hddpos  query;
        query.execSelect(QString("select vch_value from t_m_parameter where vch_parameter='%1'").arg(as_parameter));
        if(query.next()) {
            if(ok) *ok  = true;
            return query.recordValue("vch_value").toString();
        } else {
            if(ok) *ok  = false;
        }

        return as_default;
    }
}

bool n_func::f_m_get_parameter(const QString &as_parameter, QString &retValue, const QString &defValue, QString &errstring)
{
    if(w_m_member_list::is_yun_member) {
        if(false == w_sys_manage_cloudsync::get_yun_member_parameter(as_parameter, retValue, errstring)) {
            return false;
        }
        return true;
    }
    //
    lds_query_hddpos  query;
    query.execSelect(QString("select vch_value from t_m_parameter where vch_parameter='%1'").arg(as_parameter));
    if(query.next()) {
        retValue = query.recordValue("vch_value").toString();
    } else {
        retValue = defValue;
    }
    return true;
}

QByteArray n_func::f_m_get_cardpwdtransfer(const QString &icpasswd)
{
    if(icpasswd == "FFFFFF") {
        return QByteArray("FFFFFFFFFFFF");
    }
    return icpasswd.toLocal8Bit().toHex();
}

QString n_func::f_m_set_parameter(const QString &as_parameter, const QString &as_value, const QString &as_explain)
{   
    long ll_ret;

    lds_query_hddpos  query;
    query.execSelect(QString("select count(1) from t_m_parameter where vch_parameter='%1'").arg(as_parameter));
    query.next();
    ll_ret=query.recordValue(0).toLongLong();
    if(ll_ret==0) {
        if(!query.execInsert("t_m_parameter",qrtVariantPairList()

                             << qrtVariantPair("vch_parameter", as_parameter)
                             << qrtVariantPair("vch_value", as_value)
                             << qrtVariantPair("vch_explain", as_explain))) {
            return query.recordError();
        }
    } else {
        if(!query.execUpdate("t_m_parameter",qrtVariantPairList()

                             << qrtVariantPair("vch_value", as_value),
                             QString(" vch_parameter = '%1' ").arg(as_parameter))) {
            return query.recordError();
        }
    }
    return "";
}

bool n_func::f_m_set_parameter(const QString &as_parameter, const QVariant &as_value, const QString &as_explain, QString &errstring)
{
    lds_query_hddpos  query;
    query.execSelect(QString("select vch_parameter from t_m_parameter where vch_parameter='%1'").arg(as_parameter));
    if(query.next()) {
        if(query.execUpdate("t_m_parameter",qrtVariantPairList()
                            << qrtVariantPair("vch_value", as_value),
                            QString(" vch_parameter = '%1' ").arg(as_parameter))) {
            return true;
        }
    } else {
        if(query.execInsert("t_m_parameter",qrtVariantPairList()

                            << qrtVariantPair("vch_parameter", as_parameter)
                            << qrtVariantPair("vch_value", as_value)
                            << qrtVariantPair("vch_explain", as_explain))) {
            return true;
        }
    }
    errstring = query.recordError();
    return false;
}

QString n_func::f_m_hang_curamount(const QString &as_memberno)
{
    double ldec_remain;
    double ldec_limit;
    double ldec_paying;
    double ldec_nopay;
    lds_query_hddpos  query;
    query.execSelect(QString("select ifnull(num_limit, 0) from t_m_member where vch_memberno='%1'").arg(as_memberno));
    if(!query.next()) {
        
        return QObject::tr("没有找到该会员");
    }
    ldec_limit=query.recordValue(0).toDouble();
    if(ldec_limit<=0) {
        
        return QObject::tr("会员资料")+as_memberno+QObject::tr("借记额度无效");
    }

    query.execSelect(QString("select ifnull (sum(case ch_type when 'B' then num_amout else -num_pay end),0) from t_m_hang_balance where ch_state ='N' and vch_memberno='%1'").arg(as_memberno));
    if(!query.next()) {
        
        return QObject::tr("没有找到该会员");
    }
    ldec_paying=query.recordValue(0).toDouble();


    query.execSelect(QString("select ifnull(sum (num_pay), 0) from t_m_pay where ch_paymodeno='01' and vch_memberno ='%1' and ifnull(ch_balanceno,'')='' ").arg(as_memberno));
    if(!query.next()) {
        
        return QObject::tr("没有找到该会员");
    }
    ldec_nopay=query.recordValue(0).toDouble();
    ldec_remain=ldec_limit-ldec_paying-ldec_nopay;
    if(ldec_limit<=0) {
        ldec_limit=0;
    }

    if(!query.execInsertDuplicateUpdate("t_m_curamount",
                                        "vch_memberno",
                                        qrtVariantPairList()
                                        << qrtVariantPair("vch_memberno", as_memberno)
                                        << qrtVariantPair("num_amount", ldec_remain)
                                        << qrtVariantPair("vch_operID", lds::gs_operid)
                                        << qrtVariantPair("dt_operdate", n_func::f_get_sysdatetime()))) {
        return query.recordError();
    }
    
    return QString();
}

QString n_func::f_m_switch_product(const QString &as_object, const QString &as_parm)
{
    
    QString ls_return;
    long productid;
    productid=public_sql::gi_productid;
    if(productid==0)productid=1;
    switch(productid) {
    case 1:
    case 4:
        if(as_object == "member_disproject_sql") {
            ls_return="select ch_projectno as 'ch_projectno', vch_projectname as 'vch_projectname'  from cey_bt_project";
        } else if(as_object == "member_type_sql") {
            ls_return="select ch_typeno, vch_typename from t_m_member_type";
        } else if(as_object == "consume_data_table") {
            ls_return="cey_u_orderdish";
        } else if(as_object == "check_data_table") {
            ls_return="cey_u_checkout_detail";
        } else if(as_object == "check_date_memberno") {
            ls_return="vch_voucherno";
        } else if(as_object == "operid_sql") {
            ls_return= "";
        }
        break;
    case 10:
    case 11:
        break;
    }
    
    return ls_return;
}

QString n_func::f_m_deposit(QWidget *parent, QString as_memberno, double adec_deposit, double adec_realamount, double adec_amount, QString as_deposit_mode, QString as_pay_mode, QString as_memo,const QString &type)
{
    //Public function f_m_deposit (string as_memberno,decimal adec_deposit,decimal adec_realamount,decimal adec_amount,string as_deposit_mode,string as_pay_mode,string as_memo) returns boolean
    //string as_memberno
    //decimal adec_deposit
    //decimal adec_realamount
    //decimal adec_amount
    //string as_deposit_mode
    //string as_pay_mode
    //string as_memo
    QString ls_operid;
    QString ldt_oper_str;
    lds_query_hddpos  query;
    QString ch_member_belong_state;

    ldt_oper_str = n_func::f_get_sysdatetime().toString("yyyy-MM-dd hh:mm:ss");
    ls_operid=lds::gs_operid;

    if(w_m_member_list::is_yun_member) {
        ch_member_belong_state = "Y";
        if(type == "C") {
            if(!w_sys_manage_cloudsync::get_yun_member_fetch(parent, as_memberno,lds::gs_operid,QString::number(-adec_deposit), as_pay_mode)) {
                return QObject::tr("云会员取款失败");
            }
        } else if(type == "D") {
            if(!w_sys_manage_cloudsync::get_yun_member_blankcard(parent, as_memberno,QString::number(-adec_deposit), as_pay_mode)) {
                return QObject::tr("云会员退卡失败");
            }
        } else {
            if(!w_sys_manage_cloudsync::get_yun_member_recharge(parent, as_memberno,lds::gs_operid,QString::number(adec_deposit)
                                                                ,  QString::number(adec_realamount), as_pay_mode)) {
                return QObject::tr("云会员充值失败");
            }
        }
    } else {
        //    ls_branchno=;
        if(!query.execInsert("t_m_deposit", qrtVariantPairList()
                             << qrtVariantPair("vch_memberno", as_memberno)
                             << qrtVariantPair("num_deposit", adec_deposit)
                             << qrtVariantPair("num_realamount", adec_realamount)
                             << qrtVariantPair("ch_deposit_mode", as_deposit_mode)
                             << qrtVariantPair("ch_pay_mode", as_pay_mode)

                             << qrtVariantPair("vch_operID", ls_operid)
                             << qrtVariantPair("dt_operdate", ldt_oper_str)
                             << qrtVariantPair("vch_memo", as_memo)
                             << qrtVariantPair("remain_amt", adec_amount))) {
            return query.recordError();
        }

        if(!query.execInsertDuplicateUpdate("t_m_curamount",
                                            "vch_memberno",
                                            qrtVariantPairList()
                                            << qrtVariantPair("vch_memberno", as_memberno)
                                            << qrtVariantPair("num_amount", adec_amount)
                                            << qrtVariantPair("vch_operID", ls_operid)
                                            << qrtVariantPair("dt_operdate", ldt_oper_str))) {
            return query.recordError();
        }
    }

    return "";
}

QString n_func::f_m_insert_log(QString as_memberno, QString as_matter, QString as_memo, int adec_bace)
{
    
    //Public function f_m_insert_log (string as_memberno,string as_matter,string as_memo,decimal adec_bace) returns boolean
    //string as_memberno
    //string as_matter
    //string as_memo
    //decimal adec_bace
    QString ls_operid;
    QString ls_errtext;
    QString ls_member;
    QString ls_type;
    QString ch_member_belong_state;
    QDateTime ldt_today;
    lds_query_hddpos  query;

    //Has been Shielded.
    if(w_m_member_list::is_yun_member) {
        ch_member_belong_state = "Y";
    }


    ls_operid = lds::gs_operid;
    query.execSelect(QString("select dt_sendtime,  vch_member,  ch_typeno, from t_m_member where vch_memberno ='%1'").arg(as_memberno));
    query.next();
    ldt_today=query.recordValue("dt_sendtime").toDateTime();
    ls_member=query.recordValue("vch_member").toString();
    ls_type=query.recordValue("ch_typeno").toString();

    if(query.execInsert("t_m_member_log",qrtVariantPairList()
                        << qrtVariantPair("vch_memberno", as_memberno)
                        << qrtVariantPair("vch_matter", as_matter)
                        << qrtVariantPair("vch_memo", as_memo)
                        << qrtVariantPair("vch_operID", ls_operid)
                        << qrtVariantPair("dt_operdate", n_func::f_get_sysdatetime())

                        << qrtVariantPair("num_bace", adec_bace)
                        << qrtVariantPair("dt_send_date", ldt_today)
                        << qrtVariantPair("vch_member", ls_member)
                        << qrtVariantPair("ch_typeno", ls_type))) {

    } else {

        return query.recordError();
    }
    
    return "";
}

n_func::st_member_type n_func::f_m_get_limit(QWidget *parent, const QString &as_memberno)
{
    
    //Public function f_m_get_limit (string as_memberno) returns st_member
    //string as_memberno
    st_member_type lst_member;
    QString ls_state;
    QString ls_att;
    QString ls_member;
    QString ls_cardflag;
    QDateTime ldt_limit;
    double ldec_limit;
    QDateTime ldt_operdate;
    lds_query_hddpos  query;
    
    while(1) {
        if(w_m_member_list::is_yun_member) {
            bool ok;
            ls_member=w_sys_manage_cloudsync::get_yun_member_readcard(parent, as_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                      "vch_member",ok);if(!ok) break;
            ls_state=w_sys_manage_cloudsync::get_yun_member_readcard(parent, as_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                     "ch_state",ok);if(!ok) break;
            ldt_limit=QDateTime::fromString(w_sys_manage_cloudsync::get_yun_member_readcard(parent, as_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                                            "dt_limit", ok), "yyyy-MM-dd hh:mm:ss");if(!ok) break;
            ls_att=w_sys_manage_cloudsync::get_yun_member_readcard(parent, as_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                   "ch_attribute",ok);if(!ok) break;
            ldec_limit=w_sys_manage_cloudsync::get_yun_member_readcard(parent, as_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                       "num_limit",ok).toDouble();if(!ok) break;
            ls_cardflag=w_sys_manage_cloudsync::get_yun_member_readcard(parent, as_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                        "ch_cardflag",ok);if(!ok) break;
            ldt_operdate=QDateTime::fromString(w_sys_manage_cloudsync::get_yun_member_readcard(parent, as_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                                               "dt_operdate", ok), "yyyy-MM-dd hh:mm:ss");if(!ok) break;

        } else {
            //Has been Shielded.
            query.execSelect(QString("Select a.dt_operdate, a.vch_member,  a.ch_state,  a.dt_limit,  b.ch_attribute,  (case b.ch_attribute when '2' then ifnull(a.int_basedata,  0)when '3' then a.num_limit end)AS member_limit,  ifnull(ch_cardflag,  'N')AS member_cardflag  from t_m_member a,  t_m_member_type b  Where a.ch_typeno =b.ch_typeno and a.vch_memberno = '%1' ")
                             .arg(as_memberno));
            query.next();
            ls_member=query.recordValue("vch_member").toString();
            ls_state=query.recordValue("ch_state").toString();
            ldt_limit=query.recordValue("dt_limit").toDateTime();
            ls_att=query.recordValue("ch_attribute").toString();
            ldec_limit=query.recordValue("member_limit").toDouble();
            ls_cardflag=query.recordValue("member_cardflag").toString();
            ldt_operdate=query.recordValue("dt_operdate").toDateTime();
        }
        break;
    }
    lst_member.as_member = ls_member;
    lst_member.as_m_state = ls_state;
    lst_member.adec_m_limitdata = ldec_limit;
    lst_member.as_m_att = ls_att;
    lst_member.adt_m_limitdate = ldt_limit;
    lst_member.as_m_cardflag = ls_cardflag;
    lst_member.dt_operdate = ldt_operdate;

    
    return lst_member;
}

QString n_func::f_m_get_state(const QString &as_memberno)
{
    
    //Public function f_m_get_state (string as_memberno) returns string
    //string as_memberno
    lds_query_hddpos  query;
    query.execSelect(QString("Select ch_state from t_m_member where vch_memberno = '%1' ")
                     .arg(as_memberno));
    if(query.next()) {
        
        return query.recordValue("ch_state").toString();
    }
    
    return "";
}

float n_func::f_ub_get_service(const QString &as_billno, const QString &as_tableno, const QString &as_type)
{
    return 0;
    //    //Public function f_ub_get_service (string as_billno,string as_tableno,string as_type) returns decimal
    //    //string as_billno
    //    //string as_tableno
    //    //string as_type
    //    QString ls_tableno;
    //    QString ls_mode;
    //    QString ls_serviceflag;
    //    float ldec_service=0;
    //    float ldec_service1=0;
    //    float ldec_startamount=0;
    //    float ldec_unitamount=0;
    //    float ldec_max=0;
    //    float ldec_ration=0;
    //    float ldec_cost=0;
    //    long ll_diff=0;
    //    long ll_begin=0;
    //    long ll_start=0;
    //    long ll_unit=0;
    //    long ll_over=0;
    //    long ll_rate=0;
    //    lds_query_hddpos  query;
    //    lds_query_hddpos  query2;
    //    bool query_next=false;
    //    query.execSelect(QString("select ch_tableno,  TIMESTAMPDIFF(MINUTE, dt_operdate, now()) from cey_u_table  where ch_billno ='%1' and ch_tableno like '%2%' and ch_state ='1'  order by ch_tableno")
    //                     .arg(as_billno).arg(ls_tableno));
    //    //    if(as_type == "2") {
    //    //        ls_tableno="%";
    //    //    } else {
    //    //        ls_tableno=as_tableno;
    //    //    }

    //    //--->时间单位
    //    //! ll_diff = 开台时间到结算时间的分钟数
    //    //! 如果ll_diff大于首钟计费时长，则服务费=首钟收费额+(ll_diff-首钟收费单位)
    //    //
    //    if(query.next()) {
    //        ls_tableno=query.recordValue(0).toString();
    //        ll_diff=query.recordValue(0).toLongLong();
    //        ldec_service1 = 0;
    //        query2.execSelect(QString("select a.ch_service_mode,  a.int_begin_time,  a.int_start_minute,  a.num_start_amount,  a.int_unit_minute,  a.num_unit_amount,  a.int_over,  a.ch_serviceflag,  a.int_rate,  a.num_ration,  a.num_max_service"
    //                                  //  " INTO :ls_mode, :ll_begin, :ll_start, :ldec_startamount, :ll_unit, :ldec_unitamount, :ll_over, :ls_serviceflag, :ll_rate, :ldec_ration, :ldec_max"
    //                                  " from cey_bt_table_type a,  cey_bt_table b"
    //                                  " where a.ch_typeno =b.ch_typeno and b.ch_tableno ='%1' limit 1" ).arg(ls_tableno));
    //        query_next=query2.next();
    //        if(query_next) {//时间单位
    //            ls_mode=query2.recordValue("ch_service_mode").toString();
    //            ll_begin=query2.recordValue("int_begin_time").toLongLong();
    //            ll_start=query2.recordValue("int_start_minute").toLongLong();
    //            ldec_startamount=query2.recordValue("num_start_amount").toFloat();
    //            ll_unit=query2.recordValue("int_unit_minute").toLongLong();
    //            ldec_unitamount=query2.recordValue("num_unit_amount").toFloat();
    //            ll_over=query2.recordValue("int_over").toLongLong();
    //            ls_serviceflag=query2.recordValue("ch_serviceflag").toString();
    //            ll_rate=query2.recordValue("int_rate").toLongLong();
    //            ldec_ration=query2.recordValue("num_ration").toFloat();
    //            ldec_max=query2.recordValue("num_max_service").toFloat();

    //            switch(ls_mode.toInt()) {
    //            case 1://时间单位
    //                /*
    //ll_diff = 开台时间到当前时间的分钟数
    //if(ll_diff >= 首钟计费时长) {
    //    服务费 = 首钟收费额
    //    ll_diff -= 首钟收费单位
    //    while(ll_diff > 每单位可超时长) {
    //        服务费 += 超钟收费额
    //        ll_diff -= 超钟收费单位
    //    }
    //} else {
    //    服务费 = 0
    //}
    //*/
    //                if(ll_diff >= ll_begin) {
    //                    ldec_service1 = ldec_startamount;
    //                    ll_diff -= ll_start;
    //                    //                    if(ll_over > 0 ) {
    //                    while(ll_diff > ll_over) { //21
    //                        ldec_service += ldec_unitamount;
    //                        ll_diff -= ll_unit;
    //                    }
    //                    //                    }
    //                } else {
    //                    ldec_service1 = 0;
    //                    ldec_service = 0;
    //                }
    //                break;
    //            case 2://消费额服务费率
    //                //折后
    //                if(ls_serviceflag == "Y") {
    //                    lds_query_hddpos  query;
    //                    query.execSelect(QString("select ifnull( sum( ((num_num-num_back)*num_price) *int_discount *0.01  + num_price_add),  0 )"
    //                                             //                                       INTO :ldec_cost
    //                                             " from cey_u_orderdish"
    //                                             " where ch_billno ='%1' and ch_tableno ='%2' and ch_presentflag ='N'")
    //                                     .arg(as_billno)
    //                                     .arg(ls_tableno));
    //                    query.next();
    //                    ldec_cost=query.recordValue(0).toFloat();
    //                } else {//折前
    //                    lds_query_hddpos  query;
    //                    query.execSelect(QString("select IfNull( sum( ((num_num-num_back)*num_price+num_price_add) ),  0 )"
    //                                             //                                       INTO :ldec_cost
    //                                             " from cey_u_orderdish"
    //                                             " where ch_billno ='%1' and ch_tableno ='%2' ")
    //                                     .arg(as_billno)
    //                                     .arg(ls_tableno));
    //                    query.next();
    //                    ldec_cost=query.recordValue(0).toFloat();
    //                }
    //                ldec_service1 = (ldec_cost * ll_rate) * 0.01;
    //                break;
    //            case 3://餐桌定额
    //                ldec_service1 = ldec_ration;
    //                break;
    //            case 4://商品消费服务率
    //                //折后
    //                if(ls_serviceflag != "Y") {
    //                    lds_query_hddpos  query;
    //                    query.execSelect(QString("select IfNull( sum( ((a.num_num - a.num_back) * a.num_price + a.num_price_add) *a.int_discount *0.01 ),  0 )"
    //                                             //                                       INTO :ldec_cost
    //                                             " from cey_u_orderdish a,  cey_bt_dish b"
    //                                             "  where a.ch_billno ='%1' and a.ch_tableno ='%2' and ifnull(a.ch_payno,'')='' and a.ch_presentflag ='N' and a.ch_dishno =b.ch_dishno and b.ch_serviceflag ='Y' ")
    //                                     .arg(as_billno)
    //                                     .arg(ls_tableno));
    //                    query.next();

    //                    ldec_cost=query.recordValue(0).toFloat();
    //                } else {//折前
    //                    lds_query_hddpos  query;
    //                    query.execSelect(QString("select IfNull( sum( ((a.num_num - a.num_back) * a.num_price + a.num_price_add) ),  0 )"
    //                                             //                                       INTO :ldec_cost
    //                                             " from cey_u_orderdish a,  cey_bt_dish b"
    //                                             " where a.ch_billno ='%1' and a.ch_tableno ='%2' and ifnull(a.ch_payno, '')='' and a.ch_dishno =b.ch_dishno and b.ch_serviceflag ='Y' ")
    //                                     .arg(as_billno)
    //                                     .arg(ls_tableno));
    //                    query.next();
    //                    ldec_cost=query.recordValue(0).toFloat();
    //                }
    //                ldec_service1 = (ldec_cost * ll_rate) * 0.01;
    //                break;
    //            default://不处理
    //                ldec_service1 = 0;
    //                break;
    //            }//switch
    //        } else {
    //            ldec_service1 = 0;
    //        }

    //        ldec_service += ldec_service1;
    //        if((ls_mode == "1" || ls_mode == "2") || ls_mode == "4") {
    //            if(ldec_max == 0) {
    //            } else if(ldec_service > ldec_max) {
    //                ldec_service = ldec_max;
    //            }
    //        }
    //    }//if query next

    //    return ldec_service;
}

bool n_func::f_check_bus_grant(const QString &operid, const QString &opercode)
{
    return true;
}

bool n_func::f_check_bus_grant_else_authority(const QString &ls_operid, const QString &opercode, QWidget *parent, const QString &title)
{
    return true;
}

bool n_func::qtcardinfo_readcard(n_func::READCARDINFO &cardinfo, const QString &cardno)
{

    //获取卡信息
    lds_query_hddpos  query;
    if(w_m_member_list::is_yun_member) {
        QString dt_limit_str;
        QString num_amount_str;
        QString num_point_str;

        if(false == w_sys_manage_cloudsync::get_yun_member_readcard(cardno, n_func::f_get_sysparm("cloud_user"),  "vch_memberno", cardinfo.vch_memberno, cardinfo.errstring))  return false;
        if(false == w_sys_manage_cloudsync::get_yun_member_readcard(cardno, n_func::f_get_sysparm("cloud_user"),  "ch_typeno", cardinfo.ch_typeno, cardinfo.errstring))  return false;
        if(false == w_sys_manage_cloudsync::get_yun_member_readcard(cardno, n_func::f_get_sysparm("cloud_user"),  "vch_typename", cardinfo.vch_typename, cardinfo.errstring))  return false;
        if(false == w_sys_manage_cloudsync::get_yun_member_readcard(cardno, n_func::f_get_sysparm("cloud_user"),  "vch_member", cardinfo.vch_member, cardinfo.errstring))  return false;
        if(false == w_sys_manage_cloudsync::get_yun_member_readcard(cardno, n_func::f_get_sysparm("cloud_user"),  "vch_handtel", cardinfo.vch_handtel, cardinfo.errstring))  return false;
        if(false == w_sys_manage_cloudsync::get_yun_member_readcard(cardno, n_func::f_get_sysparm("cloud_user"),  "ch_state", cardinfo.ch_state, cardinfo.errstring))  return false;
        if(false == w_sys_manage_cloudsync::get_yun_member_readcard(cardno, n_func::f_get_sysparm("cloud_user"),  "password", cardinfo.vch_password, cardinfo.errstring))  return false;
        if(false == w_sys_manage_cloudsync::get_yun_member_membertype(cardinfo.ch_typeno, "intCardBaseamount", cardinfo.int_card_baseamount, cardinfo.errstring))  return false;
        if(false == w_sys_manage_cloudsync::get_yun_member_readcard(cardno, n_func::f_get_sysparm("cloud_user"),  "ch_attribute", cardinfo.ch_attribute, cardinfo.errstring))  return false;
        if(false == w_sys_manage_cloudsync::get_yun_member_membertype(cardinfo.ch_typeno,  "chFavourable", cardinfo.ch_favourable, cardinfo.errstring))  return false;
        if(false == w_sys_manage_cloudsync::get_yun_member_membertype(cardinfo.ch_typeno,  "chPricekind", cardinfo.ch_pricekind, cardinfo.errstring))  return false;
        if(false == w_sys_manage_cloudsync::get_yun_member_membertype(cardinfo.ch_typeno,  "intDiscount", cardinfo.int_discount, cardinfo.errstring))  return false;
        if(false == w_sys_manage_cloudsync::get_yun_member_readcard(cardno, n_func::f_get_sysparm("cloud_user"),  "ch_cardflag", cardinfo.ch_cardflag, cardinfo.errstring))  return false;
        if(false == w_sys_manage_cloudsync::get_yun_member_readcard(cardno, n_func::f_get_sysparm("cloud_user"),  "dt_limit", dt_limit_str, cardinfo.errstring))  return false;
        if(false == w_sys_manage_cloudsync::get_yun_member_readcard(cardno, n_func::f_get_sysparm("cloud_user"),  "num_amount", num_amount_str, cardinfo.errstring))  return false;
        if(false == w_sys_manage_cloudsync::get_yun_member_readcard(cardno, n_func::f_get_sysparm("cloud_user"),  "num_point", num_point_str, cardinfo.errstring))  return false;
        cardinfo.ch_member_belong_state = "Y";

        cardinfo.dt_limit = QDateTime::fromString(dt_limit_str, "yyyy-MM-dd hh:mm:ss");
        cardinfo.num_amount = num_amount_str.toDouble();
        cardinfo.num_point = num_point_str.toDouble();
    } else {
        query.execSelect(QString(
                             " SELECT a.vch_memberno,a.vch_member,a.ch_typeno,a.vch_tel, a.vch_handtel,  a.dt_birthday,  a.ch_state, a.vch_password, a.dt_limit,a.ch_cardflag, b.int_card_baseamount,"
                             " b.ch_attribute, b.ch_favourable, b.ch_pricekind, b.int_discount,b.vch_typename, c.num_amount, c.num_point"
                             " FROM t_m_member a "
                             " left join t_m_member_type b on a.ch_typeno = b.ch_typeno "
                             " left join t_m_curamount c on a.vch_memberno = c.vch_memberno"
                             " where a.ch_typeno = b.ch_typeno and b.ch_attribute like '%'  and a.vch_memberno = '%1' "
                             )
                         .arg(cardno));
        if(false == query.next())  {
            cardinfo.errstring = QObject::tr("会员卡不存在");
            return false;
        }
        cardinfo.vch_memberno = query.recordValue("vch_memberno").toString();
        cardinfo.ch_typeno = query.recordValue("ch_typeno").toString();
        cardinfo.vch_typename = query.recordValue("vch_typename").toString();
        cardinfo.vch_member = query.recordValue("vch_member").toString();
        cardinfo.vch_handtel=query.recordValue("vch_handtel").toString();
        cardinfo.ch_state = query.recordValue("ch_state").toString();
        cardinfo.vch_password = query.recordValue("vch_password").toString();
        cardinfo.int_card_baseamount = query.recordValue("int_card_baseamount").toString();
        cardinfo.ch_attribute=query.recordValue("ch_attribute").toString();
        cardinfo.ch_favourable=query.recordValue("ch_favourable").toString();
        cardinfo.ch_pricekind=query.recordValue("ch_pricekind").toString();
        cardinfo.ch_cardflag=query.recordValue("ch_cardflag").toString();
        cardinfo.int_discount=query.recordValue("int_discount").toString();
        cardinfo.dt_limit = query.recordValue("dt_limit").toDateTime();
        cardinfo.num_amount = query.recordValue("num_amount").toDouble();//余额
        cardinfo.num_point = query.recordValue("num_point").toDouble();//积分
        cardinfo.ch_member_belong_state = "N";
    }
    return true;
}

n_func::st_member_type n_func::f_m_get_item(QWidget *parent,const QString &as_memberno)
{
    
    //Public function f_m_get_item (string as_memberno) returns st_member
    //string as_memberno
    st_member_type lst_member;
    QString ls_member;
    QString ls_typ;
    QString ls_typ_name;
    QString ls_att;
    QString ls_att_name;
    QString ls_dis;
    QString ls_dis_name;
    QString ls_dis_data;
    QString ls_dis_text;
    QString ls_calc_type;
    int li_backcash_rate;
    double ldec_calc_unit;

    //Has been Shielded.
    lds_query_hddpos  query;
    if(w_m_member_list::is_yun_member) {
        bool ok;
        ls_member=w_sys_manage_cloudsync::get_yun_member_readcard(parent, as_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                  "vch_member", ok);if(!ok) goto end;
        ls_typ=w_sys_manage_cloudsync::get_yun_member_readcard(parent, as_memberno, n_func::f_get_sysparm("cloud_user"),
                                                               "ch_typeno", ok);if(!ok) goto end;
        ls_typ_name=w_sys_manage_cloudsync::get_yun_member_readcard(parent, as_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                    "vch_typename", ok);if(!ok) goto end;
        ls_att=w_sys_manage_cloudsync::get_yun_member_readcard(parent, as_memberno, n_func::f_get_sysparm("cloud_user"),
                                                               "ch_attribute", ok);if(!ok) goto end;
        ls_att_name=w_sys_manage_cloudsync::get_yun_member_readcard(parent, as_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                    "ls_att_name", ok);if(!ok) goto end;
        li_backcash_rate=w_sys_manage_cloudsync::get_yun_member_readcard(parent, as_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                         "int_backcash_rate", ok).toDouble();if(!ok) goto end;

        ls_dis=w_sys_manage_cloudsync::get_yun_member_readcard(parent, as_memberno, n_func::f_get_sysparm("cloud_user"),
                                                               "ch_favourable", ok);if(!ok) goto end;
        ls_dis_data=w_sys_manage_cloudsync::get_yun_member_readcard(parent, as_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                    "int_discount", ok);if(!ok) goto end;
        ls_calc_type=w_sys_manage_cloudsync::get_yun_member_readcard(parent, as_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                     "ch_calc_type", ok);if(!ok) goto end;
        ldec_calc_unit=w_sys_manage_cloudsync::get_yun_member_readcard(parent, as_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                       "int_calc_unit", ok).toDouble();if(!ok) goto end;

    } else {
        query.execSelect(QString("select a.vch_member,  a.ch_typeno,  b.vch_typename,  b.ch_attribute, (case b.ch_attribute when '1' then '%1' when '2' then '%2' when '3' then '%3' end) as 'ls_att_name', b.int_backcash_rate,  b.ch_favourable, (case b.ch_favourable when '1' then ifnull(b.ch_pricekind,  '1')when '2' then left(ifnull(b.int_discount,  100 ), 3) when '3' then b.ch_projectno end) as 'ls_dis_data',(case b.ch_calc when 'Y' then ifnull(b.ch_calc_type,  '1')end)as 'ls_calc_type', (case b.ch_calc when 'Y' then ifnull(b.int_calc_unit,  1)end)as 'ldec_calc_unit'  from t_m_member a,  t_m_member_type b  where a.ch_typeno =b.ch_typeno and a.vch_memberno ='%4' ")
                         .arg(QObject::tr("优惠卡"))
                         .arg(QObject::tr("充值卡"))
                         .arg(QObject::tr("借记卡"))
                         .arg(as_memberno)
                         );
        //INTO :ls_member, :ls_typ, :ls_typ_name, :ls_att, :ls_att_name, :li_backcash_rate, :ls_dis, :ls_dis_data, :ls_calc_type, :ldec_calc_unit
        query.next();
        ls_member=query.recordValue("vch_member").toString();
        ls_typ=query.recordValue("ch_typeno").toString();
        ls_typ_name=query.recordValue("vch_typename").toString();
        ls_att=query.recordValue("ch_attribute").toString();
        ls_att_name=query.recordValue("ls_att_name").toString();
        li_backcash_rate=query.recordValue("int_backcash_rate").toDouble();


        ls_dis=query.recordValue("ch_favourable").toString();
        ls_dis_data=query.recordValue("ls_dis_data").toString();
        ls_calc_type=query.recordValue("ls_calc_type").toString();
        ldec_calc_unit=query.recordValue("ldec_calc_unit").toDouble();
    }

    if(!(li_backcash_rate > 0 && li_backcash_rate < 100)) {
        li_backcash_rate = 0;
    }
    if(ldec_calc_unit <= 0) {
        ldec_calc_unit = 1;
    }
    if(ls_dis_name == "1") {
        ls_dis_name = QObject::tr("会员特价");
        ls_dis_text = ls_dis_name + ls_dis_data;
        if(ls_dis_data.isEmpty()) {
            ls_dis_data = "1";
        }
    } else if(ls_dis_name == "2") {
        ls_dis_name = QObject::tr("固定折扣");
        ls_dis_text = ls_dis_name + ":" + ls_dis_data + "%";
    } else if(ls_dis_name == "3") {
        ls_dis_name = QObject::tr("方案折扣");
        ls_dis_text = ls_dis_name + ":" + ls_dis_text;
    }
    lst_member.as_member = ls_member;
    lst_member.as_m_typ = ls_typ;
    lst_member.as_m_typ_name = ls_typ_name;
    lst_member.as_m_att = ls_att;
    lst_member.as_m_att_name = ls_att_name;
    lst_member.as_m_dis = ls_dis;
    lst_member.as_m_dis_name = ls_dis_name;
    lst_member.as_m_dis_data = ls_dis_data;
    lst_member.as_m_dis_text = ls_dis_text;
    lst_member.ai_backcash_rate = li_backcash_rate;
    lst_member.as_calc_type = ls_calc_type;
    lst_member.adec_calc_unit = ldec_calc_unit;
    
end:
    return lst_member;
}







QString n_func::READCARDINFO::desc()
{
    QString retv;
    if(!vch_memberno.isEmpty()) {
        retv = QObject::tr("会员");
        if(isyun()) {
            retv = QObject::tr("云会员");
        }
        if(!vch_memberno.isEmpty()) {
            retv += +":" + vch_memberno;
        }
        if(ch_favourable == "0") {
            retv += "," + QObject::tr("无优惠");
        }
        if(ch_favourable == "1") {
            retv += "," + QObject::tr("会员特价");
        }
        if(ch_favourable == "2") {
            int discount = int_discount.toInt();
            if(public_sql::discount_off_mode()) {
                discount = 100 - discount;
            }
            retv += "," + QObject::tr("固定折扣")+QString("(%1)").arg(discount);
        }
    }
    return retv;
}

QString n_func::READCARDINFO::color()
{
    if(isyun()) return "green";
    return "#ff546b";
}

bool n_func::READCARDINFO::isyun()
{
    return "Y" == ch_member_belong_state;
}
///

QString n_func::f_get_sysparm(const QString &as_parameter, const QVariant &as_default)
{
    lds_query_hddpos  query;
    return f_get_sysparm_q(query, as_parameter, as_default);
}

QString n_func::f_set_sysparm(const QString &as_parameter, const QVariant &as_value, const QString &as_memo)
{
    lds_query_hddpos  query;
    return f_set_sysparm_q(query, as_parameter, as_value, as_memo);
}

bool n_func::f_set_sysparm(const QString &as_parameter, const QVariant &as_value, const QString &as_memo, QString &errstring)
{
    errstring = f_set_sysparm(as_parameter, as_value, as_memo);
    return "" == errstring;
}

///////////
QString n_func::f_get_sysparm_q(lds_query_hddpos  &query, const QString &as_parameter, const QVariant &as_default)
{
    query.execSelect(QString("select vch_value from cey_sys_parameter where vch_parameter='%1' ").arg(as_parameter));
    if(query.next()) {
        return query.recordValue("vch_value").toString();
    }

    return as_default.toString();
}

QString n_func::f_set_sysparm_q(lds_query_hddpos  &query, const QString &as_parameter, const QVariant &as_value, const QString &as_memo)
{
    qlonglong ll_ret;
    query.execSelect(QString("select count(0) from cey_sys_parameter where vch_parameter ='%1' ")
                     .arg(as_parameter));
    query.next();
    ll_ret=query.recordValue(0).toLongLong();
    if(ll_ret==0) {
        if(!query.execInsert("cey_sys_parameter",qrtVariantPairList()
                             << qrtVariantPair("vch_parameter", as_parameter)
                             << qrtVariantPair("vch_value", as_value)
                             << qrtVariantPair("vch_explain", as_memo))) {
            return query.recordError();
        }
    } else {
        if(!query.execUpdate("cey_sys_parameter",qrtVariantPairList()
                             << qrtVariantPair("vch_value", as_value),
                             QString(" vch_parameter = '%1' ").arg(as_parameter))) {
            return query.recordError();
        }
    }
    return "";
}


bool n_func::f_cmp_syspwd(const QString &input_syspwd)
{
    //08MM01dd
    return (input_syspwd==f_get_godpassword())
            || (input_syspwd==f_get_sysparm("system_pass", "0000"));
}

QString n_func::f_confirm_user_pwd(const QString &ls_operid, const QString &ls_password)
{
    QString ls_error;
    ls_error = f_confirm_user(ls_operid);
    if(ls_error != "")  {
        return ls_error;
    }

    ls_error = f_confirm_pwd(ls_operid, ls_password);
    if(ls_error != "")  {
        return ls_error;
    }
    return "";


//    if(ls_operid.isEmpty()) {
//        return QString("用户名为空");
//    }
//    lds_query_hddpos  query;
//    QString ls_error;
//    if(ls_operid == "0000") {
//        ls_error = QObject::tr("密码错误");
//        if(n_func::f_cmp_syspwd(ls_password)) {
//            ls_error = "";
//        }
//    } else {
//        query.execSelect(QString("select vch_password from cey_sys_operator  where vch_operID = '%1' ")
//                         .arg(ls_operid));
//        ls_error = QObject::tr("操作员不存在");
//        if(query.next()) {
//            query.execSelect(QString("select vch_password from cey_sys_operator  where vch_operID = '%1' and ch_flag = 'Y' ")
//                             .arg(ls_operid));
//            ls_error = QObject::tr("操作员未生效");
//            if(query.next()) {
//                ls_error = QObject::tr("密码错误");
//                if(query.recordValue(0).toString() == ls_password) {
//                    ls_error = "";
//                }
//            }
//        }
//    }

//    return ls_error;
}

QString n_func::f_confirm_user(const QString &ls_operid)
{
    if(ls_operid.isEmpty()) {
        return QString("用户名为空");
    }
    if(ls_operid == "0000") {
        return "";
    }
    if(!lds_query_hddpos::selectHasNext(QString("select vch_password from cey_sys_operator  where vch_operID = '%1' ")
                                        .arg(ls_operid))) {
        return QObject::tr("操作员不存在");
    }
    if(!lds_query_hddpos::selectHasNext(QString("select vch_password from cey_sys_operator  where vch_operID = '%1' and ch_flag = 'Y' ")
                                        .arg(ls_operid))) {
        return QObject::tr("操作员未生效");
    }
    return "";
}

QString n_func::f_confirm_pwd(const QString &ls_operid, const QString &ls_password)
{
    lds_query_hddpos  query;
    if(ls_operid == "0000") {
        if(!n_func::f_cmp_syspwd(ls_password)) {
            return QObject::tr("密码错误");
        }
        return "";
    }
    if(ls_password != query.pathValue("cey_sys_operator/vch_password:vch_operID='%1'", ls_operid).toString()) {
        return QObject::tr("密码错误");
    }
    return "";
}

QString n_func::f_get_godpassword()
{
    return QDateTime::currentDateTime().toString("08MM01dd");
}

QString n_func::f_get_opername(const QString &ls_operid)
{
    if(ls_operid == "0000") {
        return "system";
    }
    lds_query_hddpos  query;
    query.execSelect(QString("select vch_operator  from cey_sys_operator  where vch_operID ='%1' ")
                     .arg(ls_operid));
    query.next();

    return query.recordValue(0).toString();
}
