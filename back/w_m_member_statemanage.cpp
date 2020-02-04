
#include "w_m_member_statemanage.h"
//会员状态管理
#include "ui_w_m_member_statemanage_dialog.h"
#include <QSqlError>
#include <QSqlRecord>
#include "lds_tableview_repecify.h"
#include "w_m_member.h"
#include "w_m_member_list.h"
#include "n_func.h"
#include "lds_messagebox.h"
#include "lds_query_hddpos.h"
#include "uo_rw_member.h"
#include "backheader.h"
#include "lds_tableview_repecify.h"
#include "lds_tableview_delegate_check.h"
#include "w_scr_dish_m_hang_pay_dialog.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "lds.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "ui_w_m_member_trans_dialog.h"
#include "ui_w_m_member_state_dialog.h"


w_m_member_statemanage::w_m_member_statemanage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_m_member_statemanage_Dialog)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);
    //! init
    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("t_m_member");
    tablemodel->save_set_header("vch_memberno",QObject::tr("会员编号"));
    tablemodel->save_set_header("vch_member",QObject::tr("会员名称"));
    tablemodel->save_set_header("ch_typeno",QObject::tr("会员类型"));
    tablemodel->save_set_header("dt_birthday",QObject::tr("生日"));
    tablemodel->save_set_header("vch_handtel",QObject::tr("手机"));
    tablemodel->save_set_header("vch_tel",QObject::tr("电话"));
    tablemodel->save_set_header("ch_state",QObject::tr("状态"));
    tablemodel->save_set_header("ch_cardflag",QObject::tr("持卡"));
    tablemodel->save_set_header("dt_limit",QObject::tr("有效日期"));
    tablemodel->save_set_header("vch_operID",QObject::tr("创建人员"));

    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader( tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_cardflag"),
                                            new lds_tableview_delegate_check(tablemodel->fieldIndex("ch_cardflag"),  ui->tableView));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_state"),
                                            new lds_model_sqltablemodel_delegate_com(this, &w_m_member::get_VIP_MODEL_STATE()));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_typeno"),
                                            new lds_model_sqltablemodel_delegate_com(this, tablemodel, "t_m_member_type", "ch_typeno", "vch_typename"));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_operID"),
                                            new lds_model_sqltablemodel_delegate_com(this, tablemodel, "("+backheader::SELECT_OPERID_NAME + ")t", "vch_operID", "vch_operator"));
    //! toolbar
    ui->widget->setup(QStringList()
                      << QObject::tr("刷新") << QObject::tr("详情")
                      << QObject::tr("冻结") << QObject::tr("挂失") << QObject::tr("作废") << QObject::tr("启用")
                      << QObject::tr("退现") << QObject::tr("取款") << QObject::tr("退卡") << QObject::tr("补卡")
                      << QObject::tr("挂账付款")
                      << QObject::tr("退出")
                      );
    connect(ui->widget->index_widget(QObject::tr("刷新")), SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(QObject::tr("详情")), SIGNAL(clicked()),this,SLOT(todetail()));
    connect(ui->widget->index_widget(QObject::tr("冻结")), SIGNAL(clicked()),this,SLOT(tostop()));
    connect(ui->widget->index_widget(QObject::tr("挂失")), SIGNAL(clicked()),this,SLOT(toloss()));
    connect(ui->widget->index_widget(QObject::tr("作废")), SIGNAL(clicked()),this,SLOT(toblank()));
    connect(ui->widget->index_widget(QObject::tr("启用")), SIGNAL(clicked()),this,SLOT(toopen()));
    connect(ui->widget->index_widget(QObject::tr("退现")), SIGNAL(clicked()),this,SLOT(toblackcash()));
    connect(ui->widget->index_widget(QObject::tr("取款")), SIGNAL(clicked()),this,SLOT(tofetch()));
    connect(ui->widget->index_widget(QObject::tr("退卡")), SIGNAL(clicked()),this,SLOT(toblankcard()));
    connect(ui->widget->index_widget(QObject::tr("补卡")), SIGNAL(clicked()),this,SLOT(tonewsend()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(QObject::tr("挂账付款")), SIGNAL(clicked()),this,SLOT(tohangpay()));
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(treeselect()));
    connect(ui->lineEdit_2,SIGNAL(textChanged(QString)),this,SLOT(treeselect()));

    tablemodel_arg2 = " ( vch_memberno like '%1%' or vch_member like '%1%' )and(ifnull(vch_tel, '') like '%2%' or ifnull(vch_handtel, '') like '%2%' ) ";
    //! refresh
    QTimer::singleShot(0, this, SLOT(torefresh()));
}


lds_tableView *w_m_member_statemanage::getTableview()
{
    return ui->tableView;
}

lds_model_sqltablemodel *w_m_member_statemanage::getTablemodel()
{
    return tablemodel;
}

void w_m_member_statemanage::torefresh()
{
    //yun
    w_sys_manage_cloudsync_with_time_running::exec_no_up_then_down_byconf(this);
    ui->widget->index_widget(QObject::tr("退出"))->show_flag_top_right(w_sys_manage_cloudsync_with_time_running::q_bt_get_ready_total_byconf());
    //~yun

    treeselect();
}

void w_m_member_statemanage::treeselect()
{
    tablemodel->setFilter(tablemodel_arg2.arg(ui->lineEdit->text()).arg(ui->lineEdit_2->text()));
    ui->tableView->restore();
    ui->tableView->resizeColumnsToContents();
}

void w_m_member_statemanage::todetail()
{
    w_m_member_list::statictosendcard(this, tablemodel, ui->tableView, false, QObject::tr("会员卡详情"));
}

void w_m_member_statemanage::tostop()
{
    //ue_stop (none) returns (none)
    long ll_row;
    QString ls_memberno;
    QString ls_state;
    QString ls_memo;
    QString ls_err;
    QString ls_save_hq;
    QDateTime ldt;
    QDateTime ldt_old;


    ll_row = ui->tableView->currentIndex().row();
    if(ll_row<0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有会员被选中"));
        return;
    }
    ls_memberno=tablemodel->model_data(ll_row, "vch_memberno").toString();
    ls_state=tablemodel->model_data(ll_row, "ch_state").toString();
    //    items.clear();items<<new QStandardItem("1")<<new QStandardItem(QObject::tr("正常"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("2")<<new QStandardItem(QObject::tr("止付"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("3")<<new QStandardItem(QObject::tr("挂失"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("4")<<new QStandardItem(QObject::tr("作废"));VIP_MODEL_STATE.appendRow(items);
    if(ls_state == "2") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("指定会员状态已为止付"));
        return;

    } else if(ls_state == "4") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("指定会员状态已为作废"));
        return;
    }
    w_m_member_statemanage_state dialog(this);
    dialog.setTitile(QObject::tr("会员号") + ":" +ls_memberno + "," + QObject::tr("止付"));
    dialog.textkey=QObject::tr("止付");
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(transparentCenterDialog(&dialog).exec()==QDialog::Rejected)
        return;
    ls_memo=dialog.getExplain();

    if(!ls_memo.isEmpty()) {
    }
    if(ls_memo == "OK")ls_memo="";
    ldt = n_func::f_get_sysdatetime();
    ldt_old=tablemodel->model_data(ll_row, "dt_operdate").toDateTime();
    if(ldt_old.isValid()  &&  ldt_old > ldt) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("系统时间小于上次保存时间，请确认系统时间是否正确"));
        return;

    }
    //! <--transaction begin-->
    lds_query_hddpos::tran_saction();
    lds_query_hddpos  query;
    switch(0) {
    case 0:
        ls_err=n_func::f_m_insert_log(ls_memberno,QObject::tr("止付"),ls_memo,0);
        if(!ls_err.isEmpty()) {
            break;
        }
        if(!query.execUpdate("t_m_member",qrtVariantPairList()
                             << qrtVariantPair("ch_state", 2)
                             << qrtVariantPair("dt_operdate", ldt),
                             QString("vch_memberno = '%1' ").arg(ls_memberno),
                             &ls_err
                             )) {
            break;
        }
    }
    if(ls_err.isEmpty()) {
        lds_query_hddpos::com_mit();
        ls_err=QObject::tr("状态变更成功");

    } else {

        lds_query_hddpos::roll_back();
        ls_err=QObject::tr("状态变更失败");

    }
    //! <--transaction end-->
    torefresh();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_err);

}

void w_m_member_statemanage::toloss()
{
    //ue_loss (none) returns (none)
    long ll_row;
    QString ls_memberno;
    QString ls_state;
    QString ls_err;
    QString ls_memo;
    QDateTime ldt;
    QDateTime ldt_old;
    ll_row = ui->tableView->currentIndex().row();
    if(ll_row<0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有会员被选中"));
        return;
    }
    ls_memberno=tablemodel->model_data(ll_row, "vch_memberno").toString();
    ls_state=tablemodel->model_data(ll_row, "ch_state").toString();
    //    items.clear();items<<new QStandardItem("1")<<new QStandardItem(QObject::tr("正常"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("2")<<new QStandardItem(QObject::tr("止付"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("3")<<new QStandardItem(QObject::tr("挂失"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("4")<<new QStandardItem(QObject::tr("作废"));VIP_MODEL_STATE.appendRow(items);
    if(ls_state == "3") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("指定会员状态已为挂失"));
        return;

    } else if(ls_state == "4") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("指定会员状态已为作废"));
        return;
    }
    w_m_member_statemanage_state dialog(this);
    dialog.setTitile(QObject::tr("会员号") + ":" +ls_memberno + "," + QObject::tr("挂失"));
    dialog.textkey=QObject::tr("挂失");
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(transparentCenterDialog(&dialog).exec()==QDialog::Rejected) return;
    ls_memo=dialog.getExplain();

    if(!ls_memo.isEmpty()) {
    }
    if(ls_memo == "OK")ls_memo="";
    ldt = n_func::f_get_sysdatetime();
    ldt_old=tablemodel->model_data(ll_row, "dt_operdate").toDateTime();
    if(ldt_old.isValid()  &&  ldt_old > ldt) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("系统时间小于上次保存时间，请确认系统时间是否正确"));
        return;
    }

    //! <--transaction begin-->
    lds_query_hddpos::tran_saction();
    lds_query_hddpos  query;
    switch(0) {
    case 0:
        ls_err=n_func::f_m_insert_log(ls_memberno,QObject::tr("挂失"),ls_memo,0);
        if(!ls_err.isEmpty()) {
            break;
        }
        if(!query.execUpdate("t_m_member",qrtVariantPairList()

                             << qrtVariantPair("ch_state", 3)
                             << qrtVariantPair("dt_operdate", ldt),
                             QString(" vch_memberno = '%1' ").arg(ls_memberno),
                             &ls_err
                             )) {
            break;
        }
    }
    if(ls_err.isEmpty()) {
        lds_query_hddpos::com_mit();
        ls_err=QObject::tr("状态变更成功");

    } else {

        lds_query_hddpos::roll_back();
        ls_err=QObject::tr("状态变更失败");

    }
    //! <--transaction end-->
    torefresh();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_err);
}

void w_m_member_statemanage::toblank()
{
    //ue_blank (none) returns (none)
    long ll_row;
    QString ls_memberno;
    QString ls_err;
    //    st_member lst_member;
    uo_rw_member::read_card_type lst_card;
    uo_rw_member uo_member;
    QDateTime ldt;
    QDateTime ldt_old;
    QString ls_type;
    QString ls_bute;
    QString ls_cardflag;
    QString ls_state;
    QString ls_memo;
    double ldec_base=0;

    ll_row = ui->tableView->currentIndex().row();
    if(ll_row<0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有会员被选中"));
        return;
    }
    ls_memberno=tablemodel->model_data(ll_row, "vch_memberno").toString();
    ls_state=tablemodel->model_data(ll_row, "ch_state").toString();
    ls_cardflag=tablemodel->model_data(ll_row, "ch_cardflag").toString();
    //    items.clear();items<<new QStandardItem("1")<<new QStandardItem(QObject::tr("正常"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("2")<<new QStandardItem(QObject::tr("止付"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("3")<<new QStandardItem(QObject::tr("挂失"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("4")<<new QStandardItem(QObject::tr("作废"));VIP_MODEL_STATE.appendRow(items);
    if(ls_state == "4") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("指定会员状态已为作废"));
        return;

    } else if(ls_state == "2") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("止付卡不能作废"));
        return;
    }
    if(ls_cardflag!="Y") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("该会员卡还未制卡，不需要作废"));
        return;
    }

    //获取会员详细数据
    uo_member.of_init(this, "4");
    lst_card=uo_member.of_read_card(this, ls_memberno);
    if(lst_card.flag == false) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,lst_card.errtext);
        return;
    }
    lds_query_hddpos  query;
    query.execSelect(QString("select int_basedata,ch_typeno from t_m_member where vch_memberno='%1'").arg(ls_memberno));
    query.next();
    ldec_base=query.recordValue("int_basedata").toDouble();
    ls_type=query.recordValue("ch_typeno").toString();

    query.execSelect(QString("select ch_attribute from t_m_member_type where ch_typeno = '%1' ")
                     .arg(ls_type));
    if(query.next()) {
        ls_bute=query.recordValue("ch_attribute").toString();
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("查询会员类型失败"));
        return;
    }
    if(ls_bute!="1") {
        if(n_func::f_m_get_parameter(this,"m_fetch_base","0") == "1") {
            if(lst_card.amount>0) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("卡余额(包含保本金额,保本金额可取)不为零，可进行取款操作，待余额为零再作废"));
                return;
            }
            ldec_base=0;
        } else {
            if(lst_card.amount>ldec_base) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("卡余额(不包含保本金额,保本金额不可取)不为零，可进行取款操作，待余额为零再作废"));
                return;
            }
        }
    }
    w_m_member_statemanage_state dialog(this);
    dialog.setTitile(QObject::tr("会员号") + ":" +ls_memberno + "," + QObject::tr("作废"));
    dialog.textkey=QObject::tr("作废");
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(transparentCenterDialog(&dialog).exec()==QDialog::Rejected) return;
    ls_memo=dialog.getExplain();
    if(!ls_memo.isEmpty()) {
    }
    if(ls_memo == "OK")ls_memo="";
    ldt = n_func::f_get_sysdatetime();
    ldt_old=tablemodel->model_data(ll_row, "dt_operdate").toDateTime();
    if(ldt_old.isValid()  &&  ldt_old > ldt) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("系统时间小于上次保存时间，请确认系统时间是否正确"));
        return;
    }

    //! <--transaction begin-->
    lds_query_hddpos::tran_saction();
    switch(0) {
    case 0:
        ls_err=n_func::f_m_insert_log(ls_memberno,QObject::tr("作废"),ls_memo,0);
        if(!ls_err.isEmpty()) {
            break;
        }
        if(!query.execUpdate("t_m_member",qrtVariantPairList()

                             << qrtVariantPair("ch_state", 4)
                             << qrtVariantPair("dt_operdate", ldt),
                             QString(" vch_memberno = '%1' ").arg(ls_memberno),
                             &ls_err)) {
            break;
        }
    }
    if(ls_err.isEmpty()) {
        lds_query_hddpos::com_mit();
        ls_err=QObject::tr("状态变更成功");

    } else {

        lds_query_hddpos::roll_back();
        ls_err=QObject::tr("状态变更失败");

    }
    //! <--transaction end-->
    torefresh();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_err);

}

void w_m_member_statemanage::toopen()
{
    //ue_loss (none) returns (none)
    long ll_row;
    QString ls_memberno;
    QString ls_state;
    QString ls_err;
    QString ls_memo;
    QDateTime ldt;
    QDateTime ldt_old;
    ll_row = ui->tableView->currentIndex().row();
    if(ll_row<0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有会员被选中"));
        return;
    }
    ls_memberno=tablemodel->model_data(ll_row, "vch_memberno").toString();
    ls_state=tablemodel->model_data(ll_row, "ch_state").toString();
    //    items.clear();items<<new QStandardItem("1")<<new QStandardItem(QObject::tr("正常"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("2")<<new QStandardItem(QObject::tr("止付"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("3")<<new QStandardItem(QObject::tr("挂失"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("4")<<new QStandardItem(QObject::tr("作废"));VIP_MODEL_STATE.appendRow(items);
    if(ls_state == "1") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("指定会员状态已为正常"));
        return;

    } else if(ls_state == "4") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("指定会员状态已为作废"));
        return;
    }
    w_m_member_statemanage_state dialog(this);
    dialog.setTitile(QObject::tr("会员号") + ":" +ls_memberno + "," + QObject::tr("启用"));
    dialog.textkey=QObject::tr("启用");
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(transparentCenterDialog(&dialog).exec()==QDialog::Rejected) return;
    ls_memo=dialog.getExplain();

    if(!ls_memo.isEmpty()) {
    }
    if(ls_memo == "OK")ls_memo="";
    ldt = n_func::f_get_sysdatetime();
    ldt_old=tablemodel->model_data(ll_row, "dt_operdate").toDateTime();
    if(ldt_old.isValid()  &&  ldt_old > ldt) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("系统时间小于上次保存时间，请确认系统时间是否正确"));
        return;
    }

    //! <--transaction begin-->
    lds_query_hddpos::tran_saction();
    lds_query_hddpos  query;
    switch(0) {
    case 0:
        ls_err=n_func::f_m_insert_log(ls_memberno,QObject::tr("启用"),ls_memo,0);
        if(!ls_err.isEmpty()) {
            break;
        }

        if(!query.execUpdate("t_m_member",qrtVariantPairList()

                             << qrtVariantPair("ch_state", 1)
                             << qrtVariantPair("dt_operdate", ldt),
                             QString(" vch_memberno = '%1' ").arg(ls_memberno),
                             &ls_err)) {
            break;
        }
    }
    if(ls_err.isEmpty()) {
        lds_query_hddpos::com_mit();
        ls_err=QObject::tr("状态变更成功");

    } else {

        lds_query_hddpos::roll_back();
        ls_err=QObject::tr("状态变更失败");

    }
    //! <--transaction end-->
    torefresh();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_err);
}

void w_m_member_statemanage::toblackcash()
{
    //ue_blank_cash (none) returns (none)
    long ll_row;
    QString ls_memberno;
    QString ls_err;
    n_func::st_member_type  lst_member;
    uo_rw_member::read_card_type lst_card;
    uo_rw_member uo_member;
    QDateTime ldt;
    QDateTime ldt_old;
    double ldec_bcash;

    ll_row = ui->tableView->currentIndex().row();
    if(ll_row<0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有会员被选中"));
        return;
    }
    ls_memberno=tablemodel->model_data(ll_row, "vch_memberno").toString();
    lst_member = n_func::f_m_get_limit(this, ls_memberno);
    //    items.clear();items<<new QStandardItem("1")<<new QStandardItem(QObject::tr("正常"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("2")<<new QStandardItem(QObject::tr("止付"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("3")<<new QStandardItem(QObject::tr("挂失"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("4")<<new QStandardItem(QObject::tr("作废"));VIP_MODEL_STATE.appendRow(items);


    if(lst_member.as_m_state != "3") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("退现功能只针对挂失状态会员"));
        return;
    }
    if(lst_member.as_m_att != "2") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("退现功能只针对充值类会员"));
        return;
    }

    ldt = n_func::f_get_sysdatetime();
    ldt_old=tablemodel->model_data(ll_row, "dt_operdate").toDateTime();
    if(ldt_old.isValid()  &&  ldt_old > ldt) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("系统时间小于上次保存时间，请确认系统时间是否正确"));
        return;
    }

    //    uo_member = CREATE uo_rw_member
    uo_member.of_init(this, "4");
    lst_card = uo_member.of_read_card(this, ls_memberno);
    if(lst_card.flag == false) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,lst_card.errtext);
        return;
    }
    if(lst_card.flag) {
        if(lst_card.amount==0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员当前余额为零，无需退现"));
            return;
        }
        ldec_bcash = lst_card.amount - lst_member.adec_m_limitdata;
        if(ldec_bcash <= 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员当前无保本外可退现金额"));
            return;
        }


        if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("会员退现处理").arg(lst_member.as_member)
                                       + "\n" + QObject::tr("会员") + ":" + lst_member.as_member
                                       + "\n" + QObject::tr("会员余额") + ":" + QString().sprintf("%0.2f", ldec_bcash)
                                      ,  QObject::tr("确定"), QObject::tr("取消"))) {
            //! <--transaction begin-->
            lds_query_hddpos::tran_saction();
            ls_err= n_func::f_m_insert_log(ls_memberno,
                                           QObject::tr("退现"),
                                           QObject::tr("金额") + QString().sprintf("%0.2f", ldec_bcash),
                                           0);
            if(ls_err.isEmpty()) {
                ls_err=n_func::f_m_deposit(this, ls_memberno, - ldec_bcash, - ldec_bcash,lst_member.adec_m_limitdata,"5","5",QObject::tr("挂失退现转结"));
            }
            if(ls_err.isEmpty()) {
                lds_query_hddpos::com_mit();
            } else {
                lds_query_hddpos::roll_back();
            }
            //! <--transaction end-->
            torefresh();
            if(ls_err.isEmpty()) {
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("退现成功"));

            } else {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("读取会员帐务数据失败"));
            }
        }
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,lst_card.errtext);
    }
}

void w_m_member_statemanage::tofetch()
{
    //ue_fetch (none) returns (none)
    w_m_member_list::statictorecharge(this, tablemodel, ui->tableView,"C");
    torefresh();
}

void w_m_member_statemanage::toblankcard()
{
    //ue_blank_card (none) returns (none)
    w_m_member_list::statictorecharge(this, tablemodel, ui->tableView,"D");
    torefresh();
}

void w_m_member_statemanage::tonewsend()
{
    //ue_new_send (none) returns (none)
    long ll_row;
    QString ls_memberno;
    QString ls_err;
    QString ls_value;
    n_func::st_member_type  lst_member;
    uo_rw_member::read_card_type lst_card;
    uo_rw_member uo_member;
    QDateTime ldt;
    QDateTime ldt_old;
    double ldec_bcash;

    uo_member.of_init(this, "4");
    ll_row = ui->tableView->currentIndex().row();
    if(ll_row<0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有会员被选中"));
        return;
    }
    ls_memberno=tablemodel->model_data(ll_row, "vch_memberno").toString();
    lst_member = n_func::f_m_get_limit(this, ls_memberno);

    if(lst_member.as_m_state != "3") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("补卡功能只针对挂失状态会员"));
        return;
    }
    if(lst_member.as_m_att != "2") {
        if(lst_member.as_m_att == "3") {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("挂帐会员请结清帐务后，创建新会员并发卡"));
            return;
        } else {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("补卡功能只针对充值类会员"));
            return;
        }
    }

    if(lst_member.as_m_cardflag!="Y") {
        ls_value = w_m_member::get_vip_card_type(this);
        if(ls_value == "1" || ls_value == "2") {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员卡%1还未发卡").arg(ls_memberno));
            return;
        }
    }

    ldt = n_func::f_get_sysdatetime();
    ldt_old = tablemodel->model_data(ll_row, "dt_operdate").toDateTime();
    if(ldt_old.isValid() && ldt_old > ldt) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("系统时间小于上次保存时间，请确认系统时间是否正确"));
        return;
    }
    lst_card = uo_member.of_read_card(this, ls_memberno);
    if(lst_card.flag == false) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,lst_card.errtext);
        return;
    }
    if(lst_card.flag) {
        if(lst_card.amount==0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员当前余额为零，无需补卡，请直接创建新会员并发卡"));
            return;
        }
        ldec_bcash = lst_card.amount - lst_member.adec_m_limitdata;
        if(ldec_bcash <= 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员当前无保本外可用金额"));
            return;
        }
        /*开始补卡*/
        w_m_member_statemanage_trans dialog(this);
        if(dialog.initData1()) {
            dialog.setWindowTitle(_TEXT_SLOT(this));transparentCenterDialog(&dialog).exec();
            torefresh();
        }
        return;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("读取会员帐务数据失败"));
    return;
}

void w_m_member_statemanage::toexit()
{
    w_sys_manage_cloudsync_cloud2::exec_keepdata_byconf(this);

    this->reject();
}

void w_m_member_statemanage::tohangpay()
{
    QPushButton *b0 = qobject_cast<QPushButton *>(this->sender());
    w_scr_dish_m_hang_pay_Dialog dialog(this);
    dialog.setWindowTitle(b0->text());
    transparentCenterDialog(&dialog).exec();
}

//===============================
w_m_member_statemanage_state::w_m_member_statemanage_state(QWidget *parent):
    QDialog(parent),ui(new Ui_w_m_member_state_Dialog)
{
    ui->setupUi(this);
    ui->widget->setup(QStringList()<<QObject::tr("确定")<<QObject::tr("取消"), Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("确定")),SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->widget->index_widget(QObject::tr("取消")),SIGNAL(clicked()),this,SLOT(reject()));

}

void w_m_member_statemanage_state::setTitile(const QString &text)
{
    ui->label->setText(text);
}

QString w_m_member_statemanage_state::getExplain()
{
    return ui->lineEdit->text();
}

void w_m_member_statemanage_state::took()
{

    if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确定指定会员")+textkey+"?", QObject::tr("确定"), QObject::tr("取消"))) {
        this->accept();
        return;
    }
    this->reject();
}

void w_m_member_statemanage_state::tocancel()
{
}

//===================================
w_m_member_statemanage_trans::w_m_member_statemanage_trans(QWidget *parent):
    QDialog(parent),
    ui(new Ui_w_m_member_trans_Dialog),
    _parent(static_cast<w_m_member_statemanage *>(parent))
{

    ui->setupUi(this);
    cb_read1=new uo_rw_member;
    cb_read2=new uo_rw_member;

    cb_read1->of_init(this, "1");
    cb_read2->of_init(this, "1");

    is_type=QObject::tr("补卡");
    is_lmemberno=_parent->getTablemodel()->model_data(_parent->getTableview()->currentIndex().row(), "vch_memberno").toString();
    //! toolbar
    ui->widget->setup(QStringList() << QObject::tr("补卡") << QObject::tr("退出"), Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("补卡")), SIGNAL(clicked()),this,SLOT(tonewcard()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));

}
w_m_member_statemanage_trans::~w_m_member_statemanage_trans()
{
    delete cb_read1;
    delete cb_read2;
}


void w_m_member_statemanage_trans::tonewcard()
{
    //clicked (none) returns long [pbm_bnclicked]
    //long ancestorreturnvalue
    uo_rw_member::read_card_type lst_card1;
    uo_rw_member::read_card_type lst_card2;
    uo_rw_member::read_card_type lst_card;
    n_func::st_member_type lst_member1;
    uo_rw_member uo_member;
    QString ls_memberno1;
    QString ls_memberno2;
    QString ls_err;
    QString ls_memo;
    QString ls_hq_save;
    QString ls_cardno;
    double ldec_trans;
    QDateTime ldt;


    uo_member.of_init(this, "4");
    ls_memberno1 = ui->lineEdit_oldvipno->text();
    ls_memberno2 = ui->lineEdit_newvipno->text();
    ls_cardno = ui->lineEdit_newcardno->text();
    if(ls_memberno1.isEmpty() || ls_memberno2.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("请先输入新卡卡号"));
        return;
    }
    if(ls_cardno.trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("请先输入新卡卡面卡号"));
        return;
    }

    ldec_trans = ui->lineEdit_tonewnum->value();
    if(ldec_trans <= 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("转入新卡金额为零"));
        return;
    }
    if(1==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确定要执行此%1操作吗?").arg(this->is_type), QObject::tr("确定"), QObject::tr("取消"))) {
        return;
    }

    //! <--transaction begin-->
    lds_query_hddpos::tran_saction();
    lds_query_hddpos  query;
    switch(0) {//4
    case 0:
        if(ls_memberno1==ls_memberno2) {
            ls_err=QObject::tr("转入库会员卡号不能与转出卡号相同");
            break;
        }

        if((ls_err=this->wf_addsendcard()).isEmpty()) {//3
            lst_card1 = uo_member.of_read_card(this, ls_memberno1);
            if(lst_card1.flag == false) {
                ls_err=lst_card1.errtext;
                break;
            }
            lst_card2 = uo_member.of_read_card(this, ls_memberno2);
            if(lst_card2.flag == false) {
                ls_err=lst_card2.errtext;
                break;
            }
            lst_member1 = n_func::f_m_get_limit(this, ls_memberno1);

            if(ui->checkBox_istonewyajin->isChecked()) {
                if(lst_card1.amount<ldec_trans) {
                    ls_err=QObject::tr("原挂失会员余额不足以完成本次补卡操作");
                    break;
                }
            } else {
                if((lst_card1.amount - lst_member1.adec_m_limitdata) < ldec_trans) {
                    ls_err=QObject::tr("转出会员余额除保本金额外，不足以完成本次转帐操作") + "\n"
                            + QObject::tr("转出余额") + ":" + QString().sprintf("%0.2f", lst_card1.amount) + "\n"
                            + QObject::tr("保本金额") + ":" + QString().sprintf("%0.2f", lst_member1.adec_m_limitdata);
                    break;
                }
            }
            if((ls_err=n_func::f_m_deposit(this, ls_memberno1, - ldec_trans,0,lst_card1.amount - ldec_trans,"5","5",QObject::tr("挂失补卡转帐"))).isEmpty()) {//3
                if((ls_err=n_func::f_m_deposit(this, ls_memberno2,ldec_trans,0,lst_card2.amount + ldec_trans,"5","5",QObject::tr("挂失补卡转帐"))).isEmpty()) {//3
                    if(lst_card1.point !=0 ) {//2
                        if(!query.execUpdate("t_m_curamount",qrtVariantPairList()

                                             << qrtVariantPair("num_point", lst_card1.point),
                                             QString(" vch_memberno = '%1' ").arg(ls_memberno2),
                                             &ls_err)) {
                            break;
                        }
                        if(!query.execInsert("t_m_point",qrtVariantPairList()

                                             << qrtVariantPair("vch_memberno", ls_memberno2)
                                             << qrtVariantPair("ch_type", 5)
                                             << qrtVariantPair("num_point", lst_card1.point)
                                             << qrtVariantPair("vch_operid", lds::gs_operid)
                                             << qrtVariantPair("dt_operdate", n_func::f_get_sysdatetime())

                                             << qrtVariantPair("vch_memo", QObject::tr("系统")+QObject::tr("补卡")),
                                             &ls_err)) {
                            break;
                        }
                    }//2
                    if((ls_err=cb_read2->of_send_card(this, ls_memberno2)).isEmpty()) {//1
                        if((ls_err=n_func::f_m_insert_log(ls_memberno1,QObject::tr("补卡"),QObject::tr("目标卡号") + ls_memberno2 + QObject::tr("转出金额") + QString().sprintf("%0.2f", ldec_trans),0)).isEmpty()) {//1
                            if((ls_err=n_func::f_m_insert_log(ls_memberno2,QObject::tr("补卡"),QObject::tr("原卡号") + ls_memberno1 + QObject::tr("转入金额") + QString().sprintf("%0.2f", ldec_trans),0)).isEmpty()) {//1
                                /*success*/
                                break;
                            }//1
                        }//1
                    }//1
                    break;
                }//3
            }//3
        }//3
    }//4

    if(ls_err.isEmpty()) {
        lds_query_hddpos::com_mit();
        ls_err = QObject::tr("操作成功") + ":" + this->is_type;
    } else {
        lds_query_hddpos::roll_back();
    }
    //! <--transaction end-->

    _parent->torefresh();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_err);
}

void w_m_member_statemanage_trans::toexit()
{
    this->reject();
}

bool w_m_member_statemanage_trans::initData1()
{
    //clicked (none) returns long [pbm_bnclicked]
    //long ancestorreturnvalue
    uo_rw_member::read_card_type lst_card;
    n_func::st_member_type  lst_member;

    lst_card = cb_read1->of_read_card(this, is_lmemberno);
    if(lst_card.flag == false) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,lst_card.errtext);
        return false;
    }
    if(lst_card.flag) {
        QString m_state = w_m_member::get_VIP_MODEL_STATE().value(n_func::f_m_get_state(is_lmemberno)).toString();
        if(m_state != QObject::tr("挂失")) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("非挂失卡"));
            return false;
        }

        lst_member=n_func::f_m_get_item(this, lst_card.cardid);
        if(lst_member.as_m_att != "2") {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("非充值卡").arg(m_state));
            return false;
        }

        if(!lst_card.cardid.isEmpty() && lst_card.cardid==ui->lineEdit_newvipno->text()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("原挂失会员与新发卡会员不能为同一会员"));
            return false;
        }
        ui->lineEdit_oldvipno->setText(lst_card.cardid);
        ui->lineEdit_oldvipname->setText(lst_member.as_member);
        lst_member=n_func::f_m_get_limit(this, is_lmemberno);
        ii_basedata=int(lst_member.adec_m_limitdata);
        ui->lineEdit_tonewnum->setValue(lst_card.amount-lst_member.adec_m_limitdata);

        ui->lineEdit_oldamount->setValue(lst_card.amount);
        ui->lineEdit_oldpoint->setValue( lst_card.point);

        ui->lineEdit_newvipname->setText(ui->lineEdit_oldvipname->text());
        ui->lineEdit_newamount->setValue(ui->lineEdit_oldamount->value());
        ui->lineEdit_newpoint->setValue(ui->lineEdit_oldpoint->value());

        ui->lineEdit_tonewnum->setFocus();
        ui->lineEdit_tonewnum->selectAll();
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,lst_card.errtext);
        return false;

    }
    return true;
}

QString w_m_member_statemanage_trans::wf_addsendcard()
{
    //Public function wf_addsendcard (none) returns integer
    QString ls_oldmemberno;
    QString ls_newmemberno;
    QString ls_cardno;
    int li_exists;
    int li_basedata=0;

    if(!ui->checkBox_istonewyajin->isChecked()) {
        li_basedata=this->ii_basedata;
    }

    ls_oldmemberno = ui->lineEdit_oldvipno->text();
    ls_newmemberno = ui->lineEdit_newvipno->text();
    ls_cardno = ui->lineEdit_newcardno->text();

    lds_query_hddpos  query;
    query.execSelect(QString("select count(1) from t_m_member where vch_member ='%1'").arg(ls_newmemberno));
    query.next();
    li_exists=query.recordValue(0).toInt();
    if(li_exists > 0) {
        return QObject::tr("会员卡号%1已被使用").arg(ls_newmemberno);
    }
    li_exists = 0;
    query.execSelect(QString("select count(1) from t_m_member where vch_cardno = '%1'").arg(ls_cardno));
    query.next();
    li_exists=query.recordValue(0).toInt();
    if(li_exists > 0) {
        return QObject::tr("此会员卡面卡号%1已被使用").arg(ls_newmemberno);
    }
    if(query.execInsert("t_m_member",
                        QString("select '%1' as `vch_memberno`, vch_member, ch_typeno, vch_id, dt_birthday,"
                                "vch_tel, vch_handtel, vch_address, dt_limit, 1 as `ch_state`,"
                                "num_limit, 'N' as `ch_cardflag`, vch_password, '%2' as `vch_operID`, now() as `dt_operdate`,"
                                " '%3' as `vch_cardno` from t_m_member where vch_memberno = '%4' ")
                        .arg(ls_newmemberno)
                        .arg(lds::gs_operid)
                        .arg(ls_cardno)
                        .arg(ls_oldmemberno))) {
        if(query.execUpdate("t_m_member",qrtVariantPairList()
                            << qrtVariantPair("ch_state", 4)
                            << qrtVariantPair("vch_cardno", QVariant())
                            << qrtVariantPair("int_basedata", li_basedata),
                            QString(" vch_memberno = '%1' ").arg(ls_oldmemberno))) {
            return "";
        }
    }
    return QObject::tr("操作失败") + ":" + query.recordError();
}
