
#include "w_m_member.h"
#include "n_func.h"
#include "lds.h"

//会员状态对照表
QMap<QString, QVariant>  w_m_member::VIP_MODEL_STATE;
//持卡状态对照表
QStandardItemModel w_m_member::VIP_MODEL_FLAG;
//会员卡类型对照表
QStandardItemModel w_m_member::VIP_MODEL_ATTRI;
//会员卡优惠方式对照表
QStandardItemModel w_m_member::VIP_MODEL_FAVOUR;
//会员卡会员价
QStandardItemModel w_m_member::VIP_MODEL_M_PRICE;
//会员卡积分
QStandardItemModel w_m_member::VIP_MODEL_CALC;
//会员卡是否取整
QStandardItemModel w_m_member::VIP_MODEL_CALCTYPE;
//会员卡消费积分范围
QStandardItemModel w_m_member::VIP_MODEL_AREA;
//会员卡积分类型
QMap<QString, QVariant> w_m_member::VIP_MODEL_POINT_CH_TYPE;
//会员付款方式
QStandardItemModel w_m_member::RECHARGE_MODEL_PAYMODE;//t_m_member_type ch_area_flag
//充值方式
QStandardItemModel w_m_member::RECHARGE_MODEL_PAYMODE_RECHARGE;//t_m_member_type ch_area_flag
//充值方式
QStandardItemModel w_m_member::RECHARGE_MODEL_PAYMODE_FETCH;//t_m_member_type ch_area_flag
//充值类型
QStandardItemModel w_m_member::RECHARGE_MODEL_CH_DEPOSIT_MODE;
//充值类型
QStandardItemModel w_m_member::CARD_TYPE_MODE;

w_m_member::w_m_member(QWidget *parent) :
    QDialog(parent)
{
}

QStandardItemModel &w_m_member::get_VIP_MODEL_FLAG()
{
    QList<QStandardItem *> items;
    if(VIP_MODEL_FLAG.rowCount() == 0) {
        items.clear();items<<new QStandardItem("N")<<new QStandardItem(QObject::tr("未发卡"));VIP_MODEL_FLAG.appendRow(items);
        items.clear();items<<new QStandardItem("Y")<<new QStandardItem(QObject::tr("已发卡"));VIP_MODEL_FLAG.appendRow(items);
    }
    return VIP_MODEL_FLAG;
}

QStandardItemModel &w_m_member::get_VIP_MODEL_ATTRI()
{
    QList<QStandardItem *> items;
    if(VIP_MODEL_ATTRI.rowCount() == 0) {
        QString ls_value=n_func::f_m_get_parameter(0, "member_ATT_mode", "123");
        //        if(ls_value.contains("2"))
        {
            items.clear();items<<new QStandardItem("2")<<new QStandardItem(QObject::tr("充值卡"));VIP_MODEL_ATTRI.appendRow(items);
        }
        //        if(ls_value.contains("1"))
        {
            items.clear();items<<new QStandardItem("1")<<new QStandardItem(QObject::tr("优惠卡"));VIP_MODEL_ATTRI.appendRow(items);
        }
    }
    return VIP_MODEL_ATTRI;
}

QStandardItemModel &w_m_member::get_VIP_MODEL_FAVOUR()
{
    QList<QStandardItem *> items;
    if(VIP_MODEL_FAVOUR.rowCount() == 0) {
        items.clear();items<<new QStandardItem("0")<<new QStandardItem(QObject::tr("无优惠"));VIP_MODEL_FAVOUR.appendRow(items);
        items.clear();items<<new QStandardItem("1")<<new QStandardItem(QObject::tr("会员特价"));VIP_MODEL_FAVOUR.appendRow(items);
        items.clear();items<<new QStandardItem("2")<<new QStandardItem(QObject::tr("固定折扣"));VIP_MODEL_FAVOUR.appendRow(items);
    }
    return VIP_MODEL_FAVOUR;
}

QStandardItemModel &w_m_member::get_VIP_MODEL_PRICEKING()
{
    QList<QStandardItem *> items;
    if(VIP_MODEL_M_PRICE.rowCount() == 0) {
        items.clear();items<<new QStandardItem("1")<<new QStandardItem(QObject::tr("会员特价1"));VIP_MODEL_M_PRICE.appendRow(items);

    }
    return VIP_MODEL_M_PRICE;
}

QStandardItemModel &w_m_member::get_VIP_MODEL_CALC()
{
    QList<QStandardItem *> items;
    if(VIP_MODEL_CALC.rowCount() == 0) {
        items.clear();items<<new QStandardItem("N")<<new QStandardItem(QObject::tr("不积分"));VIP_MODEL_CALC.appendRow(items);
        items.clear();items<<new QStandardItem("Y")<<new QStandardItem(QObject::tr("积分"));VIP_MODEL_CALC.appendRow(items);

    }
    return VIP_MODEL_CALC;
}

QStandardItemModel &w_m_member::get_VIP_MODEL_CALCTYPE()
{
    QList<QStandardItem *> items;
    if(VIP_MODEL_CALCTYPE.rowCount() == 0) {
        items.clear();items<<new QStandardItem("0")<<new QStandardItem(QObject::tr("保留小数")); VIP_MODEL_CALCTYPE.appendRow(items);
        items.clear();items<<new QStandardItem("1")<<new QStandardItem(QObject::tr("取整")); VIP_MODEL_CALCTYPE.appendRow(items);

    }
    return VIP_MODEL_CALCTYPE;
}

QStandardItemModel &w_m_member::get_VIP_MODEL_AREA()
{
    QList<QStandardItem *> items;
    if(VIP_MODEL_AREA.rowCount() == 0) {
        items.clear();items<<new QStandardItem("0")<<new QStandardItem(QObject::tr("消费额"));VIP_MODEL_AREA.appendRow(items);
        items.clear();items<<new QStandardItem("1")<<new QStandardItem(QObject::tr("折后额"));VIP_MODEL_AREA.appendRow(items);
        items.clear();items<<new QStandardItem("2")<<new QStandardItem(QObject::tr("实际收入"));VIP_MODEL_AREA.appendRow(items);

    }
    return VIP_MODEL_AREA;
}

QMap<QString, QVariant> &w_m_member::get_VIP_MODEL_POINT_CH_TYPE()
{
    if(VIP_MODEL_POINT_CH_TYPE.count() == 0) {
        VIP_MODEL_POINT_CH_TYPE.insert("3", QObject::tr("消费积分"));
        VIP_MODEL_POINT_CH_TYPE.insert("4", QObject::tr("礼品兑换"));
        VIP_MODEL_POINT_CH_TYPE.insert("5", QObject::tr("手工充积分"));
        VIP_MODEL_POINT_CH_TYPE.insert("6", QObject::tr("手工减积分"));
    }
    return VIP_MODEL_POINT_CH_TYPE;
}

//所有的
QStandardItemModel &w_m_member::get_RECHARGE_MODEL_PAYMODE()
{
    QList<QStandardItem *> items;
    if(RECHARGE_MODEL_PAYMODE.rowCount() == 0) {
        items.clear();items<<new QStandardItem("0")<<new QStandardItem(QObject::tr("会员付款"));RECHARGE_MODEL_PAYMODE.appendRow(items);
        items.clear();items<<new QStandardItem("1")<<new QStandardItem(QObject::tr("现金"));RECHARGE_MODEL_PAYMODE.appendRow(items);
        items.clear();items<<new QStandardItem("2")<<new QStandardItem(QObject::tr("银行转帐"));RECHARGE_MODEL_PAYMODE.appendRow(items);
        items.clear();items<<new QStandardItem("3")<<new QStandardItem(QObject::tr("银行支票"));RECHARGE_MODEL_PAYMODE.appendRow(items);
        items.clear();items<<new QStandardItem("5")<<new QStandardItem(QObject::tr("系统付款"));RECHARGE_MODEL_PAYMODE.appendRow(items);
        items.clear();items<<new QStandardItem("6")<<new QStandardItem(QObject::tr("微信"));RECHARGE_MODEL_PAYMODE.appendRow(items);
        items.clear();items<<new QStandardItem("7")<<new QStandardItem(QObject::tr("支付宝"));RECHARGE_MODEL_PAYMODE.appendRow(items);
        items.clear();items<<new QStandardItem("8")<<new QStandardItem(QObject::tr("摇钱树支付"));RECHARGE_MODEL_PAYMODE.appendRow(items);

        items.clear();items<<new QStandardItem("9")<<new QStandardItem(QObject::tr("其他1"));RECHARGE_MODEL_PAYMODE.appendRow(items);
        items.clear();items<<new QStandardItem("10")<<new QStandardItem(QObject::tr("其他2"));RECHARGE_MODEL_PAYMODE.appendRow(items);
        items.clear();items<<new QStandardItem("11")<<new QStandardItem(QObject::tr("其他3"));RECHARGE_MODEL_PAYMODE.appendRow(items);
        items.clear();items<<new QStandardItem("12")<<new QStandardItem(QObject::tr("联拓富"));RECHARGE_MODEL_PAYMODE.appendRow(items);

    }
    return RECHARGE_MODEL_PAYMODE;
}

//充值
QStandardItemModel &w_m_member::get_RECHARGE_MODEL_PAYMODE_RECHARGE()
{
    QList<QStandardItem *> items;
    if(RECHARGE_MODEL_PAYMODE_RECHARGE.rowCount() == 0) {
        items.clear();items<<new QStandardItem("1")<<new QStandardItem(QObject::tr("现金"));RECHARGE_MODEL_PAYMODE_RECHARGE.appendRow(items);
        items.clear();items<<new QStandardItem("2")<<new QStandardItem(QObject::tr("银行转帐"));RECHARGE_MODEL_PAYMODE_RECHARGE.appendRow(items);
        items.clear();items<<new QStandardItem("3")<<new QStandardItem(QObject::tr("银行支票"));RECHARGE_MODEL_PAYMODE_RECHARGE.appendRow(items);
        if(lds::get_soft_curent_language_first() == _CN_) {
            items.clear();items<<new QStandardItem("6")<<new QStandardItem(QObject::tr("微信"));RECHARGE_MODEL_PAYMODE_RECHARGE.appendRow(items);
            items.clear();items<<new QStandardItem("7")<<new QStandardItem(QObject::tr("支付宝"));RECHARGE_MODEL_PAYMODE_RECHARGE.appendRow(items);
            items.clear();items<<new QStandardItem("8")<<new QStandardItem(QObject::tr("摇钱树支付"));RECHARGE_MODEL_PAYMODE_RECHARGE.appendRow(items);
            items.clear();items<<new QStandardItem("12")<<new QStandardItem(QObject::tr("联拓富"));RECHARGE_MODEL_PAYMODE_RECHARGE.appendRow(items);
        }
        items.clear();items<<new QStandardItem("9")<<new QStandardItem(QObject::tr("其他1"));RECHARGE_MODEL_PAYMODE_RECHARGE.appendRow(items);
        items.clear();items<<new QStandardItem("10")<<new QStandardItem(QObject::tr("其他2"));RECHARGE_MODEL_PAYMODE_RECHARGE.appendRow(items);
        items.clear();items<<new QStandardItem("11")<<new QStandardItem(QObject::tr("其他3"));RECHARGE_MODEL_PAYMODE_RECHARGE.appendRow(items);

    }
    return RECHARGE_MODEL_PAYMODE_RECHARGE;
}

//取款
QStandardItemModel &w_m_member::get_RECHARGE_MODEL_PAYMODE_FETCH()
{
    QList<QStandardItem *> items;
    if(RECHARGE_MODEL_PAYMODE_FETCH.rowCount() == 0) {
        items.clear();items<<new QStandardItem("1")<<new QStandardItem(QObject::tr("现金"));RECHARGE_MODEL_PAYMODE_FETCH.appendRow(items);
        items.clear();items<<new QStandardItem("2")<<new QStandardItem(QObject::tr("银行转帐"));RECHARGE_MODEL_PAYMODE_FETCH.appendRow(items);
        items.clear();items<<new QStandardItem("3")<<new QStandardItem(QObject::tr("银行支票"));RECHARGE_MODEL_PAYMODE_FETCH.appendRow(items);

    }
    return RECHARGE_MODEL_PAYMODE_FETCH;
}

//充值模式
QStandardItemModel &w_m_member::get_RECHARGE_MODEL_CH_DEPOSIT_MODE()
{
    QList<QStandardItem *> items;
    if(RECHARGE_MODEL_CH_DEPOSIT_MODE.rowCount() == 0) {
        items.clear();items<<new QStandardItem("1")<<new QStandardItem(QObject::tr("常规充值"));RECHARGE_MODEL_CH_DEPOSIT_MODE.appendRow(items);
        items.clear();items<<new QStandardItem("2")<<new QStandardItem(QObject::tr("开卡充值"));RECHARGE_MODEL_CH_DEPOSIT_MODE.appendRow(items);
        items.clear();items<<new QStandardItem("3")<<new QStandardItem(QObject::tr("付款还现"));RECHARGE_MODEL_CH_DEPOSIT_MODE.appendRow(items);
        items.clear();items<<new QStandardItem("4")<<new QStandardItem(QObject::tr("反结帐单"));RECHARGE_MODEL_CH_DEPOSIT_MODE.appendRow(items);
        items.clear();items<<new QStandardItem("5")<<new QStandardItem(QObject::tr("转帐充值"));RECHARGE_MODEL_CH_DEPOSIT_MODE.appendRow(items);
        items.clear();items<<new QStandardItem("6")<<new QStandardItem(QObject::tr("退卡还现"));RECHARGE_MODEL_CH_DEPOSIT_MODE.appendRow(items);
        items.clear();items<<new QStandardItem("8")<<new QStandardItem(QObject::tr("取款"));RECHARGE_MODEL_CH_DEPOSIT_MODE.appendRow(items);
        items.clear();items<<new QStandardItem("9")<<new QStandardItem(QObject::tr("付款"));RECHARGE_MODEL_CH_DEPOSIT_MODE.appendRow(items);

    }
    return RECHARGE_MODEL_CH_DEPOSIT_MODE;
}

QStandardItemModel &w_m_member::get_CARD_TYPE_MODE()
{
    QList<QStandardItem *> items;
    if(CARD_TYPE_MODE.rowCount() == 0) {
        items.clear();items<<new QStandardItem("无会员卡")<<new QStandardItem(QObject::tr("无会员卡"));CARD_TYPE_MODE.appendRow(items);
        items.clear();items<<new QStandardItem("磁卡")<<new QStandardItem(QObject::tr("磁卡"));CARD_TYPE_MODE.appendRow(items);
        items.clear();items<<new QStandardItem("IC卡")<<new QStandardItem(QObject::tr("IC卡"));CARD_TYPE_MODE.appendRow(items);
    }
    return CARD_TYPE_MODE;
}

QString w_m_member::get_vip_card_type(QWidget *parent,  bool *ok)
{
    QString is_cardtype;
    QString ls_value=n_func::f_m_get_parameter(parent,"vip_card_type", "无会员卡", ok);
    if(ls_value == "IC卡") {
        is_cardtype="1";
    } else if(ls_value == "磁卡") {
        is_cardtype="2";
    } else {//无会员卡
        is_cardtype="4";
    }
    return is_cardtype;
}

bool w_m_member::get_vip_card_type(QString &card_type, QString &errstring)
{
    QString ls_value;
    if(false == n_func::f_m_get_parameter("vip_card_type", ls_value, "无会员卡", errstring)) {
        return false;
    }
    if(ls_value == "IC卡") {
        card_type="1";
    } else if(ls_value == "磁卡") {
        card_type="2";
    } else {//无会员卡
        card_type="4";
    }
    return true;
}

void w_m_member::clearCache()
{
    //会员状态对照表
    w_m_member::VIP_MODEL_STATE.clear();
    //持卡状态对照表
    w_m_member::VIP_MODEL_FLAG.clear();
    //会员卡类型对照表
    w_m_member::VIP_MODEL_ATTRI.clear();
    //会员卡优惠方式对照表
    w_m_member::VIP_MODEL_FAVOUR.clear();
    //会员卡会员价
    w_m_member::VIP_MODEL_M_PRICE.clear();
    //会员卡积分
    w_m_member::VIP_MODEL_CALC.clear();
    //会员卡是否取整
    w_m_member::VIP_MODEL_CALCTYPE.clear();
    //会员卡消费积分范围
    w_m_member::VIP_MODEL_AREA.clear();
    //会员卡积分类型
    w_m_member::VIP_MODEL_POINT_CH_TYPE.clear();
    //会员付款方式
    w_m_member::RECHARGE_MODEL_PAYMODE.clear();//t_m_member_type ch_area_flag
    //充值方式
    w_m_member::RECHARGE_MODEL_PAYMODE_RECHARGE.clear();//t_m_member_type ch_area_flag
    //充值方式
    w_m_member::RECHARGE_MODEL_PAYMODE_FETCH.clear();//t_m_member_type ch_area_flag
    //充值类型
    w_m_member::RECHARGE_MODEL_CH_DEPOSIT_MODE.clear();
}

QMap<QString, QVariant> & w_m_member::get_VIP_MODEL_STATE()
{
    if(VIP_MODEL_STATE.count() == 0) {
        VIP_MODEL_STATE.insert("1", QObject::tr("正常"));
        VIP_MODEL_STATE.insert("2", QObject::tr("冻结"));
        VIP_MODEL_STATE.insert("3", QObject::tr("挂失"));
        VIP_MODEL_STATE.insert("4", QObject::tr("作废"));
        VIP_MODEL_STATE.insert("5", QObject::tr("已退卡"));
    }
    return VIP_MODEL_STATE;
}


//column=(type=char(1) updatewhereclause=yes name=ch_type dbname="ch_type"
// values="常规充值	1/开卡充值	2/付款还现	3/反结帐单	4/转帐充值	5/退卡还现	6/取款	8/付款	9/"  )
//column=(type=char(1) updatewhereclause=yes name=ch_pay_mode dbname="ch_pay_mode"
// values="现金	1/银行转帐	2/银行支票	3/系统付款	5/会员付款	0/"  )
