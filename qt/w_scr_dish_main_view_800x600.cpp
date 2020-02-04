#include "w_scr_dish_main_view_800x600.h"
#include "ui_w_scr_dish_main_view_800x600.h"
#include <QTimer>
#include "lds_menu.h"
#include "lds_messagebox.h"
#include "w_scr_dish_main_bar_cancel.h"
#include "w_scr_dish_main_bar_cancel_win.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "fexpandmain_2.h"
#include "ftableview_delegate.h"
#include "w_sys_manage_outer_pay_set_tax_type.h"

w_scr_dish_main_view_800x600::w_scr_dish_main_view_800x600(QWidget *parent) :
    w_scr_dish_main(lds_scr_table::Q000, "", windowTypeKuaiCan, parent),
    ui(new Ui::w_scr_dish_main_view_800x600)
{
    ui->setupUi(this);
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
    //widget
    ui->frame_deskinfo->setGeometry(0, 0, lds::PAY_LEFT_SPACE, 70);

    ui->frame_deskinfo->setTitleColor(&titleColor);
    int layout_border = ftableview_delegate::CELL_BORDER_SPACE;
    ui->frame_deskinfo->enableBottomBorder();

    ui->frame_bottom->setGeometry(lds::PAY_LEFT_SPACE + ftableview_delegate::CELL_BORDER_SPACE, lds::MAIN_WINDOW_SIZE.height() - lds::QT_FIXED_HEIGHT_BOTTOM, lds::MAIN_WINDOW_SIZE.width() - lds::PAY_LEFT_SPACE - ftableview_delegate::CELL_BORDER_SPACE, lds::QT_FIXED_HEIGHT_BOTTOM);
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

        ui->tableView_dish->updateTableRect(lds::PAY_LEFT_SPACE, 2 * lds::QT_FIXED_HEIGHT_BOTTOM, dish_width, lds::MAIN_WINDOW_SIZE.height() - 3 * lds::QT_FIXED_HEIGHT_BOTTOM,
                                            layout_border, 0, layout_border, layout_border);
        lds::hideWidget(ui->widget_paytotal);
    } else {
        ui->tableView->setGeometry(0, ui->frame_deskinfo->y() + ui->frame_deskinfo->height(), lds::PAY_LEFT_SPACE,  lds::MAIN_WINDOW_SIZE.height() - ui->frame_deskinfo->y() - ui->frame_deskinfo->height() - lds::QT_FIXED_HEIGHT_BOTTOM - 40);
        ui->widget_paytotal->setGeometry(0, lds::MAIN_WINDOW_SIZE.height() - lds::QT_FIXED_HEIGHT_BOTTOM - 40 + 4, lds::PAY_LEFT_SPACE, 40 - 4);
        lds::hideWidget(ui->frame_quickpay);
        lds::hideWidget(ui->label_quickpay_top);
        ui->tableView_dish->updateTableRect(lds::PAY_LEFT_SPACE, 2 * lds::QT_FIXED_HEIGHT_BOTTOM, dish_width, lds::MAIN_WINDOW_SIZE.height() - 3 * lds::QT_FIXED_HEIGHT_BOTTOM,
                                            layout_border, 0, layout_border, layout_border);
    }

    //    ui->tableView_type->updateTableRect(lds::PAY_LEFT_SPACE, 0, dish_width, lds::QT_FIXED_HEIGHT_BOTTOM, layout_border);
    //    ui->tableView_subtype->updateTableRect(lds::PAY_LEFT_SPACE, lds::QT_FIXED_HEIGHT_BOTTOM, dish_width, lds::QT_FIXED_HEIGHT_BOTTOM, layout_border);

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
    ui->label_total->setFocusPolicy(Qt::NoFocus);

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
//    ui->pushButton_dishname_switch->init("vch_dishname", "vch_dishname_2", lds::sysconf->value("w_scr_dish_main/dishnameswitch", "vch_dishname").toString());
    loadData();

    connect(ui->label_head_total, SIGNAL(clicked()), this, SLOT(toSwitchSkin()));
    connect(ui->label_head_num, SIGNAL(clicked()), this, SLOT(toScaleDishPic()));
    connect(public_sql::save_login_object(), SIGNAL(signal_TelNoComing(QString)), this, SLOT(toTelComing(QString)));

    connect(ui->pushButton_sys_tax, SIGNAL(clicked()), this,SLOT(toTaxExec()));

}

w_scr_dish_main_view_800x600::~w_scr_dish_main_view_800x600()
{
    qDebug() << __FILE__ << __LINE__ << __FUNCTION__;
    delete ui;
}

void w_scr_dish_main_view_800x600::menuhide()
{
    if(ui->pushButton_other->menu()) {
        ui->pushButton_other->menu()->hide();
    }
}

void w_scr_dish_main_view_800x600::updateTakebillState()
{
    lds_query_hddpos  query;
    query.execSelect("select count(0) from cey_u_hungbill where dt_operdate >= curdate() ");
    query.next();
    ui->pushButton_takebill->set_rt_checked(query.recordValue(0).toInt() > 0);
}

void w_scr_dish_main_view_800x600::updateTakebillDesc()
{
    qDebug() << __FILE__ << __LINE__ << tablemodel->rowCount();
    if(tablemodel->rowCount() == 0) {
        ui->pushButton_takebill->setText(QObject::tr("取单"));
    } else {
        ui->pushButton_takebill->setText(QObject::tr("挂单"));
    }
}

void w_scr_dish_main_view_800x600::retranslateView()
{
    ui->retranslateUi(this);
    ui->widget_total->setLabelText(QObject::tr("合计"));
    ui->widget_change->setLabelText(QObject::tr("找零"));
    ui->label_total_desc->setText(QObject::tr("总计"));
}

void w_scr_dish_main_view_800x600::loadData()
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
        connect(ui->pushButton_wine, SIGNAL(clicked()), this, SLOT(toWine()));
        connect(ui->frame_dish_bottom2->desk, SIGNAL(clicked()), this, SLOT(toRestaurant()));
        connect(ui->frame_dish_bottom2->waidai, SIGNAL(clicked()), this, SLOT(toWaiDai()));
        ui->frame_dish_bottom2->addMenuInitAndShow();
    }
    {
        connect(ui->pushButton_sys_yingyedanju,SIGNAL(clicked()),this,SLOT(tosysyingyedanju()));
        connect(ui->pushButton_language,SIGNAL(keyChanged()),this,SLOT(toLanguageSwitch()));
        connect(ui->pushButton_language,SIGNAL(timeout()),this,SLOT(toLanguageSet()));
        connect(ui->pushButton_dishname_switch,SIGNAL(clicked()),this,SLOT(toDishnameSet()));
        connect(ui->pushButton_sys_yingyeqingkuang,SIGNAL(clicked()),this,SLOT(tosysyingyeqingkuang()));
        connect(ui->pushButton_sys_jiaoban,SIGNAL(clicked()),this,SLOT(tosysjiaoban()));
        connect(ui->pushButton_sys_chongzhi,SIGNAL(clicked()),this,SLOT(tosyschongzhi()));
        connect(ui->pushButton_sys_qukuan,SIGNAL(clicked()),this,SLOT(tosysqukuan()));
        connect(ui->pushButton_sys_xiugaimima,SIGNAL(clicked()),this,SLOT(tosysxiugaimima()));
        connect(ui->pushButton_sys_switchback,SIGNAL(clicked()),this,SLOT(tosysswitchback()));
        connect(ui->pushButton_sys_kaiqianxiang,SIGNAL(clicked()),this,SLOT(tosyskaiqianxiang()));
        connect(ui->pushButton_changyong,SIGNAL(clicked()), this, SLOT(tochangyongcai()));
        connect(ui->pushButton_sys_suoping, SIGNAL(clicked()),this,SLOT(tosyssuoping()));
        connect(ui->pushButton_sys_guqing, SIGNAL(clicked()),this,SLOT(tosysguqing()));
        connect(ui->pushButton_tel_coming, SIGNAL(clicked()), this, SLOT(toTelComing()));
    }

    connect(ui->pushButton_del, SIGNAL(clicked()), this, SLOT(toquitdish()));
    connect(ui->pushButton_detail, SIGNAL(clicked()), this, SLOT(todetail()));
    connect(tablemodel, SIGNAL(signals_total(double)), this, SLOT(updateTotal(double)));
}


void w_scr_dish_main_view_800x600::orderdishRowChanged(const QModelIndex &, const QModelIndex &)
{
}

void w_scr_dish_main_view_800x600::updatememberchecked()
{
    ui->pushButton_member->set_rt_checked(tablemodel->vch_membernod);
    w_scr_dish_main::updatememberchecked();
}

