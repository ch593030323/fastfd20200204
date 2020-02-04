#include "uo_rw_member.h"
#include "n_func.h"
#include "lds_query_hddpos.h"
#ifdef Q_OS_WIN
#include "readcard_mwrf32.h"
#endif
#include <QSqlError>
#include <QVariant>
#include "reader_rf.h"
#include "w_sys_manage_cloudsync.h"
#include "w_m_member_list.h"
#include "public_sql.h"
#include "lds.h"
#include "w_m_member.h"


uo_rw_member::uo_rw_member(QObject *parent) :
    QObject(parent)
{
}

uo_rw_member::read_card_type uo_rw_member::of_read_card(QWidget *parent, const QString &as_memberno)
{
    //Public function of_read_card (string as_memberno) returns read_card
    //string as_memberno

    read_card_type lst_card;
    QString cardno;
    

    //Has been Shielded.
    lst_card.flag = false;
    lst_card.errtext=QObject::tr("会员读卡失败");
    if(as_memberno.isEmpty()) {
        lst_card.errtext=QObject::tr("会员号不能为空");
        return lst_card;
    }

    if(is_cardtype!="1" && is_cardtype!="2" && is_cardtype!="3") {
        lst_card.errtext = QObject::tr("读卡无效，卡类型：无会员卡");
        return lst_card;
    }

    lds_query_hddpos  query;
    cardno=as_memberno;
    lst_card.cardid = cardno;
    while(1) {
        if(w_m_member_list::is_yun_member) {

            lds_messagebox_loading_show loading(parent, "", "LOADING");
            loading.show_delay();

            bool ok;
            lst_card.amount=w_sys_manage_cloudsync::get_yun_member_readcard(parent, cardno, n_func::f_get_sysparm("cloud_user"),
                                                                            "num_amount", ok).toDouble();if(!ok) break;
            lst_card.point=w_sys_manage_cloudsync::get_yun_member_readcard(parent, cardno, n_func::f_get_sysparm("cloud_user"),
                                                                           "num_point", ok).toDouble();if(!ok) break;
            lst_card.card_type=w_sys_manage_cloudsync::get_yun_member_readcard(parent, cardno, n_func::f_get_sysparm("cloud_user"),
                                                                               "vch_typename", ok);if(!ok) break;
            lst_card.cardname=w_sys_manage_cloudsync::get_yun_member_readcard(parent, cardno, n_func::f_get_sysparm("cloud_user"),
                                                                              "vch_member", ok);if(!ok)  break;
        } else {
            query.execSelect(QString(
                                 " SELECT a.vch_memberno,a.vch_member,a.ch_typeno,a.vch_tel, a.vch_handtel,  a.dt_birthday,  a.ch_state, a.vch_password, a.dt_limit,a.ch_cardflag, b.int_card_baseamount,"
                                 " b.ch_attribute, b.ch_favourable, b.ch_pricekind, b.int_discount,b.vch_typename, c.num_amount, c.num_point"
                                 " FROM t_m_member a "
                                 " left join t_m_member_type b on a.ch_typeno = b.ch_typeno "
                                 " left join t_m_curamount c on a.vch_memberno = c.vch_memberno"
                                 " where a.ch_typeno = b.ch_typeno and b.ch_attribute like '%'  and a.vch_memberno = '%1' "
                                 )
                             .arg(as_memberno));
            query.next();
            lst_card.amount=query.recordValue("num_amount").toDouble();//余额
            lst_card.point= query.recordValue("num_point").toDouble();//积分
            lst_card.card_type= query.recordValue("vch_typename").toString();//积分
            lst_card.cardname= query.recordValue("vch_member").toString();//积分
        }

        lst_card.flag = true;
        break;
    }
    return lst_card;
}

void uo_rw_member::of_init (QWidget *parent, const QString &as_action, bool *ok)
{
    is_cardtype = w_m_member::get_vip_card_type(parent, ok);
    if(as_action.isEmpty()) {
        return ;
    }
    is_action=as_action;
    if(is_action == "1") {
        this->text=QObject::tr("读卡");
    } else if(is_action == "2") {
        this->text=QObject::tr("发卡");

    } else if(is_action == "3") {
        this->text=QObject::tr("清除");

    } else if(is_action == "4") {

    } else if(is_action == "5") {

    }

    if(is_action!="4") {
        visible=true;
    }
}

QString uo_rw_member::of_send_card(QWidget *parent, const QString &as_memberno)
{
    //Public function of_send_card (string as_memberno) returns boolean
    //string as_memberno
    QString ls_cardflag;
    QString ls_state;
    QString ls_cardno;
    QString ls_cardtype;
    QDateTime ldt_sendtime;
    int li_basedata;
    n_func::st_member_type lst_member;
    lds_query_hddpos  query;

    if(as_memberno.isEmpty()) {
        return QObject::tr("会员号不能为空");
    } else {
        query.execSelect(QString("select ch_cardflag,  ch_state,  ifnull(int_basedata,  0) as int_basedata, ch_typeno,  dt_sendtime from t_m_member  where vch_memberno = '%1' ")
                         .arg(as_memberno));
        if(query.next()) {//0
            ls_cardflag=query.recordValue("ch_cardflag").toString();
            ls_state=query.recordValue("ch_state").toString();
            li_basedata=query.recordValue("int_basedata").toInt();
            ls_cardtype=query.recordValue("ch_typeno").toString();
            ldt_sendtime=query.recordValue("dt_sendtime").toDateTime();

            if(li_basedata==0) {
                query.execSelect(QString("select ifnull(int_card_baseamount,  0),  now()  from t_m_member_type where ch_typeno ='%1' ").arg(ls_cardtype));
                if(query.next()) {
                    li_basedata=query.recordValue(0).toInt();
                    ldt_sendtime=query.recordValue(1).toDateTime();
                } else {
                    return QObject::tr("会员类型不存在");
                }
            }

            if(ls_state != "1") {
                return QObject::tr("该会员状态不正常");
            }
            if(ls_cardflag == "Y") {
                return QObject::tr("该会员已经发卡");
            }

            ls_cardno = as_memberno;//会员号和卡号一致

            if(is_cardtype == "1") {
                //IC卡需要写密码，并且写卡号
                QString icpasswd=n_func::f_m_get_parameter(parent,"vip_ic_password");
                QByteArray transferpasswd = n_func::f_m_get_cardpwdtransfer(icpasswd);
                if(icpasswd.isEmpty()) {
                    return QObject::tr("IC卡密码不能为空");
                } else {
#ifdef Q_OS_WIN
                    ReadCard_mwrf32 reader;reader.setDefPwd(n_func::f_m_defcardpwd);//需要设置一次默认密码，因为changepwd后不会完全清卡
#else
                    Reader_RF reader;
#endif
                    //修改密码成功
                    if(reader.pwdChange(n_func::f_m_defcardpwd, transferpasswd)) {//0
                        reader.setDefPwd(transferpasswd);
                        //写数据
                        if(reader.writecard(ls_cardno, "", "")) {//1

                        }else {//1
                            return QObject::tr("IC卡写入卡号失败")+reader.lastError();
                        }//1
                    } else {//0
                        return QObject::tr("IC卡修改密码失败")+reader.lastError();
                    }//0
                }
            } else if(is_cardtype == "2") {//磁卡
                lst_member.as_cardtype=is_cardtype;
            } else {
                return QObject::tr("发卡类型为无会员卡");
            }

            if(query.execUpdate("t_m_member",qrtVariantPairList()
                                << qrtVariantPair("ch_cardflag", "Y")
                                << qrtVariantPair("dt_operdate", n_func::f_get_sysdatetime())
                                << qrtVariantPair("int_basedata", li_basedata)
                                << qrtVariantPair("dt_sendtime", ldt_sendtime),
                                QString(" vch_memberno = '%1' ").arg(as_memberno))) {
                /*success*/
                return "";
            } else {
                return query.recordError();
            }
        } else {//0
            return QObject::tr("会员(%1)不存在").arg(as_memberno);
        }//0
    }
    return QObject::tr("发卡失败");
}
