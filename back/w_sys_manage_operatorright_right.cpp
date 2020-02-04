#include "w_sys_manage_operatorright_right.h"
#include "public_select_stringlist.h"
#include "lds_query_hddpos.h"
#include "transparentcenterdialog.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "lds_messagebox.h"
#include "lds_tableview_delegate_check.h"
#include "lds.h"
#include "ui_w_sys_manage_operatorright_right.h"

w_sys_manage_operatorright_right::w_sys_manage_operatorright_right(const ldsStringMap &map, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_operatorright_right)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("操作员权限"));
    if(map.d.count() == 0) {
        table_map.insert("table", "cey_bt_table_area");
        table_map.insert("table_no", "ch_areano");
        table_map.insert("table_name", "vch_areaname");
        table_map.insert("table_oper", "cey_bt_table_area_oper");
    } else {
        table_map = map.d;
    }
    QString select_table = " select " +  table_map["table_no"] + ", " + table_map["table_name"] +" from  " + table_map["table"];

    model_area = new lds_model_sqlstandardmodel(this);
    model_area->setQuery(select_table);
    model_area->execQuery();
    ui->tableView_area->setModel(model_area);
    ui->tableView_area->setTransferHheader();

    model_oper = new lds_model_sqlstandardmodel(this);
    model_oper->setQuery(" select vch_operid, vch_operator, 'N' as ch_state from cey_sys_operator ");
    model_oper->execQuery();
    model_oper->save_set_header("ch_state", true);
    ui->tableView_oper->setModel(model_oper);
    ui->tableView_oper->setTransferHheader();
    ui->tableView_oper->setItemDelegateForColumn(model_oper->fieldIndex("ch_state"),
                                                 new lds_tableview_delegate_check(model_oper->fieldIndex("ch_state"), ui->tableView_oper, true));
    //
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->tableView_area, SIGNAL(clicked(QModelIndex)), this, SLOT(toupdateOper(QModelIndex)));
    connect(ui->tableView_oper, SIGNAL(clicked(QModelIndex)), this, SLOT(tooper(QModelIndex)));
    connect(ui->pushButton_clearAll, SIGNAL(clicked()), this, SLOT(toclearAll()));
    connect(ui->pushButton_selectAll, SIGNAL(clicked()), this, SLOT(toselectAll()));

    //
    if(model_area->rowCount() > 0) {
        ui->tableView_area->selectRow(0);
        toupdateOper(model_area->index(0, 0));
    }
}

w_sys_manage_operatorright_right::~w_sys_manage_operatorright_right()
{
    delete ui;
}

void w_sys_manage_operatorright_right::torefresh()
{
    model_area->execQuery();
    model_oper->execQuery();
}

void w_sys_manage_operatorright_right::toexit()
{
    this->reject();
}

void w_sys_manage_operatorright_right::toupdateOper(const QModelIndex &index)
{

    QString select_state = " select ch_state from " + table_map["table_oper"] +" where vch_operid = '%1' and " +  table_map["table_no"] + " = '%2' ";
    QString ch_areano = model_area->model_data(index.row(), table_map["table_no"]).toString();
    for(int k = 0; k < model_oper->rowCount(); k ++) {
        QString vch_operid = model_oper->model_data(k, "vch_operid").toString();
        model_oper->model_data_set(k, "ch_state",
                                   lds_query_hddpos::selectValue(select_state.arg(vch_operid).arg(ch_areano)));
    }
}

void w_sys_manage_operatorright_right::tooper(const QModelIndex &index)
{
    if(index.column() != model_oper->fieldIndex("ch_state")) {
        return;
    }
    int area_row = ui->tableView_area->currentIndex().row();
    QString ch_areano = model_area->model_data(area_row, table_map["table_no"]).toString();
    updateOper(ch_areano, index.row());
}

void w_sys_manage_operatorright_right::toselectAll()
{
    int area_row = ui->tableView_area->currentIndex().row();
    QString ch_areano = model_area->model_data(area_row, table_map["table_no"]).toString();

    lds_query_hddpos::tran_saction();
    for(int k = 0; k < model_oper->rowCount(); k ++) {
        model_oper->model_data_set(k, "ch_state", "Y");
        updateOper(ch_areano, k);

    }
    lds_query_hddpos::com_mit();
}

void w_sys_manage_operatorright_right::toclearAll()
{
    int area_row = ui->tableView_area->currentIndex().row();
    QString ch_areano = model_area->model_data(area_row, table_map["table_no"]).toString();

    lds_query_hddpos::tran_saction();
    for(int k = 0; k < model_oper->rowCount(); k ++) {
        model_oper->model_data_set(k, "ch_state", "N");
        updateOper(ch_areano, k);

    }
    lds_query_hddpos::com_mit();
}

void w_sys_manage_operatorright_right::updateOper(const QString &ch_areano, int oper_row)
{
    QString ch_state = model_oper->model_data(oper_row, "ch_state").toString();
    QString vch_operid = model_oper->model_data(oper_row, "vch_operid").toString();
    lds_query_hddpos query;
    query.execInsertDuplicateUpdate(table_map["table_oper"], QStringList() << "vch_operid" << table_map["table_no"], qrtVariantPairList()
            << qrtVariantPair(table_map["table_no"], ch_areano)
            << qrtVariantPair("vch_operid", vch_operid)
            << qrtVariantPair("ch_state", ch_state)
               );
}
