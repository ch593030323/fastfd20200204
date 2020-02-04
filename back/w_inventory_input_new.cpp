#include "w_inventory_input_new.h"
#include "ui_w_inventory_input_new.h"
#include "backheader.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "w_st_good_input_select_dish.h"
#include <QSqlError>
#include <QTimer>
#include "n_func.h"
#include "w_i_inventory.h"
#include <QSqlRelationalDelegate>
#include "w_inventory_input.h"

w_inventory_input_new::w_inventory_input_new(const QString &ch_sheetType, const QString &ch_sheetno, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_inventory_input_new)
{
    ui->setupUi(this);
    this->ch_sheetType = ch_sheetType;

    ui->comboBox_vch_audit->addItemsBySql(backheader::SELECT_OPERID_NAME_FLAG);
    ui->comboBox_vch_handle->addItemsBySql(backheader::SELECT_OPERID_NAME_FLAG);
    ui->comboBox_ch_operation->addItemsByMap(w_i_inventory::get_MODEL_INVENTORY_TYPE());
    ui->comboBox_ch_operation->setcomusrdata(w_i_inventory::get_MODEL_INVENTORY_SHEET_TYPE().value(ch_sheetType));
    ui->comboBox_ch_operation->setEnabled(false);
    ui->lineEdit_ch_sheetNo->setEnabled(false);
    ui->lineEdit_ch_sheetNo->setText(ch_sheetno);
    ui->comboBox_ch_providerNo->addItemsBySql(" select ch_providerNo, vch_providerName, int_tax from cey_st_provider");
    //tablemodel_detail
    tablemodel_detail = new lds_model_sqltablemodel(this);
    tablemodel_detail->setTable("cey_st_sheet_detail");
    tablemodel_detail->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    
    tablemodel_detail->enable_vheaderdata_dirty_translate();
    tablemodel_detail->save_set_header("int_id", QObject::tr("序号"));
    tablemodel_detail->save_set_header("ch_materialno", QObject::tr("名称"));
    tablemodel_detail->save_set_header("num_wrapping", QObject::tr("小包装"), true);
    tablemodel_detail->save_set_header("num_num", QObject::tr("大包装"), true);
    tablemodel_detail->save_set_header("num_present", QObject::tr("赠送数量"), true);
    tablemodel_detail->save_set_header("num_price", QObject::tr("商品进价"), true);
    tablemodel_detail->save_set_header("cost_price", QObject::tr("金额"));
    tablemodel_detail->save_set_header("vch_memo", QObject::tr("备注"), true);
    ui->tableView_detail->setModel(tablemodel_detail);
    ui->tableView_detail->rePecifyHeader(tablemodel_detail->get_save_set_header_saver_column_list());
    ui->tableView_detail->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView_detail->setSelectionBehavior(QTableView::SelectItems);
    ui->tableView_detail->setSelectionMode(QTableView::SingleSelection);

    //tablemodel_master
    tablemodel_master = new lds_model_sqltablemodel(this);
    tablemodel_master->setTable("cey_st_sheet_master");
    tablemodel_master->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tablemodel_master);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
    mapper->addMapping(ui->comboBox_vch_audit, tablemodel_master->fieldIndex("vch_audit"),  "comusrdata");
    mapper->addMapping(ui->comboBox_vch_handle, tablemodel_master->fieldIndex("vch_handle"),  "comusrdata");
    mapper->addMapping(ui->lineEdit_vch_memo, tablemodel_master->fieldIndex("vch_memo"));
    mapper->addMapping(ui->comboBox_ch_providerNo, tablemodel_master->fieldIndex("ch_providerNo"));
    mapper->addMapping(ui->doubleSpinBox_int_tax, tablemodel_master->fieldIndex("int_tax"));

    //delegate
    lds_tableview_delegate_virtual *d = 0;
    ui->tableView_detail->setItemDelegateForColumn(tablemodel_detail->fieldIndex("ch_materialno"), new lds_model_sqltablemodel_delegate_com(this, "cey_bt_dish", "ch_dishno", "vch_dishname"));
    ui->tableView_detail->setItemDelegate(d = new lds_tableview_delegate_virtual(this));
    connect(d, SIGNAL(signal_edit_commit(QModelIndex)),this,SLOT(to_detail_datachange(QModelIndex)));
    connect(ui->pushButton_new, SIGNAL(clicked()), this,SLOT(tonew()));
    connect(ui->pushButton_del, SIGNAL(clicked()), this,SLOT(todel()));
    connect(ui->pushButton_save, SIGNAL(clicked()), this, SLOT(tosave()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_audit, SIGNAL(clicked()), this, SLOT(toaudit()));
    connect(ui->pushButton_bad, SIGNAL(clicked()), this, SLOT(tobad()));
    connect(ui->comboBox_ch_providerNo, SIGNAL(currentIndexChanged(int)), this, SLOT(toUpdateTax(int)));

    toUpdateTax(0);
    //setFilter
    QTimer::singleShot(0, this, SLOT(torefresh()));
}

w_inventory_input_new::~w_inventory_input_new()
{
    delete ui;
}

void w_inventory_input_new::to_detail_datachange(const QModelIndex &index)
{
    int row = index.row();

    QString ch_materialno = tablemodel_detail->model_data(row, "ch_materialno").toString();
    QVariant int_unit_rate = tablemodel_detail->tablename_kvmap_value("cey_bt_dish", ch_materialno, "ch_dishno", "int_unit_rate");

    if(tablemodel_detail->fieldIndex("num_num") == index.column()) {
        tablemodel_detail->model_data_set(row, "num_wrapping", tablemodel_detail->model_data(row, "num_num").toDouble() * int_unit_rate.toDouble());
    }

    if(tablemodel_detail->fieldIndex("num_wrapping") == index.column()) {
        if(int_unit_rate.toDouble() != 0)
            tablemodel_detail->model_data_set(row, "num_num", tablemodel_detail->model_data(row, "num_wrapping").toDouble() / int_unit_rate.toDouble());
    }
    if(tablemodel_detail->fieldIndex("num_wrapping")  == index.column()
            || tablemodel_detail->fieldIndex("num_num") == index.column()
            || tablemodel_detail->fieldIndex("num_price") == index.column()) {
        tablemodel_detail->model_data_set(row, "cost_price", tablemodel_detail->model_data_NIBOLAN(row, "num_wrapping*num_price"));
        ui->tableView_detail->updateSum(tablemodel_detail->fieldIndex("cost_price"));
        ui->tableView_detail->resizeColumnsToContents();
    }
}

void w_inventory_input_new::tonew()
{
    w_st_good_input_select_dish dialog(this);
    dialog.setWindowTitle(QObject::tr("库存管理"));
    dialog.filter_by_itemflag();
    if(QDialog::Accepted ==transparentCenterDialog(&dialog).exec()) {
        int int_id = 0;
        if(tablemodel_detail->rowCount() > 0) {
            int_id = tablemodel_detail->model_data(tablemodel_detail->rowCount() - 1, "int_id").toInt();
        }
        foreach(int k, dialog.ret_row_list) {
            int_id ++;
            int rowcount = tablemodel_detail->rowCount();
            tablemodel_detail->insertRow(rowcount);
            tablemodel_detail->model_data_set(rowcount, "ch_sheetno", ui->lineEdit_ch_sheetNo->text());
            tablemodel_detail->model_data_set(rowcount, "int_id", int_id);
            tablemodel_detail->model_data_set(rowcount, "ch_materialno", dialog.get_tablemodel_value(k, "ch_dishno"));
            tablemodel_detail->model_data_set(rowcount, "num_price", dialog.get_tablemodel_value(k, "num_sale_price"));
            tablemodel_detail->model_data_set(rowcount, "num_num", 1);
            to_detail_datachange(tablemodel_detail->model_index(rowcount, "num_num"));
        }
    }

    ui->tableView_detail->resizeColumnsToContents();
}

void w_inventory_input_new::todel()
{
    int row = ui->tableView_detail->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }

    tablemodel_detail->removeRow(row);
}

void w_inventory_input_new::tosave()
{
    if(saveData()) {
        torefresh();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
    }
}

void w_inventory_input_new::toexit()
{
    if(tablemodel_master->model_is_dirty()
            || tablemodel_detail->model_is_dirty()) {
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

void w_inventory_input_new::torefresh()
{
    tablemodel_detail->setFilter(QString("ch_sheetno = '%1' ")
                                 .arg(ui->lineEdit_ch_sheetNo->text()));
    ui->tableView_detail->resizeColumnsToContents();
    ui->tableView_detail->restore();

    if(ui->lineEdit_ch_sheetNo->text().length() > 0) {
        tablemodel_master->setFilter(QString(" ch_sheetNo = '%1' ").arg(ui->lineEdit_ch_sheetNo->text()));
        mapper->setCurrentIndex(0);
    }
    //ch_state
    if(ui->lineEdit_ch_sheetNo->text().length() > 0) {
        lds_query_hddpos  query;
        query.execSelect(QString("SELECT ch_state from cey_st_sheet_master where ch_sheetno = '%1'; ").arg(ui->lineEdit_ch_sheetNo->text()));
        query.next();
        QString ch_state = query.recordValue("ch_state").toString();
        ui->pushButton_new->setEnabled(ch_state == "N");
        ui->pushButton_del->setEnabled(ch_state == "N");

        ui->pushButton_audit->setEnabled(ch_state == "N");
        ui->pushButton_bad->setEnabled(ch_state == "N");
        ui->pushButton_save->setEnabled(ch_state == "N");
        ui->tableView_detail->setEnabled(ch_state == "N");
        ui->frame->setEnabled(ch_state == "N");
    }
    ui->tableView_detail->lineView_show();
    ui->tableView_detail->updateSum(tablemodel_detail->fieldIndex("cost_price"));
}

void w_inventory_input_new::toaudit()
{
    detail_control("Y");
}

void w_inventory_input_new::tobad()
{
    detail_control("O");
}

void w_inventory_input_new::toUpdateTax(int index)
{
    ui->doubleSpinBox_int_tax->setValue(ui->comboBox_ch_providerNo->itemData(index, Qt::UserRole + 1).toDouble());
    tablemodel_master->model_data_set(0, "int_tax", ui->doubleSpinBox_int_tax->value());
}

bool w_inventory_input_new::saveData()
{
    QString errstring;

    if(tablemodel_detail->rowCount() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("单据为空"));
        return false;
    }

    if(ui->comboBox_vch_handle->curusrdata().toString().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("经手人不能为空"));
        return false;
    }
    //tablemodel_detail
    for(int row = 0; row < tablemodel_detail->rowCount(); row ++) {
        if(tablemodel_detail->opera(row) == lds_model_sqltablemodel::OpDelete)
            continue;

        if(tablemodel_detail->model_data(row, "ch_materialno").toString().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("第%1行，第%1行，名称不能为空").arg(row + 1));
            return false;
        }
        if(tablemodel_detail->model_data(row, "num_price").toDouble() <= 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("第%1行，单价必须大于0").arg(row + 1));
            return false;
        }
        double num_num = tablemodel_detail->model_data(row, "num_num").toDouble();
        double num_present = tablemodel_detail->model_data(row, "num_present").toDouble();
        if(ch_sheetType == "RK"
                || ch_sheetType == "TH") {
            if(num_num >= 0 && num_present >=0 && (num_num + num_present) > 0) {

            } else {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("第%1行，数量必须大于0").arg(row + 1));
                return false;
            }
        } else {
            if(num_num > 0) {

            } else {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("第%1行，数量必须大于0").arg(row + 1));
                return false;
            }
        }
    }

    //ls_sheetno
    QString ls_sheetno = ui->lineEdit_ch_sheetNo->text();
    if(ls_sheetno.isEmpty()) {
        lds_query_hddpos  query;
        QString flag = ch_sheetType + n_func::f_get_sysdatetime().toString("yyMMdd");
        query.execSelect(QString("SELECT max(ch_sheetNO) from cey_st_sheet_master  where ch_sheetNO like '%1%' ")
                         .arg(flag));
        query.next();
        QString ch_sheetNO_max = query.recordValue(0).toString();
        ls_sheetno = flag + QString().sprintf("%04d", ch_sheetNO_max.mid(8).toInt() + 1);

        for(int row = 0; row < tablemodel_detail->rowCount(); row ++) {
            tablemodel_detail->model_data_set(row, "ch_sheetno", ls_sheetno);
        }
    }
    //
    lds_query_hddpos  query;
    lds_query_hddpos::tran_saction();
    if(false == tablemodel_detail->trySubmitAll()) {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
        return false;
    }

    if(false == query.execInsertDuplicateUpdate("cey_st_sheet_master", "ch_sheetNo", qrtVariantPairList()
                                                << qrtVariantPair("ch_sheetNo", ls_sheetno)
                                                << qrtVariantPair("ch_sheetType", ch_sheetType)
                                                << qrtVariantPair("ch_operation", ui->comboBox_ch_operation->curusrdata())
                                                << qrtVariantPair("ch_providerNo", ui->comboBox_ch_providerNo->curusrdata())
                                                << qrtVariantPair("int_tax", ui->doubleSpinBox_int_tax->value())
                                                << qrtVariantPair("vch_handle", ui->comboBox_vch_handle->curusrdata())
                                                << qrtVariantPair("vch_memo", ui->lineEdit_vch_memo->text())

                                                << qrtVariantPair("vch_operId", lds::gs_operid)
                                                << qrtVariantPair("dt_operdate", n_func::f_get_sysdatetime())
                                                << qrtVariantPair("vch_audit", ui->comboBox_vch_audit->curusrdata())
                                                << qrtVariantPair("ch_state", "N")
                                                )) {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
        return false;
    }

    lds_query_hddpos::com_mit();
    ui->lineEdit_ch_sheetNo->setText(ls_sheetno);
    return true;
}

void w_inventory_input_new::detail_control(const QString &ch_state)
{
    if(tablemodel_master->model_is_dirty()
            || tablemodel_detail->model_is_dirty()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"), QObject::tr("取消"))) {
        case 0:
            if(!saveData()) {
                return;
            }
            torefresh();
            break;
        case 1:
            return;
        }
    }

    QString errstring;
    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认")+ (_TEXT_SLOT(this)), QObject::tr("确认"),QObject::tr("取消"))) {
        if(w_inventory_input::detail_control_transaction(ch_state, 0, tablemodel_master, tablemodel_detail, &errstring)) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            torefresh();
            return;
        }
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,errstring);
        torefresh();
    }
}
