#ifndef N_FUNC_H
#define N_FUNC_H

#include <QDialog>
#include <QDateTime>
#include "lds_query_hddpos.h"
#include "n_func.h"
//里面并没有饱和任何事物

class n_func : public QDialog
{
    Q_OBJECT
public:
    typedef struct {
        QString as_parm;
        QString as_memberno;
        QString as_member;
        QString as_handinput;
        QString as_cardtype;
        QString as_m_typ;
        QString as_m_typ_name;
        QString as_m_att;
        QString as_m_att_name;
        double adec_m_att_data;
        QString as_m_dis;
        QString as_m_dis_name;
        QString as_m_dis_data;
        QString as_m_dis_text;
        QString as_m_state;
        QDateTime adt_m_limitdate;
        double adec_m_limitdata;
        int ai_backcash_rate;
        QString as_calc_type;
        double adec_calc_unit;
        QString as_branchno;
        QString as_begin;
        QString as_end;
        QString as_m_cardflag;
        QDateTime  dt_operdate;

    }st_member_type;

    struct READCARDINFO{
        QString vch_memberno ;
        QString ch_typeno ;
        QString vch_typename;
        QString vch_member;
        QString vch_handtel;
        QString ch_state;
        QString vch_password ;
        QString int_card_baseamount;
        QString ch_attribute ;
        QString ch_favourable;//会员类型中的优惠方式中类别：1会员特价2固定折扣
        QString ch_pricekind ;//会员类型中的优惠方式中的会员价index 1- 会员特价1
        QString int_discount ;//会员类型中的优惠方式中的会员折扣
        double num_amount ;
        double num_point ;
        QString ch_cardflag ;
        QDateTime dt_limit ;
        QString ch_member_belong_state;
        QString errstring;
        QString desc();
        QString color();
        bool isyun();
    };

public:
    explicit n_func(QWidget *parent = 0);
    //获取数据库时间
    //1  return QString ----> value 当前时间
    static  QDateTime f_get_sysdatetime();
    static  QString f_get_sysdatetime_str(const QString &format = "yyyy-MM-dd hh:mm:ss");

    //【会员】

    //获取数据库参数数据
    //1  return QString ----> value 如果parent == 0，则错误弹窗不会弹出
    static QString f_m_get_parameter(QWidget *parent, const QString &as_parameter, const QString &as_default="0", bool *ok = 0);
    static bool f_m_get_parameter(const QString &as_parameter, QString &retValue, const QString &defValue, QString &errstring);

    static QByteArray f_m_get_cardpwdtransfer(const QString &icpasswd);
    static QByteArray f_m_defcardpwd;
    //设置数据库参数数据
    //1  return QString ----> error
    //2 内部有update insert操作，需要外部提供事务
    static QString f_m_set_parameter(const QString &as_parameter, const QString &as_value, const QString &as_explain);
    static bool f_m_set_parameter(const QString &as_parameter, const QVariant &as_value, const QString &as_explain, QString &errstring);

    //1  return QString ----> sql language
    static QString f_m_switch_product(const QString &as_object, const QString &as_parm);

    //1  return QString ----> error
    //2 内部有update insert操作，需要外部提供事务
    static QString f_m_deposit(QWidget *parent, QString as_memberno,double adec_deposit,double adec_realamount,
                               double adec_amount,QString as_deposit_mode,QString as_pay_mode,QString as_memo,const QString &type/*充值方式:C:取款 D:退卡 空:充值*/="OTHER");

    //1  return QString ----> error
    //2 内部有update insert操作，需要外部提供事务
    static QString f_m_insert_log(QString as_memberno,QString as_matter,QString as_memo,int adec_bace);

    //1  return QString ----> error
    //2 内部有update insert操作，需要外部提供事务
    static QString f_m_hang_curamount(const QString &as_memberno);
    /*
    lst_member.as_member = ls_member;有效
    lst_member.as_m_state = ls_state;有效
    lst_member.adec_m_limitdata = ldec_limit;有效
    lst_member.as_m_att = ls_att;有效
    lst_member.adt_m_limitdate = ldt_limit;有效
    lst_member.as_m_cardflag = ls_cardflag;有效
*/
    //1  return QString ----> value
    static st_member_type f_m_get_limit(QWidget *parent, const QString & as_memberno);


    /*
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
*/
    //1  return QString ----> value
    static st_member_type f_m_get_item(QWidget *parent, const QString & as_memberno);

    //1  return QString ----> value
    static QString f_m_get_state(const QString & as_memberno);

    //1  return float ----> value
    static float f_ub_get_service(const QString &as_billno, const QString &as_tableno, const QString &as_type);
    //1 return bool  ----->value
    static bool f_check_bus_grant(const QString &operid, const QString &opercode);
    // 根据f_check_bus_grant来连续判断两次是否有权限
    static bool f_check_bus_grant_else_authority(const QString &ls_operid, const QString &opercode, QWidget *parent, const QString &title);

    static bool qtcardinfo_readcard(n_func::READCARDINFO &cardinfo, const QString &cardno);

    ///
    static QString f_get_sysparm(const QString &as_parameter, const QVariant &as_default=QVariant());
    static QString f_set_sysparm(const QString &as_parameter, const QVariant &as_value, const QString &as_memo=QString());

    static bool f_set_sysparm(const QString &as_parameter, const QVariant &as_value, const QString &as_memo, QString &errstring);

    static QString f_get_sysparm_q(lds_query_hddpos  &query, const QString &as_parameter, const QVariant &as_default=QVariant());
    static QString f_set_sysparm_q(lds_query_hddpos  &query, const QString &as_parameter, const QVariant &as_value, const QString &as_memo=QString());

    static bool f_cmp_syspwd(const QString &input_syspwd);
    static QString f_confirm_user_pwd(const QString &ls_operid, const QString &ls_password);
    static QString f_confirm_user(const QString &ls_operid);
    static QString f_confirm_pwd(const QString &ls_operid, const QString &ls_password);
    static QString f_get_godpassword();
    static QString f_get_opername(const QString &ls_operid);
};

#endif // N_FUNC_H
