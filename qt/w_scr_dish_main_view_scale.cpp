#include "w_scr_dish_main_view_scale.h"
#include "ui_w_scr_dish_main_view_scale.h"
#include "lds_scr_table.h"
#include "lds_menu.h"
#include "public_sql.h"
#include "public_logindialog.h"
#include "frontviewdialog_virtual.h"
#include "fexpandmain_2.h"
#include "material_purchase_in_storage.h"
#include "material_loss_apply.h"
#include "material_purchase_apply.h"
#include "w_sys_manage_cloudsync_with.h"

w_scr_dish_main_view_scale::w_scr_dish_main_view_scale(QWidget *parent) :
    w_scr_dish_main(lds_scr_table::Q000, "", windowTypeKuaiCan, parent),
    ui(new Ui::w_scr_dish_main_view_scale),
    check_weight_count(0),
    stable_weight_count(5)
{
    ui->setupUi(this);

    ui->widget_scale_led->setFONT_DIGIT_Family(lds::FONT_DIGIT_Family);
    ui->widget_scale_led->appendCell("num_tare", "", "kg");
    ui->widget_scale_led->appendCell("num_num", "", "kg");
    ui->widget_scale_led->appendCell("num_price", "", "");
    ui->widget_scale_led->appendCell("num_total", "", "");

    ui->widget_scale_led->updateCellNumber("num_tare", "0.000");
    ui->widget_scale_led->updateCellNumber("num_num", "0.000");
    ui->widget_scale_led->updateCellNumber("num_price", "0.00");
    ui->widget_scale_led->updateCellNumber("num_total", "0.00");

    retranslateView();

    resize(lds::MAIN_WINDOW_SIZE);
    loadControl(
                ui->tableView_dish,
                ui->tableView_type,
                ui->tableView_subtype,
                ui->tableView,

                ui->label_logo,
                ui->label_text,
                ui->pushButton_pay,
                ui->widget_total,
                ui->widget_change,
                ui->label_total,
                ui->frame_deskinfo,
                ui->frame_dish_bottom2
                );


    //
    int led_w = 85;
    int led_h = 100;
    if(lds::MAIN_WINDOW_SIZE.width() == 800) {
        led_h = 86;
    }
    //
    lds::hideWidget(ui->label_shop_info);
    int shop_info_height = 0;
    QPixmap shop_info_pix("pos/shopinfo");
    if(false == shop_info_pix.isNull()) {
        lds::showWidget(ui->label_shop_info);
        shop_info_pix = shop_info_pix.scaledToWidth(lds::PAY_LEFT_SPACE, Qt::SmoothTransformation);
        ui->label_shop_info->setPixmap(shop_info_pix);
        shop_info_height = shop_info_pix.height();

        if(lds::MAIN_WINDOW_SIZE.width() == 1366) {
            ui->label_shop_info->setGeometry(0, 0, lds::PAY_LEFT_SPACE, shop_info_height);
        } else {
            ui->label_shop_info->setGeometry(0, led_h, lds::PAY_LEFT_SPACE, shop_info_height);
        }
    }

    int layout_border = ftableview_delegate::CELL_BORDER_SPACE;
    if(lds::MAIN_WINDOW_SIZE.width() == 1366) {
        ui->widget_scale_led->setGeometry(lds::PAY_LEFT_SPACE, 0, lds::MAIN_WINDOW_SIZE.width() - led_w - lds::PAY_LEFT_SPACE, led_h - layout_border);
        ui->frame_scale_op->setGeometry(lds::MAIN_WINDOW_SIZE.width() - led_w, 0, led_w, led_h -layout_border);
        ui->frame_deskinfo->setGeometry(0, shop_info_height, lds::PAY_LEFT_SPACE,  70);
    } else {
        ui->widget_scale_led->setGeometry(0, 0, lds::MAIN_WINDOW_SIZE.width() - led_w, led_h -layout_border);
        ui->frame_scale_op->setGeometry(lds::MAIN_WINDOW_SIZE.width() - led_w, 0, led_w, led_h - layout_border);
        ui->frame_deskinfo->setGeometry(0, led_h + shop_info_height, lds::PAY_LEFT_SPACE, 70);
    }
    QGridLayout *glayout = new QGridLayout;
    glayout->addWidget(ui->label_zhiling, 0, 0);ui->label_zhiling->setFixedWidth(15);
    glayout->addWidget(ui->label_qupi, 1, 0);ui->label_qupi->setFixedWidth(15);
    glayout->addWidget(ui->label_stable, 2, 0);ui->label_stable->setFixedWidth(15);

    glayout->addWidget(ui->pushButton_scale_zero, 0, 1);
    glayout->addWidget(ui->pushButton_scale_tare, 1, 1);
    glayout->addWidget(ui->pushButton_scale_stable, 2, 1);

    glayout->setSpacing(layout_border);
    glayout->setMargin(layout_border);

    ui->frame_scale_op->setLayout(glayout);

    ui->frame_deskinfo->setTitleColor(&titleColor);
    ui->frame_deskinfo->enableBottomBorder();

    ui->frame_bottom->setGeometry(lds::PAY_LEFT_SPACE + ftableview_delegate::CELL_BORDER_SPACE,  lds::MAIN_WINDOW_SIZE.height() - lds::QT_FIXED_HEIGHT_BOTTOM, lds::MAIN_WINDOW_SIZE.width() - lds::PAY_LEFT_SPACE - ftableview_delegate::CELL_BORDER_SPACE, lds::QT_FIXED_HEIGHT_BOTTOM);
    ui->frame_dish_bottom2->setGeometry(0, lds::MAIN_WINDOW_SIZE.height() - ui->frame_bottom->height(), lds::PAY_LEFT_SPACE,  lds::QT_FIXED_HEIGHT_BOTTOM);

    ui->label_head_num->setFixedWidth(ui->tableView->numRect().width());
    ui->label_head_total->setFixedWidth(ui->tableView->totalRect().width());

    int dish_width = lds::MAIN_WINDOW_SIZE.width() - lds::PAY_LEFT_SPACE;
    //快捷模式
    if(lds::sysconf->value("w_sys_manage_outer_pay_set/quickpay", false).toBool()) {
        int h = lds::MAIN_WINDOW_SIZE.height() - ui->frame_deskinfo->y() - ui->frame_deskinfo->height() - lds::QT_FIXED_HEIGHT_BOTTOM;
        int order_height = h  * 0.55;
        int quick_height = h - order_height;
        ui->tableView->setGeometry(0, ui->frame_deskinfo->y() + ui->frame_deskinfo->height(), lds::PAY_LEFT_SPACE,  order_height);
        ui->label_quickpay_top->setGeometry(0, lds::MAIN_WINDOW_SIZE.height() - lds::QT_FIXED_HEIGHT_BOTTOM - quick_height, lds::PAY_LEFT_SPACE,  2);
        ui->frame_quickpay->setGeometry(0, lds::MAIN_WINDOW_SIZE.height() - lds::QT_FIXED_HEIGHT_BOTTOM - quick_height + 2, lds::PAY_LEFT_SPACE,  quick_height - 2);
        ui->tableView_dish->updateTableRect(lds::PAY_LEFT_SPACE, led_h + 2 * lds::QT_FIXED_HEIGHT_BOTTOM, dish_width, lds::MAIN_WINDOW_SIZE.height() - 3 * lds::QT_FIXED_HEIGHT_BOTTOM - led_h,
                                            layout_border, 0, layout_border, layout_border);
        lds::showWidget(ui->widget_paytotal, false);
    } else {
        ui->tableView->setGeometry(0, ui->frame_deskinfo->y() + ui->frame_deskinfo->height(), lds::PAY_LEFT_SPACE,  lds::MAIN_WINDOW_SIZE.height() - ui->frame_deskinfo->height() - lds::QT_FIXED_HEIGHT_BOTTOM - ui->frame_deskinfo->y() - 40);
        lds::hideWidget(ui->frame_quickpay);
        lds::hideWidget(ui->label_quickpay_top);
        ui->tableView_dish->updateTableRect(lds::PAY_LEFT_SPACE, led_h + 2 * lds::QT_FIXED_HEIGHT_BOTTOM, dish_width, lds::MAIN_WINDOW_SIZE.height() - 3 * lds::QT_FIXED_HEIGHT_BOTTOM - led_h,
                                            layout_border, 0, layout_border, layout_border);
        lds::showWidget(ui->widget_paytotal, true);
        ui->widget_paytotal->setGeometry(0, lds::MAIN_WINDOW_SIZE.height() - lds::QT_FIXED_HEIGHT_BOTTOM - 40 + 4, lds::PAY_LEFT_SPACE, 40 - 4);
    }

    ui->frame_quickpay->layout()->setContentsMargins(layout_border, layout_border, 0, layout_border);
    ui->frame_quickpay->layout()->setSpacing(layout_border);
    ui->frame_bottom->layout()->setContentsMargins(0, 0, layout_border, layout_border);
    ui->frame_bottom->layout()->setSpacing(layout_border);
    ui->frame_dish_bottom2->layout()->setContentsMargins(layout_border, 0, 0, layout_border);
    ui->frame_dish_bottom2->layout()->setSpacing(layout_border);
    ui->widget_total->setBorder(0, 0, 0, 0);
    ui->widget_total->enableSelectAll();
    ui->widget_total->setBgColorLight(2);
    ui->widget_total->enableClickable();
    ui->widget_change->setBorder(0, 0, 0, 0);
    ui->widget_change->setBgColorLight(2);

    ui->tableView->setFocusPolicy(Qt::NoFocus);
    ui->tableView_type->setFocusPolicy(Qt::NoFocus);
    ui->tableView_subtype->setFocusPolicy(Qt::NoFocus);
    ui->tableView_dish->setFocusPolicy(Qt::NoFocus);

    //quick pay
    connect(ui->pushButton_12, SIGNAL(clicked()), this, SLOT(toQuickNum()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(toQuickNum()));
    connect(ui->pushButton_16, SIGNAL(clicked()), this, SLOT(toQuickNum()));
    connect(ui->pushButton_backspace, SIGNAL(clicked()), this, SLOT(toQuickNum()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(toQuickNum()));
    connect(ui->pushButton_8, SIGNAL(clicked()), this, SLOT(toQuickNum()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(toQuickNum()));
    connect(ui->pushButton_7, SIGNAL(clicked()), this, SLOT(toQuickNum()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(toQuickNum()));
    connect(ui->pushButton_18, SIGNAL(clicked()), this, SLOT(toQuickNum()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(toQuickNum()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(toQuickNum()));
    connect(ui->pushButton_10, SIGNAL(clicked()), this, SLOT(toQuickNum()));
    connect(ui->pushButton_19, SIGNAL(clicked()), this, SLOT(toQuickNum()));
    connect(ui->pushButton_14, SIGNAL(clicked()), this, SLOT(toQuickNum()));
    connect(ui->pushButton_11, SIGNAL(clicked()), this, SLOT(toQuickNum()));
    connect(ui->pushButton_mobile_pay, SIGNAL(clicked()), this, SLOT(to_moblie_paybill()));
    connect(ui->pushButton_mobile_pay, SIGNAL(timeout()), this, SLOT(to_moblie_payselect()));
    connect(ui->pushButton_cash_pay, SIGNAL(clicked()), this, SLOT(to_cash_paybill()));
    connect(ui->widget_total, SIGNAL(signalClicked()), this, SLOT(toshowPayInfo()));
    //~quick pay

    ui->pushButton_language->initKeys_conf();
    loadData();
    connect(ui->label_head_total, SIGNAL(clicked()), this, SLOT(toSwitchSkin()));
    connect(ui->label_head_num, SIGNAL(clicked()), this, SLOT(toScaleDishPic()));
    connect(public_sql::save_login_object(), SIGNAL(signal_TelNoComing(QString)), this, SLOT(toTelComing(QString)));

    //
    lds::DOUBLE_DATA ddata;
    ddata.udpKey = lds::udp_scale_init_request;
    foreach(const public_scale_led::cellData &d, ui->widget_scale_led->get_cell_list_ref()) {
        ddata.scale_keys += d.key + ",";
        ddata.scale_titles+= d.title + ",";
        ddata.scale_suffixs += d.suffix + ",";
    }
    ddata.scale_keys.chop(1);
    ddata.scale_titles.chop(1);
    ddata.scale_suffixs.chop(1);
    public_sql::doublescreensenddataByconf(ddata);

    QTimer::singleShot(100, this, SLOT(updategetweight()));
}



w_scr_dish_main_view_scale::~w_scr_dish_main_view_scale()
{
    delete ui;
}

void w_scr_dish_main_view_scale::menuhide()
{
    if(ui->pushButton_other->menu()) {
        ui->pushButton_other->menu()->hide();
    }
}

void w_scr_dish_main_view_scale::updateTakebillState()
{
    lds_query_hddpos  query;
    query.execSelect("select count(0) from cey_u_hungbill where dt_operdate >= curdate() ");
    query.next();
    ui->pushButton_takebill->set_rt_checked(query.recordValue(0).toInt() > 0);
}

void w_scr_dish_main_view_scale::updateTakebillDesc()
{
    if(tablemodel->rowCount() == 0) {
        ui->pushButton_takebill->setText(QObject::tr("取单"));
    } else {
        ui->pushButton_takebill->setText(QObject::tr("挂单"));
    }
}

void w_scr_dish_main_view_scale::retranslateView()
{
    ui->retranslateUi(this);
    ui->widget_total->setLabelText(QObject::tr("合计"));
    ui->widget_change->setLabelText(QObject::tr("找零"));
    ui->widget_scale_led->updateCellTitle("num_tare", "T "+QObject::tr("皮重"));
    ui->widget_scale_led->updateCellTitle("num_num", "NET "+QObject::tr("净重"));
    ui->widget_scale_led->updateCellTitle("num_price", QObject::tr("单价"));
    ui->widget_scale_led->updateCellTitle("num_total", QObject::tr("金额"));
}

void w_scr_dish_main_view_scale::loadData()
{

    w_scr_dish_main::loadData();
    //
    ui->frame_dish_bottom2->setup(ch_tableno, ui->tableView->verticalScrollBar());
    {//其他f
        lds::showWidget(ui->frame);
        lds_menu *menu=new lds_menu(this);
        QWidgetAction *widgetaction=new QWidgetAction(ui->pushButton_other);
        widgetaction->setDefaultWidget(ui->frame);
        menu->addAction(widgetaction);
        ui->pushButton_other->setMenu(menu);
    }
    {//编号
        lds_menu *menu=new lds_menu(this);
        ui->pushButton_bianma->setMenu(menu);
        connect(menu,SIGNAL(aboutToShow()), this,SLOT(showBianMa()));
    }
    //
    toUpdatePayBillButton();
    {
        connect(ui->pushButton_allbillcancel,SIGNAL(clicked()),this,SLOT(toAllbillcancel()));
        connect(ui->pushButton_change,SIGNAL(clicked()),this,SLOT(tochange()));
        connect(ui->pushButton_discount_bill,SIGNAL(clicked()),this,SLOT(toDiscountBill()));
        //        connect(ui->pushButton_del,SIGNAL(clicked()),this,SLOT(toquitdish()));
        connect(ui->pushButton_exitbill,SIGNAL(clicked()),this,SLOT(toExitbill()));

        //        connect(ui->pushButton_detail,SIGNAL(clicked()), this, SLOT(todetail()));
        connect(ui->pushButton_present,SIGNAL(clicked()),this,SLOT(topresent()));
        connect(ui->pushButton_mhangpay,SIGNAL(clicked()),this,SLOT(tomhangpay()));
        connect(ui->pushButton_takebill,SIGNAL(clicked()),this,SLOT(toTakebill()));
        connect(ui->pushButton_takeweight,SIGNAL(clicked()),this,SLOT(toTakeweight()));

        connect(ui->pushButton_pay,SIGNAL(clicked()),this,SLOT(topaybill()));
        connect(ui->pushButton_pay,SIGNAL(timeout()),this,SLOT(topaybillSelectPayMode()));
        connect(ui->pushButton_tuichu,SIGNAL(clicked()),this,SLOT(toTuichu()));
        connect(ui->pushButton_member,SIGNAL(clicked()),this,SLOT(toMRead()));
        connect(ui->frame_dish_bottom2, SIGNAL(signal_waimai()),this,SLOT(towaimai()));
        connect(ui->frame_dish_bottom2, SIGNAL(signal_waimaipay()),this,SLOT(towaimaipay()));
        //        connect(ui->pushButton_wine, SIGNAL(clicked()), this, SLOT(toWine()));
        connect(ui->frame_dish_bottom2->desk, SIGNAL(clicked()), this, SLOT(toRestaurant()));
        connect(ui->frame_dish_bottom2->waidai, SIGNAL(clicked()), this, SLOT(toWaiDai()));
        ui->frame_dish_bottom2->addMenuInitAndShow();
    }
    {
        connect(ui->pushButton_sys_yingyedanju,SIGNAL(clicked()),this,SLOT(tosysyingyedanju()));
        connect(ui->pushButton_language,SIGNAL(keyChanged()),this,SLOT(toLanguageSwitch()));
        connect(ui->pushButton_language,SIGNAL(timeout()),this,SLOT(toLanguageSet()));
        connect(ui->pushButton_sys_yingyeqingkuang,SIGNAL(clicked()),this,SLOT(tosysyingyeqingkuang()));
        connect(ui->pushButton_sys_jiaoban,SIGNAL(clicked()),this,SLOT(tosysjiaoban()));
        connect(ui->pushButton_sys_chongzhi,SIGNAL(clicked()),this,SLOT(tosyschongzhi()));
        connect(ui->pushButton_sys_qukuan,SIGNAL(clicked()),this,SLOT(tosysqukuan()));
        connect(ui->pushButton_sys_xiugaimima,SIGNAL(clicked()),this,SLOT(tosysxiugaimima()));
        connect(ui->pushButton_sys_switchback,SIGNAL(clicked()),this,SLOT(tosysswitchback()));
        connect(ui->pushButton_sys_kaiqianxiang,SIGNAL(clicked()),this,SLOT(tosyskaiqianxiang()));
        connect(ui->pushButton_changyong,SIGNAL(clicked()), this, SLOT(tochangyongcai()));
        connect(ui->pushButton_sys_suoping, SIGNAL(clicked()),this,SLOT(tosyssuoping()));
        //        connect(ui->pushButton_sys_guqing, SIGNAL(clicked()),this,SLOT(tosysguqing()));
        connect(ui->pushButton_tel_coming, SIGNAL(clicked()), this, SLOT(toTelComing()));
        connect(ui->pushButton_dishname_switch, SIGNAL(clicked()) ,this, SLOT(toDishnameSet()));
    }

    connect(ui->pushButton_del, SIGNAL(clicked()), this, SLOT(toquitdish()));
    connect(ui->pushButton_detail, SIGNAL(clicked()), this, SLOT(todetail()));
    connect(tablemodel, SIGNAL(signals_total(double)), this, SLOT(updateTotal(double)));
    //

    ElectronicCountPrice_resolve::open(
                lds::sysconf->value("system_setting/dianzichengxinghao").toString(),
                &scale_printer,
                lds::sysconf->value("system_setting/dianzichengduankou").toString(),
                lds::sysconf->value("system_setting/dianzicheng_baudrate").toInt());

    connect(ui->pushButton_scale_tare, SIGNAL(clicked()), this, SLOT(toqupi()));
    connect(ui->pushButton_scale_zero, SIGNAL(clicked()), this, SLOT(tozhiling()));
    connect(ui->pushButton_scale_stable, SIGNAL(clicked()), this, SLOT(tostable()));

    //.
    connect(ui->pushButton_cloud_inventory_in, SIGNAL(clicked()), this, SLOT(to_cloud_inventory_in()));
    connect(ui->pushButton_cloud_download, SIGNAL(clicked()), this, SLOT(to_cloud_download()));
    connect(ui->pushButton_cloud_upload, SIGNAL(clicked()), this, SLOT(to_cloud_upload()));
    connect(ui->pushButton_cloud_inventory_out, SIGNAL(clicked()), this, SLOT(to_cloud_inventory_out()));
    connect(ui->pushButton_cloud_inventory_request, SIGNAL(clicked()), this, SLOT(to_cloud_inventory_request()));
    connect(ui->pushButton_cloud_flow_upload, SIGNAL(clicked()), this, SLOT(to_cloud_flow_upload()));

}

void w_scr_dish_main_view_scale::orderdishRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    ui->widget_scale_led->updateCellNumber("num_price", QString().sprintf("%.02f", tablemodel->model_data(current.row(), "num_price").toDouble()));
    ui->widget_scale_led->updateCellNumber("num_total", QString().sprintf("%.02f", tablemodel->getKeepdata(current.row()).dish_total()));

    lds::DOUBLE_DATA ddata;
    ddata.udpKey = lds::udp_scale_update_request;
    ddata.scale_numbers = (QStringList()
                           << ui->widget_scale_led->cellNumber("num_tare")
                           << ui->widget_scale_led->cellNumber("num_num")
                           << ui->widget_scale_led->cellNumber("num_price")
                           << ui->widget_scale_led->cellNumber("num_total")
                           ).join(",");
    public_sql::doublescreensenddataByconf(ddata);
}

void w_scr_dish_main_view_scale::updatememberchecked()
{
    ui->pushButton_member->set_rt_checked(tablemodel->vch_membernod);
    w_scr_dish_main::updatememberchecked();
}

void w_scr_dish_main_view_scale::updateTotal(double total)
{
    w_scr_dish_main::updateTotal(total);
}

bool w_scr_dish_main_view_scale::getWeightValue(int row, double &w)
{
    w = ui->widget_scale_led->cellNumber("num_num").toDouble();
    return true;
}

bool w_scr_dish_main_view_scale::dishAppend(const QString &ch_dishno, const vch_print_memoData &vch_print_memo, double num_price, int int_discount, const bool time_promotion)
{
    if(tablemodel->rowCount() > 0) {
        QString flag = tablemodel->model_data(tablemodel->rowCount() - 1, 0, Qt::UserRole + 1).toString();
        if(flag == "N") {//称重商品未稳定
            return false;
        }
    }
    //
    bool retv = w_scr_dish_main::dishAppend(ch_dishno, vch_print_memo, num_price, int_discount, time_promotion);
    if(retv) {
        int row = tablemodel->rowCount() - 1;
        if(tablemodel->get_cey_bt_dish_value(tablemodel->model_data(row, "ch_dishno").toString(), "ch_weight").toString() == "Y") {
            tablemodel->model_data_set(row, 0, "N", Qt::UserRole + 1);
            //
            fexpandmain_keepData d = tablemodel->getKeepdata(row);
            ui->widget_scale_led->updateCellNumber("num_price", QString().sprintf("%.02f",
                                                                                  lds::doubleRound(num_price,
                                                                                                   tablemodel->get_cey_sys_parameter_value("bp_fmt_price_dec", 2).toInt()))
                                                   );
            ui->widget_scale_led->updateCellNumber("num_total", QString().sprintf("%.02f",
                                                                                  lds::doubleRound(d.consum,
                                                                                                   tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt()))
                                                   );
        }
        if(ui->pushButton_scale_stable->isEnabled()) {
            tablemodel->model_data_set(row, 0, "Y", Qt::UserRole + 1);
        }
    }
    return retv;
}

void w_scr_dish_main_view_scale::toqupi()
{
    ui->widget_scale_led->updateCellNumber("num_tare",
                                           QString().sprintf("%.03f", ui->widget_scale_led->cellNumber("num_num").toDouble() + ui->widget_scale_led->cellNumber("num_tare").toDouble())
                                           );
    scale_printer.writedirect(ElectronicCountPrice_resolve::ins_qupi(lds::sysconf->value("system_setting/dianzichengxinghao").toString()));
}

void w_scr_dish_main_view_scale::tozhiling()
{
    scale_printer.writedirect(ElectronicCountPrice_resolve::ins_zhiling(lds::sysconf->value("system_setting/dianzichengxinghao").toString()));
}

void w_scr_dish_main_view_scale::tostable()
{
    int row = ui->tableView->currentIndex().row();
    if(row >= 0) {
        if(tablemodel->model_data(row, 0, Qt::UserRole + 1).toString() != "") {
            tablemodel->model_data_set(row, "num_num", ui->widget_scale_led->cellNumber("num_num"));
            tablemodel->model_data_set(row, 0, "Y", Qt::UserRole + 1);
            ui->tableView->update(tablemodel->index(row, 0));
        }
    }
}

void w_scr_dish_main_view_scale::quipistate(bool f)
{
    if(f) {
        ui->label_qupi->setStyleSheet(QString("background:%1;").arg(ui->widget_scale_led->greenColor().name()));
    } else {
        ui->label_qupi->setStyleSheet("background:lightgray;");
    }
}

void w_scr_dish_main_view_scale::lingweistate(bool f)
{
    if(f) {
        ui->label_zhiling->setStyleSheet(QString("background:%1;").arg(ui->widget_scale_led->greenColor().name()));
    } else {
        ui->label_zhiling->setStyleSheet("background:lightgray;");
    }
}

void w_scr_dish_main_view_scale::stablestate(bool scale_stable)
{
    ui->label_stable->setStyleSheet("background:lightgray;");

    if(!ui->tableView->currentIndex().isValid()) {
        ui->pushButton_scale_stable->setText(QObject::tr("无商品"));
        ui->pushButton_scale_stable->setEnabled(false);
        return;
    }
    bool ok;
    double curNum = ui->widget_scale_led->cellNumber("num_num").toDouble(&ok);
    if(false == ok) {
        ui->pushButton_scale_stable->setText(QObject::tr("无称重"));
        ui->pushButton_scale_stable->setEnabled(false);
        return;
    }
    if(curNum <= 0) {
        ui->pushButton_scale_stable->setText(QObject::tr("称重为零"));
        ui->pushButton_scale_stable->setEnabled(false);
        return;
    }
    if(false == scale_stable) {
        ui->pushButton_scale_stable->setText(QObject::tr("不稳定"));
        ui->pushButton_scale_stable->setEnabled(false);
        return;
    }
    if(true == scale_stable) {
        ui->label_stable->setStyleSheet(QString("background:%1;").arg(ui->widget_scale_led->greenColor().name()));

        ui->pushButton_scale_stable->setText(QObject::tr("确定"));
        ui->pushButton_scale_stable->setEnabled(true);
        return;
    }
}

void w_scr_dish_main_view_scale::updategetweight()
{

    int row = ui->tableView->currentIndex().row();
#ifdef QT_DEBUG
#else
    if(scale_printer.bytesAvailable())
#endif
    {
        QWidget *aw = qApp->activeWindow();
        if(aw
                && QString("w_scr_dish_main_view_scale") == aw->metaObject()->className()) {

            ElectronicCountPrice_resolve::resolveData qupi;
            ElectronicCountPrice_resolve::resolveData lingwei;
            ElectronicCountPrice_resolve::resolveData scale_weight;
            ElectronicCountPrice_resolve::resolveData scale_tare;
            ElectronicCountPrice_resolve::resolve(
                        lds::sysconf->value("system_setting/dianzichengxinghao").toString(),
                        scale_printer.readAll(),
                        qupi,
                        lingwei,
                        scale_weight,
                        scale_tare);
            //称重
            if(false == scale_weight.isValid) {
                ui->widget_scale_led->updateCellNumber("num_num", "--.---");
            }
            if(scale_weight.isValid) {
                QString num_num = QString().sprintf("%.03f",
                                                    lds::doubleRound(scale_weight.doubleValue,
                                                                     tablemodel->get_cey_sys_parameter_value("bp_fmt_num_dec", 3).toInt())
                                                    );
                if(num_num != ui->widget_scale_led->cellNumber("num_num")) {
                    //
                    check_weight_count = 0;
                    //
                    ui->widget_scale_led->updateCellNumber("num_num", num_num);
                    ui->widget_scale_led->updateCellNumber("num_total", QString().sprintf("%.02f",
                                                                                          lds::doubleRound(ui->widget_scale_led->cellNumber("num_num").toDouble() * ui->widget_scale_led->cellNumber("num_price").toDouble(),
                                                                                                           tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt()))
                                                           );
                    if(row >= 0) {
                        if(tablemodel->model_data(row, 0, Qt::UserRole + 1).toString() == "N") {
                            tablemodel->model_data_set(row, "num_num", num_num);
                            ui->tableView->update(tablemodel->index(row, 0));

                            fexpandmain_keepData d = tablemodel->getKeepdata(row);
                            ui->widget_scale_led->updateCellNumber("num_total", QString().sprintf("%.02f",
                                                                                                  lds::doubleRound(d.consum,
                                                                                                                   tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt()))
                                                                   );
                        }
                    }
                } else {
                    check_weight_count ++;
                }
            }
            //
            stablestate( check_weight_count >= stable_weight_count );
            if(ui->pushButton_scale_stable->isEnabled()) {
                if(row >= 0) {
                    if(tablemodel->model_data(row, 0, Qt::UserRole + 1).toString() == "N") {
                        tablemodel->model_data_set(row, 0, "Y", Qt::UserRole + 1);
                    }
                }
            }

            if(row >= 0) {
                if(tablemodel->model_data(row, 0, Qt::UserRole + 1).toString() == "N") {
                    setScaleTableOk(false);
                }
                if(tablemodel->model_data(row, 0, Qt::UserRole + 1).toString() == "Y") {
                    setScaleTableOk(true);
                }
            }

            //皮重
            if(false == scale_tare.isValid) {
                ui->widget_scale_led->updateCellNumber("num_tare", "--.---");
            }
            if(scale_tare.isValid) {
                QString num_tare= QString().sprintf("%.03f",
                                                    lds::doubleRound(scale_tare.doubleValue,
                                                                     tablemodel->get_cey_sys_parameter_value("bp_fmt_num_dec", 3).toInt())
                                                    );
                ui->widget_scale_led->updateCellNumber("num_tare", num_tare);
            }
            //去皮
            if(qupi.isValid)
                quipistate(qupi.boolValud);

            //置零
            if(lingwei.isValid)
                lingweistate(lingwei.boolValud);

            //副屏
            //            lds::DOUBLE_DATA ddata;
            //            ddata.udpKey = lds::udp_scale_update_request;
            //            ddata.scale_numbers = (QStringList()
            //                                   << ui->widget_scale_led->cellNumber("num_tare")
            //                                   << ui->widget_scale_led->cellNumber("num_num")
            //                                   << ui->widget_scale_led->cellNumber("num_price")
            //                                   << ui->widget_scale_led->cellNumber("num_total")
            //                                   ).join(",");
            //            public_sql::doublescreensenddataByconf(ddata);
        }
    }
    scale_printer.writedirect(ElectronicCountPrice_resolve::ins_quzhong(lds::sysconf->value("system_setting/dianzichengxinghao").toString()));
    QTimer::singleShot(100, this, SLOT(updategetweight()));
}

void w_scr_dish_main_view_scale::updategetTare(const QString &w)
{
    QString num_num = w.split(".").value(0)+ "." + w.split(".").value(1).leftJustified(3, '0', true);
    if(num_num != ui->widget_scale_led->cellNumber("num_tare")) {
        ui->widget_scale_led->updateCellNumber("num_tare", num_num);
    }
}

void w_scr_dish_main_view_scale::toCheckStable()
{
    QWidget *aw = qApp->activeWindow();
    if(aw
            && QString("w_scr_dish_main_view_scale") == aw->metaObject()->className()) {
    } else {
        return;
    }

    if(stable_before_weight != ui->widget_scale_led->cellNumber("num_num")) {
        check_weight_count = 0;
        stable_before_weight = ui->widget_scale_led->cellNumber("num_num");
    } else {
        check_weight_count ++;
        ui->label_stable->setNum(check_weight_count);
    }
    //
    int row = ui->tableView->currentIndex().row();

    stablestate( check_weight_count >= stable_weight_count );
    if(ui->pushButton_scale_stable->isEnabled()) {
        if(row >= 0) {
            if(tablemodel->model_data(row, 0, Qt::UserRole + 1).toString() == "N") {
                tablemodel->model_data_set(row, 0, "Y", Qt::UserRole + 1);
            }
        }
    }

    if(row >= 0) {
        if(tablemodel->model_data(row, 0, Qt::UserRole + 1).toString() == "N") {
            setScaleTableOk(false);
        }
        if(tablemodel->model_data(row, 0, Qt::UserRole + 1).toString() == "Y") {
            setScaleTableOk(true);
        }
    }
}

void w_scr_dish_main_view_scale::to_cloud_inventory_in()
{
    material_purchase_in_storage dialog(this);
    dialog.setWindowTitle(QObject::tr("门店入库"));
    transparentCenterDialog(&dialog).exec();
}

void w_scr_dish_main_view_scale::to_cloud_download()
{
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("本操作将会清空本地所有基础资料"), QObject::tr("确定"), QObject::tr("取消"))) {
        return;
    }
    QString errstring;
    lds_messagebox_loading_show loading(this, QObject::tr("下载商品"));
    loading.show_delay();
    if(w_sys_manage_cloudsync_with::data_download(&loading, true, errstring)) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
    } else {
        lds_messagebox::warning(this,MESSAGE_TITLE_VOID, errstring);
    }
}

void w_scr_dish_main_view_scale::to_cloud_upload()
{
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确定上传"), QObject::tr("确定"), QObject::tr("取消"))) {
        return;
    }
    QString errstring;
    lds_messagebox_loading_show loading(this, QObject::tr("上传商品"));
    loading.show_delay();
    if(w_sys_manage_cloudsync_with::data_upload(&loading, true, errstring)) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
    } else {
        lds_messagebox::warning(this,MESSAGE_TITLE_VOID, errstring);
    }
}

void w_scr_dish_main_view_scale::to_cloud_inventory_out()
{
    material_loss_apply dialog(this);
    dialog.setWindowTitle(QObject::tr("门店出库"));
    transparentCenterDialog(&dialog).exec();
}

void w_scr_dish_main_view_scale::to_cloud_inventory_request()
{
    material_purchase_apply dialog(this);
    dialog.setWindowTitle(QObject::tr("采购申请"));
    transparentCenterDialog(&dialog).exec();
}

void w_scr_dish_main_view_scale::to_cloud_flow_upload()
{
    switch(lds_messagebox::question(this,MESSAGE_TITLE_VOID, QObject::tr("流水上传"), QObject::tr("全部流水上传"), QObject::tr("未上传流水上传"), QObject::tr("取消"))) {
    case 0:
    {
        if(1 == lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("全部流水将置为未上传状态，流水越多花时间越多"), QObject::tr("继续"), QObject::tr("取消"))) {
            return;
        }
        lds_messagebox_loading_show loading(this, QObject::tr("上传流水"));
        loading.show_delay();
        if(false == w_sys_manage_cloudsync_with_time_running::flow_data_clearAll_Y_transaction(this, &loading)) {
            return;
        }
    }
        break;
    case 1:
        break;
    default:
        return;
    }

    w_sys_manage_cloudsync_with_time_running dialog(w_sys_manage_cloudsync_with_time_running::cloud_flow_up);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
}

void w_scr_dish_main_view_scale::setScaleTableOk(bool enable)
{
    ui->tableView->setEnabled(enable);
    ui->tableView_dish->setEnabled(enable);
    ui->tableView_subtype->setEnabled(enable);
    ui->tableView_type->setEnabled(enable);
    ui->frame_bottom->setEnabled(enable);
}
