#include "material_purchase_apply.h"
#include "ui_material_purchase_apply.h"
#include "n_func.h"
#include "w_scr_dish_select_operid.h"
#include <QDataWidgetMapper>
#include "backheader.h"
#include "w_i_inventory.h"
#include <QSqlRelationalDelegate>
#include "lds_model_sqltablemodel_delegate_com.h"
#include "lds.h"
#include "w_sys_manage_cloudsync.h"
#include "cjson_help.h"
#include "material_purchase_apply_new.h"
#include <QTimer>

material_purchase_apply::material_purchase_apply(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::material_purchase_apply)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);
    //
    ui->comboBox_CH_STATE->addItem(QObject::tr("未申请"), "0");
    ui->comboBox_CH_STATE->addItem(QObject::tr("全部"), "");
    ui->comboBox_CH_STATE->addItem(QObject::tr("已申请"), "1");
    ui->comboBox_CH_STATE->addItem(QObject::tr("已审核"), "2");

    //tablemodel_master
    tablemodel_master = new lds_model_sqltablemodel(this);
    tablemodel_master->setTable("material_purchase_apply");
    tablemodel_master->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("CH_STATE"), QObject::tr("状态"));
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("BILL_NO"), QObject::tr("单据号码"));
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("APPLY_PEOPLE"), QObject::tr("申请人"));
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("TITLE"), QObject::tr("标题"));
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("REMARK"), QObject::tr("备注"));
    tablemodel_master->enable_vheaderdata_dirty_translate();
    ui->tableView_master->setModel(tablemodel_master);
    ui->tableView_master->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_master->rePecifyHeader(tablemodel_master->get_save_set_header_saver_column_list());
    ui->tableView_master->setItemDelegateForColumn(tablemodel_master->fieldIndex("CH_STATE"), new lds_model_sqltablemodel_delegate_com(this, ui->comboBox_CH_STATE));


    //tablemodel_detail
    tablemodel_detail = new lds_model_sqltablemodel(this);
    tablemodel_detail->setTable("material_purchase_apply_detail");
    tablemodel_detail->save_set_header(tablemodel_detail->fieldIndex("MATERIAL_CODE"), QObject::tr("编号"));
    tablemodel_detail->save_set_header(tablemodel_detail->fieldIndex("MATERIAL_NAME"), QObject::tr("名称"));
    tablemodel_detail->save_set_header(tablemodel_detail->fieldIndex("APPLY_NUM"), QObject::tr("申请数量"));
    tablemodel_detail->save_set_header(tablemodel_detail->fieldIndex("UNIT_CODE"), QObject::tr("入库单位"));
    tablemodel_detail->save_set_header(tablemodel_detail->fieldIndex("UNIT_NAME"), QObject::tr("单位名称"));
    tablemodel_detail->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);

    ui->tableView_detail->setModel(tablemodel_detail);
    ui->tableView_detail->rePecifyHeader(tablemodel_detail->get_save_set_header_saver_column_list());
    ui->tableView_detail->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView_detail->setSelectionBehavior(QTableView::SelectItems);
    ui->tableView_detail->setSelectionMode(QTableView::SingleSelection);

    //
    connect(ui->tableView_master, SIGNAL(selectChanged(int)), this, SLOT(to_update_tableview_detail(int)));
    connect(ui->pushButton_audit, SIGNAL(clicked()), this, SLOT(toaudit()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_del, SIGNAL(clicked()), this, SLOT(todel()));
    connect(ui->pushButton_new, SIGNAL(clicked()), this, SLOT(tonew()));
    connect(ui->pushButton_select, SIGNAL(clicked()), this, SLOT(torefresh()));
    connect(ui->comboBox_CH_STATE, SIGNAL(currentIndexChanged(int)), this, SLOT(torefresh()));
    connect(ui->pushButton_change, SIGNAL(clicked()), this, SLOT(tochange()));
    connect(ui->tableView_master, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(tochange()));


    //date
    QDateTime dt = n_func::f_get_sysdatetime();
    ui->dateTimeEdit_to->setDateTime(QDateTime(dt.date(), QTime(23, 59, 59)));
    ui->dateTimeEdit_from->setDateTime(QDateTime(dt.date(), QTime(0,0,0)));
#ifdef QT_DEBUG
    ui->dateTimeEdit_from->setDate(QDate(2016, 1,1));
#endif

    QTimer::singleShot(0, this, SLOT(torefresh()));
}

material_purchase_apply::~material_purchase_apply()
{
    delete ui;
}

void material_purchase_apply::to_update_tableview_detail(int row)
{
    tablemodel_detail->setFilter(QString("BILL_NO = '%1' ")
                                 .arg(tablemodel_master->model_data(row, "BILL_NO").toString())
                                 );
    ui->tableView_detail->resizeColumnsToContents();
}

void material_purchase_apply::toaudit()
{
    int row = ui->tableView_master->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return ;
    }
    QString CH_STATE = tablemodel_master->model_data(row, "CH_STATE").toString();
    if("0" != CH_STATE) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("当前状态") + ":" +
                                ui->comboBox_CH_STATE->itemText(ui->comboBox_CH_STATE->findData(CH_STATE))+
                                "," + QObject::tr("操作失败"));
        return;
    }
    billnApp(tablemodel_master->model_data(row, "BILL_NO").toString());
    torefresh();
}

void material_purchase_apply::toexit()
{
    this->reject();
}

void material_purchase_apply::todel()
{
    int row = ui->tableView_master->currentIndex().row();
    if(row >= 0) {
        QString BILL_NO = tablemodel_master->model_data(row, "BILL_NO").toString();
        QString CH_STATE = tablemodel_master->model_data(row, "CH_STATE").toString();
        if(CH_STATE == "1") {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已申请")+"," +QObject::tr("操作失败"));
            return;
        }
        if(CH_STATE == "1") {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已审核")+"," +QObject::tr("操作失败"));
            return;
        }
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认删除"), QObject::tr("是"), QObject::tr("否"))) {
            return;
        }
        lds_query_hddpos  query;
        lds_query_hddpos::tran_saction();
        if(query.execDelete("material_purchase_apply", QString("BILL_NO = '%1' ;")
                            .arg(BILL_NO))) {
            if(query.execDelete("material_purchase_apply_detail", QString("BILL_NO = '%1' ;")
                                .arg(BILL_NO))) {
                lds_query_hddpos::com_mit();
                torefresh();
                return;
            }
        }
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    }
}

void material_purchase_apply::tonew()
{
    material_purchase_apply_new dialog("", this);
    dialog.setWindowTitle(QObject::tr("新增"));
    transparentCenterDialog(&dialog).exec();
    torefresh();
}

void material_purchase_apply::torefresh()
{
    //!等待
    lds_messagebox_loading_show loading(this, "", "WAITING...");
    loading.show();
    //!~等待
    //!
    //获取云端状态，更新本地
    lds_query_hddpos  query;
    lds_query_hddpos  query2;
    QString BILL_NO;

    lds_query_hddpos::tran_saction();
    query.execSelect(QString("SELECT BILL_NO from material_purchase_apply where ifnull(BILL_NO, '') <> '' and CH_STATE = '1' "));//已申请
    while(query.next()) {
        BILL_NO = query.recordValue("BILL_NO").toString();
        if(billnoIsAduitedbyYun(BILL_NO)) {//已审核
            query2.execUpdate("material_purchase_apply",
                              "CH_STATE", 2,
                              qrtEqual("BILL_NO", BILL_NO));
        }
    }
    lds_query_hddpos::com_mit();
    //
    tablemodel_master->setFilter(QString(" DATE >= '%1' and DATE <= '%2' and CH_STATE like '%3%' ")
                                 .arg(ui->dateTimeEdit_from->dateTime().toString("yyyy-MM-dd hh:mm:ss"))
                                 .arg(ui->dateTimeEdit_to->dateTime().toString("yyyy-MM-dd hh:mm:ss"))
                                 .arg(ui->comboBox_CH_STATE->curusrdata().toString())
                                 );
    ui->tableView_master->restore();
    if(ui->tableView_master->currentIndex().row() < 0) {
        if(tablemodel_master->rowCount() > 0) {
            ui->tableView_master->selectRow(0);
        } else {
            to_update_tableview_detail(-1);
        }
    }
    ui->tableView_master->resizeColumnsToContents();
}

void material_purchase_apply::tochange()
{
    int row = ui->tableView_master->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    material_purchase_apply_new dialog(tablemodel_master->model_data(row, "BILL_NO").toString(), this);
    dialog.setWindowTitle(QObject::tr("查看"));
    transparentCenterDialog(&dialog).exec();
    torefresh();
}

bool material_purchase_apply::billnoIsAduitedbyYun(const QString &billno)
{
    //
    QMap<QString, QString> headInfo;

//    headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    headInfo.insert("requestType","CY");
    headInfo.insert("billNo",billno);
    QString http_key = http_www_yqspos_com +"/arm/material-purchase-apply/getByBillNo-rest";


    httpDataAutoDel json_yun_readcard =  w_sys_manage_cloudsync::get_yun_httpget(this, http_key, headInfo,QObject::tr("采购申请"), false, true, 10*1000);
    if(json_yun_readcard.json) {
        return "\"2\"" == json_yun_readcard.jsonToString().trimmed();
    }

    return false;
}

void material_purchase_apply::billnApp(const QString &BILL_NO)
{
    httpDataAutoDel root = cJSON_CreateObject();

    cJSON *materialPurchaseApply = cJSON_CreateObject();
    cJSON *materialPurchaseApplyDetail = cJSON_CreateArray();
    cJSON_help::_cJSON_AddItemToObject(root.json, "materialPurchaseApply", materialPurchaseApply);
    cJSON_help::_cJSON_AddItemToObject(root.json, "materialPurchaseApplyDetail", materialPurchaseApplyDetail);
    QString orgCode = n_func::f_get_sysparm("orgCode");
    ////////////////
    lds_query_hddpos  query;
    query.execSelect(QString("SELECT * from material_purchase_apply where BILL_NO ='%1'  ")
                     .arg(BILL_NO));
    query.next();
    cJSON_help::_cJSON_AddValueToObject(materialPurchaseApply, "billNo", query.recordValue("BILL_NO"));
    cJSON_help::_cJSON_AddValueToObject(materialPurchaseApply, "applyPeople", query.recordValue("APPLY_PEOPLE"));
    cJSON_help::_cJSON_AddValueToObject(materialPurchaseApply, "remark", query.recordValue("REMARK"));
    cJSON_help::_cJSON_AddValueToObject(materialPurchaseApply, "title", query.recordValue("TITLE"));
    cJSON_help::_cJSON_AddValueToObject(materialPurchaseApply, "orgCode", orgCode);
    ////////////////
    query.execSelect(QString("SELECT * from material_purchase_apply_detail where BILL_NO ='%1' ")
                     .arg(BILL_NO));
    while(query.next()) {
        cJSON *materialPurchaseApplyDetail0 = cJSON_CreateObject();
        cJSON_AddItemToArray(materialPurchaseApplyDetail, materialPurchaseApplyDetail0);
        cJSON_help::_cJSON_AddValueToObject(materialPurchaseApplyDetail0, "materialCode", query.recordValue("MATERIAL_CODE"));
        cJSON_help::_cJSON_AddValueToObject(materialPurchaseApplyDetail0, "materialName", query.recordValue("MATERIAL_NAME"));
        cJSON_help::_cJSON_AddValueToObject(materialPurchaseApplyDetail0, "format", query.recordValue("FORMAT"));
        cJSON_help::_cJSON_AddValueToObject(materialPurchaseApplyDetail0, "unitCode", query.recordValue("UNIT_CODE"));
        cJSON_help::_cJSON_AddValueToObject(materialPurchaseApplyDetail0, "unitName", query.recordValue("UNIT_NAME"));
        cJSON_help::_cJSON_AddValueToObject(materialPurchaseApplyDetail0, "applyNum", query.recordValue("APPLY_NUM"));
        cJSON_help::_cJSON_AddValueToObject(materialPurchaseApplyDetail0, "orgCode", orgCode);
    }


    QByteArray param = root.jsonToByteArray();
    //
    QMap<QString, QString> headInfo;

//    headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    headInfo.insert("requestType","CY");
    QString http_key = http_www_yqspos_com +"/arm/material-purchase-apply/add-rest";


    httpDataAutoDel json_yun_readcard =  w_sys_manage_cloudsync::get_yun_httppos(this, http_key, param, headInfo,QObject::tr("采购申请"));
    if(200 == json_yun_readcard.httpStatus) {
        if(query.execUpdate("material_purchase_apply",
                            "CH_STATE", "1",
                            "APPLY_TIME", n_func::f_get_sysdatetime(),
                            qrtEqual("BILL_NO", BILL_NO))) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            return;
        }
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
}
