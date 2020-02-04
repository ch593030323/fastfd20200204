#include "material_purchase_apply_new.h"
#include "ui_material_purchase_apply_new.h"
#include "backheader.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "w_st_good_input_select_dish.h"
#include <QSqlError>
#include <QTimer>
#include "n_func.h"
#include "w_i_inventory.h"
#include <QSqlRelationalDelegate>
#include "w_inventory_input.h"

material_purchase_apply_new::material_purchase_apply_new(const QString &BILL_NO, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::material_purchase_apply_new)
{
    ui->setupUi(this);
    ui->BILL_NO->setText(BILL_NO);
    ui->BILL_NO->setEnabled(false);
    ui->APPLY_PEOPLE->addItem("", "");
    ui->APPLY_PEOPLE->addItemsBySql(backheader::SELECT_OPERID_NAME_FLAG);
    //tablemodel_detail
    tablemodel_detail = new lds_model_sqltablemodel(this);
    tablemodel_detail->setTable("material_purchase_apply_detail");
    tablemodel_detail->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel_detail->save_set_header(tablemodel_detail->fieldIndex("MATERIAL_CODE"), QObject::tr("编号"));
    tablemodel_detail->save_set_header(tablemodel_detail->fieldIndex("MATERIAL_NAME"), QObject::tr("名称"));
    tablemodel_detail->save_set_header(tablemodel_detail->fieldIndex("APPLY_NUM"), QObject::tr("申请数量"), true);
    tablemodel_detail->save_set_header(tablemodel_detail->fieldIndex("UNIT_CODE"), QObject::tr("入库单位"));
    tablemodel_detail->save_set_header(tablemodel_detail->fieldIndex("UNIT_NAME"), QObject::tr("单位名称"));
    tablemodel_detail->enable_vheaderdata_dirty_translate();

    ui->tableView_detail->setModel(tablemodel_detail);
    ui->tableView_detail->rePecifyHeader(tablemodel_detail->get_save_set_header_saver_column_list());
    ui->tableView_detail->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView_detail->setSelectionBehavior(QTableView::SelectItems);
    ui->tableView_detail->setSelectionMode(QTableView::SingleSelection);

    //connect
    connect(ui->pushButton_new, SIGNAL(clicked()), this, SLOT(tonew()));
    connect(ui->pushButton_del, SIGNAL(clicked()), this, SLOT(todel()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_save, SIGNAL(clicked()), this, SLOT(tosave()));

    QTimer::singleShot(0, this, SLOT(torefresh()));
}

material_purchase_apply_new::~material_purchase_apply_new()
{
    delete ui;
}

void material_purchase_apply_new::tonew()
{
    w_st_good_input_select_dish dialog(this);
    dialog.setWindowTitle(QObject::tr("库存管理"));
    dialog.filter_by_itemflag();
    if(QDialog::Accepted ==transparentCenterDialog(&dialog).exec()) {
        foreach(int k, dialog.ret_row_list) {
            QString vch_pur_unitno = dialog.get_tablemodel_value(k, "vch_pur_unitno").toString();
            int rowcount = tablemodel_detail->rowCount();
            tablemodel_detail->insertRow(rowcount);
            tablemodel_detail->model_data_set(rowcount, "MATERIAL_CODE", dialog.get_tablemodel_value(k, "ch_dishno"));
            tablemodel_detail->model_data_set(rowcount, "MATERIAL_NAME", dialog.get_tablemodel_value(k, "vch_dishname"));
            tablemodel_detail->model_data_set(rowcount, "UNIT_CODE", vch_pur_unitno);
            tablemodel_detail->model_data_set(rowcount, "UNIT_NAME", tablemodel_detail->tablename_kvmap_value("cey_bt_unit", vch_pur_unitno, "ch_unitno", "vch_unitname").toString());
            tablemodel_detail->model_data_set(rowcount, "APPLY_NUM", 1);
            tablemodel_detail->model_data_set(rowcount, "DATE", n_func::f_get_sysdatetime_str());
            tablemodel_detail->model_data_set(rowcount, "BILL_NO", ui->BILL_NO->text());
        }
    }

    ui->tableView_detail->resizeColumnsToContents();
}

void material_purchase_apply_new::todel()
{
    int row = ui->tableView_detail->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }

    tablemodel_detail->removeRow(row);
}

void material_purchase_apply_new::toexit()
{
    if(isDirty()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"),QObject::tr("取消"))) {
        case 0:
            if(!saveData()) {
                return;
            }
            this->accept();
            break;
        case 1:
            break;
        case 2:
            return;
        }
    }
    this->reject();
}

void material_purchase_apply_new::torefresh()
{
    tablemodel_detail->setFilter(QString("BILL_NO = '%1' ")
                                 .arg(ui->BILL_NO->text())
                                 );

    lds_query_hddpos  query;
    query.execSelect(QString("SELECT CH_STATE,  APPLY_PEOPLE, TITLE, REMARK from  material_purchase_apply  where BILL_NO = '%1' ")
               .arg(ui->BILL_NO->text()));
    if(query.next()) {
        QString CH_STATE = query.recordValue("CH_STATE").toString();
        ui->pushButton_del->setEnabled(CH_STATE == "0");
        ui->pushButton_new->setEnabled(CH_STATE == "0");
        ui->pushButton_save->setEnabled(CH_STATE == "0");
        ui->tableView_detail->setEnabled(CH_STATE == "0");
        ui->frame->setEnabled(CH_STATE == "0");

        ui->APPLY_PEOPLE->setcomusrdata(query.recordValue("APPLY_PEOPLE"));
        ui->TITLE->setText(query.recordValue("TITLE").toString());
        ui->REMARK->setText(query.recordValue("REMARK").toString());
    }
}

void material_purchase_apply_new::tosave()
{
    if(saveData()) {
        torefresh();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        return;
    }
}

bool material_purchase_apply_new::saveData()
{
    //判断是否条件
    if(tablemodel_detail->rowCount() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("表单内容为空"));
        return false;
    }
    if(ui->APPLY_PEOPLE->curusrdata().toString().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("申请人不能为空"));
        return false;
    }
    if(ui->TITLE->text().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("标题不能为空"));
        return false;
    }
    for(int row = 0; row < tablemodel_detail->rowCount(); row++) {
        if(tablemodel_detail->opera(row) == lds_model_sqltablemodel::OpDelete) {//已删除
            continue;
        }
        if(tablemodel_detail->model_data(row, "MATERIAL_CODE").toString().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("第%1行，表单编号不能为空").arg(row + 1));
            return false;
        }
        if(tablemodel_detail->model_data(row, "APPLY_NUM").toDouble() <= 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("第%1行，表单申请数量必须大于0").arg(row + 1));
            return false;
        }
    }
    //BILL_NO
    lds_query_hddpos  query;
    QString BILL_NO = ui->BILL_NO->text();
    if(BILL_NO.isEmpty()) {
        QString filter = "M"+n_func::f_get_sysdatetime().toString("yyMMdd");
        query.execSelect(QString("select max(BILL_NO) from material_purchase_apply where BILL_NO like '%1%' ")
                   .arg(filter));
        query.next();
        BILL_NO = query.recordValue(0).toString();
        BILL_NO = filter + QString().sprintf("%06d", BILL_NO.mid(7).toInt() + 1);
        for(int drow = 0; drow < tablemodel_detail->rowCount(); drow ++) {
            tablemodel_detail->model_data_set(drow, "BILL_NO", BILL_NO);
        }
    }
    //

    lds_query_hddpos::tran_saction();
    query.execSelect(QString("select BILL_NO from material_purchase_apply where BILL_NO like '%1' ")
               .arg(BILL_NO));
    if(query.next()) {
        if(false == query.execUpdate("material_purchase_apply",qrtVariantPairList()

                                     << qrtVariantPair("APPLY_PEOPLE", ui->APPLY_PEOPLE->curusrdata().toString())
                                     << qrtVariantPair("CH_STATE", "0")
                                     << qrtVariantPair("REMARK", ui->REMARK->text())
                                     << qrtVariantPair("DATE", n_func::f_get_sysdatetime())
                                     << qrtVariantPair("TITLE", ui->TITLE->text()),
                                     qrtEqual("BILL_NO", BILL_NO))) {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
            return false;
        }
    } else {
        if(false == query.execInsert("material_purchase_apply",qrtVariantPairList()

                                     << qrtVariantPair("BILL_NO", BILL_NO)
                                     << qrtVariantPair("APPLY_PEOPLE", ui->APPLY_PEOPLE->curusrdata().toString())
                                     << qrtVariantPair("CH_STATE", "0")
                                     << qrtVariantPair("REMARK", ui->REMARK->text())
                                     << qrtVariantPair("DATE", n_func::f_get_sysdatetime())

                                     << qrtVariantPair("TITLE", ui->TITLE->text()))) {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
            return false;
        }
    }

    if(false ==tablemodel_detail->trySubmitAll()) {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
        return false;
    }

    lds_query_hddpos::com_mit();
    ui->BILL_NO->setText(BILL_NO);
    torefresh();
    return true;
}

bool material_purchase_apply_new::isDirty()
{
    lds_query_hddpos  query;
    query.execSelect(QString("select * from material_purchase_apply where BILL_NO = '%1' ")
               .arg(ui->BILL_NO->text()));
    if(query.next()) {
        if(query.recordValue("APPLY_PEOPLE") != ui->APPLY_PEOPLE->curusrdata()) {
            return true;
        }
        if(query.recordValue("TITLE").toString() != ui->TITLE->text()) {
            return true;
        }
        if(query.recordValue("REMARK").toString() != ui->REMARK->text()) {
            return true;
        }
    }
    return tablemodel_detail->model_is_dirty();
}
