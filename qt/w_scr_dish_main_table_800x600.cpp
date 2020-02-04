#include "w_scr_dish_main_table_800x600.h"
#include "ui_w_scr_dish_main_table_800x600.h"
#include <QTimer>
#include "lds_menu.h"
#include "lds_roundeddialog_rect_pop.h"
#include "n_func.h"
#include "w_scr_dish_pay_widget.h"
#include "back_view_dialog.h"
#include "public_sql.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "lds_dialog_input_double.h"
#include "ui_lds_dialog_input_dialog_double.h"
#include "w_scr_dish_select_operid.h"
#include "public_printby_ch_billno.h"
#include "ftableview_delegate.h"

w_scr_dish_main_table_800x600::w_scr_dish_main_table_800x600(const QString &ch_tableno, const QString &ch_billno, QWidget *parent, const w_m_member_telComing_table::telComingData &member_from_tel)
    :  w_scr_dish_main(ch_tableno, ch_billno, windowTypeZhongCan, parent)
{
    ui = new Ui::w_scr_dish_main_table_800x600;
    ui->setupUi(this);

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
    ui->frame_deskinfo->setGeometry(0, 0, lds::PAY_LEFT_SPACE, 130);

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
        int h = lds::MAIN_WINDOW_SIZE.height() - ui->frame_deskinfo->height() - lds::QT_FIXED_HEIGHT_BOTTOM;
        int order_height = h  * 0.55;
        int quick_height = h - order_height;
        ui->tableView->setGeometry(0, ui->frame_deskinfo->height(), lds::PAY_LEFT_SPACE,  order_height);
        ui->label_quickpay_top->setGeometry(0, lds::MAIN_WINDOW_SIZE.height() - lds::QT_FIXED_HEIGHT_BOTTOM - quick_height, lds::PAY_LEFT_SPACE,  2);
        ui->frame_quickpay->setGeometry(0, lds::MAIN_WINDOW_SIZE.height() - lds::QT_FIXED_HEIGHT_BOTTOM - quick_height + 2, lds::PAY_LEFT_SPACE,  quick_height - 2);

        ui->tableView_dish->updateTableRect(lds::PAY_LEFT_SPACE, 2 * lds::QT_FIXED_HEIGHT_BOTTOM, dish_width, lds::MAIN_WINDOW_SIZE.height() - 3 * lds::QT_FIXED_HEIGHT_BOTTOM,
                                            layout_border, 0, layout_border, layout_border);
        lds::hideWidget(ui->widget_paytotal);
    } else {
        ui->tableView->setGeometry(0, ui->frame_deskinfo->height(), lds::PAY_LEFT_SPACE,  lds::MAIN_WINDOW_SIZE.height() - ui->frame_deskinfo->height() - lds::QT_FIXED_HEIGHT_BOTTOM - 40);
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
    ui->widget_total->setLabelText(QObject::tr("合计"));
    ui->widget_total->enableSelectAll();
    ui->widget_total->setBgColorLight(2);
    ui->widget_total->enableClickable();
    ui->widget_change->setBorder(0, 0, 0, 0);
    ui->widget_change->setLabelText(QObject::tr("找零"));
    ui->widget_change->setBgColorLight(2);
    ui->label_total_desc->setText(QObject::tr("总计"));

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
    connect(ui->pushButton_cash_pay, SIGNAL(clicked()), this, SLOT(to_cash_paybill()));
    connect(ui->widget_total, SIGNAL(signalClicked()), this, SLOT(toshowPayInfo()));
    //~quick pay

//    ui->pushButton_dishname_switch->init("vch_dishname", "vch_dishname_2", lds::sysconf->value("w_scr_dish_main/dishnameswitch", "vch_dishname").toString());
//    ui->pushButton_dishname_switch->setValues(QObject::tr("名称"), QObject::tr("名称2"));

    lds::hideWidget(ui->frame_cuicai);
    lds::hideWidget(ui->frame_jiaodan);
    lds::hideWidget(ui->frame_other_menu);

    loadData();
    if(member_from_tel.vch_memberno.length() > 0) {
        fexpandmain_2::m_read_exec(this, tablemodel,  member_from_tel.vch_memberno);
        updatememberchecked();
    }
    connect(ui->label_head_total, SIGNAL(clicked()), this, SLOT(toSwitchSkin()));
    connect(ui->pushButton_sys_tax, SIGNAL(clicked()), this, SLOT(toTaxExec()));
}

w_scr_dish_main_table_800x600::~w_scr_dish_main_table_800x600()
{
    delete ui;
}

void w_scr_dish_main_table_800x600::menuhide()
{
    ui->pushButton_other->menu()->hide();
    ui->pushButton_bianma->menu()->hide();
}

void w_scr_dish_main_table_800x600::updateTakebillState()
{
}

void w_scr_dish_main_table_800x600::updateTakebillDesc()
{
}

void w_scr_dish_main_table_800x600::retranslateView()
{
    ui->retranslateUi(this);
}

void w_scr_dish_main_table_800x600::loadData()
{
    w_scr_dish_main::loadData();
    //
    ui->frame_dish_bottom2->setup(ch_tableno, ui->tableView->verticalScrollBar());
    {//frame_cuicai
        lds_menu *menu=new lds_menu(this);
        ui->pushButton_cuicai->setMenu(menu);
        menu->setPopWidget(ui->frame_cuicai);
    }
    {//frame_jiaodan
        lds_menu *menu=new lds_menu(this);
        ui->pushButton_jiaodan->setMenu(menu);
        menu->setPopWidget(ui->frame_jiaodan);
    }
    {//编号
        lds_menu *menu=new lds_menu(this);
        ui->pushButton_bianma->setMenu(menu);
        connect(menu,SIGNAL(aboutToShow()), this,SLOT(showBianMa()));
    }
    {
        lds_menu *menu=new lds_menu(this);
        ui->pushButton_other->setMenu(menu);
        menu->setPopWidget(ui->frame_other_menu);
    }

    connect(ui->pushButton_tuichu,SIGNAL(clicked()),this,SLOT(toTuichu()));
    connect(ui->pushButton_change,SIGNAL(clicked()),this,SLOT(tochange()));
    connect(ui->pushButton_discount_bill,SIGNAL(clicked()),this,SLOT(toDiscountBill()));
    //    connect(ui->pushButton_detail,SIGNAL(clicked()), this, SLOT(todetail()));
    connect(ui->pushButton_present,SIGNAL(clicked()),this,SLOT(topresent()));
    connect(ui->label_btn_vch_member,SIGNAL(clicked()),this,SLOT(toReadMember()));
    connect(ui->label_vch_member, SIGNAL(clicked()), this, SLOT(tochangeMemberShowFormat()));
    connect(ui->pushButton_paysomedish, SIGNAL(clicked()), this, SLOT(toPaySomeDish()));
    connect(ui->pushButton_print, SIGNAL(clicked()), this, SLOT(toprint()));
    connect(ui->pushButton_reprint, SIGNAL(clicked()), this, SLOT(toreprint()));


    connect(ui->pushButton_guaqi, SIGNAL(clicked()), this, SLOT(toguaqi()));connect(ui->pushButton_guaqi, SIGNAL(clicked()), this, SLOT(to_menu_special_reject()));


    connect(ui->pushButton_cuicai1, SIGNAL(clicked()), this, SLOT(tocuicaipiece()));connect(ui->pushButton_cuicai1, SIGNAL(clicked()), this, SLOT(to_menu_special_reject()));
    connect(ui->pushButton_cuicai2, SIGNAL(clicked()), this, SLOT(tocuicaitable()));connect(ui->pushButton_cuicai2, SIGNAL(clicked()), this, SLOT(to_menu_special_reject()));
    connect(ui->pushButton_cuicai3, SIGNAL(clicked()), this, SLOT(tocuicaitype()));connect(ui->pushButton_cuicai3, SIGNAL(clicked()), this, SLOT(to_menu_special_reject()));

    connect(ui->pushButton_jiaodan1, SIGNAL(clicked()), this, SLOT(tojiaodanpiece()));connect(ui->pushButton_jiaodan1, SIGNAL(clicked()), this, SLOT(to_menu_special_reject()));
    connect(ui->pushButton_jiaodan2, SIGNAL(clicked()), this, SLOT(tojiaodantable()));connect(ui->pushButton_jiaodan2, SIGNAL(clicked()), this, SLOT(to_menu_special_reject()));
    connect(ui->pushButton_jiaodan3, SIGNAL(clicked()), this, SLOT(tojiaodantype()));connect(ui->pushButton_jiaodan3, SIGNAL(clicked()), this, SLOT(to_menu_special_reject()));

    connect(ui->pushButton_dishname_switch,SIGNAL(clicked()),this,SLOT(toDishnameSet()));

    connect(ui->pushButton_dishtmp, SIGNAL(clicked()), this,SLOT(toDishTmp()));
    //    connect(ui->pushButton_quitdish, SIGNAL(clicked()), this, SLOT(toquitdish()));
    connect(ui->pushButton_changyong,SIGNAL(clicked()), this, SLOT(tochangyongcai()));
    connect(ui->pushButton_pay, SIGNAL(clicked()), this, SLOT(topaybill()));
    connect(ui->frame_dish_bottom2->desk, SIGNAL(clicked()), this, SLOT(toTuichu()));
    connect(ui->frame_dish_bottom2->waidai, SIGNAL(clicked()), this, SLOT(toWaiDai()));
    connect(ui->frame_dish_bottom2->waimai, SIGNAL(clicked()), this, SLOT(towaimai()));

    connect(ui->label_ch_tableno, SIGNAL(clicked()), this, SLOT(tochangeTableShowFormat()));
    connect(ui->label_btn_vch_waiter, SIGNAL(clicked()), this, SLOT(to_input_vch_waiter()));
    connect(ui->label_btn_int_person, SIGNAL(clicked()), this, SLOT(to_input_int_person()));
    connect(ui->label_btn_vch_memo, SIGNAL(clicked()), this, SLOT(to_input_vch_memo()));

    connect(tablemodel, SIGNAL(signals_num_total_change(QString,QString)), this, SLOT(to_switch_print_reprint()));
    connect(ui->pushButton_del, SIGNAL(clicked()), this, SLOT(toquitdish()));
    connect(ui->pushButton_detail, SIGNAL(clicked()), this, SLOT(todetail()));
    connect(tablemodel, SIGNAL(signals_total(double)), this, SLOT(updateTotal(double)));
    ///
    ui->label_vch_waiter->setText(tablemodel->get_cey_u_table_value(ch_billno, "vch_waiter").toString());
    ui->label_int_person->setText(tablemodel->get_cey_u_table_value(ch_billno, "int_person").toString());
    ui->label_vch_memo->setText(tablemodel->get_cey_u_table_value(ch_billno, "vch_memo").toString());

    table_format_index = lds::sysconf->value("w_scr_dish_main_table_800x600/table_format_index", 0).toInt();
    member_format_index = lds::sysconf->value("w_scr_dish_main_table_800x600/member_format_index", 0).toInt();
    changeTableShowFormat();

}

void w_scr_dish_main_table_800x600::orderdishRowChanged(const QModelIndex &current, const QModelIndex &)
{
    int row = current.row();
    if(row < 0) {
        return;
    }
    if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert) {
        ui->pushButton_del->setText(QObject::tr("删除"));
    } else {
        ui->pushButton_del->setText(QObject::tr("退货"));
    }
}

void w_scr_dish_main_table_800x600::updatememberchecked()
{
    changeMemberShowFormat();
    w_scr_dish_main::updatememberchecked();
}

void w_scr_dish_main_table_800x600::tochangeTableShowFormat()
{
    table_format_index++;
    table_format_index =  table_format_index % 3;
    changeTableShowFormat();
}

void w_scr_dish_main_table_800x600::tochangeMemberShowFormat()
{
    member_format_index++;
    member_format_index =  member_format_index % 3;
    changeMemberShowFormat();
}

void w_scr_dish_main_table_800x600::to_input_vch_waiter()
{
    w_scr_dish_select_operid dialog(ui->label_vch_waiter->text(), this);
    dialog.setWindowTitle(ui->label_btn_vch_waiter->text());
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        lds_query_hddpos::updateTable("cey_u_table", qrtVariantPairList() << qrtVariantPair("vch_waiter", dialog.selectOperid()), qrtEqual("ch_billno", ch_billno));
        ui->label_vch_waiter->setText(lds_query_hddpos::selectValue("select vch_waiter from cey_u_table where ch_billno = '%1' ", ch_billno).toString());
    }
}

void w_scr_dish_main_table_800x600::to_input_int_person()
{
    lds_dialog_input_double dialog(this);
    dialog.setWindowTitle(ui->label_btn_int_person->text());
    dialog.ui->lineEdit->setDecimals(0);
    dialog.ui->lineEdit->setValue(ui->label_int_person->text().toInt());
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        ui->label_int_person->setNum(dialog.ui->lineEdit->value());
        lds_query_hddpos  query;
        query.execUpdate("cey_u_table", "int_person", ui->label_int_person->text(), qrtEqual("ch_billno", ch_billno));
    }
}

void w_scr_dish_main_table_800x600::to_input_vch_memo()
{
    lds_dialog_input dialog(this);
    dialog.setWindowTitle(ui->label_btn_vch_memo->text());
    dialog.ui->label->setText(ui->label_btn_vch_memo->text());
    dialog.ui->lineEdit->setText(ui->label_vch_memo->text());
    dialog.hideKey();
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        ui->label_vch_memo->setText(dialog.ui->lineEdit->text());
        lds_query_hddpos  query;
        query.execUpdate("cey_u_table", "vch_memo", ui->label_vch_memo->text(), qrtEqual("ch_billno", ch_billno));
    }
}

void w_scr_dish_main_table_800x600::to_menu_special_reject()
{
    if(menu_special) {
        menu_special->reject();
    }
}

void w_scr_dish_main_table_800x600::to_switch_print_reprint()
{
    lds::hideWidget(ui->pushButton_reprint, tablemodel->rowCount() == 0 || tablemodel->model_is_dirty());
    lds::hideWidget(ui->pushButton_print, !ui->pushButton_reprint->isHidden());
}

void w_scr_dish_main_table_800x600::toreprint()
{
    public_printby_ch_billno printer(ch_billno);
    switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("预打单"), QObject::tr("打印并返回桌台"), QObject::tr("打印"), QObject::tr("取消"))) {
    case 0:
        printer.toprint_preprint(printer.defpath(), QObject::tr("预打单"));
        public_sql::preprinttablemap.insert(ch_billno, true);
        this->reject();
        break;
    case 1:
        printer.toprint_preprint(printer.defpath(), QObject::tr("预打单"));
        public_sql::preprinttablemap.insert(ch_billno, true);
        break;
    case 2:
        break;
    }
}

void w_scr_dish_main_table_800x600::toReadMember()
{
    toMRead();
    changeMemberShowFormat();
}

void w_scr_dish_main_table_800x600::changeTableShowFormat()
{
    switch(table_format_index) {
    case 1://name
        ui->label_ch_tableno->setText(tablemodel->get_cey_bt_table_value(ch_tableno, "vch_tablename").toString());
        break;
    case 2://name(no)
        ui->label_ch_tableno->setText(tablemodel->get_cey_bt_table_value(ch_tableno, "vch_tablename").toString() + "(" + ch_tableno + ")");
        break;
    default://no
        ui->label_ch_tableno->setText(ch_tableno);
        break;
    }
    lds::sysconf->setValue("w_scr_dish_main_table_800x600/table_format_index", table_format_index);
}

void w_scr_dish_main_table_800x600::changeMemberShowFormat()
{
    QString vch_memberno= tablemodel->vch_membernod.vch_memberno;
    QString vch_member = tablemodel->vch_membernod.vch_member;


    ui->label_vch_member->setText("");
    if(vch_memberno.length() == 0)
        return;
    if(tablemodel->vch_membernod.ch_member_belong_state == "Y") {
        ui->label_vch_member->setStyleSheet("color:#ff546b");
    } else {
        ui->label_vch_member->setStyleSheet("");
    }
    switch(member_format_index) {
    case 1://name
        ui->label_vch_member->setText(vch_member);
        break;
    case 2://name(no)
        ui->label_vch_member->setText(vch_member + "(" + vch_memberno + ")");
        break;
    default://no
        ui->label_vch_member->setText(vch_memberno);
        break;
    }
    lds::sysconf->setValue("w_scr_dish_main_table_800x600/member_format_index", member_format_index);
}
