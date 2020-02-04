#include "w_bt_dish_kitchen_view_plan.h"
#include "ui_w_bt_dish_kitchen_view_plan.h"
#include "lds_query_hddpos.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include <QTimer>
#include <QSqlRelationalDelegate>
#include "lds_messagebox.h"
#include "w_bt_dish_signal_transfer.h"
#include "lds_ogg_play.h"

w_bt_dish_kitchen_view_plan::w_bt_dish_kitchen_view_plan(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_bt_dish_kitchen_view_plan)
{
    ui->setupUi(this);
    lds_query_hddpos query;
    ui->spinBox_int_auto_take->setRange(0, 99);
    ui->spinBox_int_order_warn_mins->setRange(0, 99);
    ui->spinBox_int_voice_count->setRange(0, 99);
    //
    ui->comboBox_kt_kitchen_shop->addItem(QObject::tr("快餐模式"), "fastfdfood");
    ui->comboBox_kt_kitchen_shop->addItem(QObject::tr("酒楼模式"), "restaurant");
    ui->comboBox_kt_kitchen_shop->setcomusrdata(query.cey_sys_parameter_get("kt_kitchen_shop", "fastfdfood"));
    //
    ui->comboBox_vch_plan_id_service->addItemsBySql("select vch_plan_id, vch_plan_name from cey_bt_kitchen_view where vch_kitchen_type = 'service' ");
    ui->comboBox_vch_kitchen_type->addItem(QObject::tr("厨显"), "kitchen");
    ui->comboBox_vch_kitchen_type->addItem(QObject::tr("出品"), "service");
    ui->comboBox_vch_language->addItem(QObject::tr("默认"), "default");
    ui->comboBox_vch_language->addItem(QObject::tr("中文"), "zh");
    ui->comboBox_vch_language->addItem(QObject::tr("英文"), "en");
    ui->comboBox_vch_screen_primary_size->addItem(QObject::tr("默认"), "default");
    ui->comboBox_vch_screen_primary_size->addItem("800x600", "800x600");
    ui->comboBox_vch_screen_primary_size->addItem("1024x768", "800x600");
    ui->comboBox_vch_screen_primary_size->addItem("1366x768", "800x600");
    ui->comboBox_vch_screen_secondary_size->addItem(QObject::tr("默认"), "default");
    ui->comboBox_vch_screen_secondary_size->addItem("800x600", "800x600");
    ui->comboBox_vch_screen_secondary_size->addItem("1024x768", "800x600");
    ui->comboBox_vch_screen_secondary_size->addItem("1366x768", "800x600");
    //
    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_kitchen_view");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->save_set_header("vch_plan_id", QObject::tr("编号"));
    tablemodel->save_set_header("vch_plan_name", QObject::tr("名称"));
    tablemodel->save_set_header("vch_kitchen_type", QObject::tr("类型"));
    tablemodel->save_set_header("vch_plan_id_service", QObject::tr("出品"));
    tablemodel->tablename_kvmap_create("vch_plan_id_service_list");

    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->setColumnWidth(tablemodel->fieldIndex("vch_plan_id"), 100);
    ui->tableView->setColumnWidth(tablemodel->fieldIndex("vch_plan_name"), 150);
    ui->tableView->setColumnWidth(tablemodel->fieldIndex("vch_kitchen_type"), 100);
    ui->tableView->setColumnWidth(tablemodel->fieldIndex("vch_plan_id_service"), 80);
    ui->tableView->setFixedWidth(450);
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_kitchen_type"), new lds_model_sqltablemodel_delegate_com(this, ui->comboBox_vch_kitchen_type));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_plan_id_service"), new lds_model_sqltablemodel_delegate_com(this, tablemodel->tablename_kvmap_d("vch_plan_id_service_list")));
    //! datawidget
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tablemodel);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
    mapperAdd(ui->lineEdit_vch_plan_name);
    mapperAdd(ui->lineEdit_vch_plan_id);
    mapperAdd(ui->lineEdit_vch_terminal);
    mapperAdd(ui->checkBox_ch_auto_take, "checkYN");
    mapperAdd(ui->checkBox_ch_voice_count, "checkYN");
    mapperAdd(ui->checkBox_ch_screen_secondary, "checkYN");
    mapperAdd(ui->checkBox_ch_pop_new_bill_info, "checkYN");
    mapperAdd(ui->spinBox_int_voice_count);
    mapperAdd(ui->spinBox_int_order_warn_mins);
    mapperAdd(ui->spinBox_int_auto_take);
    mapperAdd(ui->comboBox_vch_plan_id_service, "comusrdata");
    mapperAdd(ui->comboBox_vch_kitchen_type, "comusrdata");
    mapperAdd(ui->comboBox_vch_language, "comusrdata");
    mapperAdd(ui->comboBox_vch_screen_primary_size, "eitherUsrOrText");
    mapperAdd(ui->comboBox_vch_screen_secondary_size, "eitherUsrOrText");

    ui->lineEdit_vch_plan_id->setEnabled(false);
#ifdef QT_DEBUG
#else
    ui->lineEdit_vch_terminal->setEnabled(false);
#endif

    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_refresh_machine, SIGNAL(clicked()), this, SLOT(torefresh_machine()));
    connect(ui->tableView,SIGNAL(selectChanged(int)),this,SLOT(tomapperSelect(int)));
    connect(ui->pushButton_ogg_new_order, SIGNAL(clicked()), this, SLOT(toogg_new_order()));
    connect(mapper->itemDelegate(), SIGNAL(commitData(QWidget*)), this, SLOT(tomapperCommit(QWidget*)));
    connect(ui->comboBox_vch_kitchen_type, SIGNAL(currentIndexChanged(int)), this, SLOT(toadjust_2ndscreen(int)));

    QTimer::singleShot(0, this, SLOT(torefresh()));
}

w_bt_dish_kitchen_view_plan::~w_bt_dish_kitchen_view_plan()
{
    delete ui;
}

void w_bt_dish_kitchen_view_plan::torefresh()
{
    tablemodel->select();
    ui->tableView->restore(0);
}

void w_bt_dish_kitchen_view_plan::torefresh_machine()
{
    if(tablemodel->model_is_dirty()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"),QObject::tr("取消"))) {
        case 0:
            if(!saveData()) {
                return;
            }
            break;
        case 1:
            break;
        case 2:
            return;
        }
    }

    lds_query_hddpos query;
    query.tran_saction();
    query.execSelect(" select * from cey_sys_lan_terminal");
    while(query.next()) {
        QString vch_plan_id = lds_query_hddpos::selectValue("select ifnull(max(vch_plan_id), 10) + 1 from cey_bt_kitchen_view ").toString().rightJustified(2, '0', true);
        QString vch_terminal = query.recordValue("vch_terminal").toString();
        if(!lds_query_hddpos::selectHasNext(QString(" select 1 from cey_bt_kitchen_view vch_terminal = '%1' ").arg(vch_terminal))) {
            lds_query_hddpos::insertTable("cey_bt_kitchen_view", qrtVariantPairList()
                                          << qrtVariantPair("vch_plan_id", vch_plan_id)
                                          << qrtVariantPair("vch_plan_name", vch_plan_id)
                                          << qrtVariantPair("vch_kitchen_type", "kitchen")
                                          << qrtVariantPair("vch_terminal", vch_terminal)
                                          << qrtVariantPair("int_voice_count", 1)
                                          << qrtVariantPair("int_order_warn_mins", 10)
                                          << qrtVariantPair("int_auto_take", 10)
                                          << qrtVariantPair("vch_language", "default")
                                          << qrtVariantPair("vch_screen_primary_size", "default")
                                          << qrtVariantPair("vch_screen_secondary_size", "default")
                                          );

        }
    }
    query.com_mit();
    //
    torefresh();
}

void w_bt_dish_kitchen_view_plan::took()
{
    if(saveData()) {
    }
}

void w_bt_dish_kitchen_view_plan::tocancel()
{
    if(tablemodel->model_is_dirty()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"),QObject::tr("取消"))) {
        case 0:
            if(!saveData()) {
                return;
            }
            break;
        case 1:
            break;
        case 2:
            return;
        }
    }

    this->reject();
}

void w_bt_dish_kitchen_view_plan::tomapperCommit(QWidget *editor)
{
    if(editor != ui->comboBox_vch_kitchen_type)
        return;
    int row = mapper->currentIndex();
    updatevch_plan_id_service_list(row);
    ui->comboBox_vch_plan_id_service->setcomusrdata(tablemodel->model_data(row, "vch_plan_id_service"));
}

void w_bt_dish_kitchen_view_plan::tomapperSelect(int row)
{
    updatevch_plan_id_service_list(row);
    mapper->setCurrentIndex(row);
}

void w_bt_dish_kitchen_view_plan::toogg_new_order()
{
    for(int k = 0; k < ui->spinBox_int_voice_count->value(); k ++) {
        bool f = QMetaObject::invokeMethod(w_bt_dish_signal_transfer::ogg_play_polling,
                                  "signal_play",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, "userdata/settings/ogg/"),
                                  Q_ARG(QStringList, QStringList() << "new_order.mp3")
                                  );
        if(!f) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, "invokeMethod not valid");
            break;
        }
    }
}

void w_bt_dish_kitchen_view_plan::toadjust_2ndscreen(int index)
{
    if(ui->comboBox_vch_kitchen_type->comusrdata().toString() == "kitchen") {
        lds::hideWidget(ui->checkBox_ch_screen_secondary);
        lds::hideWidget(ui->comboBox_vch_screen_secondary_size);
        ui->checkBox_ch_screen_secondary->setCheckYN("N");
        tablemodel->model_data_set(ui->tableView->currentIndex().row(), "ch_screen_secondary", "N");
    }
    if(ui->comboBox_vch_kitchen_type->comusrdata().toString() == "service") {
        lds::hideWidget(ui->checkBox_ch_screen_secondary, false);
        lds::hideWidget(ui->comboBox_vch_screen_secondary_size, false);
    }
}

void w_bt_dish_kitchen_view_plan::updatevch_plan_id_service_list(int row)
{
    tablemodel->tablename_kvmap_delete("vch_plan_id_service_list");
    for(int row = 0; row < tablemodel->rowCount(); row ++) {
        if(tablemodel->model_data(row, "vch_kitchen_type").toString() == "service") {
            QString vch_plan_id = tablemodel->model_data(row, "vch_plan_id").toString();
            QString vch_plan_name = tablemodel->model_data(row, "vch_plan_name").toString();
            tablemodel->tablename_kvmap_insert_value("vch_plan_id_service_list", vch_plan_id, vch_plan_name);
        }
    }
    //
    if(tablemodel->model_data(row, "vch_kitchen_type").toString() == "service") {
        ui->comboBox_vch_plan_id_service->clear();
        tablemodel->model_data_set(row, "vch_plan_id_service", "");
    }
    if(tablemodel->model_data(row, "vch_kitchen_type").toString() == "kitchen") {
        ui->comboBox_vch_plan_id_service->clear();
        lds_model_sqltablemodel_d::fillData(ui->comboBox_vch_plan_id_service, tablemodel->tablename_kvmap_d("vch_plan_id_service_list"));
    }
    for(int k = 0; k < tablemodel->rowCount(); k ++)
        ui->tableView->update(tablemodel->model_index(k, "vch_plan_id_service"));
}

bool w_bt_dish_kitchen_view_plan::saveData()
{
    QString errstring;
    lds_query_hddpos query;
    for(int row = 0; row < tablemodel->rowCount(); row ++) {
        if(tablemodel->model_data(row, "vch_plan_name").toString() == "") {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("第%1行，第%1行，名称不能为空").arg(row + 1));
            return false;
        }
    }
    QStringList vch_plan_id_list;
    for(int row = 0; row < tablemodel->rowCount(); row ++) {
        if(tablemodel->opera(row) & lds_model_sqltablemodel::OpUpdate) {
            vch_plan_id_list << tablemodel->model_data(row, "vch_plan_id").toString();
        }
    }

    query.tran_saction();
    if(!tablemodel->trySubmitAll(&errstring)) {
        query.roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return false;
    }
    query.cey_sys_parameter_set("kt_kitchen_shop", ui->comboBox_kt_kitchen_shop->curusrdata().toString());
    foreach(const QString &vch_plan_id, vch_plan_id_list)
        lds_query_hddpos::updateTable("cey_bt_kitchen_view", qrtVariantPairList() << qrtVariantPair("ch_refresh_sys_geometry", "Y"), QString("vch_plan_id = '%1' ").arg(vch_plan_id));
    query.com_mit();
    ui->tableView->restore(0);
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
    return true;
}

void w_bt_dish_kitchen_view_plan::mapperAdd(QWidget *widget, const QByteArray &propertyName)
{
    QString obj_name = widget->objectName();
    int k = obj_name.indexOf("_");
    if(propertyName.count() == 0) {
        mapper->addMapping(widget,tablemodel->fieldIndex(obj_name.mid(k + 1)));
    } else {
        mapper->addMapping(widget,tablemodel->fieldIndex(obj_name.mid(k + 1)), propertyName);
    }
}
