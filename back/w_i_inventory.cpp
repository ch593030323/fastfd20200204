#include "w_i_inventory.h"

QMap<QString, QVariant> w_i_inventory::MODEL_INVENTORY_TYPE;
QMap<QString, QVariant> w_i_inventory::MODEL_INVENTORY_STATE_TYPE;
QMap<QString, QVariant> w_i_inventory::MODEL_INVENTORY_STATE_DO;
QMap<QString, QVariant> w_i_inventory::MODEL_INVENTORY_SHEET_TYPE;
QMap<QString, QVariant> w_i_inventory::MODEL_INVENTORY_SHEET_NAME;

w_i_inventory::w_i_inventory(QObject *parent) :
    QObject(parent)
{
}

void w_i_inventory::clearCache()
{
    MODEL_INVENTORY_TYPE.clear();
    MODEL_INVENTORY_STATE_TYPE.clear();
    MODEL_INVENTORY_STATE_DO.clear();
    MODEL_INVENTORY_SHEET_TYPE.clear();

}

QMap<QString, QVariant>& w_i_inventory::get_MODEL_INVENTORY_TYPE()
{
    if(MODEL_INVENTORY_TYPE.count() == 0) {
        MODEL_INVENTORY_TYPE.insert("001", QObject::tr("采购入库"));
        MODEL_INVENTORY_TYPE.insert("002", QObject::tr("退货出货"));
        MODEL_INVENTORY_TYPE.insert("004", QObject::tr("物料报损"));
    }
    return MODEL_INVENTORY_TYPE;
}

QMap<QString, QVariant>& w_i_inventory::get_MODEL_INVENTORY_SHEET_TYPE()
{
    if(MODEL_INVENTORY_SHEET_TYPE.count() == 0) {
        MODEL_INVENTORY_SHEET_TYPE.insert("RK", "001");
        MODEL_INVENTORY_SHEET_TYPE.insert("TH", "002");
        MODEL_INVENTORY_SHEET_TYPE.insert("CK", "004");
    }
    return MODEL_INVENTORY_SHEET_TYPE;
}

QMap<QString, QVariant> &w_i_inventory::get_MODEL_INVENTORY_STATE_DO()
{
    if(MODEL_INVENTORY_STATE_DO.count() == 0) {
        MODEL_INVENTORY_STATE_DO.insert("N", QObject::tr("未审核"));
        MODEL_INVENTORY_STATE_DO.insert("Y", QObject::tr("审核"));
        MODEL_INVENTORY_STATE_DO.insert("O", QObject::tr("作废"));
    }
    return MODEL_INVENTORY_STATE_DO;
}

QMap<QString, QVariant>& w_i_inventory::get_MODEL_INVENTORY_STATE_TYPE()
{
    if(MODEL_INVENTORY_STATE_TYPE.count() == 0) {
        MODEL_INVENTORY_STATE_TYPE.insert("N", QObject::tr("未审核"));
        MODEL_INVENTORY_STATE_TYPE.insert("Y", QObject::tr("已审核"));
        MODEL_INVENTORY_STATE_TYPE.insert("O", QObject::tr("已作废"));
    }
    return MODEL_INVENTORY_STATE_TYPE;
}

QMap<QString, QVariant> &w_i_inventory::get_MODEL_INVENTORY_SHEET_NAME()
{
    if(MODEL_INVENTORY_SHEET_NAME.count() == 0) {
        MODEL_INVENTORY_SHEET_NAME.insert("RK", QObject::tr("采购入库"));
        MODEL_INVENTORY_SHEET_NAME.insert("TH", QObject::tr("退货出货"));
        MODEL_INVENTORY_SHEET_NAME.insert("CK", QObject::tr("物料报损"));
    }
    return MODEL_INVENTORY_SHEET_NAME;
}

QStringList w_i_inventory::inventory_queryfilter(const QString &sql)
{
    QStringList rets;
    if(sql.isEmpty()) return rets;
    lds_query_hddpos  query;
    query.execSelect(sql);
    while(query.next()) {
        rets << QString("%1(%2)")
                .arg(query.recordValue(1).toString())
                .arg(query.recordValue(0).toString());
    }
    return rets;
}

QString w_i_inventory::inventory_getopername(const QString &operid)
{
    lds_query_hddpos  query;
    query.execSelect(QString("select * from ("+backheader::SELECT_OPERID_NAME+" )t where t.vch_operID='%1'  ")
               .arg(operid));
    query.next();
    return query.recordValue("vch_operator").toString();

}

QString w_i_inventory::SHEET_NAME(const QString &ch_sheetType)
{
    return MODEL_INVENTORY_SHEET_NAME.value(ch_sheetType).toString();
}

int w_i_inventory::provide_PAY_FLAG(const QString &ch_sheetType)
{
//    MODEL_INVENTORY_SHEET_NAME.insert("RK", QObject::tr("采购入库"));
//    MODEL_INVENTORY_SHEET_NAME.insert("TH", QObject::tr("退货出货"));
//    MODEL_INVENTORY_SHEET_NAME.insert("CK", QObject::tr("物料报损"));

    if(ch_sheetType == "RK")
        return 1;
    if(ch_sheetType == "TH")
        return -1;
    if(ch_sheetType == "CK")
        return 0;
    return 0;
}
