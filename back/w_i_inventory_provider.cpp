#include "w_i_inventory_provider.h"
#include "ui_w_i_inventory_provider.h"
#include "lds_model_sqltablemodel.h"
#include "w_i_inventory_provider_new.h"
#include "transparentcenterdialog.h"
#include "lds_messagebox.h"
#include <QTimer>
#include "lds_query_hddpos.h"
#include "w_i_inventory_provider_arrears.h"
#include "lds_model_sqltablemodel_delegate_com.h"

w_i_inventory_provider::w_i_inventory_provider(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_i_inventory_provider)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("客户"));
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_st_provider");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->save_set_header("ch_providerNo",QObject::tr("编号"));
    tablemodel->save_set_header("vch_providerName",QObject::tr("名称"));
    tablemodel->save_set_header("vch_tax_number",QObject::tr("税号"));
    tablemodel->save_set_header("int_tax",QObject::tr("税率") + "(%)");
    tablemodel->save_set_header("ch_typeno",QObject::tr("类型"));
    tablemodel->save_set_header("num_remain",QObject::tr("欠款"));
    tablemodel->enable_vheaderdata_dirty_translate();
    tablemodel->tablename_kvmap_insert_value("ch_typeno", "NORMAL", tr("普通"));
    tablemodel->tablename_kvmap_insert_value("ch_typeno", "UNION", tr("联营"));
//    tablemodel->tablename_kvmap_insert_value("ch_typeno", "COMMON", tr("一般"));
//    tablemodel->tablename_kvmap_insert_value("ch_typeno", "CREDIT", tr("信用"));
//    tablemodel->tablename_kvmap_insert_value("ch_typeno", "SPECIAL", tr("特殊"));

    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_typeno"), new lds_model_sqltablemodel_delegate_com(this, tablemodel->tablename_kvmap_d("ch_typeno")));

    connect(ui->pushButton_add, SIGNAL(clicked()), this, SLOT(toadd()));
    connect(ui->pushButton_modify, SIGNAL(clicked()), this, SLOT(tomodify()));
    connect(ui->pushButton_del, SIGNAL(clicked()), this, SLOT(todel()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_arrears, SIGNAL(clicked()), this, SLOT(toarrears()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));

    lds::hideWidget(ui->pushButton_ok);
    QTimer::singleShot(100, this, SLOT(toselect()));
}

w_i_inventory_provider::~w_i_inventory_provider()
{
    delete ui;
}

QVariant w_i_inventory_provider::curData(const QString &field_name)
{
    int row = ui->tableView->currentIndex().row();
    return tablemodel->model_data(row, field_name);
}

void w_i_inventory_provider::toadd()
{
    w_i_inventory_provider_new dialog(tablemodel, this);
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        toselect();
    }
}

void w_i_inventory_provider::tomodify()
{
    int row=ui->tableView->currentIndex().row();
    if(row<0) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }

    w_i_inventory_provider_modify dialog(tablemodel, row, this);
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        toselect();
    }
}

void w_i_inventory_provider::todel()
{
    int row=ui->tableView->currentIndex().row();
    if(row<0) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    QString ch_providerNo = tablemodel->model_data(row, "ch_providerNo").toString();

    if(lds_query_hddpos::selectHasNext(QString(" select ch_providerNo from cey_st_sheet_master where ch_providerNo = '%1' ").arg(ch_providerNo))) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("已有库存相关记录"));
        return;
    }

    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("确认删除"), tr("确认"), tr("取消"))) {
        return;
    }
    lds_query_hddpos query;
    lds_query_hddpos::tran_saction();
    if(false == query.execDelete("cey_st_provider", QString(" ch_providerNo = '%1' ").arg(ch_providerNo))) {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
        return;
    }
    lds_query_hddpos::com_mit();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
    toselect();
}

void w_i_inventory_provider::toexit()
{
    this->reject();
}

void w_i_inventory_provider::toarrears()
{
    int row=ui->tableView->currentIndex().row();
    if(row<0) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    QString ch_providerNo = tablemodel->model_data(row, "ch_providerNo").toString();


    w_i_inventory_provider_arrears dialog(ch_providerNo, this);
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
    }
    toselect();
}

void w_i_inventory_provider::toselect()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0)
        row = 0;
    QString filter;
    const lds_model_sqltablemodel_d *d = tablemodel->tablename_kvmap_d("ch_typeno");
    for(int k = 0; k < d->rowCount(); k ++) {
        filter += QString(" '%1' ,").arg(d->key(k));
    }
    filter.chop(1);
    tablemodel->setFilter("ch_typeno in (" + filter + ") ");

    ui->tableView->selectRow(row);
    if(ui->tableView->currentIndex().row() >= 0)
        ui->tableView->resizeColumnsToContents();
}

void w_i_inventory_provider::took()
{
    int row = ui->tableView->currentIndex().row();
    if(row == -1) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    this->accept();
}


w_i_inventory_provider_costumer::w_i_inventory_provider_costumer(QWidget *parent)
    : w_i_inventory_provider(parent)
{
    tablemodel->save_set_header_clear();

    tablemodel->tablename_kvmap_delete("ch_typeno");
    tablemodel->save_set_header("ch_providerNo",QObject::tr("编号"));
    tablemodel->save_set_header("vch_providerName",QObject::tr("名称"));
    tablemodel->save_set_header("ch_typeno",QObject::tr("类型"));
    tablemodel->save_set_header("vch_tel",QObject::tr("电话"));
    tablemodel->save_set_header("vch_memo",QObject::tr("备注"));
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());

    //
//    tablemodel->tablename_kvmap_insert_value("ch_typeno", "NORMAL", tr("普通"));
//    tablemodel->tablename_kvmap_insert_value("ch_typeno", "UNION", tr("联营"));
    tablemodel->tablename_kvmap_insert_value("ch_typeno", "COMMON", tr("一般"));
    tablemodel->tablename_kvmap_insert_value("ch_typeno", "CREDIT", tr("信用"));
    tablemodel->tablename_kvmap_insert_value("ch_typeno", "SPECIAL", tr("特殊"));

    lds::hideWidget(ui->pushButton_arrears, true);
}


w_i_inventory_provider_costumer_select::w_i_inventory_provider_costumer_select(QWidget *parent)
    : w_i_inventory_provider_costumer(parent)
{
    lds::showWidget(ui->pushButton_ok);
}
