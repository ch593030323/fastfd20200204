#include "w_sys_manage_cloudsync_with_time.h"
#include <QtDebug>
#include <QNetworkRequest>
#include <QByteArray>
#include <QNetworkReply>
#include "lds_query_hddpos.h"
#include <QSqlError>
#include <QDate>
#include "lds_messagebox.h"
#include "public_sql.h"
#include "lds.h"
#include "w_sys_manage_cloudsync.h"
#include "ui_w_sys_manage_cloudsync_with_time.h"
#include "qtableview.h"
#include "ui_w_sys_manage_cloudsync_with_time_running.h"
#include <QTimer>
#include "public_logindialog.h"
#include "n_func.h"
#include <QMovie>
#include "saas_guide_activateterminal.h"
#include "saas_guide2.h"
#include "saas_guide.h"
#include "w_sys_manage.h"
#include "w_sys_manage_cloudsync_transfer.h"
#include "lds_query_thread.h"

QString w_sys_manage_cloudsync_with_time::bt_trigger_update_sql_arg("CREATE TRIGGER tr_web_%1_update BEFORE UPDATE ON %1 FOR EACH ROW  Begin   IF(%2 and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END ");
QString w_sys_manage_cloudsync_with_time::bt_trigger_delete_sql_arg("CREATE TRIGGER tr_web_%1_delete BEFORE DELETE ON %1 FOR EACH ROW  Begin  replace into del_%1 select * from %1 where %2; update del_%1 set ch_yun_already_keep = 'N' where %2; END");//OLD.ch_dishno
QString w_sys_manage_cloudsync_with_time::bt_trigger_insert_sql_arg("CREATE TRIGGER tr_web_%1_insert AFTER INSERT ON %1 FOR EACH ROW  Begin  delete from del_%1 where %2; END");//OLD.ch_dishno

static QString orgCode;
static QString dateFormatStr="yyyy-MM-dd hh:mm:ss";
static int keep_going = 0;
static int total_going;

w_sys_manage_cloudsync_with_time::w_sys_manage_cloudsync_with_time(QWidget *parent)
    :    QDialog(parent),  ui( new Ui_w_sys_manage_cloudsync_with_time)
{
    ui->setupUi(this);

    ui->comboBox_ip->addItem("115.28.19.85");
    ui->comboBox_ip->addItem("www.yqspos.com");
#ifdef QT_DEBUG
    ui->comboBox_ip->addItem("192.168.1.81");
#endif
    ui->lineEdit_user->setText(n_func::f_get_sysparm("cloud_user"));
    ui->label_orgCode->setText(n_func::f_get_sysparm("orgCode"));
    ui->lineEdit_password->setText(n_func::f_get_sysparm("cloud_password"));
    ui->comboBox_ip->setEditText(n_func::f_get_sysparm("cloud_ip"));

    ui->checkBox_isyunrun->setChecked(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_is_run", false).toBool());
    ui->checkBox_yun_member->setChecked(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_member", false).toBool());
    ui->checkBox_yun_member_2->setChecked(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_no_use_local_member", false).toBool());

    connect(ui->pushButton_upload, SIGNAL(clicked()),this,SLOT(toupload()));
    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_user_check, SIGNAL(clicked()),this,SLOT(tousercheck()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_data_keep, SIGNAL(clicked()),this,SLOT(todatekeep()));
    connect(ui->pushButton_download, SIGNAL(clicked()),this,SLOT(todownload()));
    connect(ui->pushButton_timecheck, SIGNAL(clicked()),this,SLOT(totimecheck()));
    connect(ui->pushButton_flow_upload, SIGNAL(clicked()),this,SLOT(toflowupload()));
    connect(ui->lineEdit_saas, SIGNAL(textChanged(QString)), this, SLOT(toshowSaaSBtn()));

    connect(ui->pushButton_saasNot2AA, SIGNAL(clicked()), this, SLOT(tosaasNot2AA()));
    connect(ui->pushButton_saas_verfy, SIGNAL(clicked()), this, SLOT(tosaasVerfy()));
    lds::hideWidget(ui->pushButton_saasNot2AA);
    lds::hideWidget(ui->pushButton_saas_verfy);
    //    lds::hideWidget(ui->lineEdit_saas);
}

w_sys_manage_cloudsync_with_time::~w_sys_manage_cloudsync_with_time()
{
    delete ui;
}

void w_sys_manage_cloudsync_with_time::took()
{
    saveData();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
}

void w_sys_manage_cloudsync_with_time::toexit()
{
    this->reject();
}

void w_sys_manage_cloudsync_with_time::toupload()
{
    w_sys_manage_cloudsync_with_time_running dialog(w_sys_manage_cloudsync_with_time_running::cloud_up);
    transparentCenterDialog(&dialog).exec();
}

void w_sys_manage_cloudsync_with_time::tousercheck()
{
    ui->label_orgCode->clear();

    QString httpkey = http_www_yqspos_com +"/account/getOrgCode-account";
    QMap<QString, QString> headInfo;

    headInfo.insert("userName",ui->lineEdit_user->text().toLocal8Bit());
    headInfo.insert("password",ui->lineEdit_password->text().toLocal8Bit());

    //!等待
    lds_messagebox_loading_show loading(this, "", "WAITING...");
    loading.show();
    //!~等待
    httpDataAutoDel json_yun_parameter=w_sys_manage_cloudsync::get_yun_httpget(this, httpkey, headInfo,QObject::tr("验证"), true, false, 5*1000);
    if(200 == json_yun_parameter.httpStatus) {
        ui->label_orgCode->setText(cJSON_help::getcJSONvalue(json_yun_parameter.json, "orgCode").toString());
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
    }

    //无论失败还是成功，都保存
    n_func::f_set_sysparm("cloud_user", ui->lineEdit_user->text());
    n_func::f_set_sysparm("orgCode",ui->label_orgCode->text());
    n_func::f_set_sysparm("cloud_password", ui->lineEdit_password->text());
    n_func::f_set_sysparm("cloud_ip", ui->comboBox_ip->currentText());
}

void w_sys_manage_cloudsync_with_time::todatekeep()
{
    w_sys_manage_cloudsync_with_time_running dialog(w_sys_manage_cloudsync_with_time_running::cloud_downup);
    transparentCenterDialog(&dialog).exec();
}

void w_sys_manage_cloudsync_with_time::todownload()
{
    if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("本操作将会清空本地所有基础资料"), QObject::tr("继续"), QObject::tr("取消"))) {
        w_sys_manage_cloudsync_with_time_running dialog(w_sys_manage_cloudsync_with_time_running::cloud_down);
        transparentCenterDialog(&dialog).exec();
    }
}

void w_sys_manage_cloudsync_with_time::totimecheck()
{
    //-1云端失败 0本地校对失败 1校对成功 2无需校对//
    switch(w_sys_manage_cloudsync::match_local_time(this)) {
    case 1:
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("校对成功"));
        break;
    case 2:
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("时间同步,无需校对"));
        break;
    }
}

void w_sys_manage_cloudsync_with_time::toflowupload()
{
    switch(lds_messagebox::question(this,MESSAGE_TITLE_VOID, QObject::tr("流水上传"), QObject::tr("全部流水上传"), QObject::tr("未上传流水上传"), QObject::tr("取消"))) {
    case 0:
    {
        if(1 == lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("全部流水将置为未上传状态，流水越多花时间越多"), QObject::tr("继续"), QObject::tr("取消"))) {
            return;
        }
        lds_messagebox_loading_show loading(this, "");
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

void w_sys_manage_cloudsync_with_time::tosaasNot2AA()
{
    if(0 == lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("终端号") + ":" +lds::terminalCode, QObject::tr("升级"), QObject::tr("取消"))) {
        if(saas_pay_detail::saas_type() == "") {
            saas_guide2 dialog(new saas_guide_activateTerminal);
            if(QDialog::Rejected == transparentCenterDialog(&dialog, false).exec()) {
                return ;
            }
        }
        if(saas_pay_detail::saas_type() == "") {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("软件未激活"));
            return ;
        }
    }
}

void w_sys_manage_cloudsync_with_time::toshowSaaSBtn()
{
    if(ui->lineEdit_saas->text().trimmed().toLower() == "saas") {
        lds::showWidget(ui->pushButton_saasNot2AA);
        lds::showWidget(ui->pushButton_saas_verfy);
    }
}

void w_sys_manage_cloudsync_with_time::tosaasVerfy()
{
    httpDataAutoDel httpd = saas_guide::getTerminal();
    if(200 == httpd.httpStatus) {
        lds_messagebox::information(this,
                                    MESSAGE_TITLE_VOID,
                                    QObject::tr("终端号") + ":" + lds::terminalCode + "\n" +
                                    QObject::tr("截止日期") + ":" +cJSON_help::getcJSONvalue(httpd.json, "dueDate").toString() +"\n"+
                                    "saas"+QObject::tr("版本") + ":" +saas_pay_detail::codeSaaSMapGet().value(cJSON_help::getcJSONvalue(httpd.json, "type").toString()) +"\n"+
                                    QObject::tr("局域网机器个数") + ":" +cJSON_help::getcJSONvalue(httpd.json, "num").toString() +"\n"
                                    );
    } else {
        lds_messagebox::warning(this,
                                MESSAGE_TITLE_VOID,
                                QObject::tr("终端号") + ":" + lds::terminalCode + "\n" +httpd.message);
    }
}

void w_sys_manage_cloudsync_with_time::saveData()
{
    //不包括验证信息
    lds::sysconf->setValue("w_sys_manage_cloudsync_with_time/cloud_yun_is_run", ui->checkBox_isyunrun->isChecked());
    lds::sysconf->setValue("w_sys_manage_cloudsync_with_time/cloud_yun_member", ui->checkBox_yun_member->isChecked());
    lds::sysconf->setValue("w_sys_manage_cloudsync_with_time/cloud_no_use_local_member", ui->checkBox_yun_member_2->isChecked());
}

//=================================

w_sys_manage_cloudsync_with_time_running::w_sys_manage_cloudsync_with_time_running(cloudFlags flag, QWidget *parent) :
    QDialog(parent),
    ui( new Ui_w_sys_manage_cloudsync_with_time_running)
{
    ui->setupUi(this);
    this->setFixedHeight(150);
    this->setWindowTitle(QObject::tr("数据同步"));
    _flag = flag;

    orgCode = n_func::f_get_sysparm("orgCode");
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    //    connect(ui->statusBar, SIGNAL(messageChanged(QString)),ui->textEdit, SLOT(append(QString)));
    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(tocancel()));

    ui->progressBar->setFormat("%p%(%v/%m)");
    ui->pushButton_ok->hide();
    ui->textEdit->hide();
    ui->label_loading->hide();
    if(cloud_flow_up == flag) {
        ui->label_loading->show();
        ui->progressBar->hide();
        if(0 == ui->label_loading->movie()) {
            QMovie *mv = new QMovie(this);
            mv->setFileName(":/image/loading.gif");
            ui->label_loading->setMovie(mv);
            ui->label_loading->setFixedSize(60, 60);
            ui->label_loading->setScaledContents(true);
            ui->label_loading->setWordWrap(true);
            this->setFixedSize(400, 100);
        }
    }
    ui->pushButton_ok->animateClick(100);
}

w_sys_manage_cloudsync_with_time_running::~w_sys_manage_cloudsync_with_time_running()
{
    delete ui;
}

QString w_sys_manage_cloudsync_with_time_running::q_bt_get_del_sql(const QString &tablename, bool skip_ch_yun_already_keep_Y)
{
    if(skip_ch_yun_already_keep_Y) {
        return  QString("select * from del_%1 ").arg(tablename);
    } else {
        return  QString("select * from del_%1 where  ifnull(ch_yun_already_keep, '') <> 'Y' ").arg(tablename);
    }
}

bool w_sys_manage_cloudsync_with_time_running::sql_update_trigger()
{
    lds_query_hddpos  query;
    lds_query_hddpos  query2;
    QString update_value;
    QString delete_value;
    QString insert_value;

    QString key;
    QString tablename;
    QStringList key_list;


    lds_bool_default_false triggercount_matCH_STATE;
    {
        int trigger_count = 0;
        query.execSelect(" show triggers where `Trigger` like 'tr_web%'  ");
        while(query.next()) {
            trigger_count ++;
        }
        if(trigger_count == (55/*tr_web*/+0/*tr_android*/+1/*inventory*/ + 3/*原料配比*/+1/*促销*/) ) {
            triggercount_matCH_STATE =true;
        }
    }
    // "triggercount_matCH_STATE:" << triggercount_matCH_STATE;
    //触发器
    if(triggercount_matCH_STATE) return true;


    QString bt_trigger_update_sql_arg =
            (triggercount_matCH_STATE? "":"DROP TRIGGER IF EXISTS tr_web_%1_update;")+
            w_sys_manage_cloudsync_with_time::bt_trigger_update_sql_arg;
    QString bt_trigger_delete_sql_arg  =
            (triggercount_matCH_STATE? "":"DROP TRIGGER IF EXISTS tr_web_%1_delete;")+
            w_sys_manage_cloudsync_with_time::bt_trigger_delete_sql_arg;
    QString bt_trigger_insert_sql_arg  =
            (triggercount_matCH_STATE? "":"DROP TRIGGER IF EXISTS tr_web_%1_insert;")+
            w_sys_manage_cloudsync_with_time::bt_trigger_insert_sql_arg;

    query.execSelect(QString("select * from cey_web_table_version "));

    while(query.next()) {
        update_value = "";
        delete_value = "";
        insert_value = "";
        key = query.recordValue("vch_sqltablename_key_name").toString();
        tablename = query.recordValue("vch_sqltablename").toString();
        key_list = key.split(",", QString::SkipEmptyParts);

        //多个条件判断
        //NEW.%1 = OLD.%1 and
        //
        for(int sqr = 0; sqr < key_list.count(); sqr++) {
            update_value += QString("NEW.%1 = OLD.%1 and ").arg(key_list[sqr]);
            delete_value += QString("%1=OLD.%1 and ").arg(key_list[sqr]);
            insert_value += QString("%1=NEW.%1 and ").arg(key_list[sqr]);
        }
        update_value.chop(5);
        delete_value.chop(5);
        insert_value.chop(5);

        //更新
        if(!query2.execTrigger(bt_trigger_update_sql_arg.arg(tablename).arg(update_value))) {

        }

        //删除
        if(!query2.execTrigger(bt_trigger_delete_sql_arg.arg(tablename).arg(delete_value))) {

        }

        //插入
        if(!query2.execTrigger(bt_trigger_insert_sql_arg.arg(tablename).arg(insert_value))) {

        }


        //
        //

    }
    return true;
}

void w_sys_manage_cloudsync_with_time_running::sql_update()
{
    lds_query_hddpos  query;
    //////////////
    //触发器
    w_sys_manage_cloudsync_with_time_running::sql_update_trigger();
    ///单独设置会员参数
    query.execTrigger(w_sys_manage_cloudsync_with_time::bt_trigger_update_sql_arg.arg("t_m_parameter").arg("NEW.vch_parameter = OLD.vch_parameter"));
    ///单独设置库存
    query.execTrigger(w_sys_manage_cloudsync_with_time::bt_trigger_update_sql_arg.arg("cey_st_material_num").arg("NEW.ch_materialno = OLD.ch_materialno"));
    ///单独设置促销
    query.execTrigger(w_sys_manage_cloudsync_with_time::bt_trigger_update_sql_arg.arg("cey_sys_parameter").arg("NEW.vch_parameter = OLD.vch_parameter"));

    //流水，云会员标示
    query.execAlter("alter table cey_u_checkout_master  add column ch_member_belong_state  VARCHAR(1)");
    query.execAlter("alter table cey_u_orderdish  add column ch_member_belong_state  VARCHAR(1)");
    query.execAlter("alter table cey_u_hungbill  add column ch_member_belong_state  VARCHAR(1)");
    query.execAlter("alter table cey_u_checkout_master  add column ch_yun_payno  VARCHAR(15)");

}

void w_sys_manage_cloudsync_with_time_running::sql_check()
{
    //////核实删除表和原表的匹配性并且新建
    lds_query_hddpos  query;
    lds_query_hddpos  query2;
    QString tablename;
    query.execSelect("select * from cey_web_table_version");
    while(query.next()) {
        tablename = query.recordValue("vch_sqltablename").toString();
        if(tablename.isEmpty()) continue;

        query2.execSelect(QString("show columns from  %1 where `Field` like 'ch_del_state'; ").arg(tablename));
        if(query2.next()) {
            query2.execAlter(QString("alter table %1 drop column ch_del_state ").arg(tablename));
        }
        query2.execSelect(QString("show columns from  del_%1 where `Field` like 'ch_del_state';").arg(tablename));
        if(query2.next()) {
            query2.execAlter(QString("alter table del_%1 drop column ch_del_state ").arg(tablename));
        }

        query2.execCreate(QString("create table IF NOT EXISTS del_%1 like %1; ").arg(tablename));

        if(!query2.execSelect(QString(" (select * from %1 limit 0) union (select * from del_%1 limit 0) ").arg(tablename))) {
#ifdef QT_DEBUG
            qDebug() << "删除表和原表的匹配不匹配" << tablename;
            //            QObject *o = 0;
            //            o->children();
#endif
            query2.execDrop(QString("del_%1").arg(tablename));
            query.previous();
        }
    }
}

int w_sys_manage_cloudsync_with_time_running::q_bt_check_state_and_del(QObject *parent_nozero_trigger_slot, const QString &tablename, cJSON *pItem, int sqr, QString *where_sql)
{
    lds_query_hddpos  query;
    int state = cJSON_help::getcJSONvalue(pItem,"state").toInt();

    //获取where_sql
    query.execSelect(QString("select vch_sqltablename_key_name, vch_sqltablename_yun_key_name FROM cey_web_table_version where vch_sqltablename = '%1' ;")
                     .arg(tablename));
    query.next();
    QString vch_sqltablename_key_name = query.recordValue("vch_sqltablename_key_name").toString();
    QString vch_sqltablename_yun_key_name = query.recordValue("vch_sqltablename_yun_key_name").toString();
    QString arg2 = QString(" %1='%2' ").arg(vch_sqltablename_key_name).arg(cJSON_help::getcJSONvalue(pItem,vch_sqltablename_yun_key_name.toLocal8Bit()).toString());
    if(vch_sqltablename_key_name.contains(",")) {
        arg2 = " ";
        QStringList  list0 = vch_sqltablename_key_name.split(",", QString::SkipEmptyParts);
        QStringList  list1 = vch_sqltablename_yun_key_name.split(",", QString::SkipEmptyParts);
        for(int sqr = 0; sqr < list0.count(); sqr++) {
            arg2  += QString(" %1 = '%2' ").arg(list0[sqr], cJSON_help::getcJSONvalue(pItem, list1[sqr].toLocal8Bit()).toString()) + " and ";
        }
        arg2.chop(5);
    }
    if(where_sql) *where_sql = arg2;

    //同步删除
    if( state == -1) {//删除
        /**/        if(parent_nozero_trigger_slot)QMetaObject::invokeMethod(parent_nozero_trigger_slot, "updateGoing",Qt::QueuedConnection,
                                                                            Q_ARG(QString, QString("%1-%2-%3").arg("更新本地-删除" ).arg(sqr+1).arg(tablename))
                                                                            );
        if(!query.execDelete(tablename, arg2)) {
            QObject *o=0;
            o->children();
        }
    }
    return state;
}

bool w_sys_manage_cloudsync_with_time_running::q_bt_get_ready_total_byconf()
{
    if(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_is_run", false).toBool()
            &&w_sys_manage_cloudsync_with_time_running::q_bt_get_ready_total() > 0) {
        return true;
    }
    return false;
}

int w_sys_manage_cloudsync_with_time_running::q_bt_get_ready_total()
{
    lds_query_hddpos  query;
    lds_query_hddpos  query2;
    QString tablename;
    int result = 0;
    //
    query.execSelect("select *  from cey_web_table_version ");
    while(query.next()) {
        tablename = query.recordValue("vch_sqltablename").toString();

        query2.execSelect(QString("select count(0) from %1 where ifnull(ch_yun_already_keep,'') <> 'Y' ").arg(tablename));
        query2.next();
        result += query2.recordValue(0).toInt();

        query2.execSelect(QString("select count(0) from del_%1 where ifnull(ch_yun_already_keep,'') <> 'Y' ").arg(tablename));
        query2.next();
        result += query2.recordValue(0).toInt();
    }

    ///单独设置会员参数
    result += q_m_parameter_get_ready_total();
    ///单独设置库存
    result += q_bt_material_num_get_ready_total();
    ///单独设置促销
    result += q_bt_promotion_get_ready_total();
    return result;
}

///单独设置会员参数
int w_sys_manage_cloudsync_with_time_running::q_m_parameter_get_ready_total()
{
    lds_query_hddpos  query2;
    int result = 0;
    query2.execSelect(QString("select count(0) from %1 where ifnull(ch_yun_already_keep,'') <> 'Y' ").arg("t_m_parameter"));
    query2.next();
    result += query2.recordValue(0).toInt();

    return result;
}

///单独设置库存
int w_sys_manage_cloudsync_with_time_running::q_bt_material_num_get_ready_total()
{
    lds_query_hddpos  query2;
    int result = 0;
    query2.execSelect(QString("select count(0) from %1 where ifnull(ch_yun_already_keep,'') <> 'Y' ").arg("cey_st_material_num"));
    query2.next();
    result += query2.recordValue(0).toInt();

    return result;
}

///单独设置促销
int w_sys_manage_cloudsync_with_time_running::q_bt_promotion_get_ready_total()
{
    int result = 0;
    lds_query_hddpos  query;
    query.execSelect("select vch_value FROM cey_sys_parameter where ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_am'; ");if(query.next()) {result ++;}
    query.execSelect("select vch_value FROM cey_sys_parameter where ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_am_2'; ");if(query.next()) {result ++;}
    query.execSelect("select vch_value FROM cey_sys_parameter where ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_date'; ");if(query.next()) {result ++;}
    query.execSelect("select vch_value FROM cey_sys_parameter where ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_date_2'; ");if(query.next()) {result ++;}
    query.execSelect("select vch_value FROM cey_sys_parameter where ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_pm'; ");if(query.next()) {result ++;}

    query.execSelect("select vch_value FROM cey_sys_parameter where ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_pm_2'; ");if(query.next()) {result ++;}
    query.execSelect("select vch_value FROM cey_sys_parameter where ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_Mon'; ");if(query.next()) {result ++;}
    query.execSelect("select vch_value FROM cey_sys_parameter where ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_Tues'; ");if(query.next()) {result ++;}
    query.execSelect("select vch_value FROM cey_sys_parameter where ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_Wed'; ");if(query.next()) {result ++;}
    query.execSelect("select vch_value FROM cey_sys_parameter where ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_Thur'; ");if(query.next()) {result ++;}

    query.execSelect("select vch_value FROM cey_sys_parameter where ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_Fri'; ");if(query.next()) {result ++;}
    query.execSelect("select vch_value FROM cey_sys_parameter where ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_Sat'; ");if(query.next()) {result ++;}
    query.execSelect("select vch_value FROM cey_sys_parameter where ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_Sun'; ");if(query.next()) {result ++;}
    query.execSelect("select vch_value FROM cey_sys_parameter where ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_target'; ");if(query.next()) {result ++;}

    return result;
}

bool w_sys_manage_cloudsync_with_time_running::q_bt_promotion_put_ch_yun_already_keep_Y()
{
    lds_query_hddpos  query;
    if(!query.execUpdate("cey_sys_parameter", "ch_yun_already_keep", 1, "ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_am'; ")) {return false;}
    if(!query.execUpdate("cey_sys_parameter", "ch_yun_already_keep", 1, "ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_am_2'; ")) {return false;}
    if(!query.execUpdate("cey_sys_parameter", "ch_yun_already_keep", 1, "ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_date'; ")) {return false;}
    if(!query.execUpdate("cey_sys_parameter", "ch_yun_already_keep", 1, "ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_date_2'; ")) {return false;}
    if(!query.execUpdate("cey_sys_parameter", "ch_yun_already_keep", 1, "ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_pm'; ")) {return false;}

    if(!query.execUpdate("cey_sys_parameter", "ch_yun_already_keep", 1, "ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_pm_2'; ")) {return false;}
    if(!query.execUpdate("cey_sys_parameter", "ch_yun_already_keep", 1, "ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_Mon'; ")) {return false;}
    if(!query.execUpdate("cey_sys_parameter", "ch_yun_already_keep", 1, "ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_Tues'; ")) {return false;}
    if(!query.execUpdate("cey_sys_parameter", "ch_yun_already_keep", 1, "ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_Wed'; ")) {return false;}
    if(!query.execUpdate("cey_sys_parameter", "ch_yun_already_keep", 1, "ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_Thur'; ")) {return false;}

    if(!query.execUpdate("cey_sys_parameter", "ch_yun_already_keep", 1, "ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_Fri'; ")) {return false;}
    if(!query.execUpdate("cey_sys_parameter", "ch_yun_already_keep", 1, "ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_Sat'; ")) {return false;}
    if(!query.execUpdate("cey_sys_parameter", "ch_yun_already_keep", 1, "ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_Sun'; ")) {return false;}
    if(!query.execUpdate("cey_sys_parameter", "ch_yun_already_keep", 1, "ifnull(ch_yun_already_keep,'') <> 'Y' and vch_parameter = 'promotion_setting_target'; ")) {return false;}

    return true;
}

cJSON *w_sys_manage_cloudsync_with_time_running::q_bt_get_piece_(const QString &tablename, lds_query_hddpos  &query, const QString &orgCode, int state)
{
    if(tablename == "cey_bt_dish")  return w_sys_manage::q_bt_get_piece_cey_bt_dish(query, orgCode, state);
    if(tablename == "cey_bt_dish_subtype")  return w_sys_manage::q_bt_get_piece_cey_bt_dish_subtype(query, orgCode, state);
    if(tablename == "cey_bt_dish_type")  return w_sys_manage::q_bt_get_piece_cey_bt_dish_type(query, orgCode, state);
    if(tablename == "cey_bt_dish_suit") return w_sys_manage::q_bt_get_piece_cey_bt_dish_suit(query, orgCode, state);
    if(tablename == "cey_bt_suit_change")  return w_sys_manage::q_bt_get_piece_cey_bt_suit_change(query, orgCode, state);

    if(tablename == "cey_bt_unit")  return w_sys_manage::q_bt_get_piece_cey_bt_unit(query, orgCode, state);
    if(tablename == "cey_bt_table")  return w_sys_manage::q_bt_get_piece_cey_bt_table(query, orgCode, state);
    if(tablename == "cey_bt_table_type")  return w_sys_manage::q_bt_get_piece_cey_bt_table_type(query, orgCode, state);
    if(tablename == "cey_bt_table_area")  return w_sys_manage::q_bt_get_piece_cey_bt_table_area(query, orgCode, state);
    if(tablename == "cey_bt_cp_dish")  return w_sys_manage::q_bt_get_piece_cey_bt_cp_dish(query, orgCode, state);

    if(tablename == "cey_bt_cp_memo")  return w_sys_manage::q_bt_get_piece_cey_bt_cp_memo(query, orgCode, state);
    if(tablename == "cey_bt_cp_type")  return w_sys_manage::q_bt_get_piece_cey_bt_cp_type(query, orgCode, state);
    if(tablename == "cey_bt_dish_warn")  return w_sys_manage::q_bt_get_piece_cey_bt_dish_warn(query, orgCode, state);
    if(tablename == "cey_bt_paymode")  return w_sys_manage::q_bt_get_piece_cey_bt_paymode(query, orgCode, state);
    if(tablename == "material_bt_dish")  return w_sys_manage::q_bt_get_piece_material_bt_dish(query, orgCode, state);


    if(tablename == "t_m_member_type")  return w_sys_manage::q_m_get_piece_t_m_member_type(query, orgCode, state);
    if(tablename == "t_m_member")  return w_sys_manage::q_m_get_piece_t_m_member(query, orgCode, state);
    if(tablename == "t_m_gift")  return w_sys_manage::q_m_get_piece_t_m_gift(query, orgCode, state);
    if(tablename == "t_m_curamount")  return w_sys_manage::q_m_get_piece_t_m_curamount(query, orgCode, state);


    if(tablename == "t_m_parameter")  return w_sys_manage::q_m_get_piece_t_m_parameter(query, orgCode);

#ifdef QT_DEBUG
    qDebug() << "not found" << __LINE__ << tablename;
    QObject *o = 0;
    o->children();
#endif
    return 0;
}

void w_sys_manage_cloudsync_with_time_running::q_bt_put_piece_(const QString &tablename, qrtVariantPairList &key_value_list, cJSON *pItem)
{
    if(tablename == "cey_bt_dish")   {w_sys_manage::q_bt_put_piece_cey_bt_dish(key_value_list, pItem);return;}
    if(tablename == "cey_bt_dish_subtype")   {w_sys_manage::q_bt_put_piece_cey_bt_dish_subtype(key_value_list, pItem);return;}
    if(tablename == "cey_bt_dish_type")   {w_sys_manage::q_bt_put_piece_cey_bt_dish_type(key_value_list, pItem);return;}
    if(tablename == "cey_bt_dish_suit")  {w_sys_manage::q_bt_put_piece_cey_bt_dish_suit(key_value_list, pItem);return;}
    if(tablename == "cey_bt_suit_change")   {w_sys_manage::q_bt_put_piece_cey_bt_suit_change(key_value_list, pItem);return;}

    if(tablename == "cey_bt_unit")   {w_sys_manage::q_bt_put_piece_cey_bt_unit(key_value_list, pItem);return;}
    if(tablename == "cey_bt_table")   {w_sys_manage::q_bt_put_piece_cey_bt_table(key_value_list, pItem);return;}
    if(tablename == "cey_bt_table_type")   {w_sys_manage::q_bt_put_piece_cey_bt_table_type(key_value_list, pItem);return;}
    if(tablename == "cey_bt_table_area")   {w_sys_manage::q_bt_put_piece_cey_bt_table_area(key_value_list, pItem);return;}
    if(tablename == "cey_bt_cp_dish")   {w_sys_manage::q_bt_put_piece_cey_bt_cp_dish(key_value_list, pItem);return;}

    if(tablename == "cey_bt_cp_memo")   {w_sys_manage::q_bt_put_piece_cey_bt_cp_memo(key_value_list, pItem);return;}
    if(tablename == "cey_bt_cp_type")   {w_sys_manage::q_bt_put_piece_cey_bt_cp_type(key_value_list, pItem);return;}
    if(tablename == "cey_bt_dish_warn")   {/*w_sys_manage::q_bt_put_piece_cey_bt_dish_warn(key_value_list, pItem);*/return;}
    if(tablename == "cey_bt_paymode")   {w_sys_manage::q_bt_put_piece_cey_bt_paymode(key_value_list, pItem);return;}
    if(tablename == "material_bt_dish")   {w_sys_manage::q_bt_put_piece_material_bt_dish(key_value_list, pItem);return;}


    if(tablename == "t_m_member_type")   {w_sys_manage::q_m_put_piece_t_m_member_type(key_value_list, pItem);return;}
    if(tablename == "t_m_member")   {w_sys_manage::q_m_put_piece_t_m_member(key_value_list, pItem);return;}
    if(tablename == "t_m_gift")   {w_sys_manage::q_m_put_piece_t_m_gift(key_value_list, pItem);return;}
    if(tablename == "t_m_curamount")   {w_sys_manage::q_m_put_piece_t_m_curamount(key_value_list, pItem);return;}


    if(tablename == "t_m_parameter")   {w_sys_manage::q_m_put_piece_t_m_parameter(key_value_list, pItem);return;}
#ifdef QT_DEBUG
    qDebug() << "not found" <<__LINE__<< tablename;
    QObject *o = 0;
    o->children();
#endif
}

int w_sys_manage_cloudsync_with_time_running::get_state_local_dish_base_keepdata()
{
    //获取map
    lds_query_hddpos  query;
    int int_version;
    QString retparam;

    QMap<QString, QString> headInfo;
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString http_key=http_www_yqspos_com +"/arm/dish-base/getVersion-rest";

    w_sys_manage_cloudsync_transfer transfer;
    httpDataAutoDel ret_json_err  = transfer.sendGet(http_key, headInfo,retparam, 20*60*1000);

    if(ret_json_err.json==NULL) {
        return -1;//-1 云端连接失败
    }
    if(!query.execSelect("select vch_sqltablename, vch_sqltablename_yun_version, int_version from cey_web_table_version")) {
        return -2;//-2数据库连接失败
    }
    while(query.next()) {
        cJSON *item = cJSON_help::getcJSON(ret_json_err.json, query.recordValue("vch_sqltablename_yun_version").toByteArray());
        if(item == 0) {
            return -3;//-3数据库和获取size不匹配
        }
        int_version = cJSON_help::getItemValue(item).toInt();
        if(query.recordValue("int_version").toInt() != int_version) {
            return 0;//0需要匹配
        }
    }
    return 1;//没有需要同步的数据
}

void w_sys_manage_cloudsync_with_time_running::exec_keepdata_byconf(QWidget *parent)
{
    if(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_is_run", false).toBool()
            &&w_sys_manage_cloudsync_with_time_running::q_bt_get_ready_total() > 0) {
        exec_keepdata(parent);
    }
}

void w_sys_manage_cloudsync_with_time_running::exec_keepdata(QWidget *parent)
{
    w_sys_manage_cloudsync_with_time_running dialog(w_sys_manage_cloudsync_with_time_running::cloud_downup, parent);
    transparentCenterDialog(&dialog).exec();
}

void w_sys_manage_cloudsync_with_time_running::exec_keepdata_down_byconf(QWidget *parent)
{
    if(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_is_run", false).toBool()
            &&w_sys_manage_cloudsync_with_time_running::q_bt_get_ready_total() > 0) {
        w_sys_manage_cloudsync_with_time_running dialog(w_sys_manage_cloudsync_with_time_running::cloud_down, parent);
        transparentCenterDialog(&dialog).exec();
    }
}

void w_sys_manage_cloudsync_with_time_running::exec_no_up_then_down_byconf(QWidget *parent)
{
    if(0 == q_bt_get_ready_total_byconf()) {
        exec_keepdata_down_byconf(parent);
    }
}

void w_sys_manage_cloudsync_with_time_running::yun_version_diff_byconf_trigger_func(QWidget *parent)
{
    if(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_is_run", false).toBool()) {
        public_sql::save_login->yun_version_diff_polling->emitsignalTrigger(parent);
    }
}

cJSON *w_sys_manage_cloudsync_with_time_running::q_bt_get_table_version_array()
{
    lds_query_hddpos  query;
    cJSON *item = cJSON_CreateObject();
    query.execSelect("select *  from cey_web_table_version  ");
    while(query.next()) {
        cJSON_AddNumberToObject(item, query.recordValue("vch_sqltablename_yun_version").toByteArray().data(), query.recordValue("int_version").toInt());
    }
    return item;
}

bool w_sys_manage_cloudsync_with_time_running::q_bt_put_table_version(cJSON *root)
{
    lds_query_hddpos  query;
    lds_query_hddpos  query2;
    //1
    query.execSelect("select *  from cey_web_table_version ");
    while(query.next()) {
        if(!query2.execUpdate("cey_web_table_version",
                              "int_version", cJSON_help::getcJSONvalue(root, query.recordValue("vch_sqltablename_yun_version").toByteArray().data()).toString(),
                              qrtEqual("vch_sqltablename", query.recordValue("vch_sqltablename").toString()))) {

            return false;
        }
    }
    return true;
}

bool w_sys_manage_cloudsync_with_time_running::q_bt_put_table_version(const cJSON_help &root)
{
    lds_query_hddpos  query;
    lds_query_hddpos  query2;
    //1
    query.execSelect("select *  from cey_web_table_version ");
    while(query.next()) {
        if(!query2.execUpdate("cey_web_table_version",
                              "int_version", cJSON_help::getcJSONvalue(root, query.recordValue("vch_sqltablename_yun_version").toByteArray()),
                              qrtEqual("vch_sqltablename", query.recordValue("vch_sqltablename").toString()))) {

            return false;
        }
    }
    return true;
}

bool w_sys_manage_cloudsync_with_time_running::q_bt_put_table_version(QSharedPointer<cJSON> root)
{
    return q_bt_put_table_version(root.data());
}

bool w_sys_manage_cloudsync_with_time_running::q_bt_put_table_cey(QObject *parent_nozero_trigger_slot, cJSON *root)
{
    lds_query_hddpos  query;
    lds_query_hddpos  query2;
    cJSON *root_sub;
    QString tablename;
    QString yun_tablename;
    QString  key;
    QString  yun_key;

    query.execSelect("select * from cey_web_table_version ");
    while(query.next()) {
        tablename = query.recordValue("vch_sqltablename").toString();
        yun_tablename = query.recordValue("vch_sqltablename_yun").toString();
        key = query.recordValue("vch_sqltablename_key_name").toString();
        yun_key = query.recordValue("vch_sqltablename_yun_key_name").toString();

        root_sub =  cJSON_GetObjectItem(root, query.recordValue("vch_sqltablename_yun").toByteArray().data());

        /**/        if(parent_nozero_trigger_slot)QMetaObject::invokeMethod(parent_nozero_trigger_slot, "updateGoing",Qt::QueuedConnection,
                                                                            Q_ARG(QString, QString("%1-%2").arg("更新本地" ).arg(tablename)),
                                                                            Q_ARG(int, keep_going++)
                                                                            );

        for(int k =0, count = cJSON_GetArraySize(root_sub); k< count; k++) {
            cJSON *pItem = cJSON_GetArrayItem(root_sub, k);

            QStringList yun_key_list = yun_key.split(",", QString::SkipEmptyParts);
            QStringList key_list = key.split(",", QString::SkipEmptyParts);
            if(key_list.count() != yun_key_list.count()) {
                qDebug() << "上传主键和主键名，个数不匹配";
                QObject *o=0;
                o->children();
            }
            QString sql_where;
            if(q_bt_check_state_and_del(parent_nozero_trigger_slot, tablename, pItem, k,  &sql_where) == 0) {
                qrtVariantPairList key_value_list;
                q_bt_put_piece_(tablename, key_value_list, pItem);
                //check existed
                query2.execSelect(QString("select %1 from %2 where ").arg(key).arg(tablename) + sql_where);
                if(query2.next()) {
                    if(!query2.execUpdate(tablename, key_value_list, sql_where)) {
                        // << "更新失败" << tablename << __LINE__;
                        return false;
                    }
                } else {
                    if(!query2.execInsert(tablename, key_value_list)) {
                        // << "更新失败" << tablename << __LINE__;
                        return false;
                    }
                }
                /**/        if(parent_nozero_trigger_slot)QMetaObject::invokeMethod(parent_nozero_trigger_slot, "updateGoing",Qt::QueuedConnection,
                                                                                    Q_ARG(QString, QString("%1-%2-%3").arg("更新本地" ).arg(k + 1).arg(tablename))
                                                                                    );
            }
        }
    }

    ///单独设置会员参数
    tablename = "t_m_parameter";
    /**/        if(parent_nozero_trigger_slot)QMetaObject::invokeMethod(parent_nozero_trigger_slot, "updateGoing",Qt::QueuedConnection,
                                                                        Q_ARG(QString,  QString("%1-%2-%3").arg("更新本地-删除" ).arg(1).arg(tablename)),
                                                                        Q_ARG(int,  keep_going++)
                                                                        );
    if(!query2.execDelete(QString("t_m_parameter"))) {
        return false;
    }
    root_sub =  cJSON_GetObjectItem(root, "memberParameterList");
    for(int i =0;i<cJSON_GetArraySize(root_sub);i++) {
        cJSON *pItem = cJSON_GetArrayItem(root_sub,i);
        qrtVariantPairList key_value_list;
        q_bt_put_piece_(tablename, key_value_list, pItem);
        if(!query2.execInsert(tablename, key_value_list)) {
            qDebug() << query2.recordError();
            return false;
        }
        /**/        if(parent_nozero_trigger_slot)QMetaObject::invokeMethod(parent_nozero_trigger_slot, "updateGoing",Qt::QueuedConnection,
                                                                            Q_ARG(QString,  QString("%1-%2-%3").arg("更新本地" ).arg(i+1).arg(tablename))
                                                                            );
    }
    ///～单独设置会员

    ///单独设置促销
    /**/        if(parent_nozero_trigger_slot)QMetaObject::invokeMethod(parent_nozero_trigger_slot, "updateGoing",Qt::QueuedConnection,
                                                                        Q_ARG(QString,  QObject::tr("更新促销")),
                                                                        Q_ARG(int,  keep_going++)
                                                                        );
    root_sub =  cJSON_GetObjectItem(root, "dishSysParameter");
    if(root_sub) {
        if(!w_sys_manage::q_bt_put_piece_promotion(root_sub)) {
            return false;
        }
    }
    ///~单独设置促销
    //1
    return true;
}

bool w_sys_manage_cloudsync_with_time_running::q_bt_put_table_cey(QObject *parent_nozero_trigger_slot, QSharedPointer<cJSON> root)
{
    return q_bt_put_table_cey(parent_nozero_trigger_slot, root.data());
}

bool w_sys_manage_cloudsync_with_time_running::q_bt_put_ch_yun_already_keep_Y()
{
    //触发器，设置1来触发Y
    lds_query_hddpos  query;
    lds_query_hddpos  query2;
    query.execSelect("select *  from cey_web_table_version ");
    while(query.next()) {
        ///有触发器触发
        if(!query2.execUpdate(query.recordValue("vch_sqltablename").toString(),
                              "ch_yun_already_keep", "1",
                              "ifnull(ch_yun_already_keep, '') <> 'Y'")) {
            return false;
        }

        if(!query2.execDelete(QString("del_%1")
                              .arg(query.recordValue("vch_sqltablename").toString()))) {

            return false;
        }
    }

    ///单独设置会员参数
    if(!query.execUpdate("t_m_parameter", "ch_yun_already_keep", 1, "ifnull(ch_yun_already_keep, '') <> 'Y' ")) {
        return false;
    }
    ///单独设置库存
    if(!query.execUpdate("cey_st_material_num", "ch_yun_already_keep", 1, "ifnull(ch_yun_already_keep, '') <> 'Y' ")) {
        return false;
    }
    ///单独设置促销
    if(!q_bt_promotion_put_ch_yun_already_keep_Y()) {
        return false;
    }
    return true;
}

bool w_sys_manage_cloudsync_with_time_running::q_st_upload_material_num(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap)
{
    QMap<QString, QString> headInfo;
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString httpkey = http_www_yqspos_com +"/arm/dish-base/update-rest";

    cJSON_help root(cJSON_CreateObject());
    cJSON *root_array = cJSON_CreateArray();
   cJSON_help::_cJSON_AddItemToObject(root, "dishInfoList", root_array);
    query.execSelect(QString("SELECT * from cey_st_material_num "));
    //ch_materialno, num_num
    while(query.next()) {
        cJSON *item = cJSON_CreateObject();
        cJSON_AddItemToArray(root_array, item);
        cJSON_help::_cJSON_AddValueToObject(item, "diChDishNo", queryValueMapFilter(query, otherValueMap, "ch_materialno"));
        cJSON_help::_cJSON_AddValueToObject(item, "inventory", queryValueMapFilter(query, otherValueMap, "num_num"));

        cJSON_help::_cJSON_AddValueToObject(item, "orgCode", orgCode);
    }

    QByteArray param = cJSON_help::getcJSONprint(root);
    httpDataAutoDel json_yun_parameter=w_sys_manage_cloudsync::get_yun_httppos(0, httpkey, param, headInfo, QObject::tr("云同步"));
    if(200 == json_yun_parameter.httpStatus) {
        return true;
    }
    return false;
}

cJSON *w_sys_manage_cloudsync_with_time_running::q_u_get_void_flow_root()
{

    cJSON *root = cJSON_CreateObject();
    //        x7
    //            cey_u_checkout_detail-----------------------checkOutDetail
    cJSON_AddItemToObject(root, "checkOutDetail", cJSON_CreateArray());
    //            cey_u_checkout_master-----------------------checkOutMaster
    cJSON_AddItemToObject(root, "checkOutMaster", cJSON_CreateArray());
    //            cey_u_orderdish-----------------------orderDish
    cJSON_AddItemToObject(root, "orderDish", cJSON_CreateArray());
    //            cey_u_service_low-----------------------serviceLow
    cJSON_AddItemToObject(root, "serviceLow", cJSON_CreateArray());
    //            cey_u_orderdish_log-----------------------orderDishLog
    cJSON_AddItemToObject(root, "orderDishLog", cJSON_CreateArray());
    //            cey_u_togo-----------------------toGo
    cJSON_AddItemToObject(root, "toGo", cJSON_CreateArray());
    //            cey_u_table-----------------------tableRecordsConsumption
    cJSON_AddItemToObject(root, "tableRecordsConsumption", cJSON_CreateArray());
    //            //x3
    //           t_m_deposit-----------------------memberDeposit
    cJSON_AddItemToObject(root, "memberDeposit", cJSON_CreateArray());
    //           t_m_point-----------------------memberPointList
    cJSON_AddItemToObject(root, "memberPointList", cJSON_CreateArray());
    //           t_m_pay-----------------------memberPay
    cJSON_AddItemToObject(root, "memberPay", cJSON_CreateArray());


    return root;
}



cJSON *w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_checkout_detail(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap)
{

    cJSON *pItem = cJSON_CreateObject();
    cJSON_help::_cJSON_AddValueToObject(pItem, "codIntFlowId", queryValueMapFilter(query, otherValueMap, "int_flowID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "codChPayNo", queryValueMapFilter(query, otherValueMap, "ch_payno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "codChPayModeNo", queryValueMapFilter(query, otherValueMap, "ch_paymodeno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "codNumPayAmount", queryValueMapFilter(query, otherValueMap, "num_payamount"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "codNumChange", queryValueMapFilter(query, otherValueMap, "num_change"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "codNumRealAmount", queryValueMapFilter(query, otherValueMap, "num_realamount"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "codNumFace", queryValueMapFilter(query, otherValueMap, "num_face"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "codVchVoucherNo", queryValueMapFilter(query, otherValueMap, "vch_voucherno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "codVchMemo", queryValueMapFilter(query, otherValueMap, "vch_memo"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "codVchOperId", queryValueMapFilter(query, otherValueMap, "vch_operID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "codDtOperDate", queryValueMapFilter(query, otherValueMap, "dt_operdate"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "codNumCardRemain", queryValueMapFilter(query, otherValueMap, "num_card_remain"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "codNumFaceNum", queryValueMapFilter(query, otherValueMap, "num_face_Num"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);
    cJSON_help::_cJSON_AddValueToObject(pItem, "terminalCode", queryValueMapFilter(query, otherValueMap, "terminal_code"));


    return pItem;
}

cJSON *w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_checkout_master(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::_cJSON_AddValueToObject(pItem, "comChPayNo", queryValueMapFilter(query, otherValueMap, "ch_payno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comChBillNo", queryValueMapFilter(query, otherValueMap, "ch_billno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comChTableNo", queryValueMapFilter(query, otherValueMap, "ch_tableno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comNumCost", queryValueMapFilter(query, otherValueMap, "num_cost"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comNumDiscount", queryValueMapFilter(query, otherValueMap, "num_discount"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comNumPresent", queryValueMapFilter(query, otherValueMap, "num_present"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comNumService", queryValueMapFilter(query, otherValueMap, "num_service"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comNumLowCost", queryValueMapFilter(query, otherValueMap, "num_lowcost"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comNumBlotout", queryValueMapFilter(query, otherValueMap, "num_blotout"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comNumFree",query.recordValue("num_free"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comVchFreeOperId", queryValueMapFilter(query, otherValueMap, "vch_free_operID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comNumCash", queryValueMapFilter(query, otherValueMap, "num_cash"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comNumOther", queryValueMapFilter(query, otherValueMap, "num_other"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "comNumRun", queryValueMapFilter(query, otherValueMap, "num_run"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comVchRunOperId", queryValueMapFilter(query, otherValueMap, "vch_run_operID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comChState", queryValueMapFilter(query, otherValueMap, "ch_state"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comVchOperId", queryValueMapFilter(query, otherValueMap, "vch_operID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comDtOperDate", queryValueMapFilter(query, otherValueMap, "dt_operdate"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comVchOperId2", queryValueMapFilter(query, otherValueMap, "vch_operID2"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comDtOperDate2", queryValueMapFilter(query, otherValueMap, "dt_operdate2"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comVchMemberNo", queryValueMapFilter(query, otherValueMap, "vch_memberno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comChType", queryValueMapFilter(query, otherValueMap, "ch_type"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "comNumRate", queryValueMapFilter(query, otherValueMap, "num_rate"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);

    cJSON_help::_cJSON_AddValueToObject(pItem, "terminalCode", queryValueMapFilter(query, otherValueMap, "terminal_code"));

    return pItem;
}

cJSON *w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_orderdish(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap)
{
    cJSON *pItem = cJSON_CreateObject();
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdIntFlowId", queryValueMapFilter(query, otherValueMap, "int_flowID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdChBillNo", queryValueMapFilter(query, otherValueMap, "ch_billno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdChTableNo", queryValueMapFilter(query, otherValueMap, "ch_tableno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdIntId", queryValueMapFilter(query, otherValueMap, "int_id"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdChDishNo", queryValueMapFilter(query, otherValueMap, "ch_dishno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdNumPrice", queryValueMapFilter(query, otherValueMap, "num_price"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdNumPriceOrg", queryValueMapFilter(query, otherValueMap, "num_price_org"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdNumPriceAdd", queryValueMapFilter(query, otherValueMap, "num_price_add"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdNumNum", queryValueMapFilter(query, otherValueMap, "num_num"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdNumBack", queryValueMapFilter(query, otherValueMap, "num_back"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdVchBackOperId", queryValueMapFilter(query, otherValueMap, "vch_back_operID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdDtBackOperDate", queryValueMapFilter(query, otherValueMap, "dt_back_operdate"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdIntDiscount", queryValueMapFilter(query, otherValueMap, "int_discount"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdVchDisOperId", queryValueMapFilter(query, otherValueMap, "vch_dis_operID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdDtDisOperDate", queryValueMapFilter(query, otherValueMap, "dt_dis_operdate"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "btVchMemberNo", queryValueMapFilter(query, otherValueMap, "vch_memberno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdChConsult", queryValueMapFilter(query, otherValueMap, "ch_consult"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdChPrintFlag", queryValueMapFilter(query, otherValueMap, "ch_printflag"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdVchPrintMemo", queryValueMapFilter(query, otherValueMap, "vch_print_memo"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "dcChSuitFlag", queryValueMapFilter(query, otherValueMap, "ch_suitflag"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "dcChSuitNo", queryValueMapFilter(query, otherValueMap, "ch_suitno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdChSpecialFlag", queryValueMapFilter(query, otherValueMap, "ch_specialflag"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "chPresentFlag", queryValueMapFilter(query, otherValueMap, "ch_presentflag"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdVchPreOperId", queryValueMapFilter(query, otherValueMap, "vch_pre_operID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdDtPreOperDate", queryValueMapFilter(query, otherValueMap, "dt_pre_operdate"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdVchOperId", queryValueMapFilter(query, otherValueMap, "vch_operID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdDtOperDate", queryValueMapFilter(query, otherValueMap, "dt_operdate"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdChTogoNo", queryValueMapFilter(query, otherValueMap, "ch_togono"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdChPayNo", queryValueMapFilter(query, otherValueMap, "ch_payno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "bdChOutFlag", queryValueMapFilter(query, otherValueMap, "ch_outflag"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);
    cJSON_help::_cJSON_AddValueToObject(pItem, "terminalCode", queryValueMapFilter(query, otherValueMap, "terminal_code"));

    return pItem;
}

cJSON *w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_service_low(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap)
{
    cJSON *pItem = cJSON_CreateObject();
    cJSON_help::_cJSON_AddValueToObject(pItem, "slChPayNo", queryValueMapFilter(query, otherValueMap, "ch_payno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "slChType", queryValueMapFilter(query, otherValueMap, "ch_type"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "slNumAmount", queryValueMapFilter(query, otherValueMap, "num_amount"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "slVchOperId", queryValueMapFilter(query, otherValueMap, "vch_operID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "slDtOperDate", queryValueMapFilter(query, otherValueMap, "dt_operdate"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "slChState", queryValueMapFilter(query, otherValueMap, "ch_state"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "slVchMemo", queryValueMapFilter(query, otherValueMap, "vch_memo"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);
    cJSON_help::_cJSON_AddValueToObject(pItem, "terminalCode", queryValueMapFilter(query, otherValueMap, "terminal_code"));

    return pItem;
}

cJSON *w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_orderdish_log(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap)
{
    cJSON *pItem = cJSON_CreateObject();
    cJSON_help::_cJSON_AddValueToObject(pItem, "odlIntFlowId", queryValueMapFilter(query, otherValueMap, "int_flowID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "odlChType", queryValueMapFilter(query, otherValueMap, "ch_type"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "odlVchImpower", queryValueMapFilter(query, otherValueMap, "vch_impower"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "odlVchReason", queryValueMapFilter(query, otherValueMap, "vch_reason"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "odlVchOperId", queryValueMapFilter(query, otherValueMap, "vch_operID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "odlDtOperDate", queryValueMapFilter(query, otherValueMap, "dt_operdate"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "odlVchMemberNo", queryValueMapFilter(query, otherValueMap, "vch_memberno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "odlVchMemo", queryValueMapFilter(query, otherValueMap, "vch_memo"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "odlIntOrderFlow", queryValueMapFilter(query, otherValueMap, "int_orderflow"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);
    cJSON_help::_cJSON_AddValueToObject(pItem, "terminalCode", queryValueMapFilter(query, otherValueMap, "terminal_code"));

    return pItem;
}

cJSON *w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_togo(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap)
{
    cJSON *pItem = cJSON_CreateObject();
    cJSON_help::_cJSON_AddValueToObject(pItem, "chTogono", queryValueMapFilter(query, otherValueMap, "ch_togono"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "chBillno", queryValueMapFilter(query, otherValueMap, "ch_billno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "chPayno", queryValueMapFilter(query, otherValueMap, "ch_payno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "numCost", queryValueMapFilter(query, otherValueMap, "num_cost"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "numDiscount", queryValueMapFilter(query, otherValueMap, "num_discount"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "numPresent", queryValueMapFilter(query, otherValueMap, "num_present"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "numBlotout", queryValueMapFilter(query, otherValueMap, "num_blotout"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "vchWaiter", queryValueMapFilter(query, otherValueMap, "vch_waiter"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "vchAddress", queryValueMapFilter(query, otherValueMap, "vch_address"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "vchLinkman", queryValueMapFilter(query, otherValueMap, "vch_linkman"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "vchTel", queryValueMapFilter(query, otherValueMap, "vch_tel"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "chState", queryValueMapFilter(query, otherValueMap, "ch_state"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "vchOperId", queryValueMapFilter(query, otherValueMap, "vch_operID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "dtOperdate", queryValueMapFilter(query, otherValueMap, "dt_operdate"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "numRate", queryValueMapFilter(query, otherValueMap, "num_rate"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);
    cJSON_help::_cJSON_AddValueToObject(pItem, "terminalCode", queryValueMapFilter(query, otherValueMap, "terminal_code"));

    return pItem;
}

cJSON *w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_table(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap)
{
    cJSON *pItem = cJSON_CreateObject();
    cJSON_help::_cJSON_AddValueToObject(pItem, "chBillno", queryValueMapFilter(query, otherValueMap, "ch_billno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "chTableno", queryValueMapFilter(query, otherValueMap, "ch_tableno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "intPerson", queryValueMapFilter(query, otherValueMap, "int_person"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "vchMemo", queryValueMapFilter(query, otherValueMap, "vch_memo"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "chState", queryValueMapFilter(query, otherValueMap, "ch_state"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "chPayno", queryValueMapFilter(query, otherValueMap, "ch_payno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "vchOperId", queryValueMapFilter(query, otherValueMap, "vch_operID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "dtOperdate", queryValueMapFilter(query, otherValueMap, "dt_operdate"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "vchOperId2", queryValueMapFilter(query, otherValueMap, "vch_operID2"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "dtOperdate2", queryValueMapFilter(query, otherValueMap, "dt_operdate2"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "chOrderState", queryValueMapFilter(query, otherValueMap, "ch_order_state"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "numCost", queryValueMapFilter(query, otherValueMap, "num_cost"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "vchWaiter", queryValueMapFilter(query, otherValueMap, "vch_waiter"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "numTransService", queryValueMapFilter(query, otherValueMap, "num_trans_service"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);
    cJSON_help::_cJSON_AddValueToObject(pItem, "terminalCode", queryValueMapFilter(query, otherValueMap, "terminal_code"));
    return pItem;
}

cJSON *w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_master(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap)
{
    cJSON *pItem = cJSON_CreateObject();
    cJSON_help::_cJSON_AddValueToObject(pItem, "omChBillNo", queryValueMapFilter(query, otherValueMap, "ch_billno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "omChState", queryValueMapFilter(query, otherValueMap, "ch_state"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "omVchOperId", queryValueMapFilter(query, otherValueMap, "vch_operID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "omDtOperDate", queryValueMapFilter(query, otherValueMap, "dt_operdate"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "omVchOperId2", queryValueMapFilter(query, otherValueMap, "vch_operID2"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "omDtOperDate2", queryValueMapFilter(query, otherValueMap, "dt_operdate2"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "omVchMemo", queryValueMapFilter(query, otherValueMap, "vch_memo"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);
    cJSON_help::_cJSON_AddValueToObject(pItem, "terminalCode", queryValueMapFilter(query, otherValueMap, "terminal_code"));
    return pItem;
}

cJSON *w_sys_manage_cloudsync_with_time_running::q_u_get_piece_t_m_deposit(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap)
{
    cJSON *pItem = cJSON_CreateObject();
    cJSON_help::_cJSON_AddValueToObject(pItem, "mdIntFlowId", queryValueMapFilter(query, otherValueMap, "int_flowID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mdVchMemberNo", queryValueMapFilter(query, otherValueMap, "vch_memberno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mdNumDeposit", queryValueMapFilter(query, otherValueMap, "num_deposit"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mdNumRealaMount", queryValueMapFilter(query, otherValueMap, "num_realamount"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mdChDepositMode", queryValueMapFilter(query, otherValueMap, "ch_deposit_mode"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mdChPayMode", queryValueMapFilter(query, otherValueMap, "ch_pay_mode"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mdVchOperId", queryValueMapFilter(query, otherValueMap, "vch_operID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "dtOperDate", queryValueMapFilter(query, otherValueMap, "dt_operdate"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mdVchMemo", queryValueMapFilter(query, otherValueMap, "vch_memo"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mdRemainAmt", queryValueMapFilter(query, otherValueMap, "remain_amt"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);
    cJSON_help::_cJSON_AddValueToObject(pItem, "terminalCode", queryValueMapFilter(query, otherValueMap, "terminal_code"));

    return pItem;
}

cJSON *w_sys_manage_cloudsync_with_time_running::q_u_get_piece_t_m_point(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap)
{
    cJSON *pItem = cJSON_CreateObject();
    cJSON_help::_cJSON_AddValueToObject(pItem, "mpIntFlowId", queryValueMapFilter(query, otherValueMap, "int_flowID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mpVchMemberNo", queryValueMapFilter(query, otherValueMap, "vch_memberno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mpChType", queryValueMapFilter(query, otherValueMap, "ch_type"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mpChPayNo", queryValueMapFilter(query, otherValueMap, "ch_payno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mpNumPoint", queryValueMapFilter(query, otherValueMap, "num_point"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mpVchMemo", queryValueMapFilter(query, otherValueMap, "vch_memo"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mpVchOperId", queryValueMapFilter(query, otherValueMap, "vch_operID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mpDtOperDate", queryValueMapFilter(query, otherValueMap, "dt_operdate"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);
    cJSON_help::_cJSON_AddValueToObject(pItem, "terminalCode", queryValueMapFilter(query, otherValueMap, "terminal_code"));

    return pItem;
}

cJSON *w_sys_manage_cloudsync_with_time_running::q_u_get_piece_t_m_pay(lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap)
{
    cJSON *pItem = cJSON_CreateObject();
    cJSON_help::_cJSON_AddValueToObject(pItem, "intFlowid", queryValueMapFilter(query, otherValueMap, "int_flowID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mpVchMemberNo", queryValueMapFilter(query, otherValueMap, "vch_memberno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mpChPayNo", queryValueMapFilter(query, otherValueMap, "ch_payno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mpChPayModeNo", queryValueMapFilter(query, otherValueMap, "ch_paymodeno"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "mpNumAmount", queryValueMapFilter(query, otherValueMap, "num_amount"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mpNumPay", queryValueMapFilter(query, otherValueMap, "num_pay"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "mpVchOperId", queryValueMapFilter(query, otherValueMap, "vch_operID"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mpDtOperDate", queryValueMapFilter(query, otherValueMap, "dt_operdate"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mpChBalanceNo", queryValueMapFilter(query, otherValueMap, "ch_balanceno"));
    cJSON_help::_cJSON_AddValueToObject(pItem, "mpIntCheckFlow", queryValueMapFilter(query, otherValueMap, "int_checkflow"));

    cJSON_help::_cJSON_AddValueToObject(pItem, "orgCode", orgCode);
    cJSON_help::_cJSON_AddValueToObject(pItem, "terminalCode", queryValueMapFilter(query, otherValueMap, "terminal_code"));

    return pItem;
}

cJSON *w_sys_manage_cloudsync_with_time_running::q_u_get_piece_(const QString &tablename, lds_query_hddpos  &query, const QString &orgCode, const QMap<QString, QVariant> &otherValueMap)
{
    if(tablename == "cey_u_checkout_detail")  return q_u_get_piece_cey_u_checkout_detail(query, orgCode);
    if(tablename == "cey_u_checkout_master")  return q_u_get_piece_cey_u_checkout_master(query, orgCode);
    if(tablename == "cey_u_orderdish")  return q_u_get_piece_cey_u_orderdish(query, orgCode);
    if(tablename == "cey_u_service_low") return q_u_get_piece_cey_u_service_low(query, orgCode);
    if(tablename == "cey_u_orderdish_log")  return q_u_get_piece_cey_u_orderdish_log(query, orgCode);

    if(tablename == "cey_u_togo")  return q_u_get_piece_cey_u_togo(query, orgCode);
    if(tablename == "cey_u_table")  return q_u_get_piece_cey_u_table(query, orgCode);

    if(tablename == "t_m_deposit")  return q_u_get_piece_t_m_deposit(query, orgCode);
    if(tablename == "t_m_point")  return q_u_get_piece_t_m_point(query, orgCode);
    if(tablename == "t_m_pay")  return q_u_get_piece_t_m_pay(query, orgCode);

#ifdef QT_DEBUG
    qDebug() << "not found" << __LINE__ << tablename;
    QObject *o = 0;
    o->children();
#endif
    return 0;
}

cJSON *w_sys_manage_cloudsync_with_time_running::q_u_get_by_ch_payno(const QString &ch_payno, const QDateTime &pay_dt, const QString &orgCode)
{
    cJSON *root = q_u_get_void_flow_root();
    cJSON *root_sub_array;
    lds_query_hddpos  query;

    //x7
    q_u_get_by(root, ch_payno, orgCode);

    //x3
    root_sub_array = cJSON_GetObjectItem(root, "memberDeposit");
    query.execSelect(QString("select * from t_m_deposit where dt_operdate = '%1'  and  ifnull(ch_yun_already_keep,'') <> 'Y' ").arg(pay_dt.toString(dateFormatStr)));
    while(query.next()) {
        cJSON_AddItemToArray(root_sub_array, q_u_get_piece_t_m_deposit(query, orgCode));
    }

    root_sub_array = cJSON_GetObjectItem(root, "memberPointList");
    query.execSelect(QString("select * from t_m_point where dt_operdate = '%1'  and  ifnull(ch_yun_already_keep,'') <> 'Y' ").arg(pay_dt.toString(dateFormatStr)));
    while(query.next()) {
        cJSON_AddItemToArray(root_sub_array, q_u_get_piece_t_m_point(query, orgCode));
    }

    root_sub_array = cJSON_GetObjectItem(root, "memberPay");
    query.execSelect(QString("select * from t_m_pay where dt_operdate = '%1'  and  ifnull(ch_yun_already_keep,'') <> 'Y' ").arg(pay_dt.toString(dateFormatStr)));
    while(query.next()) {
        cJSON_AddItemToArray(root_sub_array, q_u_get_piece_t_m_pay(query, orgCode));
    }

    return root;
}

bool w_sys_manage_cloudsync_with_time_running:: q_u_put_by_ch_payno(const QString &ch_payno, const QDateTime &pay_dt)
{
    lds_query_hddpos  query;
    //x7
    if(!q_u_put_by(ch_payno)) return false;

    //x3
    if(!query.execUpdate("t_m_deposit", "ch_yun_already_keep", "Y", QString("dt_operdate = '%1' and ifnull(ch_yun_already_keep,'') <> 'Y' ").arg(pay_dt.toString(dateFormatStr)))) return false;
    if(!query.execUpdate("t_m_point",   "ch_yun_already_keep", "Y", QString("dt_operdate = '%1' and ifnull(ch_yun_already_keep,'') <> 'Y' ").arg(pay_dt.toString(dateFormatStr)))) return false;
    if(!query.execUpdate("t_m_pay",     "ch_yun_already_keep", "Y", QString("dt_operdate = '%1' and ifnull(ch_yun_already_keep,'') <> 'Y' ").arg(pay_dt.toString(dateFormatStr)))) return false;

    return true;

}


void w_sys_manage_cloudsync_with_time_running::q_u_get_by(cJSON *root, const QString &ch_payno, const QString &orgCode)
{
    cJSON *root_sub_array;
    lds_query_hddpos  query;

    ///x7
    //        cey_u_checkout_detail-----------------------checkOutDetail
    root_sub_array = cJSON_GetObjectItem(root, "checkOutDetail");
    query.execSelect(QString("select * from cey_u_checkout_detail where ch_payno = '%1' and ifnull(ch_yun_already_keep,'') <> 'Y' ").arg(ch_payno));
    while(query.next()) {
        cJSON_AddItemToArray(root_sub_array, q_u_get_piece_("cey_u_checkout_detail", query, orgCode));
    }
    //        cey_u_checkout_master-----------------------checkOutMaster

    root_sub_array = cJSON_GetObjectItem(root, "checkOutMaster");
    query.execSelect(QString("select * from cey_u_checkout_master where ch_payno = '%1'  and ifnull(ch_yun_already_keep,'') <> 'Y' ").arg(ch_payno));
    while(query.next()) {
        cJSON_AddItemToArray(root_sub_array, q_u_get_piece_("cey_u_checkout_master", query, orgCode));
    }
    //        cey_u_orderdish-----------------------orderDish

    root_sub_array = cJSON_GetObjectItem(root, "orderDish");
    query.execSelect(QString("select * from cey_u_orderdish where ch_payno = '%1'  and ifnull(ch_yun_already_keep,'') <> 'Y' ").arg(ch_payno));
    while(query.next()) {
        cJSON_AddItemToArray(root_sub_array, q_u_get_piece_("cey_u_orderdish", query, orgCode));
    }
    //        cey_u_service_low-----------------------serviceLow

    root_sub_array = cJSON_GetObjectItem(root, "serviceLow");
    query.execSelect(QString("select * from cey_u_service_low where ch_payno = '%1'  and ifnull(ch_yun_already_keep,'') <> 'Y' ").arg(ch_payno));
    while(query.next()) {
        cJSON_AddItemToArray(root_sub_array, q_u_get_piece_("cey_u_service_low", query, orgCode));
    }
    //        cey_u_orderdish_log-----------------------orderDishLog

    root_sub_array = cJSON_GetObjectItem(root, "orderDishLog");
    query.execSelect(QString("select * from cey_u_orderdish_log where ch_payno = '%1'  and ifnull(ch_yun_already_keep,'') <> 'Y' ").arg(ch_payno));
    while(query.next()) {
        cJSON_AddItemToArray(root_sub_array, q_u_get_piece_("cey_u_orderdish_log", query, orgCode));
    }
    //        cey_u_togo-----------------------toGo

    root_sub_array = cJSON_GetObjectItem(root, "toGo");
    query.execSelect(QString("select * from cey_u_togo where ch_payno = '%1'  and ifnull(ch_yun_already_keep,'') <> 'Y' ").arg(ch_payno));
    while(query.next()) {
        cJSON_AddItemToArray(root_sub_array, q_u_get_piece_("cey_u_togo", query, orgCode));
    }
    //        cey_u_table-----------------------tableRecordsConsumption

    root_sub_array = cJSON_GetObjectItem(root, "tableRecordsConsumption");
    query.execSelect(QString("select * from cey_u_table where ch_payno = '%1'  and ifnull(ch_yun_already_keep,'') <> 'Y' ").arg(ch_payno));
    while(query.next()) {
        cJSON_AddItemToArray(root_sub_array, q_u_get_piece_("cey_u_table", query, orgCode));
    }
}

bool w_sys_manage_cloudsync_with_time_running::q_u_put_by(const QString &ch_payno)
{
    lds_query_hddpos  query;

    //    cey_u_checkout_detail-----------------------checkOutDetail
    if(!query.execUpdate("cey_u_checkout_detail", "ch_yun_already_keep", "Y", QString("ch_payno = '%1'  ").arg(ch_payno))) return false;
    //    cey_u_checkout_master-----------------------checkOutMaster
    if(!query.execUpdate("cey_u_checkout_master", "ch_yun_already_keep", "Y", QString("ch_payno = '%1'  ").arg(ch_payno))) return false;
    //    cey_u_orderdish-----------------------orderDish
    if(!query.execUpdate("cey_u_orderdish", "ch_yun_already_keep", "Y",       QString("ch_payno = '%1'  ").arg(ch_payno))) return false;
    //    cey_u_service_low-----------------------serviceLow
    if(!query.execUpdate("cey_u_service_low", "ch_yun_already_keep", "Y",     QString("ch_payno = '%1'  ").arg(ch_payno))) return false;
    //    cey_u_orderdish_log-----------------------orderDishLog
    if(!query.execUpdate("cey_u_orderdish_log", "ch_yun_already_keep", "Y",   QString("ch_payno = '%1'  ").arg(ch_payno))) return false;
    //    cey_u_togo-----------------------toGo
    if(!query.execUpdate("cey_u_togo", "ch_yun_already_keep", "Y",            QString("ch_payno = '%1'  ").arg(ch_payno))) return false;
    //    cey_u_table-----------------------tableRecordsConsumption
    if(!query.execUpdate("cey_u_table", "ch_yun_already_keep", "Y",           QString("ch_payno = '%1'  ").arg(ch_payno))) return false;

    return true;
}

void w_sys_manage_cloudsync_with_time_running::q_u_get_limit500_in_transaction(cJSON *root, const QString &orgCode, QString &dt_operdate)
{
    dt_operdate = "";
    cJSON *root_sub_array;
    lds_query_hddpos  query;
    QString limit100 = "limit 100";
#ifdef QT_DEBUG
    limit100 = "limit 5";
#endif
    ///x7
    //        cey_u_checkout_detail-----------------------checkOutDetail
    root_sub_array = cJSON_GetObjectItem(root, "checkOutDetail");
    query.execSelect(QString("select * from cey_u_checkout_detail where ifnull(ch_yun_already_keep,'') <> 'Y'  " + limit100));
    while(query.next()) {
        cJSON_AddItemToArray(root_sub_array, q_u_get_piece_("cey_u_checkout_detail", query, orgCode));
    }
    //        cey_u_checkout_master-----------------------checkOutMaster

    root_sub_array = cJSON_GetObjectItem(root, "checkOutMaster");
    query.execSelect(QString("select * from cey_u_checkout_master where ifnull(ch_yun_already_keep,'') <> 'Y'  " + limit100));
    while(query.next()) {
        if(dt_operdate.length() ==0 ) {
            dt_operdate = query.recordValue("dt_operdate").toDateTime().toString(yyyyMMddhhmmss);
        }
        if(query.recordValue("ch_member_belong_state").toString() == "Y") {
            //通过触发器1-》Y
            if(!lds_query_hddpos::updateTable("cey_u_checkout_master", qrtVariantPairList() << qrtVariantPair("ch_yun_already_keep",  "1"), QString("ch_payno = '%1' ").arg(query.recordValue("ch_payno").toString()))) {
                qDebug() << __FILE__ << __LINE__ << __FUNCTION__ ;
                QObject *o = 0;
                o->children();
            }
        }
        cJSON_AddItemToArray(root_sub_array, q_u_get_piece_("cey_u_checkout_master", query, orgCode));
    }
    //        cey_u_orderdish-----------------------orderDish

    root_sub_array = cJSON_GetObjectItem(root, "orderDish");
    query.execSelect(QString("select * from cey_u_orderdish where ifnull(ch_payno, '') <> '' and   ifnull(ch_yun_already_keep,'') <> 'Y'  " + limit100));
    while(query.next()) {
        cJSON_AddItemToArray(root_sub_array, q_u_get_piece_("cey_u_orderdish", query, orgCode));
    }
    //        cey_u_service_low-----------------------serviceLow

    root_sub_array = cJSON_GetObjectItem(root, "serviceLow");
    query.execSelect(QString("select * from cey_u_service_low where ifnull(ch_yun_already_keep,'') <> 'Y'  " + limit100));
    while(query.next()) {
        cJSON_AddItemToArray(root_sub_array, q_u_get_piece_("cey_u_service_low", query, orgCode));
    }
    //        cey_u_orderdish_log-----------------------orderDishLog

    root_sub_array = cJSON_GetObjectItem(root, "orderDishLog");
    query.execSelect(QString("select * from cey_u_orderdish_log where ifnull(ch_yun_already_keep,'') <> 'Y'  " + limit100));
    while(query.next()) {
        cJSON_AddItemToArray(root_sub_array, q_u_get_piece_("cey_u_orderdish_log", query, orgCode));
    }
    //        cey_u_togo-----------------------toGo

    root_sub_array = cJSON_GetObjectItem(root, "toGo");
    query.execSelect(QString("select * from cey_u_togo where ifnull(ch_yun_already_keep,'') <> 'Y'  " + limit100));
    while(query.next()) {
        cJSON_AddItemToArray(root_sub_array, q_u_get_piece_("cey_u_togo", query, orgCode));
    }
    //        cey_u_table-----------------------tableRecordsConsumption

    root_sub_array = cJSON_GetObjectItem(root, "tableRecordsConsumption");
    query.execSelect(QString("select * from cey_u_table where ifnull(ch_payno, '') <> '' and  ifnull(ch_yun_already_keep,'') <> 'Y' " + limit100));
    while(query.next()) {
        cJSON_AddItemToArray(root_sub_array, q_u_get_piece_("cey_u_table", query, orgCode));
    }

    ///x3
    //t_m_deposit
    root_sub_array = cJSON_GetObjectItem(root, "memberDeposit");
    query.execSelect(QString("select * from t_m_deposit where ifnull(ch_yun_already_keep,'') <> 'Y' " + limit100));
    while(query.next()) {
        cJSON_AddItemToArray(root_sub_array, q_u_get_piece_("t_m_deposit", query, orgCode));
    }
    //t_m_point
    root_sub_array = cJSON_GetObjectItem(root, "memberPointList");
    query.execSelect(QString("select * from t_m_point where ifnull(ch_yun_already_keep,'') <> 'Y' " + limit100));
    while(query.next()) {
        cJSON_AddItemToArray(root_sub_array, q_u_get_piece_("t_m_point", query, orgCode));
    }
    //t_m_pay
    root_sub_array = cJSON_GetObjectItem(root, "memberPay");
    query.execSelect(QString("select * from t_m_pay where ifnull(ch_yun_already_keep,'') <> 'Y' " + limit100));
    while(query.next()) {
        cJSON_AddItemToArray(root_sub_array, q_u_get_piece_("t_m_pay", query, orgCode));
    }
}

void w_sys_manage_cloudsync_with_time_running::q_u_get_limit500_in_transaction(QSharedPointer<cJSON> root, const QString &orgCode, QString &dt_operdate)
{
    return q_u_get_limit500_in_transaction(root.data(), orgCode, dt_operdate);
}

bool w_sys_manage_cloudsync_with_time_running::q_u_put_limit1000(cJSON *root)
{
    /*
//x7
    cey_u_checkout_detail-----------------------checkOutDetail-----------int_flowID-----------codIntFlowId
    cey_u_checkout_master-----------------------checkOutMaster-----------ch_payno-----------comChPayNo
    cey_u_orderdish-----------------------orderDish-----------int_flowID-----------bdIntFlowId
    cey_u_service_low-----------------------serviceLow-----------ch_payno-----------slChPayNo
    cey_u_orderdish_log-----------------------orderDishLog-----------int_flowID-----------odlIntFlowId
    cey_u_togo-----------------------toGo-----------ch_togono-----------chTogono
    cey_u_table-----------------------tableRecordsConsumption-----------ch_billno-----------chBillno
    //x3
   t_m_deposit-----------------------memberDeposit-----------int_flowID-----------mdIntFlowId
   t_m_point-----------------------memberPointList-----------int_flowID-----------mpIntFlowId
   t_m_pay-----------------------memberPay-----------int_flowID-----------intFlowid
    */

    lds_query_hddpos  query;
    cJSON *root_sub_array;
    //cey_u_checkout_detail-----------------------checkOutDetail-----------int_flowID-----------codIntFlowId
    root_sub_array = cJSON_GetObjectItem(root, "checkOutDetail");
    for(int k = 0, count = cJSON_GetArraySize(root_sub_array); k < count; k++) {
        cJSON *item = cJSON_GetArrayItem(root_sub_array, k);
        if(!query.execUpdate("cey_u_checkout_detail", "ch_yun_already_keep", "Y", QString("int_flowID = '%1' ")
                             .arg(cJSON_help::getcJSONvalue(item, "codIntFlowId").toString()))) return false;
    }
    //    cey_u_checkout_master-----------------------checkOutMaster-----------ch_payno-----------comChPayNo
    root_sub_array = cJSON_GetObjectItem(root, "checkOutMaster");
    for(int k = 0, count = cJSON_GetArraySize(root_sub_array); k < count; k++) {
        cJSON *item = cJSON_GetArrayItem(root_sub_array, k);
        if(!query.execUpdate("cey_u_checkout_master", "ch_yun_already_keep", "Y", QString("ch_payno = '%1' ")
                             .arg(cJSON_help::getcJSONvalue(item, "comChPayNo").toString()))) return false;
    }
    //    cey_u_orderdish-----------------------orderDish-----------int_flowID-----------bdIntFlowId
    root_sub_array = cJSON_GetObjectItem(root, "orderDish");
    for(int k = 0, count = cJSON_GetArraySize(root_sub_array); k < count; k++) {
        cJSON *item = cJSON_GetArrayItem(root_sub_array, k);
        if(!query.execUpdate("cey_u_orderdish", "ch_yun_already_keep", "Y", QString("int_flowID = '%1' ")
                             .arg(cJSON_help::getcJSONvalue(item, "bdIntFlowId").toString()))) return false;
    }
    //    cey_u_service_low-----------------------serviceLow-----------ch_payno-----------slChPayNo
    root_sub_array = cJSON_GetObjectItem(root, "serviceLow");
    for(int k = 0, count = cJSON_GetArraySize(root_sub_array); k < count; k++) {
        cJSON *item = cJSON_GetArrayItem(root_sub_array, k);
        if(!query.execUpdate("cey_u_service_low", "ch_yun_already_keep", "Y", QString("ch_payno = '%1' ")
                             .arg(cJSON_help::getcJSONvalue(item, "slChPayNo").toString()))) return false;
    }
    //    cey_u_orderdish_log-----------------------orderDishLog-----------int_flowID-----------odlIntFlowId
    root_sub_array = cJSON_GetObjectItem(root, "orderDishLog");
    for(int k = 0, count = cJSON_GetArraySize(root_sub_array); k < count; k++) {
        cJSON *item = cJSON_GetArrayItem(root_sub_array, k);
        if(!query.execUpdate("cey_u_orderdish_log", "ch_yun_already_keep", "Y", QString("int_flowID = '%1' ")
                             .arg(cJSON_help::getcJSONvalue(item, "odlIntFlowId").toString()))) return false;
    }
    //    cey_u_togo-----------------------toGo-----------ch_togono-----------chTogono
    root_sub_array = cJSON_GetObjectItem(root, "toGo");
    for(int k = 0, count = cJSON_GetArraySize(root_sub_array); k < count; k++) {
        cJSON *item = cJSON_GetArrayItem(root_sub_array, k);
        if(!query.execUpdate("cey_u_togo", "ch_yun_already_keep", "Y", QString("ch_togono = '%1' ")
                             .arg(cJSON_help::getcJSONvalue(item, "chTogono").toString()))) return false;
    }
    //    cey_u_table-----------------------tableRecordsConsumption-----------ch_billno-----------chBillno
    root_sub_array = cJSON_GetObjectItem(root, "tableRecordsConsumption");
    for(int k = 0, count = cJSON_GetArraySize(root_sub_array); k < count; k++) {
        cJSON *item = cJSON_GetArrayItem(root_sub_array, k);
        if(!query.execUpdate("cey_u_table", "ch_yun_already_keep", "Y", QString("ch_billno = '%1' ")
                             .arg(cJSON_help::getcJSONvalue(item, "chBillno").toString()))) return false;
    }
    //    //x3
    //   t_m_deposit-----------------------memberDeposit-----------int_flowID-----------mdIntFlowId
    root_sub_array = cJSON_GetObjectItem(root, "memberDeposit");
    for(int k = 0, count = cJSON_GetArraySize(root_sub_array); k < count; k++) {
        cJSON *item = cJSON_GetArrayItem(root_sub_array, k);
        if(!query.execUpdate("t_m_deposit", "ch_yun_already_keep", "Y", QString("int_flowID = '%1' ")
                             .arg(cJSON_help::getcJSONvalue(item, "mdIntFlowId").toString()))) return false;
    }
    //   t_m_point-----------------------memberPointList-----------int_flowID-----------mpIntFlowId
    root_sub_array = cJSON_GetObjectItem(root, "memberPointList");
    for(int k = 0, count = cJSON_GetArraySize(root_sub_array); k < count; k++) {
        cJSON *item = cJSON_GetArrayItem(root_sub_array, k);
        if(!query.execUpdate("t_m_point", "ch_yun_already_keep", "Y", QString("int_flowID = '%1' ")
                             .arg(cJSON_help::getcJSONvalue(item, "mpIntFlowId").toString()))) return false;
    }
    //   t_m_pay-----------------------memberPay-----------int_flowID-----------intFlowid
    root_sub_array = cJSON_GetObjectItem(root, "memberPay");
    for(int k = 0, count = cJSON_GetArraySize(root_sub_array); k < count; k++) {
        cJSON *item = cJSON_GetArrayItem(root_sub_array, k);
        if(!query.execUpdate("t_m_pay", "ch_yun_already_keep", "Y", QString("int_flowID = '%1' ")
                             .arg(cJSON_help::getcJSONvalue(item, "intFlowid").toString()))) return false;
    }
    return  true;
}

bool w_sys_manage_cloudsync_with_time_running::q_u_put_limit1000(QSharedPointer<cJSON> root)
{
    return q_u_put_limit1000(root.data());
}

bool w_sys_manage_cloudsync_with_time_running::q_u_get_yun_already_keep()
{
    lds_query_hddpos  query;
    query.execSelect("select dt_operdate,  int_flowID from cey_u_checkout_detail where ifnull(ch_yun_already_keep,'') <> 'Y' limit 1;"); if(query.next()) { return false; }
    query.execSelect("select dt_operdate,  ch_payno from cey_u_checkout_master where ifnull(ch_yun_already_keep,'') <> 'Y' limit 1;"); if(query.next()) {return false; }
    query.execSelect("select dt_operdate,  int_flowID from cey_u_orderdish where ifnull(ch_payno, '') <> '' and  ifnull(ch_yun_already_keep,'') <> 'Y' limit 1;"); if(query.next()) {return false; }
    query.execSelect("select dt_operdate,  ch_payno from cey_u_service_low where ifnull(ch_yun_already_keep,'') <> 'Y' limit 1;"); if(query.next()) {return false; }
    query.execSelect("select dt_operdate,  int_flowID from cey_u_orderdish_log where ifnull(ch_yun_already_keep,'') <> 'Y' limit 1;"); if(query.next()) {return false; }

    query.execSelect("select dt_operdate,  ch_togono from cey_u_togo where ifnull(ch_yun_already_keep,'') <> 'Y' limit 1;"); if(query.next()) {return false; }
    query.execSelect("select dt_operdate,  ch_billno from cey_u_table where ifnull(ch_payno, '') <> '' and  ifnull(ch_yun_already_keep,'') <> 'Y' limit 1;"); if(query.next()) {return false; }

    query.execSelect("select dt_operdate,  int_flowID from t_m_deposit where ifnull(ch_yun_already_keep,'') <> 'Y' limit 1;"); if(query.next()) {return false; }
    query.execSelect("select dt_operdate,  int_flowID from t_m_point where ifnull(ch_yun_already_keep,'') <> 'Y' limit 1;"); if(query.next()) {return false; }
    query.execSelect("select dt_operdate,  int_flowID from t_m_pay where ifnull(ch_yun_already_keep,'') <> 'Y' limit 1;"); if(query.next()) {return false; }

    return true;
}

cJSON *w_sys_manage_cloudsync_with_time_running::q_bt_get_table_cey(QObject *parent_nozero_trigger_slot, bool skip_ch_yun_already_keep_Y)
{
    cJSON *root = cJSON_CreateObject();
    lds_query_hddpos  query;
    lds_query_hddpos  query2;
    QString vch_sqltablename_yun;
    QString tablename;
    QString vch_sqltablename_key_name;
    qlonglong index;
    query.execSelect("select * from cey_web_table_version ");

    while(query.next()) {

        index = 0;
        vch_sqltablename_yun = query.recordValue("vch_sqltablename_yun").toString();
        tablename = query.recordValue("vch_sqltablename").toString();
        vch_sqltablename_key_name = query.recordValue("vch_sqltablename_key_name").toString();
        QString sql_where = " where ";
        QStringList keys = vch_sqltablename_key_name.split(",");
        foreach(const QString &k, keys) {
            sql_where += QString(" ifnull(%1, '') <> '' ").arg(k);
            sql_where += "and";
        }
        if(sql_where.endsWith("and")) sql_where.chop(3);


        cJSON *root_sub_array = cJSON_CreateArray();
        //
        if(skip_ch_yun_already_keep_Y) {
            query2.execSelect(QString("select * from %1 %2").arg(tablename).arg(sql_where)
                              + " limit 0, 1000 ");
        } else {
            query2.execSelect(QString("select * from %1 %2 and ifnull(ch_yun_already_keep,'') <> 'Y' ").arg(tablename).arg(sql_where));
        }
        /**/        if(parent_nozero_trigger_slot)QMetaObject::invokeMethod(parent_nozero_trigger_slot, "updateGoing",Qt::QueuedConnection,
                                                                            Q_ARG(QString, QString("%1-%2").arg(QObject::tr("获取上传数据") ).arg(tablename)),
                                                                            Q_ARG(int, keep_going++)
                                                                            );
        while(query2.next()) {
            if("cey_bt_dish_warn" == tablename)
                break;
            index ++;
            cJSON *item = q_bt_get_piece_(tablename, query2, orgCode, 0);//正常
            cJSON_AddItemToArray(root_sub_array, item);

            /**/        if(parent_nozero_trigger_slot)QMetaObject::invokeMethod(parent_nozero_trigger_slot, "updateGoing",Qt::QueuedConnection,
                                                                                Q_ARG(QString, QString("%1-%2-%3").arg(QObject::tr("获取上传数据") ).arg(index).arg(tablename))
                                                                                );
        }
        //
        query2.execSelect(q_bt_get_del_sql(tablename, skip_ch_yun_already_keep_Y));
        while(query2.next()) {
            if("cey_bt_dish_warn" == tablename)
                break;
            index ++;
            cJSON *item = q_bt_get_piece_(tablename, query2, orgCode, -1);//删除
            cJSON_AddItemToArray(root_sub_array, item);

            /**/        if(parent_nozero_trigger_slot)QMetaObject::invokeMethod(parent_nozero_trigger_slot, "updateGoing",Qt::QueuedConnection,
                                                                                Q_ARG(QString, QString("%1-%2-%3").arg(QObject::tr("获取上传数据-删除") ).arg(index).arg(tablename))
                                                                                );
        }
        cJSON_AddItemToObject(root, vch_sqltablename_yun.toLocal8Bit(), root_sub_array);
        //
    }
    ///单独设置会员参数

    tablename = "t_m_parameter";
    /**/        if(parent_nozero_trigger_slot)QMetaObject::invokeMethod(parent_nozero_trigger_slot, "updateGoing",Qt::QueuedConnection,
                                                                        Q_ARG(QString, QString("%1-%2").arg(QObject::tr("获取上传数据") ).arg(tablename)),
                                                                        Q_ARG(int, keep_going++)
                                                                        );
    if(q_m_parameter_get_ready_total() > 0) {
        cJSON *root_sub_array = cJSON_CreateArray();
        query.execSelect("select * from t_m_parameter;");
        while (query.next()) {
            index ++;

            cJSON *item = cJSON_CreateObject();
            cJSON_help::_cJSON_AddValueToObject(item, "vchParameter", query.recordValue("vch_parameter"));
            cJSON_help::_cJSON_AddValueToObject(item, "vchValue", query.recordValue("vch_value"));
            cJSON_help::_cJSON_AddValueToObject(item, "vchExplain", query.recordValue("vch_explain"));


            cJSON_help::_cJSON_AddValueToObject(item, "orgCode", orgCode);

            cJSON_AddItemToArray(root_sub_array, item);

            /**/        if(parent_nozero_trigger_slot)QMetaObject::invokeMethod(parent_nozero_trigger_slot, "updateGoing",Qt::QueuedConnection,
                                                                                Q_ARG(QString, QString("%1-%2-%3").arg(QObject::tr("获取上传数据") ).arg(index).arg(tablename))
                                                                                );
        }

        cJSON_AddItemToObject(root, "memberParameterList", root_sub_array);
    }
    //


    ///单独设置促销
    index ++;
    cJSON_AddItemToObject(root, "dishSysParameter", w_sys_manage::q_bt_get_piece_promotion());
    ///～单独设置促销
    return root;
}

void w_sys_manage_cloudsync_with_time_running::took()
{
    data_run(_flag);
}

void w_sys_manage_cloudsync_with_time_running::tocancel()
{
    if(public_sql::save_login->yun_flow_upload_polling->isRunning()) {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("已上传的数据不会删除"), QObject::tr("确认"), QObject::tr("取消"))) return;
        public_sql::save_login->yun_flow_upload_polling->manual_stop();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作取消"));
    }
    this->reject();
}

void w_sys_manage_cloudsync_with_time_running::data_run(cloudFlags f)
{
    keep_going = 0;
    total_going = 0;
    lds_query_hddpos  query;
    QString httpkey;
    QMap<QString, QString> headInfo;

    if(cloud_up & f) {
        total_going += 23;
    }
    if(cloud_down & f) {
        total_going += 25;
    }
    if(f & cloud_up) {
        bool skip_ch_yun_already_keep_Y = false;
        if(cloud_up == f) {
            skip_ch_yun_already_keep_Y = true;
        }
        //!基本资料上传
        updateGoing(QObject::tr("获取上传数据"), keep_going++);
        httpDataAutoDel json_yun_parameter = q_bt_get_table_cey(this, skip_ch_yun_already_keep_Y);
        QByteArray param = json_yun_parameter.jsonToByteArray();

        headInfo.clear();
        headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
        headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
        headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));

        httpkey = http_www_yqspos_com +"/arm/dish-base/add-rest";
        updateGoing("开始上传", keep_going++);
        json_yun_parameter = w_sys_manage_cloudsync::get_yun_httppos(this, httpkey, param, headInfo,QObject::tr("云同步"));
        if(200 != json_yun_parameter.httpStatus) {
            updateGoing(QObject::tr("操作失败"));
            return;
        }
        ///单独设置库存
        if(!q_st_upload_material_num(query, n_func::f_get_sysparm("orgCode"))) {
            updateGoing(QObject::tr("操作失败") + "q_st_upload_material_num");
            return;
        }
        ///～单独设置库存
        updateGoing("上传完成", keep_going++);
        //!~基本资料上传
    }
    //<< "基本资料上传" << keep_going;
    if(f & cloud_down) {
        //!基本资料下载 19基础资料+1会员设置
        lds_query_hddpos::tran_saction();
        if(f == cloud_down) {//仅仅下载, 则删除本地
            lds_query_hddpos  query;
            lds_query_hddpos  query2;
            query.execSelect("select vch_sqltablename from cey_web_table_version");
            while(query.next()) {
                if(!query2.execDelete(query.recordValue("vch_sqltablename").toString())) {
                    lds_query_hddpos::roll_back();
                    updateGoing(QObject::tr("操作失败"));
                    return;
                }
            }
            if(!query.execUpdate("cey_web_table_version", "int_version", 0, "")) {
                lds_query_hddpos::roll_back();
                updateGoing(QObject::tr("操作失败"));
                return;
            }
        }
        updateGoing(QObject::tr("获取本地版本号"), keep_going++);
        QByteArray  headerversion;
        httpDataAutoDel item_table_version = q_bt_get_table_version_array();
        headerversion = item_table_version.jsonToByteArray();

        //        headInfo.insert("machineCode","");
        headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
        headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
        headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
        headInfo.insert("version",headerversion.simplified());

        httpkey = http_www_yqspos_com +"/arm/dish-base/get-rest";
        httpDataAutoDel json_yun_parameter=w_sys_manage_cloudsync::get_yun_httpget(this, httpkey, headInfo,QObject::tr("云同步"));

        if(200 != json_yun_parameter.httpStatus) {
            lds_query_hddpos::roll_back();
            updateGoing(QObject::tr("操作失败"));
            return;
        }
        updateGoing("更新本地", keep_going++);
        if(!q_bt_put_table_cey(this, json_yun_parameter.json)) {//包括会员设置
            lds_query_hddpos::roll_back();
            updateGoing(QObject::tr("操作失败"));
            return;
        }

        updateGoing(QString("更新本地-%2").arg("版本号"), keep_going++);
        if(!q_bt_put_table_version(json_yun_parameter.json)) {
            lds_query_hddpos::roll_back();
            updateGoing(QObject::tr("操作失败"));
            return;
        }
        //更新组号
        query.execUpdate("cey_bt_dish", "ch_groupno", qrtFunc("ch_dishno"), " ifnull(ch_groupno, '') = '' ");


        //更新数据库上传状态
        updateGoing(QString("更新本地-%2").arg("上传状态"), keep_going++);
        if(!q_bt_put_ch_yun_already_keep_Y()) {
            lds_query_hddpos::roll_back();
            updateGoing(QObject::tr("操作失败"));
            return;
        }
        updateGoing("提交", keep_going++);
        lds_query_hddpos::com_mit();
        //!～基本资料下
    }
    if(f & cloud_flow_up) {
        flow_data_upload();
        return;
    }
    QTimer::singleShot(100, this, SLOT(accept()));
}

bool w_sys_manage_cloudsync_with_time_running::upload_data(bool skip_ch_yun_already_keep_Y)
{
    QNetworkReply *reply = 0;
    QByteArray readall;
    QNetworkRequest request;
    lds_query_hddpos  query;
    QNetworkAccessManager m;
    QEventLoop loop;
    QString vch_sqltablename_yun;
    QString tablename;
    QString vch_sqltablename_key_name;
    QString sql_where;
    QStringList keys;

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");
    request.setRawHeader("terminalCode", lds::terminalCode.toLocal8Bit());
    connect(&m, SIGNAL(finished(QNetworkReply*)),&loop, SLOT(quit()));


    query.execSelect("select * from cey_web_table_version ");
    while(query.next()) {
        vch_sqltablename_yun = query.recordValue("vch_sqltablename_yun").toString();
        tablename = query.recordValue("vch_sqltablename").toString();
        vch_sqltablename_key_name = query.recordValue("vch_sqltablename_key_name").toString();
        sql_where = " where ";
        keys = vch_sqltablename_key_name.split(",");
        foreach(const QString &k, keys) {
            sql_where += QString(" ifnull(%1, '') <> '' ").arg(k);
            sql_where += "and";
        }
        if(false == skip_ch_yun_already_keep_Y) {
            sql_where += " ifnull(ch_yun_already_keep,'') <> 'Y' and";
        }
        if(sql_where.endsWith("and"))
            sql_where.chop(3);

        //upload_sql
        if(upload_sql(m, loop, tablename, sql_where)) {

        }
    }
    return true;
}

bool w_sys_manage_cloudsync_with_time_running::upload_sql(QNetworkAccessManager &m, QEventLoop &loop, const QString &tablename, const QString &sql_where)
{
    lds_query_hddpos  query;
    int curstep;

    //沽清表不在处理
    if("cey_bt_dish_warn" == tablename)
        return true;

    //正常部分
    curstep = 0;
    query.execSelect(QString("select * from %1 %2").arg(tablename).arg(sql_where));
    while(query.next()) {

    }

    //删除部分
    return true;
}

void w_sys_manage_cloudsync_with_time_running::base_data_upload_thread()
{
    lds_query_hddpos  query;
    QString httpkey;
    QByteArray  headerversion;

    //!基本资料上传
    httpDataAutoDel json_yun_parameter = q_bt_get_table_cey(0, false);
    QByteArray param = json_yun_parameter.jsonToByteArray();

    QMap<QString, QString> headInfo;
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    httpkey = http_www_yqspos_com +"/arm/dish-base/add-rest";
    json_yun_parameter = w_sys_manage_cloudsync::get_yun_httppos(0, httpkey, param, headInfo, QObject::tr("云同步"));
    if(200 != json_yun_parameter.httpStatus) {
        return;
    }

    ///单独设置库存
    if(!q_st_upload_material_num(query, n_func::f_get_sysparm("orgCode"))) {
        return;
    }
    ///~单独设置库存
    //!~基本资料上传

    //!基本资料下
    httpDataAutoDel item_table_version = q_bt_get_table_version_array();
    headerversion = item_table_version.jsonToByteArray();
    headInfo.clear();
    //    headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    headInfo.insert("version",headerversion.simplified());

    httpkey = http_www_yqspos_com +"/arm/dish-base/get-rest";
    json_yun_parameter = w_sys_manage_cloudsync::get_yun_httpget(0, httpkey, headInfo, QObject::tr("云同步"));



    if(200 != json_yun_parameter.httpStatus) {
        return;
    }

    lds_query_hddpos::tran_saction();
    if(!q_bt_put_table_cey(0, json_yun_parameter.json)) {
        lds_query_hddpos::roll_back();

        return;
    }
    if(!q_bt_put_table_version(json_yun_parameter.json)) {
        lds_query_hddpos::roll_back();

        return;
    }
    //包括基础资料，会员设置，库存信息,促销
    if(!q_bt_put_ch_yun_already_keep_Y()) {
        lds_query_hddpos::roll_back();

        return;
    }

    lds_query_hddpos::com_mit();
    return;

    //!～基本资料下

}

//上传 18 + 1 下载18 + 1
void w_sys_manage_cloudsync_with_time_running::updateGoing(const QString &text, int step)
{
    //movie
    if(ui->label_loading->movie()) {
        ui->label_loading->movie()->jumpToNextFrame();
    }

    if(!ui->textEdit->isHidden()) {
        ui->textEdit->append(text);
        ui->textEdit->repaint();
    }
    ui->progressBar->setMaximum(total_going);//基础资料、会员参数、记录路径
    if(step != -1) {
        if(_flag & cloud_flow_up)//流水
            ui->progressBar->setValue(step);
        if(_flag & cloud_downup)//基础资料
            ui->progressBar->setValue(ui->progressBar->value() + 1);
        ui->progressBar->repaint();
    }
    ui->label_desc->setText(text);
    ui->label_desc->repaint();

    if(ui->progressBar->value() == ui->progressBar->maximum()) {
        //        ui->label_desc->setText("同步成功");
        ui->label_desc->repaint();
    }
}

void w_sys_manage_cloudsync_with_time_running::updateGoing(const QString &text, const QDateTime &dt)
{
    updateGoing(dt.toString(dateFormatStr) + text, -1);
}

void w_sys_manage_cloudsync_with_time_running::goingFinished()
{
    if(ui->label_loading->movie()) {
        ui->label_loading->movie()->stop();
    }
    ui->label_desc->setText("上传完成");
    ui->progressBar->setValue(10);
    ui->progressBar->setMaximum(10);
    QTimer::singleShot(1000, this, SLOT(accept()));
}

QVariant w_sys_manage_cloudsync_with_time_running::queryValueMapFilter(lds_query_hddpos  &query, const QMap<QString, QVariant> &otherValueMap, const QString &field_name)
{
    QVariant d = otherValueMap.value(field_name);
    if(false == d.isValid())
        d = query.recordValue(field_name);
    return d;
}

void w_sys_manage_cloudsync_with_time_running::flow_data_upload()
{
    public_sql::save_login->yun_flow_upload_polling->disconnect();
    connect(public_sql::save_login->yun_flow_upload_polling, SIGNAL(signalgoing(QString)), this,SLOT(updateGoing(QString)));
    connect(public_sql::save_login->yun_flow_upload_polling, SIGNAL(signalgoingFinished()), this,SLOT(goingFinished()));

    public_sql::save_login->yun_flow_upload_polling->start();
}

bool w_sys_manage_cloudsync_with_time_running::flow_data_clearAll_Y_transaction(QWidget *parent, lds_messagebox_loading_show *loading)
{
    /* x7
SELECT ch_payno FROM cey_u_checkout_detail;
SELECT ch_payno FROM cey_u_checkout_master;
SELECT ch_payno FROM cey_u_orderdish;
SELECT ch_payno FROM cey_u_service_low;
SELECT ch_payno FROM cey_u_orderdish_log;
SELECT ch_payno FROM cey_u_togo;
SELECT ch_payno FROM cey_u_table;

//x3
SELECT int_flowID FROM t_m_deposit;
SELECT int_flowID FROM t_m_point;
SELECT int_flowID FROM t_m_pay;
*/
    lds_query_hddpos::tran_saction();
    QString errstring;
    lds_query_hddpos  query;
    //x7
    if(!query.execUpdate("cey_u_checkout_detail", "ch_yun_already_keep", "N", "", &errstring)) { lds_query_hddpos::roll_back(); lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, errstring);  return false;  }
    if(!query.execUpdate("cey_u_checkout_master", "ch_yun_already_keep", "N", "", &errstring)) { lds_query_hddpos::roll_back(); lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, errstring);  return false;  }
    if(!query.execUpdate("cey_u_orderdish", "ch_yun_already_keep", "N", "", &errstring)) { lds_query_hddpos::roll_back(); lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, errstring);  return false;  }
    if(!query.execUpdate("cey_u_service_low", "ch_yun_already_keep", "N", "", &errstring)) { lds_query_hddpos::roll_back(); lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, errstring);  return false;  }
    if(!query.execUpdate("cey_u_orderdish_log", "ch_yun_already_keep", "N", "", &errstring)) { lds_query_hddpos::roll_back(); lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, errstring);  return false;  }

    if(!query.execUpdate("cey_u_togo", "ch_yun_already_keep", "N", "", &errstring)) { lds_query_hddpos::roll_back(); lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, errstring);  return false;  }
    if(!query.execUpdate("cey_u_table", "ch_yun_already_keep", "N", "", &errstring)) { lds_query_hddpos::roll_back(); lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, errstring);  return false;  }

    //x3
    if(!query.execUpdate("t_m_deposit", "ch_yun_already_keep", "N", "", &errstring)) { lds_query_hddpos::roll_back(); lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, errstring);  return false;  }
    if(!query.execUpdate("t_m_point", "ch_yun_already_keep", "N", "", &errstring)) { lds_query_hddpos::roll_back(); lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, errstring);  return false;  }
    if(!query.execUpdate("t_m_pay", "ch_yun_already_keep", "N", "", &errstring)) { lds_query_hddpos::roll_back(); lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, errstring);  return false;  }
    lds_query_hddpos::com_mit();
    return true;
}

bool w_sys_manage_cloudsync_with_time_running::flow_pay_data_upload(const QString &ch_payno, const QDateTime &pay_dt, const QString &orgCode, QString *errstring)
{
    w_sys_manage_cloudsync_transfer transfer;
    cJSON_help root(q_u_get_by_ch_payno(ch_payno, pay_dt, orgCode));
    QString retparam;

    if(!q_u_put_by_ch_payno(ch_payno, pay_dt)) return false;


    QMap<QString, QString> headInfo;

    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));


    QString httpkey = http_www_yqspos_com +"/arm/dish-flow/add-rest";
    QByteArray param = cJSON_help::getcJSONprint(root);
    httpDataAutoDel p = transfer.sendPost2(httpkey, param, headInfo,retparam, 5*1000);
    if(errstring) *errstring = p.message;
    if(p.json == 0) return false;

    return true;
}

void w_sys_manage_cloudsync_with_time_running::updateGoing(const QString &text)
{
    //    if(ui->label_loading->movie()
    //            && ui->label_loading->movie()->state() != QMovie::Running) {
    //        ui->label_loading->movie()->start();
    //    }
    updateGoing(text, -1);
}

w_sys_manage_cloudsync_with_time_check_state_thread::w_sys_manage_cloudsync_with_time_check_state_thread(QObject *parent)
    : lds_thread("是否需要数据同步", parent)
{

}

w_sys_manage_cloudsync_with_time_check_state_thread::~w_sys_manage_cloudsync_with_time_check_state_thread()
{

}

void w_sys_manage_cloudsync_with_time_check_state_thread::run()
{
    w_sys_manage_cloudsync_with_time_check_state_thread_event event;
    connect(this,SIGNAL(signalTrigger(QObject *)),&event, SLOT(to_check_state(QObject *)));
    this->exec();
}

void w_sys_manage_cloudsync_with_time_check_state_thread::emitsignalTrigger(QObject *parent)
{
    emit signalTrigger(parent);
}

w_sys_manage_cloudsync_with_time_check_state_thread_event::w_sys_manage_cloudsync_with_time_check_state_thread_event(QObject *parent)
    : QObject(parent)
{
}

w_sys_manage_cloudsync_with_time_check_state_thread_event::~w_sys_manage_cloudsync_with_time_check_state_thread_event()
{

}

void w_sys_manage_cloudsync_with_time_check_state_thread_event::to_check_state(QObject * parent)
{
    int state = w_sys_manage_cloudsync_with_time_running::get_state_local_dish_base_keepdata();

    QMetaObject::invokeMethod(parent,
                              "to_yun_version_diff",
                              Qt::QueuedConnection,
                              Q_ARG(int, state)
                              );
}


w_sys_manage_cloudsync_with_time_flow_upload_thread::w_sys_manage_cloudsync_with_time_flow_upload_thread(QObject *parent)
    : lds_thread("流水上传", parent)
{
    isbreak = false;
}

w_sys_manage_cloudsync_with_time_flow_upload_thread::~w_sys_manage_cloudsync_with_time_flow_upload_thread()
{

}

void w_sys_manage_cloudsync_with_time_flow_upload_thread::manual_stop()
{
    isbreak = true;
    wait();
    isbreak = false;
}

void w_sys_manage_cloudsync_with_time_flow_upload_thread::run()
{
    //1
    QString dbname = lds_query_thread::getThreadDbnameAndCreat();
    //2
    {
        QMap<QString, QString> headInfo;
        QString  httpkey;
        w_sys_manage_cloudsync_transfer transfer;
        QString retparam;
        QString dt_operdate;

        headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
        headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
        headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));

        httpkey = http_www_yqspos_com +"/arm/dish-flow/add-rest";

        while(false == isbreak
              && false == w_sys_manage_cloudsync_with_time_running::q_u_get_yun_already_keep()) {
            lds_query_hddpos::tran_saction();
            httpDataAutoDel root = w_sys_manage_cloudsync_with_time_running::q_u_get_void_flow_root();
            w_sys_manage_cloudsync_with_time_running::q_u_get_limit500_in_transaction(root.json, orgCode, dt_operdate);
            if(false == w_sys_manage_cloudsync_with_time_running::q_u_put_limit1000(root.json)) {
                lds_query_hddpos::roll_back();
                return;
            }
            QByteArray param = root.jsonToByteArray();
            httpDataAutoDel p = transfer.sendPost2(httpkey, param, headInfo,retparam, 20*60*1000);
            if(200 != p.httpStatus) {
                emit signalgoing( QObject::tr("上传失败") +p.message + "\n"+ dt_operdate);
                lds_query_hddpos::roll_back();
                return;
            }
            lds_query_hddpos::com_mit();
            emit signalgoing( QObject::tr("正在上传流水") + "\n"+ dt_operdate);
        }
        if(isbreak) {
            emit signalgoing(QObject::tr("上传终止"));
        } else {
            emit signalgoing(QObject::tr("上传完成"));
        }
        emit signalgoingFinished();
    }
    //3
    QSqlDatabase::removeDatabase(dbname);
}


w_sys_manage_cloudsync_with_time_basedata_thread::w_sys_manage_cloudsync_with_time_basedata_thread(QObject *parent)
    : lds_thread("实时上传-基础资料", parent)
{

}

w_sys_manage_cloudsync_with_time_basedata_thread::~w_sys_manage_cloudsync_with_time_basedata_thread()
{

}

void w_sys_manage_cloudsync_with_time_basedata_thread::run()
{
    w_sys_manage_cloudsync_with_time_basedata_thread_event event;
    execEvent(&event);
}

void w_sys_manage_cloudsync_with_time_basedata_thread_event::toExec()
{
    //
    // "------------实时上传-基础资料";
    w_sys_manage_cloudsync_with_time_running::base_data_upload_thread();
    // "------------实时上传-基础资料";
    QMetaObject::invokeMethod(public_sql::save_login,
                              "showprinterr",
                              Qt::QueuedConnection,
                              Q_ARG(QString, QObject::tr("上传完成")),
                              Q_ARG(QString, "")
                              );
}
