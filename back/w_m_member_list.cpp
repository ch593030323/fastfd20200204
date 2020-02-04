//会员资料维护
#include "w_m_member.h"
#include "w_m_member_list.h"
#include "ui_w_m_member_dialog.h"
#include "lds_query_hddpos.h"
#include "w_scr_dish_phone_pay.h"
#include "n_func.h"
#include "lds_messagebox.h"
#include <QSqlError>
#include <QtDebug>
#include "lds_model_sqltablemodel.h"
#include "lds_tableview_repecify.h"
#include "backheader.h"
#include "reader_rf.h"
#include "w_sys_manage_basicdataexport.h"
#include "public_cika_read_dialog.h"
#include "ui_w_m_member_sendcard_dialog.h"
#include "w_sys_manage_basicdataimport.h"
#include "w_scr_dish_pay_widget.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "w_bt_dish.h"
#ifdef Q_OS_WIN
#include "readcard_mwrf32.h"
#endif
#include "lds_model_sqltablemodel_delegate_com.h"
#include "ui_w_m_member_news_dialog.h"
#include "ui_w_m_member_pwdchange_dialog.h"
#include "ui_w_m_member_list_recharge_dialog.h"
#include "ui_w_m_member_list_type_new_dialog.h"
#include "ui_w_bt_dish_unitset_dialog.h"
#include "lds_model_sqltablemodel.h"
#include "lds_tableview_repecify.h"
#include <QDataWidgetMapper>
#include <QSqlRelationalDelegate>
#include <QTableView>
#include "w_m_member_list_yun_card_info.h"
#include "public_printby_ch_billno.h"
#include "w_m_member_list_recharge_scheme.h"
#include "ui_lds_dialog_input_dialog.h"

lds_const_bool w_m_member_list::is_yun_member = false;

w_m_member_list::w_m_member_list(QWidget *parent) :
    QDialog(parent), ui(new Ui_w_m_member_Dialog)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);


    //! init
    //! treewidget
    ui->treeWidget->header()->hide();
    ui->treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeWidget->setColumnCount(1);

    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("t_m_member");
    //! refresh
    tablemodel->save_set_header("vch_memberno", QObject::tr("会员编号"));
    tablemodel->save_set_header("vch_cardno", QObject::tr("卡面卡号"));
    tablemodel->save_set_header("vch_member", QObject::tr("会员名称"));
    tablemodel->save_set_header("dt_birthday", QObject::tr("生日"));
    tablemodel->save_set_header("vch_tel", QObject::tr("电话"));

    tablemodel->save_set_header("vch_handtel", QObject::tr("手机"));
    tablemodel->save_set_header("ch_state", QObject::tr("状态"));
    tablemodel->save_set_header("ch_cardflag", QObject::tr("持卡状态"));
    tablemodel->save_set_header("dt_limit", QObject::tr("有效期"));

    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_state"), new lds_model_sqltablemodel_delegate_com(this, &w_m_member::get_VIP_MODEL_STATE()));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_cardflag"), new lds_model_sqltablemodel_delegate_com(this, &w_m_member::get_VIP_MODEL_FLAG()));
    //! toolbar
    ui->widget->setup(QStringList()
                      << QObject::tr("会员类型")<< QObject::tr("新增") << QObject::tr("删除") << QObject::tr("批量新增")
                      << QObject::tr("发卡") << QObject::tr("充值") << QObject::tr("设密码")
                      << QObject::tr("刷新")
                      << QObject::tr("退出")
                      );


    connect(ui->widget->index_widget(QObject::tr("新增")), SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget->index_widget(QObject::tr("删除")), SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->widget->index_widget(QObject::tr("批量新增")), SIGNAL(clicked()),this,SLOT(tonews()));
    connect(ui->widget->index_widget(QObject::tr("会员类型")), SIGNAL(clicked()),this,SLOT(tomembertype()));
    connect(ui->widget->index_widget(QObject::tr("发卡")), SIGNAL(clicked()),this,SLOT(tosendcard()));
    connect(ui->widget->index_widget(QObject::tr("充值")), SIGNAL(clicked()),this,SLOT(torecharge()));
    connect(ui->widget->index_widget(QObject::tr("设密码")), SIGNAL(clicked()),this,SLOT(tosetpwd()));
    connect(ui->widget->index_widget(QObject::tr("刷新")), SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));

    connect(ui->treeWidget,SIGNAL(pressed(QModelIndex)),this,SLOT(treeselect()));
    connect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(tosendcard()));
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(treeselect()));
    connect(ui->lineEdit,SIGNAL(returnPressed()),this,SLOT(tosendcard()));
    connect(ui->lineEdit_2,SIGNAL(textChanged(QString)),this,SLOT(treeselect()));

    if(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_no_use_local_member", false).toBool()) {
        ui->widget->index_widget(QObject::tr("新增"))->setEnabled(false);
        ui->widget->index_widget(QObject::tr("删除"))->setEnabled(false);
        ui->widget->index_widget(QObject::tr("会员类型"))->setEnabled(false);
        ui->widget->index_widget(QObject::tr("批量新增"))->setEnabled(false);
    }
    ui->lineEdit->installEventFilter(this);
    tablemodel_arg3 =
            "    if('%1' = '', t_m_member.ch_typeno like '%', t_m_member.ch_typeno = '%1') "
            " and (vch_memberno like '%2%' or vch_member like '%2%' ) "
            " and (ifnull(vch_tel, '') like '%3%' or ifnull(vch_handtel, '') like '%3%') "
            ;
    QTimer::singleShot(0, this, SLOT(torefresh()));
}

lds_tableView *w_m_member_list::getTableview()
{
    return ui->tableView;
}

lds_treeWidget *w_m_member_list::getTreew()
{
    return ui->treeWidget;
}

lds_model_sqltablemodel *w_m_member_list::getTablemodel()
{
    return tablemodel;
}

void w_m_member_list::torefresh()
{
    //treewidget
    ui->treeWidget->deleteAllItem();
    ui->treeWidget->rootitem=new QTreeWidgetItem(QStringList()
                                                 <<QObject::tr("全部类型"));
    ui->treeWidget->addTopLevelItem(ui->treeWidget->rootitem);
    lds_query_hddpos  query;
    query.execSelect("select concat(vch_typename,'[',ch_typeno,']') as 'nametype',ch_typeno  from t_m_member_type "
                     );
    while(query.next()) {
        QTreeWidgetItem *item=new QTreeWidgetItem(QStringList()
                                                  <<QString("%1")
                                                  .arg(query.recordValue("nametype").toString())
                                                  );
        ui->treeWidget->rootitem->addChild(item);
    }
    ui->treeWidget->setItemSizeHint();//更新所有的 item的sizehint
    ui->treeWidget->restore();
    //tableview
    treeselect();
    ui->lineEdit->setFocus();
}

void w_m_member_list::treeselect()
{
    QString ch_typeno;
    QString vch_memberno;
    QString vch_tel;

    if(ui->treeWidget->currentItem()) {
        int index=ui->treeWidget->deepofitem(ui->treeWidget->currentItem());
        if(index==1) {
            ch_typeno=backheader::getinterdata(ui->treeWidget->currentItem()->text(0));
        }
    }
    vch_memberno = ui->lineEdit->text();
    vch_tel = ui->lineEdit_2->text();
    tablemodel->setFilter(tablemodel_arg3.arg(ch_typeno).arg(vch_memberno).arg(vch_tel));
    ui->tableView->restore();
    ui->tableView->resizeColumnsToContents();
}

void w_m_member_list::setToolbarShown(bool f)
{
    ui->widget->index_widget(QObject::tr("会员类型"))->setVisible(f);
    ui->widget->index_widget(QObject::tr("删除"))->setVisible(f);
    ui->widget->index_widget(QObject::tr("批量新增"))->setVisible(f);
    ui->widget->index_widget(QObject::tr("充值"))->setVisible(f);
    ui->widget->index_widget(QObject::tr("设密码"))->setVisible(f);
    //    ui->widget->index_widget(QObject::tr("导出"))->setVisible(f);
    ui->widget->index_widget(QObject::tr("退出"))->setVisible(f);
    disconnect(ui->lineEdit,SIGNAL(returnPressed()),this,SLOT(tosendcard()));
}

void w_m_member_list::tonew()
{
    w_m_member_list_new dialog(this, tablemodel, ui->tableView);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
}

void w_m_member_list::todel()
{
    int row=ui->tableView->currentIndex().row();
    if(row<0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有会员被选中"));
        return;
    }
    QString ls_memberno=tablemodel->model_data(row, "vch_memberno").toString();
    if(tablemodel->model_data(row, "ch_cardflag").toString() == "Y") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已经发卡会员"));
        return;
    }

    lds_query_hddpos  query;
    query.execSelect(QString("select count(1) from t_m_deposit where vch_memberno='%1'").arg(ls_memberno));
    query.next();
    if(query.recordValue(0).toLongLong()>0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员已经存在充值记录"));
        return;
    }
    query.execSelect(QString("select count(1) from t_m_pay where vch_memberno='%1'").arg(ls_memberno));
    query.next();
    if(query.recordValue(0).toLongLong()>0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员已经存在消费记录"));
        return;
    }

    query.execSelect(QString("select count(1) "+n_func::f_m_switch_product("check_data_table","1") + " where vch_voucherno ='%1' and ch_paymodeno='01'").arg(ls_memberno));
    query.next();
    if(query.recordValue(0).toLongLong()>0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员已经存在消费记录"));
        return;
    }
    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认删除该"), QObject::tr("确定"), QObject::tr("取消"))) {
        //! <--transaction begin-->
        lds_query_hddpos::tran_saction();
        if(query.execDelete("t_m_member", QString("vch_memberno='%1'").arg(ls_memberno))) {
            lds_query_hddpos::com_mit();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        }else {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
        }
        //! <--transaction end-->
        torefresh();
    }

}

void w_m_member_list::tonews()
{
    w_m_member_news dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
}

void w_m_member_list::tomembertype()
{
    w_m_member_list_type dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
    torefresh();
}

void w_m_member_list::tosendcard()
{
    QString errstring;
    //云会员
    int selectindex = 1;//1本地会员
    if(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_member", false).toBool()) {
        if(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_no_use_local_member", false).toBool()) {
            selectindex = 0;
        } else {
            selectindex = lds_messagebox::question(this, MESSAGE_TITLE_VOID,  QObject::tr("选择模式") +"\n"+  QObject::tr("检测到您已经启用云会员"),QObject::tr("云会员"), QObject::tr("本地会员"),QObject::tr("退出"));
        }
    }
    switch(selectindex) {
    case 0://云会员
    {
        w_m_member_list_yun_auto_close d;
        d.yunOpen();

        lds_dialog_input inputdialog(this);
        inputdialog.setWindowTitle(QObject::tr("云会员编号"));
        inputdialog.ui->label->setText("");
        //获取所有的会员类型和类型号
        if(QDialog::Accepted != transparentCenterDialog(&inputdialog).exec()) {
            return;
        }
        w_m_member_list_yun_card_info dialog(this);
        if(false == dialog.initData(inputdialog.ui->lineEdit->text(), errstring)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
            return;
        }
        transparentCenterDialog(&dialog).exec();
    }
        return;
    case 1://本地会员
    {
        //普通
        if(ui->tableView->currentIndex().row() >= 0) {
            statictosendcard(this, tablemodel, ui->tableView,true);
        } else {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有会员被选中"));
        }
    }
        return;
    case 2://退出
        return;
    }
}

void w_m_member_list::statictosendcard(QWidget *parent, lds_model_sqltablemodel *tablemodel,lds_tableView *tableview, bool isenabled, const QString &title)
{
    int row=tableview->currentIndex().row();
    if(row>=0) {
        w_m_member_list_sendcard dialog(parent, tablemodel, tableview, isenabled);
        dialog.setWindowTitle(title.isEmpty()?QObject::tr("会员卡发卡"):title);
        transparentCenterDialog(&dialog).exec();
    }    else {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("没有会员被选中"));
    }
}

void w_m_member_list::statictorecharge(QWidget *parent, lds_model_sqltablemodel *tablemodel, lds_tableView *tableview, const QString &parm)
{
    if(parm == "C") {//取款金额
    } else if(parm == "D") {//退卡金额
    } else {
        if(!saas_pay_detail::SaaSFuncCheck(parent, "0303"/*云、本地会员储值*/)) {
            return;
        }
    }

    bool filter_by_handtel = true;
    if(!parm.isEmpty()
            ||(parm.isEmpty()&&backheader::operate_right_check_code(parent, "0404"))) {//充值
        w_m_member_list_recharge dialog(parent, tablemodel, tableview, parm, filter_by_handtel);
        transparentCenterDialog(&dialog).exec();
        //        dialog.exec();
    }
}

void w_m_member_list::statictomembertype(w_m_member_list *parent)
{
    w_m_member_list_type dialog(parent);
    dialog.setWindowTitle(QObject::tr("会员类型"));
    transparentCenterDialog(&dialog).exec();

}

bool w_m_member_list::static_to_memeber_readcard(QWidget *parent, QString &cardid, uo_rw_member &uo_member, bool isSearchMember, bool isManualYun)
{
    vch_membernoData::chooseFlag choose = vch_membernoData::choose_local;
    if(isManualYun
            &&lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_member", false).toBool()
            ) {
        if(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_no_use_local_member", false).toBool()) {
            choose = vch_membernoData::choose_yun;
        } else {
            choose = vch_membernoData::choose_switch;
        }
    }

    return static_to_memeber_readcard_d(parent, cardid, uo_member, isSearchMember, choose);
}

bool w_m_member_list::static_to_memeber_readcard_d(QWidget *parent, QString &cardid, uo_rw_member &uo_member, bool isSearchMember, vch_membernoData::chooseFlag choose)
{
    w_m_member_list::yun_member_close();

    switch(choose) {
    ////////////////////////////////////////////////////
    case vch_membernoData::choose_switch:
        switch(lds_messagebox::question(parent, "",  QObject::tr("检测到您已经启用云会员"),QObject::tr("云会员"), QObject::tr("本地会员"),QObject::tr("退出"))) {
        case 0://云会员
            w_m_member_list::yun_member_open();
            break;
        case 1://本地会员
            break;
        default:
            return false;
        }//switch
        break;
        ////////////////////////////////////////////////////
    case vch_membernoData::choose_local:
        break;
        ////////////////////////////////////////////////////
    case vch_membernoData::choose_yun:
        w_m_member_list::yun_member_open();
        break;
    }
    lds_messagebox_loading_show loading(parent, "", "LOADING");
    loading.show_delay();

    return static_to_memeber_readcard_private(parent, cardid, uo_member, isSearchMember);
}

bool w_m_member_list::static_to_memeber_readcard_pay(QWidget *parent, QString &cardid, uo_rw_member &uo_member, vch_membernoData::chooseFlag choose)
{
    w_m_member_list::yun_member_close();

    switch(choose) {
    ////////////////////////////////////////////////////
    case vch_membernoData::choose_switch:
        switch(lds_messagebox::question(parent, "",  QObject::tr("检测到您已经启用云会员"), QObject::tr("云会员"), QObject::tr("本地会员"), QObject::tr("退出"))) {
        case 0://云会员
            w_m_member_list::yun_member_open();
            break;
        case 1://本地会员
            break;
        default:
            return false;
        }//switch
        break;
        ////////////////////////////////////////////////////
    case vch_membernoData::choose_local:
        break;
        ////////////////////////////////////////////////////
    case vch_membernoData::choose_yun:
        w_m_member_list::yun_member_open();
        break;
    }

    lds_messagebox_loading_show loading(parent, "", "LOADING");
    loading.show_delay();

    bool isSearchMember = n_func::f_m_get_parameter(parent,"local_member_select","1") == "1";
    return static_to_memeber_readcard_private(parent, cardid, uo_member, isSearchMember);
}

void w_m_member_list::yun_member_open()
{
    is_yun_member.setOther(true);
}

void w_m_member_list::yun_member_close()
{
    w_sys_manage_cloudsync::json_free();
    is_yun_member.setOther(false);
}

bool w_m_member_list::static_to_memeber_readcard_private(QWidget *parent, QString &cardid, uo_rw_member &uo_member, bool isSearchMember)
{
    bool ok;
    QString errstring;
    QString cardno;

    uo_member.of_init(parent, "4", &ok);
    if(!ok) {
        goto failure;
    }
#ifdef QT_DEBUG
    uo_member.is_cardtype = "2";
#endif
    if(uo_member.is_cardtype == "1") {//ic卡
        QString icpasswd=n_func::f_m_get_parameter(parent,"vip_ic_password");
        QByteArray transferpasswd = n_func::f_m_get_cardpwdtransfer(icpasswd);
#ifdef Q_OS_WIN
        ReadCard_mwrf32 reader;reader.setDefPwd(n_func::f_m_defcardpwd);//需要设置一次默认密码，应为changepwd会不完全清卡
#else
        Reader_RF reader;
#endif
        QString cardno_tmp1, cardno_tmp2;
        if(reader.setDefPwd(transferpasswd)) {
            if(reader.readcard(cardno, cardno_tmp1, cardno_tmp2)) {
                goto success;
            }
        }
        errstring=QObject::tr("操作失败")+reader.lastError();
    }  else if(uo_member.is_cardtype == "2") {//磁卡

        public_cika_read_Dialog dialog(parent);
        //是前台支付但不是云会员
        if(isSearchMember) {
            dialog.setmemberlist();
        }
        dialog.setWindowTitle((w_m_member_list::is_yun_member?QObject::tr("云会员") : QObject::tr("会员") ) + QObject::tr("读卡"));
        if(transparentCenterDialog(&dialog).exec()==QDialog::Accepted) {
            cardno=dialog.getcardno();
            goto success;
        }
    } else {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("无会员卡"));
        return false;
    }
    goto failure;
success:
    cardid=cardno.trimmed();
    return true;
failure:
    if(!errstring.isEmpty()) {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,errstring);
    }
    w_m_member_list::yun_member_close();
    return false;
}

void w_m_member_list::torecharge()
{
    w_m_member_list::statictorecharge(this, tablemodel, ui->tableView,"");
}

void w_m_member_list::tosetpwd()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有行被选中"));
        return;
    }
    QString vch_memberno = tablemodel->model_data(row, "vch_memberno").toString();
    w_m_member_pwdchange dialog(vch_memberno, this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        this->torefresh();
    }
}

void w_m_member_list::toexport()
{
    w_sys_manage_basicdataexport dialog(this);
    dialog.reset_export_tablenames(QStringList() << tablemodel->tableName());
    dialog.reset_export_confname(QString("%1/path").arg(this->metaObject()->className()));
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();

}

void w_m_member_list::toimport()
{
    w_sys_manage_basicdataimport_sub dialog(QStringList() << tablemodel->tableName(), QString("%1/path").arg(this->metaObject()->className()), this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
}

void w_m_member_list::toexit()
{
    //yun
    w_sys_manage_cloudsync_with_time_running::exec_no_up_then_down_byconf(this);
    ui->widget->index_widget(QObject::tr("退出"))->show_flag_top_right(w_sys_manage_cloudsync_with_time_running::q_bt_get_ready_total_byconf());
    //~yun
    w_sys_manage_cloudsync_cloud2::exec_keepdata_byconf(this);

    this->reject();
}

bool w_m_member_list::eventFilter(QObject *o, QEvent *e)
{
    if(QEvent::KeyPress == e->type()) {
        QKeyEvent *ke = ((QKeyEvent *)e);
        const int kvalue = ke->key();
        //kvalue;//46 32+14 0x2e
        if(kvalue == Qt::Key_Up) {
            ui->tableView->selectRow(qMax(ui->tableView->currentIndex().row() - 1, 0));
            e->ignore();
            return true;
        }
        if(kvalue == Qt::Key_Down) {
            ui->tableView->selectRow(qMin(ui->tableView->currentIndex().row() + 1, tablemodel->rowCount()));
            e->ignore();
            return true;
        }
    }
    return QDialog::eventFilter(o, e);
}

//====================================

w_m_member_list_sendcard::w_m_member_list_sendcard(QWidget *parent, lds_model_sqltablemodel *m, lds_tableView *tableview, bool isenable):
    QDialog(parent),
    ui(new Ui_w_m_member_sendcard_Dialog),    _tablemodel(m),
    _tableview(tableview),
    _parent(parent)
{
    ui->setupUi(this);
    ui->label_vch_handtel->setText(htmlCannotEmpty(ui->label_vch_handtel->text()));
    ui->label_vch_member->setText(htmlCannotEmpty(ui->label_vch_member->text()));
    ui->label_vch_memberno->setText(htmlCannotEmpty(ui->label_vch_memberno->text()));

    ui->comboBox_ch_state->addItemsByMap(w_m_member::get_VIP_MODEL_STATE());
    ui->comboBox_ch_cardflag->addItemsByModel(&w_m_member::get_VIP_MODEL_FLAG());
    ui->comboBox_ch_typeno->addItemsBySql(" select ch_typeno, vch_typename from t_m_member_type ");
    //! datawidget
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(_tablemodel);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));

    mapper->addMapping(ui->comboBox_ch_cardflag, _tablemodel->fieldIndex("ch_cardflag"),"comusrdata");
    mapper->addMapping(ui->comboBox_ch_state, _tablemodel->fieldIndex("ch_state"), "comusrdata");
    mapper->addMapping(ui->lineEdit_vch_address, _tablemodel->fieldIndex("vch_address"));
    mapper->addMapping(ui->lineEdit_vch_cardno, _tablemodel->fieldIndex("vch_cardno"));
    mapper->addMapping(ui->lineEdit_vch_handtel, _tablemodel->fieldIndex("vch_handtel"));
    mapper->addMapping(ui->lineEdit_vch_id, _tablemodel->fieldIndex("vch_id"));
    mapper->addMapping(ui->lineEdit_vch_member, _tablemodel->fieldIndex("vch_member"));
    mapper->addMapping(ui->lineEdit_vch_memberno, _tablemodel->fieldIndex("vch_memberno"));
    mapper->addMapping(ui->lineEdit_vch_operid, _tablemodel->fieldIndex("vch_operid"));
    mapper->addMapping(ui->lineEdit_vch_tel, _tablemodel->fieldIndex("vch_tel"));
    mapper->addMapping(ui->dateTimeEdit_dt_birthday_show,_tablemodel->fieldIndex("dt_birthday"));
    mapper->addMapping(ui->dateTimeEdit_dt_limit_show,_tablemodel->fieldIndex("dt_limit"));
    mapper->addMapping(ui->dateTimeEdit_dt_operdate_show,_tablemodel->fieldIndex("dt_operdate"));
    mapper->addMapping(ui->comboBox_ch_typeno, _tablemodel->fieldIndex("ch_typeno"), "comusrdata");
    //! tableview_other
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("t_m_member_other");
    ui->tableView_other->setModel(tablemodel);
    ui->tableView_other->setEditTriggers(QTableView::AllEditTriggers);
    tablemodel->save_set_header("vch_item", QObject::tr("项目"), true);
    tablemodel->save_set_header("vch_memo", QObject::tr("内容"), true);
    ui->tableView_other->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());

    //! tableview_log
    tablemodel_3=new lds_model_sqltablemodel(this);
    tablemodel_3->setTable("t_m_member_log");
    ui->tableView_log->setModel(tablemodel_3);
    tablemodel_3->save_set_header("vch_memberno", QObject::tr("会员编号"));
    tablemodel_3->save_set_header("vch_matter", QObject::tr("项目"));
    tablemodel_3->save_set_header("vch_memo", QObject::tr("内容"));
    tablemodel_3->save_set_header("vch_operID", QObject::tr("操作人员"));
    tablemodel_3->save_set_header("dt_operdate", QObject::tr("操作时间"));
    ui->tableView_log->rePecifyHeader(tablemodel_3->get_save_set_header_saver_column_list());
    //! toolbar
    ui->widget_2->setup(QStringList()<<QObject::tr("新增")<<QObject::tr("删除"), Qt::AlignTop);
    ui->widget->setup(QStringList()<<QObject::tr("上一行")<<QObject::tr("下一行")<<QObject::tr("保存")<<QObject::tr("发卡")<<QObject::tr("退出"), Qt::AlignRight);
    connect(ui->widget_2->index_widget(QObject::tr("新增")),SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget_2->index_widget(QObject::tr("删除")),SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->widget->index_widget(QObject::tr("发卡")),SIGNAL(clicked()),this,SLOT(tosendcard()));
    connect(ui->widget->index_widget(QObject::tr("上一行")),SIGNAL(clicked()),this,SLOT(toup()));
    connect(ui->widget->index_widget(QObject::tr("下一行")),SIGNAL(clicked()),this,SLOT(todown()));
    connect(ui->widget->index_widget(QObject::tr("保存")),SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_ch_typeno, SIGNAL(clicked()), this, SLOT(toChangeType()));

    //! after
    //获取is_cardtype的内容
    is_cardtype = w_m_member::get_vip_card_type(this);
    //只是查看详细信息的
    if(!isenable) {
        ui->frame_detail_show->setEnabled(isenable);
        ui->widget_2->setEnabled(isenable);
        ui->widget->index_widget(QObject::tr("发卡"))->setVisible(isenable);
        ui->widget->index_widget(QObject::tr("保存"))->setVisible(isenable);
    }
    //初始化数据
    cb5=new uo_rw_member(this);
    cb5->of_init(this, "2");//2是磁卡
    ui->comboBox_ch_cardflag->setEnabled(false);
    ui->comboBox_ch_state->setEnabled(false);
    ui->lineEdit_vch_operid->setEnabled(false);
    ui->dateTimeEdit_dt_operdate_show->setEnabled(false);

    ui->lineEdit_vch_memberno->setEnabled(false);
    ui->lineEdit_vch_cardno->setEnabled(false);
    ui->comboBox_ch_typeno->setEnabled(false);
    //更新当前index
    ui->label_record->setText(QString("%1/%2").arg(_tableview->currentIndex().row()+1).arg(_tableview->verticalHeader()->count()));

    //设置datetimelineedit为只有日期
    ui->dateTimeEdit_dt_birthday_show->setDisplayFormat("yyyy-MM-dd");
    ui->dateTimeEdit_dt_limit_show->setDisplayFormat("yyyy-MM-dd");
    ui->dateTimeEdit_dt_operdate_show->setDisplayFormat("yyyy-MM-dd");

    //! TIMESLOT
    QTimer::singleShot(0, this, SLOT(torefresh()));//信号槽的重写是需要这的写的，实现subclass 同时继承
}

void w_m_member_list_sendcard::tonew()
{
    int rowcount=tablemodel->rowCount();
    tablemodel->insertRow(rowcount);
    tablemodel->model_data_set(rowcount, "vch_memberno", ui->lineEdit_vch_memberno->text());
    tablemodel->model_data_set(rowcount, "vch_operid", lds::gs_operid);
}

void w_m_member_list_sendcard::todel()
{
    int row=ui->tableView_other->currentIndex().row();
    if(row<0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    tablemodel->removeRow(row);
    if(tablemodel->trySubmitAll()) {
        torefresh();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("删除成功"));
        return;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误"));
}

void w_m_member_list_sendcard::tosendcard()
{
    lds_query_hddpos  query;
    query.execSelect(QString(" select vch_memberno from t_m_member where vch_memberno = '%1' ")
                     .arg(ui->lineEdit_vch_memberno->text()));
    if(false == query.next()) {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("会员卡不存在"), QObject::tr("新建"), QObject::tr("取消"))) {
            return;
        }
        if(false == createMember()) {
            return;
        }
    }
    sendCard();
}

void w_m_member_list_sendcard::toup()
{
    if(_tablemodel->model_is_dirty()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"))) {
        case 0:
            if(!tosave()) {
                return;
            }
            break;
        case 1:
            break;
        }
    }
    _tableview->goprevious();
    mapper->setCurrentIndex(_tableview->currentIndex().row());
    torefresh();
    ui->label_record->setText(QString("%1/%2").arg(_tableview->currentIndex().row()+1).arg(_tableview->verticalHeader()->count()));
}

void w_m_member_list_sendcard::todown()
{
    if(_tablemodel->model_is_dirty()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"))) {
        case 0:
            if(!tosave()) {
                return;
            }
            break;
        case 1:
            break;
        }
    }
    int row = _tableview->gonext();
    mapper->setCurrentIndex(row);
    torefresh();
    ui->label_record->setText(QString("%1/%2").arg(row+1).arg(_tableview->verticalHeader()->count()));
}

bool w_m_member_list_sendcard::tosave()
{
    QDateTime ldt;
    QDateTime ldt_old;
    QDateTime ldt_limit;
    QDateTime ldt_birth;
    QString ls_errtext;
    QString ls_memberno;
    QString ls_membername;
    QString ls_typeno;
    QString ls_attribute;
    QString ls_cardno;
    lds_query_hddpos  query;

    ls_memberno=ui->lineEdit_vch_memberno->text();
    ls_membername=ui->lineEdit_vch_member->text();
    ls_typeno=ui->comboBox_ch_typeno->comusrdata().toString();
    ls_cardno=ui->lineEdit_vch_cardno->text();
    if(ls_memberno.trimmed().isEmpty()) {
        ls_errtext=QObject::tr("会员编号");
    } else if(ls_membername.trimmed().isEmpty()) {
        ls_errtext=QObject::tr("会员名称");
    } else if(ls_typeno.trimmed().isEmpty()) {
        ls_errtext=QObject::tr("会员类型");
    } else if(ls_cardno.trimmed().isEmpty()) {
        if(is_cardtype == "1") {
            ls_errtext="";
        } else {
            ls_errtext=QObject::tr("卡面卡号");
        }
    }

    lds_query_hddpos::tran_saction();
    switch(0) {//switch
    case 0://0
        if(!ls_errtext.isEmpty()) {
            ls_errtext=ls_errtext+QObject::tr("不能为空");
            break;
        }//0
    case 1://1
        ldt_limit=ui->dateTimeEdit_dt_limit_show->dateTime();
        if(ldt_limit < n_func::f_get_sysdatetime()) {
            ls_errtext=ls_errtext+QObject::tr("有效期限不能小于当前日期");
            break;
        }//1
    case 2://2
        ldt_birth=ui->dateTimeEdit_dt_birthday_show->dateTime();
        if(ldt_birth < QDateTime::fromString("1800-01-01", "yyyy-MM-dd")) {
            ls_errtext=ls_errtext+QObject::tr("系统不支持'超年龄'会员会员生日必须在1800年后");
            break;
        }//2
    case 3:{//3
        query.execSelect(QString("select count(0) from t_m_member where vch_cardno='%1' and vch_memberno <>'%2'").arg(ls_cardno).arg(ls_memberno));
        query.next();

        if(query.recordValue(0).toLongLong() > 0) {
            ls_errtext=ls_errtext+QObject::tr("输入卡面卡号系统已存在");
            break;
        }
        if(ui->lineEdit_vch_handtel->text().trimmed().isEmpty()) {
            ls_errtext=ls_errtext+QObject::tr("手机号不能为空");
            break;
        }
    }//3
    case 4://4
        ldt=n_func::f_get_sysdatetime();
        ldt=ui->dateTimeEdit_dt_operdate_show->dateTime();
        if(ldt_old.isValid() && ldt_old > ldt) {
            ls_errtext=ls_errtext+QObject::tr("系统时间小于上次保存时间,请确认系统时间是否正确");
            break;
        }
        ui->dateTimeEdit_dt_operdate_show->setDateTime(ldt);
        //4
    case 5://5
        if(!tablemodel->trySubmitAll()) {
            ls_errtext=QObject::tr("数据库错误");
            break;
        }

        if(!tablemodel_3->trySubmitAll()) {
            ls_errtext=QObject::tr("数据库错误");
            break;
        }
        //5
    case 6:
        ls_errtext.clear();
        if(!_tablemodel->trySubmitAll(&ls_errtext)) {
            break;
        }
        query.execSelect(QString("select ch_attribute from t_m_member_type where ch_typeno='%1'").arg(ls_typeno));
        query.next();
        ls_attribute=query.recordValue(0).toString();
        if(ls_attribute == "3") {
            ls_errtext=n_func::f_m_hang_curamount(ls_memberno);
            if(!ls_errtext.isEmpty()) {
                break;
            }
        }
        if(!tablemodel->trySubmitAll(&ls_attribute)) {
            break;
        }
        if(!tablemodel_3->trySubmitAll(&ls_attribute)) {
            break;
        }
    case 7:break;
    }//switch
    //错误信息
    if(ls_errtext.isEmpty()) {//! success
        lds_query_hddpos::com_mit();
        //刷新
        torefresh();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("会员资料保存成功"));
        return true;
    }
    //! failure
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_errtext);
    return false;
}

void w_m_member_list_sendcard::toexit()
{
    this->reject();
}

void w_m_member_list_sendcard::toChangeType()
{
    w_m_member_list_type dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();

    //save
    QVariant typeno = ui->comboBox_ch_typeno->curusrdata();
    //1
    ui->comboBox_ch_typeno->clear();
    ui->comboBox_ch_typeno->addItemsBySql(" select ch_typeno, vch_typename from t_m_member_type ");
    //restore
    ui->comboBox_ch_typeno->setcomusrdata(typeno);

}

void w_m_member_list_sendcard::torefresh()
{
    _tablemodel->select();
    _tableview->resizeColumnsToContents();
    _tableview->restore();

    //mapper
    mapper->setCurrentIndex(_tableview->currentIndex().row());

    //tableView_other
    QString memberno=_tablemodel->model_data(_tableview->currentIndex().row(), "vch_memberno").toString();
    tablemodel->setFilter(QString("t_m_member_other.vch_memberno='%1'").arg(memberno));
    ui->tableView_other->restore();

    //tableView_log
    tablemodel_3->setFilter(QString("t_m_member_log.vch_memberno='%1'").arg(memberno));
    ui->tableView_log->restore();
}

bool w_m_member_list_sendcard::sendCard()
{
    //clicked (none) returns long [pbm_bnclicked]
    //long ancestorreturnvalue
    QString ls_memberno;
    QString ls_cardflag;
    QString ls_state;
    QString ls_cardno;
    QString ls_err;
    long ll_exist;
    lds_query_hddpos  query;
    ls_memberno = ui->lineEdit_vch_memberno->text();
    ls_cardflag = ui->comboBox_ch_cardflag->curusrdata().toString();
    ls_state = ui->comboBox_ch_state->curusrdata().toString();
    ls_cardno = ui->lineEdit_vch_cardno->text();

    if(ls_memberno.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员编号不能为空"));
        return false;
    }
    query.execSelect(QString(" select vch_memberno from t_m_member where vch_memberno = '%1' ")
                     .arg(ls_memberno));
    if(false == query.next()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员卡不存在"));
        return false;
    }
    if(ls_cardno.trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("卡面卡号不能为空"));
        return false;
    }
    query.execSelect(QString("Select count(0) from t_m_member  where vch_cardno ='%1' and vch_memberno <> '%2' ")
                     .arg(ls_cardno)
                     .arg(ls_memberno)
                     );
    query.next();
    ll_exist=query.recordValue(0).toLongLong();
    if(ll_exist > 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("输入的卡面卡号系统已存在"));
        return false;
    }

    //! <--transaction begin-->
    lds_query_hddpos::tran_saction();
    switch(0) {
    case 0:
        if(!query.execUpdate("t_m_member",
                             "vch_cardno", ls_cardno,
                             QString("vch_memberno = '%1' ").arg(ls_memberno))) {

            ls_err=QObject::tr("会员") + ls_memberno + QObject::tr("操作失败");
            break;
        }
        if(ls_state == "4") {
            ls_err=QObject::tr("此会员已是作废状态");
            break;
        } else if(ls_cardflag == "Y") {
            ls_err=QObject::tr("此会员已发卡，IC卡可清除卡芯片再重发，非IC卡只能进行卡状态维护");
            break;
        } else {
            if((ls_err=cb5->of_send_card(this, ls_memberno)).isEmpty()) {
                if(!(ls_err=n_func::f_m_insert_log(ls_memberno,QObject::tr("发卡"),"",0)).isEmpty()) {
                    break;
                }
            }
        }
    }
    if(ls_err.isEmpty()) {
        lds_query_hddpos::com_mit();
        torefresh();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("发卡成功"));
        return true;
    } else {
        lds_query_hddpos::roll_back();
        torefresh();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_err);
        return false;
    }
    //! <--transaction end-->
}

bool w_m_member_list_sendcard::createMember()
{
    QDateTime ldt_limit;
    QDateTime ldt_birth;
    QString ls_errtext;
    QString ls_memberno;
    QString ls_membername;
    QString ls_typeno;
    QString ls_attribute;
    QString ls_cardno;
    lds_query_hddpos  query;

    ls_memberno=ui->lineEdit_vch_memberno->text();
    ls_membername=ui->lineEdit_vch_member->text();
    ls_typeno=ui->comboBox_ch_typeno->comusrdata().toString();
    ls_cardno=ui->lineEdit_vch_cardno->text();
    if(ls_memberno.trimmed().isEmpty()) {
        ls_errtext=QObject::tr("会员编号");
    } else if(ls_membername.trimmed().isEmpty()) {
        ls_errtext=QObject::tr("会员名称");
    } else if(ls_typeno.trimmed().isEmpty()) {
        ls_errtext=QObject::tr("会员类型");
    } else if(ls_cardno.trimmed().isEmpty()) {
        if(is_cardtype == "1") {
            ls_errtext="";
        } else {
            ls_errtext=QObject::tr("卡面卡号");
        }
    }

    switch(0) {//switch
    case 0://0
        query.execSelect(QString("select count(1) from t_m_member where vch_memberno='%1'").arg(ls_memberno));
        query.next();
        if(query.recordValue(0).toLongLong() > 0) {
            ls_errtext=QObject::tr("编号已经存在，请输入新的会员编号");
            break;
        }
        if(!ls_errtext.isEmpty()) {
            ls_errtext=ls_errtext+QObject::tr("不能为空");
            break;
        }//0
    case 1://1
        ldt_limit=ui->dateTimeEdit_dt_limit_show->dateTime();
        if(ldt_limit < n_func::f_get_sysdatetime()) {
            ls_errtext=ls_errtext+QObject::tr("有效期限不能小于当前日期");
            break;
        }//1
    case 2://2
        ldt_birth=ui->dateTimeEdit_dt_birthday_show->dateTime();
        if(ldt_birth < QDateTime::fromString("1800-01-01", "yyyy-MM-dd")) {
            ls_errtext=ls_errtext+QObject::tr("系统不支持'超年龄'会员会员生日必须在1800年后");
            break;
        }//2
    case 3:{//3
        query.execSelect(QString("select count(0) from t_m_member where vch_cardno='%1' and vch_memberno <>'%2'").arg(ls_cardno).arg(ls_memberno));
        query.next();

        if(query.recordValue(0).toLongLong() > 0) {
            ls_errtext=ls_errtext+QObject::tr("输入卡面卡号系统已存在");
            break;
        }
        if(ui->lineEdit_vch_handtel->text().trimmed().isEmpty()) {
            ls_errtext=ls_errtext+QObject::tr("手机号不能为空");
            break;
        }
    }//3
    case 4://4
        //4
    case 5://5
        if(!tablemodel->trySubmitAll()) {
            ls_errtext=QObject::tr("数据库错误");

            break;
        }
        if(!tablemodel_3->trySubmitAll()) {
            ls_errtext=QObject::tr("数据库错误");

            break;
        }
        //5
    case 6:
        //! <--transaction begin-->
        lds_query_hddpos::tran_saction();
        ls_errtext.clear();
        if(!query.execInsert("t_m_member",qrtVariantPairList()

                             << qrtVariantPair("vch_memberno", ls_memberno)
                             << qrtVariantPair("vch_member", ls_membername)
                             << qrtVariantPair("ch_typeno", ls_typeno)
                             << qrtVariantPair("dt_limit", ldt_limit)
                             << qrtVariantPair("num_limit", 0)

                             << qrtVariantPair("vch_operID", lds::gs_operid)
                             << qrtVariantPair("dt_operdate", ui->dateTimeEdit_dt_operdate_show->dateTime())
                             << qrtVariantPair("vch_cardno", ls_cardno)
                             << qrtVariantPair("dt_birthday", ldt_birth)
                             << qrtVariantPair("vch_tel", ui->lineEdit_vch_tel->text())

                             << qrtVariantPair("vch_handtel", ui->lineEdit_vch_handtel->text())
                             << qrtVariantPair("vch_id", ui->lineEdit_vch_id->text())
                             << qrtVariantPair("vch_address", ui->lineEdit_vch_address->text()),
                             &ls_errtext)) {
            lds_query_hddpos::roll_back();
            break;
        }
        query.execSelect(QString("select ch_attribute from t_m_member_type where ch_typeno='%1'").arg(ls_typeno));
        query.next();
        ls_attribute=query.recordValue(0).toString();
        if(ls_attribute == "3") {
            ls_errtext=n_func::f_m_hang_curamount(ls_memberno);
            if(!ls_errtext.isEmpty()) {
                lds_query_hddpos::roll_back();
                break;
            }
        }
        //! success
        lds_query_hddpos::com_mit();
        //! <--transaction end-->
    case 7:break;
    }//switch

    //刷新
    torefresh();
    //错误信息
    if(ls_errtext.isEmpty()) {//! success
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("会员资料保存成功"));
        return true;
    }
    //! failure
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_errtext);
    return false;
}


//===================================
w_m_member_list_new::w_m_member_list_new(QWidget *parent, lds_model_sqltablemodel *tablemodel, lds_tableView *tableview, bool isnew):
    w_m_member_list_sendcard(parent, tablemodel, tableview, isnew)
{
    ui->tabWidget->setObjectName("tab");
    ui->widget->index_widget(QObject::tr("发卡"))->setVisible(true);
    ui->widget->index_widget(QObject::tr("上一行"))->setVisible(false);
    ui->widget->index_widget(QObject::tr("下一行"))->setVisible(false);
    ui->lineEdit_vch_memberno->setEnabled(true);
    ui->comboBox_ch_typeno->setEnabled(true);
    ui->label_record->setVisible(false);
    //    Removes the tab at position index from this stack of widgets. The page widget itself is not deleted.
    ui->tabWidget->removeTab(1);
    ui->tabWidget->removeTab(1);
    ui->dateTimeEdit_dt_limit_show->setDateTime(n_func::f_get_sysdatetime().addYears(30));
    ui->dateTimeEdit_dt_operdate_show->setDateTime(n_func::f_get_sysdatetime());
    ui->dateTimeEdit_dt_birthday_show->setDateTime(n_func::f_get_sysdatetime());
    ui->lineEdit_vch_operid->setText(lds::gs_operid);
    ui->comboBox_ch_cardflag->setcomusrdata("N");

    connect(ui->lineEdit_vch_memberno, SIGNAL(textChanged(QString)), ui->lineEdit_vch_cardno, SLOT(setText(QString)));
}

void w_m_member_list_new::updateVch_tel_handtel(const QString &vch_tel)
{
    ui->lineEdit_vch_tel->setText(vch_tel);
    ui->lineEdit_vch_handtel->setText(vch_tel);
}

bool w_m_member_list_new::tosave()
{
    return createMember();
}

void w_m_member_list_new::torefresh()
{
    _tablemodel->select();
    _tableview->restore();

    //mapper
    mapper->setCurrentIndex(-1);
    w_m_member_list * mlist = qobject_cast<w_m_member_list *> (_parent);
    if(mlist)
        ui->comboBox_ch_typeno->setCurrentIndex(mlist->getTreew()->get_0_Index());
    //tableView_other
    QString memberno=_tablemodel->model_data(_tableview->currentIndex().row(), "vch_memberno").toString();
    tablemodel->setFilter(QString("t_m_member_other.vch_memberno='%1'").arg(memberno));
    ui->tableView_other->restore();

    //tableView_log
    tablemodel_3->select();
    ui->tableView_log->restore();

    //新增时，默认焦点
    ui->lineEdit_vch_memberno->setFocus();

}

void w_m_member_list_new::tosavesend()
{
    if(createMember()) {
        if(sendCard()) {
            clearData();
        }
    }
}

void w_m_member_list_new::clearData()
{
    ui->lineEdit_vch_address->clear();
    ui->lineEdit_vch_cardno->clear();
    ui->lineEdit_vch_handtel->clear();
    ui->lineEdit_vch_id->clear();
    ui->lineEdit_vch_member->clear();
    ui->lineEdit_vch_memberno->clear();
    ui->lineEdit_vch_operid->clear();
    ui->lineEdit_vch_tel->clear();
}


//====================================

w_m_member_list_type::w_m_member_list_type(QWidget *parent):
    QDialog(parent)
{
    ui = new Ui_w_bt_dish_unitset_Dialog;
    ui->setupUi(this);
    //! tableview
    tablemodel=new lds_model_sqltablemodel;
    tablemodel->setTable("t_m_member_type");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_attribute"),
                                            new lds_model_sqltablemodel_delegate_com(this, &w_m_member::get_VIP_MODEL_ATTRI()));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_favourable"),
                                            new lds_model_sqltablemodel_delegate_com(this, &w_m_member::get_VIP_MODEL_FAVOUR()));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_calc"),
                                            new lds_model_sqltablemodel_delegate_com(this, &w_m_member::get_VIP_MODEL_CALC()));

    //!refresh
    torefresh();
    //! toolbar
    ui->widget->setup(QStringList()<<QObject::tr("新增")<<QObject::tr("修改")<<QObject::tr("删除")<<QObject::tr("刷新")<<QObject::tr("退出"), Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("新增")),SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget->index_widget(QObject::tr("修改")),SIGNAL(clicked()),this,SLOT(tochange()));
    connect(ui->widget->index_widget(QObject::tr("删除")),SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->widget->index_widget(QObject::tr("刷新")),SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(tochange()));

    //! after
    tablemodel->save_set_header("ch_typeno",QObject::tr("类型编号"));
    tablemodel->save_set_header("vch_typename",QObject::tr("类型名称"));
    tablemodel->save_set_header("ch_attribute",QObject::tr("会员卡种类"));
    tablemodel->save_set_header("ch_favourable",QObject::tr("优惠方式"));
    tablemodel->save_set_header("ch_calc",QObject::tr("是否积分"));

    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    if(lds::get_soft_curent_language_first() != _CN_)
        ui->tableView->resizeColumnsToContents();
}

lds_model_sqltablemodel *w_m_member_list_type::getTablemodel()
{
    return tablemodel;
}

lds_tableView *w_m_member_list_type::getTableView()
{
    return ui->tableView;
}

void w_m_member_list_type::tonew()
{
    w_m_member_list_type_new dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));transparentCenterDialog(&dialog).exec();
}

void w_m_member_list_type::todel()
{
    int row=ui->tableView->currentIndex().row();
    if(row>=0) {
        if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认删除"), QObject::tr("确认"),QObject::tr("取消"))) {
            lds_query_hddpos  query;
            query.execSelect(QString("select count(0) from t_m_member where ch_typeno='%1'").arg(tablemodel->model_data(row, "ch_typeno").toString()));
            query.next();
            if(query.recordValue(0).toLongLong() > 0) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员类型已经使用"));
                return;
            }
            tablemodel->removeRow(row);
            if(tablemodel->trySubmitAll()) {
                ui->tableView->restore();
            } else {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误"));
            }
        }
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
    }
}

bool w_m_member_list_type::tosave()
{
    return true;
}

void w_m_member_list_type::torefresh()
{
    tablemodel->select();
    ui->tableView->restore();
}

void w_m_member_list_type::toexit()
{
    this->reject();
}

void w_m_member_list_type::tochange()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }

    w_m_member_list_type_change dialog(this);
    dialog.setWindowTitle(QObject::tr("修改"));
    transparentCenterDialog(&dialog).exec();

}
//===================================
w_m_member_list_type_change::w_m_member_list_type_change(w_m_member_list_type *parent):
    QDialog(parent),
    ui(new Ui_w_m_member_list_type_new_Dialog),
    _parent(parent)
{
    ui->setupUi(this);
    ui->label_ch_typeno->setText(htmlCannotEmpty(ui->label_ch_typeno->text()));
    ui->label_vch_typename->setText(htmlCannotEmpty(ui->label_vch_typename->text()));

    ui->comboBox_ch_area_flag->addItemsByModel(&w_m_member::get_VIP_MODEL_AREA());
    ui->comboBox_ch_attribute->addItemsByModel(&w_m_member::get_VIP_MODEL_ATTRI());
    ui->comboBox_ch_calc->addItemsByModel(&w_m_member::get_VIP_MODEL_CALC());
    ui->comboBox_ch_calc_type->addItemsByModel(&w_m_member::get_VIP_MODEL_CALCTYPE());
    ui->comboBox_ch_favourable->addItemsByModel(&w_m_member::get_VIP_MODEL_FAVOUR());
    ui->comboBox_ch_pricekind->addItemsByModel(&w_m_member::get_VIP_MODEL_PRICEKING());

    connect(ui->comboBox_ch_attribute,SIGNAL(currentIndexChanged(int)),this,SLOT(is_recharge(int)));
    connect(ui->comboBox_ch_calc,SIGNAL(currentIndexChanged(int)),this,SLOT(is_point(int)));
    connect(ui->comboBox_ch_favourable,SIGNAL(currentIndexChanged(int)),this,SLOT(is_fav(int)));

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(_parent->tablemodel);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));

    mapper->addMapping(ui->lineEdit_int_backcash_rate, _parent->tablemodel->fieldIndex("int_backcash_rate"));
    mapper->addMapping(ui->lineEdit_ch_typeno, _parent->tablemodel->fieldIndex("ch_typeno"));
    mapper->addMapping(ui->lineEdit_int_calc_unit, _parent->tablemodel->fieldIndex("int_calc_unit"));
    mapper->addMapping(ui->lineEdit_int_card_baseamount, _parent->tablemodel->fieldIndex("int_card_baseamount"));
    mapper->addMapping(ui->lineEdit_int_discount, _parent->tablemodel->fieldIndex("int_discount"));
    mapper->addMapping(ui->lineEdit_vch_typename, _parent->tablemodel->fieldIndex("vch_typename"));

    mapper->addMapping(ui->comboBox_ch_area_flag, _parent->tablemodel->fieldIndex("ch_area_flag"), "comusrdata");
    mapper->addMapping(ui->comboBox_ch_attribute, _parent->tablemodel->fieldIndex("ch_attribute"), "comusrdata");
    mapper->addMapping(ui->comboBox_ch_calc, _parent->tablemodel->fieldIndex("ch_calc"), "comusrdata");
    mapper->addMapping(ui->comboBox_ch_calc_type, _parent->tablemodel->fieldIndex("ch_calc_type"), "comusrdata");
    mapper->addMapping(ui->comboBox_ch_favourable, _parent->tablemodel->fieldIndex("ch_favourable"), "comusrdata");
    mapper->addMapping(ui->comboBox_ch_pricekind, _parent->tablemodel->fieldIndex("ch_pricekind"), "comusrdata");

    //! toolbar
    ui->widget->setup(QStringList()<<QObject::tr("上一行")<<QObject::tr("下一行")<<QObject::tr("保存")<<QObject::tr("退出"), Qt::AlignRight);
    ui->widget->setup(QStringList()<<QObject::tr("保存")<<QObject::tr("退出"),Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("保存")), SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(QObject::tr("上一行")),SIGNAL(clicked()),this,SLOT(toup()));
    connect(ui->widget->index_widget(QObject::tr("下一行")),SIGNAL(clicked()),this,SLOT(todown()));

    connect(ui->comboBox_ch_calc, SIGNAL(currentIndexChanged(int)), this,SLOT(toSaaSCheck(int)));

    ui->lineEdit_ch_typeno->setEnabled(false);
    ui->comboBox_ch_attribute->setEnabled(false);
    is_fav(ui->comboBox_ch_favourable->currentIndex());
    mapperupdate();
}

bool w_m_member_list_type_change::tosave()
{
    int row = _parent->ui->tableView->currentIndex().row();
    if(_parent->tablemodel->trySubmitAll()) {
        _parent->ui->tableView->selectRow(row);
        mapperupdate();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        return true;
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误")+QString("%1").arg(__LINE__));
        return false;
    }
}

void w_m_member_list_type_change::toexit()
{
    if(_parent->tablemodel->model_is_dirty()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"),QObject::tr("取消"))) {
        case 0:
            if(!tosave()) {
                return;
            }
            break;
        case 1:
            break;
        case 2:
            return;
        }
    }
    _parent->tablemodel->select();
    _parent->ui->tableView->restore();
    this->reject();
}

void w_m_member_list_type_change::is_recharge(int index)
{
    bool enabled = (ui->comboBox_ch_attribute->comusrdata(index).toString() == "2");//充值卡
    ui->lineEdit_int_backcash_rate->setEnabled(enabled);
    ui->lineEdit_int_card_baseamount->setEnabled(enabled);
}

void w_m_member_list_type_change::is_point(int index)
{
    bool enabled = (ui->comboBox_ch_calc->comusrdata(index).toString() == "Y");//积分

    ui->comboBox_ch_area_flag->setEnabled(enabled);
    ui->comboBox_ch_calc_type->setEnabled(enabled);
    ui->lineEdit_int_calc_unit->setEnabled(enabled);

}

void w_m_member_list_type_change::is_fav(int index)
{
    ui->comboBox_ch_pricekind->setEnabled(false);
    ui->lineEdit_int_discount->setEnabled(false);
    if(ui->comboBox_ch_favourable->itemData(index).toString() == "1") {//会员特价
        ui->comboBox_ch_pricekind->setEnabled(true);
    }
    if(ui->comboBox_ch_favourable->itemData(index).toString() == "2") {//固定折扣
        ui->lineEdit_int_discount->setEnabled(true);
    }
}

void w_m_member_list_type_change::toSaaSCheck(int index)
{
    Q_UNUSED(index)

    if(ui->comboBox_ch_calc == this->sender()) {
        if(ui->comboBox_ch_calc->curusrdata() == "Y") {
            if(!saas_pay_detail::SaaSFuncCheck(this, "0301"/*云、本地会员积分*/)) {
                ui->comboBox_ch_calc->setcomusrdata("N");
            }
        }
    }
}

void w_m_member_list_type_change::mapperupdate()
{
    mapper->setCurrentIndex(_parent->ui->tableView->currentIndex().row());
}

void w_m_member_list_type_change::toup()
{
    if(_parent->tablemodel->model_is_dirty()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"))) {
        case 0:
            if(!tosave()) {
                return;
            }
            break;
        case 1:
            break;
        }
    }

    _parent->ui->tableView->goprevious();
    mapperupdate();
    ui->label_record->setText(QString("%1/%2").arg(_parent->ui->tableView->currentIndex().row()+1).arg(_parent->ui->tableView->verticalHeader()->count()));
}

void w_m_member_list_type_change::todown()
{
    if(_parent->tablemodel->model_is_dirty()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"))) {
        case 0:
            if(!tosave()) {
                return;
            }
            break;
        case 1:
            break;
        }
    }

    _parent->ui->tableView->gonext();
    mapperupdate();
    ui->label_record->setText(QString("%1/%2").arg(_parent->ui->tableView->currentIndex().row()+1).arg(_parent->ui->tableView->verticalHeader()->count()));
}

//===================================
w_m_member_list_type_new::w_m_member_list_type_new(w_m_member_list_type *parent):
    w_m_member_list_type_change(parent)
{
    ui->widget->index_widget(QObject::tr("上一行"))->setVisible(false);
    ui->widget->index_widget(QObject::tr("下一行"))->setVisible(false);
    ui->lineEdit_ch_typeno->setEnabled(true);
    ui->comboBox_ch_attribute->setEnabled(true);
}

bool w_m_member_list_type_new::tosave()
{
    QString ls_memeber_typeno=ui->lineEdit_ch_typeno->text();
    QString ls_member_name=ui->lineEdit_vch_typename->text();
    QString ls_mess;
    QString ls_attribute=ui->comboBox_ch_attribute->currentText();
    bool ok;
    switch(0) {
    case 0:
        if(ls_memeber_typeno.isEmpty()) {
            ls_mess=QObject::tr("类型编号不能为空");
            break;
        }
        if(ls_member_name.isEmpty()) {
            ls_mess=QObject::tr("会员卡类型名称不能为空");
            break;
        }
        if(ls_attribute.isEmpty()) {
            ls_mess=QObject::tr("会员种类不能为空");
            break;
        }
        if(ls_memeber_typeno.length() != 2) {
            ls_mess=QObject::tr("类型编号")+QObject::tr("必须为2位");
            break;

        }
    case 1:
        if(ui->lineEdit_int_backcash_rate->text().toInt() < 0) {
            ls_mess=QObject::tr("充值卡付款还现比率不能小于0");
            break;
        }
        if(ui->lineEdit_int_backcash_rate->text().toInt() >= 100) {
            ls_mess=QObject::tr("充值卡付款还现比率不能大于等于100");
            break;
        }
        if(ui->lineEdit_int_card_baseamount->value() < 0) {
            ls_mess=QObject::tr("充值卡保本金额不能小于0");
            break;
        }

        ui->lineEdit_int_discount->text().toInt(&ok);
        if(!ok  && ui->lineEdit_int_discount->text().isEmpty()) {
            ls_mess=QObject::tr("固定折扣为空");
            break;
        }
        if(!ok  && !ui->lineEdit_int_discount->text().isEmpty()) {
            ls_mess=QObject::tr("固定折扣应当为纯数字");
            break;
        }

        if(ui->lineEdit_int_discount->text().toInt() < 0) {
            ls_mess=QObject::tr("固定折扣不能小于0");
            break;
        }
        if(ui->lineEdit_int_discount->text().toInt() >100) {
            ls_mess=QObject::tr("固定折扣不能大于100");
            break;
        }
    case 2:{
        lds_query_hddpos  query;
        query.execSelect(QString("select count(0) from t_m_member_type where ch_typeno = '%1' ")
                         .arg(ls_memeber_typeno));
        query.next();
        if(query.recordValue(0).toLongLong() > 0) {
            ls_mess=QObject::tr("编号已经存在,请输入新的会员类型编号");
            break;
        }
        //! <--transaction begin-->
        lds_query_hddpos::tran_saction();
        if(query.execInsert("t_m_member_type",qrtVariantPairList()

                            << qrtVariantPair("ch_typeno", ui->lineEdit_ch_typeno->text())
                            << qrtVariantPair("vch_typename", ui->lineEdit_vch_typename->text())
                            << qrtVariantPair("ch_attribute", ui->comboBox_ch_attribute->comusrdata())
                            << qrtVariantPair("int_discount", ui->lineEdit_int_discount->text())
                            << qrtVariantPair("ch_calc", ui->comboBox_ch_calc->comusrdata())

                            << qrtVariantPair("ch_calc_type", ui->comboBox_ch_calc_type->comusrdata())
                            << qrtVariantPair("int_calc_unit", ui->lineEdit_int_calc_unit->value())
                            << qrtVariantPair("ch_pricekind", ui->comboBox_ch_pricekind->comusrdata())
                            << qrtVariantPair("ch_area_flag", ui->comboBox_ch_area_flag->comusrdata())
                            << qrtVariantPair("ch_favourable", ui->comboBox_ch_favourable->comusrdata())

                            << qrtVariantPair("int_card_baseamount", ui->lineEdit_int_card_baseamount->value())
                            << qrtVariantPair("int_backcash_rate", ui->lineEdit_int_backcash_rate->text()),
                            &ls_mess)) {
            //! <--transaction commit-->
            lds_query_hddpos::com_mit();
            _parent->tablemodel->select();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
            this->accept();
            return true;
        }
        lds_query_hddpos::roll_back();
    }
        break;
    }
    //! <--transaction commit-->
    _parent->tablemodel->select();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_mess);
    return false;
}

void w_m_member_list_type_new::toexit()
{
    this->reject();
}

void w_m_member_list_type_new::mapperupdate()
{
    is_recharge(ui->comboBox_ch_attribute->currentIndex());
    is_point(ui->comboBox_ch_calc->currentIndex());
    is_fav(ui->comboBox_ch_favourable->currentIndex());
}

//===================================
w_m_member_list_recharge::w_m_member_list_recharge(QWidget *parent, lds_model_sqltablemodel *tablemodel, lds_tableView *tableview, const QString &parm, bool filter_by_handtel):
    QDialog(parent),
    ui(new Ui_w_m_member_list_recharge_Dialog),
    _isready(false),
    _tablemodel(tablemodel),
    _tableview(tableview),
    _parm(parm)

{
    ui->setupUi(this);
    _filter_by_handtel = filter_by_handtel;
    ui->pushButton_repay->setText(QObject::tr("充值"));
    //! init
    this->setWindowTitle(QObject::tr("会员卡充值"));
    if(_parm == "C") {
        ui->lineEdit_num_realamount->setVisible(false);
        ui->label_num_realamount_t->setVisible(false);
        ui->label_num_deposit_t->setText(QObject::tr("取款金额"));
        ui->label_amount_t->setText(QObject::tr("取后面额"));
        ui->label_operdate_t->setText(QObject::tr("取款时间"));
        ui->pushButton_repay->setText(QObject::tr("取款"));
        this->setWindowTitle(QObject::tr("会员卡取款"));
        ui->comboBox_ch_pay_mode->addItemsByModel(&w_m_member::get_RECHARGE_MODEL_PAYMODE_FETCH());
    } else if(_parm == "D") {
        ui->lineEdit_num_realamount->setVisible(false);
        ui->label_num_realamount_t->setVisible(false);
        ui->label_num_deposit_t->setText(QObject::tr("退卡金额"));
        ui->label_amount_t->setText(QObject::tr("退后面额"));
        ui->label_operdate_t->setText(QObject::tr("退款时间"));
        ui->pushButton_repay->setText(QObject::tr("退卡"));
        this->setWindowTitle(QObject::tr("会员卡退卡"));
        ui->comboBox_ch_pay_mode->addItemsByModel(&w_m_member::get_RECHARGE_MODEL_PAYMODE_FETCH());
    } else {
        ui->comboBox_ch_pay_mode->addItemsByModel(&w_m_member::get_RECHARGE_MODEL_PAYMODE_RECHARGE());

        btnForRechargeScheme();
    }
    connect(ui->comboBox_ch_pay_mode,SIGNAL(currentIndexChanged(int)),this,SLOT(tocheckpaymode(int)));
    connect(ui->pushButton_print, SIGNAL(clicked()),this,SLOT(toremainprint()));
    connect(ui->pushButton_read, SIGNAL(clicked()),this,SLOT(toreadcard()));
    connect(ui->pushButton_repay, SIGNAL(clicked()),this,SLOT(torecharge()));
    connect(ui->lineEdit_num_deposit, SIGNAL(returnPressedDelay()),this,SLOT(torechargeOrRead()));
    connect(ui->lineEdit_num_realamount, SIGNAL(returnPressedDelay()),this,SLOT(torechargeOrRead()));

    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->lineEdit_num_realamount,SIGNAL(valueChanged(double)),ui->lineEdit_num_deposit,SLOT(setValue(double)));
    connect(ui->lineEdit_num_deposit,SIGNAL(valueChanged(double)),this,SLOT(TtolineEdit_amount()));
    connect(ui->checkBox,SIGNAL(toggled(bool)),this,SLOT(tochecksave()));
    connect(ui->checkBox_2,SIGNAL(toggled(bool)),this,SLOT(tochecksave_2()));
    connect(ui->pushButton_50,SIGNAL(clicked()),this,SLOT(toreplacenum()));
    connect(ui->pushButton_100,SIGNAL(clicked()),this,SLOT(toreplacenum()));
    connect(ui->pushButton_200,SIGNAL(clicked()),this,SLOT(toreplacenum()));
    connect(ui->pushButton_500,SIGNAL(clicked()),this,SLOT(toreplacenum()));
    connect(ui->pushButton_1000,SIGNAL(clicked()),this,SLOT(toreplacenum()));
    //! after

    ui->checkBox->setChecked(lds::sysconf->value("w_m_member_list_recharge/isprint").toBool());
    ui->checkBox_2->setChecked(lds::sysconf->value("w_m_member_list_recharge/rechange_open_drawer").toBool());

    ui->lineEdit_vch_memberno->setEnabled(false);
    ui->lineEdit_vch_member->setEnabled(false);
    ui->lineEdit_amount->setEnabled(false);
    ui->lineEdit_remain->setEnabled(false);
    ui->lineEdit_vch_operid->setEnabled(false);
    ui->lineEdit_operdate->setEnabled(false);

    ui->pushButton_exit->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_print->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_read->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_repay->setFocusPolicy(Qt::NoFocus);
}

w_m_member_list_recharge::~w_m_member_list_recharge()
{
    w_m_member_list::yun_member_close();
}

void w_m_member_list_recharge::ue_fetch()
{
    //ue_fetch (none) returns integer
    QString ls_memberno;
    QString ls_deposit_mode;
    QString ls_pay_mode;
    QString ls_memo;
    QString ls_err;
    double ldec_deposit;
    double ldec_realamount;
    double ldec_remain;
    double ldec_amount;
    long ll_row;
    QDateTime ldt;
    QDateTime ldt_old;

    n_func::st_member_type  lst_member;

    ls_deposit_mode = "8";

    if(_tableview)ll_row = _tableview->currentIndex().row();
    if(_tablemodel)_tablemodel->model_data(ll_row, "remain").toDouble();
    ls_memberno=ui->lineEdit_vch_memberno->text();
    ldec_deposit=0-ui->lineEdit_num_deposit->value();
    ldec_realamount = ldec_deposit;
    ls_pay_mode = ui->comboBox_ch_pay_mode->comusrdata().toString();
    ls_memo = ui->lineEdit_vch_memo->text();
    ldec_remain = ui->lineEdit_remain->value();
    ldec_amount = ldec_deposit + ldec_remain;

    if(ls_memberno.trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("请先读卡"));
        return;
    }

    if(ldec_deposit>=0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("取款金额不能为非正数"));
        return;
    }

    lst_member = n_func::f_m_get_limit(this, ls_memberno);
    //2

    if(lst_member.as_m_state != "1") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("取款功能只针对正常状态会员"));
        return;
    }
    if(lst_member.as_m_att != "2") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("取款功能只针对充值类会员"));
        return;
    }
    ldt = n_func::f_get_sysdatetime();
    ldt_old = lst_member.dt_operdate;
    if(ldt_old.isValid() && ldt_old > ldt) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("系统时间小于上次保存时间，请确认系统时间是否正确"));
        return;
    }
    //3
    if(n_func::f_m_get_parameter(this,"m_fetch_base", "0") == "1") {
        if(ldec_amount < 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("取款金额超过可用金额"));
            return;
        }
    }else {
        if((ldec_amount - lst_member.adec_m_limitdata) < 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("取款金额超过保本外可用金额"));
            return;
        }
    }

    if(ls_memberno.trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("卡号不存在，请先读卡或查卡"));
        return;
    }
    if(!_isready) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("当前取款记录已取款,请重新读卡或查卡"));
        return;
    } else {
        if(1==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确定数据正确无误，是否取款"), QObject::tr("确认"), QObject::tr("取消"))) {
            ls_err=QObject::tr("手动退出");
            return;
        }
    }
    //! <--transaction begin-->
    lds_query_hddpos::tran_saction();
    switch(0) {
    case 0:
        if(!(ls_err=n_func::f_m_deposit(this, ls_memberno,ldec_deposit,ldec_realamount,ldec_amount,ls_deposit_mode,ls_pay_mode,QObject::tr("会员取款"), _parm)).isEmpty()) {
            break;
        }
        if(w_m_member_list::is_yun_member) break;///=若是云会员付款,或退卡则到此结束
    case 1:
        if(!(ls_err=n_func::f_m_insert_log(ls_memberno,QObject::tr("取款"),QObject::tr("取款金额") + ":" + QString().sprintf("%0.2f", qAbs(ldec_deposit)), 0)).isEmpty()) {
            break;
        }
    }

    if(ls_err.isEmpty()) {
        ui->lineEdit_amount->setValue(ldec_amount);
        _isready=false;
        lds_query_hddpos::com_mit();
    } else {
        lds_query_hddpos::roll_back();
    }
    //! <--transaction end-->
    if(_tablemodel)_tablemodel->select();
    if(_tableview)_tableview->restore();

    if(ls_err.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("取款成功"));
        rechargeprint();
        card_info_clear();
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误"));
    }
}

void w_m_member_list_recharge::ue_blankcard()
{
    //ue_back_card (none) returns (none)
    QString ls_err;
    QString ls_memberno;
    QString ls_memo;
    uo_rw_member uo_member;
    n_func::st_member_type  lst_member;
    QDateTime ldt;
    QDateTime ldt_old;
    double ldec_remain;
    double ldec_deposit;
    double ldec_amount;
    QString ls_value;
    double ldec_base;
    double ldec_point=0;
    QString ls_deposit_mode="5";

    uo_member.of_init(this, "4");
    ls_memberno=ui->lineEdit_vch_memberno->text();
    ldec_deposit=0-ui->lineEdit_num_deposit->value();
    ls_memo = ui->lineEdit_vch_memo->text();
    ldec_remain = ui->lineEdit_remain->value();
    //    if(_tablemodel)_tablemodel->record(ll_row, "remain").toDouble();
    ldec_amount = ldec_deposit + ldec_remain;

    if(ls_memberno.trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("请先读卡"));
        return;
    }
    if(ldec_deposit>=0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("退款金额不能为非正数"));
        return;
    }
    lst_member = n_func::f_m_get_limit(this, ls_memberno);
    //2
    if(lst_member.as_m_state == "4") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("此会员卡已作废"));
        return;
    }
    if(lst_member.as_m_state == "3") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("此会员卡已挂失"));
        return;
    }
    if(lst_member.as_m_state == "2") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("止付卡不能退卡，只能启用"));
        return;
    }

    if(lst_member.as_m_cardflag != "Y") {
        ls_value = w_m_member::get_vip_card_type(this);
        if(ls_value == "1" || ls_value == "2") {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员卡%1还未发卡").arg(ls_memberno));
            return;
        }
    }
    ldt = n_func::f_get_sysdatetime();
    ldt_old = lst_member.dt_operdate;

    if(ldt_old.isValid() && ldt_old > ldt) {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("系统时间小于上次保存时间，请确认系统时间是否正确"), QObject::tr("继续退卡"), QObject::tr("取消"))) {
            return;
        }
    }
    //3
    if(n_func::f_m_get_parameter(this,"m_fetch_base", "0") == "1") {
        if(ldec_amount < 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("退款金额超过可用金额"));
            return;
        }
        if(ldec_amount>0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("卡余额(包含保本金额,保本金额可取)不为零,可进行取款操作，待余额为零再退卡"));
            return;
        }
    }else {
        if((ldec_amount - lst_member.adec_m_limitdata) < 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("退款金额超过保本外可用金额"));
            return;
        }
        if((ldec_amount - lst_member.adec_m_limitdata) > 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("卡余额(不包含保本金额,保本金额不可取)不为零,可进行取款操作，待余额为零再退卡"));
            return;
        }
        ldec_base = lst_member.adec_m_limitdata;
    }



    lds_query_hddpos  query;
    //! <--transaction begin-->
    lds_query_hddpos::tran_saction();
    switch(0) {
    case 0:

        query.execSelect(QString("select num_point from t_m_curamount  where vch_memberno = '%1' ")
                         .arg(ls_memberno));
        query.next();
        ldec_point=query.recordValue("num_point").toDouble();

        if(ldec_point > 0) {
            if(1==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("卡积分不为零,是要执相应的积分操作,还是系统自动冲减?点YSE按系统自动冲减,点NO自己执行相关操作"), QObject::tr("确认"), QObject::tr("取消"))) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("自己执行积分操作,取消退卡"));
                return;
            }
        }

        if(!lst_card.flag) {
            ls_err=lst_card.errtext;
            break;
        }

        if(lst_card.amount>0) {
            ls_err=n_func::f_m_deposit(this, ls_memberno,ldec_deposit,ldec_deposit,0,ls_deposit_mode,"6",QObject::tr("退卡还现"), _parm);
            if(!ls_err.isEmpty()) {
                ls_err=QObject::tr("读取会员帐务数据失败");
                break;
            }
        }
        if(w_m_member_list::is_yun_member) break;///=若是云会员付款,或退卡则到此结束
    case 1:
        if(!query.execUpdate("t_m_member",qrtVariantPairList()

                             << qrtVariantPair("ch_cardflag", "N")
                             << qrtVariantPair("ch_state", "1")
                             << qrtVariantPair("int_basedata", 0),
                             QString(" vch_memberno = '%1' ").arg(ls_memberno))) {
            ls_err=QObject::tr("操作失败");
            break;
        }

        if(!query.execUpdate("t_m_curamount",qrtVariantPairList()
                             << qrtVariantPair("num_amount", 0)
                             << qrtVariantPair("num_point", 0),
                             QString(" vch_memberno = '%1' ").arg(ls_memberno))) {
            ls_err=QObject::tr("会员余额恢复");
            break;
        }

        ls_err=n_func::f_m_insert_log(lst_card.cardid, QObject::tr("退卡"),QObject::tr("金额")  +QString().sprintf("%0.2f",qAbs(ldec_deposit)),ldec_base);
        if(!ls_err.isEmpty()) {
            ls_err=QObject::tr("读取会员帐务数据失败");
            break;
        }

        //            IF  NOT (uo_member.of_clear_card(f_m_get_parameter(this,"vip_ic_password",""))) THEN //58
        ls_err.clear();
        if(uo_member.is_cardtype == "1") {
            QString icpasswd=n_func::f_m_get_parameter(this,"vip_ic_password");
            QByteArray transferpasswd = n_func::f_m_get_cardpwdtransfer(icpasswd);
#ifdef Q_OS_WIN
            ReadCard_mwrf32 reader;reader.setDefPwd(n_func::f_m_defcardpwd);//需要设置一次默认密码，应为changepwd会不完全清卡
#else
            Reader_RF reader;
#endif
            if(reader.setDefPwd(transferpasswd)) {
                if(reader.writecard("","","")) {
                    if(reader.pwdChange(transferpasswd, n_func::f_m_defcardpwd)) {
                        //操作成功
                        break;
                    }
                }
            }
            ls_err=reader.lastError();
        }
    }
    if(ls_err.isEmpty()) {
        lds_query_hddpos::com_mit();
        ls_err=QObject::tr("退卡成功");
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_err);
        rechargeprint();
        card_info_clear();
    } else {
        lds_query_hddpos::roll_back();
        ls_err=QObject::tr("操作失败");
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_err);
    }
    //! <--transaction end-->
}

void w_m_member_list_recharge::rechargeprint()
{
    //打印内容
    //title 会员卡余额
    // 会员卡号
    //会员姓名
    //会员类型
    //会员余额
    //操作员
    //操作日期

    if(ui->checkBox_2->isChecked()) {//钱箱
        public_printby_ch_billno::openDrawer();
    }

    if(ui->checkBox->isChecked()) {
        if(ui->lineEdit_vch_memberno->text().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员编号为空，请先读卡"));
        } else {

            n_func::READCARDINFO cardinfo;
            {
                lds_messagebox_loading_show loading(this, "", "LOADING");
                loading.show_delay();
                if(!n_func::qtcardinfo_readcard(cardinfo, ui->lineEdit_vch_memberno->text() )) {
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, cardinfo.errstring);
                    return;
                }
            }

            lds_query_hddpos  query;
            blockIns printdata;
            printdata += backheader::printer.drawText("i,a00,s11", "");
            query.execSelect("select vch_company from cey_sys_company limit 1;");
            if(query.next()) {
                printdata += backheader::printer.drawText("", query.recordValue("vch_company").toString()+"\n");
            }
            printdata += backheader::printer.drawText("", this->windowTitle() + (w_m_member_list::is_yun_member? ("(" +QObject::tr("云会员") + ")") : "")+  "\n");//清除缓存 居中 2x2x
            printdata += backheader::printer.drawText("i", "");//清除缓存

            printdata += backheader::printer.drawText("", ui->label_vch_memberno_t->text()+":"+cardinfo.vch_memberno + "\n");//
            printdata += backheader::printer.drawText("", ui->label_vch_member_t->text()+":"+cardinfo.vch_member+ "\n");//
            printdata += backheader::printer.drawText("", QObject::tr("会员类型") + ":" +cardinfo.vch_typename + "\n");//
            printdata += backheader::printer.drawText("", ui->label_num_realamount_t->text()+":"+lds::septNumber(ui->lineEdit_num_realamount->value())+ "\n");//
            printdata += backheader::printer.drawText("", ui->label_num_deposit_t->text()+":"+ lds::septNumber(ui->lineEdit_num_deposit->value())+ "\n");//
            printdata += backheader::printer.drawText("", ui->label_remain_t->text() + ":" + lds::septNumber(cardinfo.num_amount)+ "\n");//
            printdata += backheader::printer.drawText("", ui->label_ch_pay_mode_t->text()+":"+ui->comboBox_ch_pay_mode->currentText()+ "\n");//
            printdata += backheader::printer.drawText("", QObject::tr("操作员") + ":" +lds::gs_opername + "\n");//
            printdata += backheader::printer.drawText("", QObject::tr("操作日期") + ":" +n_func::f_get_sysdatetime().toString("yyyy-MM-dd hh:mm:ss") + "\n");

            printdata += backheader::printer.drawTextEnd("1B6405 1D5601", "");//走纸5x， 切纸
            printdata =   backheader::printer.setPrintCount(printdata, 1);

            backheader::reslove_write(printdata);
        }
    }
}

void w_m_member_list_recharge::toremainprint()
{
    //打印内容
    //title 会员卡余额
    // 会员卡号
    //会员姓名
    //会员类型
    //会员余额
    //操作员
    //操作日期


    if(ui->lineEdit_vch_memberno->text().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员编号为空，请先读卡"));
    } else {

        n_func::READCARDINFO cardinfo;
        {
            lds_messagebox_loading_show loading(this, "", "LOADING");
            loading.show_delay();
            if(!n_func::qtcardinfo_readcard(cardinfo, ui->lineEdit_vch_memberno->text() )) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, cardinfo.errstring);
                return;
            }
        }

        lds_query_hddpos  query;
        blockIns printdata;
        printdata += backheader::printer.drawText("i,a00,s11", "");
        query.execSelect("select vch_company from cey_sys_company limit 1;");
        if(query.next()) {
            printdata += backheader::printer.drawText("", query.recordValue("vch_company").toString()+"\n");//清除缓存 居中 2x2x
        }
        printdata += backheader::printer.drawText("", this->windowTitle() + (w_m_member_list::is_yun_member? ("(" +QObject::tr("云会员") + ")") : "")+  "\n");//清除缓存 居中 2x2x
        printdata += backheader::printer.drawText("i", "");//清除缓存

        printdata += backheader::printer.drawText("", ui->label_vch_memberno_t->text()+":"+cardinfo.vch_memberno + "\n");//
        printdata += backheader::printer.drawText("", ui->label_vch_member_t->text()+":"+cardinfo.vch_member+ "\n");//
        printdata += backheader::printer.drawText("", QObject::tr("会员类型") + ":" +cardinfo.vch_typename + "\n");//
        printdata += backheader::printer.drawText("", ui->label_remain_t->text() + ":" + lds::septNumber(cardinfo.num_amount)+ "\n");//
        printdata += backheader::printer.drawText("", QObject::tr("操作员") + ":" +lds::gs_opername + "\n");//
        printdata += backheader::printer.drawText("", QObject::tr("操作日期") + ":" +n_func::f_get_sysdatetime().toString("yyyy-MM-dd hh:mm:ss") + "\n");

        printdata += backheader::printer.drawTextEnd("1B6405 1D5601", "");//走纸5x， 切纸
        printdata =   backheader::printer.setPrintCount(printdata, 1);

        backheader::reslove_write(printdata);
    }
}

void w_m_member_list_recharge::toreadcard()
{
    n_func::st_member_type lst_member;
    QString cardno;
    QString as_memberno;
    uo_rw_member uo_member;
    QString ls_value;
    _isready=false;
    bool isyun=_parm.isEmpty()//充值
            ||_parm == "D"//退卡
            ||_parm == "C"//取款
            ;
    card_info_clear();
    //读卡
    if(!w_m_member_list::static_to_memeber_readcard(this, cardno, uo_member, _filter_by_handtel, isyun)) {
        goto read_card_fail;
    }
    as_memberno=cardno;
    if(as_memberno.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("对应会员号为空"));
        goto read_card_fail ;
    }

    lst_card=uo_member.of_read_card(this, as_memberno);
    if(lst_card.flag == false) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,lst_card.errtext);
        goto read_card_fail;
    }
    if(lst_card.flag) {
        lst_member=n_func::f_m_get_limit(this, lst_card.cardid);
        if(lst_member.as_m_state.isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("非本公司卡或无效卡")+lst_card.cardid);
            goto read_card_fail;
        } else if(lst_member.as_m_state!="1") {
            if(this->_parm!="D") {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("此卡状态非正常"));
                goto read_card_fail;
            }
        }

        if(lst_member.adt_m_limitdate <= n_func::f_get_sysdatetime()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("此卡已经超过有效期"));
            goto read_card_fail;
        }

        if(this->_parm!="D") {
            if(lst_member.as_m_att!="2") {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("非充值卡"));
                goto read_card_fail;
            }
        }

        if(lst_member.as_m_cardflag !="Y") {
            ls_value = w_m_member::get_vip_card_type(this);
            if(ls_value == "1" || ls_value == "2") {
                if(this->_parm == "C") {
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员卡%1还未发卡").arg(as_memberno));
                    goto read_card_fail;
                } else {
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员卡%1还未发卡").arg(cardno));
                    goto read_card_fail;
                }
            }
        }
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,lst_card.errtext);
        goto read_card_fail;
    }

read_card_success:
    //==
    ui->lineEdit_vch_memberno->setText(lst_card.cardid);
    ui->lineEdit_vch_member->setText(lst_member.as_member);
    ui->lineEdit_num_realamount->setValue(0);
    ui->lineEdit_num_deposit->setValue(0);
    ui->lineEdit_remain->setValue(lst_card.amount);
    ui->lineEdit_amount->setValue(0);
    ui->lineEdit_vch_operid->setText(lds::gs_operid);
    ui->lineEdit_operdate->setText(n_func::f_get_sysdatetime().toString("yyyy-MM-dd hh:mm"));
    _isready=true;
    if(_isready)TtolineEdit_amount();

    if(this->_parm == "C") {//取款
        ui->lineEdit_num_deposit->setFocus();
        ui->lineEdit_num_deposit->setValue(ui->lineEdit_remain->value());
    } else   if(this->_parm == "D") {//退卡
        ui->lineEdit_num_deposit->setFocus();
    } else {
        ui->lineEdit_num_realamount->setFocus();
    }
    return;
read_card_fail:
    w_m_member_list::yun_member_close();
    return;
}

void w_m_member_list_recharge::torecharge()
{
    if(this->_parm == "C") {//取款
        ue_fetch();
        return;
    }
    if(this->_parm == "D") {//退卡
        ue_blankcard();
        return;
    }
    QString ls_memberno=ui->lineEdit_vch_memberno->text();
    double ldec_deposit=ui->lineEdit_num_deposit->value();
    double ldec_realamount=ui->lineEdit_num_realamount->value();
    double ldec_remaint=ui->lineEdit_remain->value();
    double ldec_amount=ldec_deposit+ldec_remaint;
    QString ls_error;
    QString ls_deposit_mode="1";
    QString ls_pay_mode=ui->comboBox_ch_pay_mode->curusrdata().toString();//w_m_member::get_RECHARGE_MODEL_PAYMODE().item(ui->comboBox_ch_pay_mode->currentIndex(), 0)->text();
    QString ls_memo=QObject::tr("会员充值");
    //100，000
    double ldec_max_deposit=n_func::f_m_get_parameter(this,"m_max_deposit", "999999").remove(",").toDouble();

    switch(0) {
    case 0:
        if(ls_memberno.trimmed().isEmpty()) {
            ls_error=(QObject::tr("请先读卡"));
            break;
        }
        if(ldec_deposit < 0 || ldec_realamount < 0) {
            ls_error=(QObject::tr("充值金额与实收金额不能为负数"));
            break;
        }
        if(ldec_deposit>ldec_max_deposit) {
            ls_error=(QObject::tr("充值金额不能大于最大充值金额，你可已在会员参数设置中更改该值"));
            break;
        }
        if(n_func::f_m_get_parameter(this,"m_deposit_negative", "0") == "1" && ldec_deposit != ldec_realamount) {
            ls_error=(QObject::tr("充值金额与实收金额不等,你可以在会员参数设置中修改此项"));
            break;
        }

        if (ldec_deposit < 0 && ldec_realamount != ldec_deposit) {
            ls_error=(QObject::tr("负数充值时，充值金额与实收金额必须相等"));
            break;
        }
        if(ldec_deposit == 0 && ldec_realamount == 0) {
            ls_error=QObject::tr("充值金额与实收金额不能全部为0");
            break;
        }
        if((ldec_deposit < 0 && ldec_realamount >= 0) || (ldec_deposit >= 0 && ldec_realamount < 0)) {
            ls_error=(QObject::tr("充值金额与实收金额正负不一致"));
            break;
        }

        if(!_isready) {
            ls_error=(QObject::tr("当前充值记录已充值,请重新读卡或查卡"));
            break;
        } else {
            if(1==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确定数据正确无误，是否充值"), QObject::tr("确认"), QObject::tr("取消"))) {
                ls_error=QObject::tr("手动退出");
                break;
            }
        }
    case 1:
        //! <--transaction begin-->
        /*选中支付宝或者微信充值的事物*/
    {
        if("6" == ls_pay_mode) {//微信
            if(false == w_scr_dish_phone_pay::isPayRecord("05")) {//非记账
                w_scr_dish_phone_pay dialog("05", ui->lineEdit_num_realamount->value(), this);
                if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
                    ls_memo = dialog.ret_pay_name + ":" + dialog.ret_out_trade_no;
                } else {
                    ls_error = ui->comboBox_ch_pay_mode->currentText() + QObject::tr("支付取消");
                    break;
                }
            }
        }
        if("7" == ls_pay_mode) {//支付宝
            if(false == w_scr_dish_phone_pay::isPayRecord("06")) {//非记账
                w_scr_dish_phone_pay dialog("06", ui->lineEdit_num_realamount->value(), this);
                if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
                    ls_memo = dialog.ret_pay_name + ":" + dialog.ret_out_trade_no;
                } else {
                    ls_error = ui->comboBox_ch_pay_mode->currentText() + QObject::tr("支付取消");
                    break;
                }
            }
        }
        if("8" == ls_pay_mode) {//摇钱树支付
            if(false == w_scr_dish_phone_pay::isPayRecord("AA")) {//非记账
                w_scr_dish_phone_pay dialog("AA", ui->lineEdit_num_realamount->value(), this);
                if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
                    ls_memo = QObject::tr("摇钱树支付") + "_" + dialog.ret_pay_name + ":" + dialog.ret_out_trade_no;
                } else {
                    ls_error = ui->comboBox_ch_pay_mode->currentText() + QObject::tr("支付取消");
                    break;
                }
            }
        }
        if("12" == ls_pay_mode) {//联拓富
            w_scr_dish_phone_pay dialog("EE", ui->lineEdit_num_realamount->value(), this);
            if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
                ls_memo = QObject::tr("联拓富") + "_" + dialog.ret_pay_name + ":" + dialog.ret_out_trade_no;
            } else {
                ls_error = ui->comboBox_ch_pay_mode->currentText() + QObject::tr("支付取消");
                break;
            }
        }
    }

        lds_query_hddpos::tran_saction();
        if((ls_error=n_func::f_m_deposit(this, ls_memberno,ldec_deposit/*充值金额*/,
                                         ldec_realamount/*实际充值金额*/,
                                         ldec_amount/*充值金额后剩余金额*/,ls_deposit_mode,ls_pay_mode,ls_memo,_parm)).isEmpty()) {
        } else {
            lds_query_hddpos::roll_back();
            break;
        }

        if(!(ls_error=n_func::f_m_insert_log(ls_memberno,QObject::tr("充值"),QObject::tr("充值金额") + QString::number(ldec_deposit),0)).isEmpty()) {
            lds_query_hddpos::roll_back();
            break;
        }

        lds_query_hddpos::com_mit();
        if(_tablemodel)_tablemodel->select();
        if(_tableview)_tableview->restore();
        //! <--transaction end-->
        break;
    }

    _isready=false;
    if(ls_error.isEmpty()) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("充值成功"));
        rechargeprint();
        card_info_clear();
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_error);
    }

}

void w_m_member_list_recharge::torechargeOrRead()
{
    QString ls_memberno=ui->lineEdit_vch_memberno->text();
    if(ls_memberno.trimmed().length() == 0) {
        toreadcard();
    } else {
        torecharge();
    }
}

void w_m_member_list_recharge::toexit()
{
    //yun
    w_sys_manage_cloudsync_with_time_running::exec_no_up_then_down_byconf(this);
    ui->pushButton_exit->show_flag_top_right(w_sys_manage_cloudsync_with_time_running::q_bt_get_ready_total_byconf());
    //~yun
    w_sys_manage_cloudsync_cloud2::exec_keepdata_byconf(this);

    this->reject();
}

void w_m_member_list_recharge::TtolineEdit_amount()
{
    if(_parm == "C") {//取款
        ui->lineEdit_amount->setValue(ui->lineEdit_remain->value() - ui->lineEdit_num_deposit->value());
    } else if(_parm == "D") {//退卡
        ui->lineEdit_amount->setValue(ui->lineEdit_remain->value() - ui->lineEdit_num_deposit->value());
    } else {//充值
        if(n_func::f_m_get_parameter(this,"m_deposit_negative", "0") == "1") {
            ui->lineEdit_num_deposit->setValue(ui->lineEdit_num_realamount->value());
        }
        ui->lineEdit_amount->setValue(ui->lineEdit_num_deposit->value() + ui->lineEdit_remain->value());
    }
    //    if(_parm!="C") {
    //        if(n_func::f_m_get_parameter(this,"m_deposit_negative", "0") == "1") {
    //            ui->lineEdit_num_deposit->setText(ui->lineEdit_num_realamount->text());
    //        }
    //        ui->lineEdit_amount->setText(QString().sprintf("%0.2f", (ui->lineEdit_num_deposit->text().toFloat() + ui->lineEdit_remain->text().toFloat())));

    //    } else {
    //        ui->lineEdit_amount->setText(QString().sprintf("%0.2f", (ui->lineEdit_remain->text().toFloat() - ui->lineEdit_num_deposit->text().toFloat())));

    //    }
}

void w_m_member_list_recharge::tochecksave()
{
    //更新配置文件

    lds::sysconf->setValue("w_m_member_list_recharge/isprint", ui->checkBox->isChecked());
}

void w_m_member_list_recharge::tochecksave_2()
{
    lds::sysconf->setValue("w_m_member_list_recharge/rechange_open_drawer", ui->checkBox->isChecked());

}

void w_m_member_list_recharge::tocheckpaymode(int index)
{
    if(index == 3 || index == 4) {
        if(_parm == "C" || _parm == "D") {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("目前不支持%1支付方式").arg(ui->comboBox_ch_pay_mode->currentText()));
            ui->comboBox_ch_pay_mode->setCurrentIndex(0);
        }
    }
}

void w_m_member_list_recharge::toreplacenum()
{
    lds_pushbutton *p = qobject_cast<lds_pushbutton *>(this->sender());
    if(p) {
        if(p->getData("recharge_schm_real").isValid()) {
            QDateTime dead = p->getData("recharge_schm_dt").toDateTime();
            QDateTime cur = n_func::f_get_sysdatetime();
            if(cur > dead) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID,
                                        QObject::tr("当前日期") + ":" + cur.toString(yyyyMMddhhmmss) + "\n" +
                                        QObject::tr("截止日期") + ":" + dead.toString(yyyyMMddhhmmss) + "\n" +
                                        QObject::tr("剩余") + ":" + QObject::tr("不足"));
                return;
            }
            ui->lineEdit_num_realamount->setEnabled(false);
            ui->lineEdit_num_deposit->setEnabled(false);

            ui->lineEdit_num_realamount->setValue(p->getData("recharge_schm_real").toDouble());
            ui->lineEdit_num_deposit->setValue(p->getData("recharge_schm_amt").toDouble());

        } else {
            ui->lineEdit_num_realamount->setEnabled(true);
            ui->lineEdit_num_deposit->setEnabled(true);

            QDoubleSpinBox *l = qobject_cast<QDoubleSpinBox*>(this->focusWidget());
            if(l == 0) {
                l = ui->lineEdit_num_realamount;
                ui->lineEdit_num_realamount->setFocus();
            }
            if(l) {
                l->setValue(p->text().toDouble());
                l->selectAll();
            }
        }
    }
}

void w_m_member_list_recharge::toUpdateRechargeScheme()
{
    w_m_member_list_recharge_scheme dialog(this);
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        btnForRechargeScheme();
    }
}

void w_m_member_list_recharge::btnForRechargeScheme()
{
    if(false == ui->pushButton_500->getData("recharge_schm_real").isValid()) {
        ui->pushButton_500->enableLongPress(true);
        ui->pushButton_500->setIcon(QPixmap(":/image/clock.png"));
        ui->pushButton_500->setText(tr("方案") + "1");
        connect(ui->pushButton_500, SIGNAL(timeout()), this, SLOT(toUpdateRechargeScheme()));

        ui->pushButton_1000->enableLongPress(true);
        ui->pushButton_1000->setIcon(QPixmap(":/image/clock.png"));
        ui->pushButton_1000->setText(tr("方案") + "2");
        connect(ui->pushButton_1000, SIGNAL(timeout()), this, SLOT(toUpdateRechargeScheme()));
    }
    ui->pushButton_500->setData("recharge_schm_real", n_func::f_get_sysparm("recharge_schm_real_500"));
    ui->pushButton_500->setData("recharge_schm_amt", n_func::f_get_sysparm("recharge_schm_amt_500"));
    ui->pushButton_500->setData("recharge_schm_dt", n_func::f_get_sysparm("recharge_schm_dt_500"));

    ui->pushButton_1000->setData("recharge_schm_real", n_func::f_get_sysparm("recharge_schm_real_1000"));
    ui->pushButton_1000->setData("recharge_schm_amt", n_func::f_get_sysparm("recharge_schm_amt_1000"));
    ui->pushButton_1000->setData("recharge_schm_dt", n_func::f_get_sysparm("recharge_schm_dt_1000"));
}

void w_m_member_list_recharge::card_info_clear()
{
    ui->lineEdit_vch_memberno->clear();
    ui->lineEdit_vch_member->clear();
    ui->lineEdit_remain->setValue(0);
    ui->lineEdit_amount->setValue(0);
    ui->lineEdit_num_deposit->setValue(0);
    ui->lineEdit_num_realamount->setValue(0);
}

//==========================
w_m_member_pwdchange::w_m_member_pwdchange(const QString &vch_memberno, QWidget *parent):
    QDialog(parent),
    ui(new Ui_w_m_member_pwdchange_Dialog),
    is_memberno(vch_memberno)
{
    ui->setupUi(this);

    ui->label_4->hide();
    ui->lineEdit_cur_operator->hide();
    ui->label_5->hide();
    ui->lineEdit_login_operator->hide();
    ui->lineEdit_oldpwd->setEchoMode(QLineEdit::Password);
    ui->lineEdit_newpwd->setEchoMode(QLineEdit::Password);
    ui->lineEdit_newpwd2->setEchoMode(QLineEdit::Password);
    //
    this->setWindowTitle(QObject::tr("修改密码"));
    is_password = lds_query_hddpos::selectValue(QString("select ifnull(vch_password, '') from t_m_member where vch_memberno='%1'").arg(is_memberno)).toString();
    if(is_password.isEmpty()) {
        lds::hideWidget(ui->label_oldpwd);
        lds::hideWidget(ui->lineEdit_oldpwd);
    }
    //! toolbar
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(tocancel()));
}

void w_m_member_pwdchange::enableForget()
{
    is_password = "";
    lds::hideWidget(ui->label_oldpwd);
    lds::hideWidget(ui->lineEdit_oldpwd);
}

void w_m_member_pwdchange::took()
{
    QString ls_password_old;
    QString ls_password;
    QString ls_password1;
    QString ls_err;

    ls_password_old = ui->lineEdit_oldpwd->text();
    ls_password = ui->lineEdit_newpwd->text();
    ls_password1 = ui->lineEdit_newpwd2->text();
    if(is_password.isEmpty()) {

    } else {
        if(ls_password_old == is_password || ls_password_old == n_func::f_get_godpassword()) {

        } else {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("旧密码录入错误"));
            return;
        }
    }
    if(ls_password==ls_password1) {

    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("新密码与确认密码不一致"));
        return;
    }

    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("修改密码"),QObject::tr("确认"),QObject::tr("取消"))) {
        //! <--transaction begin-->
        lds_query_hddpos::tran_saction();
        lds_query_hddpos  query;

        if(query.execUpdate("t_m_member",qrtVariantPairList()

                            << qrtVariantPair("vch_password", ls_password),
                            QString(" vch_memberno = '%1' ").arg(is_memberno))) {
            lds_query_hddpos::com_mit();
            ls_err=QObject::tr("修改密码成功，请记住新密码");
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_err);
            this->accept();
        } else {
            lds_query_hddpos::roll_back();
            ls_err = QObject::tr("修改密码失败");
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_err);
        }
        //! <--transaction end-->
    }
}

void w_m_member_pwdchange::tocancel()
{
    this->reject();
}
//=======================
w_m_member_news::w_m_member_news(w_m_member_list *parent):
    QDialog(parent)
{

    _parent = parent;
    ui = new Ui_w_m_member_news_Dialog;
    ui->setupUi(this);

    //! init
    ui->comboBox_ch_typeno->addItemsBySql("select ch_typeno, vch_typename from t_m_member_type");
    ui->dateTimeEdit_dt_limit->setDateTime(n_func::f_get_sysdatetime().addYears(1));
    ui->dateTimeEdit_dt_limit->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    //! toolbar
    ui->widget->setup(QStringList() << QObject::tr("生成") << QObject::tr("退出"), Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("生成")), SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_type, SIGNAL(clicked()), this, SLOT(tonewtype()));

}

void w_m_member_news::took()
{
    //clicked (none) returns long [pbm_bnclicked]
    QString ls_memberno;
    QString ls_membername;
    QString ls_start;
    QString ls_begin;
    QString ls_end;
    QString ls_type;
    QString ls_cardno;
    qlonglong ll_begin;
    qlonglong ll_end;
    qlonglong ll_limit;
    double ldec_limit = 0;
    QDateTime ldt_limit;
    QDateTime ldt_today;
    QRegExp ls_filter;

    //    PARENT.dw_1.accepttext()
    ls_start = ui->lineEdit_code_start->text();//trim(PARENT.dw_1.object.code_start[1])
    ll_begin = ui->lineEdit_code_begin->text().toLongLong();//PARENT.dw_1.object.code_begin[1]
    ls_begin = ui->lineEdit_code_begin->text();//string(ll_begin)
    ll_end = ui->lineEdit_code_end->text().toLongLong();//PARENT.dw_1.object.code_end[1]
    ls_end = ui->lineEdit_code_end->text();//string(ll_end)
    ll_limit = ui->lineEdit_code_limit->text().toLongLong();//PARENT.dw_1.object.code_limit[1]
    ls_type = ui->comboBox_ch_typeno->curusrdata().toString();
    ldt_limit = ui->dateTimeEdit_dt_limit->dateTime();//PARENT.dw_1.object.dt_limit[1]
    ls_filter = QRegExp("["+ui->lineEdit_vch_filter->text().replace(",","")+"]");

    if(ll_limit==0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("编号位数不能为零"));
        return;
    }
    if(ldt_limit < n_func::f_get_sysdatetime()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员有效期限应该大于当前日期"));
        return;
    }
    if(ldt_limit.isNull()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("有效期限不能为空"));
        return;
    }
    if(ui->comboBox_ch_typeno->currentText().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员类型不能为空"));
        return;
    }
    if(ll_limit==0 || (ll_limit+ls_start.length()) > 20) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("编号位数不能为空且不能大于%1位")
                                .arg(QString::number(20-ls_start.length()))
                                );
        return;
    }
    if(ll_end-ll_begin >= 10000) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("一次批量创建的会员不超过1万条"));
        return;
    }
    if(ll_limit < ls_end.trimmed().length()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("编号部分位数不能小于编号到(结束)实际位数"));
        return;
    }
    if(ll_begin > ll_end) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("编号从(起始)不能大于编号到(结束)"));
        return;
    }
    if(ls_end.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("编号到(结束)不能为空"));
        return;
    }
    if(ls_begin.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("编号从(起始)不能为空"));
        return;
    }

    if(ls_start.isEmpty()) {
        if(1==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("编号前缀为空，(前缀可以固定生成在每个会员编号前面)，是否继续"), QObject::tr("继续"),QObject::tr("取消"))) {
            return;
        }
    }

    //! <--transaction begin-->
    lds_query_hddpos::tran_saction();
    QString errtext;
    lds_query_hddpos  query;
    for(qlonglong ll_i=ll_begin; ll_i <= ll_end; ll_i++) {
        ls_memberno = ls_start +QString::number(ll_i).rightJustified(ll_limit, '0').right(ll_limit);// fill("0",ll_limit - len(string(i))) + string(i)
        ls_membername = "批量" + QString::number(ll_i);
        ls_cardno=ls_memberno;
        if(ls_memberno.contains(ls_filter)) {
            continue;
        }
        query.execSelect(QString("Select count(0)    from t_m_member   where vch_memberno ='%1' ")
                         .arg(ls_memberno));
        query.next();
        if(query.recordValue(0).toLongLong() > 0) {
            errtext=QObject::tr("批量范围中的会员编号系统已经存在，例如:%1，本次操作无效")
                    .arg(ls_memberno);
            break;
        }
        ldt_today=n_func::f_get_sysdatetime();

        if(query.execInsert("t_m_member",qrtVariantPairList()

                            << qrtVariantPair("vch_memberno", ls_memberno)
                            << qrtVariantPair("vch_cardno", ls_cardno)
                            << qrtVariantPair("vch_member", ls_membername)
                            << qrtVariantPair("ch_typeno", ls_type)
                            << qrtVariantPair("num_limit", ldec_limit)

                            << qrtVariantPair("dt_limit", ldt_limit)
                            << qrtVariantPair("vch_operID", lds::gs_operid)
                            << qrtVariantPair("dt_operdate", ldt_today),
                            &errtext)) {
        } else {
            break;
        }
    }

    if(errtext.isEmpty()) {
        lds_query_hddpos::com_mit();
        _parent->torefresh();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("批量生成会员完成"));
    } else {
        lds_query_hddpos::roll_back();
        _parent->torefresh();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("批量生成会员失败")+"\n"+errtext);
    }
    //! <--transaction end-->
}

void w_m_member_news::tocancel()
{
    this->reject();
}

void w_m_member_news::tonewtype()
{
    w_m_member_list_type dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();

    QVariant typeno = ui->comboBox_ch_typeno->curusrdata();
    ui->comboBox_ch_typeno->clear();
    ui->comboBox_ch_typeno->addItemsBySql("select ch_typeno, vch_typename from t_m_member_type");
    ui->comboBox_ch_typeno->setcomusrdata(typeno);
}


w_m_member_list_yun_auto_close::w_m_member_list_yun_auto_close()
{

}

void w_m_member_list_yun_auto_close::yunOpen()
{
    w_m_member_list::yun_member_open();
}

w_m_member_list_yun_auto_close::~w_m_member_list_yun_auto_close()
{
    w_m_member_list::yun_member_close();
}
