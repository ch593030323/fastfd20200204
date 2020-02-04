#include "w_sys_manage.h"
#include <QDir>
#include "cJSON.h"

w_sys_manage::w_sys_manage(QObject *parent) :
    QObject(parent)
{
}


QStringList w_sys_manage::get_flilename_path(const QString &path, const QStringList &filter)
{
    QStringList filenames;
    foreach(const QFileInfo &info, QDir(path).entryInfoList(filter, QDir::NoFilter, QDir::Time)) {
        if(info.baseName() == "."
                || info.baseName() == ".."
                || info.baseName() == "") {
            continue;
        }
        filenames << info.fileName();
    }
    return filenames;
}

QString w_sys_manage::set_flilename_path(const QString &flilename, const QString &path)
{
    return QString();
}

cJSON *w_sys_manage::q_get_piece_table(const QString &table_name, lds_query_hddpos  &query, const QString &orgCode, int state)
{
    if(table_name == "cey_bt_dish") return q_bt_get_piece_cey_bt_dish(query, orgCode, state);//-1删除、 0正常使用
    //2
    if(table_name == "cey_bt_dish_subtype") return q_bt_get_piece_cey_bt_dish_subtype(query, orgCode, state);//-1删除、 0正常使用
    //3
    if(table_name == "cey_bt_dish_type") return q_bt_get_piece_cey_bt_dish_type(query, orgCode, state);//-1删除、 0正常使用
    //4
    if(table_name == "cey_bt_dish_suit") return q_bt_get_piece_cey_bt_dish_suit(query, orgCode, state);//-1删除、 0正常使用
    //5
    if(table_name == "cey_bt_suit_change") return q_bt_get_piece_cey_bt_suit_change(query, orgCode, state);//-1删除、 0正常使用
    //6
    if(table_name == "cey_bt_unit") return q_bt_get_piece_cey_bt_unit(query, orgCode, state);//-1删除、 0正常使用
    //7
    if(table_name == "cey_bt_table") return q_bt_get_piece_cey_bt_table(query, orgCode, state);//-1删除、 0正常使用
    //8
    if(table_name == "cey_bt_table_type") return q_bt_get_piece_cey_bt_table_type(query, orgCode, state);//-1删除、 0正常使用
    //9
    if(table_name == "cey_bt_table_area") return q_bt_get_piece_cey_bt_table_area(query, orgCode, state);//-1删除、 0正常使用
    //10
    if(table_name == "cey_bt_cp_dish") return q_bt_get_piece_cey_bt_cp_dish(query, orgCode, state);//-1删除、 0正常使用
    //11
    if(table_name == "cey_bt_cp_memo") return q_bt_get_piece_cey_bt_cp_memo(query, orgCode, state);//-1删除、 0正常使用
    //12
    if(table_name == "cey_bt_cp_type") return q_bt_get_piece_cey_bt_cp_type(query, orgCode, state);//-1删除、 0正常使用
    //13
    if(table_name == "cey_bt_dish_warn") return q_bt_get_piece_cey_bt_dish_warn(query, orgCode, state);//-1删除、 0正常使用
    //14
    if(table_name == "cey_bt_paymode") return q_bt_get_piece_cey_bt_paymode(query, orgCode, state);//-1删除、 0正常使用
    //15
    if(table_name == "material_bt_dish") return q_bt_get_piece_material_bt_dish(query, orgCode, state);//-1删除、 0正常使用

    //会员
    if(table_name == "t_m_member_type") return q_m_get_piece_t_m_member_type(query, orgCode, state);//-1删除、 0正常使用
    //16
    if(table_name == "t_m_member") return q_m_get_piece_t_m_member(query, orgCode, state);//-1删除、 0正常使用
    //17
    if(table_name == "t_m_gift") return q_m_get_piece_t_m_gift(query, orgCode, state);//-1删除、 0正常使用
    //18
    if(table_name == "t_m_curamount") return q_m_get_piece_t_m_curamount(query, orgCode, state);//-1删除、 0正常使用
    //19
    if(table_name == "t_m_parameter") return q_m_get_piece_t_m_parameter(query, orgCode);//-1删除、 0正常使用

    qDebug() << __FILE__ << __LINE__ << "no tablename matched!" << table_name;
    return 0;
}

void w_sys_manage::q_put_piece_table(const QString &table_name, qrtVariantPairList &key_value_list, cJSON *pItem)
{
    if(table_name == "cey_bt_dish") return q_bt_put_piece_cey_bt_dish(key_value_list, pItem);//-1删除、 0正常使用
    //2
    if(table_name == "cey_bt_dish_subtype") return q_bt_put_piece_cey_bt_dish_subtype(key_value_list, pItem);//-1删除、 0正常使用
    //3
    if(table_name == "cey_bt_dish_type") return q_bt_put_piece_cey_bt_dish_type(key_value_list, pItem);//-1删除、 0正常使用
    //4
    if(table_name == "cey_bt_dish_suit") return q_bt_put_piece_cey_bt_dish_suit(key_value_list, pItem);//-1删除、 0正常使用
    //5
    if(table_name == "cey_bt_suit_change") return q_bt_put_piece_cey_bt_suit_change(key_value_list, pItem);//-1删除、 0正常使用
    //6
    if(table_name == "cey_bt_unit") return q_bt_put_piece_cey_bt_unit(key_value_list, pItem);//-1删除、 0正常使用
    //7
    if(table_name == "cey_bt_table") return q_bt_put_piece_cey_bt_table(key_value_list, pItem);//-1删除、 0正常使用
    //8
    if(table_name == "cey_bt_table_type") return q_bt_put_piece_cey_bt_table_type(key_value_list, pItem);//-1删除、 0正常使用
    //9
    if(table_name == "cey_bt_table_area") return q_bt_put_piece_cey_bt_table_area(key_value_list, pItem);//-1删除、 0正常使用
    //10
    if(table_name == "cey_bt_cp_dish") return q_bt_put_piece_cey_bt_cp_dish(key_value_list, pItem);//-1删除、 0正常使用
    //11
    if(table_name == "cey_bt_cp_memo") return q_bt_put_piece_cey_bt_cp_memo(key_value_list, pItem);//-1删除、 0正常使用
    //12
    if(table_name == "cey_bt_cp_type") return q_bt_put_piece_cey_bt_cp_type(key_value_list, pItem);//-1删除、 0正常使用
    //13
    if(table_name == "cey_bt_dish_warn") return q_bt_put_piece_cey_bt_dish_warn(key_value_list, pItem);//-1删除、 0正常使用
    //14
    if(table_name == "cey_bt_paymode") return q_bt_put_piece_cey_bt_paymode(key_value_list, pItem);//-1删除、 0正常使用
    //15
    if(table_name == "material_bt_dish") return q_bt_put_piece_material_bt_dish(key_value_list, pItem);//-1删除、 0正常使用

    //会员
    if(table_name == "t_m_member_type") return q_m_put_piece_t_m_member_type(key_value_list, pItem);//-1删除、 0正常使用
    //16
    if(table_name == "t_m_member") return q_m_put_piece_t_m_member(key_value_list, pItem);//-1删除、 0正常使用
    //17
    if(table_name == "t_m_gift") return q_m_put_piece_t_m_gift(key_value_list, pItem);//-1删除、 0正常使用
    //18
    if(table_name == "t_m_curamount") return q_m_put_piece_t_m_curamount(key_value_list, pItem);//-1删除、 0正常使用
    //19
    if(table_name == "t_m_parameter") return q_m_put_piece_t_m_parameter(key_value_list, pItem);//-1删除、 0正常使用

    qDebug() << __FILE__ << __LINE__ << "no tablename matched!" << table_name;
    return ;
}



cJSON *w_sys_manage::q_bt_get_piece_cey_bt_dish(lds_query_hddpos  &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::_cJSON_AddValueToObject( pItem, "diChDishNo", query.recordValue("ch_dishno"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diVchDishName", query.recordValue("vch_dishname"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diVchSpell", query.recordValue("vch_spell"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diVchEnglish", query.recordValue("vch_english"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diChDishTypeNo", query.recordValue("ch_dish_typeno"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diChUnitNo", query.recordValue("ch_unitno"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diNumPrice", query.recordValue("num_price"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diChSuitflag", query.recordValue("ch_suitflag"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diChDiscount", query.recordValue("ch_discount"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diChCurprice", query.recordValue("ch_curprice"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diChTempflag", query.recordValue("ch_tempflag"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diChStopflag", query.recordValue("ch_stopflag"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diChPrintFlag", query.recordValue("ch_printflag"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diVchPrinter", query.recordValue("vch_printer"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diChSubTypeNo", query.recordValue("ch_sub_typeno"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diChMaterialNo", query.recordValue("ch_materialno"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diChServiceFlag", query.recordValue("ch_serviceflag"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diChLowFlag", query.recordValue("ch_lowflag"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diNumMprice", query.recordValue("num_m_price"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diChDeductFlag", query.recordValue("ch_deductflag"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diChDeductType", query.recordValue("ch_deducttype"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diIntDeduct", query.recordValue("int_deduct"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diNumDeduct", query.recordValue("num_deduct"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diChSaleFlag", query.recordValue("ch_saleflag"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diChSaleType", query.recordValue("ch_saletype"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diIntSaleDeduct", query.recordValue("int_sale_deduct"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diNumSale", query.recordValue("num_sale"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "itemFlag", query.recordValue("item_flag"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diVchPurUnitNo", query.recordValue("vch_pur_unitno"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diIntUnitRate", query.recordValue("int_unit_rate"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diChOff", query.recordValue("ch_off"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diVchSubNo", query.recordValue("vch_subno"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diNumSalePrice", query.recordValue("num_sale_price"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diChOutFlag", query.recordValue("ch_outflag"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "diVchOutPrint", query.recordValue("vch_outprint"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "intRate", query.recordValue("int_rate"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "chWeight", query.recordValue("ch_weight"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "vchBarcode", query.recordValue("vch_barcode"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "chGroupno", query.recordValue("ch_groupno"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "chMaterialFlag", query.recordValue("ch_material_flag"));

    cJSON_help::_cJSON_AddValueToObject( pItem, "state", state);//逻辑删除<0话，是逻辑删除
    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_dish(qrtVariantPairList &key_value_list, cJSON *pItem)
{

    key_value_list << qrtVariantPair("ch_dishno", cJSON_help::getcJSONvalue(pItem,"diChDishNo"));
    key_value_list << qrtVariantPair("vch_dishname", cJSON_help::getcJSONvalue(pItem,"diVchDishName"));
    key_value_list << qrtVariantPair("vch_spell", cJSON_help::getcJSONvalue(pItem,"diVchSpell"));
    key_value_list << qrtVariantPair("vch_english", cJSON_help::getcJSONvalue(pItem,"diVchEnglish"));
    key_value_list << qrtVariantPair("ch_dish_typeno", cJSON_help::getcJSONvalue(pItem,"diChDishTypeNo"));
    key_value_list << qrtVariantPair("ch_unitno", cJSON_help::getcJSONvalue(pItem,"diChUnitNo"));
    key_value_list << qrtVariantPair("num_price", cJSON_help::getcJSONvalue(pItem,"diNumPrice"));
    key_value_list << qrtVariantPair("ch_suitflag", cJSON_help::getcJSONvalue(pItem,"diChSuitflag"));
    key_value_list << qrtVariantPair("ch_discount", cJSON_help::getcJSONvalue(pItem,"diChDiscount"));
    key_value_list << qrtVariantPair("ch_curprice", cJSON_help::getcJSONvalue(pItem,"diChCurprice"));
    key_value_list << qrtVariantPair("ch_tempflag", cJSON_help::getcJSONvalue(pItem,"diChTempflag"));
    key_value_list << qrtVariantPair("ch_stopflag", cJSON_help::getcJSONvalue(pItem,"diChStopflag"));
    key_value_list << qrtVariantPair("ch_printflag", cJSON_help::getcJSONvalue(pItem,"diChPrintFlag"));
    key_value_list << qrtVariantPair("vch_printer", cJSON_help::getcJSONvalue(pItem,"diVchPrinter"));
    key_value_list << qrtVariantPair("ch_sub_typeno", cJSON_help::getcJSONvalue(pItem,"diChSubTypeNo"));
    key_value_list << qrtVariantPair("ch_materialno", cJSON_help::getcJSONvalue(pItem,"diChMaterialNo"));
    key_value_list << qrtVariantPair("ch_serviceflag", cJSON_help::getcJSONvalue(pItem,"diChServiceFlag"));
    key_value_list << qrtVariantPair("ch_lowflag", cJSON_help::getcJSONvalue(pItem,"diChLowFlag"));
    key_value_list << qrtVariantPair("num_m_price", cJSON_help::getcJSONvalue(pItem,"diNumMprice"));
    key_value_list << qrtVariantPair("ch_deductflag", cJSON_help::getcJSONvalue(pItem,"diChDeductFlag"));
    key_value_list << qrtVariantPair("ch_deducttype", cJSON_help::getcJSONvalue(pItem,"diChDeductType"));
    key_value_list << qrtVariantPair("int_deduct", cJSON_help::getcJSONvalue(pItem,"diIntDeduct"));
    key_value_list << qrtVariantPair("num_deduct", cJSON_help::getcJSONvalue(pItem,"diNumDeduct"));
    key_value_list << qrtVariantPair("ch_saleflag", cJSON_help::getcJSONvalue(pItem,"diChSaleFlag"));
    key_value_list << qrtVariantPair("ch_saletype", cJSON_help::getcJSONvalue(pItem,"diChSaleType"));
    key_value_list << qrtVariantPair("int_sale_deduct", cJSON_help::getcJSONvalue(pItem,"diIntSaleDeduct"));
    key_value_list << qrtVariantPair("num_sale", cJSON_help::getcJSONvalue(pItem,"diNumSale"));
    key_value_list << qrtVariantPair("item_flag", cJSON_help::getcJSONvalue(pItem,"itemFlag"));
    key_value_list << qrtVariantPair("vch_pur_unitno", cJSON_help::getcJSONvalue(pItem,"diVchPurUnitNo"));
    key_value_list << qrtVariantPair("int_unit_rate", cJSON_help::getcJSONvalue(pItem,"diIntUnitRate"));
    key_value_list << qrtVariantPair("ch_off", cJSON_help::getcJSONvalue(pItem,"diChOff"));
    key_value_list << qrtVariantPair("vch_subno", cJSON_help::getcJSONvalue(pItem,"diVchSubNo"));
    key_value_list << qrtVariantPair("num_sale_price", cJSON_help::getcJSONvalue(pItem,"diNumSalePrice"));
    key_value_list << qrtVariantPair("ch_outflag", cJSON_help::getcJSONvalue(pItem,"diChOutFlag"));
    key_value_list << qrtVariantPair("vch_outprint", cJSON_help::getcJSONvalue(pItem,"diVchOutPrint"));
    key_value_list << qrtVariantPair("int_rate", cJSON_help::getcJSONvalue(pItem,"intRate"));
    key_value_list << qrtVariantPair("ch_weight", cJSON_help::getcJSONvalue(pItem,"chWeight"));
    key_value_list << qrtVariantPair("vch_barcode", cJSON_help::getcJSONvalue(pItem,"vchBarcode"));
    key_value_list << qrtVariantPair("ch_groupno", cJSON_help::getcJSONvalue(pItem,"chGroupno"));
    key_value_list << qrtVariantPair("ch_material_flag", cJSON_help::getcJSONvalue(pItem,"chMaterialFlag"));

    key_value_list << qrtVariantPair("ch_special_type", cJSON_help::getcJSONvalue(pItem,"chSpecialType1"));
    key_value_list << qrtVariantPair("ch_special_type2", cJSON_help::getcJSONvalue(pItem,"chSpecialType2"));
    key_value_list << qrtVariantPair("num_special_price", cJSON_help::getcJSONvalue(pItem,"numSpecialPrice"));

    key_value_list << qrtVariantPair("ch_qr_code_online", cJSON_help::getcJSONvalue(pItem,"diNumOnFlag"));
}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_dish_subtype(lds_query_hddpos  &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::_cJSON_AddValueToObject( pItem, "dstChSubTypeNo", query.recordValue("ch_sub_typeno"));

    cJSON_help::_cJSON_AddValueToObject( pItem, "dstVchSubTypeName", query.recordValue("vch_sub_typename"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dstChDishTypeNo", query.recordValue("ch_dish_typeno"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dstChLagelPrintFlag", query.recordValue("ch_labelprint_flag"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "state", state);//逻辑删除
    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_dish_subtype(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_sub_typeno", cJSON_help::getcJSONvalue(pItem,"dstChSubTypeNo"));
    key_value_list << qrtVariantPair("vch_sub_typename", cJSON_help::getcJSONvalue(pItem,"dstVchSubTypeName"));
    key_value_list << qrtVariantPair("ch_dish_typeno", cJSON_help::getcJSONvalue(pItem,"dstChDishTypeNo"));
    key_value_list << qrtVariantPair("ch_labelprint_flag", cJSON_help::getcJSONvalue(pItem,"dstChLagelPrintFlag"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_dish_type(lds_query_hddpos  &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::_cJSON_AddValueToObject( pItem, "dtChDishTypeNo", query.recordValue("ch_dish_typeno"));

    cJSON_help::_cJSON_AddValueToObject( pItem, "dtVchDishTypeName", query.recordValue("vch_dish_typename"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "state", state);//逻辑删除
    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_dish_type(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_dish_typeno", cJSON_help::getcJSONvalue(pItem,"dtChDishTypeNo"));
    key_value_list << qrtVariantPair("vch_dish_typename", cJSON_help::getcJSONvalue(pItem,"dtVchDishTypeName"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_dish_suit(lds_query_hddpos  &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::_cJSON_AddValueToObject( pItem, "dsIntFlowId", query.recordValue("int_flowid"));

    cJSON_help::_cJSON_AddValueToObject( pItem, "dsChSuitNo", query.recordValue("ch_suitno"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dsChDishNo", query.recordValue("ch_dishno"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dsNumNum", query.recordValue("num_num"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dsChDishNoChang", query.recordValue("ch_dishno_chang"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dsNumNumChange", query.recordValue("num_num_change"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "state", state);//逻辑删除
    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_dish_suit(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("int_flowid", cJSON_help::getcJSONvalue(pItem,"dsIntFlowId"));
    key_value_list << qrtVariantPair("ch_suitno", cJSON_help::getcJSONvalue(pItem,"dsChSuitNo"));
    key_value_list << qrtVariantPair("ch_dishno", cJSON_help::getcJSONvalue(pItem,"dsChDishNo"));
    key_value_list << qrtVariantPair("num_num", cJSON_help::getcJSONvalue(pItem,"dsNumNum"));
    key_value_list << qrtVariantPair("ch_dishno_chang", cJSON_help::getcJSONvalue(pItem,"dsChDishNoChang"));
    key_value_list << qrtVariantPair("num_num_change", cJSON_help::getcJSONvalue(pItem,"dsNumNumChange"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_suit_change(lds_query_hddpos  &query, const QString &orgCode, int state)
{
    //复合主键int_flowid,ch_dishno
    cJSON *pItem = cJSON_CreateObject();
    //
    cJSON_help::_cJSON_AddValueToObject( pItem, "dscIntFlowId", query.recordValue("int_flowID"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dscChDishNo", query.recordValue("ch_dishno"));

    cJSON_help::_cJSON_AddValueToObject( pItem, "dscNumNumChange", query.recordValue("num_num_change"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "state", state);//逻辑删除
    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_suit_change(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("int_flowid", cJSON_help::getcJSONvalue(pItem,"dscIntFlowId").toInt());
    key_value_list << qrtVariantPair("ch_dishno", cJSON_help::getcJSONvalue(pItem,"dscChDishNo"));
    key_value_list << qrtVariantPair("num_num_change", cJSON_help::getcJSONvalue(pItem,"dscNumNumChange"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_unit(lds_query_hddpos  &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::_cJSON_AddValueToObject( pItem, "duChUnitNo", query.recordValue("ch_unitno"));

    cJSON_help::_cJSON_AddValueToObject( pItem, "duVchUnitName", query.recordValue("vch_unitname"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "duChNumType", query.recordValue("ch_numtype"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "duNumDefault", query.recordValue("num_default").toDouble());

    cJSON_help::_cJSON_AddValueToObject(pItem, "state", state);//逻辑删除
    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_unit(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_unitno", cJSON_help::getcJSONvalue(pItem,"duChUnitNo"));
    key_value_list << qrtVariantPair("vch_unitname", cJSON_help::getcJSONvalue(pItem,"duVchUnitName"));
    key_value_list << qrtVariantPair("ch_numtype", cJSON_help::getcJSONvalue(pItem,"duChNumType"));
    key_value_list << qrtVariantPair("num_default", cJSON_help::getcJSONvalue(pItem,"duNumDefault"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_table(lds_query_hddpos  &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::_cJSON_AddValueToObject( pItem, "dtChTableNo", query.recordValue("ch_tableno"));

    cJSON_help::_cJSON_AddValueToObject( pItem, "dtVchTableName", query.recordValue("vch_tablename"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dtChTypeNo", query.recordValue("ch_typeno"));
    //    cJSON_help::_cJSON_AddValueToObject( pItem, "dtChBillNo", query.recordValue("ch_billno"));
    //    cJSON_help::_cJSON_AddValueToObject( pItem, "dtChLockFlag", query.recordValue("ch_lockflag"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dtIntPerson", query.recordValue("int_person"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dtVchMemo", query.recordValue("vch_memo"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dtChState", query.recordValue("ch_state"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dtDtDateTime", query.recordValue("dt_datetime"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dtChAreaNo", query.recordValue("ch_areano"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "state", state);//逻辑删除
    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_table(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_tableno", cJSON_help::getcJSONvalue(pItem,"dtChTableNo"));
    key_value_list << qrtVariantPair("vch_tablename", cJSON_help::getcJSONvalue(pItem,"dtVchTableName"));
    key_value_list << qrtVariantPair("ch_typeno", cJSON_help::getcJSONvalue(pItem,"dtChTypeNo"));
    //   key_value_list << qrtVariantPair("ch_billno", cJSON_help::getcJSONvalue(pItem,"dtChBillNo"));
    //   key_value_list << qrtVariantPair("ch_lockflag", cJSON_help::getcJSONvalue(pItem,"dtChLockFlag"));
    key_value_list << qrtVariantPair("int_person", cJSON_help::getcJSONvalue(pItem,"dtIntPerson").toInt());
    key_value_list << qrtVariantPair("vch_memo", cJSON_help::getcJSONvalue(pItem,"dtVchMemo"));
    key_value_list << qrtVariantPair("ch_state", cJSON_help::getcJSONvalue(pItem,"dtChState"));
    key_value_list << qrtVariantPair("dt_datetime", cJSON_help::getcJSONvalue(pItem,"dtDtDateTime"));
    key_value_list << qrtVariantPair("ch_areano", cJSON_help::getcJSONvalue(pItem,"dtChAreaNo"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_table_type(lds_query_hddpos  &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::_cJSON_AddValueToObject( pItem, "dttChTyperNo", query.recordValue("ch_typeno"));

    cJSON_help::_cJSON_AddValueToObject( pItem, "dttVchTypeName", query.recordValue("vch_typename"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dttChPriceKind", query.recordValue("ch_pricekind"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dttChServiceMode", query.recordValue("ch_service_mode"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dttChServiceFlag", query.recordValue("ch_serviceflag"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dttIntStartMinute", query.recordValue("int_start_minute"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dttNumStartAmount", query.recordValue("num_start_amount"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dttIntUnitMinute", query.recordValue("int_unit_minute"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dttNumUnitAmount", query.recordValue("num_unit_amount"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dttIntOver",query.recordValue("int_over"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dttNumRation", query.recordValue("num_ration"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dttIntRate", query.recordValue("int_rate"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dttNumMaxService", query.recordValue("num_max_service"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dttChLowMode", query.recordValue("ch_low_mode"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dttNumLowCost", query.recordValue("num_lowcost"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dttIntBeginTime",query.recordValue("int_begin_time"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "state", state);//逻辑删除
    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_table_type(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_typeno", cJSON_help::getcJSONvalue(pItem,"dttChTyperNo"));
    key_value_list << qrtVariantPair("vch_typename", cJSON_help::getcJSONvalue(pItem,"dttVchTypeName"));
    key_value_list << qrtVariantPair("ch_pricekind", cJSON_help::getcJSONvalue(pItem,"dttChPriceKind"));
    key_value_list << qrtVariantPair("ch_service_mode", cJSON_help::getcJSONvalue(pItem,"dttChServiceMode"));
    key_value_list << qrtVariantPair("ch_serviceflag", cJSON_help::getcJSONvalue(pItem,"dttChServiceFlag"));
    key_value_list << qrtVariantPair("int_start_minute", cJSON_help::getcJSONvalue(pItem,"dttIntStartMinute"));
    key_value_list << qrtVariantPair("num_start_amount", cJSON_help::getcJSONvalue(pItem,"dttNumStartAmount"));
    key_value_list << qrtVariantPair("int_unit_minute", cJSON_help::getcJSONvalue(pItem,"dttIntUnitMinute"));
    key_value_list << qrtVariantPair("num_unit_amount", cJSON_help::getcJSONvalue(pItem,"dttNumUnitAmount"));
    key_value_list << qrtVariantPair("int_over", cJSON_help::getcJSONvalue(pItem,"dttIntOver"));
    key_value_list << qrtVariantPair("num_ration", cJSON_help::getcJSONvalue(pItem,"dttNumRation"));
    key_value_list << qrtVariantPair("int_rate", cJSON_help::getcJSONvalue(pItem,"dttIntRate"));
    key_value_list << qrtVariantPair("num_max_service", cJSON_help::getcJSONvalue(pItem,"dttNumMaxService"));
    key_value_list << qrtVariantPair("ch_low_mode", cJSON_help::getcJSONvalue(pItem,"dttChLowMode"));
    key_value_list << qrtVariantPair("num_lowcost", cJSON_help::getcJSONvalue(pItem,"dttNumLowCost"));
    key_value_list << qrtVariantPair("int_begin_time", cJSON_help::getcJSONvalue(pItem,"dttIntBeginTime"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_table_area(lds_query_hddpos  &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::_cJSON_AddValueToObject(pItem, "dtaChAreaNo",  query.recordValue("ch_areano"));

    cJSON_help::_cJSON_AddValueToObject( pItem, "dtaVchAreaName", query.recordValue("vch_areaname"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "state", state);//逻辑删除
    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);


    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_table_area(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_areano", cJSON_help::getcJSONvalue(pItem,"dtaChAreaNo"));
    key_value_list << qrtVariantPair("vch_areaname", cJSON_help::getcJSONvalue(pItem,"dtaVchAreaName"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_cp_dish(lds_query_hddpos  &query, const QString &orgCode, int state)
{
    //1 cey_bt_cp_dish、 2concat(%1.ch_dishno, ',', %1.ch_no)、3 isnull(%1.ch_dishno) and isnull(%1.ch_no)
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::_cJSON_AddValueToObject(pItem, "dpChDishNo",  query.recordValue("ch_dishno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "dpChNo",  query.recordValue("ch_no"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "state", state);//逻辑删除
    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_cp_dish(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_dishno", cJSON_help::getcJSONvalue(pItem,"dpChDishNo"));
    key_value_list << qrtVariantPair("ch_no", cJSON_help::getcJSONvalue(pItem,"dpChNo"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_cp_memo(lds_query_hddpos  &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::_cJSON_AddValueToObject(pItem, "chNo",  query.recordValue("ch_no"));

    cJSON_help::_cJSON_AddValueToObject( pItem, "vchCpMemo", query.recordValue("vch_cp_memo"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "chTypeno", query.recordValue("ch_typeno"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "numAddPrice", query.recordValue("num_add_price"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "state", state);//逻辑删除
    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);


    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_cp_memo(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_no", cJSON_help::getcJSONvalue(pItem,"chNo"));
    key_value_list << qrtVariantPair("vch_cp_memo", cJSON_help::getcJSONvalue(pItem,"vchCpMemo"));
    key_value_list << qrtVariantPair("ch_typeno", cJSON_help::getcJSONvalue(pItem,"chTypeno"));
    key_value_list << qrtVariantPair("num_add_price", cJSON_help::getcJSONvalue(pItem,"numAddPrice"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_cp_type(lds_query_hddpos  &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::_cJSON_AddValueToObject( pItem, "dptChTypeNo", query.recordValue("ch_typeno"));

    cJSON_help::_cJSON_AddValueToObject( pItem, "dptVchTypeName", query.recordValue("vch_typename"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "state", state);//逻辑删除
    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_cp_type(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_typeno", cJSON_help::getcJSONvalue(pItem,"dptChTypeNo"));
    key_value_list << qrtVariantPair("vch_typename", cJSON_help::getcJSONvalue(pItem,"dptVchTypeName"));


}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_dish_warn(lds_query_hddpos  &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();
    //    cJSON_help::_cJSON_AddValueToObject( pItem, "dgqIntFlowId", query.recordValue("int_flowID"));

    //    cJSON_help::_cJSON_AddValueToObject( pItem, "dgqDtDate", query.recordValue("dt_date"));
    //    cJSON_help::_cJSON_AddValueToObject( pItem, "dgqChDishNo", query.recordValue("ch_dishno"));
    //    cJSON_help::_cJSON_AddValueToObject( pItem, "dgqNumNum", query.recordValue("num_num"));
    //    cJSON_help::_cJSON_AddValueToObject( pItem, "dgqNumSale", query.recordValue("num_sale"));
    //    cJSON_help::_cJSON_AddValueToObject( pItem, "dgqChState", query.recordValue("ch_state"));
    //    cJSON_help::_cJSON_AddValueToObject( pItem, "dgqVchOperId", query.recordValue("vch_operID"));
    //    cJSON_help::_cJSON_AddValueToObject( pItem, "dgqDtOperDate", query.recordValue("dt_operdate"));
    //    cJSON_help::_cJSON_AddValueToObject( pItem, "dgqVchOperId2", query.recordValue("vch_operID2"));
    //    cJSON_help::_cJSON_AddValueToObject( pItem, "dgqDtOperDate2", query.recordValue("dt_operdate2"));

    //    cJSON_AddNumberToObject(pItem, "state", state);
    //    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_dish_warn(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("int_flowid", cJSON_help::getcJSONvalue(pItem,"dgqIntFlowId"));
    key_value_list << qrtVariantPair("dt_date", cJSON_help::getcJSONvalue(pItem,"dgqDtDate"));
    key_value_list << qrtVariantPair("ch_dishno", cJSON_help::getcJSONvalue(pItem,"dgqChDishNo"));
    key_value_list << qrtVariantPair("num_num", cJSON_help::getcJSONvalue(pItem,"dgqNumNum"));
    key_value_list << qrtVariantPair("num_sale", cJSON_help::getcJSONvalue(pItem,"dgqNumSale"));
    key_value_list << qrtVariantPair("ch_state", cJSON_help::getcJSONvalue(pItem,"dgqChState"));
    key_value_list << qrtVariantPair("vch_operID", cJSON_help::getcJSONvalue(pItem,"dgqVchOperId"));
    key_value_list << qrtVariantPair("dt_operdate", cJSON_help::getcJSONvalue(pItem,"dgqDtOperDate"));
    key_value_list << qrtVariantPair("vch_operID2", cJSON_help::getcJSONvalue(pItem,"dgqVchOperId2"));
    key_value_list << qrtVariantPair("dt_operdate2", cJSON_help::getcJSONvalue(pItem,"dgqDtOperDate2"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_paymode(lds_query_hddpos  &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::_cJSON_AddValueToObject( pItem, "pmChPayModeNo", query.recordValue("ch_paymodeno"));

    cJSON_help::_cJSON_AddValueToObject( pItem, "pmVchPayModeName", query.recordValue("vch_paymodename"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "pmChIdFlag", query.recordValue("ch_IDflag"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "pmChFaceFlag", query.recordValue("ch_faceflag"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "pmChChangeFlag", query.recordValue("ch_changeflag"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "pmChInComeFlag", query.recordValue("ch_incomeflag"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "pmChKey", query.recordValue("ch_key"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "pmVchARG1", query.recordValue("vch_arg1"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "pmVchARG2", query.recordValue("vch_arg2"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "pmVchARG3", query.recordValue("vch_arg3"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "pmVchARG4", query.recordValue("vch_arg4"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "pmVchARG5", query.recordValue("vch_arg5"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "pmVchARG6", query.recordValue("vch_arg6"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "pmVchARG7", query.recordValue("vch_arg7"));

    cJSON_AddNumberToObject(pItem, "state", state);
    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_paymode(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_paymodeno", cJSON_help::getcJSONvalue(pItem,"pmChPayModeNo"));
    key_value_list << qrtVariantPair("vch_paymodename", cJSON_help::getcJSONvalue(pItem,"pmVchPayModeName"));
    key_value_list << qrtVariantPair("ch_IDflag", cJSON_help::getcJSONvalue(pItem,"pmChIdFlag"));
    key_value_list << qrtVariantPair("ch_faceflag", cJSON_help::getcJSONvalue(pItem,"pmChFaceFlag"));
    key_value_list << qrtVariantPair("ch_changeflag", cJSON_help::getcJSONvalue(pItem,"pmChChangeFlag"));

    key_value_list << qrtVariantPair("ch_incomeflag", cJSON_help::getcJSONvalue(pItem,"pmChInComeFlag"));
    key_value_list << qrtVariantPair("ch_key", cJSON_help::getcJSONvalue(pItem,"pmChKey"));
    key_value_list << qrtVariantPair("vch_arg1", cJSON_help::getcJSONvalue(pItem,"pmVchARG1"));
    key_value_list << qrtVariantPair("vch_arg2", cJSON_help::getcJSONvalue(pItem,"pmVchARG2"));
    key_value_list << qrtVariantPair("vch_arg3", cJSON_help::getcJSONvalue(pItem,"pmVchARG3"));

    key_value_list << qrtVariantPair("vch_arg4", cJSON_help::getcJSONvalue(pItem,"pmVchARG4"));
    key_value_list << qrtVariantPair("vch_arg5", cJSON_help::getcJSONvalue(pItem,"pmVchARG5"));
    key_value_list << qrtVariantPair("vch_arg6", cJSON_help::getcJSONvalue(pItem,"pmVchARG6"));
    key_value_list << qrtVariantPair("vch_arg7", cJSON_help::getcJSONvalue(pItem,"pmVchARG7"));

}



cJSON *w_sys_manage::q_bt_get_piece_material_bt_dish(lds_query_hddpos  &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::_cJSON_AddValueToObject( pItem, "intFlowID", query.recordValue("int_flowID"));

    cJSON_help::_cJSON_AddValueToObject( pItem, "chDishno", query.recordValue("ch_dishno"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "chItemno", query.recordValue("ch_itemno"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "numNum", query.recordValue("num_num"));

    cJSON_AddNumberToObject(pItem, "state", state);
    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_material_bt_dish(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("int_flowID", cJSON_help::getcJSONvalue(pItem,"intFlowID"));
    key_value_list << qrtVariantPair("ch_dishno", cJSON_help::getcJSONvalue(pItem,"chDishno"));
    key_value_list << qrtVariantPair("ch_itemno", cJSON_help::getcJSONvalue(pItem,"chItemno"));
    key_value_list << qrtVariantPair("num_num", cJSON_help::getcJSONvalue(pItem,"numNum"));

}

cJSON *w_sys_manage::q_bt_get_piece_promotion()
{
    cJSON *pItem = cJSON_CreateObject();
    q_bt_get_piece_promotion(pItem);
    return pItem;
}

int w_sys_manage::q_bt_get_piece_promotion(cJSON *pItem)
{
    lds_query_hddpos  query;
    //不存在依旧上传
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_am'; ");cJSON_help::_cJSON_AddValueToObject( pItem, "promotionSettingAm", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_am_2'; ");cJSON_help::_cJSON_AddValueToObject( pItem, "promotionSettingAm2", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_date'; ");cJSON_help::_cJSON_AddValueToObject( pItem, "promotionSettingDate", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_date_2'; ");cJSON_help::_cJSON_AddValueToObject( pItem, "promotionSettingDate2", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_pm'; ");cJSON_help::_cJSON_AddValueToObject( pItem, "promotionSettingPm", query.recordValue(0));

    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_pm_2'; ");cJSON_help::_cJSON_AddValueToObject( pItem, "promotionSettingPm2", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_Mon'; ");cJSON_help::_cJSON_AddValueToObject( pItem, "promotionSettingMon", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_Tues'; ");cJSON_help::_cJSON_AddValueToObject( pItem, "promotionSettingTues", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_Wed'; ");cJSON_help::_cJSON_AddValueToObject( pItem, "promotionSettingWed", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_Thur'; ");cJSON_help::_cJSON_AddValueToObject( pItem, "promotionSettingThur", query.recordValue(0));

    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_Fri'; ");cJSON_help::_cJSON_AddValueToObject( pItem, "promotionSettingFri", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_Sat'; ");cJSON_help::_cJSON_AddValueToObject( pItem, "promotionSettingSat", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_Sun'; ");cJSON_help::_cJSON_AddValueToObject( pItem, "promotionSettingSun", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_target'; ");cJSON_help::_cJSON_AddValueToObject( pItem, "promotionSettingTarget", query.recordValue(0));

    return 14;
}

bool w_sys_manage::q_bt_put_piece_promotion(cJSON *pItem)
{
    lds_query_hddpos  sqlQuery;

    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_am")     << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingAm")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_am_2")   << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingAm2")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_date")   << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingDate")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_date_2") << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingDate2")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_pm")     << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingPm")))) return false;

    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_pm_2")   << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingPm2")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_Mon")    << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingMon")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_Tues")   << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingTues")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_Wed")    << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingWed")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_Thur")   << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingThur")))) return false;

    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_Fri")    << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingFri")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_Sat")    << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingSat")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_Sun")    << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingSun")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_target") << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingTarget")))) return false;

    return true;
}

cJSON *w_sys_manage::q_m_get_piece_t_m_member_type(lds_query_hddpos  &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::_cJSON_AddValueToObject( pItem, "chTypeno", query.recordValue("ch_typeno"));

    cJSON_help::_cJSON_AddValueToObject( pItem, "vchTypename", query.recordValue("vch_typename"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "chAttribute", query.recordValue("ch_attribute"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "chFavourable", query.recordValue("ch_favourable"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "chPricekind", query.recordValue("ch_pricekind"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "intDiscount", query.recordValue("int_discount"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "chProjectno", query.recordValue("ch_projectno"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "chCalc", query.recordValue("ch_calc"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "chCalcType", query.recordValue("ch_calc_type"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "intCalcUnit", query.recordValue("int_calc_unit"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "chAreaFlag", query.recordValue("ch_area_flag"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "intBackcashRate", query.recordValue("int_backcash_rate"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "intCardBaseamount", query.recordValue("int_card_baseamount"));

    cJSON_AddNumberToObject(pItem, "state", state);
    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_m_put_piece_t_m_member_type(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_typeno", cJSON_help::getcJSONvalue(pItem,"chTypeno"));
    key_value_list << qrtVariantPair("vch_typename", cJSON_help::getcJSONvalue(pItem,"vchTypename"));
    key_value_list << qrtVariantPair("ch_attribute", cJSON_help::getcJSONvalue(pItem,"chAttribute"));
    key_value_list << qrtVariantPair("ch_favourable", cJSON_help::getcJSONvalue(pItem,"chFavourable"));
    key_value_list << qrtVariantPair("ch_pricekind", cJSON_help::getcJSONvalue(pItem,"chPricekind"));
    key_value_list << qrtVariantPair("int_discount", cJSON_help::getcJSONvalue(pItem,"intDiscount"));
    key_value_list << qrtVariantPair("ch_projectno", cJSON_help::getcJSONvalue(pItem,"chProjectno"));
    key_value_list << qrtVariantPair("ch_calc", cJSON_help::getcJSONvalue(pItem,"chCalc"));
    key_value_list << qrtVariantPair("ch_calc_type", cJSON_help::getcJSONvalue(pItem,"chCalcType"));
    key_value_list << qrtVariantPair("int_calc_unit", cJSON_help::getcJSONvalue(pItem,"intCalcUnit"));
    key_value_list << qrtVariantPair("ch_area_flag", cJSON_help::getcJSONvalue(pItem,"chAreaFlag"));
    key_value_list << qrtVariantPair("int_backcash_rate", cJSON_help::getcJSONvalue(pItem,"intBackcashRate"));
    key_value_list << qrtVariantPair("int_card_baseamount", cJSON_help::getcJSONvalue(pItem,"intCardBaseamount"));

}

cJSON *w_sys_manage::q_m_get_piece_t_m_member(lds_query_hddpos  &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::_cJSON_AddValueToObject( pItem, "miVchMemberNo", query.recordValue("vch_memberno"));

    cJSON_help::_cJSON_AddValueToObject( pItem, "miVchMember", query.recordValue("vch_member"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "miChTypeNo", query.recordValue("ch_typeno"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "miVchId", query.recordValue("vch_id"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "miDtBirthday", query.recordValue("dt_birthday"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "miVchTel", query.recordValue("vch_tel"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "miVchHandTel", query.recordValue("vch_handtel"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "miVchAddress", query.recordValue("vch_address"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "miDtLimit", query.recordValue("dt_limit"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "miChState", query.recordValue("ch_state"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "numLimit",query.recordValue("num_limit"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "miChCardFlag", query.recordValue("ch_cardflag"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "miVchCardNo", query.recordValue("vch_cardno"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "miVchPassword", query.recordValue("vch_password"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "miVchOperId", query.recordValue("vch_operID"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "miDtOperDate", query.recordValue("dt_operdate"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "miIntBasedata", query.recordValue("int_basedata"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "miDtSendTime", query.recordValue("dt_sendtime"));

    cJSON_AddNumberToObject(pItem, "state", state);
    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_m_put_piece_t_m_member(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("vch_memberno", cJSON_help::getcJSONvalue(pItem,"miVchMemberNo"));
    key_value_list << qrtVariantPair("vch_member", cJSON_help::getcJSONvalue(pItem,"miVchMember"));
    key_value_list << qrtVariantPair("ch_typeno", cJSON_help::getcJSONvalue(pItem,"miChTypeNo"));
    key_value_list << qrtVariantPair("vch_id", cJSON_help::getcJSONvalue(pItem,"miVchId"));
    key_value_list << qrtVariantPair("dt_birthday", cJSON_help::getcJSONvalue(pItem,"miDtBirthday"));
    key_value_list << qrtVariantPair("vch_tel", cJSON_help::getcJSONvalue(pItem,"miVchTel"));
    key_value_list << qrtVariantPair("vch_handtel", cJSON_help::getcJSONvalue(pItem,"miVchHandTel"));
    key_value_list << qrtVariantPair("vch_address", cJSON_help::getcJSONvalue(pItem,"miVchAddress"));
    key_value_list << qrtVariantPair("dt_limit", cJSON_help::getcJSONvalue(pItem,"miDtLimit"));
    key_value_list << qrtVariantPair("ch_state", cJSON_help::getcJSONvalue(pItem,"miChState"));
    key_value_list << qrtVariantPair("num_limit", cJSON_help::getcJSONvalue(pItem,"numLimit"));
    key_value_list << qrtVariantPair("ch_cardflag", cJSON_help::getcJSONvalue(pItem,"miChCardFlag"));
    key_value_list << qrtVariantPair("vch_cardno", cJSON_help::getcJSONvalue(pItem,"miVchCardNo"));
    key_value_list << qrtVariantPair("vch_password", cJSON_help::getcJSONvalue(pItem,"miVchPassword"));
    key_value_list << qrtVariantPair("vch_operID", cJSON_help::getcJSONvalue(pItem,"miVchOperId"));
    key_value_list << qrtVariantPair("dt_operdate", cJSON_help::getcJSONvalue(pItem,"miDtOperDate"));
    key_value_list << qrtVariantPair("int_basedata", cJSON_help::getcJSONvalue(pItem,"miIntBasedata"));
    key_value_list << qrtVariantPair("dt_sendtime", cJSON_help::getcJSONvalue(pItem,"miDtSendTime"));

}

cJSON *w_sys_manage::q_m_get_piece_t_m_gift(lds_query_hddpos  &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::_cJSON_AddValueToObject( pItem, "intFlowid", query.recordValue("int_flowID"));

    cJSON_help::_cJSON_AddValueToObject( pItem, "chGiftno", query.recordValue("ch_giftno"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "vhcGifname", query.recordValue("vch_giftname"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "numPoint", query.recordValue("num_point"));

    cJSON_AddNumberToObject(pItem, "state", state);
    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_m_put_piece_t_m_gift(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("int_flowID",cJSON_help::getcJSONvalue(pItem,"intFlowid"));
    key_value_list << qrtVariantPair("ch_giftno",cJSON_help::getcJSONvalue(pItem,"chGiftno"));
    key_value_list << qrtVariantPair("vch_giftname",cJSON_help::getcJSONvalue(pItem,"vhcGifname"));
    key_value_list << qrtVariantPair("num_point",cJSON_help::getcJSONvalue(pItem,"numPoint"));

}


cJSON *w_sys_manage::q_m_get_piece_t_m_curamount(lds_query_hddpos  &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::_cJSON_AddValueToObject( pItem, "vchMemberno", query.recordValue("vch_memberno"));

    cJSON_help::_cJSON_AddValueToObject( pItem, "numAmount", query.recordValue("num_amount"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "numHangamount", query.recordValue("num_hangamount"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "numPoint", query.recordValue("num_point"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "vchOperId", query.recordValue("vch_operID"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "dtOperdate", query.recordValue("dt_operdate"));
    cJSON_help::_cJSON_AddValueToObject( pItem, "chBranchno", query.recordValue("ch_branchno"));

    cJSON_AddNumberToObject(pItem, "state", state);
    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_m_put_piece_t_m_curamount(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("vch_memberno",cJSON_help::getcJSONvalue(pItem,"vchMemberno"));
    key_value_list << qrtVariantPair("num_amount",cJSON_help::getcJSONvalue(pItem,"numAmount"));
    key_value_list << qrtVariantPair("num_hangamount",cJSON_help::getcJSONvalue(pItem,"numHangamount"));
    key_value_list << qrtVariantPair("num_point",cJSON_help::getcJSONvalue(pItem,"numPoint"));
    key_value_list << qrtVariantPair("vch_operID",cJSON_help::getcJSONvalue(pItem,"vchOperId"));
    key_value_list << qrtVariantPair("dt_operdate",cJSON_help::getcJSONvalue(pItem,"dtOperdate"));
    key_value_list << qrtVariantPair("ch_branchno",cJSON_help::getcJSONvalue(pItem,"chBranchno"));

}

cJSON *w_sys_manage::q_m_get_piece_t_m_parameter(lds_query_hddpos  &query, const QString &orgCode)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::_cJSON_AddValueToObject(pItem, "vchParameter", query.recordValue("vch_parameter"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "vchValue", query.recordValue("vch_value"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "vchExplain", query.recordValue("vch_explain"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_m_put_piece_t_m_parameter(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("vch_parameter", cJSON_help::getcJSONvalue(pItem,"vchParameter").toString());
    key_value_list << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"vchValue").toString());
    key_value_list << qrtVariantPair("vch_explain", cJSON_help::getcJSONvalue(pItem,"vchExplain").toString());

}
