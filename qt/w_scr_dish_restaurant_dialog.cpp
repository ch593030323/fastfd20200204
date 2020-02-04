#include "w_scr_dish_restaurant_dialog.h"
#include "lds_query_hddpos.h"
#include <QSqlRecord>
#include <QSqlError>
#include "backheader.h"
#include "lds_messagebox.h"
#include "n_func.h"
#include "lock_screen.h"
#include "lds_menu.h"
#include "table_lock.h"
#include "w_scr_dish.h"
#include <QElapsedTimer>
#include <QTimeLine>
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "lds_pushbuttonlock.h"
#include "w_scr_dish_switch_detail.h"
#include "w_scr_dish_m_hang_pay_dialog.h"
#include <QPainter>
#include "w_scr_dish_restaurant_preorder.h"
#include "ftableview_standmodel_sql_table.h"
#include "ftableview_delegate.h"
#include "public_printby_ch_billno.h"
#include "ftableview_standmodel_sql_none.h"
#include "w_scr_dish_restaurant_table_transfer.h"
#include "w_scr_dish_restaurant_table_copy.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "w_scr_dish_restaurant_table_cmb.h"
#include "w_scr_dish_pay_widget.h"
#include "lds.h"
#include "w_sys_manage_wirelessordermonitoring.h"
#include "w_scr_dish_restaurant_table_dish_transfer.h"
#include "w_bt_dish_kitchen_dish_state.h"
#include "w_bt_dish.h"
#include "w_rpt_report_frame_business_summary.h"
#include "w_m_member_list.h"
#include "modify_password.h"
#include "business_documents_query.h"
#include "w_scr_dish_restaurant_dishinfo.h"
#include "w_scr_dish_restaurant_parameter.h"
#include "w_scr_dish_guqing_report.h"
#include "pointerautodel.h"
#include "w_scr_dish_language_switch_set.h"
#include "w_bt_dish_table.h"
#include "fexpandmain_2.h"
#include "w_scr_dish_main_table_800x600.h"
#include "btnlanguageswicth.h"
#include "table_management_desk.h"
#include "table_management_graphicsview.h"
#include "public_pushbuton_rt_check.h"
#include "lds_roundeddialog_title_frame.h"
#include "lds_widgetgoing.h"
#include "ui_w_scr_dish_restaurant_dialog.h"
#include "w_m_member_telcoming.h"
#include "restraurantdelegate_list_table.h"
#include "cjson_help.h"
#include "w_scr_dish_main_qr_code_over_view.h"
#include "w_scr_dish_main_bar_cancel.h"

w_scr_dish_restaurant_dialog::w_scr_dish_restaurant_dialog(QWidget *parent) :
    lds_roundeddialog_rect0(parent)
{
    lds::setwflagFrameless(this);
    ui = new Ui_w_scr_dish_restaurant_dialog;
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);
    ordertable_msg_loop = -1;//第一次进入为-1， 确保不会出现二次弹窗

    qr_code_polliing_count = -1;//第一次不刷先新
    qr_code_polling_has_pop = false;
    //====================

    ui->pushButton_map_model_edit_and_save->setFocusPolicy(Qt::NoFocus);
    lds::hideWidget(ui->pushButton_map_model_edit_and_save);
    pushButton_map_model_edit_and_save_popup.setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    ui->frame_top->move(0, 0);
    ui->frame_top->setFixedSize(lds::MAIN_WINDOW_SIZE.width(), lds::QT_FIXED_HEIGHT_BOTTOM);

    int left_space = 120;
    if(lds::MAIN_WINDOW_SIZE.width()==1920) {
        left_space = 200;
    }
    int frame_area_margin = 9;

    ui->tableView_3->verticalHeader()->setDefaultSectionSize(20);
    ui->tableView_3->setFixedHeight(20 * restraurantDelegate::getTablestatelist().count());
    QRect rect_main = QRect(0, lds::QT_FIXED_HEIGHT_BOTTOM, lds::MAIN_WINDOW_SIZE.width(), lds::MAIN_WINDOW_SIZE.height() - lds::QT_FIXED_HEIGHT_BOTTOM);
    QSize size_area = QSize(left_space, rect_main.height());
    QSize size_view = QSize(lds::MAIN_WINDOW_SIZE.width() - left_space, rect_main.height());

    ui->frame_area->setGeometry(QStyle::alignedRect(this->layoutDirection(), Qt::AlignLeft, size_area, rect_main));
    //    ui->frame_area->setGeometry(0, lds::QT_FIXED_HEIGHT_BOTTOM, left_space, lds::MAIN_WINDOW_SIZE.height() - lds::QT_FIXED_HEIGHT_BOTTOM);
    ui->frame_area->layout()->setContentsMargins(frame_area_margin, frame_area_margin, frame_area_margin, frame_area_margin);

    ui->widget_switch->setFixedHeight(29);
    if(lds::MAIN_WINDOW_SIZE.width()==1920) {
        ui->widget_switch->setFixedHeight(40);
    }
    ui->pushButton_table->setGeometry(1, 1, (ui->frame_area->width() - frame_area_margin * 2 - 4) / 3, ui->widget_switch->height()  - 2);
    ui->pushButton_list->setGeometry(ui->pushButton_table->geometry().right() + 2, 1, ui->pushButton_table->width(), ui->pushButton_table->height());
    ui->pushButton_map->setGeometry(ui->pushButton_list->geometry().right() + 2, 1, ui->pushButton_table->width(), ui->pushButton_table->height());
#ifdef QT_DEBUG
#else
    lds::hideWidget(ui->tableView_print_state);
    lds::hideWidget(ui->pushButton_print_state_hheader);
#endif
    ui->frame_view_table->setGeometry(QStyle::alignedRect(this->layoutDirection(), Qt::AlignRight, size_view, rect_main));
    //    ui->frame_view_table->setGeometry(left_space, lds::QT_FIXED_HEIGHT_BOTTOM, lds::MAIN_WINDOW_SIZE.width() - left_space, lds::MAIN_WINDOW_SIZE.height() - lds::QT_FIXED_HEIGHT_BOTTOM);
    ui->frame_top->setTitleColor(&titleColor);
    lds::hideWidget(ui->frame_manager_view);
    //    lds::hideWidget(ui->frame);
    lds::hideWidget(ui->graphicsView);
    lds::hideWidget(ui->tableView_ch_tableno);
    map_mode_switch();
    //
    //
    {
        lds_menu *menu=new lds_menu(this);
        QWidgetAction *widgetaction=new QWidgetAction(ui->pushButton_set);
        lds::showWidget(ui->frame_manager_view);
        widgetaction->setDefaultWidget(ui->frame_manager_view);
        menu->addAction(widgetaction);
        ui->pushButton_set->setMenu(menu);
    }
    //微信功能
    //    {
    //        lds_menu *menu=new lds_menu(this);
    //        QWidgetAction *widgetaction=new QWidgetAction(ui->pushButton_other);
    //        lds::showWidget(ui->frame);
    //        widgetaction->setDefaultWidget(ui->frame);
    //        menu->addAction(widgetaction);
    //        ui->pushButton_other->setMenu(menu);
    //    }
    modeltable = new ftableview_standmodel_sql_table;
    ui->tableView_ch_tableno->setModel(modeltable);
    ui->tableView_list_table->setModel(modeltable);
    modeltable_list_dish = new lds_model_sqlstandardmodel(this);
    standmodel_print_state = new QStandardItemModel(this);
    modeltype = new ftableview_standmodel_sql;
    ui->tableView_ch_areano->setModel(modeltype);
    //
    connect(ui->pushButton_switchback,SIGNAL(clicked()),this,SLOT(toswitchback()));
    connect(ui->pushButton_mhangpay,SIGNAL(clicked()),this,SLOT(tomhangpay()));
    connect(ui->pushButton_mrecharge,SIGNAL(clicked()),this,SLOT(tomrecharge()));

    connect(ui->pushButton_business_status,SIGNAL(clicked()),this,SLOT(tobusiness_status()));
    connect(ui->pushButton_lockscreen,SIGNAL(clicked()),this,SLOT(tolockscreen()));
    connect(ui->pushButton_curtable_status,SIGNAL(clicked()),this,SLOT(tocurtable_status()));
    connect(ui->pushButton_businessdata_query,SIGNAL(clicked()),this,SLOT(tobusinessdata_query()));

    connect(ui->pushButton_pwdchange,SIGNAL(clicked()),this,SLOT(topwdchange()));
    connect(ui->pushButton_switch,SIGNAL(clicked()),this,SLOT(toswitch()));
    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(toexit()));
    //
    connect(ui->pushButton_waimaipay,SIGNAL(clicked()),this,SLOT(to_waimaipay()));

    connect(ui->pushButton_lockstate,SIGNAL(clicked()),this,SLOT(tolockstate()));
    connect(ui->pushButton_open,SIGNAL(clicked()),this,SLOT(toopencash()));
    //    connect(ui->pushButton_other,SIGNAL(clicked()),this,SLOT(toother()));
    connect(ui->pushButton_order,SIGNAL(clicked()),this,SLOT(topreorder()));
    connect(ui->pushButton_parameter,SIGNAL(clicked()),this,SLOT(toparameter()));

    connect(ui->pushButton_menu, SIGNAL(clicked()), this, SLOT(toFastfdMode()));
    connect(ui->pushButton_print_state_hheader, SIGNAL(clicked()),this,SLOT(toshowprintstate()));

    connect(ui->pushButton_map_model_edit_and_save, SIGNAL(clicked()),this, SLOT(to_mode_map_edit_and_save()));
    connect(ui->pushButton_guqing_report, SIGNAL(clicked()),this,SLOT(tosysguqing()));
    connect(ui->pushButton_language,SIGNAL(keyChanged()),this,SLOT(toLanguageSwitch()));
    connect(ui->pushButton_language,SIGNAL(timeout()),this,SLOT(toLanguageSet()));
    connect(ui->pushButton_wine, SIGNAL(clicked()), this, SLOT(toWine()));
    connect(ui->pushButton_tel_coming, SIGNAL(clicked()), this, SLOT(toTelComing()));

    connect(ui->pushButton_list, SIGNAL(signalpressed(bool)), this, SLOT(to_restaurant_mode_list(bool)));
    connect(ui->pushButton_table, SIGNAL(signalpressed(bool)), this, SLOT(to_restaurant_mode_table(bool)));
    connect(ui->pushButton_map, SIGNAL(signalpressed(bool)), this, SLOT(to_restaurant_mode_map(bool)));
    connect(ui->pushButton_map, SIGNAL(signalpressed(bool)), this, SLOT(to_map_mode_edit_and_save(bool)));
    connect(ui->tableView_list_table, SIGNAL(clicked(QModelIndex)), this, SLOT(to_mode_list_dish_click(QModelIndex)));
    connect(ui->tableView_list_table, SIGNAL(double_clicked(QModelIndex)), this, SLOT(totablemessage(QModelIndex)));

    connect(ui->graphicsView, SIGNAL(signal_table_deleted(QString)),this, SLOT(to_mode_table_deselect(QString)));
    connect(ui->graphicsView, SIGNAL(signal_table_clicked(QString)),this, SLOT(totablemessage(QString)));

    connect(ui->tableView_ch_tableno,SIGNAL(clicked(QModelIndex)),this,    SLOT(totablemessage(QModelIndex)));
    connect(ui->tableView_ch_tableno,SIGNAL(timeout(QModelIndex)), this,SLOT(toLongPressTableTimeOut(QModelIndex)));

    connect(ui->tableView_ch_areano,SIGNAL(signalclick(QString)),this,SLOT(totable_ch_areano(QString)));
    connect(ui->tableView_ch_areano,SIGNAL(signalclickNew(QModelIndex)),this,SLOT(toLongPressTableAreaNew(QModelIndex)));
    connect(ui->tableView_ch_areano,SIGNAL(timeout(QModelIndex)),this,SLOT(toLongPressTableAreaNew(QModelIndex)));
    //    connect(ui->pushButton_qr_code_order_over_view, SIGNAL(clicked()), this, SLOT(toqr_code_order_over_view()));

    ui->tableView_list_dish->setTransferHheader();
    ui->tableView_list_table->setTransferHheader();
    ui->tableView_list_table->horizontalHeader()->setStretchLastSection(true);
    lds::hideWidget(ui->pushButton_dishcopy);
    //
    QTimer::singleShot(100, this, SLOT(toFirstGoIn()));
    //    lds::hideWidget(ui->pushButton_qr_code_order_over_view);

}

w_scr_dish_restaurant_dialog::~w_scr_dish_restaurant_dialog()
{
    delete ui;
}

void w_scr_dish_restaurant_dialog::retranslateView()
{
    ui->retranslateUi(this);
    //
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ui->tableView_3->model());
    for(int row = 0; row < model->rowCount(); row++) {
        tableState state = (tableState)model->item(row, 0)->data().toInt();
        model->item(row, 0)->setText(restraurantDelegate::gettableState(state).key);
    }
    //
    ftableview_standmodel_sql *modeltype = static_cast<ftableview_standmodel_sql *>( ui->tableView_ch_areano->m );
    modeltype->sql_cur = modeltypeSql();
    modeltype->refreshcur();
}

bool w_scr_dish_restaurant_dialog::tablemessageOrder(QWidget *parent, const QString &ch_tableno, const QString &sql, bool is_div, const w_m_member_telComing_table::telComingData &member_from_tel, bool isRequestOrder)
{
    //    QrCodeMasterList qr_code_order_list;
    //    w_scr_dish_main_qr_code::QrCodePay qr_code_pay_data;
    //    QString vch_qr_code_sn;
    lds_query_hddpos  query;
    QString ch_billno;
    //    QString errstring;
    query.execSelect(sql + QString(" and y.ch_tableno = '%1' ").arg(ch_tableno)  );
    if(false == query.next()) {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("餐桌不存在"));
        return false;
    }
    QString order_tableno = query.recordValue("order_tableno").toString();
    QString ch_billnos = query.recordValue("ch_billnos").toString();
    QMap<int, QString> ch_billno_map = restraurantDelegate::get_ch_billno_div_map(ch_billnos);
    QStringList div_bar = query.recordValue("div_bar").toString().split(",", QString::SkipEmptyParts);
    lds_messagebox_loading_show loading(parent, ".", "WAITING...");
    QString vch_waiter;
    bool rect_is_bar = false;
    if(2 == lds::sysconf->value("system_setting/yingyemoshi", 0).toInt()) {//酒吧模式)
        rect_is_bar = restraurantDelegate::bar_is_div(div_bar);
    }
    int div_sqr = 0;
    if(is_div
            || restraurantDelegate::ch_billno_is_div(ch_billno_map)
            || rect_is_bar) {
        w_scr_dish_restaurant_div_pop dialog_div(parent);
        dialog_div.update_by_ch_billnos_ch_tableno(ch_billno_map, ch_tableno);
        dialog_div.enableSinglSelectedWhenMutil();
        dialog_div.enableItemFlagSelectedWithAll();
        if(QDialog::Accepted != transparentCenterDialog(&dialog_div).exec()) {
            return false;
        }
        QModelIndexList index_list = dialog_div.get_cur_check_index_list();
        if(index_list.isEmpty()) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("没有桌台被选中"));
            return false;
        }
        ch_billno = index_list[0].data(w_scr_dish_restaurant_div_pop::Model_ch_billno).toString();
        div_sqr = index_list[0].data(w_scr_dish_restaurant_div_pop::Model_sqr).toInt();
    } else {//原单号，未开台，分单开台，则读取分单的裁剪
        ch_billno = ch_billno_map.value(0);
    }

    if(ch_billno.isEmpty()) {//若是非单号
        ch_billno = lds_scr_table::get_max_ch_billno_add1("B");
    }
    //questionRequestOrder
    if(isRequestOrder) {
        if(1 == lds_messagebox::question(parent, MESSAGE_TITLE_VOID, QObject::tr("是否点单"), QObject::tr("是"), QObject::tr("否"))) {
            goto end;
        }
    }
    //锁台
    if(lds_scr_table::cey_bt_table_has_locked(parent, ch_tableno, div_sqr)) {
        goto end;
    }
    //3
    if(!order_tableno.isEmpty()) {
        if(1 == lds_messagebox::question(parent, MESSAGE_TITLE_VOID, QObject::tr("该餐台已经预订?"), QObject::tr("继续操作"), QObject::tr("退出"))) {
            goto end;
        }
    }

    //!等待
    loading.show();
    //!~等待

    //二维码点餐，为处理的需要继续付清
    if(ch_billno.startsWith("R"))
    {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("中餐下不处理二维码点餐信息"));
        goto end;
    }
    {
        //            qr_code_order_list = w_scr_dish_main_qr_code::qr_code_master_get(ch_tableno, w_scr_dish_main_qr_code::billTypeRCV);//1 83, 2
        //            w_scr_dish_main_qr_code::qr_code_list_print(qr_code_order_list);
        //            vch_qr_code_sn = lds_query_hddpos::selectValue(QString(" select vch_qr_code_sn from cey_u_table where ch_billno = '%1' ").arg(ch_billno)).toString();
        //            if(w_scr_dish_main_qr_code::qr_code_has_paid(
        //                        vch_qr_code_sn,
        //                        qr_code_pay_data, errstring)) {
        //                if(w_scr_dish_main_qr_code::qr_code_bill_do_pay_transaction(ch_tableno, ch_billno, qr_code_pay_data, errstring)) {
        //                    lds_messagebox::information(parent, MESSAGE_TITLE_VOID, QObject::tr("二维码点餐") + "\n" +
        //                                                QObject::tr("本单已支付") + "\n" +
        //                                                QObject::tr("餐桌") + ":" +ch_tableno+ "\n"  +
        //                                                qr_code_pay_data.information()
        //                                                );
        //                    return true;
        //                }
        //                lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, errstring);
        //                return false;
        //            }
        //        }

        //        w_scr_dish_main_qr_code_over_view dialog("", parent);
        //        dialog.selectOrderSn(vch_qr_code_sn);
        //        //        w_scr_dish_main_qr_code dialog(ch_tableno, qr_code_order_list, vch_qr_code_sn, parent);
        //        transparentCenterDialog(&dialog).exec();
        //        return true;
    }
    //4
    //有未处理订单,未开启的话不启用
    if(n_func::f_get_sysparm_q(query,"qr_code_order_mode","0") == "1") {
        //        qr_code_order_list = w_scr_dish_main_qr_code::qr_code_master_get(ch_tableno, w_scr_dish_main_qr_code::billTypeAVailable);
        //        if(qr_code_order_list.count() > 0) {
        //            //分单
        //            if(div_sqr != 0) {
        //                if(1 == lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("分单和导入订单无法同时进行"),  QObject::tr("继续点餐"), QObject::tr("退出"))) {
        //                    goto end;
        //                }
        //                goto orderdish;
        //            }
        //            if(false == w_scr_dish_main_qr_code::qr_code_can_order(ch_tableno, errstring)) {
        //                if(1 == lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("订单无法导入") + "\n" + errstring,  QObject::tr("继续点餐"), QObject::tr("退出"))) {
        //                    goto end;
        //                }
        //                goto orderdish;
        //            }
        //            switch(lds_messagebox::question(parent, MESSAGE_TITLE_VOID, QObject::tr("有订单未处理, 是否导入"), QObject::tr("导入"), QObject::tr("继续点餐"), QObject::tr("退出"))) {
        //            case 0:
        //            {
        //                w_scr_dish_main_qr_code_over_view dialog(w_scr_dish_main_qr_code::billTypeAVailable, parent);
        //                dialog.selectTableNo(ch_tableno);
        //                transparentCenterDialog(&dialog).exec();
        //                return true;
        //            }
        //                break;
        //            case 1:
        //                break;
        //            case 2:
        //                goto end;
        //            }
        //        }
    }
    //~有未处理订单

    //已经开台
    if(lds_query_hddpos::selectValue(query, QString(" select ch_billno from cey_u_table where ch_billno = '%1' ").arg(ch_billno)).toString().length() > 0) {
        goto orderdish;
    }
    if(n_func::f_get_sysparm("restrant_auto_lock_flag", "0") == "1") {
        lock_screen_servant dialog(parent);
        if(QDialog::Rejected ==  transparentCenterDialog(&dialog).exec()) {
            goto end;
        }
        vch_waiter = dialog.getOperator();
    }

    lds_query_hddpos::tran_saction();
    //num_cost不计入
    if(lds_scr_table::insert_cey_u_master_Create_serial_no(ch_billno)) {
        if(query.execInsert("cey_u_table",qrtVariantPairList()
                            << qrtVariantPair("ch_billno", ch_billno)
                            << qrtVariantPair("ch_tableno", ch_tableno)
                            << qrtVariantPair("int_person", lds_query_hddpos::selectValue(QString("select int_person from cey_bt_table where ch_tableno = '%1' ").arg(ch_tableno))) // select_table_dialog.ui->lineEdit_int_person->value())
                            << qrtVariantPair("ch_state", 1)
                            << qrtVariantPair("vch_operID", lds::gs_operid)

                            << qrtVariantPair("dt_operdate", qrtFunc("now()"))//select_table_dialog.ui->dateTimeEdit_dt_operdate->dateTime())
                            << qrtVariantPair("ch_order_state", 0)
                            << qrtVariantPair("vch_waiter", vch_waiter)
                            << qrtVariantPair("vch_memo", ""))) {
            if(lds_scr_table::cey_u_taxsale_master_insert_table(query, ch_billno)) {
                if(lds_scr_table::cey_bt_table_update(ch_tableno, ch_billno, div_sqr)) {
                    if(cey_bt_table_bar_replace(ch_tableno, div_sqr, ch_billno)) {
                        lds_query_hddpos::com_mit();
                        goto orderdish;
                    }
                }
            }
        }
    }
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("开台失败"));
    goto end;
orderdish:
    loading.hide();

    if(lds_scr_table::cey_bt_table_has_locked(parent, ch_tableno, div_sqr)) {
        goto end;
    }
    {
        //lock
        lds_scr_table::cey_bt_table_lock(ch_tableno, div_sqr);
        //~lock
        int ret;
        w_scr_dish_main_table_800x600 d(ch_tableno, ch_billno, parent, member_from_tel);
        ret = d.exec();
        //unlock
        if(QDialog::Accepted == ret) {//没有付款或付款失败/取消
            lds_scr_table::cey_bt_table_update(ch_tableno, "", div_sqr);
        }
        lds_scr_table::cey_bt_table_unlock(ch_tableno, div_sqr);
        query.execUpdate("cey_u_table", "num_cost", public_sql::getTotalBy_ch_billno(ch_billno), qrtEqual("ch_billno", ch_billno));
        //~unlock
    }
    if(n_func::f_get_sysparm("restrant_auto_lock_flag", "0") == "1") {
        public_sql::save_login->restrant_auto_lock_unique();
    }
    return true;
end:
    return false;
}

void w_scr_dish_restaurant_dialog::toFirstGoIn()
{
    manager = new QNetworkAccessManager(this);
    ///
    modeltype->enableEndAppendNew(n_func::f_get_sysparm("qt_forbide_change_new","0") == "0");
    modeltype->disablePageRecord();
    if(lds::MAIN_WINDOW_SIZE.width()==800) {
        ui->tableView_ch_areano->updateFixed_fillData(1, 4, 100, lds::QT_FIXED_HEIGHT_BOTTOM);
    } else if(lds::MAIN_WINDOW_SIZE.width()==1024) {
        ui->tableView_ch_areano->updateFixed_fillData(1, 4, 100, lds::QT_FIXED_HEIGHT_BOTTOM);
    } else if(lds::MAIN_WINDOW_SIZE.width()==1920) {
        ui->tableView_ch_areano->updateFixed_fillData(1, 6, 180, lds::QT_FIXED_HEIGHT_BOTTOM);
    } else {
        ui->tableView_ch_areano->updateFixed_fillData(1, 6, 100, lds::QT_FIXED_HEIGHT_BOTTOM);
    }
    ui->tableView_ch_areano->enableLongPress(n_func::f_get_sysparm("qt_forbide_change_new","0") == "0");
    modeltype->sql_cur = modeltypeSql();
    ftableview_delegate *d = new ftableview_delegate(ui->tableView_ch_areano);
    d->indexisEmptyDrawGrad = true;
    d->keepConnect(ui->tableView_ch_areano);
    ui->tableView_ch_areano->setItemDelegate(d);
    modeltype->toinit();
    ///
    modeltable->enableEndAppendNew(n_func::f_get_sysparm("qt_forbide_change_new","0") == "0");
    modeltable->disablePageRecord();

    ui->tableView_list_table->setItemDelegate(new restraurantDelegate_list_table(this));
    ui->tableView_list_table->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_list_dish->setModel(modeltable_list_dish);
    ui->tableView_list_dish->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_list_dish->setItemDelegate(new restraurantDelegate_list_dish(this));

    restraurantDelegate *delegatetable = new restraurantDelegate(ui->tableView_ch_tableno);
    ui->tableView_ch_tableno->setItemDelegate(delegatetable);
    ui->tableView_ch_tableno->enableLongPress(n_func::f_get_sysparm("qt_forbide_change_new","0") == "0");
    ui->tableView_ch_tableno->disConnectCenter();
    ///
    QStandardItemModel *standmodel_tablestate = new QStandardItemModel(ui->tableView_3);
    foreach(const tableSTATE &t, delegatetable->getTablestatelist()) {
        QStandardItem * item = new QStandardItem;
        item->setData(t.state);
        item->setData(restraurantDelegate::getPixmapByStateList(t.state), Qt::DecorationRole);
        standmodel_tablestate->appendRow(item);

        if(t.state == tableState_BAR
                && 2 != lds::sysconf->value("system_setting/yingyemoshi", 0).toInt()) {//酒吧模式
            standmodel_tablestate->removeRow(standmodel_tablestate->rowCount() - 1);
        }
    }
    ui->tableView_3->setShowGrid(false);
    ui->tableView_3->setModel(standmodel_tablestate);
    ui->tableView_3->setItemDelegate(new restraurantTableStateDelegate(this));
    ui->tableView_3->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_3->setFocusPolicy(Qt::NoFocus);
    ui->tableView_3->setSelectionMode(QTableView::NoSelection);
    ui->tableView_3->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableView_3->horizontalHeader()->setStretchLastSection(true);
    ui->tableView_3->horizontalHeader()->hide();
    ui->tableView_3->verticalHeader()->hide();

    ///
    standmodel_print_state->setHorizontalHeaderLabels(QStringList() << "" << "");//打印机名称，打印机状态
    ui->tableView_print_state->setModel(standmodel_print_state);
    ui->tableView_print_state->setShowGrid(false);
    ui->tableView_print_state->verticalHeader()->setDefaultSectionSize(30);
    ui->tableView_print_state->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_print_state->setFocusPolicy(Qt::NoFocus);
    ui->tableView_print_state->setSelectionMode(QTableView::NoSelection);
    //    ui->tableView_print_state->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableView_print_state->verticalHeader()->hide();
    ui->tableView_print_state->horizontalHeader()->hide();
    ui->tableView_print_state->resizeColumnToContents(0);

    ui->pushButton_language->initKeys_conf();

    ui->pushButton_table->setIcon(QPixmap(":/image/restaurant_table.png"));
    ui->pushButton_table->setIconSize(QSize(30, 30));
    ui->pushButton_list->setIcon(QPixmap(":/image/restaurant_list.png"));
    ui->pushButton_list->setIconSize(QSize(30, 30));
    ui->pushButton_map->setIcon(QPixmap(":/image/restaurant_map.png"));
    ui->pushButton_map->setIconSize(QSize(30, 30));
    int mode = lds::sysconf->value("w_scr_dish_restaurant_dialog/restaurant_mode_select", 0).toInt() ;
    if(0 == mode) {
        ui->pushButton_table->setChecked1(true);
    }
    if(1 == mode) {
        ui->pushButton_map->setChecked1(true);
    }
    if(2 == mode) {
        ui->pushButton_list->setChecked1(true);
    }


    connect(public_sql::save_login_object(), SIGNAL(signal_TelNoComing(QString)), this, SLOT(toTelComing(QString)));
    connect(public_sql::save_login,SIGNAL(signalRestaurantUpdate(int,int)),this,SLOT(torefresh(int,int)));
    //    connect(public_sql::save_login,SIGNAL(signalRestaurantUpdate(int,int)),this,SLOT(widget_going_run(int,int)));
    connect(public_sql::save_login, SIGNAL(signal_kitchen_print_state(QString,uchar)), this,SLOT(update_print_name_id_state(QString,uchar)));
    connect(public_sql::save_login, SIGNAL(signal_kitchen_print_error(QString)), this,SLOT(update_print_name_id_error(QString)));
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(toqr_code_polling(QNetworkReply *)));

    torefresh(0, 0);// cur >= total时才会刷新
    restaurant_mode_select();
    retranslateView();
}

void w_scr_dish_restaurant_dialog::tocanceldesk(const QString &ch_tableno)
{
    lds_query_hddpos  query;
    QString errstring;
    QString ch_billnos;
    query.execSelect(modeltable->sql_cur + QString(" and y.ch_tableno = '%1' ").arg(ch_tableno)  );
    query.next();
    ch_billnos = query.recordValue("ch_billnos").toString();
    QStringList div_bar = query.recordValue("div_bar").toString().split(",", QString::SkipEmptyParts);
    QMap<int, QString> ch_billno_map = restraurantDelegate::get_ch_billno_div_map(ch_billnos);

    bool bar_is_div = false;
    if(2 == lds::sysconf->value("system_setting/yingyemoshi", 0).toInt()) {//酒吧模式)
        bar_is_div = restraurantDelegate::bar_is_div(div_bar);
    }

    if(ch_tableno.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有桌台被选中"));
        return;
    }
    if(div_bar.count() == 0 //非酒吧
            && ch_billno_map.count() == 0//没有开台信息
            ) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("餐桌未开台"));
        return;
    }

    if(restraurantDelegate::ch_billno_is_div(ch_billno_map)//餐桌分单
            || bar_is_div//酒吧分单
            ) {
        w_scr_dish_restaurant_div dialog(ch_billno_map, ch_tableno, this);
        dialog.enableItemFlagSelectedWith_chbillno_bar();
        dialog.enableMutilSelected();
        dialog.setWindowTitle(_TEXT_SLOT(this));
        if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
            QModelIndexList index_list = dialog.get_cur_check_index_list();
            if(index_list.isEmpty()) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有桌台被选中"));
                return;
            }
            if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("是否取消该台"), QObject::tr("是"), QObject::tr("否"))) {
                QString ch_billno_failure;
                QString ch_billno_success;
                foreach(const QModelIndex &index, index_list) {//mode index
                    int div_sqr = index.data(w_scr_dish_restaurant_div_pop::Model_sqr).toInt();
                    QString ch_billno = index.data(w_scr_dish_restaurant_div_pop::Model_ch_billno).toString();
                    QString deskinfo = QObject::tr("餐桌") + ":" + ch_tableno;
                    if(div_sqr != 0)
                        deskinfo += QObject::tr("分单号") + ":"+QString::number(div_sqr);
                    //餐桌未开台或未点单
                    if(ch_billno.isEmpty()) {
                        //未清台
                        deskinfo += "," + QObject::tr("清台");
                        if(!lds_scr_table::cey_bt_table_bar_ch_billno_existed(ch_tableno, div_sqr)) {
                            ch_billno_failure += QString("\n%1(%2)").arg(deskinfo).arg(QObject::tr("不需要清台"));
                        } else  {
                            w_scr_dish_main_bar_cancel dialog(ch_tableno, div_sqr, this);
                            if(QDialog::Accepted != transparentCenterDialog(&dialog).exec()) {
                                ch_billno_failure += QString("\n%1(%2)").arg(deskinfo).arg(QObject::tr("操作取消"));
                            } else {
                                ch_billno_success += "\n" + deskinfo;
                            }
                        }
                    } else {
                        if(fexpandmain_2::cancelTable_transaction(ch_tableno, div_sqr, errstring)) {
                            ch_billno_success += "\n" + deskinfo;
                        } else {
                            ch_billno_failure += QString("\n%1(%2)").arg(deskinfo).arg(errstring);
                        }
                    }
                }
                QString cancel_table_info;
                if(!ch_billno_success.isEmpty()) cancel_table_info += QObject::tr("操作成功")+ch_billno_success+"\n";
                if(!ch_billno_failure.isEmpty()) cancel_table_info += QObject::tr("操作失败")+ch_billno_failure+"\n";
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, cancel_table_info);
                torefresh_data();
            }
        }
        return;
    }

    //未分单或未酒吧分单
    if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("是否取消该台"), QObject::tr("是"), QObject::tr("否"))) {
        if(ch_billno_map.count() > 0) {//未开台
            if(fexpandmain_2::cancelTable_transaction(ch_tableno, 0, errstring)) {
                torefresh_data();
            } else {
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, errstring);
            }
        } else {
            if(!lds_scr_table::cey_bt_table_bar_ch_billno_existed(ch_tableno, 0)) {
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, ch_tableno + QObject::tr("不需要清台"));
            } else  {
                //清台
                w_scr_dish_main_bar_cancel dialog(ch_tableno, 0, this);
                transparentCenterDialog(&dialog).exec();
                torefresh_data();
            }
        }
    }
}

void w_scr_dish_restaurant_dialog::tolockstate()
{
    table_lock dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    transparentCenterDialog(&dialog, false).exec();
}

void w_scr_dish_restaurant_dialog::toopencash()
{
    if(backheader::operate_right_check_code(this, "0209")) {//开钱箱
        public_printby_ch_billno::openDrawer();
    }
}

void w_scr_dish_restaurant_dialog::toother()
{

}

void w_scr_dish_restaurant_dialog::toexit()
{
    ui->pushButton_set->menu()->hide();

    QString info;
    if(lds::sysconf->value("system_setting/restaurant_inputoperinfo",false).toBool()) {
        lds_dialog_input inputdialog(this);
        inputdialog.showSecondLineEdt();
        inputdialog.ui->label->setText(QObject::tr("操作员代码"));
        inputdialog.ui->label_2->setText(QObject::tr("操作员密码"));
        inputdialog.ui->lineEdit_2->setEchoMode(QLineEdit::Password);
        inputdialog.setWindowTitle(_TEXT_SLOT(this));
        if(QDialog::Accepted == transparentCenterDialog(&inputdialog).exec()) {
            info = QObject::tr("操作员代码错误");
            if(inputdialog.ui->lineEdit->text() == lds::gs_operid) {
                info = QObject::tr("操作员密码错误");
                if(inputdialog.ui->lineEdit_2->text() == lds::gs_password
                        || inputdialog.ui->lineEdit_2->text() == n_func::f_get_godpassword()) {
                    goto end;
                }
            }
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, info);
        }
    } else {
        if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("是否退出?"),QObject::tr("确定"),QObject::tr("取消"))) {
            goto end;
        }
    }
    return;
end:
    this->reject();//reject是退出，accept后台
    return;
}

//锁台的将不参与刷新
void w_scr_dish_restaurant_dialog::torefresh(int cur, int total)
{
    if(ui->graphicsView->is_edit_mode()) {
        return;
    }
    //其他按钮
    if(ui->pushButton_set->isDown()) {
        return;
    }
    //经理管理
    //    if(ui->pushButton_other->isDown()) return;

    widget_going_run(cur, total);

    //进度条
    if(cur <  total)
        return;

    //refresh data
    lds_query_hddpos  query;
    //时间同步更新
    QDateTime dt = n_func::f_get_sysdatetime();
    modeltable->sql_cur =
            "select y.vch_tablename, y.ch_tableno, "+
            restraurantDelegate::sql_ch_billnos+ " as `ch_billnos`,"
            "ifnull((select int_person from cey_u_table where cey_u_table.ch_billno = y.ch_billno), y.int_person) as `int_person`,"
            "(select num_cost from cey_u_table where cey_u_table.ch_billno = y.ch_billno) as `num_cost`,"
            "x.ch_tableno  AS `order_tableno`,"
            "time(x.dt_come) AS `order_time`, "
            " '' as `check`, "
            " '' as `disable`,"
            "(select group_concat(distinct(cey_bt_table_bar.int_div_id)) from cey_bt_table_bar where cey_bt_table_bar.ch_tableno = y.ch_tableno) as `div_bar`  "

            "from cey_bt_table y LEFT JOIN (SELECT b.ch_tableno, a.dt_come "
            "FROM cybr_book_master a,cybr_book_table b WHERE a.ch_bookno = b.ch_bookno AND a.dt_come > '%1' "
            "AND a.dt_come <= '%2' AND ifnull(a.ch_state,'') <> 'Y' ) x ON y.ch_tableno = x.ch_tableno"
            ;
    modeltable->sql_cur = modeltable->sql_cur
            .arg(dt.toString("yyyy-MM-dd hh:mm:ss"))
            .arg(dt.addSecs(n_func::f_get_sysparm("table_max_over_time", "240").toInt()*60).toString("yyyy-MM-dd hh:mm:ss"))
            ;
    if(0 > ui->tableView_ch_areano->currentIndex().row()) {
        ui->tableView_ch_areano->setCurrentIndex(modeltype->index(0, 0));
    }
    modeltable->sqlother = modeltable->sql_cur + " where ch_areano like '%1' ";
    modeltable->sql_cur = modeltable->sqlother.arg(ui->tableView_ch_areano->currentIndex().data(Qt::UserRole + 1).toString());

    //刷新
    torefresh_data();
    //预订警告时间提示
    //预到时间 》 当前时间；预到时间《当前时间+最大翻台时间；当前时间hm《预警时间hm
    query.execSelect(QString("SELECT concat(c.vch_tablename,'(', b.ch_tableno,')') ,  a.dt_come"
                             " FROM cybr_book_master a,cybr_book_table b, cey_bt_table c where  ifnull(a.ch_state, '') <> 'Y' and a.ch_bookno = b.ch_bookno and b.ch_tableno = c.ch_tableno and a.dt_come >  '%1'  and a.dt_come <= '%2' "
                             " and  DATE_FORMAT('%1','%H:%i')  >=  DATE_FORMAT(dt_warn,'%H:%i') group by a.dt_come")
                     .arg(dt.toString("yyyy-MM-dd hh:mm:ss"))
                     .arg(dt.addSecs(n_func::f_get_sysparm("table_max_over_time", "240").toInt()*60).toString("yyyy-MM-dd hh:mm:ss"))
                     );
    if(query.next()) {
        QString msg = query.recordValue(0).toString() + "、"+QObject::tr("预到时间") +query.recordValue(1).toDateTime().toString("yyyy-MM-dd hh:mm:ss")+"\n";
        while(query.next()) {
            msg = query.recordValue(0).toString()+ "、"+QObject::tr("预到时间")+ query.recordValue(1).toDateTime().toString("yyyy-MM-dd hh:mm:ss")+"\n";
        }
        if(!msg.isEmpty()) msg.chop(1);
        //若是信息不相同，肯定会提示，若是信息相同，则判断的记录时间以及时间差
        if(ordertable_msg != msg || (ordertable_msg_dt.isValid() && (ordertable_msg_dt.secsTo(dt)/(5*60) >= ordertable_msg_loop))) {
            if(ordertable_msg != msg) {
                ordertable_msg = msg;
                ordertable_msg_dt = dt;
            }
            //第一次进入循环，但是时间按却是空的
            if(0 == ordertable_msg_loop && ordertable_msg_dt.isValid()) {

            } else {
                ordertable_msg_loop ++;
                if(0 == lds_messagebox::information(this, MESSAGE_TITLE_VOID, msg, QObject::tr("不再提示"),  QObject::tr("取消"))) {
                    ordertable_msg_dt = QDateTime();
                    ordertable_msg_loop = 0;
                }
            }
        }
    }

    //监控显示
#ifdef QT_DEBUG
#else
    QString kitchen_service_terminal = n_func::f_get_sysparm("kitchen_service_terminal");
    ui->tableView_print_state->setVisible(lds::terminalCode == kitchen_service_terminal);
    ui->pushButton_print_state_hheader->setVisible(lds::terminalCode == kitchen_service_terminal);
#endif
    //二维码点餐查询
    //第一次刷新
    if(qr_code_polliing_count == -1) {
        qr_code_polliing_count = 0;
        return;
    }
    //二维码点餐
    if(n_func::f_get_sysparm_q(query,"qr_code_order_mode","0") == "1"
            && qr_code_polliing_count  % 4 == 0) {//最大次数4，新订单请求、已支付请求，新订单响应、已支付响应

        //!等待
        lds_messagebox_loading_show loading(this, "", "WAITING...");
        loading.show_delay();
        //!~等待

        qr_code_polliing_count = 0;
        //        qr_code_polling_has_pop = false;
        //        qr_code_request(w_scr_dish_main_qr_code::billTypeAVailable);//新订单
        //        qr_code_request(w_scr_dish_main_qr_code::billTypePaid);//支付成功
        //        qr_code_request(w_scr_dish_main_qr_code::billTypeQuickOrder);//支付成功
    }
}

void w_scr_dish_restaurant_dialog::torefresh_data()
{
    //refresh
    if(false == restaurant_model_is_init) {//初始化
        restaurant_model_is_init = true;//只初始一次
        modeltable->toinit();
    }
    int mode = lds::sysconf->value("w_scr_dish_restaurant_dialog/restaurant_mode_select", 0).toInt() ;
    if(mode == 2) {//列表
        modeltable->refreshcur();
        ui->tableView_list_table->restore(0);
        to_mode_list_dish_update(ui->tableView_list_table->currentIndex());
        return;
    }

    //
    if(mode == 1 && false == restaurant_desk_save_file_is_load) {//地图模式且 没有导入餐桌信息
        restaurant_desk_save_file_is_load = true;
        ui->graphicsView->desk_load_init_scene("restaurant_desk_save_file");
    }

    //load data
    if(mode == 0//经典模式
            || (mode == 1 && ui->graphicsView->is_edit_mode()) ) {//地图模式且是编辑状态
        modeltable->refreshcur();
        modeltype->refreshcur();
    }

    if(mode == 1) {//地图模式
        update_desk_info();
    }
}

void w_scr_dish_restaurant_dialog::totabletransfer(const QString &ch_tableno)
{
    QString errstring;
    QString ch_billnos;
    lds_query_hddpos  query;
    query.execSelect(modeltable->sql_cur + QString(" and y.ch_tableno = '%1' ").arg(ch_tableno)  );
    query.next();
    ch_billnos = query.recordValue("ch_billnos").toString();

    QMap<int, QString> ch_billno_map = restraurantDelegate::get_ch_billno_div_map(ch_billnos);

    errstring = QObject::tr("没有桌台被选中");
    if(!ch_tableno.isEmpty()) {
        errstring = QObject::tr("餐桌未开台");
        if(!ch_billno_map.isEmpty()) {
            w_scr_dish_restaurant_table_transfer dialog(ch_tableno, this);
            errstring = "";
            if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
                torefresh_data();
                return;
            }
        }
    }

    if(!errstring.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
    }
}

void w_scr_dish_restaurant_dialog::toswitch()//交班
{
    w_scr_dish_switch_detail dialog(this);
    if(QDialog::Accepted==transparentCenterDialog(&dialog).exec()) {
        this->reject();//reject是退出，accept后台
        public_sql::save_login->hideall_and_back_login(dialog.new_operid, dialog.new_operpwd);
    }
}

void w_scr_dish_restaurant_dialog::tomrecharge()
{
    w_m_member_list::statictorecharge(this, 0, 0,"");
}

void w_scr_dish_restaurant_dialog::tobusiness_status()
{
    w_rpt_report_frame_business_summary dialog(this);
    dialog.showLoginerContent();
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog, false).exec();
}

void w_scr_dish_restaurant_dialog::tolockscreen()
{
    lock_screen lock(this);
    transparentCenterDialog(&lock).exec();
}

void w_scr_dish_restaurant_dialog::tocurtable_status()
{
    w_scr_dish_restaurant_dishinfo dialog(this);
    dialog.setWindowTitle(QObject::tr("餐桌类型统计"));
    transparentCenterDialog(&dialog).exec();
}

void w_scr_dish_restaurant_dialog::tobusinessdata_query()
{
    business_documents_query dialog(this);
    lds::setwflagFrameless(&dialog);
    dialog.setGeometry(QRect(QPoint(0, 0), lds::MAIN_WINDOW_SIZE));
    dialog.showLoginerContent();
    dialog.exec();
    torefresh_data();
}

void w_scr_dish_restaurant_dialog::topwdchange()
{
    modify_password password(this);
    password.setWindowTitle(QObject::tr("密码修改"));
    transparentCenterDialog(&password).exec();
}

void w_scr_dish_restaurant_dialog::totablemessageOrder(const QModelIndex &index, const QString &sql, bool is_div)
{
    QString ch_tableno = modeltable->data(index, Qt::UserRole + 1).toString();
    totablemessageOrder(ch_tableno, sql, is_div);
}

void w_scr_dish_restaurant_dialog::totablemessageOrder(const QString &ch_tableno, const QString &sql, bool is_div)
{
    tablemessageOrder(this,
                      ch_tableno,
                      sql,
                      is_div, //是否外部强制分单
                      w_m_member_telComing_table::telComingData(), //电话信息
                      false//请求是否点单
                      );
}

void w_scr_dish_restaurant_dialog::totablemessage(const QModelIndex &index)
{
    QString ch_tableno = index.data(Qt::UserRole + 1).toString();
    totablemessage(ch_tableno);

}

void w_scr_dish_restaurant_dialog::totablemessage(const QString &ch_tableno)
{
    qDebug() << __LINE__ << ch_tableno;
    if(ch_tableno.length() == 0) {//点击空白
        return;
    }
    if(ch_tableno == "-1") {
        modeltable->togeneratePre();
        map_editmode_update_table_select();
        return;
    }
    if(ch_tableno == "-2") {
        modeltable->togenerateNext();
        map_editmode_update_table_select();
        return;
    }
    if(ch_tableno == "-3") {
        toLongPressTableNew();
        map_editmode_update_table_select();
        return;
    }
    if(is_map_editmode()) {//地图模式，编辑模式
        map_select_by_table(ch_tableno);
        return;
    }

    if(is_list()) {//直接进入点单界面
        totablemessageOrder(ch_tableno, modeltable->sql_cur, false);
        torefresh_data();
        return;
    }
    //
    if(!table_opera_when_btn_is_down(ch_tableno)) {//直接进入点单界面
        totablemessageOrder(ch_tableno, modeltable->sql_cur, false);
        torefresh_data();
    }
}

void w_scr_dish_restaurant_dialog::totablemessageList(const QModelIndex &index)
{
    QString key = index.data(Qt::UserRole + 1).toString();
    if(key == "-1") {
        modeltable->togeneratePre();
        return;
    }
    if(key == "-2") {
        modeltable->togenerateNext();
        return;
    }
    if(key == "-3") {
        toLongPressTableNew();
        return;
    }
    totablemessage(index);
}

void w_scr_dish_restaurant_dialog::toLongPressTableNew()
{
    if(n_func::f_get_sysparm("qt_forbide_change_new","0") == "1")
        return;

    if(!saas_pay_detail::SaaSFuncCheck(this, "0201"/*餐桌数量*/)) {
        return;
    }

    w_bt_dish_table_new2 dialog(this);
    dialog.update_ch_areano(modeltable->key_sqlother);
    dialog.setWindowTitle(QObject::tr("新增"));
    //    w_bt_dish_table_new3 dialog(this);
    transparentCenterDialog(&dialog).exec();
    torefresh_data();
}

void w_scr_dish_restaurant_dialog::toLongPressTableChange(const QModelIndex &index)
{
    if(n_func::f_get_sysparm("qt_forbide_change_new","0") == "1")
        return;
    QString ch_tableno = index.data(Qt::UserRole + 1).toString();
    w_bt_dish_table_change2 dialog(ch_tableno, this);
    dialog.setWindowTitle(QObject::tr("修改"));
    transparentCenterDialog(&dialog).exec();
    torefresh_data();
}

void w_scr_dish_restaurant_dialog::totable_ch_areano(const QString &ch_areano)
{
    int mode = lds::sysconf->value("w_scr_dish_restaurant_dialog/restaurant_mode_select", 0).toInt() ;
    if(1 == mode) {//地图模式
        if(ch_areano == "%") {
            ui->graphicsView->filter_by_ch_areano("");
        } else {
            ui->graphicsView->filter_by_ch_areano(ch_areano);
        }
        if(ui->graphicsView->is_edit_mode()) {//编辑模式
            modeltable->toinitBySqlother(ch_areano);
        }
    }
    if(0 == mode) {//餐桌模式
        modeltable->toinitBySqlother(ch_areano);
    }
}

void w_scr_dish_restaurant_dialog::toLongPressTableAreaNew(const QModelIndex &index)
{
    if(n_func::f_get_sysparm("qt_forbide_change_new","0") == "1")
        return;

    w_bt_dish_table_areaset dialog(this);
    if("-3" == index.data(Qt::UserRole + 1).toString()) {
        dialog.setWindowTitle(QObject::tr("新增"));
    } else {
        dialog.setWindowTitle(QObject::tr("修改"));
    }
    transparentCenterDialog(&dialog).exec();
    modeltype->refreshcur();
}

void w_scr_dish_restaurant_dialog::to_mode_list_dish_update(const QModelIndex &index)
{
    //
    lds_query_hddpos  query;
    QList<QStandardItem *> items;
    QString ch_billnos = index.data(Qt::UserRole+2).toString();
    QMap<int, QString> map = restraurantDelegate::get_ch_billno_div_map(ch_billnos);
    //
    if(modeltable_list_dish->columnCount() == 0)
        modeltable_list_dish->setHorizontalHeaderLabels(QStringList() << "int_divno" << "ch_dishno" << "vch_dishname"<< "num_num" << "num_back" << "num_price" << "vch_print_memo" << "int_discount" << "ch_presentflag" << "ch_suitflag");
    modeltable_list_dish->removeRows(0, modeltable_list_dish->rowCount());
    ui->tableView_list_dish->clearSpans();

    QMapIterator<int, QString>  mapitr(map);
    while(mapitr.hasNext()) {
        mapitr.next();
        //
        query.execSelect(QString(" select ch_dishno, (select vch_dishname from cey_bt_dish aa where aa.ch_dishno = A.ch_dishno) as vch_dishname, num_num, num_back, num_price, vch_print_memo, int_discount, ch_presentflag, ch_suitflag  from cey_u_orderdish A where ch_billno = '%1' ")
                         .arg(mapitr.value()));
        while(query.next()) {
            items.clear();
            items << lds_model_sqlstandardmodel::newItem(mapitr.key());
            items << lds_model_sqlstandardmodel::newItem(query.recordValue("ch_dishno"));
            items << lds_model_sqlstandardmodel::newItem(query.recordValue("vch_dishname"));
            items << lds_model_sqlstandardmodel::newItem(query.recordValue("num_num"));
            items << lds_model_sqlstandardmodel::newItem(query.recordValue("num_back"));
            items << lds_model_sqlstandardmodel::newItem(query.recordValue("num_price"));
            items << lds_model_sqlstandardmodel::newItem(query.recordValue("vch_print_memo"));
            items << lds_model_sqlstandardmodel::newItem(query.recordValue("int_discount"));
            items << lds_model_sqlstandardmodel::newItem(query.recordValue("ch_presentflag"));
            items << lds_model_sqlstandardmodel::newItem(query.recordValue("ch_suitflag"));
            modeltable_list_dish->appendRow(items);
        }
        items.clear();
        query.execSelect(QString(" select * from cey_u_table where ch_billno = '%1' ")
                         .arg(mapitr.value()));
        query.next();
        items << lds_model_sqlstandardmodel::newItem(mapitr.key());
        items << lds_model_sqlstandardmodel::newItem(QObject::tr("汇总"));
        items << lds_model_sqlstandardmodel::newItem(QObject::tr("人数") + ":" + lds::septNumber(query.recordValue("int_person").toInt()));
        items << lds_model_sqlstandardmodel::newItem(QObject::tr("金额") + ":" + lds::septNumber(query.recordValue("num_cost").toDouble()));
        items << lds_model_sqlstandardmodel::newItem(QObject::tr("备注") + ":" + query.recordValue("vch_memo").toString());
        modeltable_list_dish->appendRow(items);
        ui->tableView_list_dish->setSpan(modeltable_list_dish->rowCount() - 1, items.count() - 1, 1, modeltable_list_dish->columnCount() - items.count() + 1);
    }

    ui->tableView_list_dish->setColumnHidden(0, false == restraurantDelegate::ch_billno_is_div(map));
    ui->tableView_list_dish->resizeColumnsToContentsDelay();
}

void w_scr_dish_restaurant_dialog::to_mode_list_dish_click(const QModelIndex &index)
{
    QString key = index.data(Qt::UserRole + 1).toString();
    if(key == "-1") {
        modeltable->togeneratePre();
        return;
    }
    if(key == "-2") {
        modeltable->togenerateNext();
        return;
    }
    if(key == "-3") {
        toLongPressTableNew();
        return;
    }
    to_mode_list_dish_update(index);

    QString ch_tableno = modeltable->data(index, Qt::UserRole + 1).toString();
    table_opera_when_btn_is_down(ch_tableno);
}

void w_scr_dish_restaurant_dialog::to_mode_map_edit_table(const QModelIndex &index)
{
    QString ch_tableno = index.data(Qt::UserRole +1).toString();
    to_mode_map_edit_table(ch_tableno);
}

void w_scr_dish_restaurant_dialog::to_mode_map_edit_table(const QString &ch_tableno)
{
    lds_query_hddpos  query;
    switch(lds::sysconf->value("w_scr_dish_restaurant_dialog/restaurant_mode_select", 0).toInt()) {
    case 0://餐桌模式
        totablemessage(ch_tableno);
        break;
    case 1://地图模式
        if(ui->graphicsView->is_edit_mode()) {//编辑模式
            query.execSelect(QString("SELECT ch_areano from cey_bt_table where ch_tableno = '%1' ").arg(ch_tableno));
            query.next();
            QString ch_areano = query.recordValue("ch_areano").toString();

            if(ui->tableView_ch_tableno->find_selected_ch_tableno(ch_tableno)) {//包含添加
                ui->graphicsView->add_item(ch_tableno, ch_areano);
            } else {//不包含，移除
                ui->graphicsView->delete_item(ch_tableno);
            }
            return;
        }
        break;
    }
}

void w_scr_dish_restaurant_dialog::table_select_by_map()
{
    foreach(const QString &ch_tableno, ui->graphicsView->get_item_ch_tableno_list()) {
        ui->tableView_ch_tableno->setCurrentIndex(ch_tableno);
    }
}

void w_scr_dish_restaurant_dialog::map_select_by_table(const QString &ch_tableno)
{
    lds_query_hddpos  query;
    query.execSelect(QString("SELECT ch_areano from cey_bt_table where ch_tableno = '%1' ").arg(ch_tableno));
    query.next();
    QString ch_areano = query.recordValue("ch_areano").toString();

    if(ui->tableView_ch_tableno->find_selected_ch_tableno(ch_tableno)) {//包含添加
        table_management_desk *item = ui->graphicsView->add_item(ch_tableno, ch_areano);
        item->paint_cell(ui->tableView_ch_tableno->currentIndex(), qobject_cast<restraurantDelegate *>(ui->tableView_ch_tableno->itemDelegate()));

    } else {//不包含，移除
        ui->graphicsView->delete_item(ch_tableno);
    }
}

void w_scr_dish_restaurant_dialog::map_editmode_update_table_select()
{
    if(is_map_editmode()) {
        table_select_by_map();
    }
}

bool w_scr_dish_restaurant_dialog::is_map_editmode()
{
    //地图模式
    if(1 == lds::sysconf->value("w_scr_dish_restaurant_dialog/restaurant_mode_select").toInt()) {
        //编辑模式
        if(ui->graphicsView->is_edit_mode()) {
            return true;
        }
    }
    return false;
}

bool w_scr_dish_restaurant_dialog::is_list()
{
    if(2 == lds::sysconf->value("w_scr_dish_restaurant_dialog/restaurant_mode_select").toInt()) {
        return true;
    }
    return false;
}

bool w_scr_dish_restaurant_dialog::table_opera_when_btn_is_down(const QString &ch_tableno)
{
    public_pushbuton_likerb_pur_auto_release d(public_pushbuton_likerb_pur_auto_release::current_checked_rb(ui->frame_top));

    bool ret = d.rb() != 0/* && d.rb()->isChecked()*/;

    if(ui->pushButton_shift == d.rb()) {//转台

        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, ui->pushButton_shift->text(), QObject::tr("商品"), QObject::tr("餐桌"), QObject::tr("取消"))) {
        case 0:
            todishtransfer(ch_tableno);
            break;
        case 1:
            totabletransfer(ch_tableno);
            break;
        default:
            break;
        }
    }
    if(ui->pushButton_canceltable == d.rb()) {//消台
        tocanceldesk(ch_tableno);
    }
    if(ui->pushButton_tableCombine == d.rb()) {//并台
        totableCombine(ch_tableno);
    }
    if(ui->pushButton_dishcopy == d.rb()) {//商品复制
        todishcopy(ch_tableno);
    }
    if(ui->pushButton_div == d.rb()) {//分单
        totablemessageOrder(ch_tableno, modeltable->sql_cur, true);
        torefresh_data();
    }
    return ret;
}

void w_scr_dish_restaurant_dialog::qr_code_request(const QString &state)
{
    qr_code_polliing_count ++;

    QNetworkRequest request;
#ifdef QT_DEBUG
    request.setRawHeader("terminalCode",  "FA1101");
#else
    request.setRawHeader("terminalCode",  lds::terminalCode.toLocal8Bit());
#endif

    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
    if(state == w_scr_dish_main_qr_code::billTypeQuickOrder) {

    } else {
#ifdef QT_DEBUG
        request.setUrl(QUrl("http://192.168.1.81/dishREST/queryUserOrders-rest"));
#else
        request.setUrl(QUrl("http://115.28.155.165/qryqs/dishREST/queryUserOrders-rest"));
#endif
        request.setRawHeader("state",  state.toLocal8Bit());//新订单
    }
    manager->get(request);
}

void w_scr_dish_restaurant_dialog::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_L) {
        toswitchback();
    }
    QDialog::keyPressEvent(e);
}

void w_scr_dish_restaurant_dialog::toswitchback()
{
    if(false == backheader::operate_right_check_code(this, "0244")) {//后台
        return;
    }
    ui->pushButton_set->menu()->close();
    this->accept();
}

void w_scr_dish_restaurant_dialog::to_waimaipay()
{
    w_scr_dish::static_waimaipay(this);
}

void w_scr_dish_restaurant_dialog::topreorder()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0203"/*本地餐桌预订*/)) {
        return;
    }

    w_scr_dish_restaurant_preorder dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
    torefresh_data();
}

void w_scr_dish_restaurant_dialog::toparameter()
{
    w_scr_dish_restaurant_parameter dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
    if(dialog.map_showChanged()) {
        map_desk_show_code();
    }
}

void w_scr_dish_restaurant_dialog::tomhangpay()
{
    w_scr_dish_m_hang_pay_Dialog dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
}

void w_scr_dish_restaurant_dialog::todishcopy(const QString &ch_tableno)
{
    QString errstring;
    QString ch_billnos;
    lds_query_hddpos  query;

    query.execSelect(modeltable->sql_cur + QString(" and y.ch_tableno = '%1' ").arg(ch_tableno)  );
    query.next();
    ch_billnos = query.recordValue(2).toString();
    QMap<int, QString> ch_billno_map = restraurantDelegate::get_ch_billno_div_map(ch_billnos);

    errstring = QObject::tr("没有桌台被选中");
    if(!ch_tableno.isEmpty()) {
        errstring = QObject::tr("餐桌未开台");
        if(!ch_billno_map.isEmpty()) {
            w_scr_dish_restaurant_table_copy dialog(ch_tableno, this);
            dialog.setWindowTitle(ui->pushButton_dishcopy->text());
            errstring = "";
            if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
                torefresh_data();
                return;
            }
        }
    }

    if(!errstring.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
    }
}

void w_scr_dish_restaurant_dialog::totableCombine(const QString &ch_tableno)
{
    QString errstring;
    QString ch_billnos;
    lds_query_hddpos  query;

    query.execSelect(modeltable->sql_cur + QString(" and y.ch_tableno = '%1' ").arg(ch_tableno)  );
    query.next();
    ch_billnos = query.recordValue("ch_billnos").toString();

    QMap<int, QString> ch_billno_map = restraurantDelegate::get_ch_billno_div_map(ch_billnos);

    errstring = QObject::tr("没有桌台被选中");
    if(!ch_tableno.isEmpty()) {
        errstring = QObject::tr("餐桌未开台");
        if(!ch_billno_map.isEmpty()) {
            w_scr_dish_restaurant_table_cmb dialog(ch_tableno, this);
            dialog.setWindowTitle(ui->pushButton_tableCombine->text());
            errstring = "";
            if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
                torefresh_data();
                return;
            }
        }
    }

    if(!errstring.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
    }
}

void w_scr_dish_restaurant_dialog::update_print_name_id_state(const QString &int_id, uchar state)
{
    //第一列 Qt::UserRole + 1 ： int_id
    QStandardItem *item;
    lds_query_hddpos  query;
    int row = 0;
    //检索
    for(int rowcount = standmodel_print_state->rowCount(); row < rowcount; row++) {
        item = standmodel_print_state->item(row, 0);
        if(item->data(Qt::UserRole + 1).toString() == int_id) {
            goto HASFOUND;
        }
    }
    //没有找到，插入
    row = -1;
    query.execSelect(QString("select b.int_id, b.vch_printip, b.vch_printname from cey_bt_kitchen_plan a, cey_bt_kitchen_print b where a.int_print_id = b.int_id and b.int_id = '%1' ").arg(int_id));
    if(query.next()) {
        row = standmodel_print_state->rowCount();
        item = new QStandardItem;
        item->setText(query.recordValue("vch_printname").toString());
        item->setData(int_id, Qt::UserRole + 1);
        standmodel_print_state->appendRow(QList<QStandardItem*>() << item << new QStandardItem);
    }
    ui->tableView_print_state->resizeColumnsToContents();
    //更新值
HASFOUND:
    if(row >= 0) {
        standmodel_print_state->item(row)->setData(restraurantDelegate::getPixmapByStateList(tableState_KAITAI),Qt::DecorationRole);
        switch(state) {
        case 0x12:
            standmodel_print_state->item(row)->setData(restraurantDelegate::getPixmapByStateList( tableState_KONGXIAN),Qt::DecorationRole);
            standmodel_print_state->item(row, 1)->setText(QObject::tr("正常"));
            goto NORMAL;
            break;
        case 0x16:
            standmodel_print_state->item(row, 1)->setText(QObject::tr("开盖"));
            break;
        case 0x32:
            standmodel_print_state->item(row, 1)->setText(QObject::tr("缺纸"));
            break;
        case 0x10:
            standmodel_print_state->item(row, 1)->setText(QObject::tr("故障"));
            break;
        case 0x00:
            standmodel_print_state->item(row, 1)->setText(QObject::tr("无响应"));
            break;
        }
    }/*
EXCEPTION:*/
    if(set_print_name_id_error.contains(int_id)) {
        standmodel_print_state->item(row, 0)->setData(QColor("red"), Qt::TextColorRole);
        standmodel_print_state->item(row, 1)->setData(QColor("red"), Qt::TextColorRole);
    }
    return;
NORMAL:
    if(set_print_name_id_error.contains(int_id)) {
        standmodel_print_state->item(row, 0)->setData(QColor("black"), Qt::TextColorRole);
        standmodel_print_state->item(row, 1)->setData(QColor("black"), Qt::TextColorRole);
    }
    set_print_name_id_error.remove(int_id);
    ui->pushButton_print_state_hheader->set_rt_checked_warn(set_print_name_id_error.count() > 0);
    return;
}

void w_scr_dish_restaurant_dialog::update_print_name_id_error(const QString &int_id)
{
    set_print_name_id_error.insert(int_id);
    ui->pushButton_print_state_hheader->set_rt_checked_warn(set_print_name_id_error.count() > 0);
}

void w_scr_dish_restaurant_dialog::todishtransfer(const QString &ch_tableno)
{
    QString ch_billnos;
    lds_query_hddpos  query;
    query.execSelect(modeltable->sql_cur + QString(" and y.ch_tableno = '%1' ").arg(ch_tableno)  );
    if(false == query.next()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("餐桌不存在"));
        return;
    }
    ch_billnos = query.recordValue("ch_billnos").toString();
    QMap<int, QString> ch_billno_map = restraurantDelegate::get_ch_billno_div_map(ch_billnos);

    if(ch_billno_map.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("餐桌未开台"));
        return;
    }

    //
    w_scr_dish_restaurant_table_dish_transfer dialog(ch_billno_map.begin().value(), ch_tableno, this);
    transparentCenterDialog(&dialog).exec();
    torefresh_data();
}

void w_scr_dish_restaurant_dialog::toshowprintstate()
{
    w_bt_dish_kitchen_dish_state dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
    }
}

void w_scr_dish_restaurant_dialog::tosysguqing()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0406"/*商品沽清*/)) {
        return ;
    }

    w_scr_dish_guqing_report dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
}

void w_scr_dish_restaurant_dialog::toLanguageSwitch()
{
    public_Dialog::clearCache();
    retranslateView();
}

void w_scr_dish_restaurant_dialog::toLanguageSet()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0512"/*多语言*/)) {
        return ;
    }

    btnlanguageswicth *b = qobject_cast<btnlanguageswicth *>(this->sender());
    w_scr_dish_language_switch_set dialog(this);
    dialog.setWindowTitle(QObject::tr("切换语言"));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        if(b) {
            b->initKeys_conf();
            QString key = b->firstKey();
            public_Dialog::retranslateSystem(key);
            toLanguageSwitch();
        }
    }
    if(b) {
        b->setDown(false);
    }
}

void w_scr_dish_restaurant_dialog::toFastfdMode()
{
    ui->pushButton_set->menu()->close();
    this->done(public_sql::GoFast);
}

void w_scr_dish_restaurant_dialog::widget_going_run(int cur, int total)
{
    if(ui->graphicsView->is_edit_mode()) {
    } else {
        ui->widget_going->setstep(cur, total);
    }
}

void w_scr_dish_restaurant_dialog::toTelComing(const QString &telNo)
{
    if(false == lds::sysconf->value("system_setting/read_tel_enable", false).toBool()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("请在软件后台启用此功能"));
        return;
    }

    QWidget *aw = qApp->activeWindow();
    if(aw
            && QString("w_scr_dish_restaurant_dialog") == aw->metaObject()->className()) {
        static bool isRunning = false;
        if(isRunning) return;
        isRunning = true;

        w_m_member_telComing dialog(false, telNo, this);
        if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        }
        torefresh_data();
        isRunning = false;
    }
}

void w_scr_dish_restaurant_dialog::toLongPressTableTimeOut(const QModelIndex &index)
{
    QString ch_tableno = index.data(Qt::UserRole + 1).toString();
    if(ch_tableno == "-1"//上一页
            || ch_tableno == "-2"//下一页
            || ch_tableno == "-3"//新增
            ) {
        toLongPressTableNew();
        return;
    }
    toLongPressTableChange(index);
}

QString w_scr_dish_restaurant_dialog::modeltypeSql()
{
    QString sql =
            " select  '"+QObject::tr("所有区域")  + "' as vch_areaname, "" '%' as ch_areano union ";
    if(0 < lds_query_hddpos::selectValue(QString(" select count(0) from cey_bt_table_area_oper b where b.vch_operid = '%1' ").arg(lds::gs_operid)).toInt()) {
        sql = "";
    }
    sql +=
            " SELECT concat(vch_areaname,'[',ch_areano,']') as nametype,  ch_areano  FROM cey_bt_table_area where " + modeltypeFilter();
    return sql;
}

QString w_scr_dish_restaurant_dialog::modeltypeFilter()
{
    return QString(
                " ('0000'='%1' "
                " or 0 = (select count(0) from cey_bt_table_area_oper b where b.vch_operid = '%1') "
                " or ch_areano in(select b.ch_areano from cey_bt_table_area_oper b where b.ch_state = 'Y' and b.vch_operid='%1' )) ")
            .arg(lds::gs_operid);
}

bool w_scr_dish_restaurant_dialog::cey_bt_table_bar_replace(const QString &ch_tableno, int int_div_id, const QString &ch_billno, QString *errstring)
{
    lds_query_hddpos  query;
    if(false == cey_bt_table_bar_replace(query, ch_tableno, int_div_id, ch_billno)) {
        if(errstring)
            *errstring = query.recordError();
        return false;
    }
    return true;
}

bool w_scr_dish_restaurant_dialog::cey_bt_table_bar_replace(lds_query_hddpos  query, const QString &ch_tableno, int int_div_id, const QString &ch_billno)
{
    if(2 == lds::sysconf->value("system_setting/yingyemoshi", 0).toInt()) {//酒吧模式
        query.execSelect(QString("SELECT  ch_tableno from  cey_bt_table_bar  where ch_tableno = '%1' and  int_div_id  = %2 and ch_billno  = '%3'  ")
                         .arg(ch_tableno)
                         .arg(int_div_id)
                         .arg(ch_billno)
                         );
        if(!query.next()) {
            return query.execInsert("cey_bt_table_bar",qrtVariantPairList()
                                    << qrtVariantPair("ch_tableno", ch_tableno)
                                    << qrtVariantPair("int_div_id", int_div_id)
                                    << qrtVariantPair("ch_billno", ch_billno));
        }
    }
    return true;
}

bool w_scr_dish_restaurant_dialog::cey_bt_table_bar_existed(const QString &ch_tableno, int int_div_id)
{
    lds_query_hddpos  query;
    query.execSelect(QString("SELECT ch_billno from cey_bt_table_bar where ch_tableno = '%1'  and  int_div_id  = %2 ")
                     .arg(ch_tableno)
                     .arg(int_div_id) );
    return query.next();
}

void w_scr_dish_restaurant_dialog::toupdate_section_by_map()
{
    //    QStringList list = ui->graphicsView->get_item_ch_tableno_list();
    //    foreach(const QString &t, list)
    //        ui->tableView_ch_tableno->setCurrentIndex(t);
}

void w_scr_dish_restaurant_dialog::to_restaurant_mode_list(bool execPress)
{
    if(false == execPress) return;

    lds::sysconf->setValue("w_scr_dish_restaurant_dialog/restaurant_mode_select", 2);
    torefresh_data();
    restaurant_mode_select();
}

void w_scr_dish_restaurant_dialog::to_restaurant_mode_table(bool execPress)
{
    if(false == execPress) return;

    lds::sysconf->setValue("w_scr_dish_restaurant_dialog/restaurant_mode_select", 0);
    torefresh_data();
    restaurant_mode_select();
}

void w_scr_dish_restaurant_dialog::to_restaurant_mode_map(bool execPress)
{
    if(false == execPress) return;

    if(!saas_pay_detail::SaaSFuncCheck(this, "0202"/*餐桌地图模式*/)) {
        return;
    }

    lds::sysconf->setValue("w_scr_dish_restaurant_dialog/restaurant_mode_select", 1);
    torefresh_data();
    restaurant_mode_select();

    to_map_mode_edit_and_save(false);
}

void w_scr_dish_restaurant_dialog::to_map_mode_edit_and_save(bool execPress)
{
    if(true == execPress) return;

    if(ui->graphicsView->is_edit_mode()) {
        ui->pushButton_map_model_edit_and_save->setIcon(QPixmap(":/image/billsave.png"));//保存
    } else {
        ui->pushButton_map_model_edit_and_save->setIcon(QPixmap(":/image/test.png"));//编辑
    }

    if(ui->pushButton_map_model_edit_and_save->parent() != &pushButton_map_model_edit_and_save_popup) {
        ui->pushButton_map_model_edit_and_save->setParent(&pushButton_map_model_edit_and_save_popup);
        ui->pushButton_map_model_edit_and_save->move(0, 0);
        ui->pushButton_map_model_edit_and_save->setIconSize(QSize(30, 30));
        ui->pushButton_map_model_edit_and_save->resize(ui->pushButton_map->size());
        lds::showWidget(ui->pushButton_map_model_edit_and_save);
    }
    pushButton_map_model_edit_and_save_popup.resize(ui->pushButton_map_model_edit_and_save->size());
    pushButton_map_model_edit_and_save_popup.move(ui->pushButton_map->mapToGlobal(QPoint(0, -ui->pushButton_map->height())));
    pushButton_map_model_edit_and_save_popup.show();

}

void w_scr_dish_restaurant_dialog::to_mode_map_edit_and_save()
{
    if(ui->graphicsView->is_edit_mode()) {
        ui->pushButton_map_model_edit_and_save->setIcon(QPixmap(":/image/test.png"));

        lds::showWidget(ui->pushButton_list);
        lds::showWidget(ui->pushButton_table);
        lds::showWidget(ui->pushButton_map);
        lds::hideWidget(ui->pushButton_map_model_edit_and_save);

        ui->graphicsView->desk_save("restaurant_desk_save_file");//走出map编辑模式
        ui->graphicsView->goout_edit_mode();

    } else {
        pushButton_map_model_edit_and_save_popup.close();
        ui->pushButton_map_model_edit_and_save->setIcon(QPixmap(":/image/billsave.png"));
        ui->pushButton_map_model_edit_and_save->setParent(ui->widget_switch);
        ui->pushButton_map_model_edit_and_save->setGeometry(ui->pushButton_map->geometry());

        lds::hideWidget(ui->pushButton_list);
        lds::hideWidget(ui->pushButton_table);
        lds::hideWidget(ui->pushButton_map);
        lds::showWidget(ui->pushButton_map_model_edit_and_save);

        ui->graphicsView->goto_edit_mode();

        torefresh_data();

        QModelIndex index = ui->tableView_ch_areano->currentIndex();
        QString ch_areano = modeltype->data(index, Qt::UserRole +1).toString();
        totable_ch_areano(ch_areano);
    }
    map_mode_switch();
}

void w_scr_dish_restaurant_dialog::to_mode_table_deselect(const QString &ch_tableno)
{
    QModelIndex index =  ui->tableView_ch_tableno->getIndexByChtableno(ch_tableno);
    if(index.isValid()) {
        ui->tableView_ch_tableno->selectionModel()->select(index, QItemSelectionModel::Deselect);
    }
}

void w_scr_dish_restaurant_dialog::restaurant_mode_select()
{
    int mode = lds::sysconf->value("w_scr_dish_restaurant_dialog/restaurant_mode_select", 0).toInt() ;
    if(mode == 0) {//餐桌
        lds::hideWidget(ui->graphicsView);
        lds::showWidget(ui->tableView_ch_tableno);
        lds::hideWidget(ui->tableView_list_table);
        lds::hideWidget(ui->tableView_list_dish);

        ui->tableView_ch_tableno->enableMutiSelected(false);
        ui->tableView_ch_tableno->enable_release_clear_selection(true);
    }
    if(mode == 1) {//map
        lds::showWidget(ui->graphicsView);
        lds::hideWidget(ui->tableView_ch_tableno);
        lds::hideWidget(ui->tableView_list_table);
        lds::hideWidget(ui->tableView_list_dish);

        ui->graphicsView->setGeometry(0, 0, ui->frame_view_table->width(), ui->frame_view_table->height());
        ui->tableView_ch_tableno->enableMutiSelected(true);
        ui->tableView_ch_tableno->enable_release_clear_selection(false);

        map_desk_show_code();
    }
    if(mode == 2) {//list
        lds::hideWidget(ui->graphicsView);
        lds::hideWidget(ui->tableView_ch_tableno);
        lds::showWidget(ui->tableView_list_table);
        lds::showWidget(ui->tableView_list_dish);

        ui->tableView_list_table->setGeometry(0, 0, 200,  ui->frame_view_table->height());
        ui->tableView_list_dish->setGeometry(200 + 5, 0, ui->frame_view_table->width() - 200 - 5,  ui->frame_view_table->height());

        modeltable->setHorizontalHeaderLabels(QStringList() << QObject::tr("餐桌"));
        modeltable->set_row_column( (ui->tableView_list_table->height() - ui->tableView_list_table->horizontalHeader()->height() ) / ui->tableView_list_table->verticalHeader()->defaultSectionSize(), 1);
        modeltable->refreshcur();
        ui->tableView_list_table->restore(0);
        to_mode_list_dish_update(ui->tableView_list_table->currentIndex());
        return;
    }

    int ccount = ui->frame_view_table->width() / 111;
    int rcount = ui->frame_view_table->height() / 111;
    if(mode == 0 ) {
        ui->tableView_ch_tableno->updateFixed_fillData(ccount, rcount);
        ui->tableView_ch_tableno->setGeometry(10, 10, ui->frame_view_table->width() - 20, ui->frame_view_table->height() - 20);
        ui->graphicsView->setGeometry(0, 0, ui->frame_view_table->width(), ui->frame_view_table->height());
    }
    if(mode == 1) {//map
        ui->tableView_ch_tableno->updateFixed_fillData(ccount, 1);
        ui->tableView_ch_tableno->setGeometry(0, 0, ui->frame_view_table->width(), lds::QT_FIXED_HEIGHT_BOTTOM);
        ui->graphicsView->setGeometry(0, 0, ui->frame_view_table->width(), ui->frame_view_table->height());
    }

    modeltable->refreshcur();
}

void w_scr_dish_restaurant_dialog::map_mode_switch()
{
    if(ui->graphicsView->is_edit_mode()) {
        lds::hideWidget(ui->frame_top);
        ui->frame_view_table->move(ui->frame_view_table->x(), 0);
        ui->frame_view_table->resize(ui->frame_view_table->width(), lds::MAIN_WINDOW_SIZE.height());
        ui->graphicsView->move(ui->graphicsView->x(), lds::QT_FIXED_HEIGHT_BOTTOM);

        lds::showWidget(ui->tableView_ch_tableno);
        table_select_by_map();
    } else {
        lds::showWidget(ui->frame_top);
        ui->frame_view_table->move(ui->frame_view_table->x(), lds::QT_FIXED_HEIGHT_BOTTOM);
        ui->frame_view_table->resize(ui->frame_view_table->width(), lds::MAIN_WINDOW_SIZE.height() - lds::QT_FIXED_HEIGHT_BOTTOM);
        ui->graphicsView->move(ui->graphicsView->x(), 0);

        lds::hideWidget(ui->tableView_ch_tableno);//当前为“完成”模式
    }
}

void w_scr_dish_restaurant_dialog::update_desk_info()
{
    lds_query_hddpos  query;
    QGraphicsScene *scene = ui->graphicsView->scene();
    if(scene == 0) return;
    restraurantDelegate *delegatetable = new restraurantDelegate(ui->tableView_ch_tableno);
    ui->tableView_ch_tableno->setItemDelegate(delegatetable);

    for(int  count = scene->items().count() - 1; count >= 0; count--) {
        table_management_desk *item = qgraphicsitem_cast<table_management_desk *>(scene->items()[count]);
        if(item && item->isVisible()) {

            query.execSelect(modeltable->sql_cur + QString(" and y.ch_tableno = '%1' ")
                             .arg(item->data(table_management_desk::item_ch_tableno).toString())
                             );
            query.next();
            item->paint_cell(query.record(),qobject_cast<restraurantDelegate *>(ui->tableView_ch_tableno->itemDelegate()));
        }
    }
}

void w_scr_dish_restaurant_dialog::map_desk_show_code()
{
    //    if(lds::sysconf->value("w_scr_dish_restaurant_parameter/map_show", "show_table_name").toString() == "show_table_code") {
    //        ui->graphicsView->is_desk_show_code = true;
    //    } else {
    //        ui->graphicsView->is_desk_show_code = false;
    //    }
}

void w_scr_dish_restaurant_dialog::toWine()
{
    fexpandmain_2::wineTake(this);
}

void w_scr_dish_restaurant_dialog::toqr_code_polling(QNetworkReply *reply)
{
    qr_code_polliing_count++;
    //
    if(qr_code_polling_has_pop) // won`t to check qr_code data when qr_code_polling_has_pop is true
        return;

    lds_query_hddpos  query;
    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray readall = reply->readAll();
    if(200 != httpStatus) {
        qr_code_polling_has_pop = true;
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QObject::tr("二维码点餐") + "\n" + QObject::tr("无法获取相关数据"));
        return;
    }
    cJSON_help root(readall);
    cJSON *array = cJSON_help::getcJSON(root, "data");
    for(int k = 0; k < cJSON_help::array_count(array); k ++) {
        cJSON *item = cJSON_GetArrayItem(array, k);
        cJSON_help::getcJSONvalue(item, "tableNo").toString();
        //3 已支付
        if("3" == cJSON_help::getcJSONvalue(item, "state").toString()) {
            query.execSelect(QString("select a.vch_qr_code_sn from cey_u_table a, cey_bt_table b where a.ch_billno = b.ch_billno and a.vch_qr_code_sn = '%1'; ")
                             .arg(cJSON_help::getcJSONvalue(item, "sn").toString()));
            if(query.next()) {//新的订单需要支付
                qr_code_polling_has_pop = true;
                break;
            }
        }
        //1新订单
        if("1" == cJSON_help::getcJSONvalue(item, "state").toString()) {
            query.execSelect(QString("select a.vch_qr_code_sn from cey_u_table a, cey_bt_table b where a.ch_billno = b.ch_billno and a.vch_qr_code_sn = '%1'; ")
                             .arg(cJSON_help::getcJSONvalue(item, "sn").toString()));
            if(!query.next()) {//新的订单未处理
                qr_code_polling_has_pop = true;
                break;
            }
        }
    }

    if(qr_code_polling_has_pop) {
        toqr_code_order_over_view();
    }
}

void w_scr_dish_restaurant_dialog::toqr_code_order_over_view()
{
    //    if(n_func::f_get_sysparm("qr_code_order_mode","0") == "0") {
    //        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("请在软件后台启用此功能"));
    //        return;
    //    }
    //    w_scr_dish_main_qr_code_over_view dialog(this);
    //    if(public_sql::GoRestaurantOrder == transparentCenterDialog(&dialog).exec()) {
    //    }
    //    torefresh_data();
}

void w_scr_dish_restaurant_dialog::closeEvent(QCloseEvent *e)
{
    QDialog::closeEvent(e);
}
