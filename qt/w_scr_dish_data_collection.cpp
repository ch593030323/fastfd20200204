#include "w_scr_dish_data_collection.h"
#include "ui_w_scr_dish_data_collection_dialog.h"
#include "ui_w_bt_dish_unitset_dialog.h"
#include "lds_tableview_delegate_check.h"
#include "backheader.h"
#include "lds_messagebox.h"

#include "lds_query_hddpos.h"
#include <QDebug>
#include <qsqlerror.h>
#include <QDataWidgetMapper>
#include <QSqlRelationalDelegate>
#include "w_scr_dish.h"
#include "n_func.h"
#include "lds_model_sqltablemodel.h"

w_scr_dish_data_collection::w_scr_dish_data_collection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_scr_dish_data_collection_dialog)
{
    ui->setupUi(this);
    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_web_data_collection");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->select();
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    tablemodel->save_set_header("service_ip", QObject::tr("服务器ip"));
    tablemodel->save_set_header("service_port", QObject::tr("服务器port"));
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setColumnWidth(0, 100);

    //2
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tablemodel);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
    mapper->addMapping(ui->lineEdit_cashier,tablemodel->fieldIndex("cashier"));
    mapper->addMapping(ui->lineEdit_ch_uploadflag,tablemodel->fieldIndex("ch_uploadflag"));
    mapper->addMapping(ui->lineEdit_ch_uploadmemo,tablemodel->fieldIndex("ch_uploadmemo"));
    mapper->addMapping(ui->lineEdit_itemcode,tablemodel->fieldIndex("itemcode"));
    mapper->addMapping(ui->lineEdit_mallid,tablemodel->fieldIndex("mallid"));
    mapper->addMapping(ui->lineEdit_service_ip,tablemodel->fieldIndex("service_ip"));
    mapper->addMapping(ui->lineEdit_service_port,tablemodel->fieldIndex("service_port"));
    mapper->addMapping(ui->lineEdit_service_submit_path,tablemodel->fieldIndex("service_submit_path"));
    mapper->addMapping(ui->lineEdit_storecode,tablemodel->fieldIndex("storecode"));
    mapper->addMapping(ui->lineEdit_txdate_yyyymmdd,tablemodel->fieldIndex("txdate_yyyymmdd"));
    mapper->addMapping(ui->lineEdit_txtime_hhmmss,tablemodel->fieldIndex("txtime_hhmmss"));
    mapper->addMapping(ui->lineEdit_username,tablemodel->fieldIndex("username"));

    //3
    QDateTime sqldt = n_func::f_get_sysdatetime();
    connect(ui->tableView,SIGNAL(selectChanged(int)),this,SLOT(mapper_setcurindex(int)));
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_save, SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->pushButton_serach,SIGNAL(clicked()),this,SLOT(toserach()));
    connect(ui->pushButton_repair,SIGNAL(clicked()),this,SLOT(torepair()));
    mapper->toFirst();
    ui->tableView->selectRow(0);
    ui->dateTimeEdit->setDateTime(QDateTime(sqldt.date().addDays(-1),QTime(0,0,0)));
    ui->dateTimeEdit_2->setDateTime(sqldt);
    ui->pushButton_serach->animateClick();
}

void w_scr_dish_data_collection::toexit()
{
    this->reject();
}

void w_scr_dish_data_collection::tosave()
{
    if(tablemodel->trySubmitAll()) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误")+QString("%1").arg(__LINE__));
    }
}

void w_scr_dish_data_collection::toserach()
{
    tablemodel->setFilter(QString("txdate_yyyymmdd >= '%1' and txdate_yyyymmdd <= '%2' ")
                          .arg(ui->dateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss"))
                          .arg(ui->dateTimeEdit_2->dateTime().toString("yyyy-MM-dd hh:mm:ss"))
                          );
}

void w_scr_dish_data_collection::mapper_setcurindex(int index)
{
    mapper->setCurrentIndex(index);
}

void w_scr_dish_data_collection::torepair()
{
    lds_query_hddpos  query0;
    qlonglong id;
    lds_query_hddpos  query;
    query.execSelect("select sum(t.result) from (select ifnull((select sum(b.baseamount)- a.netamount from cey_web_data_collection a JOIN cey_web_data_collection_tendercode b ON a.id = b.id "
                     " where a.txdocno = x.txdocno),0) as result, txdocno  from cey_web_data_collection x GROUP BY txdocno) t where t.result < 0");
    query.next();
    if(query.recordValue(0).toLongLong() == 0) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("没有需要修复的数据"));
        return;
    }
    lds_query_hddpos::tran_saction();
    query.execSelect("select * from "
                     " (select  a.netamount - sum(b.baseamount) as otherpay,  a.*"
                     " from cey_web_data_collection a join cey_web_data_collection_tendercode b on a.id = b.id GROUP BY a.id )t "
                     " where otherpay > 0"
                     );
    while(query.next()) {
        //上传
        query0.execSelect("select count(0) from cey_web_data_collection");
        query0.next();
        id=query0.recordValue(0).toLongLong();
        if(query.execInsert("cey_web_data_collection",qrtVariantPairList()

                            << qrtVariantPair("id", id)
                            << qrtVariantPair("service_ip", query.recordValue("service_ip").toString())
                            << qrtVariantPair("service_port", query.recordValue("service_port").toString())
                            << qrtVariantPair("service_action", query.recordValue("service_action").toString())
                            << qrtVariantPair("service_method_name", query.recordValue("service_method_name").toString())

                            << qrtVariantPair("service_method_uri", query.recordValue("service_method_uri").toString())
                            << qrtVariantPair("service_submit_path", query.recordValue("service_submit_path").toString())
                            << qrtVariantPair("username", query.recordValue("username").toString())
                            << qrtVariantPair("password", query.recordValue("password").toString())
                            << qrtVariantPair("messagetype", query.recordValue("messagetype").toString())

                            << qrtVariantPair("messageid", query.recordValue("messageid").toString())
                            << qrtVariantPair("version", query.recordValue("version").toString())
                            << qrtVariantPair("txdate_yyyymmdd", query.recordValue("txdate_yyyymmdd").toString())
                            << qrtVariantPair("txtime_hhmmss", query.recordValue("txtime_hhmmss").toString())
                            << qrtVariantPair("mallid", query.recordValue("mallid").toString())

                            << qrtVariantPair("storecode", query.recordValue("storecode").toString())
                            << qrtVariantPair("tillid", query.recordValue("tillid").toString())
                            << qrtVariantPair("txdocno", query.recordValue("txdocno").toString())
                            << qrtVariantPair("cashier", query.recordValue("cashier").toString())
                            << qrtVariantPair("salesman", query.recordValue("salesman").toString())

                            << qrtVariantPair("itemcode", query.recordValue("itemcode").toString())
                            << qrtVariantPair("qty", query.recordValue("qty").toString())
                            << qrtVariantPair("netamount", query.recordValue("netamount").toString())
                            << qrtVariantPair("ch_uploadflag", "N")
                            << qrtVariantPair("ch_uploadmemo", "ready")
                            )) {
            if(query0.execInsert("cey_web_data_collection_tendercode",qrtVariantPairList()
                                 << qrtVariantPair("id", id) << qrtVariantPair("tendercode", "OT") << qrtVariantPair("baseamount", query.recordValue(0)))) {
                continue;
            }//! if
        }//! if
        break;
    }//! while

    QString sqlerr = query.recordError().trimmed();
    if(sqlerr.isEmpty()) {
        lds_query_hddpos::com_mit();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        return;
    }
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
}
