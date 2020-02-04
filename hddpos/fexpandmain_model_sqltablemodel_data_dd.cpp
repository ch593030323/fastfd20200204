#include "fexpandmain_model_sqltablemodel_data_dd.h"
#include "lds_scr_qt.h"

fexpandmain_model_sqltablemodel_data_dd::fexpandmain_model_sqltablemodel_data_dd(QObject *parent)
    : fexpandmain_model_sqltablemodel_data(parent)
{
}

void fexpandmain_model_sqltablemodel_data_dd::senddd(const lds::DOUBLE_DATA &data, float dish_num_old, const QString &SENDD_DATA_FROM_DATABASE)
{
    lds_scr_qt::doublescreensenddataByconf(data);
    fexpandmain_model_sqltablemodel_data::senddd(data, dish_num_old, SENDD_DATA_FROM_DATABASE);
}

void fexpandmain_model_sqltablemodel_data_dd::senddd_update_insert(int row, const QString &udp_request_type, const fexpandmain_keepData &keepd, double dish_num_old, const QString &SENDD_DATA_FROM_DATABASE)
{
    lds::DOUBLE_DATA ddata;
    ddata.udpKey = udp_request_type;
    ddata.row = row;
    ddata.items.vch_dishname = get_vch_dishname_by_map(model_data(row, "ch_dishno").toString());
    ddata.items.ch_dishno = model_data(row, "ch_dishno").toString();
    ddata.items.dish_price = get_cey_sys_parameter_value("pay_unit_prefix").toString()+ QString::number(model_data_NIBOLAN(row, "num_price"), 'f', get_cey_sys_parameter_value("bp_fmt_price_dec", 2).toInt());
    ddata.items.dish_num = QString::number(keepd.dish_num(), 'f', get_cey_sys_parameter_value("bp_fmt_num_dec", 3).toInt());
    ddata.items.dish_total = QString::number(keepd.dish_total(), 'f', get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    ddata.items.vch_other = get_dish_desc(row);
    ddata.items.ch_suitflag = model_data(row, "ch_suitflag").toString();

    senddd(ddata, dish_num_old, SENDD_DATA_FROM_DATABASE);
}

void fexpandmain_model_sqltablemodel_data_dd::emit_num_total_change_DD(const ldsVariantMap &num, const ldsVariantMap &total) const
{
    lds::DOUBLE_DATA ddata;
    ddata.udpKey = lds::udp_title_request;
    ddata.title_num = cJSON_help::mapTocJSONByteArray(num, true);
    ddata.title_total = cJSON_help::mapTocJSONByteArray(total, true);
    lds_scr_qt::doublescreensenddataByconf(ddata);
}
