#ifndef W_M_MEMBER_H
#define W_M_MEMBER_H

#include <QDialog>
#include "lds_query_hddpos.h"
#include <QStandardItemModel>

#include "backheader.h"


//主界面
class w_m_member: public QDialog
{
    Q_OBJECT
public:
    explicit w_m_member(QWidget *parent = 0);
private:
    //会员状态对照表
    static QMap<QString, QVariant> VIP_MODEL_STATE;//t_m_member, ch_state
    //持卡状态对照表
    static QStandardItemModel VIP_MODEL_FLAG;// t_m_member,ch_cardflag
    //会员卡类型对照表
    static QStandardItemModel VIP_MODEL_ATTRI;//t_m_member_type ch_attribute
    //会员卡优惠方式对照表
    static QStandardItemModel VIP_MODEL_FAVOUR;//t_m_member_type ch_favourable
    //会员卡会员价
    static QStandardItemModel VIP_MODEL_M_PRICE;//t_m_member_type ch_calc_type
    //会员卡积分
    static QStandardItemModel VIP_MODEL_CALC;//t_m_member_type ch_calc
    //会员卡是否取整
    static QStandardItemModel VIP_MODEL_CALCTYPE;//t_m_member_type ch_calc_type
    //会员卡消费积分范围
    static QStandardItemModel VIP_MODEL_AREA;//t_m_member_type ch_area_flag
    //会员卡积分类型
    static QMap<QString, QVariant> VIP_MODEL_POINT_CH_TYPE;//t_m_point ch_type
    //充值支付方式
    static QStandardItemModel RECHARGE_MODEL_PAYMODE;//t_m_member_type ch_area_flag
    //充值支付方式
    static QStandardItemModel RECHARGE_MODEL_PAYMODE_RECHARGE;//t_m_member_type ch_area_flag
    //充值其他
    static QStandardItemModel RECHARGE_MODEL_PAYMODE_FETCH;//t_m_member_type ch_area_flag
    //充值类型
    static QStandardItemModel RECHARGE_MODEL_CH_DEPOSIT_MODE;
    //卡类型
    static QStandardItemModel CARD_TYPE_MODE;

public:
    //会员状态对照表
    static QMap<QString, QVariant>& get_VIP_MODEL_STATE();//t_m_member, ch_state
    //持卡状态对照表
    static QStandardItemModel& get_VIP_MODEL_FLAG();// t_m_member,ch_cardflag
    //会员卡类型对照表
    static QStandardItemModel& get_VIP_MODEL_ATTRI();//t_m_member_type ch_attribute
    //会员卡优惠方式对照表
    static QStandardItemModel& get_VIP_MODEL_FAVOUR();//t_m_member_type ch_favourable
    //会员卡会员价
    static QStandardItemModel& get_VIP_MODEL_PRICEKING();//t_m_member_type ch_calc_type
    //会员卡积分
    static QStandardItemModel& get_VIP_MODEL_CALC();//t_m_member_type ch_calc
    //会员卡是否取整
    static QStandardItemModel& get_VIP_MODEL_CALCTYPE();//t_m_member_type ch_calc_type
    //会员卡消费积分范围
    static QStandardItemModel& get_VIP_MODEL_AREA();//t_m_member_type ch_area_flag
    //会员卡积分类型
    static QMap<QString, QVariant>& get_VIP_MODEL_POINT_CH_TYPE();//t_m_point ch_type
    //充值支付方式
    static QStandardItemModel& get_RECHARGE_MODEL_PAYMODE();//t_m_member_type ch_area_flag
    //充值支付方式
    static QStandardItemModel& get_RECHARGE_MODEL_PAYMODE_RECHARGE();//t_m_member_type ch_area_flag
    //充值其他
    static QStandardItemModel& get_RECHARGE_MODEL_PAYMODE_FETCH();//t_m_member_type ch_area_flag
    //充值类型
    static QStandardItemModel& get_RECHARGE_MODEL_CH_DEPOSIT_MODE();
    //卡类型
    static QStandardItemModel& get_CARD_TYPE_MODE();
    //卡类型index
    static QString get_vip_card_type(QWidget *parent, bool *ok = 0);
    static bool get_vip_card_type(QString &card_type, QString &errstring);

    static void clearCache();

    //querymodelsetfilters("select * from cey_bt_table")
};

#endif // W_M_MEMBER_H
