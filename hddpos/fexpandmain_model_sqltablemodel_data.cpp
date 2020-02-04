#include "fexpandmain_model_sqltablemodel_data.h"
#include "lds_network.h"
#include "lds_scr_table.h"
#include "lds_scr_qt.h"
#include "cjson_help.h"
#include "lds_messagebox.h"
#include "lds_scr_table.h"
#include "w_bt_dish.h"
#include "w_sys_manage_outer_pay_set_tax_type.h"

const QString fexpandmain_model_sqltablemodel_data::SENDD_DATA_FROM_DATABASE = "SENDD_DATA_FROM_DATABASE";

fexpandmain_model_sqltablemodel_data::fexpandmain_model_sqltablemodel_data(QObject *parent)
    : fexpandmain_model_sqltablemodel(parent)
{
    kd_enable_row_begin = 0;
    kd_enable = true;
    n_m_filter_name_list
            << "num_num"
            << "num_back"
            << "num_price"
            << "num_price_add"
            << "int_discount"
            << "ch_presentflag"
            << "int_rate"
            << "ch_suitflag"
            << "ch_specialflag"

            << "vch_print_memo";

}

bool fexpandmain_model_sqltablemodel_data::insertRow(int arow, const QModelIndex &aparent)
{
    ///
    lds_model_sqltablemodel::insertRow(arow, aparent);
    ///
    return true;
}

bool fexpandmain_model_sqltablemodel_data::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role != Qt::EditRole) {
        return lds_model_sqltablemodel::setData(index, value, role);
    }
    fexpandmain_keepData _keepdata0, _keepdata1;

    int row = index.row();
    bool f;
    lds_bool_default_false need2update;
    float dish_num_old = 0;
    ///
    if(n_m_filter_index_list.isEmpty()) {
        foreach(const QString &f, n_m_filter_name_list)
            n_m_filter_index_list.append(this->fieldIndex(f));
    }
    need2update = n_m_filter_index_list.contains(index.column())
            && kd_enable;

    ///
    if(need2update) {
        get_keep_data(row, _keepdata0);
        dish_num_old = model_data_NIBOLAN(row, "num_num-num_back");
    }
    ///
    f = true;

    if(this->fieldIndex("num_num") == index.column()) {
        f = lds_model_sqltablemodel::setData(index, num_dec(value.toDouble()));
    } else if(this->fieldIndex("num_back") == index.column()) {
        f = lds_model_sqltablemodel::setData(index, num_dec(value.toDouble()));
    } else if(this->fieldIndex("num_price") == index.column()) {
        f = lds_model_sqltablemodel::setData(index, price_dec(value.toDouble()));
    } else if(this->fieldIndex("num_price_org") == index.column()) {
        f = lds_model_sqltablemodel::setData(index, price_dec(value.toDouble()));
    } else if(this->fieldIndex("num_price_add") == index.column()) {
        f = lds_model_sqltablemodel::setData(index, price_dec(value.toDouble()));
    } else if(this->fieldIndex("int_flowid") == index.column()) {
        /*不赋值，由数据库自动生成*/
    } else {
        f = lds_model_sqltablemodel::setData(index, value, role);
    }

    //    if(this->fieldIndex("int_discount") == index.column()
    //            || this->fieldIndex("num_price_add") == index.column()
    //            || this->fieldIndex("vch_print_memo") == index.column()) {
    //        if(get_cey_sys_parameter_value("cook_discount_with_dish_flag", "0").toString() == "1") {
    //            //这里不能用 model_data_set函数，虚函数virtual会影响父类
    //            f = lds_model_sqltablemodel::setData( this->index(row, fieldIndex("num_price_add")) ,
    //                                                  price_dec(vch_print_memoData::total(lds_model_sqltablemodel::model_data(row, "vch_print_memo").toString())
    //                                                            * lds_model_sqltablemodel::model_data(row, "int_discount").toInt() / 100.0
    //                                                            ) );
    //        }
    //    }
    ///
    if(need2update) {
        get_keep_data(row, _keepdata1);
        senddd_update_insert(row, lds::udp_update_request, _keepdata1, dish_num_old);

        fexpandmain_keepData k = _keepdata1 - _keepdata0;
        _keepdata += k;
        emit_num_total_change_label(_keepdata.dish_num(), _keepdata.dish_total());
    }
    return f;
}

bool fexpandmain_model_sqltablemodel_data::removeRows(int row, int count, const QModelIndex &parent)
{
    //
    if(kd_enable)
    {
        fexpandmain_keepData _keepdata0;
        for(int k = 0; k < count; k ++) {
            int delRow = row + count - k - 1;

            fexpandmain_keepData d;
            get_keep_data(delRow, d);
            //
            _keepdata0 += d;
        }
        //
        _keepdata -= _keepdata0;
        //
        lds::DOUBLE_DATA ddata;
        ddata.udpKey = lds::udp_delete_request;
        ddata.row = row;
        ddata.count = count;
        senddd(ddata);
        //
    }
    ///
    bool f = lds_model_sqltablemodel::removeRows(row, count, parent);
    if(kd_enable)
        emit_num_total_change_label(_keepdata.dish_num(), _keepdata.dish_total());
    return f;
}

void fexpandmain_model_sqltablemodel_data::keepdata_insert_prepare(int rowbegin)
{
    if(rowbegin != -1) {
        kd_enable_row_begin =  rowbegin;
    } else {
        kd_enable_row_begin = this->rowCount();
    }
    kd_enable = false;
}

void fexpandmain_model_sqltablemodel_data::keepdata_insert_submit(int rowEnd)
{
    if(rowEnd == -1) {
        rowEnd = this->rowCount() - 1;
    }
    kd_enable = true;

    for(int row = kd_enable_row_begin; row <= rowEnd; row++) {
        fexpandmain_keepData d;
        get_keep_data(row, d);
        //发送副屏数据
        senddd_update_insert(row, lds::udp_insert_request, d, 0);
        _keepdata += d;
    }

    emit_num_total_change_label(_keepdata.dish_num(), _keepdata.dish_total());
}

void fexpandmain_model_sqltablemodel_data::keepdata_insert_del(int rowEnd)
{
    if(rowEnd == -1) {
        rowEnd = this->rowCount() - 1;
    }
    removeRows(kd_enable_row_begin, rowEnd + 1 - kd_enable_row_begin);

    kd_enable = true;
}

void fexpandmain_model_sqltablemodel_data::keepdata_Y_dish_fold(QTableView *tableview, int rowP)
{
    for(int row = rowP; row < this->rowCount(); row ++) {
        if(this->can_qt_P_dish_fold(row)) {
            this->row_fold_map.insert(row, true);
        }

        if(this->can_qt_Y_dish_fold(row)) {
            tableview->hideRow(row);
        }
    }
}

QString fexpandmain_model_sqltablemodel_data::cur_ch_billno() const
{
    return _ch_billno;
}

QString fexpandmain_model_sqltablemodel_data::cur_ch_tableno() const
{
    if(_ch_billno.startsWith("B")) {
        lds_query_hddpos  query;
        query.execSelect(QString("SELECT ch_tableno from cey_u_table where ch_billno = '%1' ")
                         .arg(_ch_billno));
        query.next();
        return query.recordValue(0).toString();
    }

    return lds_scr_table::Q000;
}

void fexpandmain_model_sqltablemodel_data::senddd(const lds::DOUBLE_DATA &data, float dish_num_old, const QString &SENDD_DATA_FROM_DATABASE)
{
    int row = data.row;

    if(data.udpKey == lds::udp_insert_request) {//插入新的数量，只有在commitdata 时，有效
        ///编号<可追加的set_row>
        ch_dishno_set_row_map_UPDATE(row, SENDD_DATA_FROM_DATABASE);
        ///编号<数量>
        ch_dishno_dish_num_map_UPDATE(row, data.items.dish_num.toDouble(), SENDD_DATA_FROM_DATABASE);//插入新的数量
        ///商品沽清
        ch_dishno_guqing_UPDATE(row, data.items.dish_num.toDouble(), SENDD_DATA_FROM_DATABASE);
        ///fexpandmain_delegate_row
        delegate_row_INSERT(row);
    }
    if(data.udpKey == lds::udp_update_request) {
        ///编号<可追加的set_row>
        ch_dishno_set_row_map_UPDATE(row, SENDD_DATA_FROM_DATABASE);
        ///编号<数量>
        ch_dishno_dish_num_map_UPDATE(row, data.items.dish_num.toDouble() - dish_num_old, SENDD_DATA_FROM_DATABASE);//更新时，新旧互减
        ///商品沽清
        ch_dishno_guqing_UPDATE(row, data.items.dish_num.toDouble() - dish_num_old, SENDD_DATA_FROM_DATABASE);
    }
    if(data.udpKey == lds::udp_delete_request) {
        ///编号<可追加的set_row>
        ch_dishno_set_row_map_DEL(row, data.count, SENDD_DATA_FROM_DATABASE);
        ///编号<数量>
        ch_dishno_dish_num_map_DEL(row, data.count, SENDD_DATA_FROM_DATABASE);
        ///商品沽清
        ch_dishno_guqing_DEL(row, data.count, SENDD_DATA_FROM_DATABASE);
        ///fexpandmain_delegate_row
        delegate_row_DEL(row, data.count);
    }
    if(data.udpKey == lds::udp_delete_all_request) {
        //do nothing
    }
}

double fexpandmain_model_sqltablemodel_data::model_expression_value(int row,  QString expression, bool num_price_is_m) const
{
    QString ch_dishno = model_data(row, "ch_dishno").toString();
    QString num_m_price = QString::number(get_num_m_price_by_map(ch_dishno));
    if(true == num_price_is_m) {
        expression = model_expression_replace(expression, "num_price", num_m_price);
    }
    return model_data_IF(row, expression);
}

void fexpandmain_model_sqltablemodel_data::senddd_update_insert(int row, const QString &udp_request_type, const fexpandmain_keepData &keepd, double dish_num_old, const QString &SENDD_DATA_FROM_DATABASE)
{

}

void fexpandmain_model_sqltablemodel_data::get_keep_data(int row, fexpandmain_keepData &kdata)
{
    get_keep_data_pay_by_isMPrice_intDiscount(this, row, kdata);
}

void fexpandmain_model_sqltablemodel_data::get_keep_data_pay_by_isMPrice_intDiscount(const fexpandmain_model_sqltablemodel_data *tablemodel, int row, fexpandmain_keepData &kdata, bool pay_is_m_price, int pay_int_discount)
{
    QString ch_dishno = tablemodel->model_data(row, "ch_dishno").toString();
    QString min_num_m_price;
    if(false == tablemodel->can_discount(row)) {
        pay_int_discount = -1;
    }

    if(false == tablemodel->can_m_price(row)) {
        pay_is_m_price = false;
    }
    if(pay_is_m_price) {
        min_num_m_price = QString::number(qMin(tablemodel->model_data(row, "num_price").toDouble(), tablemodel->get_num_m_price_by_map(ch_dishno)));
    }

    kdata.num_price_add = tablemodel->model_data_IF(row, expression_replace(tablemodel, "num_price_add", min_num_m_price, pay_int_discount));
    kdata.consum = tablemodel->model_data_IF(row, expression_replace(tablemodel, "(num_num-num_back)*num_price", min_num_m_price, pay_int_discount));
    kdata.discount = kdata.consum * tablemodel->model_data_IF(row, expression_replace(tablemodel, "(1-int_discount*0.01)", min_num_m_price, pay_int_discount));
    kdata.present = kdata.consum * tablemodel->model_data_IF(row, expression_replace(tablemodel, "if(ch_presentflag = 'Y', 1, 0)", min_num_m_price, pay_int_discount));
    kdata.num = tablemodel->model_data_IF(row, expression_replace(tablemodel, "num_num-num_back", min_num_m_price, pay_int_discount));

    //P
    const double P =  kdata.consum - kdata.discount - kdata.present + kdata.num_price_add;//做法算到服务费和税收里
    kdata.consum  += kdata.num_price_add;
    //都精确小数
    kdata.consum = lds::doubleRound(kdata.consum, tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());

    //tax sc
    QString ch_billno = tablemodel->cur_ch_billno();
    QString ch_tableno =  tablemodel->get_cey_u_table_value(ch_billno, "ch_tableno").toString();
    double num_rate_total = 0;
    double tax_1 = 0;
    double tax_2 = 0;
    double tax_3 = 0;
    double tax = 0;

    double sc = 0;
    //tax
    if(tablemodel->get_cey_u_taxsale_master_value(ch_billno, "ch_tax_type").toInt() != 0 ) {
        if(tablemodel->get_cey_u_taxsale_master_value(ch_billno, "ch_fix_flag").toString() == "Y") {
            tax_1 = tablemodel->get_cey_u_taxsale_master_value(ch_billno, "int_fix_tax").toDouble() / 100.0;
            tax_2 = tablemodel->get_cey_u_taxsale_master_value(ch_billno, "int_fix_tax_2").toDouble() / 100.0;
            tax_3 = tablemodel->get_cey_u_taxsale_master_value(ch_billno, "int_fix_tax_3").toDouble() / 100.0;
        } else {
            int tax_check1 = tablemodel->tablename_kvmap_value("cey_sys_parameter", "tax_check1", "vch_parameter", "vch_value").toInt();
            int tax_check2 = tablemodel->tablename_kvmap_value("cey_sys_parameter", "tax_check2", "vch_parameter", "vch_value").toInt();
            int tax_check3 = tablemodel->tablename_kvmap_value("cey_sys_parameter", "tax_check3", "vch_parameter", "vch_value").toInt();

            tax_1 = tablemodel->get_cey_bt_dish_value(ch_dishno, "int_rate").toDouble()  / 100.0 ;
            tax_2 = tablemodel->get_cey_bt_dish_value(ch_dishno, "int_rate_2").toDouble()  / 100.0 ;
            tax_3 = tablemodel->get_cey_bt_dish_value(ch_dishno, "int_rate_3").toDouble()  / 100.0 ;

            if(tax_check1 != 1) tax_1 = 0;
            if(tax_check2 != 1) tax_2 = 0;
            if(tax_check3 != 1) tax_3 = 0;
        }
    }
    tax = tax_1 + tax_2 + tax_3;
    //sc
    if(tablemodel->get_cey_u_taxsale_master_value(ch_billno, "ch_fix_service").toString() == "Y") {
        sc = tablemodel->get_cey_u_taxsale_master_value(ch_billno, "int_fix_service").toDouble() * 0.01;
    } else {
        if(tablemodel->get_sevice_mode_by_map(ch_tableno).toInt() == 4//商品消费服务率
                && tablemodel->get_ch_serviceflag_by_map(ch_dishno) == "Y"//ch_serviceflag = 'Y'
                ) {
            sc = tablemodel->get_sevice_int_rate_by_map(ch_tableno) * 0.01;
        }
    }


    //ch_tax_mode
    int ch_tax_mode = tablemodel->get_cey_u_taxsale_master_value(ch_billno, "ch_tax_mode").toInt();
    switch(ch_tax_mode) {
    case 1://含税,不含服务费
        num_rate_total = P * tax / (tax + 1);
        kdata.num_service = P *sc / (tax + 1);
        break;
    case 2://不含税,含服务费
        kdata.num_service = P * sc;
        num_rate_total = P * (sc + 1) * tax;
        break;
    case 3://含税,含服务费
        kdata.num_service = P * sc/ ((1 + sc ) *tax +1);
        num_rate_total =  P * tax * (1 + sc) / ((1 + sc ) *tax +1);
        break;
    default://不含税,不含服务费
        num_rate_total = P *tax;
        kdata.num_service = P * sc;
        break;
    }
    kdata.num_rate_before = P;

    if(tax != 0) {
        kdata.num_rate1 = tax_1 / tax * num_rate_total;
        kdata.num_rate2 = tax_2 / tax * num_rate_total;
        kdata.num_rate3 = tax_3 / tax * num_rate_total;
    }

    kdata.print();
}

QString fexpandmain_model_sqltablemodel_data::expression_replace(const fexpandmain_model_sqltablemodel_data *tablemodel, QString expression, const QString &min_num_m_price, int pay_int_discount)
{
    if(min_num_m_price.length() > 0) {
        expression = tablemodel->model_expression_replace(expression, "num_price", min_num_m_price);
    }
    if(-1 != pay_int_discount) {
        expression = tablemodel->model_expression_replace(expression, "int_discount", QString::number(pay_int_discount));
    }
    return expression;
}

fexpandmain_keepData fexpandmain_model_sqltablemodel_data::get_keep_data_pay_by_isMPrice_intDiscount(const fexpandmain_model_sqltablemodel_data *tablemodel, bool pay_is_m_price, int pay_int_discount)
{
    fexpandmain_keepData kdata;
    tablemodel->keepdata_init(kdata);
    for(int row = 0, rowcount = tablemodel->rowCount(); row < rowcount; row++) {
        if(tablemodel->model_data(row, "ch_billno").toString() != tablemodel->cur_ch_billno()) continue;

        fexpandmain_keepData d;
        get_keep_data_pay_by_isMPrice_intDiscount(tablemodel, row, d, pay_is_m_price, pay_int_discount);
        kdata += d;
    }
    return kdata;
}

bool fexpandmain_model_sqltablemodel_data::select(const QString &ch_billno, const QString &select_filter)
{
    //1
    _ch_billno = ch_billno;
    setFilter(QString("ch_billno = '%1' ").arg(ch_billno) + (select_filter.length() == 0 ? "" : " and ") + select_filter);
    //副屏delete all
    lds::DOUBLE_DATA d;
    d.udpKey = lds::udp_delete_all_request;
    senddd(d);
    //初始化实时数据
    keepdata_init();
    //初始化检索的数据
    for(int row = 0, rowcount = this->rowCount(); row < rowcount; row++) {
        fexpandmain_keepData k;//汇总信息
        get_keep_data(row, k);
        //dd
        senddd_update_insert(row, lds::udp_insert_request, k, 0, SENDD_DATA_FROM_DATABASE);
        //
        _keepdata += k;
    }
    emit_num_total_change_label(_keepdata.dish_num(), _keepdata.dish_total());

    return true;
}

bool fexpandmain_model_sqltablemodel_data::select(const QString &ch_billno, QWidget *parent)
{
    bool f = select(ch_billno);
    //
    vch_membernod.init();
    if(rowCount() > 0) {
        QString errstring;
        QString ch_member_belong_state = model_data(0, "ch_member_belong_state").toString();
        QString vch_memberno = model_data(0, "vch_memberno").toString();
        if(ch_member_belong_state == "Y") {
            lds_messagebox_loading_show loading(parent, "", "LOADING");
            loading.show_delay();
        }
        if(!vch_membernod.init_by_memberno(vch_memberno,ch_member_belong_state, errstring)) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, errstring);
        }
    }
    //
    return f;
}

void fexpandmain_model_sqltablemodel_data::emit_num_total_change_DD(const ldsVariantMap &num, const ldsVariantMap &total) const
{
}

void fexpandmain_model_sqltablemodel_data::keepdata_init()
{
    ///点单记录初始化
    _keepdata = fexpandmain_keepData();
    keepdata_init(_keepdata);
    ///编号<可追加的set_row>
    ch_dishno_set_row_map_get.clear();
    ///编号<数量>
    ch_dishno_dish_num_map.clear();
}

void fexpandmain_model_sqltablemodel_data::keepdata_init(fexpandmain_keepData &kd) const
{
    ///点单记录初始化
    QString ch_tableno = get_cey_u_table_value(_ch_billno, "ch_tableno").toString();
    QString ch_typeno = get_cey_bt_table_value(ch_tableno, "ch_typeno").toString();
    int ch_service_mode = get_cey_bt_table_type_value(ch_typeno, "ch_service_mode").toInt();
    double num_ration = get_cey_bt_table_type_value(ch_typeno, "num_ration").toDouble();

    kd.num_pay_low = pay_low(ch_tableno, this->get_cey_u_table_value(_ch_billno, "int_person").toInt());
    kd.ch_service_mode = ch_service_mode;
    kd.num_ration = num_ration;

}

void fexpandmain_model_sqltablemodel_data::emit_num_total_change_label(float num, float total) const
{
    QString num_label = dish_num_str(num);
    QString total_label = dish_total_str(total);
    emit signals_num_total_change(num_label, total_label);
    emit signals_total(total);

    emit_num_total_change_DD(
                ldsVariantMap()
                .append("no", "qty")
                .append("name", QObject::tr("数量"))
                .append("value", lds::septNumber(num, get_cey_sys_parameter_value("bp_fmt_num_dec", 3).toInt())) ,
                ldsVariantMap()
                .append("no", "order")
                .append("name", QObject::tr("金额"))
                .append("value", lds::septNumber(total, get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt())));
}

void fexpandmain_model_sqltablemodel_data::emit_num_total_change_label_pay(float num, float total) const
{
    QString num_label = dish_num_str(num);
    QString total_label = dish_total_str(total, QObject::tr("应收"));
    emit signals_num_total_change(num_label, total_label);
    emit signals_total(total);

    emit_num_total_change_DD(
                ldsVariantMap()
                .append("no", "qty")
                .append("name", QObject::tr("数量"))
                .append("value", lds::septNumber(num, get_cey_sys_parameter_value("bp_fmt_num_dec", 3).toInt())) ,
                ldsVariantMap()
                .append("no", "pay")
                .append("name", QObject::tr("应收"))
                .append("value", lds::septNumber(total, get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt())));
}

QString fexpandmain_model_sqltablemodel_data::dish_total_str(float t) const
{
    return dish_total_str(t, QObject::tr("金额"));
}

QString fexpandmain_model_sqltablemodel_data::dish_total_str(float t, const QString &prefix) const
{
    return prefix+":"+
            get_cey_sys_parameter_value("pay_unit_prefix").toString()  + lds::septNumber(t, get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt(), true);
}

QString fexpandmain_model_sqltablemodel_data::dish_num_str(float n) const
{
    return dish_num_str(n,  QObject::tr("数量"));
}

QString fexpandmain_model_sqltablemodel_data::dish_num_str(float n, const QString &prefix) const
{
    return prefix+":" + lds::septNumber(n, get_cey_sys_parameter_value("bp_fmt_num_dec", 3).toInt(), true);
}

void fexpandmain_model_sqltablemodel_data::cey_u_orderdish_append(lds_query_hddpos  &queryRect, const QDateTime &dt, const QString &ch_tableno, const vch_print_memoData &vch_print_memo)
{
    cey_u_orderdish_insert(this->rowCount(), queryRect, dt, ch_tableno, vch_print_memo);
}

void fexpandmain_model_sqltablemodel_data::cey_u_orderdish_insert(int row, lds_query_hddpos &queryRect, const QDateTime &dt, const QString &ch_tableno, const vch_print_memoData &vch_print_memo)
{
    QString vch_plan_k3o3_state;
    QString vch_plan_k3o3_id;
    QString ch_suitflag = queryRect.recordValue("ch_suitflag").toString();
    QString ch_dishno = queryRect.recordValue("ch_dishno").toString();

    //int_id
    w_bt_dish::kitchenState x = w_bt_dish::KITCHEN_MIN_DEF;
    if(this->rowCount() != this->rowCountOp(lds_model_sqltablemodel::OpInsert)) {
        x = w_bt_dish::KITCHEN_JIACAI;
    }
    w_bt_dish::get_vch_plan_k3o3(vch_plan_k3o3_state, vch_plan_k3o3_id, ch_dishno, x);

    //int_id
    int int_id = 0;
    if(row > 0) {
        int_id = this->model_data(row - 1, "int_id").toInt();
    }
    int_id++;

    this->insertRow(row);

    this->model_data_set(row, "int_id", int_id);
    this->model_data_set(row, "ch_billno", _ch_billno);
    this->model_data_set(row, "ch_tableno", ch_tableno);
    this->model_data_set(row, "ch_dishno", ch_dishno);
    this->model_data_set(row, "num_price", this->get_cey_bt_dish_value (ch_dishno, "num_price") );

    this->model_data_set(row, "num_price_org", this->get_cey_bt_dish_value(ch_dishno, "num_price") );
    this->model_data_set(row, "num_price_add", 0);
    this->model_data_set(row, "num_num", queryRect.recordValue("num_default"));
    this->model_data_set(row, "num_back", 0);
    this->model_data_set(row, "vch_back_operID", "");

    this->model_data_set(row, "dt_back_operdate", DATETIME_VOID);
    this->model_data_set(row, "int_discount", 100);
    this->model_data_set(row, "vch_dis_operID", "");
    this->model_data_set(row, "dt_dis_operdate", DATETIME_VOID);
    this->model_data_set(row, "vch_memberno", this->vch_membernod.vch_memberno);
    this->model_data_set(row, "ch_member_belong_state", this->vch_membernod.ch_member_belong_state);

    this->model_data_set(row, "ch_consult", "");
    this->model_data_set(row, "vch_print_memo", "");
    this->model_data_set(row, "ch_suitflag", ch_suitflag);///
    this->model_data_set(row, "ch_suitno", queryRect.recordValue("ch_suitno"));///
    this->model_data_set(row, "ch_waidai_flag", "N");///
    this->model_data_set(row, "ch_addon_flag", "N");///
    this->model_data_set(row, "ch_specialflag", "N");

    this->model_data_set(row, "ch_presentflag", "N");
    this->model_data_set(row, "vch_pre_operID", "");
    this->model_data_set(row, "dt_pre_operdate", DATETIME_VOID);
    this->model_data_set(row, "vch_operID", lds::gs_operid);
    this->model_data_set(row, "dt_operdate", dt);

    this->model_data_set(row, "ch_togono", "");
    this->model_data_set(row, "ch_payno",  "");
    this->model_data_set(row, "vch_plan_k3o3_id", vch_plan_k3o3_id);
    this->model_data_set(row, "vch_plan_k3o3_state", vch_plan_k3o3_state);

    this->model_data_set(row, "vch_plan_desc", "");
    this->model_data_set(row, "int_rate",  get_ch_dishno_rate(_ch_billno, ch_dishno));

    //优先级
    if(ch_suitflag == "Y") {
        this->model_data_set(row, "num_price", 0);
    }
    //非子商品
    if(ch_suitflag != "Y" && vch_print_memo.count() > 0) {
        this->model_data_set(row, "vch_print_memo", vch_print_memo.sqlValue());
        this->model_data_set(row, "num_price_add", vch_print_memo.total());
    }
}

int fexpandmain_model_sqltablemodel_data::upper_bound_rowP(int rowY)
{
    if(model_data(rowY, "ch_suitflag").toString() == "Y") {
        rowY--;
        while(rowY >= 0 && rowY < rowCount()) {
            if(model_data(rowY, "ch_suitflag").toString() == "P") {
                return rowY;
            }
            rowY--;
        }
    }

    if(model_data(rowY, "ch_addon_flag").toString() == "Y") {
        rowY--;
        while(rowY >= 0 && rowY < rowCount()) {
            if(model_data(rowY, "ch_addon_flag").toString() == "P") {
                return rowY;
            }
            rowY--;
        }
    }

    return -1;
}

int fexpandmain_model_sqltablemodel_data::upper_bound_rowPN(int row, const QSet<int> &skip)
{
    for(row--; row >= 0 && row < rowCount(); row --) {
        if(skip.contains(row)) {
            continue;
        }
        if(can_qt_Y_dish_fold(row)) {

        } else {
            return row;
        }
    }
    return -1;
}

//bool kuaican_quit = false, bool only_update = false) const;
bool fexpandmain_model_sqltablemodel_data::try_commit_data(QString &errstring, fexpandmain_model_sqltablemodel_data::commitType type, QStringList *insert_int_flowid_list) const
{
    lds_query_hddpos  query;
    qrtVariantPairList key_value_list;
    QDateTime cur_dt_str = query.curDateTime();
    bool kuaican_quit = type & commitKuaicanQuit;
    bool only_update = type & commitUpdate;

    if(_ch_billno.isEmpty()) {
        errstring = "model ch_billno is empty";
        return false;
    }
    for(int row = 0; row < this->rowCount(); row ++) {

        //非新增和修改的,不进行本操作
        if(opera(row) != OpInsert
                && opera(row) != OpUpdate)
            continue;
        //若只是更新
        if(only_update)
            if(opera(row) != OpUpdate)
                continue;

        //qrtVariantPair
        if(opera(row) == OpInsert) {
            //c == 0,int_flowid skip
            key_value_list.clear();
            for(int c = 1; c < this->columnCount(); c ++) {
                qrtVariantPair pair = qrtVariantPair(headerData(c, Qt::Horizontal).toString(), model_data(row, c, Qt::EditRole));
                //kuaican_quit
                if(kuaican_quit) {
                    if("num_num" == pair.first) {
                        pair.second = 0;
                    }
                    if("num_back" == pair.first) {
                        pair.second = model_data_NIBOLAN(row, "num_num - num_back");
                    }
                    if("vch_back_operID" == pair.first) {
                        pair.second = lds::gs_operid;
                    }
                    if("dt_back_operdate" == pair.first) {
                        pair.second = cur_dt_str;
                    }
                    if("vch_print_memo" == pair.first) {
                        vch_print_memoData d(pair.second.getValue().toString());
                        pair.second = d.sqlValue(-1);
                    }
                    if("num_price_add" == pair.first) {
                        pair.second = -1 * pair.second.getValue().toDouble();
                        qDebug() << pair.second;
                    }
                }
                //~kuaican_quit
                key_value_list << pair;
                if("num_back" == pair.first) {
                    qDebug() << pair.first << pair.second;
                }
            }
        }
        if(opera(row) == OpUpdate) {
            key_value_list.clear();
            for(int column = 0; column < this->columnCount(); column ++) {
                if(isDirty(index(row, column))) {
                    key_value_list << qrtVariantPair(record(row).fieldName(column), model_data(row, column, Qt::EditRole));
                }
            }
        }
        //exec
        if(opera(row) == OpInsert) {
            if(!query.execInsert(tableName(), key_value_list, &errstring)) {
                return false;
            }
            if(insert_int_flowid_list) {
                insert_int_flowid_list->append(lds_query_hddpos::selectValue(QString(" select int_flowid from cey_u_orderdish where ch_billno = '%1' and int_id = %2  ")
                                                                             .arg(_ch_billno)
                                                                             .arg(this->model_data(row, "int_id").toInt())).toString());
            }
            {
                lds_query_db query;
                query.execSelect(QString(" select int_flowid, num_num, num_back from cey_u_orderdish where ch_billno = '%1' and int_id = %2  ")
                                 .arg(_ch_billno)
                                 .arg(this->model_data(row, "int_id").toInt()));
                query.next();
                qDebug() << "num_back:" << query.recordValue("num_back").toString() << "num_num:"<< query.recordValue("num_num").toString();
                query.execSelect(QString(" select if(isnull(num_back), 'null', 'valid'), if(isnull(num_num), 'null', 'valid') from cey_u_orderdish where ch_billno = '%1' and int_id = %2  ")
                                 .arg(_ch_billno)
                                 .arg(this->model_data(row, "int_id").toInt()));
                query.next();
                qDebug() << "num_back:" << query.recordValue(0).toString() << "num_num:"<< query.recordValue(1).toString();
            }
        }
        if(opera(row) == OpUpdate) {
            if(!query.execUpdate(tableName(), key_value_list,
                                 qrtEqual("int_flowid",  model_data(row, "int_flowid").toString()), &errstring)) {
                return false;
            }
        }
        //material
        if(!lds_scr_qt::material_insert(query, _ch_billno, model_data(row, "int_id").toInt())) {
            errstring = query.recordError();
            return false;
        }
    }//for

    //判断负库存不允许销售
    if(get_cey_sys_parameter_value("inventory_negative_cannot_pay", "0").toString() == "1") {
        QString msg;
        QString record;
        QString ch_dishno;
        QString ch_dishno_sub;
        QString int_id;
        lds_query_hddpos  query2;
        //销售数量大于0
        query.execSelect(QString("select c.ch_dishno,  a.num_num, b.int_id, b.int_flowid "
                                 "from cey_st_material_num a, cey_u_orderdish b, cey_bt_dish c  where b.ch_billno = '%1' and (b.num_num-b.num_back) > 0 and a.ch_materialno = b.ch_dishno and b.ch_dishno = c.ch_dishno  order by b.int_flowid asc; ")
                         .arg(_ch_billno));
        while(query.next()) {
            ch_dishno = query.recordValue("ch_dishno").toString();
            int_id = query.recordValue("int_id").toString();
            if(get_item_flag_by_map(ch_dishno) == "Y" && query.recordValue("num_num").toDouble() < 0) {//库存管理
                record = int_id.rightJustified(2, ' ') + "." + QObject::tr("商品") + ":\t"+ get_vch_dishname_by_map(ch_dishno);
                msg += record + "\n";
                ch_dishno = "";
            }

            query2.execSelect(QString("select b.ch_dishno, a.num_num "
                                      "from cey_st_material_num a, material_u_orderdish b  where b.int_order_flowID = %1 and a.ch_materialno = b.ch_dishno  and a.num_num < 0 ; ")
                              .arg(query.recordValue("int_flowid").toString()));
            while(query2.next()) {
                ch_dishno_sub = query2.recordValue("ch_dishno").toString();
                if(get_item_flag_by_map(ch_dishno_sub) == "Y") {//库存管理
                    record = "---"+QObject::tr("原料")+":\t"+ get_vch_dishname_by_map(ch_dishno_sub);
                    if(!ch_dishno.isEmpty()) {
                        record += "\t"+ QObject::tr("所属主商品") + ":" +int_id.rightJustified(2, ' ') + "." + get_vch_dishname_by_map(ch_dishno);
                    }
                    msg += record + "\n";
                }
            }
        }
        if(!msg.isEmpty()) {
            errstring = QObject::tr("负库存不允许销售")+"\n"+msg;
            return false;
        }
    }
    return true;
}

bool fexpandmain_model_sqltablemodel_data::try_update_data(QString &errstring) const
{
    return try_commit_data(errstring, commitUpdate);
}

QString fexpandmain_model_sqltablemodel_data::msg_mid_by_N(const QString &msg, int pos, int length, bool show_dot_when_not_complete)
{
    QStringList m = msg.split("\n", QString::SkipEmptyParts);
    QString app;
    if(show_dot_when_not_complete) {
        if(m.length() > pos + length) {
            app = "\n...";
        }
    }
    m = m.mid(pos, length);
    return m.join("\n")+app;
}

const fexpandmain_keepData &fexpandmain_model_sqltablemodel_data::getKeepdata() const
{
    return _keepdata;
}

fexpandmain_keepData fexpandmain_model_sqltablemodel_data::getKeepdata(int row) const
{
    fexpandmain_keepData kdata;
    get_keep_data_pay_by_isMPrice_intDiscount(this, row, kdata);

    return kdata;
}

bool fexpandmain_model_sqltablemodel_data::vch_membernod_m_update(QString &errstring)
{
    //会员价
    if(vch_membernod.ch_favourable == "1") {//会员价
        if(vch_membernod.ch_pricekind.isEmpty()) {
            errstring = QObject::tr("没有设置会员价")+","+QObject::tr("请到会员类型里修改");
            return false;
        }
    }

    for(int row = 0, rowcount = rowCount(); row < rowcount; row ++) {
        if(vch_membernod.is_m_price()) {
            if(can_m_price(row)) {
                model_data_set(row, "num_price", get_num_m_price_by_map(model_data(row, "ch_dishno").toString()));
            }
        }
        if(vch_membernod.is_m_discount()) {
            if(can_discount(row)) {
                model_data_set(row, "int_discount", vch_membernod.int_discount);
            }
        }
    }

    for(int row = 0, rowcount = rowCount(); row < rowcount; row ++) {
        model_data_set(row, "vch_memberno", vch_membernod.vch_memberno);
        model_data_set(row, "ch_member_belong_state", vch_membernod.ch_member_belong_state);
    }
    return true;
}

QSet<int> fexpandmain_model_sqltablemodel_data::ch_dishno_set_row_map_GET(const QString &ch_dishno) const
{
    return ch_dishno_set_row_map_get.value(ch_dishno);
}

QSet<int> fexpandmain_model_sqltablemodel_data::ch_dishno_set_row_map_GET(int row) const
{
    return ch_dishno_set_row_map_GET(model_data(row, "ch_dishno").toString());
}

const QMap<QString, QSet<int> > &fexpandmain_model_sqltablemodel_data::ch_dishno_set_row_map_GET() const
{
    return ch_dishno_set_row_map_get;
}

void fexpandmain_model_sqltablemodel_data::ch_dishno_set_row_map_VOID(int row, int count)
{
    for(QMap<QString, QSet<int> >::iterator k = ch_dishno_set_row_map_get.begin(); k != ch_dishno_set_row_map_get.end(); k++) {
        while(count > 0 && k.value().contains(row)) {
            k.value().remove(row);

            count--;
            row ++;
        }
        if(count == 0)
            break;
    }
}

//bool fexpandmain_model_sqltablemodel_data::can_change_num(int row) const
//{
//    int count = ch_dishno_set_row_map_GET(row).count();
//    //没有记录在案，但是允许追加
//    if(count == 0
//            && can_org_dish_append(row)
//            ) return true;

//    return count;
//}

void fexpandmain_model_sqltablemodel_data::ch_dishno_set_row_map_PRINT()
{
#ifdef QT_DEBUG
    QMapIterator<QString, QSet<int> > mapitr(ch_dishno_set_row_map_get);
    int count = 0;
    qDebug() << "================ == ";
    while(mapitr.hasNext()) {
        mapitr.next();
        if(!mapitr.value().isEmpty()) {
            qDebug() << "---------------------------------";
            qDebug() << get_vch_dishname_by_map(mapitr.key()) << mapitr.value();
            foreach(int d, mapitr.value()) {
                qDebug() << "行:" << d << get_vch_dishname_by_map(model_data(d, "ch_dishno").toString());
            }
            count += mapitr.value().count();
        }
    }
    qDebug() << "总行数" << count << ch_dishno_set_row_map_get;
#endif
}

void fexpandmain_model_sqltablemodel_data::ch_dishno_set_row_map_INSERT( int row, const QString &SENDD_DATA_FROM_DATABASE)
{
    if(SENDD_DATA_FROM_DATABASE == fexpandmain_model_sqltablemodel_data::SENDD_DATA_FROM_DATABASE) {
        return;
    }
    QString ch_dishno = model_data(row, "ch_dishno").toString();
    if(ch_dishno_set_row_map_get.find(ch_dishno) == ch_dishno_set_row_map_get.end()) {
        ch_dishno_set_row_map_get.insert(ch_dishno, QSet<int>());
    }
    ch_dishno_set_row_map_get[ch_dishno].insert(row);
}

void fexpandmain_model_sqltablemodel_data::ch_dishno_set_row_map_UPDATE(int row, const QString &SENDD_DATA_FROM_DATABASE)
{
    if(SENDD_DATA_FROM_DATABASE == fexpandmain_model_sqltablemodel_data::SENDD_DATA_FROM_DATABASE) {
        return;
    }
    QString errstring;
    if(can_org_dish_append(row, &errstring)) {
        ch_dishno_set_row_map_INSERT(row, SENDD_DATA_FROM_DATABASE);
    } else {
        ch_dishno_set_row_map_VOID(row, 1);
    }
}

void fexpandmain_model_sqltablemodel_data::ch_dishno_set_row_map_DEL(int row, int count, const QString &SENDD_DATA_FROM_DATABASE)
{
    if(SENDD_DATA_FROM_DATABASE == fexpandmain_model_sqltablemodel_data::SENDD_DATA_FROM_DATABASE) {
        return;
    }
    QMap<QString, QString> ch_dishno_op_map;
    QString ch_dishno;
    //10002,  x4,x5,7-5,8-6
    for(int rowbegin = row, rowcount = rowCount(); rowbegin < rowcount; rowbegin ++) {
        ch_dishno = model_data(rowbegin, "ch_dishno").toString();
        if(rowbegin - row < count) {
            ch_dishno_op_map.insert(ch_dishno, ch_dishno_op_map.value(ch_dishno)+","+"x"+QString::number(rowbegin));
        } else {
            ch_dishno_op_map.insert(ch_dishno, ch_dishno_op_map.value(ch_dishno)+","+QString("%1=%2").arg(rowbegin).arg(rowbegin-count));
        }
    }
    //
    QMap<QString, QSet<int> >::iterator k;
    QMapIterator<QString, QString> mapitr(ch_dishno_op_map);
    while(mapitr.hasNext()) {
        mapitr.next();
        if((k=ch_dishno_set_row_map_get.find(mapitr.key())) != ch_dishno_set_row_map_get.end()) {
            QStringList op_list = mapitr.value().split(",", QString::SkipEmptyParts);
            foreach(QString op, op_list) {
                if(op.contains("x")) {//delete
                    op.remove(0, 1);
                    k.value().remove(op.toInt());
                }
                if(op.contains("=")) {//replace
                    QStringList ops = op.split("=", QString::SkipEmptyParts);
                    if(k.value().remove(ops[0].toInt())) {
                        k.value().insert(ops[1].toInt());
                    }
                }
            }
        }
    }
}

void fexpandmain_model_sqltablemodel_data::ch_dishno_set_row_map_CLEAR()
{
}

void fexpandmain_model_sqltablemodel_data::row_fold_map_RELEASE(QTableView *tableView)
{
    for(int k = 0; k < tableView->verticalHeader()->count(); k++) {
        tableView->verticalHeader()->showSection(k);
    }
    row_fold_map.clear();
}

void fexpandmain_model_sqltablemodel_data::row_fold_map_FOLD(QTableView *tableView, const QModelIndex &index)
{
    int row = index.row();
    bool fold_state = row_fold_map.value(row, get_cey_sys_parameter_value("qt_Y_dish_fold") == "1");
    for(int row = index.row() + 1; row < rowCount(); row ++) {
        if(can_qt_Y_dish_fold(row)) {
            if(fold_state) {
                tableView->showRow(row);
            } else {
                tableView->hideRow(row);
            }
            continue;
        }
        break;
    }
    row_fold_map.insert(index.row(), !fold_state);
    tableView->update(index);
}

void fexpandmain_model_sqltablemodel_data::ch_dishno_dish_num_map_DEL(int row, int count, const QString &SENDD_DATA_FROM_DATABASE)
{
    for(int k = 0; k < count; k++) {
        double off = -model_data_NIBOLAN(row + k, "num_num-num_back");
        ch_dishno_dish_num_map_UPDATE(row + k, off, SENDD_DATA_FROM_DATABASE);
    }
}

void fexpandmain_model_sqltablemodel_data::ch_dishno_dish_num_map_UPDATE(int row, double off, const QString &SENDD_DATA_FROM_DATABASE)
{
    QString ch_dishno = model_data(row, "ch_dishno").toString();
    QString ch_suitflag = model_data(row, "ch_suitflag").toString();

    //主要是记录子商品不计入数量计算，涉及到第二杯半价的情况
    if(ch_suitflag != "Y") {
        ch_dishno_dish_num_map.insert(ch_dishno, ch_dishno_dish_num_map.value(ch_dishno, 0) + off);
    }
}

void fexpandmain_model_sqltablemodel_data::ch_dishno_dish_num_map_PRINT()
{
#ifdef QT_DEBUG
    QMapIterator<QString, double> mapitr(ch_dishno_dish_num_map);
    while(mapitr.hasNext()) {
        mapitr.next();

        qDebug() <<  mapitr.key() + ":"+QString::number(mapitr.value()) + "\t";
    }
#endif
}

bool fexpandmain_model_sqltablemodel_data::ch_dishno_guqing_UPDATE(int row, double off, const QString &SENDD_DATA_FROM_DATABASE)
{
    if(SENDD_DATA_FROM_DATABASE == fexpandmain_model_sqltablemodel_data::SENDD_DATA_FROM_DATABASE) {
        return true;
    }
    QString ch_dishno = model_data(row, "ch_dishno").toString();
    return lds_scr_qt::guqing_update(ch_dishno, off);
}

bool fexpandmain_model_sqltablemodel_data::ch_dishno_guqing_DEL(int row, int count, const QString &SENDD_DATA_FROM_DATABASE)
{
    for(int k = 0; k < count; k++) {
        ch_dishno_guqing_UPDATE(row + k, -model_data_NIBOLAN(row + k, "num_num-num_back"), SENDD_DATA_FROM_DATABASE);
    }
    return true;
}

void fexpandmain_model_sqltablemodel_data::delegate_row_INSERT(int row)
{
    for(int k = row; k < rowCount(); k++) {
        tablename_kvmap_insert_value("fexpandmain_delegate_row",  QString::number(k),
                                     tablename_kvmap_value("fexpandmain_delegate_row",  QString::number(upper_bound_rowPN(k))).toInt() + 1);
    }
}

void fexpandmain_model_sqltablemodel_data::delegate_row_DEL(int row, int count)
{
    //删除行后，产生的序号差
    int jump =
            tablename_kvmap_value("fexpandmain_delegate_row", QString::number(row + count)).toInt() -
            tablename_kvmap_value("fexpandmain_delegate_row", QString::number(row)).toInt();

    //更新序号差
    for(int k = row; k < rowCount() - count; k++) {
        tablename_kvmap_insert_value("fexpandmain_delegate_row", QString::number(k),
                                     tablename_kvmap_value("fexpandmain_delegate_row", QString::number(k + count)).toInt() - jump);

    }
}

const QMap<QString, double> &fexpandmain_model_sqltablemodel_data::ch_dishno_dish_num_map_GET() const
{
    return ch_dishno_dish_num_map;
}

void fexpandmain_model_sqltablemodel_data::print_dish_info()
{
    //    ch_dishno_dish_num_map_PRINT();
    //    ch_dishno_set_row_map_PRINT();
}
