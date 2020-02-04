#include "kds_main_view.h"
#include "ui_kds_main_view.h"
#include "cjson_help.h"
#include <QPainter>
#include <QtDebug>
#include "lds.h"
#include "lds_query_db.h"
#include "kds.h"
#include <QTimer>
#include "lds_roundeddialog.h"
#include "lds_messagebox.h"
#include "ftableview_delegate.h"
#include "public_select_stringlist.h"
#include "kds_main_info_brief.h"
#include "vch_print_memodata.h"
#include "lds_ogg_play.h"
#include  <QFile>
#include  <QWidgetAction>
#include  <QMenu>
#include "kds_quit.h"
#include  <QKeyEvent>

QStringList kds_main_view::uniqueStrList(QStringList list, const QString &str)
{
    if(list.indexOf(str) == -1)
        return list << str;
    return list;
}

QString kds_main_view::strList2jsonArray(const QStringList &list)
{
    QString array;
    foreach(const QString &str, list) {
        array += "\"" + str + "\",";
    }
    array.chop(1);
    return "[" + array + "]";
}

kds_main_view::kds_main_view(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::kds_main_view)
{
    ui->setupUi(this);
    lds::setwflagFrameless(this);
    barcode_scan = 0;
    this->setStyleSheet("QDialog#kds_main_view{background-color:rgb(18, 26, 34); }");

    //
    connect(ui->pushButton_main_service, SIGNAL(clicked()), this, SLOT(toservice()));
    connect(ui->pushButton_main_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_main_prepare, SIGNAL(clicked()), this, SLOT(torefreshprepare()));
    connect(ui->pushButton_main_prepare_warn, SIGNAL(clicked()), this, SLOT(torefreshpreparewarn()));
    connect(ui->pushButton_main_cooking, SIGNAL(clicked()), this, SLOT(torefreshcooking()));
    connect(ui->tableWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(toshowtablebrief(QModelIndex)));
    connect(ui->tableView_service, SIGNAL(clicked(QModelIndex)), this, SLOT(toserviceclick(QModelIndex)));
    connect(ui->tableView_group, SIGNAL(clicked(QModelIndex)), this, SLOT(togroupclick(QModelIndex)));
    connect(ui->tableView_take_table, SIGNAL(clicked(QModelIndex)), this, SLOT(totake_tableclick(QModelIndex)));
    connect(ui->frame_take_brief, SIGNAL(signalBack()), this, SLOT(tostateback()));
    connect(ui->frame_take_brief, SIGNAL(signalRefresh()), this, SLOT(torefresh()));
    connect(ui->frame_take_brief, SIGNAL(signalRefreshSingle()), this, SLOT(torefreshsingle()));
    connect(ui->frame_login, SIGNAL(signalExit()), this, SLOT(toexit()));
    connect(ui->frame_login, SIGNAL(signalLogin()), this, SLOT(toinit()));

    //number
    connect(ui->pushButton_18, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_14, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_16, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_17, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_9, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_10, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_11, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_12, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_8, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_13, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_backspace, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_clear, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_take, SIGNAL(clicked()), this, SLOT(tonum()));

    refreshView("item_login");
}

kds_main_view::~kds_main_view()
{
    delete ui;
}

void kds_main_view::toinit()
{
    if(kds::model == 0)
        kds::model = new lds_model_sqltablemodel_m(this, QSqlDatabase::database());
    ui->lineEdit_serialno->setText("");
    //
    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setItemDelegate(new kds_main_view_delegate(this));
    ui->tableWidget->setContentSize(2, 3);
    ui->tableWidget->setEditTriggers(QTableView::NoEditTriggers);

    model_service = new kds_standmodel_sql;
    model_service->set_row_column(8, 2);
    model_service->disablePageRecord();
    ui->tableView_service->setModel(model_service);
    ui->tableView_service->setItemDelegate(new kds_main_service_delegate(this));
    ui->tableView_service->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableView_service->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableView_service->horizontalHeader()->hide();
    ui->tableView_service->verticalHeader()->hide();
    ui->tableView_service->setEditTriggers(QTableView::NoEditTriggers);
    model_service->setSelectionModel(ui->tableView_service->selectionModel());
    //
    model_group = new kds_standmodel_sql;
    model_group->set_row_column(9 , 1);
    model_group->disablePageRecord();
    //    model_group->removePageRecordDeep(0);//不保存菜品数量
    ui->tableView_group->setModel(model_group);
    ui->tableView_group->setItemDelegate(new kds_main_group_delegate(this));
    ui->tableView_group->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableView_group->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableView_group->horizontalHeader()->hide();
    ui->tableView_group->verticalHeader()->hide();
    ui->tableView_group->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_group->setSelectionMode(QTableView::SingleSelection);
    ui->tableView_group->setSelectionBehavior(QTableView::SelectItems);
    model_group->setSelectionModel(ui->tableView_group->selectionModel());
    //
    model_take_table = new kds_standmodel_sql;
    model_take_table->set_row_column(9 , 1);
    model_take_table->disablePageRecord();
    ui->tableView_take_table->setModel(model_take_table);
    ui->tableView_take_table->setItemDelegate(new kds_main_take_table_delegate(this));
    ui->tableView_take_table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableView_take_table->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableView_take_table->horizontalHeader()->hide();
    ui->tableView_take_table->verticalHeader()->hide();
    ui->tableView_take_table->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_take_table->setSelectionMode(QTableView::SingleSelection);
    ui->tableView_take_table->setSelectionBehavior(QTableView::SelectItems);
    model_take_table->setSelectionModel(ui->tableView_take_table->selectionModel());
    //
    ui->frame_take_brief->setCommitType(kds_main_info_brief::commitAuto);
    //
    lds::sysconf->setValue("system_setting/screen_primary_size", kds::cey_bt_kitchen_view_data("vch_screen_primary_size").toString());
    lds::resetWINDOW_SIZE();
    refreshView("");
    refresh_state_push(refresh_state_left());
    //
    resetGeometry(refresh_state_cur());
    refreshView(refresh_state_cur());

    toloopChecknew();
    QTimer::singleShot(1000, this, SLOT(torefresh()));

    //
    if("Y" == kds::cey_bt_kitchen_view_data("ch_screen_secondary")
            && !kds::pos_kdsdd_existed()) {
        kds::pos_kdsdd_start();
    }
}

void kds_main_view::toservice()
{
    refresh_state_push("item_finished_serial");
    refresh();
}

void kds_main_view::toexit()
{
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确定退出"), QObject::tr("确定"), QObject::tr("取消"))) {
        return;
    }
    kds::kdsdd_send_type(kds::kdsdd_request_close);
    //
    this->reject();
}

void kds_main_view::torefresh()
{
    refresh();
}

void kds_main_view::torefreshsingle()
{
    QModelIndex index;
    if(ui->frame_take_brief->which == ui->tableView_group->objectName()) {
        index = ui->tableView_group->currentIndex();
        model_group->refreshcur();
        tableview_select_index_default_first(index, ui->tableView_group);
    }
    if(ui->frame_take_brief->which == ui->tableView_take_table->objectName()) {
        index = ui->tableView_take_table->currentIndex();
        model_take_table->refreshcur();
        if(model_take_table->model_data(0, 0).toString() == "") {
            QString item_finished_from = "from cey_u_orderdish_print_kt_table where vch_plan_id in " + kds::sql_plan_service_in() + " and int_finish > 0 ";
            ui->label_num_service->setText(kds::selectValue("select count(0) " + item_finished_from).toString());
        }
        tableview_select_index_default_first(index, ui->tableView_take_table);
        kdsddsend();
    }
    if(ui->frame_take_brief->which == ui->tableWidget->objectName()) {
        if(ui->frame_take_brief->getIndexJsonCount() == 0)
            tostateback();
        else
            model_group->refreshcur();
    }
}

void kds_main_view::torefreshprepare()
{
    refresh_state_push("item_prepared");
    refresh();
}

void kds_main_view::torefreshpreparewarn()
{
    refresh_state_push("item_prepared_warn");
    refresh();
}

void kds_main_view::torefreshcooking()
{
    refresh_state_push("item_cooking");
    refresh();
}

void kds_main_view::toshowtablebrief(const QModelIndex &index)
{
    //点6个cell 的table
    cJSON_path p(index.data().toByteArray());
    if(p.isNull())
        return;
    QString bill_no = index.data(Qt::UserRole).toString();
    QString plan_id = index.data(Qt::UserRole + 1).toString();

    p.replaceItemValue("", "row_id_visible", 1);
    p.replaceItemValue("", "less_than_state", KDS::cookFinished);
    p.replaceItemValue("", "exception_finish_sql", QString(" update cey_u_orderdish_print_kt_table set ch_state = %1 where ch_billno = '%2' and vch_plan_id = '%3' ").arg(KDS::cookFinished).arg(bill_no).arg(plan_id));
    ui->frame_take_brief->setIndexJson(p.toByteArray(), ui->tableWidget->objectName());
    refreshView(refresh_state_push("item_brief"));
}

void kds_main_view::toserviceclick(const QModelIndex &index)
{
    if(index.data(Qt::UserRole + 1).toInt() == -1) {
        model_service->togeneratePre();
    } else if(index.data(Qt::UserRole + 1).toInt() == -2) {
        model_service->togenerateNext();
    } else {
        servicetoken(index);
        kdsddsend();
    }
}

void kds_main_view::togroupclick(const QModelIndex &index)
{
    if(index.data(Qt::UserRole + 1).toInt() == -1) {
        model_group->togeneratePre();
    } else if(index.data(Qt::UserRole + 1).toInt() == -2) {
        model_group->togenerateNext();
    } else if(index.data(Qt::UserRole + 1).toString().count() > 0) {
        QList<kds::billPlanDetail> list;
        QString dish_no = index.data(Qt::UserRole + 1).toString();
        QString bill_no_pre;
        QString bill_no;
        QString vch_plan_id = kds::cey_bt_kitchen_view_data("vch_plan_id").toString();
        lds_query_db query;
        query.execSelect(QString(" select ch_billno, int_flowID from cey_u_orderdish_print_kt_order where ch_dishno = '%1' and vch_plan_id = '%2'  and ch_state < %3 ; ")
                         .arg(dish_no)
                         .arg(vch_plan_id)
                         .arg(KDS::cookFinished)
                         );
        while(query.next()) {
            bill_no = query.recordValue("ch_billno").toString();
            if(bill_no != bill_no_pre|| bill_no_pre == "") {
                bill_no_pre = bill_no;
                kds::billPlanDetail d;
                d.bill_no = bill_no;
                d.vch_plan_id = vch_plan_id;
                list << d;
            }
            list.last().flow_id_list << query.recordValue("int_flowID").toString();
        }
        cJSON_path p(kds::btn_detail_json(list));
        p.replaceItemValue("", "less_than_state", KDS::cookFinished);
        p.replaceItemValue("", "exception_finish_sql", QString(" update cey_u_orderdish_print_kt_dish set num_num=0, num_back=0, num_cook=0, num_finish=0 where ch_dishno = '%1' and vch_plan_id = '%2' ").arg(dish_no).arg(vch_plan_id));
        ui->frame_take_brief->setIndexJson(p.toByteArray(), ui->tableView_group->objectName());
        refreshView(refresh_state_push("item_brief"));
    }
}

void kds_main_view::totake_tableclick(const QModelIndex &index)
{
    if(index.data(Qt::UserRole + 1).toInt() == -1) {
        model_take_table->togeneratePre();
    } else if(index.data(Qt::UserRole + 1).toInt() == -2) {
        model_take_table->togenerateNext();
    } else {
        QString bill_no = index.data(Qt::UserRole).toString();
        QString bill_plan = index.data(Qt::UserRole + 1).toString();
        QString vch_plan_id = index.data(Qt::UserRole + 2).toString();
        if(bill_no.count() > 0) {
            cJSON_path p(kds::btn_detail_json(bill_no, vch_plan_id, KDS::cookToken));
            p.replaceItemValue("", "less_than_state", KDS::cookToken);
            ui->frame_take_brief->setIndexJson(p.toByteArray(), ui->tableView_take_table->objectName());
        }
    }
}

void kds_main_view::tonum()
{
    QPushButton *b = qobject_cast<QPushButton*>(this->sender());
    if(b == 0)
        return;
    if(b == ui->pushButton_take) {
        QString serialno = ui->lineEdit_serialno->text();
        lds_query_db query;
        query.execSelect(
                    "select b.ch_billno, a.vch_plan_id, b.vch_memo from cey_u_orderdish_print_kt_table a, cey_u_master b "
                    "where a.ch_billno = b.ch_billno and a.int_finish > 0 and a.vch_plan_id in " + kds::sql_plan_service_in() + " and vch_memo = " + kds::sql_string_equal(serialno));
        if(query.next()) {
            servicetoken(query.recordValue("ch_billno").toString(), query.recordValue("vch_plan_id").toString());
        }
        return;
    }
    if(b == ui->pushButton_backspace) {
        ui->lineEdit_serialno->backspace();
        return;
    }
    if(b == ui->pushButton_clear) {
        ui->lineEdit_serialno->clear();
        return;
    }
    ui->lineEdit_serialno->insert(b->text());
}

void kds_main_view::tostateback()
{
    refresh_state_push(refresh_state_old());
    refresh();
}

void kds_main_view::toloopChecknew()
{
    lds_query_db query;
    QStringList flow_id_list;
    bool sql_refresh = false;
    QString errstring;
    kds::cey_bt_kitchen_view_clear();
    //1.有新的菜品或菜品被修改
    if("Y" == kds::cey_bt_kitchen_view_data("ch_refresh_order_insert").toString()) {
        kds::cey_bt_kitchen_view_update("ch_refresh_order_insert", "N");

        if("Y" == kds::cey_bt_kitchen_view_data("ch_pop_new_bill_info").toString()) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("新的订单"), true);
        }
        //快餐出品的效果
        if(kds::is_fastfdfood_kitchen()) {
            kds::cey_bt_kitchen_view_update_service("ch_refresh_order_update", "Y");
        }
        sql_refresh = true;
        kds::voice_play();
    }
    if("Y" == kds::cey_bt_kitchen_view_data("ch_refresh_order_update").toString()) {
        kds::cey_bt_kitchen_view_update("ch_refresh_order_update", "N");

        if(lds::widgetIsShow(ui->tableView_service)) {
            kds::voice_play();
            sql_refresh = true;
        }
        if(lds::widgetIsShow(ui->tableWidget)) {
            sql_refresh = true;
        }
        if(lds::widgetIsShow(ui->tableView_take_table)) {
            kds::voice_play();
            sql_refresh = true;
        }
    }

    //系统参数被修改
    if("Y" == kds::cey_bt_kitchen_view_data("ch_refresh_sys_geometry").toString()) {
        kds::cey_bt_kitchen_view_update("ch_refresh_sys_geometry", "N");
        //language
        lds::sysconf->setValue("kds_manager/language", kds::cey_bt_kitchen_view_data("vch_language").toString());
        kds::setTranslator(lds::sysconf->value("kds_manager/language", "zh").toString());
        kds::kds_second_screen_translate();
        kds::kdsdd_send_type(kds::kdsdd_request_setTitle);
        //~language
        //all geometry
        lds::sysconf->setValue("system_setting/screen_primary_size", kds::cey_bt_kitchen_view_data("vch_screen_primary_size").toString());
        lds::resetWINDOW_SIZE();
        resetGeometry();
        kds::kdsdd_send_scree_size(kds::cey_bt_kitchen_view_data("vch_screen_secondary_size").toString());
        //~all geometry

        if(kds::cey_bt_kitchen_view_data("ch_screen_secondary").toString() == "Y"
                && !kds::pos_kdsdd_existed()) {
            kds::pos_kdsdd_start();
        }
        if(kds::cey_bt_kitchen_view_data("ch_screen_secondary").toString() != "Y"
                && kds::pos_kdsdd_existed()) {
            kds::kdsdd_send_type(kds::kdsdd_request_close);
        }
    }

    //自动取餐
    if(kds::cey_bt_kitchen_view_data("vch_kitchen_type").toString() == "kitchen"
            && "Y" == kds::cey_bt_kitchen_view_data("ch_auto_take").toString()) {
        if(conf_parameter_data("last_take_time_mins").toDateTime().isNull()) {
            conf_parameter_set("last_take_time_mins", kds::currentDateTime());
        }
        if(conf_parameter_data("last_take_time_mins").toDateTime().secsTo(kds::currentDateTime()) / 60 >= kds::cey_bt_kitchen_view_data("int_auto_take").toInt()) {
            conf_parameter_set("last_take_time_mins", kds::currentDateTime());
            //
            flow_id_list.clear();
            query.execSelect(QString(" select int_flowID from cey_u_orderdish_print_kt_order where vch_plan_id =  '%1' and ch_state = %2 ")
                             .arg(kds::cey_bt_kitchen_view_data("vch_plan_id").toString())
                             .arg(KDS::cookFinished)
                             );
            while(query.next()) {
                flow_id_list << query.recordValue("int_flowID").toString();
            }
            if(flow_id_list.count() > 0) {
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("自动取餐") + ":" + QString::number(flow_id_list.count()), true);
                query.db.transaction();
                foreach(const QString &flow_id, flow_id_list) {
                    if(!kds::update_print_kt_dish(flow_id, KDS::cookToken, errstring)) {
                        query.db.rollback();
                        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
                        goto auto_take_end;
                    }
                }
                query.db.commit();
                /*! */
                kds::cey_bt_kitchen_view_update_service("ch_refresh_order_update", "Y");
                kds::model->tablename_kvmap_delete_for_select_and_autoselect();//
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("自动取餐") + ":" + QObject::tr("操作成功"), true);
            }
        }
    }
auto_take_end:
    //任何界面超时15s,切换到prepare界面
    if(kds::kds_model_data("cur_parameter", "mouse_time").toDateTime().isNull()) {
        kds::kds_model_setdata("cur_parameter", "mouse_time", kds::currentDateTime());
    }
    if(kds::cey_bt_kitchen_view_data("vch_kitchen_type").toString() == "kitchen"
            && kds::kds_model_data("cur_parameter", "mouse_time", kds::currentDateTime()).toDateTime().secsTo(kds::currentDateTime()) >15) {
        kds::kds_model_setdata("cur_parameter", "mouse_time", kds::currentDateTime());
        if(kds::kds_model_data("cur_parameter", "mouse_running").toInt() == 0) {
            sql_refresh = true;
        }
        kds::kds_model_setdata("cur_parameter", "mouse_running", 0);
    }
    //退单请求
    if(kds::cey_bt_kitchen_view_data("ch_refresh_order_quit").toString() == "Y"
            && kds::cey_bt_kitchen_view_data("vch_kitchen_type").toString() == "kitchen"
            && lds::widgetIsShow(ui->tableWidget)
            && lds::isqAppActiveWindow("kds_main_view")
            ) {
        kds::cey_bt_kitchen_view_update("ch_refresh_order_quit", "N");

        kds_quit dialog(this);
        lds_roundeddialog_rect_align_600x400 d(&dialog);
#ifdef QT_DEBUG
        d.move(1000, 0);
#endif
        d.exec();
    }

    if(sql_refresh) {
        refresh_state_push(refresh_state_left());
        refresh();
    }
    //刷新时间
    ui->tableWidget->updateAllIndex();
    //第二次循环
    QTimer::singleShot(1000, this, SLOT(toloopChecknew()));
}

//state{0:未制作,1:正在制作,2:已完成,3:挂起}
void kds_main_view::refresh()
{
    lds_query_db query;
    const QString view_state = refresh_state_cur();
    QString select_sql;
    //
    refreshView(view_state);
    ui->tableWidget->setContentSize(2, 3);
    kds::model->tablename_kvmap_delete_for_select_and_autoselect();
    //
    QString item_prepared_from;
    item_prepared_from = "from cey_u_orderdish_print_kt_table where vch_plan_id = '%1' and ch_state = '%2' ";
    item_prepared_from = item_prepared_from.arg(kds::cey_bt_kitchen_view_data("vch_plan_id").toString()).arg(KDS::cookPrepared);
    QString item_prepared_warn_from;
    item_prepared_warn_from = "from cey_u_orderdish_print_kt_table where vch_plan_id = '%1' and ch_state = '%2' and dt_operdate <= '%3' ";
    item_prepared_warn_from = item_prepared_warn_from.arg(kds::cey_bt_kitchen_view_data("vch_plan_id").toString()).arg(KDS::cookPrepared).arg(kds::currentDateTime().addSecs(-60 * lds::sysconf->value("kds_manager/order_warn_mins", 5).toInt()).toString(yyyyMMddhhmmss));
    QString item_cooking_from;
    item_cooking_from = "from cey_u_orderdish_print_kt_table where vch_plan_id = '%1' and ch_state = '%2' ";
    item_cooking_from = item_cooking_from.arg(kds::cey_bt_kitchen_view_data("vch_plan_id").toString()).arg(KDS::cookCooking);
    QString item_finished_from = "from cey_u_orderdish_print_kt_table where vch_plan_id in " + kds::sql_plan_service_in() + " and int_finish > 0 ";
    if(view_state == "item_prepared") {
        select_sql = "select ch_billno, ch_state, dt_operdate " + item_prepared_from + " limit 0, 6";
    }
    if(view_state == "item_prepared_warn") {
        select_sql = "select ch_billno, ch_state, dt_operdate " + item_prepared_warn_from + " limit 0, 6";
    }
    if(view_state == "item_cooking") {
        select_sql = "select ch_billno, ch_state, dt_operdate " + item_cooking_from + " limit 0, 6";
    }
    if(lds::widgetIsShow(ui->tableWidget)) {
        query.execSelect(select_sql);
        for(int k = 0; k < ui->tableWidget->count() && query.next(); k ++) {
            ui->tableWidget->setText(
                        k / ui->tableWidget->columnCount(),
                        k % ui->tableWidget->columnCount(),
                        kds::btn_detail_json(query.recordValue("ch_billno").toString(), kds::cey_bt_kitchen_view_data("vch_plan_id").toString(), KDS::cookFinished, k + 1));
        }
    }

    //item_group
    if(lds::widgetIsShow(ui->tableView_group)) {
        model_group->sql_cur =
                " select num_num - num_back - num_finish as num_dish, ch_dishno, num_cook from cey_u_orderdish_print_kt_dish "
                " where vch_plan_id = " + kds::sql_string_equal(kds::cey_bt_kitchen_view_data("vch_plan_id").toString()) + " and num_num - num_back - num_finish > 0";
        model_group->toinit();
        if(lds::widgetIsShow(ui->frame_take_brief))
            tableview_select_first(model_group, ui->tableView_group);
    }
    //
    if(lds::widgetIsShow(ui->label_num_prepare))
        ui->label_num_prepare->setText(kds::selectValue("select count(0) " + item_prepared_from).toString());
    if(lds::widgetIsShow(ui->label_num_prepare_warn))
        ui->label_num_prepare_warn->setText(kds::selectValue("select count(0) " + item_prepared_warn_from).toString());
    if(lds::widgetIsShow(ui->label_num_cooking))
        ui->label_num_cooking->setText(kds::selectValue("select count(0) " + item_cooking_from).toString());
    if(lds::widgetIsShow(ui->label_num_service))
        ui->label_num_service->setText(kds::selectValue("select count(0) " + item_finished_from).toString());

    if(lds::widgetIsShow(ui->tableView_service)) {
        model_service->sql_cur = "select ch_billno, concat(ch_billno, vch_plan_id) as bill_plan, vch_plan_id from cey_u_orderdish_print_kt_table where vch_plan_id in " + kds::sql_plan_service_in() + " and ch_state = " + QString::number(KDS::cookFinished);//
        model_service->toinit();
    }
    if(lds::widgetIsShow(ui->tableView_take_table)) {
        model_take_table->sql_cur = "select ch_billno, concat(ch_billno, vch_plan_id) as bill_plan, vch_plan_id from cey_u_orderdish_print_kt_table where vch_plan_id in " + kds::sql_plan_service_in() + " and int_finish > 0 ";//
        model_take_table->toinit();
        tableview_select_first(model_take_table, ui->tableView_take_table);
    }
    //
    kdsddsend();
}

void kds_main_view::refreshView()
{
    refreshView(refresh_state_cur());
}

void kds_main_view::refreshView(const QString &state)
{
    QString kt_kitchen_type = kds::f_get_sysparm("kt_kitchen_type", "kitchen_print");//kitchen_view
    QString kt_kitchen_shop = kds::f_get_sysparm("kt_kitchen_shop", "fastfdfood");//restaurant
    QString vch_kitchen_type = kds::cey_bt_kitchen_view_data("vch_kitchen_type").toString();//kitchen,service
    resetGeometry(state);
    viewPrepareHide();
    if(state == "item_prepared"
            || state == "item_prepared_warn"
            || state == "item_cooking") {
        viewShowWidget(ui->tableView_group);
        viewShowWidget(ui->tableWidget);
        viewShowWidget(ui->frame_bottom);
        viewShowWidget(ui->label_btn_arrow);
    }
    if(state == "item_finished_serial") {
        if(kt_kitchen_shop == "fastfdfood") {
            viewShowWidget(ui->tableView_service);
            viewShowWidget(ui->frame_key);
            viewShowWidget(ui->frame_bottom);
            viewShowWidget(ui->label_btn_arrow);
            ui->lineEdit_serialno->setFocus();
        }
        if(kt_kitchen_shop == "restaurant") {
            viewShowWidget(ui->tableView_take_table);
            viewShowWidget(ui->frame_take_brief);
            viewShowWidget(ui->label_btn_arrow);
        }
    }
    if(state == "item_login") {
        if(lds::sysconf->value("kds_login/db_host_name").toString().split(".").count() != 4) {
            lds::sysconf->setValue("kds_login/toauto_login", false);
        }
        lds_messagebox_loading_show loading(this, "", QObject::tr("正在连接数据库...") + ":" + lds::sysconf->value("kds_login/db_host_name").toString());
#ifdef QT_DEBUG
        loading.widget()->move(1000, 0);
#endif
        loading.show_delay();
        if(false == lds::sysconf->value("kds_login/toauto_login", false).toBool()) {
            viewShowWidget(ui->frame_login);
        } else if(false == ui->frame_login->login()) {
            viewShowWidget(ui->frame_login);
        } else {
            /*登录成功*/
        }
    }
    if(state == "item_brief") {
        viewShowWidget(ui->tableView_group);
        viewShowWidget(ui->frame_take_brief);
        viewShowWidget(ui->frame_bottom);
        viewShowWidget(ui->pushButton_main_exit);
        viewShowWidget(ui->pushButton_main_prepare);
        viewShowWidget(ui->pushButton_main_prepare_warn);
        viewShowWidget(ui->pushButton_main_cooking);
        viewShowWidget(ui->pushButton_main_service);
        viewShowWidget(ui->label_num_prepare);
        viewShowWidget(ui->label_num_prepare_warn);
        viewShowWidget(ui->label_num_cooking);
        viewShowWidget(ui->label_num_service);
        viewShowWidget(ui->label_btn_arrow);
    }

    if(kt_kitchen_shop == "fastfdfood" && vch_kitchen_type == "kitchen") {
        viewShowWidget(ui->frame_bottom);
        viewShowWidget(ui->pushButton_main_prepare, true);
        viewShowWidget(ui->pushButton_main_prepare_warn, false);
        viewShowWidget(ui->pushButton_main_cooking, false);
        viewShowWidget(ui->pushButton_main_service, false);
        viewShowWidget(ui->pushButton_main_exit, true);
        viewShowWidget(ui->label_num_prepare, true);
        viewShowWidget(ui->label_num_prepare_warn, false);
        viewShowWidget(ui->label_num_cooking, false);
        viewShowWidget(ui->label_num_service, false);
    }
    if(kt_kitchen_shop == "fastfdfood" && vch_kitchen_type == "service") {
        viewShowWidget(ui->frame_bottom);
        viewShowWidget(ui->pushButton_main_prepare, false);
        viewShowWidget(ui->pushButton_main_prepare_warn, false);
        viewShowWidget(ui->pushButton_main_cooking, false);
        viewShowWidget(ui->pushButton_main_service, true);
        viewShowWidget(ui->pushButton_main_exit, true);
        viewShowWidget(ui->label_num_prepare, false);
        viewShowWidget(ui->label_num_prepare_warn, false);
        viewShowWidget(ui->label_num_cooking, false);
        viewShowWidget(ui->label_num_service, true);
    }
    if(kt_kitchen_shop == "restaurant" && vch_kitchen_type == "kitchen") {
        viewShowWidget(ui->frame_bottom);
        viewShowWidget(ui->pushButton_main_prepare, true);
        viewShowWidget(ui->pushButton_main_prepare_warn, true);
        viewShowWidget(ui->pushButton_main_cooking, true);
        viewShowWidget(ui->pushButton_main_service, false);
        viewShowWidget(ui->pushButton_main_exit, true);
        viewShowWidget(ui->label_num_prepare, true);
        viewShowWidget(ui->label_num_prepare_warn, true);
        viewShowWidget(ui->label_num_cooking, true);
        viewShowWidget(ui->label_num_service, false);
    }
    if(kt_kitchen_shop == "restaurant" && vch_kitchen_type == "service") {
        viewShowWidget(ui->frame_bottom);
        viewShowWidget(ui->pushButton_main_prepare, false);
        viewShowWidget(ui->pushButton_main_prepare_warn, false);
        viewShowWidget(ui->pushButton_main_cooking, false);
        viewShowWidget(ui->pushButton_main_service, true);
        viewShowWidget(ui->pushButton_main_exit, true);
        viewShowWidget(ui->label_num_prepare, false);
        viewShowWidget(ui->label_num_prepare_warn, false);
        viewShowWidget(ui->label_num_cooking, false);
        viewShowWidget(ui->label_num_service, true);
    }
    viewExecHide();
    this->setFocus();
}

void kds_main_view::resetGeometry()
{
    resetGeometry(refresh_state_cur());
}

void kds_main_view::resetGeometry(const QString &state)
{
    this->resize(lds::MAIN_WINDOW_SIZE);
    int button_width = (this->width() - 10 - 10 * 5) /5;
    int cell_width = this->width() / 8;
    int center_h = this->height() - 80;

    ui->frame_bottom->setGeometry(0, this->height() - 80, this->width(), 80);
    ui->tableWidget->setGeometry(0, 0, cell_width * 6, center_h - 0);
    ui->tableView_group->setGeometry(cell_width * 6 - 10, 0, cell_width * 2 + 10, center_h);//tableView_group 和 tableWidget 有10像素的重叠
    ui->tableView_service->setGeometry(QRect(0, 0, cell_width * 5, center_h).adjusted(10, 10, -10, -10));
    ui->frame_key->setGeometry(QRect(cell_width * 5, 0, cell_width * 3, center_h).adjusted(0, 10, -10, -10));
    QRect rect = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QSize(ui->frame_key->width() - 20, ui->frame_key->width() - 20 + 50 + 70), ui->frame_key->rect());
    ui->lineEdit_serialno->setGeometry(QRect(rect.x(), rect.y(), rect.width(), 70));
    ui->frame_serialno_key->setGeometry(QRect(rect.x(), rect.y() + 70 + 50, rect.width(), rect.width()));
    ui->tableView_take_table->setGeometry(ui->tableView_group->geometry());
    ui->frame_take_brief->setGeometry(ui->tableWidget->geometry().adjusted(0, 10, -0, -10));
    ui->frame_login->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QSize(340, 420), this->rect()));

    ui->pushButton_main_prepare->setGeometry(QRect(10, 10, button_width, 60));
    ui->pushButton_main_prepare_warn->setGeometry(ui->pushButton_main_prepare->geometry().translated(10 + button_width, 0));
    ui->pushButton_main_cooking->setGeometry(ui->pushButton_main_prepare_warn->geometry().translated(10 + button_width, 0));
    ui->pushButton_main_service->setGeometry(ui->pushButton_main_cooking->geometry().translated(10 + button_width, 0));
    ui->pushButton_main_exit->setGeometry(ui->pushButton_main_service->geometry().translated(10 + button_width, 0));

    if(state == "item_prepared")
        ui->label_btn_arrow->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QSize(24, 12), QRect(ui->pushButton_main_prepare->x(), 0, ui->pushButton_main_prepare->width(), 12)));
    if(state == "item_prepared_warn")
        ui->label_btn_arrow->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QSize(24, 12), QRect(ui->pushButton_main_prepare_warn->x(), 0, ui->pushButton_main_prepare_warn->width(), 12)));
    if(state == "item_cooking")
        ui->label_btn_arrow->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QSize(24, 12), QRect(ui->pushButton_main_cooking->x(), 0, ui->pushButton_main_cooking->width(), 12)));
    if(state == "item_finished_serial")
        ui->label_btn_arrow->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QSize(24, 12), QRect(ui->pushButton_main_service->x(), 0, ui->pushButton_main_service->width(), 12)));

    ui->label_num_prepare->setup(ui->pushButton_main_prepare, QSize(40, 40), QColor(kds::color_prepared), 10, 10);
    ui->label_num_prepare_warn->setup(ui->pushButton_main_prepare_warn, QSize(40, 40), QColor(kds::color_prepared_warn), 10, 10);
    ui->label_num_cooking->setup(ui->pushButton_main_cooking, QSize(40, 40), QColor(kds::color_cooking), 10, 10);
    ui->label_num_service->setup(ui->pushButton_main_service, QSize(40, 40), QColor(kds::color_finished), 10, 10);
}

void kds_main_view::servicetoken(const QModelIndex &index)
{
    QString bill_no = index.data(Qt::UserRole).toString();
    QString bill_plan = index.data(Qt::UserRole + 1).toString();
    QString vch_plan_id = index.data(Qt::UserRole + 2).toString();
    servicetoken(bill_no, vch_plan_id);
}

void kds_main_view::servicetoken(const QString &ch_billno, const QString &vch_plan_id)
{
    QString errstring;
    lds_query_db query;
    query.db.transaction();
    query.execSelect(QString(" select int_flowID from cey_u_orderdish_print_kt_order where ch_billno = '%1' and vch_plan_id ='%2' and ch_state = %3  ")
                     .arg(ch_billno)
                     .arg(vch_plan_id)
                     .arg(KDS::cookFinished)
                     );
    while(query.next()) {
        if(!kds::update_print_kt_dish(query.recordValue("int_flowID").toString(), KDS::cookToken, errstring)) {
            query.db.rollback();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
            return;
        }
    }
    query.db.commit();
    model_service->refreshcur();
}

QVariant kds_main_view::varMin(const QVariant &p1, int p2)
{
    if(!p1.isValid()) {
        return p2;
    }
    return qMin(p1.toInt(), p2);
}

QVariant kds_main_view::varMin(const QVariant &p1, QDateTime p2)
{
    if(!p1.isValid()) {
        return p2;
    }
    return qMin(p1.toDateTime(), p2);
}

void kds_main_view::model_tablename_kvmap_init(const QString &tablename)
{
    kds::model->tablename_kvmap_create(tablename);
    kds::model->tablename_kvmap_delete(tablename);
}

void kds_main_view::setGeometrys(QWidget *w1, QWidget *w2, const QRect &rect)
{
    w1->setGeometry(rect);
    w2->setGeometry(rect);
}

QString kds_main_view::refresh_state_push(const QString &refresh_state)
{
    QString old = kds::model->tablename_kvmap_value("cur_parameter", "refresh_state").toString();
    if(old == refresh_state)
        return refresh_state;
    kds::model->tablename_kvmap_insert_value("cur_parameter", "refresh_state_old", old);
    kds::model->tablename_kvmap_insert_value("cur_parameter", "refresh_state", refresh_state);
    return refresh_state;
}

QString kds_main_view::refresh_state_cur()
{
    return kds::model->tablename_kvmap_value("cur_parameter", "refresh_state").toString();
}

QString kds_main_view::refresh_state_old()
{
    return kds::model->tablename_kvmap_value("cur_parameter", "refresh_state_old").toString();
}

QString kds_main_view::refresh_state_left()
{
    if(lds::widgetIsShow(ui->pushButton_main_prepare))
        return "item_prepared";
    if(lds::widgetIsShow(ui->pushButton_main_prepare_warn))
        return "item_prepared_warn";
    if(lds::widgetIsShow(ui->pushButton_main_cooking))
        return "item_cooking";
    if(lds::widgetIsShow(ui->pushButton_main_service))
        return "item_finished_serial";
    return "";
}

QString kds_main_view::setUpdateTime(const QString &dt)
{
    lds::sysconf->setValue("kds_parameter/db_update_time", dt);
    return dt;
}

QString kds_main_view::curUpdateTime()
{
    return lds::sysconf->value("kds_parameter/db_update_time").toString();
}

QString kds_main_view::setParamerTime(const QString &dt)
{
    conf_parameter_set("db_parameter_time", dt);
    return dt;
}

QString kds_main_view::curParamerTime()
{
    return conf_parameter_data("db_parameter_time").toString();
}

QVariant kds_main_view::conf_parameter_data(const QString &field_name)
{
    return lds::sysconf->value("kds_parameter/" + field_name);
}

void kds_main_view::conf_parameter_set(const QString &field_name, const QVariant &value)
{
    lds::sysconf->setValue("kds_parameter/" + field_name, value);
}

void kds_main_view::viewPrepareHide()
{
    if(view_show_widget_map.count() == 0) {
        foreach(QObject *o, this->children()) {
            QWidget *w = qobject_cast<QWidget *>(o);
            if(w)
                view_show_widget_map[w] = true;
        }
        foreach(QObject *o, ui->frame_bottom->children()) {
            QWidget *w = qobject_cast<QWidget *>(o);
            if(w)
                view_show_widget_map[w] = true;
        }
    }
    for(QMap<QWidget*, bool>::iterator k = view_show_widget_map.begin(); k != view_show_widget_map.end(); k ++) {
        k.value() = false;
    }
}

void kds_main_view::viewShowWidget(QWidget *w, bool show)
{
    view_show_widget_map[w] = show;
    lds::showWidget(w, show);
}

void kds_main_view::viewExecHide()
{
    for(QMap<QWidget*, bool>::iterator k = view_show_widget_map.begin(); k != view_show_widget_map.end(); k ++) {
        if(k.value() == false) {
            viewShowWidget(k.key(), false);
        }
    }
}

void kds_main_view::fileAppend(const QString &str)
{
    QFile file("kds_log.txt");
    file.open(QFile::WriteOnly | QFile::Append);
    file.write(str.toLocal8Bit() + "\n");
    file.close();
}

void kds_main_view::fileClear()
{
    QFile file("kds_log.txt");
    file.open(QFile::WriteOnly);
    file.write("");
    file.close();
}

void kds_main_view::tableview_select_index_default_first(const QModelIndex &index, kds_tableview *tableview)
{
    if(index.data(Qt::UserRole + 1).toString() == "") {
        tableview_select_first(tableview->model(), tableview);
    } else {
        tableview->setCurrentIndex(index);
        tableview->emitClicked(index);
    }
}

void kds_main_view::tableview_select_first(const QAbstractItemModel *model, kds_tableview *tableview)
{
    QModelIndex idx = model->index(0, 0);
    QString k = model->data(idx, Qt::UserRole + 1).toString();
    if("-1" == k) {
        idx = model->index(1, 0);
        k = model->data(idx, Qt::UserRole + 1).toString();
    }
    if("" != k) {
        tableview->setCurrentIndex(idx);
        tableview->emitClicked(idx);
    } else {
        QList<kds::billPlanDetail> d;
        ui->frame_take_brief->setIndexJson(kds::btn_detail_json(d), "");
    }
}

void kds_main_view::kdsddsend()
{
    lds_query_db query;
    QSet<QString> select_cooking_set;
    QSet<QString> select_service_set;
    //副屏
    kds::kdsddData dd;
    QString select_service = "select (select b.vch_memo from cey_u_master b where b.ch_billno = a.ch_billno) as serial_no from cey_u_orderdish_print_kt_table a where ";
    QString select_cooking = "select (select b.vch_memo from cey_u_master b where b.ch_billno = a.ch_billno) as serial_no from cey_u_orderdish_print_kt_table a where ";
    if("Y" == kds::cey_bt_kitchen_view_data("ch_screen_secondary").toString()) {
        //1快餐出品的副屏
        if(kds::is_fastfdfood_service()) {
            select_cooking += QString("  a.vch_plan_id in %1 and a.ch_state = %2 ; ").arg(kds::sql_plan_service_in()).arg(KDS::cookPrepared);
            select_service += QString("  a.vch_plan_id in %1 and a.ch_state = %2 ; ").arg(kds::sql_plan_service_in()).arg(KDS::cookFinished);
        }
        //2快餐厨显的副屏
        if(kds::is_fastfdfood_kitchen()) {
            select_cooking += QString("  a.vch_plan_id = '%1'  and a.ch_state = %2 ; ").arg(kds::cey_bt_kitchen_view_data("vch_plan_id").toString()).arg(KDS::cookPrepared);
            select_service += QString("  a.vch_plan_id = '%1'  and a.ch_state = %2 ; ").arg(kds::cey_bt_kitchen_view_data("vch_plan_id").toString()).arg(KDS::cookFinished);
        }
        //3中餐出品的副屏
        if(kds::is_restaurant_service()) {
            select_cooking += QString("  a.vch_plan_id in %1 and a.int_cook > 0 ; ").arg(kds::sql_plan_service_in());
            select_service += QString("  a.vch_plan_id in %1 and a.int_finish > 0 ; ").arg(kds::sql_plan_service_in());
        }
        //4中餐厨显的副屏
        if(kds::is_restaurant_kitchen()) {
            select_cooking += QString("  a.vch_plan_id = '%1' and a.int_cook > 0 ; ").arg(kds::cey_bt_kitchen_view_data("vch_plan_id").toString());
            select_service += QString("  a.vch_plan_id = '%1' and a.int_finish > 0 ; ").arg(kds::cey_bt_kitchen_view_data("vch_plan_id").toString());
        }
        query.execSelect(select_cooking);
        while(query.next())
            select_cooking_set << query.recordValue(0).toString();
        //
        query.execSelect(select_service);
        while(query.next())
            select_service_set << query.recordValue(0).toString();
        //
        dd.type = kds::kdsdd_request_setCookingService;
        dd.list_cooking = select_cooking_set.toList();
        dd.list_service = select_service_set.toList();
        kds::kdsdd_send(dd);
    }
}

void kds_main_view::keyPressEvent(QKeyEvent *e)
{
    QString errstring;
    QString barcode;
    int key = e->key();
    kds::writeLog("keyPress:" + QString::number(key));
    switch(key) {
    case Qt::Key_0:
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    case Qt::Key_6:
    case Qt::Key_7:
    case Qt::Key_8:
    case Qt::Key_9:{
        if(barcode_scan == 0) {

            barcode_scan = new QLineEdit(this);
            barcode_scan->setEnabled(false);
            barcode_scan->setGeometry((this->width() - 300) / 2, (this->height() - 40) / 2, 300, 40);
            barcode_scan->setStyleSheet(
                        "font:bold 20pt;"
                        "color:#333333;"
                        "border:0px;"
                        "background-color:peachpuff;");
        }
        barcode_scan->insert(QString::number(key - Qt::Key_0));
        lds::showWidget(barcode_scan);
    }
        break;
    case Qt::Key_Backspace:
        if(barcode_scan != 0) barcode_scan->backspace();
        break;
    case Qt::Key_Period:
        if(barcode_scan != 0) barcode_scan->insert(".");
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if(barcode_scan != 0) {
            barcode = barcode_scan->text().trimmed();
            barcode_scan->clear();
            lds::hideWidget(barcode_scan);
            if(barcode.startsWith(kds::print_flag_cook)) {
                barcode = barcode.mid(2);
                lds_query_db query;
                kds::writeLog("begin:" + barcode);
                query.db.transaction();
                if(false == kds::update_print_kt_dish(barcode, KDS::cookToken, errstring)) {
                    query.db.rollback();
                    kds::writeLog("rollback");
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring, true);
                    break;
                }
                query.db.commit();
                kds::writeLog("commit");
                torefreshsingle();
            }
        }
    case Qt::Key_Escape:
        break;
    }

    QDialog::keyPressEvent(e);
}

void kds_main_view::kds_main_view_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    kds::paintTableBrief(painter, index, option.rect);
}

void kds_main_view::kds_main_service_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString bill_no = index.data(Qt::UserRole).toString();
    QString bill_plan = index.data(Qt::UserRole + 1).toString();
    QString vch_plan_id = index.data(Qt::UserRole + 2).toString();
    QString serial_no = kds::model->tablename_kvmap_value("cey_u_master", bill_no, "ch_billno", "vch_memo").toString();

    QRect rect = option.rect;
    painter->fillRect(rect, QColor(18, 26, 34));
    rect = lds::indexRect(rect, index, 2, 0);
    painter->fillRect(rect, QColor(37, 48, 65));
    if(serial_no == "-1") {
        QPixmap p(":/image/widgets_pic/arrow_white_up.png");
        p = p.scaledToHeight(rect.height() * 0.6);
        painter->drawPixmap(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, p.size(), rect), p);
    } else if(serial_no == "-2") {
        QPixmap p(":/image/widgets_pic/arrow_white_down.png");
        p = p.scaledToHeight(rect.height() * 0.6);
        painter->drawPixmap(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, p.size(), rect), p);
    } else {
        painter->drawText(rect, Qt::AlignVCenter | Qt::AlignHCenter, serial_no);
    }
}

kds_main_view_table_brief::kds_main_view_table_brief(const QByteArray &json, QWidget *parent) : QDialog(parent)
{
    this->json = json;
}

kds_main_view_table_brief::~kds_main_view_table_brief()
{
    qDebug() << __FUNCTION__;
}

void kds_main_view_table_brief::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    cJSON_path p(json);
    if(p.isValid()) {
        p.replaceItemValue("", "row_count", this->height() / 40);//include title row
        kds::paintTableBrief(&painter, p.toByteArray(), this->rect());
    }
}


void kds_main_view::kds_main_group_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString dish_qty = index.data(Qt::UserRole).toString();
    QString dish_no = index.data(Qt::UserRole + 1).toString();
    QString dish_cook = index.data(Qt::UserRole + 2).toString();
    QString dish_prepare = QString::number(dish_qty.toDouble() - dish_cook.toDouble());
    QString dish_name = kds::model->tablename_kvmap_value("cey_bt_dish", dish_no, "ch_dishno", "vch_dishname").toString();
    QRect rect = option.rect;
    painter->setPen(QColor("white"));
    painter->fillRect(rect, QColor(18, 26, 34));
    rect = lds::indexRect(rect, index, 10);
    painter->fillRect(rect, QColor(37, 48, 65));
    painter->setRenderHint(QPainter::Antialiasing);

    int ellipse_h = rect.height() / 2;
    if(option.state & QStyle::State_Selected) {
        painter->fillRect(rect, QColor(kds::color_prepared_warn));
    }
    if(dish_no == "-1") {
        QPixmap p(":/image/widgets_pic/arrow_white_up.png");
        p = p.scaledToHeight(rect.height() * 0.7);
        painter->drawPixmap(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, p.size(), option.rect), p);
    } else if(dish_no == "-2") {
        QPixmap p(":/image/widgets_pic/arrow_white_down.png");
        p = p.scaledToHeight(rect.height() * 0.7);
        painter->drawPixmap(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, p.size(), option.rect), p);
    } else if(dish_no.count() > 0) {
        painter->setFont(pixelFont(painter, ellipse_h * 0.7));
        painter->drawText(rect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, dish_name);
        int xoff = 0;
        painter->setFont(pixelFont(painter, ellipse_h - 4));
        if(dish_cook.toDouble() > 0)
        {
            int cell_width = textWidth(painter, dish_cook);
            cell_width = qMax(ellipse_h, cell_width);
            xoff += cell_width;
            QRect ellipse_r = QRect(rect.width() - xoff, ellipse_h, cell_width, ellipse_h).translated(rect.x(), rect.y()).adjusted(2, 2, -2, -2);
            painter->setBrush(QColor(kds::color_cooking));
            painter->setPen(QPen(QColor(255, 255, 255, 120), 1));
            painter->drawRoundedRect(ellipse_r, ellipse_r.height() / 2, ellipse_r.height() / 2);
            painter->setPen(QColor("white"));
            painter->drawText(ellipse_r, Qt::AlignHCenter | Qt::AlignVCenter, dish_cook);
        }
        if(dish_prepare.toDouble() > 0)
        {
            int cell_width = textWidth(painter, dish_prepare);
            cell_width = qMax(ellipse_h, cell_width);
            xoff += cell_width;
            QRect ellipse_r = QRect(rect.width() - xoff, ellipse_h, cell_width, ellipse_h).translated(rect.x(), rect.y()).adjusted(2, 2, -2, -2);
            painter->setBrush(QColor(kds::color_prepared));
            painter->setPen(QPen(QColor(255, 255, 255, 120), 1));
            painter->drawRoundedRect(ellipse_r, ellipse_r.height() / 2, ellipse_r.height() / 2);
            painter->setPen(QColor("white"));
            painter->drawText(ellipse_r, Qt::AlignHCenter | Qt::AlignVCenter, dish_prepare);
        }
    }
}

int kds_main_view::kds_main_group_delegate::textWidth(QPainter *p, const QString &text) const
{
    QFontMetrics fm(p->font());
    return fm.width(text) + 12;
}

QFont kds_main_view::kds_main_group_delegate::pixelFont(QPainter *p, int pixel) const
{
    QFont f = p->font();
    f.setPixelSize(pixel);
    return f;
}


void kds_main_view::kds_main_take_table_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString bill_no = index.data(Qt::UserRole).toString();
    QString bill_plan = index.data(Qt::UserRole + 1).toString();
    QString vch_plan_id = index.data(Qt::UserRole + 2).toString();
    QString table_no = kds::model->tablename_kvmap_value("cey_u_table", bill_no, "ch_billno", "ch_tableno").toString();
    QString table_name = kds::model->tablename_kvmap_value("cey_bt_table", table_no, "ch_tableno", "vch_tablename").toString();
    if(table_no == "Q000") {
        table_name = QObject::tr("快餐");
    }
    QRect rect = option.rect;
    painter->setPen(QColor("white"));
    painter->fillRect(rect, QColor(18, 26, 34));
    rect = lds::indexRect(rect, index, 10);
    painter->fillRect(rect, QColor(37, 48, 65));
    if(option.state & QStyle::State_Selected) {
        painter->fillRect(rect, QColor(kds::color_prepared_warn));
    }

    if(index.data(Qt::UserRole + 1).toString() == "-1") {
        QPixmap p(":/image/widgets_pic/arrow_white_up.png");
        p = p.scaledToHeight(rect.height() * 0.7);
        painter->drawPixmap(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, p.size(), option.rect), p);
    } else if(index.data(Qt::UserRole + 1).toString() == "-2") {
        QPixmap p(":/image/widgets_pic/arrow_white_down.png");
        p = p.scaledToHeight(rect.height() * 0.7);
        painter->drawPixmap(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, p.size(), option.rect), p);
    } else if(bill_no.count() > 0) {
        painter->save();
        painter->drawText(rect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, table_name);
        painter->setFont(kds::fontScaled(painter->font(), 0.8));
        painter->drawText(rect, Qt::AlignRight | Qt::AlignBottom, bill_no);
        painter->restore();
    }
}
