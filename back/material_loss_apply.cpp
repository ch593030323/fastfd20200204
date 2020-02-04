#include "material_loss_apply.h"
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
#include "material_purchase_in_storage.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "ui_material_purchase_apply.h"
#include <QTimer>

material_loss_apply::material_loss_apply(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_material_purchase_apply)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->comboBox_CH_STATE->addItem(QObject::tr("未确认"), "0");
    ui->comboBox_CH_STATE->addItem(QObject::tr("全部"), "");
    ui->comboBox_CH_STATE->addItem(QObject::tr("已确认"), "1");
    tablemodel_master = new lds_model_sqltablemodel(this);
    tablemodel_master->tablename_kvmap_insert_value("CH_STATE", "0", QObject::tr("未确认"));
    tablemodel_master->tablename_kvmap_insert_value("CH_STATE", "1", QObject::tr("已确认"));

    //
    tablemodel_master->setTable("material_loss_apply");
    tablemodel_master->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("CH_STATE"), QObject::tr("状态"));
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("BILL_NO"), QObject::tr("单据号码"));
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("PEOPLE"), QObject::tr("调拨人"));
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("ORG_NAME"), QObject::tr("调拨门店"));
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("DATE"), QObject::tr("调拨时间"));
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("TITLE"), QObject::tr("标题"));
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("REMARK"), QObject::tr("备注"));
    ui->tableView_master->setModel(tablemodel_master);
    ui->tableView_master->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_master->rePecifyHeader(tablemodel_master->get_save_set_header_saver_column_list());
    ui->tableView_master->setItemDelegateForColumn(tablemodel_master->fieldIndex("CH_STATE"), new lds_model_sqltablemodel_delegate_com(this, ui->comboBox_CH_STATE));

    tablemodel_detail = new lds_model_sqltablemodel(this);
    tablemodel_detail->setTable("material_loss_apply_detail");
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
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_select, SIGNAL(clicked()), this, SLOT(torefresh()));
    connect(ui->comboBox_CH_STATE, SIGNAL(currentIndexChanged(int)), this, SLOT(torefresh()));
    connect(ui->pushButton_audit, SIGNAL(clicked()),this,SLOT(toaudit()));


    QDateTime dt = n_func::f_get_sysdatetime();
    ui->dateTimeEdit_to->setDateTime(QDateTime(dt.date(), QTime(23, 59, 59)));
    ui->dateTimeEdit_from->setDateTime(QDateTime(dt.date(), QTime(0,0,0)));
#ifdef QT_DEBUG
    ui->dateTimeEdit_from->setDate(QDate(2016, 1,1));
#endif

    ui->pushButton_audit->setText(QObject::tr("单据确认"));
    ui->pushButton_change->hide();
    ui->pushButton_del->hide();
    ui->pushButton_new->hide();
    QTimer::singleShot(0, this, SLOT(torefresh()));
}

material_loss_apply::~material_loss_apply()
{
    delete ui;
}

void material_loss_apply::to_update_tableview_detail(int row)
{
    tablemodel_detail->setFilter(QString("BILL_NO = '%1' ")
                                 .arg(tablemodel_master->model_data(row, "BILL_NO").toString())
                                 );
    ui->tableView_detail->resizeColumnsToContents();
}

void material_loss_apply::toexit()
{
    this->reject();
}

void material_loss_apply::torefresh()
{
    getDataFromYun();
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

void material_loss_apply::toaudit()
{
    int row = ui->tableView_master->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }

    QString BILL_NO = tablemodel_master->model_data(row, "BILL_NO").toString();
    lds_query_hddpos::tran_saction();
    if(material_purchase_in_storage::local_inventory_in_storage(this, BILL_NO,
                                                                "material_loss_apply",
                                                                "material_loss_apply_detail",
                                                                "TH")) {//采购入库
        if(billnoIsAduitedbyYun(BILL_NO)) {
            lds_query_hddpos  query;
            if(query.execUpdate("material_loss_apply","CH_STATE", "1", qrtEqual("BILL_NO", BILL_NO))) {
                lds_query_hddpos::com_mit();
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
                torefresh();
                w_sys_manage_cloudsync_cloud2::exec_keepdata_byconf(this);
                return;
            }
        }
    }
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    torefresh();
}

void material_loss_apply::getDataFromYun()
{
    //!等待
    lds_messagebox_loading_show loading(this, "", "WAITING...");
    loading.show();
    //!~等待

    QMap<QString, QString> headInfo;

//    headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    headInfo.insert("requestType","CY");
    QString http_key = http_www_yqspos_com +"/arm/material-store-out/getStoreOut-rest";

    httpDataAutoDel json_yun_readcard =  w_sys_manage_cloudsync::get_yun_httpget(this, http_key, headInfo,QObject::tr("商品出库"), false, true, 10*1000 );

    lds_query_hddpos  query;
    lds_query_hddpos::tran_saction();
    if(200 == json_yun_readcard.httpStatus) {
        int gcount;
        if((gcount=cJSON_help::array_count(json_yun_readcard.json)) > 0) {
            for(int i =0;i<gcount;i++) {
                cJSON *pItem = cJSON_help::_cJSON_GetArrayItem(json_yun_readcard.json,i);
                cJSON *p0 = cJSON_GetObjectItem(pItem, "materialStoreOutApply");
                cJSON *p1 = cJSON_GetObjectItem(pItem, "materialStoreOutDetail");
                QString billNo;

                billNo = cJSON_help::getcJSONvalue(p0, "billNo").toString();
                query.execSelect(QString("SELECT BILL_NO from material_loss_apply where BILL_NO = '%1' ").arg(billNo));
                if(query.next()) continue;

                if(false == query.execInsert("material_loss_apply",qrtVariantPairList()

                                    << qrtVariantPair("BILL_NO", billNo)
                                    << qrtVariantPair("ORG_CODE", cJSON_help::getcJSONvalue(p0, "distributionOrgCode"))
                                    << qrtVariantPair("ORG_NAME", cJSON_help::getcJSONvalue(p0, "distributionOrgName"))
                                    << qrtVariantPair("PEOPLE", cJSON_help::getcJSONvalue(p0, "distributionPeople"))
                                    << qrtVariantPair("REMARK", cJSON_help::getcJSONvalue(p0, "remark"))

                                    << qrtVariantPair("CH_STATE", "0")
                                    << qrtVariantPair("DATE", cJSON_help::getcJSONvalue(p0, "date"))
                                    << qrtVariantPair("TITLE", cJSON_help::getcJSONvalue(p0, "title"))

                                    )) {
                    goto rollback;
                }

                int gcount0;
                if((gcount0 = cJSON_GetArraySize(p1)) > 0) {
                    for(int i = 0;i<gcount0;i++) {
                        pItem = cJSON_GetArrayItem(p1,i);
                        if(false == query.execInsert("material_loss_apply_detail",qrtVariantPairList()

                                         << qrtVariantPair("BILL_NO", billNo)
                                         << qrtVariantPair("MATERIAL_CODE", cJSON_help::getcJSONvalue(pItem, "materialCode"))
                                         << qrtVariantPair("MATERIAL_NAME", cJSON_help::getcJSONvalue(pItem, "materialName"))
                                         << qrtVariantPair("FORMAT", cJSON_help::getcJSONvalue(pItem, "format"))
                                         << qrtVariantPair("UNIT_CODE", cJSON_help::getcJSONvalue(pItem, "unitCode"))

                                         << qrtVariantPair("UNIT_NAME", cJSON_help::getcJSONvalue(pItem, "unitName"))
                                         << qrtVariantPair("APPLY_NUM", cJSON_help::getcJSONvalue(pItem, "num"))
                                         << qrtVariantPair("APPLY_PRICE", 0)
                                         << qrtVariantPair("DATE", cJSON_help::getcJSONvalue(pItem, "date"))

                                         )) {
                            goto rollback;
                        }
                    }
                }
            }
        }
    }

    lds_query_hddpos::com_mit();
    return;
rollback:
    lds_query_hddpos::roll_back();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    return;
}

bool material_loss_apply::billnoIsAduitedbyYun(const QString &billno)
{
    //
    QMap<QString, QString> headInfo;

//    headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    headInfo.insert("requestType","CY");
    headInfo.insert("billNo",billno);
    QString http_key = http_www_yqspos_com +"/arm/material-store-out/submitByBillNo-rest";


    httpDataAutoDel json_yun_readcard =   w_sys_manage_cloudsync::get_yun_httpget(this, http_key, headInfo,QObject::tr("采购申请"), false);

    if(200 == json_yun_readcard.httpStatus) {
        return true;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, json_yun_readcard.message + "," + QObject::tr("操作失败"));
    return false;
}

