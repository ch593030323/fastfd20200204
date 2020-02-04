#include "w_inventory_draw.h"
#include "ui_w_inventory_input.h"
#include "ui_w_inventory_draw.h"
#include "backheader.h"
#include "w_i_inventory.h"
#include "backheader.h"
#include "n_func.h"
#include <QTimer>
#include "lds_model_sqltablemodel_delegate_com.h"
#include "ui_w_inventory_input_new.h"
#include <QSqlRelationalDelegate>
#include "lds.h"
#include "lds_tableview_delegate_check.h"
#include "w_st_good_input_select_dish.h"
#include "lds_query_hddpos.h"
#include <QSqlRecord>

bool w_inventory_draw::detail_control_transaction(const QString &ch_state, const QString &ch_sheetno, QString *errstring)
{
    lds_query_hddpos  query;
    if(ch_sheetno.isEmpty()) {
        if(errstring) *errstring = QObject::tr("单据号码不能为空");
        return false;
    }
    query.execSelect(QString(" select ch_sheetno  from cey_st_check_detail where ch_sheetno='%1' ")
                     .arg(ch_sheetno));
    if(!query.next()) {
        if(errstring) *errstring = QObject::tr("空单据不允许审核");
        return false;
    }

    query.execSelect(QString(" select vch_audit, ch_state  from cey_st_check_master where ch_sheetno='%1' ")
                     .arg(ch_sheetno));
    query.next();
    if(query.recordValue("vch_audit").toString().isEmpty()) {
        if(errstring) *errstring = QObject::tr("审核人不能为空");
        return false;
    }
    if(query.recordValue("ch_state").toString() == "Y") {//审核
        if(errstring) *errstring = QObject::tr("单据")+ w_i_inventory::get_MODEL_INVENTORY_STATE_TYPE().value("Y").toString();
        return false;
    }
    if(query.recordValue("ch_state").toString() == "O") {//作废
        if(errstring) *errstring = QObject::tr("单据")+ w_i_inventory::get_MODEL_INVENTORY_STATE_TYPE().value("O").toString();
        return false;
    }

    lds_query_hddpos::tran_saction();
    if(false == query.execUpdate("cey_st_check_master", "ch_state", ch_state,"dt_audit", n_func::f_get_sysdatetime(), qrtEqual("ch_sheetno", ch_sheetno), errstring)) {
        lds_query_hddpos::roll_back();
        return false;
    }
    lds_query_hddpos::com_mit();
    return true;
}

w_inventory_draw::w_inventory_draw(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_inventory_draw)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->comboBox_ch_state->addItem(QObject::tr("全部"), "");
    ui->comboBox_ch_state->addItemsByMap(w_i_inventory::get_MODEL_INVENTORY_STATE_TYPE());
    ui->comboBox_ch_state->setcomusrdata("N");
    //tableview
    querymodel = new report_querymodel(this);
    ui->tableView->setTransferHheader();
    ui->tableView->setModel(querymodel);
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView->setSelectionMode(QTableView::SingleSelection);
    //datetime
    QDateTime dt = n_func::f_get_sysdatetime();
    ui->dateTimeEdit_to->setDateTime(QDateTime(dt.date(), QTime(23, 59, 59)));
    ui->dateTimeEdit_from->setDateTime(QDateTime(dt.date(), QTime(0,0,0)));
#ifdef QT_DEBUG
    ui->dateTimeEdit_from->setDateTime(ui->dateTimeEdit_from->dateTime().addYears(-1));
#endif
    //connect
    connect(ui->pushButton_exit, SIGNAL(clicked()), this,   SLOT(toexit()));
    connect(ui->pushButton_select, SIGNAL(clicked()), this,  SLOT(torefresh()));
    connect(ui->pushButton_audit, SIGNAL(clicked()), this, SLOT(toaudit()));
    connect(ui->pushButton_bad, SIGNAL(clicked()), this, SLOT(tobad()));
    connect(ui->pushButton_change, SIGNAL(clicked()), this, SLOT(tochange()));
    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(tochange()));
    connect(ui->pushButton_del, SIGNAL(clicked()), this, SLOT(todel()));
    connect(ui->pushButton_new, SIGNAL(clicked()), this, SLOT(tonew()));
    connect(ui->comboBox_ch_state, SIGNAL(currentIndexChanged(int)), this,SLOT(torefresh()));

    //
    QTimer::singleShot(0, this, SLOT(torefresh()));
}

w_inventory_draw::~w_inventory_draw()
{
    delete ui;
}

void w_inventory_draw::torefresh()
{
    //cey_sys_operator_d
    lds_query_hddpos  query;
    cey_sys_operator_d.clear();
    query.execSelect( backheader::SELECT_OPERID_NAME);
    while(query.next()) {
        cey_sys_operator_d.setData(query.recordValue(0).toString(), query.record().fieldName(1), query.recordValue(1));
    }
    //
    querymodel->setQuery(QString(" select a.ch_sheetno,a.ch_state, b.ch_materialno, b.num_sys, b.num_check, b.num_pro_loss, b.chg_flag, "
                                 " a.vch_operId,  a.dt_operdate,a.vch_audit, a.dt_audit, a.vch_handle   from cey_st_check_detail b left join cey_st_check_master a on a.ch_sheetno = b.ch_sheetno  "
                                 " where ch_state like '%1%' and dt_operdate >= '%2' and dt_operdate <= '%3' "
                                 " order by dt_operdate desc")
                         .arg(ui->comboBox_ch_state->curusrdata().toString())
                         .arg(ui->dateTimeEdit_from->dateTimeStr())
                         .arg(ui->dateTimeEdit_to->dateTimeStr()));

    if(0 ==ui->tableView->itemDelegateForColumn(querymodel->fieldIndex("ch_state"))) {
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("ch_state"), new lds_model_sqltablemodel_delegate_com(this, ui->comboBox_ch_state));
    }
    if(0 ==ui->tableView->itemDelegateForColumn(querymodel->fieldIndex("vch_handle"))) {
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("vch_handle"), new lds_model_sqltablemodel_delegate_com(this, &cey_sys_operator_d));
    }
    if(0 ==ui->tableView->itemDelegateForColumn(querymodel->fieldIndex("vch_audit"))) {
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("vch_audit"), new lds_model_sqltablemodel_delegate_com(this, &cey_sys_operator_d));
    }
    if(0 ==ui->tableView->itemDelegateForColumn(querymodel->fieldIndex("vch_operId"))) {
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("vch_operId"), new lds_model_sqltablemodel_delegate_com(this, &cey_sys_operator_d));
    }
    //
    if(0 ==ui->tableView->itemDelegateForColumn(querymodel->fieldIndex("ch_materialno"))) {
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("ch_materialno"), new lds_model_sqltablemodel_delegate_com(this, "cey_bt_dish", "ch_dishno", "vch_dishname"));
    }
    if(0 ==ui->tableView->itemDelegateForColumn(querymodel->fieldIndex("chg_flag"))) {
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("chg_flag"), new lds_tableview_delegate_check(querymodel->fieldIndex("chg_flag"), ui->tableView));
    }
    //
    ui->tableView->resizeColumnsToContents();
    ui->tableView->restore();

    ui->tableView->tableviewSetpan(ui->tableView,
                                   querymodel->fieldIndex("ch_sheetno"),
                                   QList<int>()
                                   << querymodel->fieldIndex("ch_sheetno")
                                   << querymodel->fieldIndex("ch_state")
                                   << querymodel->fieldIndex("vch_operId")
                                   << querymodel->fieldIndex("dt_operdate")
                                   << querymodel->fieldIndex("vch_audit")
                                   << querymodel->fieldIndex("dt_audit")
                                   << querymodel->fieldIndex("vch_handle")
                                   );
}

void w_inventory_draw::toexit()
{
    this->reject();
}

void w_inventory_draw::toaudit()
{
    detail_control("Y");
}

void w_inventory_draw::tobad()
{
    detail_control("O");
}

void w_inventory_draw::tochange()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    w_inventory_draw_new dialog(querymodel->model_data(row, "ch_sheetno").toString(),  this);
    dialog.setWindowTitle(QObject::tr("查看"));
    transparentCenterDialog(&dialog).exec();
    torefresh();
}

void w_inventory_draw::todel()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    if(querymodel->model_data(row, "ch_state").toString() == "Y") {//审核
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("单据")+ ui->comboBox_ch_state->comText("Y"));
        return;
    }
    if(querymodel->model_data(row, "ch_state").toString() == "O") {//作废
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("单据")+ ui->comboBox_ch_state->comText("O"));
        return;
    }
    if( 1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认删除"), QObject::tr("确认"), QObject::tr("取消"))) {
        return;
    }
    //未审核
    QString ls_sheetno = querymodel->model_data(row, "ch_sheetno").toString();
    if(querymodel->model_data(row, "ch_state").toString() == "N") {
        lds_query_hddpos::tran_saction();
        lds_query_hddpos  query;
        if(query.execDelete("cey_st_check_master",qrtEqual("ch_sheetNo", ls_sheetno))) {
            if(query.execDelete("cey_st_check_detail", qrtEqual("ch_sheetNo", ls_sheetno))) {
                lds_query_hddpos::com_mit();
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
                torefresh();
                return;
            }
        }
        lds_query_hddpos::roll_back();
    }
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    torefresh();
}

void w_inventory_draw::tonew()
{
    w_inventory_draw_new dialog("", this);
    transparentCenterDialog(&dialog).exec();
    torefresh();
}

void w_inventory_draw::detail_control(const QString &ch_state)
{
    QString errstring;
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认")+ (_TEXT_SLOT(this)), QObject::tr("确认"),QObject::tr("取消"))) {
        if(detail_control_transaction(ch_state, querymodel->model_data(row, "ch_sheetno").toString(), &errstring)) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            torefresh();
            return;
        }
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,errstring);
    }
}

w_inventory_draw_new::w_inventory_draw_new(const QString &ch_sheetno, QWidget *parent)
    : QDialog(parent)
{
    ui = new Ui_w_inventory_input_new;
    ui->setupUi(this);

    ui->comboBox_vch_audit->addItem("", "");
    ui->comboBox_vch_audit->addItemsBySql(backheader::SELECT_OPERID_NAME_FLAG);
    ui->comboBox_vch_handle->addItem("", "");
    ui->comboBox_vch_handle->addItemsBySql(backheader::SELECT_OPERID_NAME_FLAG);

    ui->lineEdit_ch_sheetNo->setEnabled(false);
    ui->lineEdit_ch_sheetNo->setText(ch_sheetno);
    //tablemodel_detail
    tablemodel_detail = new lds_model_sqltablemodel(this);
    tablemodel_detail->setTable("cey_st_check_detail");
    tablemodel_detail->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    
    tablemodel_detail->enable_vheaderdata_dirty_translate();
    //    tablemodel_detail->save_set_header("ch_sheetno", QObject::tr("单据号码"));
    tablemodel_detail->save_set_header("ch_materialno", QObject::tr("商品名称"));
    tablemodel_detail->save_set_header("num_sys", QObject::tr("系统库存"));
    tablemodel_detail->save_set_header("num_check", QObject::tr("盘点数量"), true);
    tablemodel_detail->save_set_header("num_pro_loss", QObject::tr("盈亏数量"));
    //    tablemodel_detail->save_set_header("cost_price", QObject::tr("商品进价"));
    tablemodel_detail->save_set_header("chg_flag", QObject::tr("调整库存"), true);

    ui->tableView_detail->setModel(tablemodel_detail);
    ui->tableView_detail->rePecifyHeader(tablemodel_detail->get_save_set_header_saver_column_list());
    ui->tableView_detail->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView_detail->setSelectionBehavior(QTableView::SelectItems);
    ui->tableView_detail->setSelectionMode(QTableView::SingleSelection);
    ui->tableView_detail->setItemDelegateForColumn(tablemodel_detail->fieldIndex("chg_flag"),
                                                   new lds_tableview_delegate_check(tablemodel_detail->fieldIndex("chg_flag"), ui->tableView_detail, true));

    //tablemodel_master
    tablemodel_master = new lds_model_sqltablemodel(this);
    tablemodel_master->setTable("cey_st_check_master");
    tablemodel_master->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tablemodel_master);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
    mapper->addMapping(ui->comboBox_vch_audit, tablemodel_master->fieldIndex("vch_audit"),  "comusrdata");
    mapper->addMapping(ui->comboBox_vch_handle, tablemodel_master->fieldIndex("vch_handle"),  "comusrdata");
    mapper->addMapping(ui->lineEdit_vch_memo, tablemodel_master->fieldIndex("vch_memo"));

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

    lds::hideWidget(ui->comboBox_ch_operation);
    lds::hideWidget(ui->label_2);
    this->setWindowTitle(QObject::tr("新增"));
    //setFilter
    QTimer::singleShot(0, this, SLOT(torefresh()));
}

w_inventory_draw_new::~w_inventory_draw_new()
{
    delete ui;
}

void w_inventory_draw_new::to_detail_datachange(const QModelIndex &index)
{
    int row = index.row();
    if(tablemodel_detail->fieldIndex("num_check") == index.column()) {
        tablemodel_detail->model_data_set(row, "num_pro_loss", tablemodel_detail->model_data_NIBOLAN(row, "num_check - num_sys"));
    }
}

void w_inventory_draw_new::tonew()
{
    w_st_good_input_select_dish dialog(this);
    dialog.setWindowTitle(QObject::tr("库存管理"));
    dialog.filter_by_itemflag();
    dialog.filter_by_st_check();
    if(QDialog::Accepted ==transparentCenterDialog(&dialog).exec()) {
        lds_query_hddpos  query;
        double num_sys;
        double cost_price;
        bool ch_dishno_existed;
        QString ch_dishno;
        foreach(const int &k, dialog.ret_row_list) {
            ch_dishno = dialog.get_tablemodel_value(k, "ch_dishno").toString();
            //
            ch_dishno_existed = false;
            for(int row = 0; row < tablemodel_detail->rowCount(); row ++) {
                if(tablemodel_detail->model_data(row, "ch_materialno").toString() ==ch_dishno ) {
                    ch_dishno_existed = true;
                    break;
                }
            }
            if(ch_dishno_existed)
                continue;
            //
            query.execSelect(QString("select num_num, cost_price  from cey_st_material_num where ch_materialno ='%1' ")
                             .arg(ch_dishno)
                             );
            query.next();
            num_sys = query.recordValue("num_num").toDouble();
            cost_price = query.recordValue("cost_price").toDouble();

            int rowcount = tablemodel_detail->rowCount();
            tablemodel_detail->insertRow(rowcount);
            tablemodel_detail->model_data_set(rowcount, "ch_sheetno", ui->lineEdit_ch_sheetNo->text());
            tablemodel_detail->model_data_set(rowcount, "ch_materialno", ch_dishno);
            tablemodel_detail->model_data_set(rowcount, "num_sys", num_sys);
            tablemodel_detail->model_data_set(rowcount, "num_check", num_sys);
            tablemodel_detail->model_data_set(rowcount, "cost_price", cost_price);
            tablemodel_detail->model_data_set(rowcount, "chg_flag", "Y");
            to_detail_datachange(tablemodel_detail->model_index(rowcount, "num_sys"));
        }
    }

    ui->tableView_detail->resizeColumnsToContents();
}

void w_inventory_draw_new::todel()
{
    int row = ui->tableView_detail->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }

    tablemodel_detail->removeRow(row);
}

void w_inventory_draw_new::tosave()
{
    if(saveData()) {
        torefresh();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
    }
}

void w_inventory_draw_new::toexit()
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

void w_inventory_draw_new::torefresh()
{
    tablemodel_detail->setFilter(QString("ch_sheetno = '%1' ")
                                 .arg(ui->lineEdit_ch_sheetNo->text()));
    ui->tableView_detail->resizeColumnsToContents();
    ui->tableView_detail->restore();

    if(ui->lineEdit_ch_sheetNo->text().length() > 0) {
        tablemodel_master->setFilter(QString(" ch_sheetno = '%1' ").arg(ui->lineEdit_ch_sheetNo->text()));
        mapper->setCurrentIndex(0);
    }
    //ch_state
    if(ui->lineEdit_ch_sheetNo->text().length() > 0) {
        lds_query_hddpos  query;
        query.execSelect(QString("SELECT ch_state from cey_st_check_master where ch_sheetno = '%1'; ").arg(ui->lineEdit_ch_sheetNo->text()));
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
}

void w_inventory_draw_new::toaudit()
{
    detail_control("Y");
}

void w_inventory_draw_new::tobad()
{
    detail_control("O");
}

bool w_inventory_draw_new::saveData()
{
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
        if(tablemodel_detail->opera(row) == lds_model_sqltablemodel::OpDelete) continue;

        if(tablemodel_detail->model_data(row, "ch_materialno").toString().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QObject::tr("行数") + ":" + QString::number(row+1), QObject::tr("名称不能为空"));
            return false;
        }
    }

    //ls_sheetno
    QString ls_sheetno = ui->lineEdit_ch_sheetNo->text();
    if(ls_sheetno.isEmpty()) {
        lds_query_hddpos  query;
        QString flag = "PD" + n_func::f_get_sysdatetime().toString("yyMMdd");
        query.execSelect(QString("SELECT max(ch_sheetno) from cey_st_check_master  where ch_sheetno like '%1%' ")
                         .arg(flag));
        query.next();
        QString ch_sheetno_max = query.recordValue(0).toString();
        ls_sheetno = flag + QString().sprintf("%04d", ch_sheetno_max.mid(8).toInt() + 1);

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

    query.execSelect(QString(" select ch_sheetno from cey_st_check_master where ch_sheetno = '%1' ")
                     .arg(ls_sheetno));
    if(query.next()) {
        if(!query.execUpdate("cey_st_check_master", qrtVariantPairList()
                             << qrtVariantPair("vch_handle", ui->comboBox_vch_handle->curusrdata())
                             << qrtVariantPair("vch_audit", ui->comboBox_vch_audit->curusrdata())
                             << qrtVariantPair("ch_state", "N")
                             << qrtVariantPair("vch_memo", ui->lineEdit_vch_memo->text()),
                             qrtEqual("ch_sheetno", ls_sheetno)
                             )) {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
            return false;
        }
    } else {
        if(!query.execInsert("cey_st_check_master", qrtVariantPairList()
                             << qrtVariantPair("ch_sheetno", ls_sheetno)
                             << qrtVariantPair("vch_handle", ui->comboBox_vch_handle->curusrdata())
                             << qrtVariantPair("vch_operId", lds::gs_operid)
                             << qrtVariantPair("dt_operdate", n_func::f_get_sysdatetime())
                             << qrtVariantPair("vch_audit", ui->comboBox_vch_audit->curusrdata())

                             << qrtVariantPair("ch_state", "N")
                             << qrtVariantPair("vch_memo", ui->lineEdit_vch_memo->text())
                             )) {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
            return false;
        }
    }

    lds_query_hddpos::com_mit();
    ui->lineEdit_ch_sheetNo->setText(ls_sheetno);
    return true;
}

void w_inventory_draw_new::detail_control(const QString &ch_state)
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
        if(w_inventory_draw::detail_control_transaction(ch_state, tablemodel_master->model_data(0, "ch_sheetno").toString(), &errstring)) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            torefresh();
            return;
        }
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,errstring);
        torefresh();
    }
}
