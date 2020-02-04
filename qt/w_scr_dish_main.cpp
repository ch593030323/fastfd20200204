#include "w_scr_dish_main.h"
#include "back_view_dialog.h"
#include <QTimer>
#include <QWidgetAction>
#include "lds_menu.h"
#include "bianma_cook.h"
#include "ftableview_delegate.h"
#include "w_bt_dish_base.h"
#include "ui_w_bt_dish_new_dialog_2.h"
#include "fexpandmain_2.h"
#include "w_scr_dish_group_dialog.h"
#include "w_scr_dish_cook.h"
#include "take_weight.h"
#include "w_scr_dish_num_cook_dialog.h"
#include "w_bt_dish.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "w_scr_dish_pay_widget.h"
#include "w_scr_dish_dish_change.h"
#include "w_scr_dish_m_hang_pay_dialog.h"
#include "fast_qudan.h"
#include "takeout_cook.h"
#include "take_cashier.h"
#include "business_documents_query.h"
#include "w_scr_dish_language_switch_set.h"
#include "w_rpt_report_frame_business_summary.h"
#include "w_scr_dish_switch_detail.h"
#include "w_m_member_list.h"
#include "public_logindialog.h"
#include "modify_password.h"
#include "w_scr_dish_guqing_report.h"
#include "lock_screen.h"
#include "btnlanguageswicth.h"
#include <QLineEdit>
#include <QLabel>
#include "w_scr_dish_dish_quit.h"
#include "public_printby_ch_billno.h"
#include "linshi_cook.h"
#include "w_scr_dish_restaurant_paysomedish.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "w_scr_dish_cuicaipiece_choose.h"
#include "w_scr_dish_cuicai_choose.h"
#include "w_scr_dish_main_table_pay.h"
#include "w_scr_dish_pay_dialog.h"
#include "public_pushbutton_pay.h"
#include "w_m_member_telcoming.h"
#include "fexpandmain_dish_btn.h"
#include "ui_w_bt_dish_unitset_dialog.h"
#include "public_pushbuton_rt_check.h"
#include "ui_fexpandmain_payover.h"
#include "lds_dialog_input_double.h"
#include "ui_lds_dialog_input_dialog_double.h"
#include "ui_takeout_cook.h"
#include "lds_roundeddialog_title_frame.h"
#include "w_scr_dish_main_qr_code_over_view.h"
#include "w_sys_manage_outer_pay_set_tax_type.h"
#include "public_select_stringlist.h"
#include "w_scr_dish_main_separate.h"
#include "fexpandmain_model_sqltablemodel_data_dd.h"
#include "w_sys_manage_outer_dishname_show.h"
#include "public_select_stringlist.h"
#include "w_scr_dish_main_quit.h"
#include "ui_w_scr_dish_special_cook.h"

w_scr_dish_main::w_scr_dish_main(const QString &ch_tableno, const QString &ch_billno, windowType window_type, QWidget *parent)
    : lds_roundeddialog_rect0(parent)
{
    this->window_type = window_type;
    this->ch_tableno = ch_tableno;
    this->ch_billno = ch_billno;

    tablemodel = new fexpandmain_model_sqltablemodel_data_dd(this);//点菜部分
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("cey_u_orderdish");
    ///抓取促销信息  vch_parameter, vch_value
    tablemodel->tablename_kvmap_insert_sql("cey_sys_parameter", "vch_parameter", "vch_value", " vch_parameter like 'promotion_setting%' ", false,
                                           lds_model_sqltablemodel::SELECT_AUTO_REFRESH_KEY);
    model_type = new ftableview_standmodel_sql;
    model_subtype = new ftableview_standmodel_sql;
    model_dish = new ftableview_standmodel_sql(ftableview_standmodel_sql::TYPE_icon);

    barcode_scan = 0;
}

void w_scr_dish_main::initLogo()
{
    initLogo(label_logo, label_text);
}

void w_scr_dish_main::refreshChBillno()
{
    if(windowTypeKuaiCan == window_type) {
        torefresh_control_Q000();
    } else {
        torefresh_control(ch_billno);
    }
    //2 更新会员状态
    updatememberchecked();
}

void w_scr_dish_main::payBillFinished()
{
    QString old_ch_billno = tablemodel->cur_ch_billno();
    if(windowTypeKuaiCan == window_type) {
        refreshChBillno();
        tableView_dish->m->refreshcur();
        to_payfor_info_ch_billno(old_ch_billno, w_scr_dish_pay_widget::OPERAT_PAY);
    } else {
        tablemodelSelect("B1=2", this);//1=2必定为空
        to_payfor_info_ch_billno(old_ch_billno, w_scr_dish_pay_widget::OPERAT_PAY);
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("结算成功"), true);
    }

    if(windowTypeKuaiCan == window_type) {

    } else {//restaurant
        this->accept();
    }
}

void w_scr_dish_main::torefresh_dish()
{
    torefresh_dish_sql();
    model_type->toinit();
    model_subtype->toinitBySqlother("");
    //商品重新初始化,必须延时发送，否则会第一次会没有图片
    model_dish->initDelay();
}

void w_scr_dish_main::torefresh_dish_sql()
{
    //刷新 type subtype dish
    QString operfileter = QString(" ('0000'='%1' or 0 = (select count(0) from cey_bt_dish_type_oper b where b.vch_operid = '%1') or ch_dish_typeno in(select b.ch_dish_typeno from cey_bt_dish_type_oper b where b.ch_state = 'Y' and b.vch_operid='%1' )) ")
            .arg(lds::gs_operid);
    model_dish->imagedishdir = lds::sysconf->value("system_setting/bofanglujing").toString();
    model_type->sql_cur = QString("select '%1','%', '', '', '', '', '', '', '',''  union (select vch_dish_typename,ch_dish_typeno, '', '', '', '', '', '', '',vch_color  from cey_bt_dish_type where "+operfileter+")")
            .arg(QObject::tr("全部"));
    model_type->clearCache();

    model_subtype->sql_cur = "select vch_sub_typename, ch_sub_typeno, '', '', '', '', '', '', '', vch_color  from cey_bt_dish_subtype";
    model_subtype->sqlother = model_subtype->sql_cur +"  where ch_dish_typeno like '%1%' and "+operfileter;
    model_subtype->sql_cur = model_subtype->sql_cur +" where"+operfileter;
    model_subtype->clearCache();

    QString qt_show_ingredient;
    if(n_func::f_get_sysparm("qt_show_ingredient","0") == "0") {
        qt_show_ingredient = " and ifnull(ch_material_flag, '') <> 'Y' ";
    }

    QString dishname;
    QString dishname_2;
    public_printby_ch_billno::dishname_split(lds::sysconf->value("w_scr_dish_main/dishnameswitch", "vch_dishname").toString(), dishname, dishname_2, "b");//cey_bt_dish b

    model_dish->sql_cur =
            "  select "
            "  concat( ifnull(" + dishname + ",'') , '\n' , b.num_price) as name_price" //描述
            ", b.ch_dishno as dishno1"//品码
            ", b.ch_dishno as dishno2"//图片路径
            ",(select count(0) from cey_bt_dish AA where AA.ch_groupno = b.ch_dishno) group_count"
            ", if(b.item_flag = 'Y', ifnull( (select num_num from cey_st_material_num where ch_materialno = b.ch_dishno), 0),  '') as dish_st_remain"//库存剩余
            ", 0 as dishcount "
            ", '' as ischecked "
            ", num_item_warn "
            ", (select GROUP_CONCAT(concat(AA.ch_itemno, '/', ifnull(BB.num_num, ''), '/', CC.num_item_warn)) from material_bt_dish AA,  cey_st_material_num BB, cey_bt_dish CC where AA.ch_itemno = BB.ch_materialno and AA.ch_itemno = CC.ch_dishno and ifnull(CC.item_flag, '') = 'Y' and AA.ch_dishno = b.ch_dishno) as ingredient_st "
            ;
    if(lds::sysconf->value("system_setting/dish_color_keep_subtype", false).toBool())
        model_dish->sql_cur += ", (select vch_color from cey_bt_dish_subtype AA where AA.ch_sub_typeno = b.ch_sub_typeno ) as vch_color";
    else
        model_dish->sql_cur += ",b.vch_color";
    model_dish->sql_cur += "," + dishname_2 + " as dishname_2";

    model_dish->sql_cur += " from cey_bt_dish b ";
    model_dish->sql_cur += " where b.ch_stopflag <> 'Y' and b.ch_tempflag <> 'Y' ";
    model_dish->sql_cur += " and b.ch_groupno = b.ch_dishno ";
    model_dish->sql_cur += qt_show_ingredient;
    model_dish->sql_cur += " %1 ";

    model_dish->sqlother = model_dish->sql_cur.arg("  and ch_sub_typeno = '%1' and"+operfileter);
    model_dish->sqlother2 = model_dish->sql_cur.arg("  and ch_dish_typeno like '%1%' and"+operfileter);
    model_dish->sqlother3 = model_dish->sql_cur.arg("  and (b.vch_barcode like '%1%' or b.ch_dishno like '%1%' or vch_spell like '%1%' ) and"+operfileter);
    model_dish->sqlother4 = model_dish->sql_cur.arg("  and (ch_off = 'Y' ) and"+operfileter);
    model_dish->sql_cur = model_dish->sql_cur.arg(" and"+operfileter);

    model_dish->clearCache();
}

void w_scr_dish_main::torefresh_control(const QString &ch_billno)
{
    //最后设置model
    if(0 == tableView->model()) {
        tableView->setModel(tablemodel);
        tableView->justShowColumn(0);
        connect(tableView->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(orderdishRowChanged(QModelIndex,QModelIndex)));
        //切换行时，充值滚动效果
        connect(tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), tableView, SLOT(to_show_full_zero()));
    }

    ///清空 主商品 折叠
    tablemodel->row_fold_map_RELEASE(tableView);
    ///~清空 主商品 折叠
    ///
    tablemodelSelect(ch_billno, this);

    ///初始化 主商品 折叠
    if("1" == tablemodel->get_cey_sys_parameter_value("qt_Y_dish_fold")) {
        for(int k = 0; k < tableView->verticalHeader()->count(); k++) {
            if(tablemodel->can_qt_P_dish_fold(k)) {
                tablemodel->row_fold_map.insert(k, true);
            }
            if(tablemodel->can_qt_Y_dish_fold(k)) {
                tableView->hideRow(k);
            }
        }
    }
    ///~初始化 主商品 折叠

    ///~促销读会员卡
    to_payfor_info("", w_scr_dish_pay_widget::OPERAT_NONE);
}


void w_scr_dish_main::torefresh_control_Q000()
{
    lds_scr_table::get_max_ch_billno_add1_Insert_cey_u_master_Create_serial_no("Q", ch_billno);
    torefresh_control(ch_billno);
}

//-1没有设置 0不符合折扣情况 1正常
int w_scr_dish_main::autoDiscount(QString &discount_desc, int &int_discount_value)
{
    bool ch_discount = false;
    int int_discount = 0;
    QString title;
    if(n_func::f_get_sysparm("pay_auto_discount", "0") == "1") {
        ch_discount = true;
        int_discount = n_func::f_get_sysparm("pay_auto_discount_value", 0).toInt();
        title = QObject::tr("自动折扣");
    }

    //
    if(tablemodel->vch_membernod.is_m_discount()) {
        ch_discount = true;
        int_discount = tablemodel->vch_membernod.int_discount;
        title = QObject::tr("会员折扣");
    }

    //
    if(ch_discount) {
        if(public_sql::can_discount_limit(this, int_discount, title, discount_desc)) {
            discount_desc = title + ":" + QString::number(int_discount) + "%";
            int_discount_value = int_discount;
            return 1;
        }
        return 0;
    }
    return -1;
}

bool w_scr_dish_main::tablemodelSelect(const QString &ch_billno, QWidget *m)
{
    bool f = tablemodel->select(ch_billno, m);
    tableView->updateTableHeightAll();
    if(windowTypeKuaiCan == window_type) {
        lds_scr_table::cey_u_taxsale_master_insert_model(tablemodel, ch_billno);
    }
    return f;
}

bool w_scr_dish_main::tablemodelSelect(const QString &ch_billno)
{
    bool f = tablemodel->select(ch_billno);
    tableView->updateTableHeightAll();
    if(windowTypeKuaiCan == window_type) {
        lds_scr_table::cey_u_taxsale_master_insert_model(tablemodel, ch_billno);
    }
    return f;
}

void w_scr_dish_main::tablemodel_setData_updateRow(int row, const QString &field_name, const QVariant &value)
{
    tablemodel->model_data_set(row, field_name, value);
    tableView->updateTableHeight(row);
}

void w_scr_dish_main::loadData()
{
    //
    bool SCPOS_enable = false;
    if(lds::sysconf->value("w_sys_manage_outer_pay_set/scale_led_mode", false).toBool()) {
        SCPOS_enable = true;
    }
#if defined(QT_SCPOS_SUPPORT)
    SCPOS_enable = true;
#endif
    lds::hideWidget(frame_dish_bottom2->desk, SCPOS_enable);
    lds::hideWidget(frame_dish_bottom2->waimai, SCPOS_enable);
    lds::hideWidget(frame_dish_bottom2->waidai, SCPOS_enable);
    //~

    QStringList xy = lds::sysconf->value("system_setting/qt_iconsize").toString().split("x");
    int dish_hcount = qMin(qMax(xy.value(0).toInt(), 3), 20);
    int dish_vcount = qMin(qMax(xy.value(1).toInt(), 3), 20);

    QString cc = lds::sysconf->value("system_setting/qt_type_column_count").toString();
    int type_hcount  =qMin(qMax(4, cc.toInt()), 20);

    //点单信息
    {
        /// ～抓取促销信息
        connect(tablemodel, SIGNAL(signals_num_total_change(QString,QString)),this,SLOT(toCLearPayInfo()));
        connect(tablemodel, SIGNAL(signals_num_total_change(QString,QString)),this,SLOT(updateTakebillDesc()));

        tableView->horizontalHeader()->setStretchLastSection(true);
        tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tableView->setShowGrid(false);
        tableView->setEditTriggers(QTableView::NoEditTriggers);
        connect(tableView, SIGNAL(signal_dishname_rect_release(QModelIndex)),this, SLOT(todetail()));
        connect(tableView, SIGNAL(signal_suit_fold(QModelIndex)),this,SLOT(to_suit_fold(QModelIndex)));
    }

    //4 大类
    {
        tableView_type->setModel(model_type);
        /// ---------------delegate -------------------begin
        //修改delegate的某些样式单独修改
        ftableview_delegate *d = new ftableview_delegate(tableView_type);
        d->keepConnect(tableView_type);
        //        d->setMargin(2);
        d->indexisEmptyDrawGrad = true;
        tableView_type->setItemDelegate(d);
        /// ---------------delegate -------------------end
        tableView_type->fillData_addAnimation(type_hcount, 1);
        tableView_type->enableLongPress(n_func::f_get_sysparm("qt_forbide_change_new","0") == "0");
        model_type->enableEndAppendNew(n_func::f_get_sysparm("qt_forbide_change_new","0") == "0");
        model_type->disablePageRecord();
    }
    {
        tableView_subtype->setModel(model_subtype);
        /// ---------------delegate -------------------begin
        //修改delegate的某些样式单独修改
        ftableview_delegate *d = new ftableview_delegate(tableView_subtype);
        d->keepConnect(tableView_subtype);
        //        d->setMargin(2);
        d->indexisEmptyDrawGrad = true;
        tableView_subtype->setItemDelegate(d);
        /// ---------------delegate -------------------end

        tableView_subtype->fillData_addAnimation(type_hcount, 1);
        tableView_subtype->enableLongPress(n_func::f_get_sysparm("qt_forbide_change_new","0") == "0");
        model_subtype->enableEndAppendNew(n_func::f_get_sysparm("qt_forbide_change_new","0") == "0");
        model_subtype->disablePageRecord();
        connect(tableView_type,SIGNAL(signalclick(QString)),model_subtype, SLOT(toinitBySqlother(QString)));
        connect(tableView_type,SIGNAL(timeout(QModelIndex)),this,SLOT(toLongPressTypeSet()));
        connect(tableView_subtype,SIGNAL(timeout(QModelIndex)),this,SLOT(toLongPressTypeSet()));
    }

    //2 商品
    {
        tableView_dish->setModel(model_dish);
        tableView_dish->enableLongPress(n_func::f_get_sysparm("qt_forbide_change_new","0") == "0");
        model_dish->enableEndAppendNew(n_func::f_get_sysparm("qt_forbide_change_new","0") == "0");
        model_dish->disablePageRecord();
        /// ---------------delegate -------------------begin
        //修改delegate的某些样式单独修改
        ftableview_delegate *d = new ftableview_delegate(tableView_dish);
        d->setColorLight(3);
        d->keepConnect(tableView_dish);
        //        d->setMargin(2, 2, 2, 0);
        d->set_pay_unit_prefix(n_func::f_get_sysparm("pay_unit_prefix", " "));
        d->indexisEmptyDrawGrad = false;
        d->drawTableViewLeftTopBorder = false;
        d->setImageScaled(lds::sysconf->value("system_setting/dish_image_scale", 1.0).toDouble());
        tableView_dish->setItemDelegate(d);
        /// ---------------delegate -------------------end
        tableView_dish->fillData_addAnimation(dish_hcount, dish_vcount);

        connect(tableView_subtype,SIGNAL(signalclick(QString)),model_dish, SLOT(toinitBySqlother(QString)));
        connect(tableView_type,SIGNAL(signalclick(QString)),model_dish,SLOT(toinitBySqlother2(QString)));
        connect(tableView_dish,SIGNAL(signalclick(QString)),this,SLOT(todishclick(QString)));
        connect(tableView_dish,SIGNAL(timeout(QModelIndex)),this,SLOT(toLongPressUpdateDishImage(QModelIndex)));
        //        connect(tableView_dish,SIGNAL(timeout_pre()),this,SLOT(toLongPressUpdateDishImageAdd()));
        //        connect(tableView_dish,SIGNAL(timeout_next()),this,SLOT(toLongPressUpdateDishImageAdd()));

        connect(tableView_subtype,SIGNAL(signalclickNew(QModelIndex)),this, SLOT(toLongPressTypeSet()));
        connect(tableView_type,SIGNAL(signalclickNew(QModelIndex)),this,SLOT(toLongPressTypeSet()));
        connect(tableView_dish,SIGNAL(signalclickNew(QModelIndex)),this,SLOT(toLongPressUpdateDishImage(QModelIndex)));
    }

    //logo
    initLogo();
    //
    update_type_subtype_view();
    //
    torefresh_dish();
    QTimer::singleShot(50, this, SLOT(refreshChBillno()));
}

void w_scr_dish_main::orderdishRowChanged(const QModelIndex &, const QModelIndex &)
{

}

void w_scr_dish_main::toAllbillcancel()
{
    removeAllInsertDish();
    menuhide();
}

void w_scr_dish_main::tochange()
{
    QString errstring;
    int row = tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有商品被选中"));
        return;
    }
    if(!tablemodel->can_change_dish(row, &errstring)) {
        lds_messagebox::warning(this,MESSAGE_TITLE_VOID,errstring);
        return;
    }
    int rowp = row;
    double num_num = tablemodel->model_data_NIBOLAN(row, "num_num-num_back");
    double num_price_org = tablemodel->model_data(row, "num_price_org").toDouble();
    if(tablemodel->model_data(row, "ch_suitflag").toString() == "Y") {//是子商品
        for(rowp --; rowp >=0 && tablemodel->model_data(rowp, "ch_suitflag").toString() == "Y"; rowp --) {

        }
    }

    lds_query_hddpos  query;
    w_scr_dish_dish_change dialog(
                tablemodel->model_data(row, "ch_dishno").toString(),
                tablemodel->model_data(row, "num_price_org").toDouble(),
                tablemodel->model_data(row, "ch_suitno").toString(),
                this);
    dialog.setWindowTitle(QObject::tr("修改商品"));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        QString msg;
        if(!fexpandmain_2::guqing_check(msg, dialog.ch_dishno_new, tablemodel->model_data_NIBOLAN(row, "num_num-num_back"))) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, msg+QObject::tr("操作失败"));
            return;
        }
        if(!msg.isEmpty()) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, msg);
        }

        query.execSelect(QString("select ch_dishno, num_price  from cey_bt_dish  where ch_dishno =  '%1' ").arg(dialog.ch_dishno_new));
        query.next();
        double num_price_new = query.recordValue("num_price").toDouble();
        QString ch_dishno = query.recordValue("ch_dishno").toString();

        tablemodel->model_data_set(row, "num_price",num_price_new);
        tablemodel->model_data_set(row, "num_price_org",num_price_new);
        tablemodel->model_data_set(row, "ch_dishno",ch_dishno);
        tablemodel->model_data_set(row, "num_num",dialog.rate*num_num);

        if(tablemodel->model_data(row, "ch_suitflag").toString() == "Y") {//是套菜
            tablemodel->model_data_set(row, "num_price",0);
            if(!dialog.suitreplace) {//如果不是非可替换商品，需要补加
                if(num_price_org != num_price_new) {
                    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("替换商品和被替换价格不一样，是否在主商品上补差价？"),QObject::tr("确认"),QObject::tr("取消"))) {
                        double diff = (num_price_new-num_price_org)*num_num;
                        double num_nump = tablemodel->model_data_NIBOLAN(rowp, "num_num-num_back");
                        tablemodel->model_data_set(rowp, "num_price",  tablemodel->model_data(rowp, "num_price").toDouble() + diff/num_nump);
                    }
                }
            }
            /// 换菜后，不在追加
            tablemodel->ch_dishno_set_row_map_VOID(rowp, 1);
        }
    }
}

void w_scr_dish_main::toquitdish()
{
    //#ifdef QT_DEBUG
    return toquitdish2();
    //#endif

    int ret = QDialogButtonBox::InvalidRole;
    int row = tableView->currentIndex().row();
    if(tablemodel->rowCount() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有商品被选中"));
        return ;
    }

    if(windowTypeKuaiCan == window_type) {
        if(row < 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有商品被选中"));
            return ;
        }
    }

    if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert) {
        QString errstring;
        if(false == to_dish_del(row, &errstring)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        }
        return;
    }

    //更新或者没有任何操作
    if(tablemodel->opera(row) == lds_model_sqltablemodel::OpUpdate
            || tablemodel->opera(row) == lds_model_sqltablemodel::OpNone) {
        if(!backheader::operate_right_check_code(this, "0218")) {//退货
            return ;
        }
        w_scr_dish_dish_quit dialog(this);
        dialog.setWindowTitle(QObject::tr("退货"));
        dialog.initData(row, tablemodel, this->ch_billno);
        ret = transparentCenterDialog(&dialog).exec();
        //switch
        switch(ret) {
        case QDialogButtonBox::AcceptRole://单个退货
        {
            tablemodelSelect(this->ch_billno, this);
            frontViewDialog_virtual::static_display_cash(tablemodel->getKeepdata().dish_total());
            break;
        }
        case QDialogButtonBox::ApplyRole://整单退货
        {
            tablemodelSelect(this->ch_billno, this);
            frontViewDialog_virtual::static_display_cash(tablemodel->getKeepdata().dish_total());

            if(tablemodel->getKeepdata().dish_num() == 0) {
                //消台
                QString errstring;
                lds_query_hddpos  query;
                lds_query_hddpos::tran_saction();
                QString ch_payno = lds_scr_table::get_max_ch_payno_add1();
                int sqr = lds_scr_table::cey_bt_table_div_sqr(ch_tableno, this->ch_billno);
                if(lds_scr_table::cey_bt_table_update(ch_tableno, "", sqr, &errstring)) {
                    if(query.execInsert("cey_u_checkout_master",qrtVariantPairList()
                                        << qrtVariantPair("ch_billno", this->ch_billno)
                                        << qrtVariantPair("ch_tableno", ch_tableno)
                                        << qrtVariantPair("num_cost", 0)
                                        << qrtVariantPair("num_discount", 0)
                                        << qrtVariantPair("num_present", 0)

                                        << qrtVariantPair("num_service", 0)
                                        //                                        << qrtVariantPair("vch_service_desc", "")
                                        << qrtVariantPair("num_lowcost", 0)
                                        << qrtVariantPair("num_blotout", 0)
                                        << qrtVariantPair("num_run", 0)

                                        << qrtVariantPair("vch_run_operID", "")
                                        << qrtVariantPair("ch_state", "Y")
                                        << qrtVariantPair("vch_operID", lds::gs_operid)
                                        << qrtVariantPair("dt_operdate", n_func::f_get_sysdatetime())
                                        << qrtVariantPair("vch_operID2", "")

                                        << qrtVariantPair("num_rate", 0)
                                        << qrtVariantPair("ch_payno", ch_payno)
                                        << qrtVariantPair("vch_memberno", "")
                                        << qrtVariantPair("ch_member_belong_state", "")
                                        << qrtVariantPair("ch_yun_payno", "")

                                        << qrtVariantPair("num_cash", 0),  &errstring)) {
                        if(query.execUpdate("cey_u_table","ch_payno", ch_payno, qrtEqual("ch_billno", this->ch_billno),  &errstring)) {
                            if(query.execUpdate("cey_u_orderdish","ch_payno", ch_payno, qrtEqual("ch_billno", this->ch_billno),  &errstring)) {
                                lds_query_hddpos::com_mit();
                                this->accept();
                                return;
                            }
                        }
                    }
                    lds_query_hddpos::roll_back();
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
                }//~消台
            }
            break;
        }
        }
        //~switch
    }
}

void w_scr_dish_main::toquitdish2()
{
    int row = tableView->currentIndex().row();
    QString errstring;

    if(tablemodel->rowCount() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有商品被选中"));
        return ;
    }

    if(windowTypeKuaiCan == window_type) {
        if(row < 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有商品被选中"));
            return ;
        }
    }

    if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert) {
        if(false == to_dish_del(row, &errstring)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        }
        return;
    }

    //更新或者没有任何操作
    if(!backheader::operate_right_check_code(this, "0218")) {//退货
        return ;
    }
    w_scr_dish_main_quit dialog(tablemodel, this);
    if(QDialog::Rejected ==  transparentCenterDialog(&dialog).exec()) {
        return;
    }
    //select
    QList<num_backData> list_quit;
    QMap<int, qrtVariantPairList> map_insert;
    dialog.row_data_map2List(list_quit, map_insert);
    QString ch_billno = tablemodel->cur_ch_billno();
    tablemodel->select(ch_billno, this);
    tablemodel->keepdata_insert_prepare();
    QMap<int, qrtVariantPairList>::iterator k;
    int column;
    for(k = map_insert.begin(); k != map_insert.end(); k ++) {
        row = tablemodel->rowCount();
        tablemodel->insertRow(row);
        for(column = 0; column < k.value().count(); column ++) {
            tablemodel->model_data_set(row, column, k.value().at(column).second);
        }
    }
    tablemodel->keepdata_insert_submit();
    tableView->updateTableHeightAll();
    //print
    if(list_quit.count() > 0) {
        public_printby_ch_billno::toprint_dish_return(ch_billno, public_printby_ch_billno::defpath(), num_backData::int_quit_flow_id_list(list_quit));
    }

    //
    double num_dish_total = 0;
    for(row = 0; row < tablemodel->rowCount(); row ++) {
        num_dish_total += tablemodel->model_data_NIBOLAN(row, "num_num-num_back");
    }

    //
    lds_query_hddpos::updateTable("cey_u_table", qrtVariantPairList() << qrtVariantPair("num_cost", tablemodel->getKeepdata().dish_total()), qrtEqual("ch_billno", ch_billno));
    if(num_dish_total == 0) {
        if(1 == lds_messagebox::question(this,MESSAGE_TITLE_VOID, QObject::tr("是否取消该台"), QObject::tr("确定"), QObject::tr("取消"))) {
            return;
        }
        if(!w_scr_dish_pay_widget::cey_u_orderdish_empty_pay_transaction(tablemodel->cur_ch_billno(), &errstring)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
            return;
        }
        this->accept();
    }
}

void w_scr_dish_main::toExitbill()
{
    if(!backheader::operate_right_check_code(this, "0218")) {
        return;
    }
    //切换到中餐模式
    business_documents_query dialog(this, true);
    dialog.showLoginerContent();
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    transparentCenterDialog(&dialog, false).exec();
}

void w_scr_dish_main::todetail()
{
    int row = tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有行被选中"));
        return;
    }

    w_scr_dish_num_cook_Dialog dialog(tablemodel, row, this);
    if(QDialog::Accepted != transparentCenterDialog(&dialog).exec()) {
        return;
    }
    QString msg;
    if(false == numcook_check_commit(dialog.model(), row, msg)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, msg + "\n" + QObject::tr("操作失败"));
        return;
    }

    tableView->updateTableHeight(row);
    tableView->updateTableHeight(tablemodel->upper_bound_rowP(row));
}

void w_scr_dish_main::topresent()
{
    int row = tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有商品被选中"));
        return;
    }
    if(row >= 0) {
        if(tablemodel->can_present(row)) {
            w_scr_dish_num_cook_Dialog::static_to_present(
                        this,
                        tablemodel,
                        row,
                        tablemodel->model_data_NIBOLAN(row, "num_num-num_back"));
            tableView->updateTableHeight(row);
        }
    }
}

void w_scr_dish_main::tomhangpay()
{
    w_scr_dish_m_hang_pay_Dialog dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
}

void w_scr_dish_main::toTakebill()
{
    QString errstring;
    int int_flowid;
    if(tablemodel->rowCount() > 0) {///挂单
        if(fast_qudan::pendTransaction(errstring, tablemodel,int_flowid)) {
            //释放model的折叠信息，与折叠map
            tablemodel->row_fold_map_RELEASE(tableView);
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("挂单成功"));
            goto end;
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
        goto end;
    } else {///取单
        fast_qudan dialog(this);
        dialog.setWindowTitle(QObject::tr("快餐取单表"));
        if(QDialog::Accepted == transparentCenterDialog(&dialog).exec() ) {
            int_flowid = dialog.int_flowid;
            ///估清判断
            lds_query_hddpos  query;
            QString msg;
            bool guqing_flag = true;
            ///
            query.execSelect(QString("SELECT ch_dishno, num_num - num_back as dish_num from cey_u_hungbill where int_flowID = %1 group by ch_dishno; ")
                             .arg(int_flowid));
            while(query.next()) {
                if(!fexpandmain_2::guqing_check(msg, query.recordValue("ch_dishno").toString(), query.recordValue("dish_num").toDouble())) {
                    guqing_flag = false;
                }
            }
            if(!msg.isEmpty()) {//设置了沽清
                if(guqing_flag) {//沽清成功
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, msg);
                } else {//沽清失败
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, msg+QObject::tr("操作失败"));
                    return;
                }
            }
            /// ～估清判断
            lds_query_hddpos::tran_saction();
            if(fast_qudan::take(errstring, tablemodel,int_flowid)) {
                lds_query_hddpos::com_mit();
                //自动折叠
                if("1" == tablemodel->get_cey_sys_parameter_value("qt_Y_dish_fold")) {
                    tablemodel->keepdata_Y_dish_fold(tableView, 0);
                }
                tableView->updateTableHeightAll();

                lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
                goto end;
            }
            lds_query_hddpos::roll_back();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
            goto end;
        }
    }
end:
    updateTakebillState();
}

void w_scr_dish_main::toTakeweight()
{
    int row = tableView->currentIndex().row();
    //没有数据被选中
    if(row >= 0) {
        double dish_num_old = tablemodel->model_data_NIBOLAN(row, "num_num - num_back");
        QString ch_dishno = tablemodel->model_data(row, "ch_dishno").toString();
        take_weight dialog(tablemodel->model_data(row, "ch_dishno").toString(),
                           tablemodel->model_data(row, "num_price").toDouble(),
                           tablemodel->model_data_NIBOLAN(row, "num_num - num_back"),
                           tablemodel->model_data(row, "int_discount").toInt(),
                           this);
        dialog.setWindowTitle(QObject::tr("取重"));
        if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
            QString msg;
            if(fexpandmain_2::guqing_check(msg, ch_dishno, dialog._weight - dish_num_old)) {
                if(!msg.isEmpty()) {
                    lds_messagebox::information(this, MESSAGE_TITLE_VOID, msg);
                }
                tablemodel->model_data_set(row, "num_num", tablemodel->model_data_NIBOLAN(row, QString("%1 + num_back").arg(dialog._weight)));
                return;
            }
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, msg+QObject::tr("操作失败"));
        }
    }
}

void w_scr_dish_main::topaybill()
{
    //2
    QList<ftableview_standmodel_sql_none::noneData> list_data;
    list_data << ftableview_standmodel_sql_none::noneData(QObject::tr("结算"), "0");
    list_data << ftableview_standmodel_sql_none::noneData(QObject::tr("会员付款"), "1");
    list_data << ftableview_standmodel_sql_none::noneData(QObject::tr("退货"), "2");

    if(false == lds::sysconf->value("system_setting/kuaican_mpay_enabled", false).toBool())
        ftableview_standmodel_sql_none::list_remove(list_data, "1");
    if(false == lds::sysconf->value("system_setting/kuaican_tuihuo_enabled", false).toBool())
        ftableview_standmodel_sql_none::list_remove(list_data, "2");

    w_scr_dish_pay_widget::operatMode operat_mode = w_scr_dish_pay_widget::OPERAT_PAY;
    if(list_data.count() > 1) {
        if(windowTypeKuaiCan == window_type) {
            switch(lds::sysconf->value("w_scr_dish_main/paymode", list_data.last().value(Qt::UserRole + 1).toInt()).toInt()) {
            case 0://结算
                break;
            case 1://会员支付
                topaybillmember();
                return;
            case 2://退货
                operat_mode = w_scr_dish_pay_widget::OPERAT_QUITBILL;
                break;
            }
        }
    }
    if(operat_mode & w_scr_dish_pay_widget::OPERAT_QUITBILL) {
        if(!backheader::operate_right_check_code(this, "0218")) {//退货
            return  ;
        }
    }
    if(operat_mode & w_scr_dish_pay_widget::OPERAT_PAY) {
        if(!backheader::operate_right_check_code(this, "0214")) {//付款
            return  ;
        }
    }
    paybill(operat_mode);

}

void w_scr_dish_main::topaybillreturn()
{

}

void w_scr_dish_main::topaybillSelectPayMode()
{
    QList<ftableview_standmodel_sql_none::noneData> list_data;
    list_data << ftableview_standmodel_sql_none::noneData(QObject::tr("结算"), "0");
    list_data << ftableview_standmodel_sql_none::noneData(QObject::tr("会员付款"), "1");
    list_data << ftableview_standmodel_sql_none::noneData(QObject::tr("退货"), "2");

    if(false == lds::sysconf->value("system_setting/kuaican_mpay_enabled", false).toBool())
        ftableview_standmodel_sql_none::list_remove(list_data, "1");
    if(false == lds::sysconf->value("system_setting/kuaican_tuihuo_enabled", false).toBool())
        ftableview_standmodel_sql_none::list_remove(list_data, "2");

    public_select_stringlist dialog(list_data, this);
    dialog.setWindowTitle(QObject::tr("选择模式"));
    if(QDialog::Rejected == transparentCenterDialog(&dialog).exec()) {
        return;
    }
    lds::sysconf->setValue("w_scr_dish_main/paymode", dialog.curSelect(Qt::UserRole + 1));
    pushButton_pay->setText(dialog.curSelect(Qt::UserRole).toString());
    pushButton_pay->setDown(false);
}

void w_scr_dish_main::toUpdatePayBillButton()
{
    bool enabel_long_press = false;
    QString btnText = QObject::tr("结算");
    //
    QList<ftableview_standmodel_sql_none::noneData> list_data;
    list_data << ftableview_standmodel_sql_none::noneData(QObject::tr("结算"), "0");
    list_data << ftableview_standmodel_sql_none::noneData(QObject::tr("会员付款"), "1");
    list_data << ftableview_standmodel_sql_none::noneData(QObject::tr("退货"), "2");

    if(false == lds::sysconf->value("system_setting/kuaican_mpay_enabled", false).toBool())
        ftableview_standmodel_sql_none::list_remove(list_data, "1");
    if(false == lds::sysconf->value("system_setting/kuaican_tuihuo_enabled", false).toBool())
        ftableview_standmodel_sql_none::list_remove(list_data, "2");
    if(list_data.count() > 1) {
        enabel_long_press = true;
        QString key = lds::sysconf->value("w_scr_dish_main/paymode", list_data.last().value(Qt::UserRole + 1)).toString();
        if(ftableview_standmodel_sql_none::list_index(list_data, key) == -1)
            key = list_data.last().value(Qt::UserRole + 1).toString();
        btnText = ftableview_standmodel_sql_none::list_data(list_data, key, Qt::UserRole).toString();
    }
    pushButton_pay->enableLongPress(enabel_long_press);
    pushButton_pay->setText(btnText);
}

void w_scr_dish_main::topaybillmember()
{
    if(tablemodel->rowCount() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有点单记录"), true);
        return;
    }
    if(tablemodel->getKeepdata().consum == 0) {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("金额为零，确认付款"), QObject::tr("确定"), QObject::tr("取消"))) {
            return;
        }
    }
    //
    w_scr_dish_pay_widget::operatModes flags;
    flags = w_scr_dish_pay_widget::OPERAT_PAY|w_scr_dish_pay_widget::TYPE_FAST;
    w_scr_dish_pay_widget paywidget;
    paywidget.initGeometry(flags, false);
    paywidget.initdataByM(tablemodel);
    if(QDialog::Accepted == paywidget.mread(true, QObject::tr("会员付款"))) {
        paywidget.toScrpay();
        if(paywidget.pay_finished) {
            payBillFinished();
        }
    }
}

void w_scr_dish_main::toTuichu()
{
    menuhide();
    if(windowTypeKuaiCan == window_type) {
        if(tablemodel->rowCount() > 0)
        {
            if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("存在消费记录! 是否整单取消然后退出?"),QObject::tr("确认"),QObject::tr("取消"))) {
                //清空并且还原沽清信息
                removeAllInsertDish();
                this->reject();
            }
        } else {
            if(0 ==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认退出"),QObject::tr("确认"),QObject::tr("取消"))) {
                this->reject();
            }
        }
    }
    if(windowTypeKuaiCan == window_type) {

    } else {//中餐
        QString errstring;
        bool data_has_changed = tablemodel->rowCountOp(lds_model_sqltablemodel::OpInsert) > 0 || tablemodel->rowCountOp(lds_model_sqltablemodel::OpUpdate) > 0;
        if(data_has_changed) {//插入
            switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("点单信息有更改,是否落单"), QObject::tr("落单"), QObject::tr("不落单"), QObject::tr("取消"))) {
            case 0://落单
            {
                if(!printdish_tran_saction(errstring)) {
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
                    return;
                }
            }
                break;
            case 1://不落单
                //清空并且还原沽清信息
                removeAllInsertDish();
                break;
            case 2://取消
                return;
            }
        } else {
            if(!tablemodel->try_update_data(errstring)) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
                return;
            }
        }

        //退出
        this->reject();
    }
}

void w_scr_dish_main::toMRead()
{
    if(fexpandmain_2::rejected != fexpandmain_2::m_read_exec(this, tablemodel)) {
        updatememberchecked();
    }
    //2
}

void w_scr_dish_main::towaimai()
{
    if(tablemodel->rowCount() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有点单记录"));
        return ;
    }
    for(int row = 0; row < tablemodel->rowCount(); row ++) {
        if(tablemodel->opera(row) != lds_model_sqltablemodel::OpInsert) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("有商品已经落单"));
            return;
        }
    }
    takeout_cook dialog(tablemodel, ch_tableno, this);
    dialog.ui->lineEdit_orderman->setText(tablemodel->tablename_kvmap_value("t_m_member", tablemodel->vch_membernod.vch_memberno, "vch_memberno", "vch_member").toString());
    dialog.ui->lineEdit_tel->setText(tablemodel->tablename_kvmap_value("t_m_member", tablemodel->vch_membernod.vch_memberno, "vch_memberno", "vch_tel").toString());

    dialog.setWindowTitle(QObject::tr("普通外卖"));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        if(windowTypeKuaiCan == window_type) {
            refreshChBillno();
            tableView_dish->m->refreshcur();
        } else {
            this->accept();
        }
        return ;
    }
    return ;
}

void w_scr_dish_main::towaimaipay()
{
    take_cashier cashierdialog(this);
    cashierdialog.setWindowTitle(QObject::tr("外卖收银结算"));
    transparentCenterDialog(&cashierdialog).exec();
}

void w_scr_dish_main::tosysyingyedanju()
{
    //切换到中餐模式
    business_documents_query dialog(this);
    dialog.showLoginerContent();
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    transparentCenterDialog(&dialog, false).exec();
}

void w_scr_dish_main::toLanguageSwitch()
{
    public_Dialog::clearCache();
    retranslateView();
    initLogo(label_logo, label_text);
    tablemodel->emit_num_total_change_label(tablemodel->getKeepdata().dish_num(), tablemodel->getKeepdata().dish_total());
}

void w_scr_dish_main::toDishNameSwitch(const QString &key)
{
    lds::sysconf->setValue("w_scr_dish_main/dishnameswitch", key);
    torefresh_dish();
}

void w_scr_dish_main::toLanguageSet()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0512"/*多语言*/)) {
        return ;
    }

    QString old_language = lds::get_soft_curent_language_first();
    btnlanguageswicth *b = qobject_cast<btnlanguageswicth *>(this->sender());
    w_scr_dish_language_switch_set dialog(this);
    dialog.setWindowTitle(QObject::tr("切换语言"));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        if(b) {
            b->initKeys_conf();
            QString key = b->firstKey();
            public_Dialog::retranslateSystem(key);
            toLanguageSwitch();

            if(old_language != lds::get_soft_curent_language_first()) {
            }
        }
    }
    if(b) {
        b->setDown(false);
    }
}

void w_scr_dish_main::toDishnameSet()
{
    w_sys_manage_outer_dishname_show dialog(this);
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        torefresh_dish();
    }
}

void w_scr_dish_main::tosysyingyeqingkuang()
{
    w_rpt_report_frame_business_summary dialog(this);
    dialog.showLoginerContent();
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog, false).exec();
}

void w_scr_dish_main::tosysjiaoban()
{
    w_scr_dish_switch_detail dialog(this);
    if(QDialog::Accepted==transparentCenterDialog(&dialog).exec()) {
        lds_messagebox::question(this,MESSAGE_TITLE_VOID,  QObject::tr("将会重启"));
        this->reject();//退出
        public_sql::save_login->hideall_and_back_login(dialog.new_operid, dialog.new_operpwd);
    }
}

void w_scr_dish_main::tosyschongzhi()
{
    w_m_member_list::statictorecharge(this, 0, 0,"");
}

void w_scr_dish_main::tosysqukuan()
{
    w_m_member_list::statictorecharge(this, 0, 0,"C");
}

void w_scr_dish_main::tosysxiugaimima()
{
    modify_password dialog(this);
    dialog.setWindowTitle(QObject::tr("密码修改"));
    transparentCenterDialog(&dialog).exec();
}

void w_scr_dish_main::tosysswitchback()
{
    menuhide();

    if(false == backheader::operate_right_check_code(this, "0244")) {//后台
        return;
    }

    QString title;
    if(tablemodel && tablemodel->rowCount() > 0) {
        title+=QObject::tr("点单信息将会清空")+",";
        if(0 == lds_messagebox::question(this,MESSAGE_TITLE_VOID, title+QObject::tr("是否切换到后台?"), QObject::tr("确定"), QObject::tr("取消"))) {
            toAllbillcancel();
            this->accept();
        }
    } else{
        this->accept();
    }
}

void w_scr_dish_main::tosyskaiqianxiang()
{
    if(backheader::operate_right_check_code(this, "0209")) {//开钱箱
        public_printby_ch_billno::openDrawer();
    }
}

void w_scr_dish_main::tochangyongcai()
{
    ftableview_standmodel_sql *model_dish = static_cast<ftableview_standmodel_sql *>(tableView_dish->m);
    model_dish->toinitBySqlother4();
}

void w_scr_dish_main::tosyssuoping()
{
    lock_screen dialog(this);
    transparentCenterDialog(&dialog).exec();
}

void w_scr_dish_main::tosysguqing()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0406"/*商品沽清*/)) {
        return ;
    }

    w_scr_dish_guqing_report dialog(this);
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
}

void w_scr_dish_main::toRestaurant()
{
    menuhide();
    if(tablemodel->rowCount() > 0)
    {
        if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("存在消费记录! 是否整单取消然后切换到酒楼模式"),QObject::tr("确认"),QObject::tr("取消"))) {

            //清空并且还原沽清信息
            removeAllInsertDish();

            this->done(public_sql::GoRestaurant);
        }
    } else {
        this->done(public_sql::GoRestaurant);
    }
}

void w_scr_dish_main::toScanbar(const QString &text)
{
    if(add_dish_by_scanbar(text)) {
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    }
}

void w_scr_dish_main::tosub()
{
    if(window_type == windowTypeKuaiCan) {//快餐
        dish_add(-1, true);
    }
    if(window_type == windowTypeZhongCan) {//中餐
        int row = tableView->currentIndex().row();
        if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert) {//未落单
            dish_add(-1, true);
        } else {//落单
            toquitdish();
        }
    }
}

void w_scr_dish_main::toadd()
{
    dish_add(1, true);
}

void w_scr_dish_main::tochangenum()
{
    int row = tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    int num = tablemodel->model_data_NIBOLAN(row, "num_num-num_back");
    lds_dialog_input_double dialog(this);
    dialog.setWindowTitle(QObject::tr("数量"));
    dialog.ui->lineEdit->setValue(num);
    dialog.ui->lineEdit->setDecimals(0);
    dialog.disableDot();
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        dish_add((int)dialog.ui->lineEdit->value() - num, false);
    }
}

void w_scr_dish_main::todishup()
{
    int row = tableView->currentIndex().row() - 1;
    if(row < 0) return;

    tableView->selectRow(row);
}

void w_scr_dish_main::todishdown()
{
    int row = tableView->currentIndex().row() + 1;
    if(row >= tableView->verticalHeader()->count()) return;

    tableView->selectRow(row);
}

void w_scr_dish_main::showBianMa()
{
    QMenu *m = qobject_cast<QMenu *>(this->sender());
    if(m->actions().count() == 0) {
        QWidgetAction *widgetaction=new QWidgetAction(m);
        bianma_cook *b = new bianma_cook(this);
        b->setFixedSize(600,300);
        widgetaction->setDefaultWidget(b);
        m->addAction(widgetaction);
        ftableview_standmodel_sql *model_dish = static_cast<ftableview_standmodel_sql *>(tableView_dish->m);
        connect(b,SIGNAL(edit_text(QString)), model_dish,SLOT(toinitBySqlother3(QString)));
    }
}

void w_scr_dish_main::toLongPressUpdateDishImage(const QModelIndex &index)
{
    lds_query_hddpos  query;
    QString ch_sub_typeno;


    //        connect(tableView_dish,SIGNAL(timeout_pre()),this,SLOT(toLongPressUpdateDishImageAdd()));
    //        connect(tableView_dish,SIGNAL(timeout_next()),this,SLOT(toLongPressUpdateDishImageAdd()));

    if(n_func::f_get_sysparm("qt_forbide_change_new","0") == "1")
        return;
    QString key = index.data(Qt::UserRole + 1).toString();
    query.execSelect(QString(" select ch_dishno from cey_bt_dish where ch_dishno = '%1' ")
                     .arg(key));
    if(!query.next()) {
        toLongPressUpdateDishImageAdd();
        return;
    }

    QModelIndex subtypeindex = tableView_subtype->currentIndex();
    QModelIndex typeindex = tableView_type->currentIndex();
    if(subtypeindex.isValid()) {
        ch_sub_typeno = subtypeindex.data(Qt::UserRole + 1).toString();
    } else if(typeindex.isValid()) {
        QString ch_dish_typeno = typeindex.data(Qt::UserRole + 1).toString();
        query.execSelect(QString("SELECT ch_sub_typeno from cey_bt_dish_subtype where ch_dish_typeno = '%1'  ")
                         .arg(ch_dish_typeno));
        query.next();
        ch_sub_typeno = query.recordValue(0).toString();
    }
    ///
    fexpandmain_2::updateDishImage(this,index, tableView_dish->m, ch_sub_typeno);
    tablemodel->tablename_kvmap_delete_for_select_and_autoselect();
}

void w_scr_dish_main::toLongPressUpdateDishImageAdd()
{
    if(tableView_dish == 0
            || n_func::f_get_sysparm("qt_forbide_change_new","0") == "1")
        return;

    QString ch_sub_typeno;
    QModelIndex subtypeindex = tableView_subtype->currentIndex();
    QModelIndex typeindex = tableView_type->currentIndex();
    if(subtypeindex.isValid()) {
        ch_sub_typeno = subtypeindex.data(Qt::UserRole + 1).toString();
    } else if(typeindex.isValid()) {
        QString ch_dish_typeno = typeindex.data(Qt::UserRole + 1).toString();
        lds_query_hddpos  query;
        query.execSelect(QString("SELECT ch_sub_typeno from cey_bt_dish_subtype where ch_dish_typeno = '%1'  ")
                         .arg(ch_dish_typeno));
        query.next();
        ch_sub_typeno = query.recordValue(0).toString();
    }
    ///
    fexpandmain_2::updateDishImage_add(this, tableView_dish->m, ch_sub_typeno);
    tablemodel->tablename_kvmap_delete_for_select_and_autoselect();
}

void w_scr_dish_main::toLongPressTypeSet()
{
    if(n_func::f_get_sysparm("qt_forbide_change_new","0") == "1") {
        return;
    }

    w_bt_dish_base_typeset dialog(this);
    dialog.setWindowTitle(QObject::tr("品类设置"));
    transparentCenterDialog(&dialog).exec();

    update_type_subtype_view();
    torefresh_dish();
}

void w_scr_dish_main::to_suit_fold(const QModelIndex &index)
{
    tablemodel->row_fold_map_FOLD(tableView, index);
}

void w_scr_dish_main::menuhide()
{
}

void w_scr_dish_main::updateTakebillState()
{
}

void w_scr_dish_main::updateTakebillDesc()
{
}

void w_scr_dish_main::retranslateView()
{
}

bool w_scr_dish_main::dishAppend(const QString &ch_dishno, const vch_print_memoData &vch_print_memo, double num_price, int  int_discount, const bool time_promotion)
{
    lds_bool_default_false appflag;
    lds_query_hddpos  query;
    QString guqing_check_msg;
    bool guqing_check_flag = false;
    int gooddegree = 0;
    if(public_sql::discount_off_mode()) {
        int_discount = 100 - int_discount;//最后必须转出折扣
    }

    //1 默认数量 2 编号
    QString ch_suitflag = tablemodel->get_cey_bt_dish_value(ch_dishno, "ch_suitflag").toString();
    QString vch_dishname = tablemodel->get_cey_bt_dish_value(ch_dishno, "vch_dishname").toString();
    QString ch_weight = tablemodel->get_cey_bt_dish_value(ch_dishno, "ch_weight").toString();
    QString ch_unitno = tablemodel->get_cey_bt_dish_value(ch_dishno, "ch_unitno").toString();
    double num_default = tablemodel->get_cey_bt_unit_value(ch_unitno, "num_default").toDouble();
    QString ch_specialflag = "N";
    if(time_promotion
            && "1" == tablemodel->get_cey_bt_dish_value(ch_dishno, "ch_special_type2").toString()) {
        num_default = 1;//第二份半价，默认数量固定为1
        ch_weight = "N";//第二份半价，无法称重
        ch_specialflag = "Y";
        gooddegree++;
    }
    QString sql0 =
            " SELECT b.ch_dishno, (case ch_suitflag when 'Y' then 'P' else 'N' end) as ch_suitflag, (case ch_suitflag when 'Y' then ch_dishno else '' end) as ch_suitno,  1 * %1 as num_default  FROM cey_bt_dish b where b.ch_dishno = '%2' and b.ch_stopflag = 'N' ";

    if(ch_suitflag == "Y" ) {
        sql0 += " union "
                "(select b.ch_dishno, 'Y' as ch_suitflag, ch_suitno, a.num_num as num_default  from cey_bt_dish_suit a left join cey_bt_dish b  on a.ch_dishno = b.ch_dishno  where a.ch_suitno = '%2' and b.ch_stopflag = 'N')  ;";
    }

    QDateTime dt = n_func::f_get_sysdatetime();
    const int const_row = tablemodel->rowCount();
    int row = const_row;
    ///-------------------开始预插入
    tablemodel->keepdata_insert_prepare(const_row);
    query.execSelect(sql0.arg(num_default).arg(ch_dishno));//只记录，点菜编号、主子商品的标记、主商品编号
    while(query.next()) {
        tablemodel->cey_u_orderdish_append(query, dt, ch_tableno, vch_print_memo);
    }
    tablemodel->model_data_set(const_row, "num_price", num_price);
    tablemodel->model_data_set(const_row, "ch_specialflag", ch_specialflag);
    if(tablemodel->get_ch_discount_by_map(ch_dishno) == "Y") {//
        tablemodel->model_data_set(const_row, "int_discount", int_discount);
    } else {
        tablemodel->model_data_set(const_row, "int_discount", 100);
    }

    if(num_price != tablemodel->model_data(row, "num_price_org").toDouble()) {
        gooddegree ++;
    }
    //判断是否是称重商品
    if(ch_weight == "Y") {
        double w;
        if(false == getWeightValue(const_row, w)) {
            goto CANCEL;
        }
        //获取重量，等待提交
        double num_num_old = tablemodel->model_data_NIBOLAN(const_row, "num_num-num_back");
        tablemodel->model_data_set(const_row, "num_num", w);
        tablemodel->update_num_num_Y(num_num_old, row);
        gooddegree ++;
    }

    //商品详细
    if(gooddegree == 0
            && "1" == n_func::f_get_sysparm("ordertonum")) {
        w_scr_dish_num_cook_Dialog dialog(tablemodel, const_row, this);
        if(QDialog::Accepted != transparentCenterDialog(&dialog).exec()) {
            goto CANCEL;
        }
        numcook_commit(dialog.model(), const_row, false);
    }
    //做法
    if(!tablemodel->model_data(const_row, "vch_print_memo").toString().isEmpty()) {
        gooddegree ++;
    }
    //折扣
    if(tablemodel->model_data(const_row, "int_discount").toInt() != 100) {
        gooddegree ++;
    }
    //点菜数量
#ifdef QT_SCPOS_SUPPORT
#else
    if(tablemodel->model_data(const_row, "num_num").toDouble()  <= 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数量不能小于等于0"), true);
        goto CANCEL;
    }
#endif
    //估清check
    guqing_check_msg.clear();
    guqing_check_flag = true;
    for(int row = const_row; row < tablemodel->rowCount(); row ++) {
        QString ch_dishno = tablemodel->model_data(row, "ch_dishno").toString();
        double num_cur_num = tablemodel->model_data_NIBOLAN(row, "num_num-num_back");
        if(!fexpandmain_2::guqing_check(guqing_check_msg, ch_dishno, num_cur_num)) {
            guqing_check_flag = false;
        }
    }
    if(!guqing_check_msg.isEmpty()) {
        if(!guqing_check_flag) {
            guqing_check_msg += "\n" + QObject::tr("操作失败");
        }
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, guqing_check_msg);
        if(!guqing_check_flag) {
            goto CANCEL;
        }
    }

    //累计
    if(gooddegree == 0) {
#ifdef QT_SCPOS_SUPPORT
        if(false)
#else
        if(false == lds::sysconf->value("system_setting/samedishNotadd_order", false).toBool())
#endif
        {
            QSet<int> rows = tablemodel->ch_dishno_set_row_map_GET(ch_dishno);//由于在select ch_dishno的时候，不在插入ch_dishno_set_row_map中已落单的内容
            foreach(int r, rows) {
                if(r != const_row) {
                    double addnum = tablemodel->model_data(const_row, "num_num").toDouble();
                    double num_num_old = tablemodel->model_data(r, "num_num").toDouble();
                    tablemodel->keepdata_insert_del();

                    tablemodel->model_data_set(r, "num_num", tablemodel->model_data(r, "num_num").toDouble() + addnum);
                    if(tablemodel->model_data(r, "ch_suitflag").toString() == "P") {
                        tablemodel->update_num_num_Y(num_num_old, r);
                    }
                    row = r;
                    tableView->update(tablemodel->model_index(r, 0));
                    appflag = true;
                    goto SELECTROW;
                }
            }
        }
    }
    /////////////////
    tablemodel->keepdata_insert_submit();
    if("1" == tablemodel->get_cey_sys_parameter_value("qt_Y_dish_fold")) {
        tablemodel->keepdata_Y_dish_fold(tableView, const_row);
    }
    tableView->updateTableHeight(const_row);
    appflag = true;
    goto SELECTROW;
CANCEL:
    tablemodel->keepdata_insert_del();
SELECTROW:
    if(row>=0 && row < tablemodel->rowCount()) {
        frontViewDialog_virtual::static_display_dish(vch_dishname,
                                                     tablemodel->model_data_NIBOLAN(row, "num_num-num_back"),
                                                     tablemodel->model_data_NIBOLAN(row, "num_price"),
                                                     tablemodel->model_data_NIBOLAN(row, "(num_num-num_back)*num_price *int_discount *0.01+num_price_add")
                                                     );


        tableView->selectRow(row);
    }
    return appflag;
}

void w_scr_dish_main::todishclick(const QString &ch_dishno)
{
    if(ch_dishno == "-3") {
        toLongPressUpdateDishImageAdd();
        return;
    }
    if(ch_dishno.isEmpty()) {
        return;
    }
    QStringList dishno_list = QStringList() << ch_dishno;
    //分组
    if(w_scr_dish_group_dialog::check_group(ch_dishno)) {
        w_scr_dish_group_dialog dialog(ch_dishno, this);
        if(QDialog::Accepted != transparentCenterDialog(&dialog).exec()) {
            return;
        }
        dishno_list = dialog.ret_ch_dishno_list;
    }
    todishclickgroup(dishno_list);
}

void w_scr_dish_main::todishclickgroup(const QStringList &ch_dishno_list)
{
    //
    lds_query_hddpos  query;
    foreach(const QString &dishno, ch_dishno_list) {
        double num_price = 0;
        vch_print_memoData vch_print_memo;
        int int_discount = public_sql::discount_off_mode() ? 0 : 100;
        //负库存不允许销售
        if(n_func::f_get_sysparm_q(query,"inventory_negative_cannot_pay","0") == "1") {
            query.execSelect(QString(" select num_num from cey_st_material_num where ch_materialno = '%1'; ")
                             .arg(dishno));
            if(query.next()) {
                if(query.recordValue("num_num").toDouble() <= 0) {
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("负库存不允许销售"));
                    return;
                }
            }
        }
        //做法
        if(tablemodel->can_cook_pop(dishno)) {
            w_scr_dish_cook dialog(dishno, this);
            if(QDialog::Accepted != transparentCenterDialog(&dialog).exec()) {
                return;
            }
            vch_print_memo = dialog.vch_print_memo;
            vch_print_memo.print();
        }

        //促销
        bool time_promotion = can_promotion();
        if(!get_num_price_when_dishclick(dishno, num_price, int_discount, time_promotion)) {
            return;
        }

        //点菜
        dishAppend(dishno, vch_print_memo, num_price, int_discount, time_promotion);
    }
}

bool w_scr_dish_main::to_dish_del(const int row, QString *errstring)
{
    if(tablemodel->can_qt_Y_dish_fold(row)) {
        if(errstring)*errstring = QObject::tr("子商品不能单独删除");
        return false;
    }
    int row0 = row + 1;
    if(tablemodel->can_qt_P_dish_fold(row)) {//model_data(row, "ch_suitflag").toString() == "P") {
        while(row0 < tablemodel->rowCount() && tablemodel->can_qt_Y_dish_fold(row0)) {//model_data(row0, "ch_suitflag").toString() == "Y") {
            row0++;
        }
    }

    tablemodel->removeRows(row, row0 - row);
    int restore_row = qMax(0, qMin(row, tablemodel->rowCount()-1));//删除为空时，0， 删除最后一个时，为最后一个，
    for(;restore_row >= 0 && restore_row < tablemodel->rowCount(); restore_row --) {
        if(tablemodel->can_qt_Y_dish_fold(restore_row)) {//tablemodel->model_data(restore_row, "ch_suitflag").toString() == "Y") {//删除后选中行为子商品时，自动上移至主商品
            continue;
        }
        break;
    }

    tableView->selectRow(restore_row);

    frontViewDialog_virtual::static_display_cash(tablemodel->getKeepdata().dish_total() );
    //删除后，int_id重新排布
    for(int rowBegin = row ; rowBegin <tablemodel->rowCount(); rowBegin ++) {
        int int_id = rowBegin;
        if(rowBegin > 0) {
            int_id = tablemodel->model_data(rowBegin - 1, "int_id").toInt();
        }
        int_id++;
        tablemodel->model_data_set(rowBegin, "int_id", int_id);
    }

    if(tablemodel->rowCount() == 0) {
        to_payfor_info("", w_scr_dish_pay_widget::OPERAT_NONE);
    }

    return true;
}

bool w_scr_dish_main::to_dish_del(const QModelIndex &index)
{
    return to_dish_del(index.row(), 0);
}

bool w_scr_dish_main::add_dish_by_scanbar(const QString &pay_barcode)
{
    QString _price;
    QString _weight;
    QString _amount;
    lds_query_hddpos  query;
    QString ch_dishno;
    QDateTime dt = n_func::f_get_sysdatetime();
    QString  vch_barcode;

    //条码秤码
    if(CommonHeader::barcode_scale_resolve(vch_barcode,_price,_weight,_amount,pay_barcode)) {
        query.execSelect(QString("select * from cey_bt_dish a  where a.ch_stopflag <> 'Y' and a.vch_barcode='%1' ")
                         .arg(vch_barcode));
        if(query.next()) {
            if(!_weight.isEmpty()&&!_amount.isEmpty()) {
                double f  = 0;
                if(_weight.toDouble() != 0) f = _amount.toDouble() /_weight.toDouble();
                _price=QString::number(f);
            }

            if(_price.isEmpty()) {
                _price=QString::number(query.recordValue("num_price").toDouble());
            } else {
                _price=QString::number(_price.toDouble());
            }

            if(_weight.isEmpty()&&!_amount.isEmpty()) {
                double f  = 0;
                if(_price.toDouble() != 0) f = _amount.toDouble() /_price.toDouble();
                _weight=QString::number(f,  'f', tablemodel->get_cey_sys_parameter_value("bp_fmt_num_dec", 3).toInt());
            }

            if(!_weight.isEmpty()&&_amount.isEmpty()) {
                _amount=QString::number(_weight.toDouble()*_price.toDouble(), 'f', tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 3).toInt());
            }

            if(_weight.isEmpty()&&_amount.isEmpty()) {
                _weight="1";
                _amount=QString::number(_weight.toDouble()*_price.toDouble(), 'f', tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 3).toInt());;
            }
            ch_dishno = query.recordValue("ch_dishno").toString();

            tablemodel->keepdata_insert_prepare();
            tablemodel->cey_u_orderdish_append(query, dt, ch_tableno, vch_print_memoData());
            tablemodel->model_data_set(tablemodel->rowCount() - 1, "num_num", _weight);
            tablemodel->model_data_set(tablemodel->rowCount() - 1, "num_price", _price);
            tablemodel->model_data_set(tablemodel->rowCount() - 1, "ch_suitflag",  "N");
            tablemodel->keepdata_insert_submit();

            return true;
        }
    }

    vch_barcode = pay_barcode;
    //条码
    query.execSelect(QString("select ch_dishno from cey_bt_dish  a where a.ch_stopflag <> 'Y' and  vch_barcode = '%1'  ")
                     .arg(vch_barcode));
    if(query.next()) {
        ch_dishno = query.recordValue("ch_dishno").toString();
        todishclickgroup(QStringList() << ch_dishno);
        return true;
    }
    //编号
    query.execSelect(QString("select ch_dishno from cey_bt_dish  a where a.ch_stopflag <> 'Y' and ch_dishno = '%1' ")
                     .arg(vch_barcode));
    if(query.next()) {
        ch_dishno = query.recordValue("ch_dishno").toString();
        todishclickgroup(QStringList() << ch_dishno);
        return true;
    }
    return false;
}

void w_scr_dish_main::toprint()
{
    QString errstring;
    bool reject = false;
    if(tablemodel->rowCount() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有点单记录"));
        return;
    }
    //    switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("点单信息有更改"), QObject::tr("落单并返回桌台"), QObject::tr("落单"), QObject::tr("取消"))) {
    //    case 0://落单并返回桌台
    //        reject = true;
    //        break;
    //    case 1://仅落单
    //        reject = false;
    //        break;
    //    case 2://取消
    //        return;
    //    }
    reject = true;
    if(!printdish_tran_saction(errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return ;
    }
    //退出
    if(reject)
        this->reject();
}

void w_scr_dish_main::tocuicaipiece()
{
    w_scr_dish_cuicaipiece_choose dialog(tablemodel->cur_ch_billno(), w_bt_dish::KITCHEN_CUICAI,  this);
    dialog.setWindowTitle(QObject::tr("单品催起"));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {

    }
}

void w_scr_dish_main::tocuicaitable()
{
    lds_query_hddpos  query;

    if(tablemodel->rowCountOp(lds_model_sqltablemodel::OpInsert) > 0) {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("未落单将不会执行本操作")+","+("是否整桌催起"), QObject::tr("是"), QObject::tr("否"))) {
            return;
        }
    } else {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("是否整桌催起"), QObject::tr("是"), QObject::tr("否"))) {
            return;
        }
    }
    lds_query_hddpos::tran_saction();
    if(query.execUpdate("cey_u_orderdish",
                        "vch_plan_k3o3_state", QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_CUICAI)),
                        qrtEqual("ch_billno", ch_billno))) {

        lds_query_hddpos::com_mit();
        lds_messagebox::information(this,MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        return;
    }
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
}

void w_scr_dish_main::tocuicaitype()
{
    lds_query_hddpos  query;

    QMap<QString, QString> datamap;
    query.execSelect(QString("select b.ch_dish_typeno, c.vch_dish_typename,  sum((num_num-num_back)) as dish_num"
                             " from cey_u_orderdish a, cey_bt_dish b, cey_bt_dish_type c where a.ch_dishno = b.ch_dishno and b.ch_dish_typeno = c.ch_dish_typeno and ch_billno = '%1' group by ch_dish_typeno")
                     .arg(ch_billno));
    while(query.next()) {
        datamap.insert(query.recordValue("ch_dish_typeno").toString(),
                       query.recordValue("vch_dish_typename").toString() + "\n" + query.recordValue("dish_num").toString()
                       );
    }
    if(datamap.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有商品被选中"));
        return;
    }
    //2
    w_scr_dish_cuicai_choose dialog(this);
    dialog.ui->label_state->setText(QObject::tr("未落单将不会执行本操作"));
    dialog.reloaddata(datamap);
    dialog.setWindowTitle(QObject::tr("类别催起"));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {

        lds_query_hddpos::tran_saction();
        QStringList ch_dish_typeno_list = dialog.hasSelected();
        foreach(const QString & ch_dish_typeno, ch_dish_typeno_list) {
            if(!query.execUpdate("cey_u_orderdish",
                                 QStringList() << "int_flowID",
                                 QString("select a.int_flowID, '%1' as `vch_plan_k3o3_state` from cey_u_orderdish a, cey_bt_dish b where "
                                         " a.ch_dishno = b.ch_dishno and b.ch_dish_typeno = '%2' and a.ch_billno = '%3'  ")
                                 .arg(QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_CUICAI)))
                                 .arg(ch_dish_typeno)
                                 .arg(ch_billno))) {
                goto rollback;
            }
        }
        lds_query_hddpos::com_mit();
        lds_messagebox::information(this,MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        return;
    }
    return;
rollback:
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    return;
}

void w_scr_dish_main::toguaqi()
{
    QString errstring;
    lds_query_hddpos  query;
    int choice = -1;
    QStringList insert_int_flowid_list;
    //
    if(tablemodel->rowCountOp(lds_model_sqltablemodel::OpInsert) > 0) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有未落单商品"), QObject::tr("只挂起未落单的商品"), QObject::tr("全部挂起"), QObject::tr("取消"))) {
        case 0://只挂起未落单的商品
            choice = 0;
            break;
        case 1://全部挂起
            choice = 1;
            break;
        case 2://取消
            choice = 2;
            return;
        }
        if(!tablemodel->try_commit_data(errstring, fexpandmain_model_sqltablemodel_data::commitAll, &insert_int_flowid_list)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败")+"\n"+errstring);
            return;
        }
    } else {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("是否整桌挂起"), QObject::tr("是"), QObject::tr("否"))) {
            return;
        }
        choice = 1;
    }
    //
    lds_query_hddpos::tran_saction();
    switch(choice) {
    case 0://只挂起未落单的商品
    {
        if(insert_int_flowid_list.count() > 0) {
            if(!query.execUpdate("cey_u_orderdish",
                                 "vch_plan_k3o3_state", QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_GUAQI)),
                                 QString("  ch_billno = '%1' and int_flowID in %2  ")
                                 .arg(ch_billno)
                                 .arg("(" + insert_int_flowid_list.join(",") + ")")
                                 )) {
                goto rollback;
            }
        }
        lds_messagebox::information(this,MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
    }
        break;
    case 1://全部挂起
    {
        if(!query.execUpdate("cey_u_orderdish",
                             "vch_plan_k3o3_state", QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_GUAQI)),
                             QString(" ch_billno = '%1' ").arg(ch_billno))) {
            goto rollback;
        }
        lds_messagebox::information(this,MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
    }
        break;
    }

    lds_query_hddpos::com_mit();
    public_printby_ch_billno::print_label(ch_billno, insert_int_flowid_list);
    tablemodelSelect(tablemodel->cur_ch_billno(), this);
    return;
rollback:
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    return;
}

void w_scr_dish_main::tojiaodanpiece()
{
    w_scr_dish_cuicaipiece_choose dialog(tablemodel->cur_ch_billno(), w_bt_dish::KITCHEN_JIAOQI,  this);
    dialog.setWindowTitle(QObject::tr("单品叫起"));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {

    }
}

void w_scr_dish_main::tojiaodantable()
{
    lds_query_hddpos  query;

    if(tablemodel->rowCountOp(lds_model_sqltablemodel::OpInsert) > 0) {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("未落单将不会执行本操作")+","+("是否整桌叫起"), QObject::tr("是"), QObject::tr("否"))) {
            return;
        }
    } else {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("是否整桌叫起"), QObject::tr("是"), QObject::tr("否"))) {
            return;
        }
    }
    lds_query_hddpos::tran_saction();
    if(query.execUpdate("cey_u_orderdish",
                        "vch_plan_k3o3_state", QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_JIAOQI)),
                        qrtEqual("ch_billno", ch_billno))) {

        lds_query_hddpos::com_mit();
        lds_messagebox::information(this,MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        return;
    }
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
}

void w_scr_dish_main::tojiaodantype()
{
    lds_query_hddpos  query;
    QMap<QString, QString>  datamap;
    query.execSelect(QString("select b.ch_dish_typeno, c.vch_dish_typename,  sum((num_num-num_back)) as dish_num"
                             " from cey_u_orderdish a, cey_bt_dish b, cey_bt_dish_type c where a.ch_dishno = b.ch_dishno and b.ch_dish_typeno = c.ch_dish_typeno and ch_billno = '%1' group by ch_dish_typeno")
                     .arg(ch_billno));
    while(query.next()) {
        datamap.insert(query.recordValue("ch_dish_typeno").toString(),
                       query.recordValue("vch_dish_typename").toString() + "\n" + query.recordValue("dish_num").toString()
                       );
    }
    if(datamap.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有商品被选中"));
        return;
    }

    //2
    w_scr_dish_cuicai_choose dialog(this);
    dialog.ui->label_state->setText(QObject::tr("未落单将不会执行本操作"));
    dialog.reloaddata(datamap);
    dialog.setWindowTitle(QObject::tr("类别叫起"));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {

        lds_query_hddpos::tran_saction();
        QStringList ch_dish_typeno_list = dialog.hasSelected();
        foreach(const QString & ch_dish_typeno, ch_dish_typeno_list) {
            if(!query.execUpdate("cey_u_orderdish",
                                 QStringList() << "int_flowID",
                                 QString(" select a.int_flowID, '%1' as `vch_plan_k3o3_state`  from cey_u_orderdish a, cey_bt_dish b where a.ch_dishno = b.ch_dishno and b.ch_dish_typeno = '%2' and a.ch_billno = '%3'  ")
                                 .arg(QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_JIAOQI)))
                                 .arg(ch_dish_typeno)
                                 .arg(ch_billno))) {
                goto rollback;
            }
        }
        lds_query_hddpos::com_mit();
        lds_messagebox::information(this,MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        return;
    }
    return;
rollback:
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    return;
}

void w_scr_dish_main::toDishTmp()
{
    linshi_cook dialog(this);
    dialog.setWindowTitle(QObject::tr("临时商品"));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        todishclick(dialog.ch_dishno);
    }
    tablemodel->tablename_kvmap_delete_for_select_and_autoselect();
}

void w_scr_dish_main::toPaySomeDish()
{
    lds_query_hddpos  query;
    QString errstring;
    //提交更新部分
    if(false == tablemodel->try_update_data(errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    //选择分开结算的商品
    w_scr_dish_main_separate dialog(tablemodel, this);
    if(QDialog::Accepted != transparentCenterDialog(&dialog).exec()) {
        return;
    }

    //prepare mutile select
    w_scr_dish_main_auto_exit_multiSelection d(tableView);
    //select

    tableView->clearSelection();
    for(QMap<int, w_scr_dish_main_separate::separateData>::iterator k = dialog.row_data_map.begin(); k != dialog.row_data_map.end(); k++ ) {
        tableView->selectRow(k.key());
    }

    //新开台
    lds_query_hddpos::tran_saction();
    //cey_u_master
    QString ch_billno_new = lds_scr_table::get_max_ch_billno_add1("B");
    if(false == lds_scr_table::insert_cey_u_master_Create_serial_no(ch_billno_new, &errstring)) {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    qrtVariantPairList bind_value_list;
    query.execSelect(QString(" select * from cey_u_table where ch_billno = '%1' ").arg(tablemodel->cur_ch_billno()));
    query.next();
    query.recordFill(bind_value_list);
    lds_query_hddpos::replace(bind_value_list, "ch_billno", ch_billno_new);

    //cey_u_table
    if(false == query.execInsert("cey_u_table",bind_value_list, &errstring)) {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }

    if(false == lds_scr_table::cey_u_taxsale_master_insert_table(query, ch_billno_new, &errstring)) {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }

    lds_query_hddpos::com_mit();
    //////////////////////////////////
    bool payFlag = false;
    {
        //cey_u_orderdish
        QStringList exec_sql_after_paid;
        fexpandmain_model_sqltablemodel_data m;
        m.setTable("cey_u_orderdish");
        m.setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
        m.select(ch_billno_new);
        for(QMap<int, w_scr_dish_main_separate::separateData>::iterator k = dialog.row_data_map.begin(); k != dialog.row_data_map.end(); k++ ) {
            int tablemodel_row = k.key();
            int row = m.rowCount();
            m.insertRow(row);
            for(int column = 0; column < tablemodel->columnCount(); column ++) {
                m.model_data_set(row, column, tablemodel->model_data(tablemodel_row, column));
            }
            m.model_data_set(row, "ch_billno", ch_billno_new);
            m.model_data_set(row, "num_num", k.value().num_dish);
            m.model_data_set(row, "vch_print_memo", k.value().d.sqlValue());
            m.model_data_set(row, "num_price_add", k.value().d.total());
            m.model_data_set(row, "ch_payno", "");
            //分开结算不厨打
            if(tablemodel->opera(tablemodel_row) != lds_model_sqltablemodel::OpInsert) {
                m.model_data_set(row, "vch_plan_k3o3_id", "000000000000");
                m.model_data_set(row, "vch_plan_k3o3_state", "111111");
            }

            vch_print_memoData vch_print_memo(tablemodel->model_data(tablemodel_row, "vch_print_memo").toString());
            vch_print_memo.sub(k.value().d);
            exec_sql_after_paid << QString("update cey_u_orderdish set num_num = num_num - %1, vch_print_memo = '%2', num_price_add = %3 where ch_billno = '%4' and int_id = %5 ")
                                   .arg(k.value().num_dish)
                                   .arg(vch_print_memo.sqlValue())
                                   .arg(vch_print_memo.total())
                                   .arg(tablemodel->cur_ch_billno())
                                   .arg(tablemodel->model_data(tablemodel_row, "int_id").toInt())
                                   ;
        }
        //支付
        w_scr_dish_pay_dialog dialog_pay(w_scr_dish_pay_widget::OPERAT_PAY|w_scr_dish_pay_widget::TYPE_RESTAURANT, this, &m);
        dialog_pay.pay_widget()->set_exec_sql_after_paid(exec_sql_after_paid);
        if(QDialog::Accepted == dialog_pay.exec()) {//付款成功
            payFlag = true;
        }
    }

    if(false == payFlag) {
        return;
    }
    lds_model_sqltablemodel order_m;
    order_m.setTable("cey_u_orderdish");
    order_m.setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    for(int row = 0; row < tablemodel->rowCount(); row ++) {
        if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert) {
            //未选中的新增商品
            if(dialog.row_data_map.find(row) == dialog.row_data_map.end()) {
                order_m.appendTableRow(tablemodel, row);
            }

            //当前结算
            if(dialog.row_data_map.find(row) != dialog.row_data_map.end()) {
                vch_print_memoData vch_print_memo(tablemodel->model_data(row, "vch_print_memo").toString());
                vch_print_memo.sub(dialog.row_data_map[row].d);
                double num_remain = tablemodel->model_data_NIBOLAN(row, "num_num-num_back") - dialog.row_data_map[row].num_dish;

                if(num_remain == 0 && vch_print_memo.count() == 0) {

                } else {
                    order_m.appendTableRow(tablemodel, row);
                    order_m.setLastData("num_num", num_remain);
                    order_m.setLastData("vch_print_memo", vch_print_memo.sqlValue());
                    order_m.setLastData("num_price_add", vch_print_memo.total());
                }
            }
        }
    }
    //
    tablemodel->select(ch_billno, this);
    tablemodel->keepdata_insert_prepare();
    for(int row = 0; row < order_m.rowCount();  row++) {
        tablemodel->insertRow(tablemodel->rowCount());
        for(int column = 0; column < order_m.columnCount(); column ++) {
            tablemodel->model_data_set(tablemodel->rowCount() - 1, column, order_m.model_data(row, column));
        }
    }
    tablemodel->keepdata_insert_submit();
    //
    if(tablemodel->getKeepdata().dish_num() == 0) {
        QString errstring;
        if(!w_scr_dish_pay_widget::cey_u_orderdish_empty_pay_transaction(ch_billno, &errstring)) {
            qDebug() << errstring;
        }
        this->accept();
        return;
    }
    //
    tableView->updateTableHeightAll();
}

void w_scr_dish_main::toWaiDai()
{
    int row = tableView->currentIndex().row();
    if(tablemodel->rowCountOp(lds_model_sqltablemodel::OpInsert) == 0 ) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("所有商品已落单"));
        return;
    }

    if( row < 0) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("外带"), QObject::tr("全部外带"), QObject::tr("全部取消"), QObject::tr("取消"))) {
        case 0:
            goto all_waidai;
        case 1:
            goto all_waidai_cancel;
        }
    }

    if( row >= 0) {
        if(tablemodel->model_data(row, "ch_waidai_flag").toString() == "Y") {
            switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID,
                                            tablemodel->tablename_kvmap_value("cey_bt_dish", tablemodel->model_data(row, "ch_dishno").toString(),  "ch_dishno", "vch_dishname").toString()
                                            + "\n" +  QObject::tr("只能操作未落单的商品"), QObject::tr("单品取消"), QObject::tr("全部外带"), QObject::tr("全部取消"), QObject::tr("取消"))) {
            case 0:
                goto row_waidai_cancel;
            case 1:
                goto all_waidai;
            case 2:
                goto all_waidai_cancel;
            }
        } else {
            switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID,
                                            tablemodel->tablename_kvmap_value("cey_bt_dish", tablemodel->model_data(row, "ch_dishno").toString(),  "ch_dishno", "vch_dishname").toString()
                                            + "\n" + QObject::tr("只能操作未落单的商品"), QObject::tr("单品外带"), QObject::tr("全部外带"), QObject::tr("全部取消"), QObject::tr("取消"))) {
            case 0:
                goto row_waidai;
            case 1:
                goto all_waidai;
            case 2:
                goto all_waidai_cancel;
            }
        }
    }
    return;

all_waidai:
    for(int row = 0; row < tablemodel->rowCount(); row++) {
        if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert) {
            tablemodel_setData_updateRow(row, "ch_waidai_flag", "Y");
        }
    }
    return;

all_waidai_cancel:
    for(int row = 0; row < tablemodel->rowCount(); row++) {
        if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert) {
            tablemodel_setData_updateRow(row, "ch_waidai_flag", "N");
        }
    }
    return;

row_waidai:
    if(tablemodel->opera(row) != lds_model_sqltablemodel::OpInsert) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("商品已落单"));
        return;
    }
    tablemodel_setData_updateRow(row, "ch_waidai_flag", "Y");
    return;

row_waidai_cancel:
    if(tablemodel->opera(row) != lds_model_sqltablemodel::OpInsert) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("商品已落单"));
        return;
    }
    tablemodel_setData_updateRow(row, "ch_waidai_flag", "N");
    return;
}

void w_scr_dish_main::toTelComing(const QString &telNo)
{
    if(false == lds::sysconf->value("system_setting/read_tel_enable", false).toBool()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("请在软件后台启用此功能"));
        return;
    }
    QWidget *aw = qApp->activeWindow();
    if(aw && QString("w_scr_dish_main_view_800x600") == aw->metaObject()->className()) {
        static bool isRunning = false;
        if(isRunning) return;
        isRunning = true;

        w_m_member_telComing dialog(true, telNo, this);
        if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
            if(fexpandmain_2::readed == fexpandmain_2::m_read_exec(this, tablemodel, dialog.ret_vch_memberno)) {
                if(lds::sysconf->value("w_m_member_telComing/delRecord", true).toBool()) {
                    dialog.delTelFileTelNo(dialog.ret_tel);
                }
            }
            updatememberchecked();
        }

        isRunning = false;
    }
}

void w_scr_dish_main::to_moblie_payselect()
{
    //支持几种的单独的付款方式
}

void w_scr_dish_main::to_moblie_paybill()
{
    if(tablemodel->rowCount() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有点单记录"), true);
        return ;
    }
    if(tablemodel->getKeepdata().consum == 0) {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("金额为零，确认付款"), QObject::tr("确定"), QObject::tr("取消"))) {
            return;
        }
    }

    w_scr_dish_pay_widget::operatModes flags;
    if(windowTypeKuaiCan == window_type) {
        flags = w_scr_dish_pay_widget::OPERAT_PAY|w_scr_dish_pay_widget::TYPE_FAST;
    } else {
        flags = w_scr_dish_pay_widget::OPERAT_PAY|w_scr_dish_pay_widget::TYPE_RESTAURANT;
    }
    w_scr_dish_pay_widget paywidget;
    paywidget.initGeometry(flags, false);
    paywidget.initdataByM(tablemodel);
    paywidget.setPayMoney(widget_total->value());
    if(QDialog::Accepted == paywidget.mobilepay(true)) {
        paywidget.toScrpay();
        if(paywidget.pay_finished) {
            payBillFinished();
        }
    }
}

void w_scr_dish_main::to_cash_paybill()
{
    if(tablemodel->rowCount() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有点单记录"), true);
        return ;
    }
    if(tablemodel->getKeepdata().consum == 0) {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("金额为零，确认付款"), QObject::tr("确定"), QObject::tr("取消"))) {
            return;
        }
    }

    w_scr_dish_pay_widget::operatModes flags;
    if(windowTypeKuaiCan == window_type) {
        flags = w_scr_dish_pay_widget::OPERAT_PAY|w_scr_dish_pay_widget::TYPE_FAST;
    } else {
        flags = w_scr_dish_pay_widget::OPERAT_PAY|w_scr_dish_pay_widget::TYPE_RESTAURANT;
    }
    w_scr_dish_pay_widget paywidget;
    paywidget.initGeometry(flags, false);
    paywidget.initdataByM(tablemodel);
    paywidget.setPayMoney(widget_total->value() - widget_change->value());
    //    if( 1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID,
    //                                      QObject::tr("现金") + ":" + QString().sprintf("%.02f", paywidget.moneyPay()) + "\n" +
    //                                      QObject::tr("找零") + ":" + QString().sprintf("%.02f", paywidget.changePay()) + "\n",
    //                                      QObject::tr("支付"), QObject::tr("取消")
    //                                      )) {
    //        return;
    //    }

    paywidget.toScrpay();
    if(paywidget.pay_finished) {
        payBillFinished();
    }
}

void w_scr_dish_main::initLogo(QLabel *label_logo, QLabel *label_text)
{
    label_logo->setPixmap(lds::get_fastfdbacklogo());
    QStringList ret_list = lds::get_fastfdbacktext();
    label_text->setText(ret_list.value(0).trimmed());
    label_text->setAlignment(Qt::AlignBottom|Qt::AlignLeft);
    label_text->setStyleSheet("font-size:14pt;");
    if(lds::MAIN_WINDOW_SIZE.width() == 800) {
        label_text->setStyleSheet("font-size:12pt;");
    }
}

QString w_scr_dish_main::cur_ch_billno()
{
    return ch_billno;
}

bool w_scr_dish_main::paybill(w_scr_dish_pay_widget::operatMode operat_mode)
{
    //1
    w_scr_dish_main_auto_exit_multiSelection d(tableView);

    lds_query_hddpos  query;
    if(tablemodel->rowCount() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有点单记录"), true);
        return false;
    }
    if(tablemodel->getKeepdata().consum == 0) {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("金额为零，确认付款"), QObject::tr("确定"), QObject::tr("取消"))) {
            return false;
        }
    }
    w_scr_dish_pay_widget::operatModes flags;
    if(windowTypeKuaiCan == window_type) {
        flags = operat_mode |w_scr_dish_pay_widget::TYPE_FAST;
    } else {
        flags = operat_mode |w_scr_dish_pay_widget::TYPE_RESTAURANT;
    }

    w_scr_dish_pay_dialog dialog(flags, this, tablemodel);
    if(QDialog::Accepted == dialog.exec()) {//付款成功
        payBillFinished();
        return true;
    }
    return false;
}

void w_scr_dish_main::removeAllInsertDish()
{
    for(int row = tablemodel->rowCount() - 1; row >= 0; row --) {
        if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert) {
            tablemodel->removeRows(row, 1);
        }
    }
}

bool w_scr_dish_main::numcook_check_commit(const fexpandmain_model_sqltablemodel *numcook_model, int row, QString &errstring)
{
    double off = numcook_model->model_data_NIBOLAN(0, "num_num-num_back") - tablemodel->model_data_NIBOLAN(row, "num_num-num_back");
    if(false == fexpandmain_2::guqing_try_check_P(tablemodel, errstring, row, off)) {
        return false;
    }
    numcook_commit(numcook_model, row);
    return true;
}

void w_scr_dish_main::numcook_commit(const fexpandmain_model_sqltablemodel *numcook_model, int row)
{
    numcook_commit(numcook_model, row, true);
}

void w_scr_dish_main::numcook_commit(const fexpandmain_model_sqltablemodel *numcook_model, int row, bool commited)
{
    lds_query_hddpos query;
    QDateTime dt = n_func::f_get_sysdatetime();
    QString sql0;
    const lds_model_sqltablemodel_d *addon_model = numcook_model->tablename_kvmap_d("addon");

    float num_num_old = tablemodel->model_data_NIBOLAN(row, "num_num-num_back");
    for(int c = 0, cc = numcook_model->columnCount(); c < cc; c++) {
        tablemodel->model_data_set(row, c,  numcook_model->model_data(0, c));
    }
    if(tablemodel->model_data(row, "ch_suitflag").toString() == "Y") {
        tablemodel->update_num_price_add_2_P(row);
    }
    if(tablemodel->model_data(row, "ch_suitflag").toString() == "P") {
        tablemodel->update_num_num_Y(num_num_old, row);
    }

    //addon
    int int_id_off = 0;
    if(tablemodel->model_data(row, "ch_addon_flag").toString() == "P") {
        int k = row + 1;
        while(k < tablemodel->rowCount() && tablemodel->model_data(k, "ch_addon_flag").toString() == "Y" ) {
            tablemodel->removeRow(k);
            int_id_off --;
        }
    }

    if(commited)  tablemodel->keepdata_insert_prepare(row + 1);
    if(tablemodel->model_data(row, "ch_addon_flag").toString() != "Y") {
        sql0 = " SELECT b.ch_dishno, 'N' as ch_suitflag, '' as ch_suitno,  %1 as num_default  FROM cey_bt_dish b where b.ch_dishno = '%2' and b.ch_stopflag = 'N' ";

        for(int k = 0; k < addon_model->rowCount(); k ++) {
            query.execSelect(
                        sql0
                        .arg(addon_model->data(k, "num").toDouble())
                        .arg(addon_model->data(k, "no").toString()));
            query.next();

            int_id_off ++;
            tablemodel->cey_u_orderdish_insert(k + row + 1, query, dt, ch_tableno,  vch_print_memoData());
            tablemodel->model_data_set(k + row + 1, "ch_addon_flag", "Y");
            tablemodel->model_data_set(k + row + 1, "ch_addon_no", tablemodel->model_data(row, "ch_dishno"));
            tablemodel->model_data_set(k + row + 1, "int_discount", tablemodel->model_data(row, "int_discount"));
            tableView->updateTableHeight(k + row + 1);
        }
        tablemodel->model_data_set(row, "ch_addon_flag", (addon_model->rowCount() > 0) ? "P" : "N");
    }
    if(commited)  tablemodel->keepdata_insert_submit(row + addon_model->rowCount());

    //int_id_off
    if(int_id_off != 0) {
        for(int k = row + addon_model->rowCount() + 1; k < tablemodel->rowCount(); k ++) {
            tablemodel->model_data_set(k, "int_id", tablemodel->model_data(k, "int_id").toInt() + int_id_off);
        }
    }
    //自动折叠
    if("1" == tablemodel->get_cey_sys_parameter_value("qt_Y_dish_fold")) {
        tablemodel->keepdata_Y_dish_fold(tableView, row);
    }
}

bool w_scr_dish_main::get_num_price_when_dishclick(const QString &ch_dishno, double &num_price, int &int_discount, bool &time_promotion)
{
    //判断促销
    const double num_price_org = tablemodel->get_cey_bt_dish_value(ch_dishno, "num_price").toDouble();
    const double num_m_price = tablemodel->get_cey_bt_dish_value(ch_dishno, "num_m_price").toDouble();
    const double num_special_price = tablemodel->get_cey_bt_dish_value(ch_dishno, "num_special_price").toDouble();

    QString discount_desc;
    int int_discount_value;
    if(1 == autoDiscount(discount_desc, int_discount_value)) {
        int_discount = int_discount_value;
    }
    //
    num_price = num_price_org;
    if(tablemodel->vch_membernod.is_m_price()) {
        num_price = qMin(num_m_price, num_price);
    }
    if(tablemodel->vch_membernod.is_m_discount()) {
        int_discount = tablemodel->vch_membernod.int_discount;
    }
    //
    if(time_promotion //当前正在促销
            && tablemodel->get_cey_sys_parameter_value("promotion_setting_target").toInt() == 1//会员
            && tablemodel->vch_membernod.vch_memberno.isEmpty() //未读会员
            ) {
        time_promotion = false;
    }
    //
    if(time_promotion //当前正在促销
            && tablemodel->get_cey_sys_parameter_value("promotion_setting_target").toInt() == 2//非会员
            && tablemodel->vch_membernod.vch_memberno.length() > 0 //已读会员
            ) {
        time_promotion = false;
    }

    //时间符合促销
    if(time_promotion) {
        ///1 策略1 选择价格
        if("1" == tablemodel->get_cey_bt_dish_value(ch_dishno, "ch_special_type").toString()) {
            num_price = qMin(num_special_price, num_price);
        }
        if("2" == tablemodel->get_cey_bt_dish_value(ch_dishno, "ch_special_type").toString()) {//可选特价
            switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("选择特价"),
                                            QObject::tr("特价") + ":" +QString::number(num_special_price, 'f', 2), QObject::tr("现价") + ":" +QString::number(num_price, 'f', 2), QObject::tr("取消"))) {
            case 0://特价
                num_price = num_special_price;
                break;
            case 1://原价
                break;
            case 2://取消
                return false;
            }
        }
    }


    ///3策略2 第二份半价、
    /**
                                 *          第二份半价，直接用字面意思，第二杯价格半价
                                 *          软件中，原价不等于现价不允许折扣，故：第二份半价不能折扣
                                 *
                                 *         商品A为第二份半价，商品B的子套餐包含A, 点商品B时，A不参与第二份半价的优惠
                                 *
                                 *         先判断会员价、然后特价、然后第二份半价、然后时价
                                 *         原件 10，会员8实际8=min(10,8)，特价9,实际8=min(9, 8)，半价4=8/2， 时价
                                 *         第二份半价的商品 不能叠加，默认数量强制为1
                                 *         第二份半价的商品 不能称重
                                 *         第二份半价的商品 不能修改数量
                                      */
    //时间符合促销
    if(time_promotion) {
        if("1" == tablemodel->get_cey_bt_dish_value(ch_dishno, "ch_special_type2").toString()) {
            if(1 == int(tablemodel->ch_dishno_dish_num_map_GET().value(ch_dishno))%2) {
                lds_messagebox::question(this, MESSAGE_TITLE_VOID,
                                         tablemodel->get_cey_bt_dish_value(ch_dishno, "vch_dishname").toString() +"  X" + QString::number(1 + tablemodel->ch_dishno_dish_num_map_GET().value(ch_dishno)) + "\n"
                                         + QObject::tr("第二份半价"));
                num_price = num_price/2;
            }
        }
    }
    ///~3策略2 第二份半价、

    ///实价菜
#ifdef QT_SCPOS_SUPPORT
    if(tablemodel->get_cey_bt_dish_value(ch_dishno, "ch_curprice").toString() == "Y" || num_price == 0 )
#else
    if(tablemodel->get_cey_bt_dish_value(ch_dishno, "ch_curprice").toString() == "Y")
#endif
    {
        lds_dialog_input inputdialog(this);
        inputdialog.ui->label->setText(QObject::tr("时价商品"));
        inputdialog.ui->lineEdit->setText(QString::number(num_price));
        inputdialog.ui->label_2->setText(QObject::tr("数量"));
        if(QDialog::Accepted != transparentCenterDialog(&inputdialog).exec()) {
            return false;
        }
        num_price = inputdialog.ui->lineEdit->text().toDouble();
    }
    ///～实价菜
    return true;
}

bool w_scr_dish_main::can_promotion()
{
    //
    QVector<int> promotion_week_days;
    bool promotion_enabled = false;
    QDate promotion_setting_date =   tablemodel->get_cey_sys_parameter_value("promotion_setting_date").toDate();
    QDate promotion_setting_date_2 =   tablemodel->get_cey_sys_parameter_value("promotion_setting_date_2").toDate();

    QTime promotion_setting_am =   tablemodel->get_cey_sys_parameter_value("promotion_setting_am").toTime();
    QTime promotion_setting_am_2 =   tablemodel->get_cey_sys_parameter_value("promotion_setting_am_2").toTime();
    QTime promotion_setting_pm =   tablemodel->get_cey_sys_parameter_value("promotion_setting_pm").toTime();
    QTime promotion_setting_pm_2 =   tablemodel->get_cey_sys_parameter_value("promotion_setting_pm_2").toTime();

    if(tablemodel->get_cey_sys_parameter_value("promotion_setting_Fri").toBool())promotion_week_days.append(5);
    if(tablemodel->get_cey_sys_parameter_value("promotion_setting_Mon").toBool())promotion_week_days.append(1);
    if(tablemodel->get_cey_sys_parameter_value("promotion_setting_Sat").toBool())promotion_week_days.append(6);
    if(tablemodel->get_cey_sys_parameter_value("promotion_setting_Sun").toBool())promotion_week_days.append(7);

    if(tablemodel->get_cey_sys_parameter_value("promotion_setting_Thur").toBool())promotion_week_days.append(4);
    if(tablemodel->get_cey_sys_parameter_value("promotion_setting_Tues").toBool())promotion_week_days.append(2);
    if(tablemodel->get_cey_sys_parameter_value("promotion_setting_Wed").toBool())promotion_week_days.append(3);
    QDateTime sqldt = n_func::f_get_sysdatetime();
    QDate curdate = sqldt.date();
    QTime curtime = sqldt.time();


    if(curdate >= promotion_setting_date && curdate <= promotion_setting_date_2) {
        if(promotion_week_days.contains(curdate.dayOfWeek())) {
            if(promotion_setting_am <= curtime && curtime <= promotion_setting_am_2) {
                promotion_enabled = true;
            }
            if(promotion_setting_pm <= curtime && curtime <= promotion_setting_pm_2) {
                promotion_enabled = true;
            }
        }
    }

    return promotion_enabled;
}

void w_scr_dish_main::dish_add(int f, bool autoScale)
{
    int row = tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有行被选中"));
        return;
    }
    QString errstring;
    if(false == tablemodel->can_change_num(row, &errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    //
    QString ch_dishno = tablemodel->model_data(row, "ch_dishno").toString();
    QString ch_unitno = tablemodel->get_cey_bt_dish_value(ch_dishno, "ch_unitno").toString();
    double num_add = f;
    if(autoScale) {
        num_add = num_add * tablemodel->get_cey_bt_unit_value(ch_unitno, "num_default").toDouble();
    }

    //guqing
    QString msg;
    if(fexpandmain_2::guqing_try_check_P(tablemodel, msg, row,  num_add)) {
        if(!msg.isEmpty()) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, msg);
        }
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, msg + QObject::tr("操作失败"));
        return;
    }
    //~guqing

    //num_num
    double num_num_old = tablemodel->model_data_NIBOLAN(row, "num_num-num_back");
    tablemodel->model_data_set(row, "num_num", tablemodel->model_data(row, "num_num").toDouble() + num_add);

    if(tablemodel->model_data(row, "ch_suitflag").toString() == "P") {
        //num_num
        tablemodel->update_num_num_Y(num_num_old, row);
        //vch_print_memo
        tablemodel->update_vch_print_memo_Y(num_num_old, row);
        //num_price_add
        tablemodel->update_num_price_add_2_P(row);
    }
    if(tablemodel->model_data(row, "ch_suitflag").toString() == "N") {
        //vch_print_memo
        tablemodel->update_vch_print_memo_N(num_num_old, row);
        //num_price_add
        tablemodel->model_data_set(row, "num_price_add", vch_print_memoData(tablemodel->model_data(row, "vch_print_memo").toString()).total());
    }


    //paint
    //Y
    int paint_row_p = row;
    if(tablemodel->model_data(paint_row_p, "ch_suitflag").toString() == "Y") {
        for(paint_row_p --; paint_row_p >= 0 && tablemodel->model_data(paint_row_p, "ch_suitflag").toString() == "Y"; paint_row_p --) {

        }
        tableView->update(tablemodel->model_index(row, 0));
        tableView->update(tablemodel->model_index(paint_row_p, 0));
    }
    //P
    if(tablemodel->model_data(paint_row_p, "ch_suitflag").toString() == "P") {
        tableView->update(tablemodel->model_index(paint_row_p, 0));
        for(paint_row_p ++; paint_row_p < tablemodel->rowCount() && tablemodel->model_data(paint_row_p, "ch_suitflag").toString() == "Y"; paint_row_p ++) {
            tableView->update(tablemodel->model_index(paint_row_p, 0));
        }
    }
    //N
    if(tablemodel->model_data(paint_row_p, "ch_suitflag").toString() == "N") {
        tableView->update(tablemodel->model_index(row, 0));
    }

    //if 0 del
    if(tablemodel->model_data_NIBOLAN(row, "num_num - num_back") <= 0) {
        to_dish_del(row);
    }
}

void w_scr_dish_main::toSwitchSkin()
{
    QVariantMap::iterator k = lds::skin_name_map.find(lds::sysconf->value("system_setting/skin_name", "obsidian").toString());
    if(k == lds::skin_name_map.end()) {
        k = lds::skin_name_map.begin();
    } else {
        k ++;
    }
    if(k == lds::skin_name_map.end()) {
        k = lds::skin_name_map.begin();
    }
    QString skin_name = k.key();
    qApp->setStyleSheet(public_sql::stylesheetData(skin_name));
    lds::sysconf->setValue("system_setting/skin_name", skin_name);

    lds::DOUBLE_DATA d;
    d.udpKey = lds::udp_change_skin_request;
    d.skin_name = skin_name;
    public_sql::doublescreensenddataByconf(d);
}

void w_scr_dish_main::toScaleDishPic()
{
    double scale = ftableview_delegate::getTableViewImageScaled(tableView_dish);
    scale += 0.1;
    if(scale > 2)
        scale = 1.0;
    ftableview_delegate::setTableViewImageScaled(scale, tableView_dish);

    lds::sysconf->setValue("system_setting/dish_image_scale", scale);
}

void w_scr_dish_main::toCLearPayInfo()
{
    if(pay_info_widget) {
        lds::hideWidget(pay_info_widget);
    }
}

void w_scr_dish_main::to_payfor_info(const QString &ch_payno, int paytype)
{
    ///促销读会员卡
    if(tablemodel->rowCount() == 0) {
        bool ispromotion = false;
        if(can_promotion()
                && tablemodel->get_cey_sys_parameter_value("promotion_setting_target").toInt() == 1) {//会员
            ispromotion = true;
        }
        //
        QString discount_desc;
        int int_discount_value;
        autoDiscount(discount_desc, int_discount_value);
        //
        if(0 == pay_info_widget) {
            pay_info_widget =new  w_scr_dish_main_pay_info(tableView);
        }
        fexpandmain_2::showPayinfo(tableView, pay_info_widget, ch_payno, paytype, ispromotion, discount_desc);
    }
}

void w_scr_dish_main::to_payfor_info_ch_billno(const QString &ch_billno, int paytype)
{
    lds_query_hddpos  query;
    query.execSelect(QString("SELECT ch_payno FROM cey_u_checkout_master where ch_billno = '%1' ; ")
                     .arg(ch_billno));
    query.next();

    to_payfor_info(query.recordValue("ch_payno").toString(), paytype);
}

void w_scr_dish_main::updatememberchecked()
{
    if(pay_info_widget != 0) {
        if(lds::widgetIsShow(pay_info_widget->ui->label_discount)) {
            QString discount_desc;
            int int_discount_value;
            if(-1 != autoDiscount(discount_desc, int_discount_value))
                pay_info_widget->ui->label_discount->setText(discount_desc);
        }
    }
}

void w_scr_dish_main::toQuickNum()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if(!btn)
        return;
    QString text = btn->text();

    if(text == "C") {
        widget_total->setValue(0);
        goto end;
    }
    if(text.length() == 1) {
        widget_total->setValue(text);
    } else {
        widget_total->clear();
        widget_total->setValue(text.toDouble());
    }

end:
    widget_change->setValue(widget_total->value() - tablemodel->getKeepdata().pay_total(tablemodel));
}

void w_scr_dish_main::toshowPayInfo()
{
    if(0 == lds_messagebox::information(this, MESSAGE_TITLE_VOID, tablemodel->getKeepdata().pay_msg, QObject::tr("重置"), QObject::tr("退出"))) {
        updateTotal(tablemodel->getKeepdata().pay_total(tablemodel));
    }
}

void w_scr_dish_main::updateTotal(double total)
{
    if(lds::sysconf->value("w_sys_manage_outer_pay_set/quickpay", false).toBool()) {
        widget_total->setValue(tablemodel->getKeepdata().pay_total(tablemodel));
        widget_change->setValue(widget_total->value() - tablemodel->getKeepdata().pay_total(tablemodel));
    } else {
        label_total->setText(
                    tablemodel->get_cey_sys_parameter_value("pay_unit_prefix").toString() +
                    lds::septNumber(total, tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt()
                                    ));
    }
}

bool w_scr_dish_main::getWeightValue(int row, double &w)
{
    ///称重时初始数量为0
    ///称重后无法进入商品详细界面
    take_weight dialog(tablemodel->model_data(row, "ch_dishno").toString(),
                       tablemodel->model_data(row, "num_price").toDouble(),
                       tablemodel->model_data_NIBOLAN(row, "num_num - num_back"),
                       tablemodel->model_data(row, "int_discount").toInt(),
                       this);
    dialog.setWindowTitle(tablemodel->get_cey_bt_dish_value(tablemodel->model_data(row, "ch_dishno").toString(), "vch_dishname").toString() + ","+ QObject::tr("取重"));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        w = dialog._weight;
        return true;
    }
    return false;
}

void w_scr_dish_main::toWine()
{
    fexpandmain_2::wineTake(this);
}

void w_scr_dish_main::toqr_code_order_over_view()
{
    //    if(n_func::f_get_sysparm("qr_code_order_mode","0") == "0") {
    //        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("请在软件后台启用此功能"));
    //        return;
    //    }
    //    w_scr_dish_main_qr_code_over_view dialog(this);
    //    if(public_sql::GoRestaurantOrder == transparentCenterDialog(&dialog).exec()) {
    //    }
}

void w_scr_dish_main::toDiscountBill()
{
    if(tablemodel->rowCount() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有点单记录"), true);
        return;
    }

    //! 折扣获取
    lds_dialog_input_double inputdialog(this);
    inputdialog.setWindowTitle(QObject::tr("折扣"));
    inputdialog.ui->lineEdit->setRange(0, 100);
    inputdialog.ui->lineEdit->setDecimals(0);
    inputdialog.ui->lineEdit->setSuffix("%");
    inputdialog.ui->lineEdit->setValue(100);

    if(public_sql::discount_off_mode()) {//折让
        inputdialog.ui->lineEdit->enableValueMaxMinus();
    }
    if(QDialog::Accepted != transparentCenterDialog(&inputdialog).exec()) {
        return;
    }
    QString discount_desc;
    //enableValueMaxMinus， 则trueValue返回的就是折让的值
    if(false == public_sql::can_discount_limit(this, inputdialog.ui->lineEdit->trueValue(), QObject::tr("折扣"), discount_desc)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, discount_desc);
    } else {
        //enableValueMaxMinus， 则value返回的折扣的值
        int discount = inputdialog.ui->lineEdit->value();

        for(int row = 0; row < tablemodel->rowCount(); row ++) {
            if(tablemodel->can_discount(row)) {
                tablemodel->model_data_set(row, "int_discount", discount);
                tableView->updateTableHeight(row);
            }
        }
    }
}

void w_scr_dish_main::toTaxExec()
{

    QString tax_name1 = n_func::f_get_sysparm("tax_name1", QObject::tr("税率") + "1");
    QString tax_name2 = n_func::f_get_sysparm("tax_name2", QObject::tr("税率") + "2");
    QString tax_name3 = n_func::f_get_sysparm("tax_name3", QObject::tr("税率") + "3");

    int ch_tax_type = tablemodel->get_cey_u_taxsale_master_value(ch_billno, "ch_tax_type").toInt();

    if(ch_tax_type == 1) {//单一
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tablemodel->get_cey_u_taxsale_master_desc(ch_billno));
    }
    if(ch_tax_type == 2) {//三选一
        int index = lds_messagebox::question(this, MESSAGE_TITLE_VOID,
                                             tablemodel->get_cey_u_taxsale_master_desc(ch_billno)
                                             , tax_name1, tax_name2, tax_name3,  QObject::tr("取消"));
        double tax_1 = 0;
        double tax_2 = 0;
        double tax_3 = 0;
        switch(index) {
        case 3:
            return;
        case 0:
            tax_1 = tablemodel->get_cey_sys_parameter_value("pay_fixed_tax_value").toDouble();
            break;
        case 1:
            tax_2 = tablemodel->get_cey_sys_parameter_value("pay_fixed_tax_2_value").toDouble();
            break;
        case 2:
            tax_3 = tablemodel->get_cey_sys_parameter_value("pay_fixed_tax_3_value").toDouble();
            break;
        }

        qrtVariantPairList bind_value_list;
        bind_value_list
                << qrtVariantPair("int_fix_tax", tax_1)
                << qrtVariantPair("int_fix_tax_2", tax_2)
                << qrtVariantPair("int_fix_tax_3", tax_3);

        tablemodel->tablename_kvmap_insert_field("cey_u_taxsale_master", ch_billno, bind_value_list);

        lds_query_hddpos::updateTable("cey_u_taxsale_master", bind_value_list, QString("ch_billno = '%1' ").arg(ch_billno)  );
    }
    if(ch_tax_type == 3) {//累加
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tablemodel->get_cey_u_taxsale_master_desc(ch_billno));
    }
}

void w_scr_dish_main::update_type_subtype_view()
{
    ftableview_standmodel_sql *model_subtype = static_cast<ftableview_standmodel_sql *>(tableView_subtype->m);
    int dish_width = lds::MAIN_WINDOW_SIZE.width() - lds::PAY_LEFT_SPACE;

    int layout_border = ftableview_delegate::CELL_BORDER_SPACE;
    int type_y = tableView_dish->y() - 2 * lds::QT_FIXED_HEIGHT_BOTTOM;

    if(lds::sysconf->value("system_setting/dish_type_enabled", false).toBool()) {
        tableView_subtype->fillData_addAnimation(model_subtype->columnCount(), 1);
        lds::showWidget(tableView_type);
        lds::showWidget(tableView_subtype);
        tableView_type->updateTableRect(lds::PAY_LEFT_SPACE,  type_y - layout_border, dish_width, lds::QT_FIXED_HEIGHT_BOTTOM + layout_border,
                                        layout_border, 0, layout_border, layout_border);
        tableView_subtype->updateTableRect(lds::PAY_LEFT_SPACE, type_y + lds::QT_FIXED_HEIGHT_BOTTOM, dish_width, lds::QT_FIXED_HEIGHT_BOTTOM,
                                           layout_border, 0, layout_border, layout_border);
    } else {
        tableView_subtype->fillData_addAnimation(model_subtype->columnCount(), 2);
        lds::hideWidget(tableView_type);
        lds::showWidget(tableView_subtype);

        tableView_subtype->updateTableRect(lds::PAY_LEFT_SPACE, type_y - layout_border, dish_width, 2 * lds::QT_FIXED_HEIGHT_BOTTOM + layout_border,
                                           layout_border, 0, layout_border, layout_border);
    }
}

bool w_scr_dish_main::printdish_tran_saction(QString &errstring)
{
    lds_query_hddpos::tran_saction();
    QStringList insert_int_flowid_list;

    if(!try_commit_data_update_num_cost(errstring, insert_int_flowid_list)) {
        lds_query_hddpos::roll_back();
        return false;
    }
    lds_query_hddpos::com_mit();
    //打印
    qDebug() << "insert_int_flowid_list:" << insert_int_flowid_list;
    if(insert_int_flowid_list.count() > 0) {
        //消费单
        if(lds::sysconf->value("system_setting/luodanzongdan").toString() == "1") {
            public_printby_ch_billno printer(this->ch_billno);
            printer.toprint_preprint(lds::sysconf->value("system_setting/printerpath").toString(), QObject::tr("消费单"), QString(" int_flowid in (%1) ").arg(insert_int_flowid_list.join(",")));
        }//success
        //标签
        public_printby_ch_billno::print_label(this->ch_billno, insert_int_flowid_list);
        tablemodelSelect(tablemodel->cur_ch_billno(), this);
        //厨打
        public_sql::save_login->to_everytime_to_print_failed_kitchen_bill();
    }
    return true;
}

bool w_scr_dish_main::try_commit_data_update_num_cost(QString &errstring, QStringList &insert_int_flowid_list)
{
    //try_commit_data
    if(!tablemodel->try_commit_data(errstring, fexpandmain_model_sqltablemodel_data::commitAll, &insert_int_flowid_list)) {
        return false;
    }
    //update_num_cost
    if(!update_num_cost(errstring)) {
        return false;
    }
    return true;
}

bool w_scr_dish_main::update_num_cost(QString &errstring)
{
    lds_query_hddpos  query;
    if(!query.execUpdate("cey_u_table",qrtVariantPairList()
                         << qrtVariantPair("num_cost", tablemodel->getKeepdata().dish_total()),
                         QString(" ch_billno = '%1' ").arg(this->ch_billno))) {
        errstring = query.recordError();
        return false;
    }
    return true;
}


void w_scr_dish_main::keyPressEvent(QKeyEvent *e)
{
    int key = e->key();
    if(false == public_short_cut_map::keyPressQt(this, e))
        return;
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

        //输入编号时，条码时
        if(barcode_scan == 0) {

            barcode_scan = new QLineEdit(this);
            barcode_scan->setEnabled(false);
            barcode_scan->setGeometry((this->width() - 300) / 2, (this->height() - 40) / 2, 300, 40);
            barcode_scan->setProperty("outer_stylesheet", QVariant("lineedit_qt_input"));
        }
        barcode_scan->insert(QString::number(key - Qt::Key_0));

        lds::showWidget(barcode_scan);

    }
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:

        if(barcode_scan != 0
                && barcode_scan->text().length() > 0) {

            toScanbar(barcode_scan->text());
            barcode_scan->clear();
            lds::hideWidget(barcode_scan);

        } else {

            topaybill();
        }

        break;
    case Qt::Key_Backspace:
        if(barcode_scan != 0) {
            barcode_scan->backspace();
            if(barcode_scan->text() == "") {
                lds::hideWidget(barcode_scan);
            }
        }
        break;
    default:
        break;
    }

    //
    QDialog::keyPressEvent(e);
}

void w_scr_dish_main::loadControl(ftableview *tableView_dish, ftableview *tableView_type, ftableview *tableView_subtype, fexpandmain_tableview *tableView, QLabel *label_logo, QLabel *label_text, public_pushbutton_pay *pushButton_pay, lds_label_doublespinbox *widget_total, lds_label_doublespinbox *widget_change, QLabel *label_total, lds_roundeddialog_title_frame *frame_deskinfo, fexpandmain_dish_btn *frame_dish_bottom2)
{
    this->tableView_dish = tableView_dish;
    this->tableView_type = tableView_type;
    this->tableView_subtype = tableView_subtype;
    this->tableView = tableView;

    this->label_logo = label_logo;
    this->label_text = label_text;
    this->pushButton_pay = pushButton_pay;

    this->widget_change = widget_change;
    this->widget_total = widget_total;

    this->label_total = label_total;
    this->frame_deskinfo =frame_deskinfo;

    this->frame_dish_bottom2 = frame_dish_bottom2;
}

QLineEdit *w_scr_dish_main::get_barcode_scan()
{
    barcode_scan = new QLineEdit(this);
    barcode_scan->setEnabled(false);
    barcode_scan->setGeometry((this->width() - 300) / 2, (this->height() - 40) / 2, 300, 40);
    barcode_scan->setProperty("outer_stylesheet", QVariant("lineedit_qt_input"));
    return barcode_scan;
}
