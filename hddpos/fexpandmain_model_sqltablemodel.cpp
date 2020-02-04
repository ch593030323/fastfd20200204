#include "fexpandmain_model_sqltablemodel.h"
#include <QTableView>
#include <QHeaderView>
#include <QFile>
#include <QSqlRecord>
#include <QNetworkRequest>
#include "lds_scr_table.h"
#include "lds_scr_qt.h"
#include "lds_network.h"
#include "fexpandmain_model_sqltablemodel_data.h"
#include "backheader.h"
#include "w_bt_dish.h"
#include "w_sys_manage_operatorright.h"


fexpandmain_model_sqltablemodel::fexpandmain_model_sqltablemodel(QObject *parent)
    : lds_model_sqltablemodel(parent)
{
}

fexpandmain_model_sqltablemodel::~fexpandmain_model_sqltablemodel()
{

}

QString fexpandmain_model_sqltablemodel::get_dish_desc(int row) const
{
    QString text = "";
    if("Y"== model_data(row, "ch_presentflag").toString())text.append(QObject::tr("赠")+"  ");
    if("Y"== model_data(row, "ch_specialflag").toString())text.append(QObject::tr("特")+"  ");
    if("Y"== model_data(row, "ch_waidai_flag").toString())text.append(QObject::tr("外带")+"  ");
    if(100 > model_data(row, "int_discount").toInt()) {
        if(discount_off_mode()) {//折让
            text.append(QObject::tr("折扣") + "(" + QString::number(100 - model_data(row, "int_discount").toInt())+"%)  ");
        } else {
            text.append(QObject::tr("折扣") + "(" + QString::number(model_data(row, "int_discount").toInt())+"%)  ");
        }
    }

    return text;
}

bool fexpandmain_model_sqltablemodel::can_m_price(const fexpandmain_model_sqltablemodel *m, int row, QString *errstring)
{
    /// 会员价：～赠送、～子商品,   ~ 特价
    if("Y" == m->model_data(row, "ch_presentflag")) {
        if(errstring) *errstring = "赠送商品不能修改会员特价";
        return false;
    }
    if("Y" == m->model_data(row, "ch_suitflag")) {
        if(errstring) *errstring = "子商品不能修改会员特价";
        return false;
    }
    if("Y" == m->model_data(row, "ch_specialflag")) {
        if(errstring) *errstring = "特价商品不能修改会员特价";
        return false;
    }
    return true;
}

QString fexpandmain_model_sqltablemodel::get_vch_dishname_by_map(const QString &ch_dishno) const
{
    return this->tablename_kvmap_value("cey_bt_dish", ch_dishno, "ch_dishno", "vch_dishname").toString();
}

QString fexpandmain_model_sqltablemodel::get_ch_curprice_by_map(const QString &ch_dishno) const
{
    return this->tablename_kvmap_value("cey_bt_dish", ch_dishno, "ch_dishno", "ch_curprice").toString();
}

QString fexpandmain_model_sqltablemodel::get_ch_discount_by_map(const QString &ch_dishno) const
{
    return this->tablename_kvmap_value("cey_bt_dish", ch_dishno, "ch_dishno", "ch_discount").toString();
}

double fexpandmain_model_sqltablemodel::get_num_m_price_by_map(const QString &ch_dishno) const
{
    return this->tablename_kvmap_value("cey_bt_dish", ch_dishno, "ch_dishno", "num_m_price").toDouble();
}

QString fexpandmain_model_sqltablemodel::get_ch_serviceflag_by_map(const QString &ch_dishno) const
{
    return this->tablename_kvmap_value("cey_bt_dish", ch_dishno, "ch_dishno", "ch_serviceflag").toString();
}

QString fexpandmain_model_sqltablemodel::get_item_flag_by_map(const QString &ch_dishno) const
{
    return this->tablename_kvmap_value("cey_bt_dish", ch_dishno, "ch_dishno", "item_flag").toString();
}

QVariant fexpandmain_model_sqltablemodel::get_cey_bt_dish_value(const QString &ch_dishno, const QString &valuename) const
{
    return this->tablename_kvmap_value("cey_bt_dish", ch_dishno, "ch_dishno", valuename);
}

QVariant fexpandmain_model_sqltablemodel::get_cey_u_taxsale_master_value(const QString &ch_billno, const QString &valuename) const
{
    return this->tablename_kvmap_value("cey_u_taxsale_master", ch_billno, "ch_billno", valuename);
}

double fexpandmain_model_sqltablemodel::get_ch_dishno_rate(const QString &ch_billno, const QString &ch_dishno) const
{
    if(0 == get_cey_u_taxsale_master_value(ch_billno, "ch_tax_type").toInt()) {
        return get_cey_bt_dish_value(ch_dishno, "int_rate").toDouble();
    }

    return get_cey_u_taxsale_master_value(ch_billno, "int_fix_tax").toDouble()
            + get_cey_u_taxsale_master_value(ch_billno, "int_fix_tax_2").toDouble()
            + get_cey_u_taxsale_master_value(ch_billno, "int_fix_tax_3").toDouble();
}

QString fexpandmain_model_sqltablemodel::get_cey_u_taxsale_master_desc(const QString &ch_billno)
{
    int ch_tax_type = get_cey_u_taxsale_master_value(ch_billno, "ch_tax_type").toInt();
    double tax_1 = get_cey_u_taxsale_master_value(ch_billno, "int_fix_tax").toDouble();
    double tax_2 = get_cey_u_taxsale_master_value(ch_billno, "int_fix_tax_2").toDouble();
    double tax_3 = get_cey_u_taxsale_master_value(ch_billno, "int_fix_tax_3").toDouble();
    double tax = tax_1 + tax_2 + tax_3;

    if(ch_tax_type == 0) {
        return QObject::tr("税率类型")  + ":" + QObject::tr("不启用");
    }
    if(ch_tax_type == 1) {
        return QObject::tr("税率类型")  + ":" + QObject::tr("单一") + "\n" + QObject::tr("当前税率") + ":" + lds::septNumber(tax) + "%";
    }
    if(ch_tax_type == 2) {
        return QObject::tr("税率类型")  + ":" + QObject::tr("三选一") + "\n" + QObject::tr("当前税率") + ":" +  lds::septNumber(tax) + "%";
    }
    if(ch_tax_type == 3) {
        return QObject::tr("税率类型")  + ":" + QObject::tr("累加") + "\n" + QObject::tr("当前税率") + ":" +  lds::septNumber(tax) + "%";
    }
    return "";
}

QVariant fexpandmain_model_sqltablemodel::get_cey_bt_unit_value(const QString &ch_unitno, const QString &valuename) const
{
    return this->tablename_kvmap_value("cey_bt_unit", ch_unitno, "ch_unitno", valuename);
}

QString fexpandmain_model_sqltablemodel::get_sevice_mode_by_map(const QString &ch_tableno) const
{
    QString ch_typeno = get_cey_bt_table_value(ch_tableno, "ch_typeno").toString();
    return this->tablename_kvmap_value("cey_bt_table_type", ch_typeno, "ch_typeno", "ch_service_mode").toString();
}

double fexpandmain_model_sqltablemodel::get_sevice_int_rate_by_map(const QString &ch_tableno) const
{
    QString ch_typeno = get_cey_bt_table_value(ch_tableno, "ch_typeno").toString();
    return this->tablename_kvmap_value("cey_bt_table_type", ch_typeno, "ch_typeno", "int_rate").toDouble();
}

QVariant fexpandmain_model_sqltablemodel::get_cey_bt_table_type_value(const QString &ch_typeno, const QString &valuename) const
{
    return this->tablename_kvmap_value("cey_bt_table_type", ch_typeno, "ch_typeno", valuename);
}

QVariant fexpandmain_model_sqltablemodel::get_cey_bt_table_value(const QString &ch_tableno, const QString &valuename) const
{
    return this->tablename_kvmap_value("cey_bt_table", ch_tableno, "ch_tableno",  valuename);
}

QVariant fexpandmain_model_sqltablemodel::get_cey_u_table_value(const QString &ch_billno, const QString &valuename) const
{
    return this->tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", valuename).toString();
}

QVariant fexpandmain_model_sqltablemodel::get_cey_bt_paymode_value(const QString &ch_paymodeno, const QString &valuename) const
{
    return this->tablename_kvmap_value("cey_bt_paymode", ch_paymodeno, "ch_paymodeno", valuename);
}

QVariant fexpandmain_model_sqltablemodel::get_cey_sys_parameter_value(const QString &vch_parameter, const QVariant &def_value) const
{
    return this->tablename_kvmap_value("cey_sys_parameter", vch_parameter, "vch_parameter", "vch_value", "", def_value);
}

double fexpandmain_model_sqltablemodel::num_dec(double value) const
{
    return lds::doubleRound(value, get_cey_sys_parameter_value("bp_fmt_num_dec", 3).toInt());
}

double fexpandmain_model_sqltablemodel::price_dec(double value) const
{
    return lds::doubleRound(value, get_cey_sys_parameter_value("bp_fmt_price_dec", 2).toInt());
}

double fexpandmain_model_sqltablemodel::amount_dec(double value) const
{
    return lds::doubleRound(value, get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
}

QString fexpandmain_model_sqltablemodel::get_vch_cp_memo_by_map(const QString &ch_no) const
{
    return this->tablename_kvmap_value("cey_bt_cp_memo", ch_no, "ch_no", "vch_cp_memo").toString();
}

QString fexpandmain_model_sqltablemodel::get_num_add_price_by_map(const QString &ch_no) const
{
    return this->tablename_kvmap_value("cey_bt_cp_memo", ch_no, "ch_no", "num_add_price").toString();
}

bool fexpandmain_model_sqltablemodel::can_discount(int row, QString *errstring) const
{
    QString ch_dishno = model_data(row, "ch_dishno").toString();
    /// 折扣：折扣商品、~赠送，～子商品,   ~ 特价

    if("Y" != get_ch_discount_by_map(ch_dishno)) {
        if(errstring) *errstring = "非折扣商品不能修改折扣";
        return false;
    }
    if("Y" == model_data(row, "ch_presentflag")) {
        if(errstring) *errstring = "赠送商品不能修改折扣";
        return false;
    }
    if("Y" == model_data(row, "ch_suitflag")) {
        if(errstring) *errstring = "子商品不能修改折扣";
        return false;
    }
    if("Y" == model_data(row, "ch_specialflag")) {
        if(errstring) *errstring = "特价商品不能修改折扣";
        return false;
    }
    return true;
}

bool fexpandmain_model_sqltablemodel::can_m_price(int row, QString *errstring) const
{
    return can_m_price(this, row, errstring);
}

bool fexpandmain_model_sqltablemodel::can_change_price(int row, QString *errstring) const
{
    /// 修改价格：~特价、~子商品、~赠送、~时价商品
    ///
    if("Y" == model_data(row, "ch_specialflag").toString()) {
        if(errstring) *errstring = "特价商品不能修改单价";
        return false;
    }
    if("Y" == model_data(row, "ch_suitflag").toString()) {
        if(errstring) *errstring = "子商品不能修改单价";
        return false;
    }
    if("Y" == model_data(row, "ch_presentflag").toString()) {
        if(errstring) *errstring = "已赠送商品不能修改单价";
        return false;
    }
    if(false == backheader::operate_right_check("0816", lds::gs_operid)) {
        if(errstring) *errstring = "没有改价权限";
        return false;
    }
//    if("Y" != get_ch_curprice_by_map(model_data(row, "ch_dishno").toString()) ) {
//        if(errstring) *errstring = "非时价商品不能修改单价";
//        return false;
//    }
    return true;
}

bool fexpandmain_model_sqltablemodel::can_change_num(int row, QString *errstring) const
{
    if(this->opera(row) != lds_model_sqltablemodel::OpInsert) {
        if(errstring) *errstring = "已落单不能修数量";
        return false;
    }
    if(model_data(row, "ch_suitflag").toString() == "Y") {
        if(errstring) *errstring = "子商品不能修数量";
        return false;
    }
    if(model_data(row, "ch_specialflag").toString() == "Y") {
        if(errstring) *errstring = "特价商品不能修数量";
        return false;
    }
    return true;
}

bool fexpandmain_model_sqltablemodel::can_change_dish(int row, QString *errstring) const
{
    /// 替换：~落单、~非特价、~非主商品、~赠送
    if(this->opera(row) != lds_model_sqltablemodel::OpInsert) {
        if(errstring) *errstring = QObject::tr("商品已落单");
        return false;
    }
    if(model_data(row, "ch_specialflag").toString() == "Y") {
        if(errstring) *errstring = QObject::tr("商品为特价");
        return false;
    }
    if(can_qt_P_dish_fold(row)) {// model_data(row, "ch_suitflag").toString() == "P") {
        if(errstring) *errstring = QObject::tr("商品为主商品");
        return false;
    }
    if(model_data(row, "ch_presentflag").toString() == "Y") {
        if(errstring) *errstring = QObject::tr("商品已被赠送");
        return false;
    }
    return true;
}

bool fexpandmain_model_sqltablemodel::can_org_dish_append(int row, QString *errstring) const
{
    ///可以追加：
    if(!model_data(row, "vch_print_memo").toString().isEmpty()) {
        if(errstring) *errstring = QObject::tr("商品已有做法");
        return false;
    }
    if(model_data(row, "ch_suitflag").toString() == "P") {
        for(int rowY = row + 1; rowY < this->rowCount(); rowY++) {
            if(model_data(rowY, "ch_suitflag").toString() == "Y") {
                if(!model_data(rowY, "vch_print_memo").toString().isEmpty()) {
                    if(errstring) *errstring = QObject::tr("子商品已有做法");
                    return false;
                }
                continue;
            }
            break;
        }
    }
    if(model_data(row, "num_price_add").toDouble() != 0) {
        if(errstring) *errstring = QObject::tr("商品已有做法");
        return false;
    }

    if("Y" == model_data(row, "ch_presentflag").toString()) {
        if(errstring) *errstring = QObject::tr("赠送商品");
        return false;
    }
    if("Y" == model_data(row, "ch_suitflag").toString()) {
        if(errstring) *errstring = QObject::tr("子商品");
        return false;
    }
    if("Y" == model_data(row, "ch_specialflag").toString()) {
        if(errstring) *errstring = QObject::tr("特价商品");
        return false;
    }
    if(model_data(row, "num_price") != model_data(row, "num_price_org") ) {
        if(errstring) *errstring = QObject::tr("商品现价不等于原价");
        return false;
    }
    if(100 != model_data(row, "int_discount").toInt()) {
        if(errstring) *errstring = QObject::tr("折扣不等于100");
        return false;
    }
    if(this->opera(row) != lds_model_sqltablemodel::OpInsert) {//已落单
        return false;
    }
    return true;
}

bool fexpandmain_model_sqltablemodel::can_present(int row, QString *errstring) const
{
    if("Y" == model_data(row, "ch_suitflag").toString()) {
        if(errstring) *errstring = QObject::tr("子商品不能赠送");
        return false;
    }
    return true;
}

bool fexpandmain_model_sqltablemodel::can_cook(int row, QString *errstring) const
{
    if(this->opera(row) != lds_model_sqltablemodel::OpInsert) {//已落单
        if(errstring) *errstring = "已落单不能设置做法";
        return false;
    }
    if(model_data(row, "ch_suitflag").toString() == "P") {
        if(errstring) *errstring = "主商品不能设置做法";
        return false;
    }
    return true;
}

bool fexpandmain_model_sqltablemodel::can_cook_pop(const QString &ch_dishno) const
{
    lds_query_hddpos  query;
    //非主商品
    query.execSelect(QString("SELECT ch_dishno from cey_bt_dish where ch_dishno = '%1' and ifnull(ch_suitflag, '') <> 'Y' ").arg(ch_dishno));
    if(query.next()) {
        //有做法, 可能添加了做法，但是该做法在memo里没有找到
        query.execSelect(QString("select a.ch_no,a.vch_cp_memo,a.ch_typeno,a.num_add_price FROM cey_bt_cp_memo a,cey_bt_cp_dish b WHERE (a.ch_no = b.ch_no) and ( b.ch_dishno = '%1')")
                         .arg(ch_dishno));
        if(query.next()) {
            return true;
        }
    }
    return false;
}

bool fexpandmain_model_sqltablemodel::can_addon(int row, QString *errstring) const
{
    if(this->opera(row) != lds_model_sqltablemodel::OpInsert) {//已落单
        if(errstring) *errstring = "已落单不能设置加料加价";
        return false;
    }
    if(model_data(row, "ch_suitflag").toString() != "N") {
        if(errstring) *errstring = "已是套菜不能设置加料加价";
        return false;
    }
    if(model_data(row, "ch_addon_flag").toString() == "Y") {
        if(errstring) *errstring = "已是加料加价不能设置加料加价";
        return false;
    }
    return true;
}

bool fexpandmain_model_sqltablemodel::can_qt_Y_dish_fold(int row) const
{
    return model_data(row, "ch_suitflag").toString() == "Y" || model_data(row, "ch_addon_flag").toString() == "Y";
}

bool fexpandmain_model_sqltablemodel::can_qt_P_dish_fold(int row) const
{
    return model_data(row, "ch_suitflag").toString() == "P" || model_data(row, "ch_addon_flag").toString() == "P";
}

void fexpandmain_model_sqltablemodel::update_num_price_add_2_P(const int &row)
{
    int rowP = row;
    do {
        if(model_data(rowP, "ch_suitflag").toString() == "P")
            break;
        rowP--;
    } while(rowP >= 0);

    float num_price_add_P= 0;
    for(int k = rowP+1; k < rowCount(); k++) {
        if(model_data(k, "ch_suitflag").toString() == "Y") {
            num_price_add_P += vch_print_memoData(model_data(k, "vch_print_memo").toString()).total();
            continue;
        }
        break;
    }
    model_data_set(rowP, "num_price_add", num_price_add_P);
}

void fexpandmain_model_sqltablemodel::update_num_num_Y(float num_num_P_old, const int &rowP)
{
    if(num_num_P_old == 0) {
        qDebug() << __LINE__ << "num_num_P_old is 0";
    }
    float num_num_P_new = model_data_NIBOLAN(rowP, "num_num - num_back");
    float rate_P_new_old = num_num_P_new / num_num_P_old;
    for(int row = rowP+1; row < rowCount(); row++) {
        if(model_data(row, "ch_suitflag").toString() == "Y") {
            model_data_set(row, "num_num", model_data_NIBOLAN(row, "num_num-num_back") * rate_P_new_old);
            continue;
        }
        break;
    }
}

void fexpandmain_model_sqltablemodel::update_vch_print_memo_Y(float num_num_old_P, const int &rowP)
{
    if(num_num_old_P == 0) {
        qDebug() << __LINE__ << "num_num_P_old is 0";
        return;
    }
    float num_num_P_new = model_data_NIBOLAN(rowP, "num_num - num_back");
    float rate_P_new_old = num_num_P_new / num_num_old_P;
    for(int row = rowP+1; row < rowCount(); row++) {
        if(model_data(row, "ch_suitflag").toString() == "Y") {

            vch_print_memoData d(model_data(row, "vch_print_memo").toString());
            model_data_set(row, "vch_print_memo", d.sqlValue(rate_P_new_old));

            continue;
        }
        break;
    }
}

void fexpandmain_model_sqltablemodel::update_vch_print_memo_N(float num_num_old_N, const int &row_N)
{
    if(num_num_old_N == 0) {
        qDebug() << "num_num_old_N == 0";
        return;
    }
    float num_num_new_N = model_data_NIBOLAN(row_N, "num_num - num_back");
    float rate_new_old = num_num_new_N / num_num_old_N;

    vch_print_memoData d(model_data(row_N, "vch_print_memo").toString());
    model_data_set(row_N, "vch_print_memo", d.sqlValue(rate_new_old));
}

bool fexpandmain_model_sqltablemodel::discount_off_mode()
{
    return lds::get_soft_curent_language_first() != _CN_;
}

double fexpandmain_model_sqltablemodel::pay_low(const QString &ch_tableno, int int_person) const
{
    if(ch_tableno != lds_scr_table::Q000) {
        QString ch_typeno = get_cey_bt_table_value(ch_tableno, "ch_typeno").toString();
        int ch_low_mode = get_cey_bt_table_type_value(ch_typeno, "ch_low_mode").toInt();
        double num_lowcost = get_cey_bt_table_type_value(ch_typeno, "num_lowcost").toDouble();

        if(1 == ch_low_mode) {//餐桌低消
            return num_lowcost;
        }
        if(2 == ch_low_mode) {//人均低消
            return num_lowcost * int_person;
        }
    }
    return 0;
}

double fexpandmain_model_sqltablemodel::pay_without_low_service(int ch_tax_mode, double consum, double present, double discount, double num_rate)
{
    switch(ch_tax_mode) {
    case 1://含税,不含服务费
        return consum - present  - discount;
    case 2://不含税,含服务费
        return consum - present  - discount + num_rate;
    case 3://含税,含服务费
        return consum - present  - discount;
    case 0://不含税,不含服务费
    default:
        return consum - present  - discount + num_rate;
    }

    return 0;
}

double fexpandmain_model_sqltablemodel::roundBySystem(const fexpandmain_model_sqltablemodel *cache, double value)
{
    //1
    QString str;
    QString type = cache->get_cey_sys_parameter_value("blotout_round").toString();
    if(type == "1")//抹零
    {
        str = cache->get_cey_sys_parameter_value("blotout_mode").toString();
        if(str == "2")//到元
        {
            return(int(value));//2.63 -> 2
        }
        else if(str == "3")//到十元
        {
            return(int(value)/10*10);//26.36->20
        }
        else if(str == "4")//到百元
        {
            return(int(value)/100*100);//26.36
        }
    } else {//四舍五入
        str = cache->get_cey_sys_parameter_value("round_mode").toString();
        if(str == "2")//到角12.36 -> 12.4
        {
            return(qRound(value*10)/10.0);//
        }
        else if(str == "3")//到元
        {
            return(qRound(value));
        }
        else if(str == "4")//到十元
        {
            return(qRound(value/10)*10);
        }
        else if(str == "5")//到五厘
            /*
q
        0.00--------0.00
        0.01--------0.00
        0.02--------0.00
        0.03--------0.05
        0.04--------0.05
        0.05--------0.05
        0.06--------0.05
        0.07--------0.05
        0.08--------0.10
        0.09--------0.10
*/
        {
            int v = qRound(value * 100);//value:2.69999, v:270
            switch(v % 10) {
            case 0:
            case 1:
            case 2:
                return (int(v / 10.0) + 0.0)/10.0;//2.70

            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                return (int(v / 10.0) + 0.5)/10.0;//

            case 8:
            case 9:
                return (int(v / 10.0) + 1.0)/10.0;
            }
        }
    }
    return(value);
}

double fexpandmain_model_sqltablemodel::roundBySystem(double value) const
{
    return roundBySystem(this, value);
}

bool fexpandmain_model_sqltablemodel::select()
{
    return lds_model_sqltablemodel::select();
}

fexpandmain_keepData::fexpandmain_keepData()
///变化
    :    consum(0)
    ,    discount(0)
    ,    present(0)
    ,    num(0)
    ,    num_service(0)

    ,    num_rate_before(0)
    ,    num_rate1(0)
    ,    num_rate2(0)
    ,    num_rate3(0)

    ,    num_price_add(0)
    ///～变化

    ,    num_pay_low(0)
    ,    ch_service_mode(0)
    ,    num_ration(0)
{
}

void fexpandmain_keepData::print() const
{
#ifdef QT_DEBUG
    qDebug() << "consum"<<consum;
    qDebug() << "discount"<<discount;
    qDebug() << "present"<<present;
    qDebug() << "num"<<num;
    qDebug() << "num_service"<<num_service;

    qDebug() << "num_rate_before"<<num_rate_before;
    qDebug() << "num_rate1"<<num_rate1;
    qDebug() << "num_rate2"<<num_rate2;
    qDebug() << "num_rate3"<<num_rate3;

    qDebug() << "num_price_add"<<num_price_add;
#endif
}

double fexpandmain_keepData::dish_total() const
{
    return consum - discount - present;
}

double fexpandmain_keepData::pay_total(const fexpandmain_model_sqltablemodel_data *m) const
{
    QString ch_tableno = m->cur_ch_tableno();
    QString ch_billno = m->cur_ch_billno();
    int ch_tax_mode = m->get_cey_u_taxsale_master_value(ch_billno, "ch_tax_mode").toInt();
    double service = pay_service(ch_billno);
    double total =  fexpandmain_model_sqltablemodel::pay_without_low_service(ch_tax_mode, consum, present, discount,num_rate());
    double pay_lowcost =  qMax(double(0), num_pay_low - total - service);
    total = total + pay_lowcost + service;
    double v = fexpandmain_model_sqltablemodel::roundBySystem(m, total);
    double num_blotout = total - v;

    pay_msg =
            "<p>" + QObject::tr("应收") + "\t:" +  lds::septNumber(v, 2) + "</p>" +
            "<p>" + QObject::tr("消费") + "\t:" +  lds::septNumber(consum, 2) + "</p>" +
            "<p>" + QObject::tr("折扣") + "\t:" +  lds::septNumber(discount, 2) + "</p>" +
            "<p>" + QObject::tr("赠送") + "\t:" +  lds::septNumber(present, 2) + "</p>";
    if(num_blotout != 0)
        pay_msg +=
                "<p>" + QObject::tr("抹零") + "\t" + lds::septNumber(num_blotout, 2) + "</p>";
    if(ch_tax_mode == 1
            || ch_tax_mode == 3)
        pay_msg +=
                "<p><span style=\" text-decoration: underline;\">" + QObject::tr("价格含税") +"\t:" +  lds::septNumber(num_rate(), 2) + "</span></p>";
    else
        pay_msg +=
                "<p><span style=\" text-decoration: underline;\">" + QObject::tr("税率") +"\t:" +  lds::septNumber(num_rate(), 2) + "</span></p>";
    if(ch_tableno != lds_scr_table::Q000) {
        pay_msg +=
                "<p>" + QObject::tr("服务费") + "\t:" + lds::septNumber(service, 2) + "</p>" +
                "<p>" + QObject::tr("低消") + "\t:" + lds::septNumber(pay_lowcost, 2) + "</p>"
                ;
    }
    return v;
}

double fexpandmain_keepData::dish_num() const
{
    return num;
}

double fexpandmain_keepData::pay_service(const QString &ch_billno) const
{
    double service = 0;
    //手动设置服务费
//    if(ch_service_manual)
//        return num_service_manual;

    if(4 == ch_service_mode) {//商品消费服务率
        service = num_service;
    }
    if(3 == ch_service_mode) {//餐桌定额
        service = num_ration;
    }
    if(1 == ch_service_mode) {//按时间收费
        QDateTime begin = lds_query_hddpos::selectValue(QString("select dt_operdate from cey_u_table where ch_billno = '%1' ").arg(ch_billno)).toDateTime();
        QDateTime end = lds_query_hddpos::selectValue(QString("select now() ")).toDateTime();
        int second_off = begin.msecsTo(end) / 1000;
        qDebug()<< begin.msecsTo(end) / 1000 / 60;
        QString ch_tableno = lds_query_hddpos::selectValue(QString(" select ch_tableno from cey_u_table where ch_billno = '%1' " ).arg(ch_billno)).toString();
        QString ch_typeno = lds_query_hddpos::selectValue(QString(" select ch_typeno from cey_bt_table where ch_tableno = '%1' " ).arg(ch_tableno)).toString();
        int int_unit_minute = lds_query_hddpos::selectValue(QString(" select int_unit_minute from cey_bt_table_type where ch_typeno = '%1' " ).arg(ch_typeno)).toInt();
        double num_unit_amount = lds_query_hddpos::selectValue(QString(" select num_unit_amount from cey_bt_table_type where ch_typeno = '%1' " ).arg(ch_typeno)).toDouble();

        int second_block = 60 * int_unit_minute;
        int minute_off = (qMax(1, second_off) + second_block - 1) / second_block;
        service = minute_off * num_unit_amount;
    }
    return service;
}

QString fexpandmain_keepData::pay_service_desc(const QString &ch_billno) const
{
    QString msg;
    msg += backheader::modelValue(&w_bt_dish::get_MODEL_DISH_SERVICE_MODE_CUT(), QString::number(ch_service_mode)) + "\n";
    double service = 0;
    //手动设置服务费
//    if(ch_service_manual)
//        return QObject::tr("手工处理服务费") + ":" + QString().sprintf("%.02f", num_service_manual);

    if(4 == ch_service_mode) {//商品消费服务率
        service = num_service;
        msg += QObject::tr("服务费") + ":" + QString().sprintf("%.02f", service) + "\n";
    }
    if(3 == ch_service_mode) {//餐桌定额
        service = num_ration;
        msg += QObject::tr("服务费") + ":" + QString().sprintf("%.02f", service) + "\n";
    }

    if(1 == ch_service_mode) {//按时间收费
        QDateTime begin = lds_query_hddpos::selectValue(QString("select dt_operdate from cey_u_table where ch_billno = '%1' ").arg(ch_billno)).toDateTime();
        QDateTime end = lds_query_hddpos::selectValue(QString("select now() ")).toDateTime();
        int second_off = begin.msecsTo(end) / 1000;
        QString ch_tableno = lds_query_hddpos::selectValue(QString(" select ch_tableno from cey_u_table where ch_billno = '%1' " ).arg(ch_billno)).toString();
        QString ch_typeno = lds_query_hddpos::selectValue(QString(" select ch_typeno from cey_bt_table where ch_tableno = '%1' " ).arg(ch_tableno)).toString();
        int int_unit_minute = lds_query_hddpos::selectValue(QString(" select int_unit_minute from cey_bt_table_type where ch_typeno = '%1' " ).arg(ch_typeno)).toInt();
        double num_unit_amount = lds_query_hddpos::selectValue(QString(" select num_unit_amount from cey_bt_table_type where ch_typeno = '%1' " ).arg(ch_typeno)).toDouble();

        if(int_unit_minute == 0) {
            msg = "int_unit_minute cannot be 0";
            return 0;
        }
        int mintue_off = (second_off + 60 - 1) / 60;
        int minute_block_off = (qMax(1, mintue_off) + int_unit_minute - 1) / int_unit_minute;
        service = minute_block_off * num_unit_amount;

        msg += QObject::tr("开始时间") + ":" + begin.toString(yyyyMMddhhmmss) + "\n";
        msg += QObject::tr("结束时间") + ":" + end.toString(yyyyMMddhhmmss) + "\n";
        msg += QObject::tr("时长") + "(" + QObject::tr("分钟") + ")" + ":" + QString::number(mintue_off) + "\n";
        msg += QObject::tr("时长") + "(" + QString::number(int_unit_minute) + QObject::tr("分钟") + ")" + ":" + QString::number(minute_block_off) + "\n";
        msg += QObject::tr("收费单位") + "(" + QObject::tr("分钟") + ")" + ":" + QString::number(int_unit_minute) + "\n";
        msg += QObject::tr("单位时间收费额") + ":" + QString().sprintf("%.02f", num_unit_amount) + "\n";
        msg += QObject::tr("服务费") + ":" + QString().sprintf("%.02f", service) + "\n";
    }
    return msg;
}

double fexpandmain_keepData::num_rate() const
{
    return num_rate1 + num_rate2 + num_rate3;
}

double fexpandmain_keepData::num_rate()
{
    return num_rate1 + num_rate2 + num_rate3;
}

fexpandmain_keepData fexpandmain_keepData::operator-(const fexpandmain_keepData &other)
{
    consum -= other.consum;
    discount -= other.discount;
    present -= other.present;
    num -= other.num;
    num_service -= other.num_service;

    num_rate_before -= other.num_rate_before;
    num_rate1 -= other.num_rate1;
    num_rate2 -= other.num_rate2;
    num_rate3 -= other.num_rate3;

    num_price_add -= other.num_price_add;

    return *this;
}

fexpandmain_keepData fexpandmain_keepData::operator+(const fexpandmain_keepData &other)
{
    consum += other.consum;
    discount += other.discount;
    present += other.present;
    num += other.num;
    num_service += other.num_service;

    num_rate_before += other.num_rate_before;
    num_rate1 += other.num_rate1;
    num_rate2 += other.num_rate2 ;
    num_rate3 += other.num_rate3;

    num_price_add += other.num_price_add;

    return *this;
}

vch_membernoData::vch_membernoData()
{
    init();
}

void vch_membernoData::init()
{
    //    ch_attribute = "";//1：优惠卡、2充值卡
    //    ch_favourable = "";//1会员价, 2固定折扣
    //    ch_pricekind = "";
    //    int_discount = -1;
    //    ch_member_belong_state = "";
    //    vch_memberno = "";
    //    vch_typename = "";
    init0("", "", "","", -1, "", "", "");
}

void vch_membernoData::init0(const QString &vch_memberno, const QString &vch_member, const QString &ch_member_belong_state, const QString &ch_favourable, int int_discount, const QString &ch_pricekind, const QString &ch_attribute, const QString &vch_typename)
{
    this->ch_attribute = ch_attribute;//1：优惠卡、2充值卡
    this->ch_favourable = ch_favourable;//1会员价, 2固定折扣
    this->ch_pricekind = ch_pricekind;
    this->int_discount = int_discount;
    this->ch_member_belong_state = ch_member_belong_state;
    this->vch_memberno = vch_memberno;
    this->vch_member = vch_member;
    this->vch_typename = vch_typename;
}

//                  福

bool vch_membernoData::init_by_memberno(const QString &memberno, const QString &ch_member_belong_state, QString &errstring)
{
    lds_query_hddpos  query;
    QString ch_typeno;
    QByteArray readall;
    int httpcode;
    this->vch_memberno = memberno;
    this->ch_member_belong_state = ch_member_belong_state;

    init();
    if(vch_memberno.isEmpty()) {
        return true;
    }
    if("Y" == ch_member_belong_state) {//云会员
        QNetworkRequest request;
        request.setUrl(http_www_yqspos_com +"/cvs/member/dukFuk-rest");
        request.setRawHeader("orgCode",lds_scr_table::cey_sys_parameter_value(query, "orgCode").toByteArray());
        request.setRawHeader("userName",lds_scr_table::cey_sys_parameter_value(query, "cloud_user").toByteArray());
        request.setRawHeader("password",lds_scr_table::cey_sys_parameter_value(query, "cloud_password").toByteArray());

        QByteArray body =  cJSON_help::mapTocJSONByteArray(
                    "cardNo", vch_memberno,
                    "operId", lds_scr_table::cey_sys_parameter_value(query, "cloud_user"));

        if(false == lds_network::managerPost(request, body, readall, errstring, httpcode)) {
            return false;
        }
        if(200 == httpcode) {
            cJSON_help json(readall);

            ch_typeno = cJSON_help::getcJSONvalue(json, "ch_typeno").toString();
            ch_attribute = cJSON_help::getcJSONvalue(json, "ch_attribute").toString();
            ch_favourable = cJSON_help::getcJSONvalue(json, "ch_favourable").toString();
            ch_pricekind = cJSON_help::getcJSONvalue(json, "ch_pricekind").toString();
            int_discount = cJSON_help::getcJSONvalue(json, "int_discount").toInt();
            vch_typename = cJSON_help::getcJSONvalue(json, "vch_typename").toString();
            vch_member = cJSON_help::getcJSONvalue(json, "vch_member").toString();
            return true;
        }
        errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
    } else  {//本地会员
        query.execSelect(QString(
                             " SELECT a.vch_memberno,a.vch_member,a.ch_typeno,a.vch_tel, a.vch_handtel,  a.dt_birthday,  a.ch_state, a.vch_password, a.dt_limit,a.ch_cardflag, b.int_card_baseamount,"
                             " b.ch_attribute, b.ch_favourable, b.ch_pricekind, b.int_discount,b.vch_typename, c.num_amount, c.num_point"
                             " FROM t_m_member a "
                             " left join t_m_member_type b on a.ch_typeno = b.ch_typeno "
                             " left join t_m_curamount c on a.vch_memberno = c.vch_memberno"
                             " where a.ch_typeno = b.ch_typeno and b.ch_attribute like '%'  and a.vch_memberno = '%1' "
                             )
                         .arg(vch_memberno));
        if(query.next()) {
            ch_typeno=query.recordValue("ch_typeno").toString();
            ch_attribute=query.recordValue("ch_attribute").toString();
            ch_favourable=query.recordValue("ch_favourable").toString();
            ch_pricekind=query.recordValue("ch_pricekind").toString();
            int_discount=query.recordValue("int_discount").toInt();
            vch_typename=query.recordValue("vch_typename").toString();
            vch_member=query.recordValue("vch_member").toString();
            return true;
        }
        errstring = query.recordError();
    }
    return false;
}

QString vch_membernoData::desc()
{
    QString retv;
    retv = QObject::tr("会员类型");
    if(isyun()) {
        retv = QObject::tr("云会员类型");
    }
    retv += +":"+vch_typename+",";

    if(vch_memberno.length() > 0) {
        if(ch_attribute == "1") {
            retv += QObject::tr("优惠卡")+":" + vch_memberno+",";
        }
        if(ch_attribute == "2") {
            retv += QObject::tr("充值卡")+":" + vch_memberno;
        }
    }
    return retv;
}

QString vch_membernoData::color()
{
    if(isyun()) {
        return "green";
    }
    return "#ff546b";
}

QString vch_membernoData::paymodeno()
{
    if(isyun()) {
        return "10";
    }
    return "02";
}

vch_membernoData::chooseFlag vch_membernoData::mutil_choose()
{
    /*第一次读卡，且没有开启云会员*/if(vch_memberno.isEmpty() && !(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_member", false).toBool()))
        return  vch_membernoData::choose_local;
    /*第一次读卡，且开启云会员，且禁止本地会员*/if(vch_memberno.isEmpty()
                               && lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_member", false).toBool()
                               && lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_no_use_local_member", false).toBool()
                               ) return  vch_membernoData::choose_yun;
    /*已经读过卡，且之前是云会员*/if(!vch_memberno.isEmpty() && "Y" == ch_member_belong_state)
        return  vch_membernoData::choose_yun;
    /*已经读过卡，且之前是非会员*/if(!vch_memberno.isEmpty() && "Y" != ch_member_belong_state)
        return  vch_membernoData::choose_local;

    return  vch_membernoData::choose_switch;
}

void vch_membernoData::print()
{
    qDebug() << "vch_memberno" << vch_memberno;
    qDebug() << "ch_attribute" << ch_attribute;
    qDebug() << "ch_favourable" << ch_favourable;
    qDebug() << "ch_pricekind" << ch_pricekind;
    qDebug() << "int_discount" << int_discount;
    qDebug() << "ch_member_belong_state" << ch_member_belong_state;
}

bool vch_membernoData::isyun() const
{
    return "Y" == ch_member_belong_state;
}

bool vch_membernoData::is_m_price() const
{
    if(!vch_memberno.isEmpty() && ch_favourable == "1" && ch_pricekind == "1" ) {
        return true;
    }
    return false;
}

bool vch_membernoData::is_m_discount() const
{
    if(!vch_memberno.isEmpty() && ch_favourable == "2") {
        return true;
    }
    return false;
}

double vch_membernoData::mprice(double org_price, double m_price) const
{
    if(is_m_price()) {
        return m_price;
    }
    return org_price;
}
