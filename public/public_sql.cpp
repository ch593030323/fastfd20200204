#include <QSqlError>
#include <QApplication>
#include <QFile>
#include <QDir>
#include "n_func.h"
#include <QDebug>
#include <qdir.h>
#include <QDirIterator>
#include "backheader.h"
#include <QSqlDriver>
#include "clientdisplay.h"
#include "lds_messagebox.h"
#include <QFontDatabase>
#include "w_sys_manage_sqlcopy.h"
#include "lds.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "public_logindialog.h"
#include "w_sys_manage_wirelessordermonitoring.h"
#include <QFileDialog>
#include <QDateTime>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtConcurrent>
#else
#include <QtCore>
#endif
#include "frontviewdialog_virtual.h"
#include "w_sys_manage_outer_bill_preview.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "public_short_cut_map.h"
#include "printer_pos.h"
#include <QSqlDatabase>
#include "public_dialog.h"
#include <QSqlRecord>
#include "fastfd_update.h"
#include "commonheader.h"
#include <QNetworkInterface>
#include "fexpandmain_model_sqltablemodel_data.h"
#include "w_m_member.h"
#include "lds_scr_qt.h"
#include "w_sys_manage_sqlcopy_soft_limit2.h"

#ifdef Q_OS_WIN32
#include "mysql_install.h"
#endif
#include "lds.h"
#include "public_encryption_dog.h"
#include "lock_screen.h"
#include "lds_query_hddpos.h"
#include "w_bt_dish_paymode.h"
#include "lds_query_thread.h"

QString public_sql::dbHostName;
QString public_sql::dbDatabaseName;
QString public_sql::dbUserName;
QString public_sql::dbPassword;

bool public_sql::islocal;
QString public_sql::localdbHostName;
QString public_sql::localdbDatabaseName;
QString public_sql::localdbUserName;
QString public_sql::localdbPassword;

QString public_sql::MySQL_info;
QString public_sql::sql_danwei = "sql";
QString public_sql::sql_backup_dir;
QString public_sql::sql_table_export_dir;

QString public_sql::show_empty_sql_file_dir;
QString public_sql::show_empty_sql_file;

QString public_sql::show_chinese_sql_file_dir;
QString public_sql::show_chinese_sql_file;

QString public_sql::show_english_sql_file_dir;
QString public_sql::show_english_sql_file;

QString public_sql::image_dish_dir;
QString public_sql::image_double_screen_dir;


QSharedPointer<QSettings> public_sql::fastfd_limit_conf;

QString public_sql::err_path_arg =
        "<span style=\" color:#ff546b;\">%1</span>"
        "<span style=\" color:#000000;\">%2</span>";

QList<public_sql::MySQL_SQLVERSION_DATA >public_sql::MySQL_sqlversion_data_content_list;

QString public_sql::ftp_businessdataupload_dir;

public_loginDialog *public_sql::save_login = 0;//保存前台的指针

int public_sql::gi_productid = 4;
//QObject::tr("数据库错误")+ QString::number(__LINE__)

QString public_sql::msg_sql_error;
//----------------------------------------------------------------------- "0602014500d21003"
//----------------------------------------------------------------------- "060202410101a71003"
//false
//----------------------------------------------------------------------- "0602034500e71003"
//----------------------------------------------------------------------- "060204410101df1003"

QMap<QString/*ch_tableno*/, bool> public_sql::preprinttablemap;
QMap<QString, QString> public_sql::sqlkeeptracemap;

//const QString public_sql::alipay_sys_service_provider_id = "2088511075880511";
QElapsedTimer public_sql::etimer;
QStandardItemModel public_sql::OPER_KEY_MODEL;
ClientDisplay public_sql::display_led;
bool public_sql::restrant_auto_lock_flag = false;

static QStringList SQL_ch_yun_already_keep(const QString &table) {
    return
            QStringList()
            <<QString("alter table %1  add column ch_yun_already_keep  VARCHAR(1) default 'N' ").arg(table)
           <<QString("ALTER TABLE `%1` ADD INDEX index_name (`ch_yun_already_keep` ) ").arg(table)
             ;
}

static bool db_open(QSqlDatabase &db){
    //  This must be done before the connection is opened
    db.setConnectOptions("MYSQL_OPT_RECONNECT=1;");
    return db.open();
}

static void db_close(QSqlDatabase &db){
    db.setConnectOptions();
    db.close();
}
/*
    QProcess p;
    p.setStandardOutputFile(outputFile);
    p.setStandardInputFile(inputFile);
    p.start(program);
    nextflag = p.waitForFinished(-1);

*/
static void PUBLIC_EXCUTE_PROCESS(QEventLoop *eloop,
                                  QString program,
                                  const QString &outputFile,
                                  const QString &inputFile,
                                  QString *errstring
                                  )
{
    *errstring = "";
    QProcess process;
    QString mysql_dynamic_cmd_filepath = QDir::currentPath() + "/pos/mysql_dynamic_cmd";
#ifdef Q_OS_LINUX
    mysql_dynamic_cmd_filepath = QDir::currentPath() + "/pos/mysql_dynamic_cmd.sh";
#endif
#ifdef Q_OS_WIN
    //window 下需要全路径启动dat文件
    mysql_dynamic_cmd_filepath = QDir::currentPath() + "/pos/mysql_dynamic_cmd.bat";
    program = "chcp 65001\n" + program;
#endif
    //outputFile
    if(outputFile.length() > 0) {
        if(false == lds::fileWriteByte(mysql_dynamic_cmd_filepath, (program + " > " + outputFile).toLocal8Bit(), errstring)) {
            goto end;
        }
    }
    //inputFile
    if(inputFile.length() > 0) {
        if(false == lds::fileWriteByte(mysql_dynamic_cmd_filepath, (program + " < " + inputFile).toLocal8Bit(), errstring)) {
            goto end;
        }
    }
    //start
#ifdef Q_OS_LINUX
    process.start("sh", QStringList() << mysql_dynamic_cmd_filepath);
#endif
#ifdef Q_OS_WIN
    process.start("cmd", QStringList() << "/c" << mysql_dynamic_cmd_filepath);
#endif
    if(false == process.waitForFinished(-1)) {
        *errstring =process.errorString();
    }
end:
    eloop->quit();
    return;
}

#ifdef Q_OS_WIN32
static void PUBLIC_MYSQL_INSTALL_PROCESS(QEventLoop *eloop, QObject *messageBar, bool *ret, QString *msg) {
    QString errstring;
    mysql_install_obj d;
    d.setMessageBar(messageBar, "setText");
    *ret = d.install(errstring);
    eloop->quit();
    *msg = errstring;
}

static bool PUBLIC_MYSQL_INSTALL(lds_messagebox_loading_show *messageBar, QString &msg) {
    messageBar->setTile(QObject::tr("正在安装数据库(整个过程需要几分钟)"));
    bool ret;
    QEventLoop eloop;
    QFuture<void> future = QtConcurrent::run(PUBLIC_MYSQL_INSTALL_PROCESS,
                                             &eloop,
                                             messageBar,
                                             &ret,
                                             &msg
                                             );
    eloop.exec();
    messageBar->setTile("");
    return ret;
}
#endif

public_sql::public_sql(QObject *parent) :
    QObject(parent)
{

}

QString public_sql::get_MySQL_sqlversion()
{
    int version=MySQL_sqlversion_data_content_list.count();
    //1.0->2.3 step 13
    //1.0->1.1 step 1
    return QString().sprintf("%.01f", (10+version)/10.0);
}
//新版本充V4.0开始
bool public_sql::MySQL_sqlversion_update(const QString &fromversion, QString &errstring, lds_messagebox_loading_show *loading)
{
    int begin = fromversion.toDouble() * 10 - 10;
    int end = begin = qMax(0, begin);
    if(begin > MySQL_sqlversion_data_content_list.count()) {
        errstring =  QObject::tr("数据库版本号大于软件版本号，请升级软件");
        return false;
    }
    if(begin == MySQL_sqlversion_data_content_list.count()) {
        return true;
    }
    //1.2>>1.8
    //! <--transaction begin-->
    lds_query_hddpos  query;
    Pre_sql_update();
    for(; end < MySQL_sqlversion_data_content_list.count(); end ++) {
        if(loading)
            loading->image_dynamic_run();
        //
        query.execInsertDuplicateUpdate("cey_sys_sqlversion",
                                        "sqlversion",
                                        qrtVariantPairList()
                                        << qrtVariantPair("sqlversion", QString().sprintf("%.01f", (10 + end + 1) / 10.0))
                                        << qrtVariantPair("sqlupdatetime", qrtFunc("now()")));

        for(int index = 0, count = MySQL_sqlversion_data_content_list[end].normal_sql.count(); index < count; index++) {
            if(false == query.execDirect(MySQL_sqlversion_data_content_list[end].normal_sql[index])) {
            }
        }
    }

    errstring =  QString("[%1]>>>[%2]")
            .arg((begin + 10) / 10.0, 0, 'f', 1)
            .arg((end   + 10) / 10.0, 0, 'f', 1) + QObject::tr("操作成功");
    return true;
}

bool public_sql::Other_sql_update(QWidget *parent)
{

    saas_pay_detail::mobile_connect_max_value_set_zero();

    ////最新版本
    lds_query_hddpos  query;
    query.execSelect("select sqlversion, sqlversion_sub from cey_sys_sqlversion order by sqlversion desc limit 1; ");
    query.next();
    QString sqlversion = query.recordValue("sqlversion").toString();
    int sqlversion_sub = query.recordValue("sqlversion_sub").toInt();
    qDebug() << "sqlversion:" << sqlversion << "sqlversion_sub:" << sqlversion_sub;

    {
        if(0 == sqlversion_sub) {
            ///若是打印机列表为空的话，会添加 lp0
            query.execSelect("select count(0) from cey_bt_kitchen_print ");
            query.next();
            if(0 == query.recordValue(0).toLongLong()) {
                query.execInsert("cey_bt_kitchen_print",
                                 "int_id, vch_printname, vch_printip, vch_printtype, vch_instype, int_paperwidth",
                                 "10, P10, /dev/usb/lp0, 0, 0, 32",
                                 ",");
                w_sys_manage_outer_bill_preview::if_bill_empty_import_58_def();

                ///实时上传，del和数据更新字段时，需要同步
                w_sys_manage_cloudsync_with_time_running::sql_update();

                ///由于配置文件的移动,删除旧版本的文件
                system("rm -rf /usr/desk/pos/font");
                system("rm -rf /usr/desk/pos/image");
                system("rm -rf /usr/desk/pos/language");
                system("rm -rf /usr/desk/pos/skin");
                system("rm -rf /usr/desk/pos/sql");
                system("rm -rf /usr/desk/pos/imagepath");
                system("rm -rf /usr/desk/pos/imagepath0");
                system("rm -rf /usr/desk/pos/imagepath1");
                ///单独设置促销触发器
                query.execDirect(w_sys_manage_cloudsync_with_time::bt_trigger_update_sql_arg.arg("cey_sys_parameter").arg("NEW.vch_parameter = OLD.vch_parameter"));
            }
            //
            if(lds::sysconf->value("system_setting/fupingxianshi", false).toBool()) {//版本升级,若是没有启动副屏,则自动设置为-1
                lds::sysconf->setValue("system_setting/fuping_mode", -1);
            }
            query.execInsert("cey_bt_paymode",  "ch_paymodeno,vch_paymodename,ch_IDflag,ch_faceflag,ch_changeflag,ch_incomeflag,ch_key,vch_arg4","BB,快捷支付,N,N,N,Y,,N", ",");
            //


            //补丁厨打餐桌格式设置默认值
            query.execSelect(" select vch_value from cey_sys_parameter where vch_parameter = 'kt_fmt_table_fmts' ");
            if(query.next()) {
                if(query.recordValue(0).toString().length() == 0) {
                    query.execUpdate("cey_sys_parameter", " vch_value", "no", qrtEqual("vch_parameter", "kt_fmt_table_fmts"));
                }
            }
            //补丁厨打流水号设置默认值
            query.execSelect(" select vch_value from cey_sys_parameter where vch_parameter = 'kt_fmt_serial_no_desc' ");
            if(query.next()) {
                if(query.recordValue(0).toString().length() == 0) {
                    query.execUpdate("cey_sys_parameter", " vch_value",  QObject::tr("流水号"), qrtEqual("vch_parameter", "kt_fmt_serial_no_desc"));
                }
            }
            //补丁小票餐桌设置默认值
            query.execSelect(" select vch_value from cey_sys_parameter where vch_parameter = 'bp_fmt_table_fmts' ");
            if(query.next()) {
                if(query.recordValue(0).toString().length() == 0) {
                    query.execUpdate("cey_sys_parameter", " vch_value", "no", qrtEqual("vch_parameter", "bp_fmt_table_fmts"));
                }
            }
            //补丁小票流水号设置默认值
            query.execSelect(" select vch_value from cey_sys_parameter where vch_parameter = 'bp_fmt_serial_no_desc' ");
            if(query.next()) {
                if(query.recordValue(0).toString().length() == 0) {
                    query.execUpdate("cey_sys_parameter", " vch_value", QObject::tr("流水号"), qrtEqual("vch_parameter", "bp_fmt_serial_no_desc"));
                }
            }
            query.execAlter("alter table cey_u_handover_master  add column `vch_memo2`  varchar(1023) default '' ");
            query.execInsert("cey_sys_purview_list", "0244,	后台                  	,N	,EDIT	,w_sys_parameter	,营业管理	,02	,N,	NULL", ",");
            query.execAlter("insert into cey_bt_paymode(ch_paymodeno, vch_paymodename, ch_IDflag, ch_faceflag, ch_changeflag, ch_incomeflag, ch_key, vch_arg4) values('CC','龙支付','N', 'N','N', 'Y', '', 'N' );");
            //
            sqlversion_sub = 1;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
        }
        if(1 == sqlversion_sub) {
            query.execAlter("update cey_bt_kitchen_print set ch_kitchen_view = 'N' where ifnull(ch_kitchen_view, '') = '' ;");

            sqlversion_sub ++;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
        }
        if(2 == sqlversion_sub) {
            query.execAlter(" update cey_bt_dish set num_m_price = num_price where ifnull(num_m_price, 0) = 0; ");

            //
            query.execInsertDuplicateUpdate("t_m_parameter", "vch_parameter", qrtVariantPairList()
                                            << qrtVariantPair("vch_parameter", "vip_phone_verify")
                                            << qrtVariantPair("vch_value", "1")
                                            << qrtVariantPair("vch_explain", "会员付款短信验证"));
            //
            sqlversion_sub ++;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
        }
        if(3 == sqlversion_sub) {
            query.execAlter("insert into cey_bt_paymode(ch_paymodeno, vch_paymodename, ch_IDflag, ch_faceflag, ch_changeflag, ch_incomeflag, ch_key, vch_arg4) values('DD','微支付','N', 'N','N', 'Y', '', 'N' );");
            query.execAlter("delete from cey_bt_paymode where ch_paymodeno = 'BB' ;");

            sqlversion_sub ++;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
        }
        if(4 == sqlversion_sub) {

            sqlversion_sub ++;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
        }
        if(5 == sqlversion_sub) {
            query.execAlter("ALTER TABLE `cey_bt_paymode`   CHANGE COLUMN `vch_arg3` `vch_arg3` TEXT NULL DEFAULT NULL ;");
            sqlversion_sub ++;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
        }
        if(6 == sqlversion_sub) {
            query.execSelect(QString(" select vch_value from t_m_parameter where vch_parameter = 'vip_card_type'; "));
            query.next();
            QString cardtype = query.recordValue("vch_value").toString();
            query.execSelect(QString(" select vch_memberno from t_m_member "));
            if(query.next()) {
                if(w_m_member::get_CARD_TYPE_MODE().findItems(cardtype).count() == 0) {
                    switch(lds_messagebox::critical(parent, MESSAGE_TITLE_VOID, QObject::tr("重新选择卡类型"), QObject::tr("磁卡"), QObject::tr("IC卡"))) {
                    case 0:
                        query.execUpdate("t_m_parameter", "vch_value", "磁卡", " vch_parameter = 'vip_card_type' ");
                        break;
                    case 1:
                        query.execUpdate("t_m_parameter", "vch_value", "IC卡", " vch_parameter = 'vip_card_type' ");
                        break;
                    }
                }
            }
            sqlversion_sub ++;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
        }
        if(7 == sqlversion_sub) {
            query.execAlter("ALTER TABLE `cey_u_checkout_detail`   ADD COLUMN `terminal_code` varchar(35) ;");
            query.execAlter("ALTER TABLE `cey_u_checkout_master`   ADD COLUMN `terminal_code` varchar(35) ;");
            query.execAlter("ALTER TABLE `cey_u_orderdish`   ADD COLUMN `terminal_code` varchar(35) ;");
            query.execAlter("ALTER TABLE `cey_u_service_low`   ADD COLUMN `terminal_code` varchar(35) ;");
            query.execAlter("ALTER TABLE `cey_u_orderdish_log`   ADD COLUMN `terminal_code` varchar(35) ;");
            query.execAlter("ALTER TABLE `cey_u_togo`   ADD COLUMN `terminal_code` varchar(35) ;");
            query.execAlter("ALTER TABLE `cey_u_table`   ADD COLUMN `terminal_code` varchar(35) ;");
            query.execAlter("ALTER TABLE `cey_u_master`   ADD COLUMN `terminal_code` varchar(35) ;");

            query.execAlter("ALTER TABLE `t_m_deposit`   ADD COLUMN `terminal_code` varchar(35) ;");
            query.execAlter("ALTER TABLE `t_m_point`   ADD COLUMN `terminal_code` varchar(35) ;");
            query.execAlter("ALTER TABLE `t_m_pay`   ADD COLUMN `terminal_code` varchar(35) ;");

            sqlversion_sub ++;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
        }
        if(8 == sqlversion_sub) {
            query.execAlter("alter table `cey_u_master`  add column ch_yun_already_keep  VARCHAR(1) default 'N' ");
            sqlversion_sub ++;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
        }
        if(9 == sqlversion_sub) {

            sqlversion_sub ++;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
        }
        if(10 == sqlversion_sub) {
            if(false == lds_query_hddpos::selectHasNext(" select ch_paymodeno from cey_bt_paymode where ch_paymodeno = 'EE' ")) {
                query.execAlter("insert into cey_bt_paymode(ch_paymodeno, vch_paymodename, ch_IDflag, ch_faceflag, ch_changeflag, ch_incomeflag, ch_key, vch_arg4) values('EE','联拓富','N', 'N','N', 'Y', '', 'N' );");
            }
            sqlversion_sub ++;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
        }
        if(11 == sqlversion_sub) {
            query.execAlter("ALTER TABLE `cey_u_checkout_detail`   ADD COLUMN `vch_card_name` VARCHAR(40) ;");
            query.execAlter("ALTER TABLE `cey_u_checkout_detail`   ADD COLUMN `num_card_point` DECIMAL(12,2) ;");

            sqlversion_sub ++;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
        }
        if(12 == sqlversion_sub) {
            query.execAlter("update cey_bt_paymode set vch_arg4 = 'N' where isnull(vch_arg4);");
            query.execAlter("update cey_bt_paymode set vch_arg6 = 'N' where isnull(vch_arg6);");

            sqlversion_sub ++;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
        }
        if(13 == sqlversion_sub) {
            //暂时不做处理了
            // query.execAlter("insert into cey_bt_paymode(ch_paymodeno, vch_paymodename, ch_IDflag, ch_faceflag, ch_changeflag, ch_incomeflag, ch_key, vch_arg4) values('XF','小费','N', 'N', 'N', 'N', '', 'N' );");

            sqlversion_sub ++;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
        }
        if(14 == sqlversion_sub) {
            query.execAlter("ALTER TABLE `cey_bt_dish`   ADD COLUMN `ch_addon_flag` VARCHAR(1) ;");

            sqlversion_sub ++;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
        }
        if(15 == sqlversion_sub) {
            query.execAlter("ALTER TABLE `cey_u_orderdish`   ADD COLUMN `ch_addon_flag` VARCHAR(1) ;");
            query.execAlter("ALTER TABLE `cey_u_orderdish`   ADD COLUMN `ch_addon_no` VARCHAR(7) ;");
            query.execAlter("ALTER TABLE `cey_u_hungbill`   ADD COLUMN `ch_addon_flag` VARCHAR(1) ;");
            query.execAlter("ALTER TABLE `cey_u_hungbill`   ADD COLUMN `ch_addon_no` VARCHAR(7) ;");

            sqlversion_sub ++;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
        }
        if(16 == sqlversion_sub) {

            sqlversion_sub ++;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));

        }
        if(17 == sqlversion_sub) {
            if(tableColumnCount("cey_st_provider") != 12)
                query.execCreate(
                            "drop table IF  EXISTS cey_st_provider;"
                            "CREATE TABLE IF NOT EXISTS cey_st_provider("
                            "`ch_providerNo`	varchar(8)  NOT NULL ,"
                            "`vch_providerName`	varchar(60) NOT NULL,"
                            "`vch_tax_number`	varchar(60)  DEFAULT NULL ,"
                            "`ch_typeno`	varchar(15)  DEFAULT NULL ,"
                            "`vch_linkman`	varchar(60)  DEFAULT NULL ,"

                            "`vch_tel`varchar(16)  DEFAULT NULL ,"
                            "`vch_email`varchar(60)  DEFAULT NULL ,"
                            "`vch_adress`varchar(60)  DEFAULT NULL ,"
                            "`vch_voucherno`varchar(60)  DEFAULT NULL ,"
                            "`vch_memo`varchar(60)  DEFAULT NULL ,"

                            "`int_tax` DECIMAL(12,2) default 0 ,"
                            "`num_remain` DECIMAL(12,2) default 0 ,"

                            " PRIMARY KEY (`ch_providerNo`)"
                            ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
                            );
            if(tableColumnCount("cey_st_provider_pay") != 8)
                query.execCreate(
                            "drop table IF  EXISTS cey_st_provider_pay;"
                            "CREATE TABLE IF NOT EXISTS cey_st_provider_pay("
                            "`ch_sheetno`	VARCHAR(12)  NOT NULL ,"
                            "`ch_sheetType`	VARCHAR(2)  NOT NULL ,"
                            "`ch_providerNo`	varchar(8)  NOT NULL ,"
                            "`num_cost` DECIMAL(12,2) default 0 ,"
                            "`num_tax` DECIMAL(12,2) default 0 ,"
                            "`num_cash` DECIMAL(12,2) default 0 ,"

                            "`num_remain` DECIMAL(12,2) default 0 ,"
                            "`dt_operdate` DATETIME ,"

                            " PRIMARY KEY (`ch_sheetno`), "
                            " KEY `idx_cey_st_provider_pay1` (`ch_providerNo`),"
                            " KEY `idx_cey_st_provider_pay2` (`dt_operdate`) "
                            ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
                            );
            if(tableColumnCount("cey_st_provider_pay_flow") != 6)
                query.execCreate(
                            "drop table IF  EXISTS cey_st_provider_pay_flow;"
                            "CREATE TABLE IF NOT EXISTS cey_st_provider_pay_flow("
                            "`int_flowID` int(11) NOT NULL AUTO_INCREMENT ,"
                            "`ch_sheetno`	VARCHAR(12)  NOT NULL ,"
                            "`ch_providerNo`	varchar(8)  NOT NULL ,"
                            "`num_pay` DECIMAL(12,2) default 0 ,"
                            "`num_remain` DECIMAL(12,2) default 0 ,"

                            "`dt_operdate` DATETIME ,"

                            " PRIMARY KEY (`int_flowID`), "
                            " KEY `idx_cey_st_provider_flow1` (`ch_sheetno`), "
                            " KEY `idx_cey_st_provider_flow2` (`ch_providerNo`), "
                            " KEY `idx_cey_st_provider_flow3` (`dt_operdate`) "
                            ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
                            );
            query.execAlter("ALTER TABLE `cey_st_sheet_master`   ADD COLUMN `int_tax` DECIMAL(12,2) default 0;");
            query.execAlter("ALTER TABLE `cey_sys_company`   ADD COLUMN `vch_tax_number` varchar(60)  DEFAULT NULL ");
            query.execAlter("ALTER TABLE `cey_st_provider_pay`   CHANGE COLUMN `ch_providerNo` `ch_providerNo` varchar(8)  DEFAULT NULL ");
            query.execAlter("ALTER TABLE `cey_st_provider_pay_flow`   CHANGE COLUMN `ch_providerNo` `ch_providerNo` varchar(8)  DEFAULT NULL ");

            //        "CREATE TABLE IF NOT EXISTS cey_st_warehouse("
            //        "`ch_warehouseNo`	varchar(15)  NOT NULL ,"
            //        "`vch_warehouseName`	varchar(60) NOT NULL,"
            //        " PRIMARY KEY (`ch_warehouseNo`)"
            //        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"

            //        "CREATE TABLE IF NOT EXISTS cey_st_warehouse_num("
            //        "`ch_warehouseNo`	varchar(15)  NOT NULL ,"
            //        "`ch_materialno`	varchar(7) NOT NULL,"
            //        "`num_num`DECIMAL(18,4) DEFAULT '0.00' ,"
            //        " PRIMARY KEY (`vch_no`)"
            //        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"

            //        "CREATE TABLE IF NOT EXISTS cey_st_warehouse_flow("
            //        "`int_flowId` int(11) NOT NULL AUTO_INCREMENT ,"
            //        "`ch_warehouseNo`	varchar(15)  NOT NULL ,"
            //        "`ch_materialno`	varchar(7) NOT NULL ,"
            //        "`dt_operdate` datetime NOT NULL ,"
            //        "`vch_operId` VARCHAR(6) NOT NULL ,"
            //        "`sheettype` varchar(3) NOT NULL ,"
            //        "`num_num` decimal(18,4) DEFAULT '0.0000' ,"
            //        "`remain_qty` decimal(18,4) DEFAULT '0.0000' ,"
            //        " PRIMARY KEY (`vch_no`)"
            //        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
            //        ;

            sqlversion_sub ++;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
        }
        if(18 == sqlversion_sub) {
            QString la1 = lds::sysconf->value("system_setting/SYS_LANGUAGE").toString();
            QString la2 = lds::sysconf->value("system_setting/SYS_LANGUAGE_SECOND").toString();
            if(la1 == "CN")
                lds::sysconf->setValue("system_setting/SYS_LANGUAGE", _CN_);
            else if(la1 == "EN")
                lds::sysconf->setValue("system_setting/SYS_LANGUAGE", _EN_);
            if(la2 == "CN")
                lds::sysconf->setValue("system_setting/SYS_LANGUAGE_SECOND", _CN_);
            else if(la2 == "EN")
                lds::sysconf->setValue("system_setting/SYS_LANGUAGE_SECOND", _EN_);

            sqlversion_sub ++;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
        }
        if(19 == sqlversion_sub) {
            query.execAlter("ALTER TABLE `cey_u_checkout_detail`   ADD COLUMN `vch_card_adress` varchar(50) ;");
            sqlversion_sub ++;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
        }
        if(20 == sqlversion_sub) {
            sqlversion_sub ++;
            query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
        }
    }
    if(21 == sqlversion_sub) {
        sqlversion_sub ++;
        query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
    }
    if(22 == sqlversion_sub) {
        query.execAlter("ALTER TABLE `cey_sys_parameter`   CHANGE COLUMN `vch_value` `vch_value` TEXT ;");

        sqlversion_sub ++;
        query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
    }
    if(23 == sqlversion_sub) {
        query.execAlter(" ALTER TABLE `cey_u_orderdish_log` ADD INDEX `idx_int_orderflow` (`int_orderflow` ) " );

        sqlversion_sub ++;
        query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
    }
    if(24 == sqlversion_sub) {
        query.execAlter("ALTER TABLE `cey_bt_kitchen_plan`   ADD COLUMN `vch_plan_id_service` varchar(2) ;");
        query.execAlter("ALTER TABLE `cey_bt_kitchen_plan`   ADD COLUMN `vch_kitchen_type` varchar(32) ;");

        query.execAlter("ALTER TABLE `cey_bt_kitchen_plan`   DROP COLUMN `vch_plan_id_service` ;");
        query.execAlter("ALTER TABLE `cey_bt_kitchen_plan`   DROP COLUMN `vch_kitchen_type` ;");


        sqlversion_sub ++;
        query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
    }
    if(25 == sqlversion_sub) {
        sqlversion_sub ++;
        query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
    }
    if(26 == sqlversion_sub) {
        sqlversion_sub ++;
        query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
    }

    if(27 == sqlversion_sub) {
        query.execAlter(
                    "CREATE TABLE IF NOT EXISTS cey_bt_kitchen_view("///厨显方案
                    "`vch_plan_id` varchar(2) primary key NOT NULL ,"
                    "`vch_plan_name` varchar(32) ,"
                    "`vch_plan_id_service` varchar(2) ,"
                    "`vch_kitchen_type` varchar(32) ,"
                    "`vch_shop_type` varchar(32) ,"
                    "`ch_pop_new_bill_info` varchar(1) ,"
                    "`ch_auto_take` varchar(1) ,"
                    "`int_auto_take` int ,"
                    "`int_order_warn_mins` int ,"
                    "`ch_voice_count` varchar(1) ,"
                    "`int_voice_count` int ,"
                    "`vch_language` varchar(8) ,"
                    "`vch_screen_primary_size` varchar(32) ,"
                    "`ch_screen_secondary` varchar(1) ,"
                    "`vch_screen_secondary_size` varchar(32), "
                    "`vch_terminal`	varchar(64) NOT NULL UNIQUE"

                    ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
                    );
        query.execAlter("ALTER TABLE `cey_bt_kitchen_view`   ADD COLUMN `ch_refresh_order_insert` varchar(1)  default 'N'; ");
        query.execAlter("ALTER TABLE `cey_bt_kitchen_view`   ADD COLUMN `ch_refresh_order_update` varchar(1)  default 'N'; ");
        query.execAlter("ALTER TABLE `cey_bt_kitchen_view`   ADD COLUMN `ch_refresh_order_quit` varchar(1)  default 'N'; ");
        query.execAlter("ALTER TABLE `cey_bt_kitchen_view`   ADD COLUMN `ch_refresh_sys_geometry` varchar(1)  default 'N'; ");

        //none,order,sys,quit


        //发起请求,若是重复请求则提示并返回
        //发起请求,若超时则删除本次请求
        //正常响应后,由kt负责退菜
        //退菜成功,餐饮刷新界面,删除本次请求
        //批量处理时,先设置的厨显A,然后A改成出品,但是菜品的厨显还是A,如何避免
        //出品厨显,不能退单
        //plan_id 从厨显改成出品会破坏[消息只能发给厨显]的情况,如何避免
        query.execAlter("DROP TABLE `cey_u_orderdish_print_kt_quit_queue`;");
        query.execAlter(
                    "CREATE TABLE IF NOT EXISTS cey_u_orderdish_print_kt_quit_queue("///厨显方案
                    "`int_orderflow` int(11) NOT NULL ,"
                    "`vch_plan_id` varchar(2) NOT NULL ,"
                    "`ch_state` varchar(1) ,"//0待确认,1确认中,2确认完成,3操作拒绝原因见memo
                    "`vch_memo` varchar(60) ,"//响应的消息内容
                    "`vch_back_operID` varchar(6) ,"
                    "`vch_print_memo` varchar(1023) ,"
                    "`num_back` decimal(12,3), "

                    "KEY `idx_vch_plan_id` (`vch_plan_id`),"
                    " PRIMARY KEY (`int_orderflow`) "

                    ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
                    );
        query.execAlter("DROP TABLE `cey_u_orderdish_print_kt`;");
        query.execAlter("DROP TABLE `cey_u_orderdish_print_kt_order`;");
        query.execAlter("CREATE TABLE `cey_u_orderdish_print_kt_order` ("
                        "`int_flowID` int(11) NOT NULL,"
                        "`vch_plan_id`	varchar(2),"
                        "`ch_billno` varchar(15),"
                        "`ch_dishno` varchar(7),"
                        "`ch_state` varchar(1),"
                        "KEY `idx_vch_plan_id` (`vch_plan_id`),"
                        "KEY `idx_ch_billno` (`ch_billno`),"
                        "KEY `idx_ch_dishno` (`ch_dishno`),"
                        "KEY `idx_ch_state` (`ch_state`),"
                        "PRIMARY KEY (`int_flowID`)"
                        ") ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT ;"
                        );
        query.execAlter("DROP TABLE `cey_u_orderdish_print_kt_table`;");
        query.execAlter("CREATE TABLE `cey_u_orderdish_print_kt_table` ("
                        "`vch_plan_id`	varchar(2) NOT NULL,"
                        "`ch_billno` varchar(15) NOT NULL,"
                        "`ch_state` varchar(1),"
                        "`dt_operdate` datetime,"
                        "`int_cook` int default 0 ,"
                        "`int_finish` int default 0 ,"
                        "KEY `idx_ch_state` (`ch_state`),"
                        "KEY `idx_dt_operdate` (`dt_operdate`),"
                        "KEY `idx_int_finish` (`int_finish`),"
                        "KEY `idx_int_cook` (`int_cook`),"
                        "PRIMARY KEY (`vch_plan_id`, `ch_billno`)"
                        ") ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT ;"
                        );
        query.execAlter("DROP TABLE `cey_u_orderdish_print_kt_dish`;");
        query.execAlter("CREATE TABLE `cey_u_orderdish_print_kt_dish` ("
                        "`vch_plan_id`	varchar(2) NOT NULL,"
                        "`ch_dishno` varchar(7) NOT NULL,"
                        "`ch_billno` varchar(15) NOT NULL,"
                        "`num_num`	decimal(12,3) default 0 ,"
                        "`num_back`	decimal(12,3) default 0 ,"
                        "`num_cook`	decimal(12,3) default 0 ,"
                        "`num_finish`	decimal(12,3) default 0 ,"
                        "KEY `idx_ch_billno` (`ch_billno`),"
                        "PRIMARY KEY (`vch_plan_id`, `ch_dishno`)"
                        ") ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT ;"
                        );

        query.execAlter(
                    ///tr_cey_u_orderdish_insert
                    "    DROP TRIGGER IF EXISTS tr_cey_u_orderdish_insert;\n"
                    "    CREATE TRIGGER tr_cey_u_orderdish_insert AFTER INSERT ON cey_u_orderdish FOR EACH ROW \n"

                    "   BEGIN"

                    "      DECLARE flowId integer;\n"
                    "      DECLARE dishno varchar(7);\n"
                    "      DECLARE numnum numeric(10, 3);\n"
                    "      DECLARE materialno varchar(7);\n"
                    "      DECLARE intflowId integer;\n"
                    "      DECLARE initqty numeric(16,4);\n"
                    "      DECLARE initprice numeric(16,4);\n"
                    "      DECLARE remainqty numeric(16,4);\n"
                    "      DECLARE remainprice numeric(16,4);\n"
                    "      DECLARE itemflag char(1);\n"
                    "      DECLARE plan_id varchar(2);\n"
                    "      DECLARE kitchen_type varchar(32);\n"

                    "        set flowId = NEW.int_flowId;\n"
                    "        set dishno = NEW.ch_dishno;\n"
                    "        set numnum = NEW.num_num - NEW.num_back;\n"

                    "        set itemflag = 'N';\n"
                    "        set itemflag = (select ifnull(item_flag,'N') from cey_bt_dish where ch_dishno = dishno);\n"
                    "        set materialno = dishno;\n"
                    "        set kitchen_type = (select vch_value from cey_sys_parameter where vch_parameter = 'kt_kitchen_type');\n"
                    "        IF (kitchen_type = 'kitchen_print' ) THEN\n"
                    ///cey_u_orderdish_print_tr
                    "        insert cey_u_orderdish_print_tr(int_orderflow, vch_plan_k3o3_id, vch_plan_k3o3_state, dt_operdate, ch_billno, ch_dishno) values (flowId, NEW.vch_plan_k3o3_id, NEW.vch_plan_k3o3_state, NEW.dt_operdate, NEW.ch_billno, NEW.ch_dishno);\n"
                    "        END IF;\n"
                    "        IF (kitchen_type = 'kitchen_view' ) THEN\n"
                    ///cey_u_orderdish_print_kt
                    "               set plan_id = mid(NEW.vch_plan_k3o3_id, 1, 2);\n"
                    "               IF  exists (select 1 from cey_bt_kitchen_view  where vch_plan_id = plan_id) THEN\n"
                    "                           insert into cey_u_orderdish_print_kt_order(int_flowID,vch_plan_id,ch_billno, ch_dishno, ch_state) values (flowId,plan_id, NEW.ch_billno,NEW.ch_dishno, 0);\n"
                    "                           IF not exists (select 1 from cey_u_orderdish_print_kt_table  where vch_plan_id = plan_id and ch_billno = NEW.ch_billno) THEN\n"
                    "                                              insert into cey_u_orderdish_print_kt_table(vch_plan_id,ch_billno) values (plan_id, NEW.ch_billno);\n"
                    "                           END IF;\n"
                    "                           update cey_u_orderdish_print_kt_table set ch_state = 0, dt_operdate = now() where vch_plan_id = plan_id and ch_billno = NEW.ch_billno;\n"
                    "                           IF not exists (select 1 from cey_u_orderdish_print_kt_dish  where vch_plan_id = plan_id and ch_dishno = NEW.ch_dishno) THEN\n"
                    "                                              insert into cey_u_orderdish_print_kt_dish(vch_plan_id, ch_dishno) values (plan_id, NEW.ch_dishno);\n"
                    "                           END IF;\n"
                    "                           update cey_u_orderdish_print_kt_dish set num_num = num_num + NEW.num_num where vch_plan_id = plan_id and ch_dishno = NEW.ch_dishno;\n"
                    "                           update cey_u_orderdish_print_kt_dish set num_back = num_back + NEW.num_back where vch_plan_id = plan_id and ch_dishno = NEW.ch_dishno;\n"
                    "                           update cey_bt_kitchen_view set ch_refresh_order_insert = 'Y' where vch_plan_id = plan_id;\n"
                    "               END IF;\n"
                    "        END IF;\n"
                    ///cey_st_material_flow
                    "       IF (materialno<>'' and itemflag = 'Y') THEN"

                    "         set intflowId = (select ifnull(max(int_flowId),0) from cey_st_material_flow where ch_materialno = materialno);\n"

                    "      IF (intflowId = 0) THEN"
                    "               set initqty = 0;\n"
                    "           ELSE"
                    "               set initqty = (select remain_qty from cey_st_material_flow where int_flowId = intflowId);\n"
                    "       END IF;\n"

                    "      IF (intflowId = 0) THEN"
                    "         set initprice = 0;\n"
                    "         ELSE"
                    "           set initprice = (select remain_price from cey_st_material_flow where int_flowId = intflowId);\n"
                    "       END IF;\n"

                    "    set remainqty  = initqty - numnum ;\n"

                    "       IF (remainqty = 0) THEN"
                    "          set remainprice = 0;\n"
                    "       ELSE"
                    "                set remainprice = initprice;\n"
                    "        END IF;\n"

                    "       insert cey_st_material_flow (ch_materialno, dt_operdate, sheettype, int_flag,"
                    "               num_num, sheet_price, cost_price, remain_qty, remain_price, ch_sheetno )"
                    "          values (materialno,now(), 'QT', if(numnum < 0, -1, 1), abs(numnum), 0, remainprice, remainqty, remainprice, LEFT(flowId,20));\n"


                    "      IF exists (select 1 from cey_st_material_num where ch_materialno = materialno) THEN"
                    "       update cey_st_material_num set num_num = remainqty, cost_price = remainprice  where ch_materialno = materialno;\n"
                    "       ELSE"
                    "         insert into cey_st_material_num (ch_materialno, num_num, cost_price) values (materialno, remainqty, remainprice);\n"
                    "       END IF;\n"
                    "      END IF;\n"
                    "    END"
                    );
        ///tr_cey_u_orderdish_update
        query.execAlter(
                    "    DROP TRIGGER IF EXISTS tr_cey_u_orderdish_update;\n"

                    "    CREATE TRIGGER tr_cey_u_orderdish_update AFTER UPDATE ON cey_u_orderdish FOR EACH ROW \n"

                    "  BEGIN"
                    "    DECLARE m_done int default 0;\n"
                    "    DECLARE flowId integer;\n"
                    "    DECLARE dishno varchar(7);\n"
                    "    DECLARE numnum numeric(10, 3);\n"
                    "    DECLARE materialno varchar(7);\n"
                    "    DECLARE intflowId integer;\n"
                    "    DECLARE initqty numeric(16,4);\n"
                    "    DECLARE initprice numeric(16,4);\n"
                    "    DECLARE remainqty numeric(16,4);\n"
                    "    DECLARE remainprice numeric(16,4);\n"
                    "    DECLARE itemflag char(1);\n"
                    "    DECLARE kitchen_type varchar(32);\n"
                    "    DECLARE plan_id varchar(2);\n"

                    "    set kitchen_type = (select vch_value from cey_sys_parameter where vch_parameter = 'kt_kitchen_type');\n"
                    "    set flowId = NEW.int_flowId;\n"
                    ///cey_u_orderdish_print_tr
                    "    IF(NEW.int_flowId = OLD.int_flowId and kitchen_type = 'kitchen_print'  and NEW.vch_plan_k3o3_state <> OLD.vch_plan_k3o3_state) THEN\n"
                    "             update cey_u_orderdish_print_tr set vch_plan_k3o3_state = NEW.vch_plan_k3o3_state where int_orderflow = flowId;\n"
                    "    END IF;\n"
                    ///~cey_u_orderdish_print_tr
                    ///cey_u_orderdish_print_kt
                    "    IF (NEW.int_flowId = OLD.int_flowId and kitchen_type = 'kitchen_view' ) THEN\n"
                    "             set plan_id = mid(OLD.vch_plan_k3o3_id, 1, 2);\n"
                    "             IF  exists (select 1 from cey_bt_kitchen_view  where vch_plan_id = plan_id) THEN\n"
                    "                      IF (NEW.num_back <> OLD.num_back or NEW.num_num <> OLD.num_num ) THEN\n"
                    "                               update cey_u_orderdish_print_kt_dish set num_num = num_num - (OLD.num_num - NEW.num_num) where vch_plan_id = plan_id and ch_dishno = NEW.ch_dishno;\n"
                    "                               update cey_u_orderdish_print_kt_dish set num_back = num_back - (OLD.num_back - NEW.num_back) where vch_plan_id = plan_id and ch_dishno = NEW.ch_dishno;\n"
                    "                               update cey_bt_kitchen_view set ch_refresh_order_update = 'Y' where vch_plan_id = plan_id;\n"
                    "                      END IF;\n"
                    "             END IF;\n"
                    "    END IF;\n"
                    ///~cey_u_orderdish_print_kt

                    "   IF(NEW.int_flowId = OLD.int_flowId and (NEW.num_back <> OLD.num_back or NEW.num_num <> OLD.num_num)) THEN\n"

                    "    set dishno = NEW.ch_dishno;\n"
                    "      set numnum = OLD.num_num - OLD.num_back - (NEW.num_num - NEW.num_back);\n"

                    "    set itemflag = 'N';\n"
                    "    set itemflag = (select ifnull(item_flag,'N') from cey_bt_dish where ch_dishno = dishno);\n"
                    "    set materialno = dishno;\n"

                    "    IF (materialno<>'' and itemflag = 'Y')  THEN\n"

                    "         set intflowId = (select ifnull(max(int_flowId),0) from cey_st_material_flow where ch_materialno = materialno);\n"

                    "         IF (intflowId = 0) THEN\n"
                    "              set initqty = 0;\n"
                    "         ELSE\n"
                    "              set initqty = (select remain_qty from cey_st_material_flow where int_flowId = intflowId);\n"
                    "         END IF;\n"

                    "         IF (intflowId = 0) THEN\n"
                    "              set initprice = 0;\n"
                    "         ELSE\n"
                    "              set initprice = (select remain_price from cey_st_material_flow where int_flowId = intflowId);\n"
                    "         END IF;\n"

                    "         set remainqty = initqty + numnum ;\n"

                    "         IF (remainqty = 0)  THEN\n"
                    "              set remainprice = 0;\n"
                    "         ELSE\n"
                    "              set remainprice = initprice;\n"
                    "         END IF;\n"

                    "         insert cey_st_material_flow (ch_materialno, dt_operdate, sheettype, int_flag,\n"
                    "                         num_num, sheet_price, cost_price, remain_qty, remain_price, ch_sheetno )\n"
                    "                 values (materialno,now(), 'QT', if(numnum < 0, -1, 1), abs(numnum), 0, remainprice, remainqty, remainprice, left(flowId,20));\n"

                    "        IF exists (select 1 from cey_st_material_num where ch_materialno = materialno) THEN\n"
                    "                 update cey_st_material_num set num_num = remainqty, cost_price = remainprice\n"
                    "                  where ch_materialno = materialno;\n"
                    "        ELSE\n"
                    "                 insert into cey_st_material_num (ch_materialno, num_num, cost_price)\n"
                    "                 values (materialno, remainqty, remainprice);\n"
                    "        END IF;\n"

                    "     END IF;\n"

                    "       END IF;\n"

                    " END\n");
        sqlversion_sub ++;
        query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
    }

    if(28 == sqlversion_sub) {
        query.execAlter("ALTER TABLE `cey_bt_kitchen_view`   ADD COLUMN `ch_auto_login` varchar(1)  default 'N'; ");
        query.execAlter("ALTER TABLE `cey_bt_kitchen_view`   ADD COLUMN `vch_operID`	varchar(6); ");

        sqlversion_sub ++;
        query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
    }

    if(29 == sqlversion_sub) {
        query.execAlter("CREATE TABLE `interface_malayisa_hourly_pos` ("
                        "`int_batchid` int(11) NOT NULL,"
                        "`dt_operdate` date NOT NULL UNIQUE,"
                        "`ch_state` varchar(1) NOT NULL,"
                        "`vch_memo` varchar(60) ,"
                        "KEY `idx_ch_state` (`ch_state`),"
                        "PRIMARY KEY (`int_batchid`)"
                        ") ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT ;"
                        );
        query.execAlter("alter table cey_bt_paymode ADD column `vch_english` varchar(40) unique;");
        w_bt_dish_paymode::update_vch_english();

        sqlversion_sub ++;
        query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
    }

    if(30 == sqlversion_sub) {
        query.execAlter("CREATE TABLE IF NOT EXISTS cey_bt_table_area_oper("
                        "vch_operid varchar(6) NOT NULL,"
                        "ch_areano varchar(2) NOT NULL,"
                        "ch_state char(1) NULL"
                        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
                        );

        sqlversion_sub ++;
        query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
    }

    if(31 == sqlversion_sub) {
        query.execAlter("alter table cey_bt_dish_type_oper add column ch_state char(1) default 'Y';");

        sqlversion_sub ++;
        query.execUpdate("cey_sys_sqlversion", "sqlversion_sub", sqlversion_sub, qrtEqual("sqlversion", sqlversion));
    }
    //会员打印详细初始化
    w_sys_manage_outer_bill_preview::import_bp_fmt_m_default();
    w_sys_manage_cloudsync_with_time_running::sql_check();
    return false;
}

bool public_sql::Pre_sql_update()
{
    lds_query_hddpos  query;
    ////
    Pre_sql_update_one(query, "material_bt_dish", 5);
    ////
    Pre_sql_update_one(query, "del_material_bt_dish", 5);
    ////
    Pre_sql_update_one(query, "material_loss_apply", 8);
    ////
    Pre_sql_update_one(query, "material_loss_apply_detail", 10);
    ////
    Pre_sql_update_one(query, "material_purchase_apply", 7);
    ///
    Pre_sql_update_one(query, "material_purchase_apply_detail", 9);
    ////
    Pre_sql_update_one(query, "material_purchase_in_storage", 8);
    ////
    Pre_sql_update_one(query, "material_purchase_in_storage_detail", 10);
    ////
    Pre_sql_update_one(query, "material_u_orderdish", 7);

    return true;
}

bool public_sql::Pre_sql_update_one(lds_query_hddpos  &query, const QString &tablename, int count)
{
    query.execSelect(QString("SELECT count(0) from information_schema.columns where table_schema='hddpos' and table_name='%1'   ")
                     .arg(tablename));
    query.next();
    if(count != query.recordValue(0).toInt()) {
        query.execDrop(tablename);
    }

    return true;
}

void public_sql::PUBLIC_SQL_INIT()
{
    if(show_empty_sql_file_dir.isEmpty()) {
        show_empty_sql_file_dir = "userdata/settings/sql";
        show_chinese_sql_file_dir = "userdata/settings/sql";
        show_english_sql_file_dir = "userdata/settings/sql";

        show_empty_sql_file="sql_empty_data";
#ifdef QT_SCPOS_SUPPORT
        show_chinese_sql_file="shengxian.sql";
#else
        show_chinese_sql_file="sql_chinese_data";
#endif
        show_english_sql_file="sql_english_data";
    }

    if(sql_backup_dir.isEmpty()) {
        sql_backup_dir =  lds::localdataPath +"/"+"sql_backup_dir";
        QDir().mkpath(sql_backup_dir);
    }
    if(sql_table_export_dir.isEmpty()) {
        sql_table_export_dir =  lds::localdataPath +"/"+"sql_table_export_dir";
        QDir().mkpath(sql_table_export_dir);
    }

    if(image_dish_dir.isEmpty()) {
        image_dish_dir =  lds::localdataPath +"/"+"image_dish_dir";
        QDir().mkpath(image_dish_dir);
    }
    if(image_double_screen_dir.isEmpty()) {
        image_double_screen_dir =  lds::localdataPath +"/"+"image_double_screen_dir";
        QDir().mkpath(image_double_screen_dir);
    }
    if(ftp_businessdataupload_dir.isEmpty()) {
        ftp_businessdataupload_dir = lds::localdataPath +"/"+"ftp_businessdataupload";
        QDir().mkpath(ftp_businessdataupload_dir);
    }
    if(fastfd_limit_conf.isNull()) {
#if defined(Q_OS_WIN)
        public_sql::fastfd_limit_conf = QSharedPointer<QSettings>(new QSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\fastfd_limit", QSettings::NativeFormat));                //配置文件的软件限时ini，filepath
#else
        public_sql::fastfd_limit_conf = QSharedPointer<QSettings>(new QSettings("pos/fastfd_limit", QSettings::IniFormat));                //配置文件的软件限时ini，filepath
#endif
    }

    //! 本地厨打单 等于1 为80

    //数据库升级的详细内容
    //1.1 厨打补充的说明
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  ,  ("1.1")// 厨打补充的说明")
                                                  ));
    //1.2称重属性,条码
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  << "alter table cey_bt_dish add column ch_weight VARCHAR(1);"
                                                  << "alter table cey_bt_dish add column vch_barcode  VARCHAR(64);"
                                                  ,  ("1.2")//称重属性,条码")
                                                  ));
    //1.3操作员权限
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  <<
                                                  "insert into cey_sys_purview_list(ch_purviewno, vch_purviewname, ch_day_cue, ch_open_type, vch_window, vch_purview_typename, ch_purview_typeno, ch_report, vch_withparm) values('0910','MySQL服务器','N', 'EDIT',null, '系统管理', '09', 'N', null);"
                                                  "insert into cey_sys_purview_list(ch_purviewno, vch_purviewname, ch_day_cue, ch_open_type, vch_window, vch_purview_typename, ch_purview_typeno, ch_report, vch_withparm) values('0911','文件管理器','N', 'EDIT',null, '系统管理', '09', 'N', null);"
                                                  "insert into cey_sys_purview_list(ch_purviewno, vch_purviewname, ch_day_cue, ch_open_type, vch_window, vch_purview_typename, ch_purview_typeno, ch_report, vch_withparm) values('0912','无线点菜监控','N', 'EDIT',null, '系统管理', '09', 'N', null);"
                                                  "insert into cey_sys_purview_list(ch_purviewno, vch_purviewname, ch_day_cue, ch_open_type, vch_window, vch_purview_typename, ch_purview_typeno, ch_report, vch_withparm) values('0913','云同步','N', 'EDIT',null, '系统管理', '09', 'N', null);"
                                                  ,  ("1.3")//操作员权限")
                                                  ));
    //1.4流水号
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  <<"replace into cey_sys_parameter(vch_parameter, vch_value, vch_explain) values('serial_no_begin', 0, '开始流水号');"
                                                  <<"replace into cey_sys_parameter(vch_parameter, vch_value, vch_explain) values('serial_no_end', 999, '结束流水号');"
                                                  <<"replace into cey_sys_parameter(vch_parameter, vch_value, vch_explain) values('serial_no_now', 0, '开始流水号');"
                                                  ,  ("1.4")//流水号")
                                                  ));
    //1.5打印中倍高倍宽
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  <<"replace into cey_sys_parameter(vch_parameter, vch_value, vch_explain) values('bp_dishname_left', 1, '账单品名左空');"
                                                  <<"replace into cey_sys_parameter(vch_parameter, vch_value, vch_explain) values('bp_unit_left', 1, '账单品码左空');"
                                                  <<"replace into cey_sys_parameter(vch_parameter, vch_value, vch_explain) values('bp_num_left', 1, '账单数量左空');"
                                                  <<"replace into cey_sys_parameter(vch_parameter, vch_value, vch_explain) values('bp_price_left', 1, '账单价格左空');"
                                                  <<"replace into cey_sys_parameter(vch_parameter, vch_value, vch_explain) values('bp_amount_left', 1, '账单金额左空');"
                                                  <<"replace into cey_sys_parameter(vch_parameter, vch_value, vch_explain) values('bp_discount_left', 1, '账单折扣左空');"
                                                  <<"replace into cey_sys_parameter(vch_parameter, vch_value, vch_explain) values('bp_content_len', '00', '账单内容是否倍高倍宽');"//00 10 11
                                                  ,  ("1.5")//打印中倍高倍宽")
                                                  ));
    //1.6 南京国际广场的，数据采集
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  <<
                                                  "CREATE TABLE IF NOT EXISTS cey_web_data_collection("
                                                  "id varchar(20) NOT NULL UNIQUE,"
                                                  "service_ip varchar(100) NOT NULL,"
                                                  "service_port varchar(100) NOT NULL,"
                                                  "service_action varchar(100) NOT NULL,"
                                                  "service_method_name varchar(100) NOT NULL,"
                                                  "service_method_uri varchar(100) NOT NULL,"
                                                  "service_submit_path varchar(100) NOT NULL,"

                                                  "username varchar(20) NOT NULL,"
                                                  "password varchar(20), "
                                                  "messagetype varchar(20) NOT NULL,"
                                                  "messageid varchar(20) NOT NULL,"
                                                  "version varchar(20) NOT NULL,"

                                                  "txdate_yyyymmdd date NOT NULL,"
                                                  "txtime_hhmmss time NOT NULL,"
                                                  "mallid varchar(20) NOT NULL,"
                                                  "storecode varchar(20) NOT NULL,"
                                                  "tillid varchar(20)  NULL,"
                                                  "txdocno varchar(20) NOT NULL,"
                                                  "cashier varchar(20)  NULL,"
                                                  "salesman varchar(20)  NULL,"

                                                  "itemcode varchar(20) NOT NULL,"
                                                  "qty DECIMAL(10,2) NOT NULL,"
                                                  "netamount DECIMAL(10,2) NOT NULL,"

                                                  "ch_uploadflag varchar(1) NULL,"
                                                  "ch_uploadmemo varchar(100) NULL"

                                                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
                                                  <<
                                                  "CREATE TABLE IF NOT EXISTS cey_web_data_collection_tendercode("
                                                  "id varchar(20) NOT NULL,"
                                                  "tendercode varchar(10) NOT NULL,"
                                                  "baseamount DECIMAL(10,2) NULL"
                                                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
                                                  ,  ("1.6 ")//南京国际广场的数据采集")
                                                  ));
    //1.7 大类权限
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  <<
                                                  "CREATE TABLE IF NOT EXISTS cey_bt_dish_type_oper("
                                                  "vch_operid varchar(10) NOT NULL,"
                                                  "ch_dish_typeno varchar(2) NOT NULL,"
                                                  "ch_state char(1) NULL"
                                                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
                                                  ,  ("1.7 ")//大类权限")
                                                  ));
    //1.8 库存入库失败
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  ,  ("1.8")// 库存入库失败")
                                                  ));
    //1.9 优惠
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  << QString("update cey_bt_paymode set vch_paymodename = '%1' where ch_paymodeno='ZZ' ")
                                                  .arg(QObject::tr("优惠"))
                                                  ,  ("1.9 ")//优惠")
                                                  ));

    //2.0微信支付信息
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  <<
                                                  "CREATE TABLE IF NOT EXISTS cey_web_weixinpay("
                                                  "out_trade_no nvarchar(32) NOT NULL UNIQUE,"
                                                  "appid nvarchar(32) NOT NULL,"
                                                  "appkey nvarchar(128) NOT NULL,"
                                                  "mch_id nvarchar(32) NOT NULL"
                                                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
                                                  ,  ("2.0")//微信支付信息")
                                                  ));
    //2.1 数据库入库审核失败和操作失败
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  <<
                                                  "    DROP TRIGGER IF EXISTS tr_st_sheet_master_update;\n"
                                                  "    CREATE TRIGGER tr_st_sheet_master_update AFTER UPDATE ON cey_st_sheet_master FOR EACH ROW \n"
                                                  "    BEGIN\n"
                                                  "        DECLARE m_de int default 0;\n"
                                                  "        DECLARE m_done int default 0;\n"
                                                  "        DECLARE sheetNo  varchar(12);\n"
                                                  "        DECLARE sheetType varchar(2);\n"
                                                  "        DECLARE operation varchar(3);\n"
                                                  "        DECLARE dtaudit datetime;\n"
                                                  "        DECLARE materialno varchar(7);\n"
                                                  "        DECLARE numnum numeric(16,4);\n"
                                                  "        DECLARE numpresent numeric(15,4);\n"
                                                  "        DECLARE numprice numeric(16,4);\n"
                                                  "        DECLARE costprice  numeric(16,4);\n"
                                                  "        DECLARE unitrate numeric(16,2);\n"
                                                  "        DECLARE intflowId integer;\n"
                                                  "        DECLARE initqty   numeric(16,4);\n"
                                                  "        DECLARE initprice numeric(16,4);\n"
                                                  "        DECLARE remainqty numeric(16,4);\n"
                                                  "        DECLARE remainprice numeric(16,4);\n"
                                                  "        DECLARE num_amount numeric(16,2);\n"
                                                  "        DECLARE cur_rows cursor for\n"
                                                  "          select b.ch_materialno, b.num_wrapping, b.num_present,  b.num_price, ifnull(c.int_unit_rate,1)\n"
                                                  "          from  cey_st_sheet_detail b, cey_bt_dish c\n"
                                                  "          where NEW.ch_sheetNo = b.ch_sheetno and b.ch_materialno = c.ch_dishno;\n"
                                                  "          DECLARE continue handler for not found set m_done = 1;\n"
                                                  "       open cur_rows;\n"
                                                  "       fetch cur_rows into  materialno, numnum, numpresent, numprice, unitrate;\n"
                                                  "     WHILE (m_done = 0) DO \n"
                                                  "          IF(NEW.ch_state = 'Y')THEN\n"
                                                  "            set sheetNo = NEW.ch_sheetNo ;\n"
                                                  "            set sheetType = NEW.ch_sheetType ;\n"
                                                  "            set operation = NEW.ch_operation ;\n"
                                                  "            set dtaudit = NEW.dt_audit ;\n"
                                                  "            set intflowId = (select ifnull(max(int_flowId),0) from cey_st_material_flow  where ch_materialno = materialno);\n"
                                                  "            IF (intflowId = 0) THEN \n"
                                                  "              set initqty =  0 ;\n"
                                                  "            ELSE\n"
                                                  "              set initqty = (select remain_qty from cey_st_material_flow where int_flowId = intflowId);\n"
                                                  "            END IF;\n"
                                                  "            IF (intflowId = 0) THEN\n"
                                                  "              set initprice =  0 ;\n"
                                                  "            ELSE\n"
                                                  "              set initprice = (select remain_price from cey_st_material_flow where int_flowId = intflowId);\n"
                                                  "            END IF;\n"
                                                  "            IF (sheetType = 'RK')  THEN\n"
                                                  "                 set remainqty = initqty + numnum;\n"
                                                  "                 IF (remainqty = 0) THEN\n"
                                                  "                    set remainprice = 0;\n"
                                                  "                 ELSE\n"
                                                  "                    set remainprice = (abs(((numnum / unitrate) * numprice  + initqty * initprice) / remainqty));\n"
                                                  "                 END IF;\n"
                                                  "                 insert cey_st_material_flow (ch_materialno, dt_operdate, sheettype, int_flag,\n"
                                                  "                           num_num, sheet_price, cost_price, chg_amount, remain_qty, remain_price, ch_sheetno)\n"
                                                  "                   values (materialno, now(), sheetType, 1, numnum, numprice / unitrate, remainqty, 0, remainqty, remainprice, sheetNo);\n"
                                                  "                 IF (numpresent > 0) THEN\n"
                                                  "                      set initqty = remainqty;\n"
                                                  "                      set initprice = remainprice;\n"
                                                  "                      set remainqty = initqty + numpresent;\n"
                                                  "                      IF (remainqty = 0) THEN\n"
                                                  "                         set remainprice = 0;\n"
                                                  "                      ELSEIF (initqty < 0 and remainqty > 0) THEN \n"
                                                  "                             set remainprice = numprice / unitrate;\n"
                                                  "                      ELSE \n"
                                                  "                            set remainprice = abs((initqty * initprice) / remainqty);\n"
                                                  "                      END IF;\n"
                                                  "                      insert cey_st_material_flow (ch_materialno, dt_operdate, sheettype, int_flag,\n"
                                                  "                             num_num, sheet_price, cost_price, chg_amount, remain_qty, remain_price, ch_sheetno)\n"
                                                  "                      values (materialno, now(), sheetType, 1, numpresent, 0, remainprice, 0, remainqty, remainprice, sheetNo);\n"
                                                  "                 END IF;\n"
                                                  "                     IF exists ( Select * from cey_sys_parameter where vch_parameter = 'sys_stock_price' and vch_value = '1') THEN\n"
                                                  "                    update cey_bt_dish set num_sale_price = numprice where ch_dishno = materialno;\n"
                                                  "                 END IF;\n"
                                                  "                 IF exists (select 1 from cey_st_material_num where ch_materialno = materialno) THEN\n"
                                                  "                  update cey_st_material_num set num_num = remainqty, cost_price = remainprice\n"
                                                  "                   where ch_materialno = materialno;\n"
                                                  "                 ELSE \n"
                                                  "                   insert into cey_st_material_num (ch_materialno, num_num, cost_price)\n"
                                                  "                     values (materialno, remainqty, remainprice);\n"
                                                  "                 END IF;\n"
                                                  "            ELSEIF (sheetType = 'TH')  THEN\n"
                                                  "                 set remainqty = initqty - numnum;\n"
                                                  "                 IF (remainqty = 0) THEN\n"
                                                  "                    set remainprice = 0;\n"
                                                  "                 ELSE\n"
                                                  "                    set remainprice = (abs((initqty * initprice - numnum / unitrate * numprice   ) / remainqty));\n"
                                                  "                 END IF;\n"
                                                  "                 insert cey_st_material_flow (ch_materialno, dt_operdate, sheettype, int_flag,\n"
                                                  "                           num_num, sheet_price, cost_price, chg_amount, remain_qty, remain_price, ch_sheetno)\n"
                                                  "                   values (materialno, now(), sheetType, 1, numnum, numprice / unitrate, remainprice, 0, remainqty, remainprice, sheetNo);\n"
                                                  "                 IF (numpresent > 0) THEN \n"
                                                  "                      set initqty   = remainqty;\n"
                                                  "                      set initprice = remainprice;\n"
                                                  "                      set remainqty = initqty - numpresent;\n"
                                                  "                      IF (remainqty = 0) THEN\n"
                                                  "                         set remainprice = 0;\n"
                                                  "                      ELSE\n"
                                                  "                           set remainprice = abs((initqty * initprice) / remainqty);\n"
                                                  "                      END IF;\n"
                                                  "                      insert cey_st_material_flow (ch_materialno, dt_operdate, sheettype, int_flag,\n"
                                                  "                             num_num, sheet_price, cost_price, chg_amount, remain_qty, remain_price, ch_sheetno)\n"
                                                  "                      values (materialno, now(), sheetType, 1, numpresent, 0, remainprice, 0, remainqty, remainprice, sheetNo);\n"
                                                  "                  END IF;\n"
                                                  "                 IF exists (select 1 from cey_st_material_num where ch_materialno = materialno) THEN\n"
                                                  "                  update cey_st_material_num set num_num = remainqty, cost_price = remainprice\n"
                                                  "                   where ch_materialno = materialno;\n"
                                                  "                 ELSE \n"
                                                  "                   insert into cey_st_material_num (ch_materialno, num_num, cost_price)\n"
                                                  "                     values (materialno, remainqty, remainprice);\n"
                                                  "                 END IF;\n"
                                                  "            ELSEIF (sheetType = 'CK') THEN\n"
                                                  "                 set remainqty = initqty - numnum;\n"
                                                  "                 IF (remainqty = 0) THEN\n"
                                                  "                    set remainprice = 0;\n"
                                                  "                 ELSEIF (initqty < 0 and remainqty > 0) THEN\n"
                                                  "                    set m_de = 1;\n"
                                                  "                 ELSE \n"
                                                  "                    set remainprice = abs((initqty * initprice - numnum / unitrate * numprice   ) / remainqty);\n"
                                                  "                  END IF;\n"
                                                  "                 insert cey_st_material_flow (ch_materialno, dt_operdate, sheettype, int_flag,\n"
                                                  "                           num_num, sheet_price, cost_price, chg_amount, remain_qty, remain_price, ch_sheetno)\n"
                                                  "                   values (materialno, now(), sheetType, 1, numnum, numprice/unitrate, remainprice, 0, remainqty, remainprice, sheetNo);\n"
                                                  "                 IF exists (select 1 from cey_st_material_num where ch_materialno = materialno) THEN\n"
                                                  "                  update cey_st_material_num set num_num = remainqty, cost_price = remainprice\n"
                                                  "                   where ch_materialno = materialno;\n"
                                                  "                 ELSE \n"
                                                  "                   insert into cey_st_material_num (ch_materialno, num_num, cost_price)\n"
                                                  "                     values (materialno, remainqty, remainprice);\n"
                                                  "                 END IF;\n"
                                                  "            END IF;\n"
                                                  "          END IF;\n"
                                                  "          fetch cur_rows into  materialno, numnum, numpresent, numprice, unitrate;\n"
                                                  "     END WHILE;\n"
                                                  "      close cur_rows;\n"
                                                  "    END\n"
                                                  ,  ("2.1")// 数据库入库审核失败和操作失败")
                                                  ));
    //2.2 支付方式里新增微信支付， 支付宝支付， 百度糯米， 大众，美团/ 数据库新增强制升级flag
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  << "replace into cey_bt_paymode(ch_paymodeno, vch_paymodename, ch_IDflag, ch_faceflag, ch_changeflag, ch_incomeflag, ch_key) values('06','支付宝支付','N', 'N','N', 'Y', '');"
                                                  << "replace into cey_bt_paymode(ch_paymodeno, vch_paymodename, ch_IDflag, ch_faceflag, ch_changeflag, ch_incomeflag, ch_key) values('07','百度糯米','N', 'N','N', 'Y', '');"
                                                  << "replace into cey_bt_paymode(ch_paymodeno, vch_paymodename, ch_IDflag, ch_faceflag, ch_changeflag, ch_incomeflag, ch_key) values('08','大众点评','N', 'N','N', 'Y', '');"
                                                  << "replace into cey_bt_paymode(ch_paymodeno, vch_paymodename, ch_IDflag, ch_faceflag, ch_changeflag, ch_incomeflag, ch_key) values('09','美团','N', 'N','N', 'Y', '');"

                                                  << "alter table cey_bt_paymode add column vch_arg1  VARCHAR(128);"
                                                  << "alter table cey_bt_paymode add column vch_arg2  VARCHAR(128);"
                                                  << "alter table cey_bt_paymode add column vch_arg3  VARCHAR(128);"
                                                  << "alter table cey_bt_paymode add column vch_arg4  VARCHAR(128);"
                                                  ,  ("2.2")// 支付方式里新增微信支付， 支付宝支付， 百度糯米， 大众，美团")
                                                  ));
    //2.3点菜宝新增字段--》vch_wirelessno ，系统加最近的无线点采机的流水号
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  << "alter table cey_u_orderdish add column vch_wirelessno  VARCHAR(20);"//Y0042 00001 0400: 00:33
                                                  << "replace into cey_sys_parameter(vch_parameter, vch_value, vch_explain) values('last_wirelessno', 0, '无线点菜最后流水');"
                                                  ,  ("2.3")//点菜宝新增字段")
                                                  ));
    //2.4新增支付方式10(云会员卡)
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  << QString("replace into cey_bt_paymode(ch_paymodeno, vch_paymodename, ch_IDflag, ch_faceflag, ch_changeflag, ch_incomeflag, ch_key) values('10','%1','N', 'N','N', 'Y', '');")
                                                  .arg(QObject::tr("云会员卡"))
                                                  ,  ("2.4")//新增支付方式10(云会员卡)")
                                                  ));
    //2.5新增支付方式11(会员挂账)
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  << QString("replace into cey_bt_paymode(ch_paymodeno, vch_paymodename, ch_IDflag, ch_faceflag, ch_changeflag, ch_incomeflag, ch_key) values('11','%1','N', 'N','N', 'Y', '');")
                                                  .arg(QObject::tr("会员挂账"))
                                                  ,  ("2.5")//新增支付方式11(会员挂账)")
                                                  ));
    //2.6软件对支付方式进行分类：N一般付款；S特殊处理；T团购
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  << "update cey_bt_paymode set vch_arg4 = 'N' where ch_paymodeno in ('00', '01', '02', '03', '04', '05', '06', '10');"
                                                  << "update cey_bt_paymode set vch_arg4 = 'S' where ch_paymodeno in ('11', 'XX', 'YY', 'ZZ');"
                                                  << "update cey_bt_paymode set ch_incomeflag = 'N' where ch_paymodeno = '11' ;"
                                                  << "update cey_bt_paymode set vch_arg2 = 'http://login.nuomi.com/account/login',vch_arg4 = 'T' where ch_paymodeno = '07';"
                                                  << "update cey_bt_paymode set vch_arg2 = 'http://www.dianping.com/login',vch_arg4 = 'T' where ch_paymodeno = '08';"
                                                  << "update cey_bt_paymode set vch_arg2 = 'http://e.meituan.com/account/login',vch_arg4 = 'T' where ch_paymodeno = '09';"

                                                  ,  ("2.6")//软件对支付方式进行分类：N一般付款；S特殊处理；T团购")
                                                  ));
    //2.7厨打表属性名更新、后台票尾被高倍宽追加字段
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  << "alter table cey_bt_kitchen_print change column vch_printport vch_printtype VARCHAR(32);"
                                                  << "alter table cey_bt_kitchen_print add column vch_instype  VARCHAR(32);"

                                                  ,  ("2.7")//厨打表属性名更新、后台票尾被高倍宽追加字段")
                                                  ));
    //2.8 升级cey_u_orderdish vch_print_memo 设置varchar -》 TinyBlob，方便插入图片，二进制流等数据
    //TinyBlob 最大 255
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  << "alter table cey_sys_parameter change column vch_value vch_value VARCHAR(511);"
                                                  ,  ("2.8")//厨打表属性名更新、后台票尾被高倍宽追加字段")
                                                  ));
    //2.9

    //    vch_plan_id, vch_plan_name,
    //    int_suit_type, int_dish_type, int_print_count,
    //    int_tail_block, int_print_id_second, vch_plan_title, vch_plan_tail,int_print_id

    //    ch_dishno,
    //            ch_print_flag, int_print_id1, int_print_id2, int_print_id3，
    //                ch_out_flag, int_out_id1, int_out_id2, int_out_id3
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  ///云端添加机器号
                                                  << " alter table cey_u_checkout_detail drop column vch_machine_id ;"
                                                  << " alter table cey_u_checkout_master drop column vch_machine_id ;"
                                                  << " alter table cey_u_orderdish drop column vch_machine_id ;"
                                                  << " alter table cey_u_service_low drop column vch_machine_id ;"
                                                  << " alter table cey_u_orderdish_log drop column vch_machine_id ;"

                                                  << " alter table cey_u_togo drop column vch_machine_id ;"
                                                  << " alter table cey_u_table drop column vch_machine_id ;"

                                                  << " alter table t_m_deposit drop column vch_machine_id ;"
                                                  << " alter table t_m_point drop column vch_machine_id ;"
                                                  << " alter table t_m_pay drop column vch_machine_id ;"

                                                  << " alter table cey_u_checkout_detail add column terminal_code varchar(128) default '' ;"
                                                  << " alter table cey_u_checkout_master add column terminal_code varchar(128) default '' ;"
                                                  << " alter table cey_u_orderdish add column terminal_code varchar(128) default '' ;"
                                                  << " alter table cey_u_service_low add column terminal_code varchar(128) default '' ;"
                                                  << " alter table cey_u_orderdish_log add column terminal_code varchar(128) default '' ;"

                                                  << " alter table cey_u_togo add column terminal_code varchar(128) default '' ;"
                                                  << " alter table cey_u_table add column terminal_code varchar(128) default '' ;"

                                                  << " alter table t_m_deposit add column terminal_code varchar(128) default '' ;"
                                                  << " alter table t_m_point add column terminal_code varchar(128) default '' ;"
                                                  << " alter table t_m_pay add column terminal_code varchar(128) default '' ;"

                                                  //                                                  << "alter table cey_u_checkout_master add column vch_service_desc  VARCHAR(32);"///添加服务费描述
                                                  << "alter table cey_u_orderdish_log add column num_back_cur  decimal(12,2);"///添加退货实时数量
                                                  << "alter table cey_bt_kitchen_print add column int_paperwidth  int(4);"///添加cey_bt_kitchen_print——int_paperwidth
                                                  << "alter table cey_bt_kitchen_print add column ch_areaNo  VARCHAR(2);"///添加cey_bt_kitchen_print——ch_areaNo
                                                  << "alter table cey_u_orderdish add column vch_print_desc  VARCHAR(60);"///cey_u_orderdish 更新厨打出品说明字段 vch_print_desc
                                                  << "alter table cey_u_orderdish change column vch_print_desc vch_wirelessno_desc  VARCHAR(60);"
                                                  << "delete from cey_bt_paymode where ch_paymodeno = 'XX';"///取消免单

                                                  <<"CREATE TABLE IF NOT EXISTS cey_bt_kitchen_plan("///厨打方案
                                                  "vch_plan_id varchar(2) primary key NOT NULL,"
                                                  "vch_plan_name varchar(32),"
                                                  "int_print_id int(2),"
                                                  "int_print_id_second int(2),"
                                                  "int_suit_type int(2),"
                                                  "int_dish_type int(2),"
                                                  "int_print_count int(2),"
                                                  "int_tail_block int(2),"
                                                  "vch_plan_title varchar(100),"
                                                  "vch_plan_tail varchar(100)"
                                                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"

                                                  << "alter table cey_bt_dish add column ch_kitchen_print_flag varchar(1);"///添加cey_bt_dish——ch_kitchen_print_flag
                                                  << "alter table cey_bt_dish add column vch_kitchen_print_id1 varchar(2);"///添加cey_bt_dish——vch_kitchen_print_id1
                                                  << "alter table cey_bt_dish add column vch_kitchen_print_id2 varchar(2);"///添加cey_bt_dish——vch_kitchen_print_id2
                                                  << "alter table cey_bt_dish add column vch_kitchen_print_id3 varchar(2);"///添加cey_bt_dish——vch_kitchen_print_id3

                                                  << "alter table cey_bt_dish add column ch_kitchen_out_flag varchar(1);"///添加cey_bt_dish——ch_kitchen_out_flag
                                                  << "alter table cey_bt_dish add column vch_kitchen_out_id1 varchar(2);"///添加cey_bt_dish——vch_kitchen_out_id1
                                                  << "alter table cey_bt_dish add column vch_kitchen_out_id2 varchar(2);"///添加cey_bt_dish——vch_kitchen_out_id2
                                                  << "alter table cey_bt_dish add column vch_kitchen_out_id3 varchar(2);"///添加cey_bt_dish——vch_kitchen_out_id3

                                                  //cey_u_orderdish_log
                                                  << "alter table cey_u_orderdish add column vch_plan_k3o3_id  VARCHAR(12);"///厨打方案：112233--5566
                                                  << "alter table cey_u_orderdish add column vch_plan_k3o3_state  VARCHAR(6);"///厨打状态 111-11

                                                  << "alter table cey_u_hungbill add column vch_plan_k3o3_id  VARCHAR(12);"///厨打方案：112233--5566
                                                  << "alter table cey_u_hungbill add column vch_plan_k3o3_state  VARCHAR(6);"///厨打状态 111-11

                                                  << "alter table cey_u_orderdish_log add column vch_plan_k3o3_state  VARCHAR(6);"///厨打状态 111-11
                                                  //                                                  << "alter table cey_u_orderdish drop ch_printflag; "
                                                  //                                                  << "alter table cey_u_orderdish drop ch_outflag; "
                                                  << "alter table cey_u_orderdish drop vch_wirelessno_desc; "
                                                  << "alter table cey_u_orderdish drop vch_wirelessno; "
                                                  << "alter table cey_u_orderdish add column vch_plan_desc  VARCHAR(1023);"///厨打状态说明 111-11
                                                  << "update cey_u_table set ch_order_state= 4 where ifnull(ch_payno, '') <> '' "///若已有餐桌消费信息则，将ch_order_state置为付款结束(4)

                                                  ,  "2.9 "//大类权限")
                                                  ));

    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()

                                                  << "drop table `1`"
                                                  << "alter table cey_bt_kitchen_plan add column int_head_block  int(2);"
                                                  << "delete from cey_bt_class"
                                                  << "alter table cey_bt_kitchen_plan add column ch_check_state  varchar(1);"

                                                  << "alter table cey_bt_dish add column ch_groupno  VARCHAR(7)"

                                                  << "alter table cey_bt_paymode add column vch_arg5  VARCHAR(128)"
                                                  << "alter table cey_bt_paymode add column vch_arg6  VARCHAR(128)"
                                                  << "alter table cey_bt_paymode add column vch_arg7  VARCHAR(128)"

                                                  << "alter table cey_bt_kitchen_plan add column ch_print_id_second_use  VARCHAR(1)"

                                                  <<"alter table cey_u_hungbill change column vch_print_memo vch_print_memo VARCHAR(1023)"
                                                  <<"alter table cey_u_orderdish change column vch_print_memo vch_print_memo VARCHAR(1023)"
                                                  <<"alter table cybr_book_detail change column vch_print_memo vch_print_memo VARCHAR(1023)"
                                                  <<"alter table cey_u_orderdish_log add column  vch_print_memo VARCHAR(1023)"
                                                  <<"alter table cey_u_orderdish_log add column  num_price_add decimal(12,2)"

                                                  //更新本地数据x14 ch_yun_already_keep
                                                  << SQL_ch_yun_already_keep("cey_bt_dish")
                                                  << SQL_ch_yun_already_keep("cey_bt_dish_subtype")
                                                  <<SQL_ch_yun_already_keep("cey_bt_dish_type")
                                                  <<SQL_ch_yun_already_keep("cey_bt_dish_suit")
                                                  <<SQL_ch_yun_already_keep("cey_bt_suit_change")

                                                  <<SQL_ch_yun_already_keep("cey_bt_unit")
                                                  <<SQL_ch_yun_already_keep("cey_bt_table")
                                                  <<SQL_ch_yun_already_keep("cey_bt_table_type")
                                                  <<SQL_ch_yun_already_keep("cey_bt_table_area")
                                                  <<SQL_ch_yun_already_keep("cey_bt_cp_dish")

                                                  <<SQL_ch_yun_already_keep("cey_bt_cp_memo")
                                                  <<SQL_ch_yun_already_keep("cey_bt_cp_type")
                                                  <<SQL_ch_yun_already_keep("cey_bt_dish_warn")
                                                  <<SQL_ch_yun_already_keep("cey_bt_paymode")

                                                  //会员x5 ch_yun_already_keep
                                                  <<SQL_ch_yun_already_keep("t_m_member_type")
                                                  <<SQL_ch_yun_already_keep("t_m_member")
                                                  <<SQL_ch_yun_already_keep("t_m_gift")
                                                  <<SQL_ch_yun_already_keep("t_m_curamount")

                                                  //cey_web_table_version
                                                  << " CREATE TABLE IF NOT EXISTS cey_web_table_version("
                                                  " vch_sqltablename varchar(30) primary key NOT NULL,"
                                                  " vch_sqltablename_key_name  varchar(30) NOT NULL,"

                                                  " vch_sqltablename_yun varchar(30) NOT NULL,"
                                                  " vch_sqltablename_yun_key_name varchar(30) NOT NULL,"
                                                  " vch_sqltablename_yun_version varchar(30) NOT NULL,"
                                                  " int_version int(10) NOT NULL"
                                                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"


                                                  //基础资料x14
                                                  << "replace into cey_web_table_version values( 'cey_bt_cp_dish', 'ch_dishno,ch_no', 'dishPracticeList', 'dpChDishNo,dpChNo', 'dishPracticeVersion',ifnull((select t.int_version from cey_web_table_version t where t.vch_sqltablename = 'cey_bt_cp_dish'),0));"
                                                  << "replace into cey_web_table_version values('cey_bt_cp_memo', 'ch_no', 'dishPracticeMemoList', 'chNo', 'dishPracticeMemoVersion',ifnull((select t.int_version from cey_web_table_version t where t.vch_sqltablename = 'cey_bt_cp_memo'),0));"
                                                  << "replace into cey_web_table_version values('cey_bt_cp_type', 'ch_typeno', 'dishPracticeTypeList', 'dptChTypeNo', 'dishPracticeTypeVersion',ifnull((select t.int_version from cey_web_table_version t where t.vch_sqltablename = 'cey_bt_cp_type'),0));"
                                                  << "replace into cey_web_table_version values('cey_bt_dish', 'ch_dishno', 'dishInfoList', 'diChDishNo', 'dishInfoVersion',ifnull((select t.int_version from cey_web_table_version t where t.vch_sqltablename = 'cey_bt_dish'),0));"
                                                  << "replace into cey_web_table_version values('cey_bt_dish_subtype', 'ch_sub_typeno', 'dishSubTypeList', 'dstChSubTypeNo', 'dishSubTypeVersion',ifnull((select t.int_version from cey_web_table_version t where t.vch_sqltablename = 'cey_bt_dish_subtype'),0));"
                                                  << "replace into cey_web_table_version values('cey_bt_dish_suit', 'int_flowid', 'dishSuitList', 'dsIntFlowId', 'dishSuitVersion',ifnull((select t.int_version from cey_web_table_version t where t.vch_sqltablename = 'cey_bt_dish_suit'),0));"
                                                  << "replace into cey_web_table_version values('cey_bt_dish_type', 'ch_dish_typeno', 'dishTypeList', 'dtChDishTypeNo', 'dishTypeVersion',ifnull((select t.int_version from cey_web_table_version t where t.vch_sqltablename = 'cey_bt_dish_type'),0));"
                                                  << "replace into cey_web_table_version values('cey_bt_dish_warn', 'int_flowid', 'dishGuQingList', 'dgqIntFlowId', 'dishGuQingVersion',ifnull((select t.int_version from cey_web_table_version t where t.vch_sqltablename = 'cey_bt_dish_warn'),0));"
                                                  << "replace into cey_web_table_version values('cey_bt_paymode', 'ch_paymodeno', 'payModeList', 'pmChPayModeNo', 'payModeVersion',ifnull((select t.int_version from cey_web_table_version t where t.vch_sqltablename = 'cey_bt_paymode'),0));"
                                                  << "replace into cey_web_table_version values('cey_bt_suit_change', 'int_flowid,ch_dishno', 'dishSuitChangeList', 'dscIntFlowId,dscChDishNo', 'dishSuitChangeVersion',ifnull((select t.int_version from cey_web_table_version t where t.vch_sqltablename = 'cey_bt_suit_change'),0));"
                                                  << "replace into cey_web_table_version values('cey_bt_table', 'ch_tableno', 'dishTableList', 'dtChTableNo', 'dishTableVersion',ifnull((select t.int_version from cey_web_table_version t where t.vch_sqltablename = 'cey_bt_table'),0));"
                                                  << "replace into cey_web_table_version values('cey_bt_table_area', 'ch_areano', 'dishTableAreaList', 'dtaChAreaNo', 'dishTableAreaVersion',ifnull((select t.int_version from cey_web_table_version t where t.vch_sqltablename = 'cey_bt_table_area'),0));"
                                                  << "replace into cey_web_table_version values('cey_bt_table_type', 'ch_typeno', 'dishTableTypeList', 'dttChTyperNo', 'dishTableTypeVersion',ifnull((select t.int_version from cey_web_table_version t where t.vch_sqltablename = 'cey_bt_table_type'),0));"
                                                  << "replace into cey_web_table_version values('cey_bt_unit', 'ch_unitno', 'dishUnitList', 'duChUnitNo', 'dishUnitVersion',ifnull((select t.int_version from cey_web_table_version t where t.vch_sqltablename = 'cey_bt_unit'),0));"

                                                  //会员 x4
                                                  << "replace into cey_web_table_version values('t_m_curamount', 'vch_memberno', 'memberCuramountList', 'vchMemberno', 'memberCuramountVersion',ifnull((select t.int_version from cey_web_table_version t where t.vch_sqltablename = 't_m_curamount'),0));"
                                                  << "replace into cey_web_table_version values('t_m_gift', 'int_flowID', 'memberGiftList', 'intFlowid', 'memberGiftVersion',ifnull((select t.int_version from cey_web_table_version t where t.vch_sqltablename = 't_m_gift'),0));"
                                                  << "replace into cey_web_table_version values('t_m_member', 'vch_memberno', 'memberInfoList', 'miVchMemberNo', 'memberInfoVersion',ifnull((select t.int_version from cey_web_table_version t where t.vch_sqltablename = 't_m_member'),0));"
                                                  << "replace into cey_web_table_version values('t_m_member_type', 'ch_typeno', 'memberTypeList', 'chTypeno', 'memberTypeVersion',ifnull((select t.int_version from cey_web_table_version t where t.vch_sqltablename = 't_m_member_type'),0));"

                                                  ///单独设置会员
                                                  <<SQL_ch_yun_already_keep("t_m_parameter")
                                                  ///流水单独设置
                                                  <<SQL_ch_yun_already_keep("cey_u_checkout_detail")
                                                  <<SQL_ch_yun_already_keep("cey_u_checkout_master")
                                                  <<SQL_ch_yun_already_keep("cey_u_orderdish")
                                                  <<SQL_ch_yun_already_keep("cey_u_orderdish_log")
                                                  <<SQL_ch_yun_already_keep("cey_u_service_low")
                                                  <<SQL_ch_yun_already_keep("t_m_deposit")
                                                  <<SQL_ch_yun_already_keep("t_m_point")
                                                  <<SQL_ch_yun_already_keep("t_m_pay")
                                                  <<SQL_ch_yun_already_keep("cey_u_togo")
                                                  <<SQL_ch_yun_already_keep("cey_u_table")

                                                  //中餐下点菜宝流水号
                                                  <<"alter table cey_u_orderdish add column vch_wx_serial  VARCHAR(32)"
                                                  <<"alter table t_m_hang_balance change column ch_balanceno ch_balanceno VARCHAR(15)"
                                                  <<"alter table t_m_pay change column ch_balanceno ch_balanceno VARCHAR(15)"
                                                  <<"alter table t_m_pay change column ch_balanceno ch_balanceno VARCHAR(15)"
                                                  ,  "3.0"
                                                  ));


    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList() <<
                                                  "    DROP procedure IF EXISTS get_time;\n"
                                                  "CREATE procedure get_time( str1 CHAR(25), str2 CHAR(25),temp1 CHAR(25),temp2 CHAR(25)) \n"

                                                  "BEGIN \n"
                                                  "declare beginTime datetime;\n"
                                                  "declare endTime datetime;\n"

                                                  "IF (select count(dt_begin) from cey_u_day_sumup where dt_date =str1 limit 1)>0 THEN\n"
                                                  "set beginTime = (select dt_begin from cey_u_day_sumup where dt_date =str1 limit 1);\n"
                                                  "ELSE \n"
                                                  "IF (select count(1) from cey_u_day_sumup where dt_date <= str1 )>0 THEN\n"
                                                  "set beginTime = (select  IFNULL( MAX(dt_end),temp1) from cey_u_day_sumup where dt_date <= str1);\n"
                                                  "ELSE \n"
                                                  "set beginTime = (select  IFNULL( MIN(dt_begin),temp1) from cey_u_day_sumup where dt_date >= str1); \n"
                                                  "END IF; \n"
                                                  "END IF;\n"

                                                  "IF (select count(dt_end) from cey_u_day_sumup where dt_date =str2 limit 1)>0 THEN\n"
                                                  "set endTime = (select dt_end from cey_u_day_sumup where dt_date =str2 limit 1);\n"
                                                  "ELSE \n"
                                                  "IF (select count( 1 ) from cey_u_day_sumup where dt_date <= str2)>0 THEN\n"
                                                  "	set endTime = (select IFNULL(MAX(dt_end),temp2) from cey_u_day_sumup where dt_date <= str2 ); \n"
                                                  "ELSE \n"
                                                  "set endTime = (select IFNULL(MIN(dt_begin),temp2) from cey_u_day_sumup where dt_date >= str2); \n"
                                                  "END IF;\n""END IF; \n"

                                                  " SELECT beginTime,endTime;\n"

                                                  "END\n"
                                                  <<
                                                  "    DROP procedure IF EXISTS get_time2;\n"
                                                  "CREATE procedure get_time2(str1 VARCHAR(25),str2 VARCHAR(25)) \n"

                                                  "BEGIN\n"
                                                  "declare beginTime datetime ;\n"
                                                  "declare endTime datetime;\n"
                                                  "set beginTime=(Select dt_begin from cey_u_day_sumup where dt_date =str1);\n"
                                                  "set endTime=(Select dt_end from cey_u_day_sumup where dt_date =str2);\n"
                                                  "SELECT beginTime,endTime;\n" "END\n"
                                                  <<
                                                  "    DROP procedure IF EXISTS get_time3;\n"
                                                  "CREATE procedure get_time3(str1 VARCHAR(25),str2 VARCHAR(25)) \n"

                                                  "BEGIN\n"
                                                  "declare beginTime datetime ;\n"
                                                  "declare endTime datetime;\n"
                                                  "set beginTime=(Select dt_begin from cey_u_day_sumup where dt_date =str1);\n"
                                                  "set endTime=(Select dt_end from cey_u_day_sumup where dt_date =str2);\n"
                                                  "SELECT beginTime,endTime;\n" "END\n"
                                                  <<
                                                  "    DROP procedure IF EXISTS pr_m_deposit_point_detail;\n"
                                                  "CREATE procedure pr_m_deposit_point_detail(sdate VARCHAR(12),edate VARCHAR(12),memberno VARCHAR(12)) \n"

                                                  "BEGIN\n"

                                                  "DROP TABLE IF EXISTS temp;\n"

                                                  "CREATE TEMPORARY TABLE temp(memberno varchar(20) not null,membername varchar(40) null,typeno varchar(30) null,\n"
                                                  "smoney decimal(18,2) default 0,spoint decimal(18,2) default 0,mpay decimal(18,2) default 0,\n"
                                                  "mdeposit	decimal(18,2) default 0,	mpoint 	  decimal(18,2) default 0, emoney decimal(18,2) default 0,\n"
                                                  "epoint decimal(18,2) default 0,dpdate datetime null,\n"
                                                  "memo varchar(100) null);\n"

                                                  "INSERT INTO temp\n"
                                                  "SELECT A.vch_memberno memberno,B.vch_member vch_member,(select vch_typename from t_m_member_type where ch_typeno = B.ch_typeno limit 0,1) typeno,\n"
                                                  "IFNULL((select sum(IFNULL(num_deposit,0)) from t_m_deposit where vch_memberno = A.vch_memberno and dt_operdate < A.dt_operdate ),0) -\n"
                                                  " IFNULL((select sum(IFNULL(num_pay,0)) from t_m_pay where vch_memberno = A.vch_memberno and dt_operdate < A.dt_operdate ),0) + \n"
                                                  "IFNULL(num_limit,0) smoney,\n"
                                                  " IFNULL((select sum(IFNULL(num_point,0)) from t_m_point where vch_memberno = A.vch_memberno and dt_operdate < A.dt_operdate ),0) spoint,\n"
                                                  "  0 mpay,	IFNULL(num_deposit,0) mdeposit, 0 mpoint,\n"
                                                  " IFNULL((select sum(IFNULL(num_deposit,0)) from t_m_deposit where vch_memberno = A.vch_memberno and dt_operdate < A.dt_operdate ),0) -\n"
                                                  "  IFNULL((select sum(IFNULL(num_pay,0)) from t_m_pay where vch_memberno = A.vch_memberno and dt_operdate < A.dt_operdate ),0) + IFNULL(num_deposit,0) + IFNULL(num_limit,0) emoney,\n"
                                                  " IFNULL((select sum(IFNULL(num_point,0)) from t_m_point where vch_memberno = A.vch_memberno and dt_operdate <= A.dt_operdate ),0) epoint,\n"
                                                  " A.dt_operdate dpdate, case when A.ch_deposit_mode = '3' and ch_pay_mode = '5' then '系统付款返现' + IFNULL(A.vch_memo,'') else A.vch_memo end memo\n"
                                                  " FROM t_m_deposit A, t_m_member B WHERE A.vch_memberno = B.vch_memberno AND LEFT(A.dt_operdate,10) >= sdate AND LEFT(A.dt_operdate,10) <= edate AND A.vch_memberno = memberno;\n"

                                                  "INSERT INTO temp\n"
                                                  "SELECT A.vch_memberno memberno,B.vch_member vch_member,(select vch_typename from t_m_member_type where ch_typeno = B.ch_typeno LIMIT 0,1) typeno, \n"
                                                  "  IFNULL((select sum(IFNULL(num_deposit,0)) from t_m_deposit where vch_memberno = A.vch_memberno and dt_operdate < A.dt_operdate ),0) -\n"
                                                  " IFNULL((select sum(IFNULL(num_pay,0)) from t_m_pay where vch_memberno = A.vch_memberno and dt_operdate < A.dt_operdate ),0) + \n"
                                                  "IFNULL(num_limit,0)  smoney,\n"
                                                  "  IFNULL((select sum(IFNULL(num_point,0)) from t_m_point where vch_memberno = A.vch_memberno and dt_operdate < A.dt_operdate ),0) spoint,\n"
                                                  "  0 mpay,	0 mdeposit, IFNULL( num_point,0) mpoint,\n"
                                                  "  IFNULL((select sum(ifnull(num_deposit,0)) from t_m_deposit where vch_memberno = A.vch_memberno and dt_operdate <= A.dt_operdate ),0) -\n"
                                                  "IFNULL((select sum(ifnull(num_pay,0)) from t_m_pay where vch_memberno = A.vch_memberno and dt_operdate <= A.dt_operdate ),0) + IFNULL(num_limit,0) emoney,\n"
                                                  "  IFNULL((select sum(ifnull(num_point,0)) from t_m_point where vch_memberno = A.vch_memberno and dt_operdate < A.dt_operdate ),0) + IFNULL(num_point,0)  epoint,A.dt_operdate dpdate, case when A.ch_type = '3' then '消费系统积分' + IFNULL(A.vch_memo,'') else A.vch_memo end memo\n"
                                                  " FROM t_m_point A, t_m_member B WHERE A.vch_memberno = B.vch_memberno AND IFNULL( A.num_point,0) <> 0	AND  LEFT(A.dt_operdate,10) >= sdate AND LEFT(A.dt_operdate,10) <= edate AND A.vch_memberno = memberno;\n"

                                                  "INSERT INTO temp\n"
                                                  "SELECT A.vch_memberno memberno,B.vch_member vch_member,(select vch_typename from t_m_member_type where ch_typeno = B.ch_typeno LIMIT 0,1) typeno, \n"
                                                  "  IFNULL((select sum(IFNULL(num_deposit,0)) from t_m_deposit where vch_memberno = A.vch_memberno and dt_operdate < A.dt_operdate ),0) -\n"
                                                  "  IFNULL((select sum(IFNULL(num_pay,0)) from t_m_pay where vch_memberno = A.vch_memberno and dt_operdate < A.dt_operdate ),0) + IFNULL(num_limit,0) smoney, \n"
                                                  "   IFNULL((select sum(IFNULL(num_point,0)) from t_m_point where vch_memberno = A.vch_memberno and dt_operdate < A.dt_operdate ),0) spoint,\n"
                                                  "   IFNULL(num_pay,0) mpay,0 mdeposit, 0 mpoint,\n"
                                                  "   IFNULL((select sum(IFNULL(num_deposit,0)) from t_m_deposit where vch_memberno = A.vch_memberno and dt_operdate <= A.dt_operdate ),0) -\n"
                                                  " IFNULL((select sum(IFNULL(num_pay,0)) from t_m_pay where vch_memberno = A.vch_memberno and dt_operdate < A.dt_operdate ),0) - IFNULL(num_pay,0) + IFNULL(num_limit,0) emoney,\n"
                                                  "  IFNULL((select sum(IFNULL( num_point,0)) from t_m_point where vch_memberno = A.vch_memberno and dt_operdate <= A.dt_operdate ),0) epoint,\n"
                                                  " A.dt_operdate dpdate, '' memo\n"
                                                  " FROM t_m_pay A, t_m_member B WHERE A.vch_memberno = B.vch_memberno AND LEFT(A.dt_operdate,10) >= sdate AND LEFT(A.dt_operdate,10) <= edate AND A.vch_memberno = memberno;\n"

                                                  "SELECT * FROM temp ORDER BY dpdate;\n"

                                                  "END\n"
                                                  << "    DROP procedure IF EXISTS sys_set_permissions;\n"
                                                  "CREATE procedure sys_set_permissions(id VARCHAR(1000),list VARCHAR(1000))\n"

                                                  "BEGIN\n"
                                                  "DECLARE ix int;\n"
                                                  "DECLARE pos int;\n"
                                                  " DECLARE str varchar(1000);\n"
                                                  "SET pos = 1 ;\n"
                                                  " SET ix = 1 ;\n"

                                                  "delete from cey_sys_oper_purview where vch_operID = id ;\n"
                                                  "WHILE ix > 0 do\n"

                                                  "SET ix = LOCATE(',', list, pos); \n"
                                                  "IF ix > 0 THEN\n"
                                                  "SET str = SUBSTRING(list, pos, ix - pos);\n"
                                                  "ELSE \n"
                                                  "SET str = substring(list, pos, LENGTH(list));\n"
                                                  "end if;\n"
                                                  "insert into cey_sys_oper_purview values(id,str) ;\n"
                                                  "SET pos = ix + 1 ;\n" "END WHILE;\n" "END\n"
                                                  << " DROP procedure IF EXISTS sys_settings;\n"
                                                  "CREATE procedure sys_settings(attribute VARCHAR(20000)) \n"

                                                  "BEGIN\n"
                                                  " DECLARE ix int;\n"
                                                  "DECLARE pos int;\n"
                                                  " DECLARE attributeLenght int;\n"
                                                  "DECLARE str1 varchar(30);\n"
                                                  " DECLARE str2 varchar(60);\n"
                                                  " DECLARE str3 varchar(30);\n"
                                                  "SET pos = 1 ;\n"
                                                  " SET ix = 1 ;\n"
                                                  " SET attributeLenght = LENGTH(attribute);\n"

                                                  "loop_label:WHILE ix>0 do\n"

                                                  "SET ix = LOCATE('&,&', attribute, pos);\n"
                                                  "SET str1 = SUBSTRING(attribute, pos, ix - pos);\n"
                                                  "SET pos = ix + 3 ;\n"

                                                  "SET ix = LOCATE('&,&', attribute, pos); \n"
                                                  "	SET str2 = SUBSTRING(attribute, pos, ix - pos);\n"
                                                  " SET pos = ix + 3 ;\n"

                                                  " SET ix = LOCATE('&,&', attribute, pos);\n"
                                                  "IF ix > 0 THEN\n"
                                                  "	SET str3 = SUBSTRING(attribute, pos, ix - pos);\n"
                                                  "	ELSE\n"
                                                  "	SET str3 = substring(attribute, pos, attributeLenght);\n"
                                                  "END IF;\n"
                                                  "SET pos = ix + 3;\n"

                                                  "IF (Select count(0) from cey_sys_parameter where vch_parameter =str1)>0 THEN \n"
                                                  "	update cey_sys_parameter set vch_value =str2 where vch_parameter =str1;\n"
                                                  "	ELSE \n"
                                                  "Insert Into cey_sys_parameter values ( str1,  str2, str3);\n"
                                                  "END IF;\n"

                                                  "SET ix = LOCATE('&,&', attribute, pos);\n"

                                                  "END WHILE;\n"

                                                  "END\n"

                                                  << "drop trigger tr_android_cey_u_table_update;"
                                                  << "drop trigger tr_android_cey_u_table_insert;"
                                                  )
                                              );
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  ///脏数据
                                                  << "delete from cey_bt_kitchen_print where vch_printip = '123';"
                                                  ///小票默认厨打格式
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_checkBox_ch_billno','1') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_checkBox_ch_checkprintstate','1') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_checkBox_ch_rowno','1') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_checkBox_ch_serialno','1') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_checkBox_ch_tableno','1') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_checkBox_ch_tailtotal','0') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_checkBox_dt_cur','1') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_checkBox_dt_operdate','1') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_checkBox_int_person','1') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_checkBox_num_num','1') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_checkBox_num_price','0') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_checkBox_vch_dishname','1') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_checkBox_vch_operid','1') "

                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_lineEdit_ch_billno_scale','00') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_lineEdit_ch_serialno_scale','11') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_lineEdit_ch_tableno_scale','11') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_lineEdit_content_scale','01') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_lineEdit_dt_cur_scale','00') "

                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_lineEdit_dt_operdate_scale','00') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_lineEdit_int_person_scale','00') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_lineEdit_vch_operid_scale','00') "

                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_spinBox_num_num_row','1') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_spinBox_num_num_width','15') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_spinBox_num_price_row','1') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_spinBox_num_price_width','15') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_spinBox_vch_dishname_row','1') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_spinBox_vch_dishname_width','15') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_spinBox_vtimeout','5') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_checkBox_title','0') "
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_checkBox_vch_plan_id','0') "

                                                  ///基础资料
                                                  <<"alter table cey_u_togo  add column vch_yun_sn  VARCHAR(15)"
                                                  <<"alter table cey_u_orderdish_log  add column ch_payno  VARCHAR(15)"

                                                  <<"update cey_bt_dish set ch_groupno = ch_dishno where ifnull(ch_groupno, '') = '' "
                                                  <<"update del_cey_bt_dish set ch_groupno = ch_dishno where ifnull(ch_groupno, '') = '' "

                                                  <<"alter table cey_bt_dish  add column num_item_warn decimal(12,2)  default 0"//打印机新烨打印机的蜂鸣
                                                  <<"alter table del_cey_bt_dish  add column num_item_warn decimal(12,2)  default 0"//打印机新烨打印机的蜂鸣

                                                  <<"alter table cey_bt_dish  add column ch_guqing_flag varchar(1) "//是否估清
                                                  <<"alter table del_cey_bt_dish  add column ch_guqing_flag varchar(1) "//是否估清

                                                  <<"alter table cey_bt_dish  add column num_guqing_num decimal(12,2)  default 0 "//估清数量
                                                  <<"alter table del_cey_bt_dish  add column num_guqing_num decimal(12,2)  default 0"//估清剩余

                                                  <<"alter table cey_bt_dish  add column ch_material_flag varchar(1) "//是否原料
                                                  <<"alter table del_cey_bt_dish  add column ch_material_flag varchar(1)"//是否原料

                                                  <<"alter table cey_bt_dish  add column ch_special_type varchar(1) default '0' "//策略1  ：0 1 2
                                                  <<"alter table del_cey_bt_dish  add column ch_special_type varchar(1)"//策略1 ：0 1 2

                                                  <<"alter table cey_bt_dish  add column ch_special_type2 varchar(1) default '0' "//策略2  ：0 1 2
                                                  <<"alter table del_cey_bt_dish  add column ch_special_type2 varchar(1)"//策略2  ：0 1 2

                                                  <<"alter table cey_bt_dish  add column ch_special_type3 varchar(1) default '0' "//策略3  ：0 1 2
                                                  <<"alter table del_cey_bt_dish  add column ch_special_type3 varchar(1)"//策略3  ：0 1 2

                                                  <<"alter table cey_bt_dish  add column ch_special_type4 varchar(1) default '0' "//策略4  ：0 1 2
                                                  <<"alter table del_cey_bt_dish  add column ch_special_type4 varchar(1)"//策略4  ：0 1 2

                                                  <<"alter table cey_bt_dish  add column num_special_price DECIMAL(12, 2)  default 0"//特价 1 2
                                                  <<"alter table del_cey_bt_dish  add column num_special_price DECIMAL(12, 2)  default 0"//特价 1 2


                                                  << "alter table cey_u_orderdish change column num_num num_num DECIMAL(12, 3)  default 0"
                                                  << "alter table cey_u_orderdish change column num_back num_back DECIMAL(12, 3)  default 0"
                                                  << "alter table cey_u_hungbill change column num_num num_num DECIMAL(12, 3)  default 0"
                                                  << "alter table cey_u_hungbill change column num_back num_back DECIMAL(12, 3)  default 0"
                                                  << "alter table cey_u_orderdish change column vch_plan_desc vch_plan_desc VARCHAR(1023)"

                                                  ///微信支付宝，刷卡支付，调整
                                                  <<"insert into cey_bt_paymode(ch_paymodeno, vch_paymodename, ch_IDflag, ch_faceflag, ch_changeflag, ch_incomeflag, ch_key, vch_arg4) values('AA','摇钱树支付','N', 'N','N', 'Y', '', 'S' );"
                                                  <<        "update cey_bt_paymode set vch_arg4 = 'N' where ch_paymodeno = '05';"
                                                  <<           "update cey_bt_paymode set vch_arg4 = 'N' where ch_paymodeno = 'AA'; "
                                                  <<          "update cey_bt_paymode set vch_arg4 = 'N' where ch_paymodeno = '11'; "
                                                  <<          "update cey_bt_paymode set vch_arg4 = 'N' where ch_paymodeno = 'YY'; "
                                                  <<           "update cey_bt_paymode set vch_arg4 = 'N' where ch_paymodeno = 'ZZ'; "

                                                  ///cey_u_orderdish，加税率
                                                  <<"alter table cey_u_orderdish  add column int_rate decimal(12,2)  default 0 "//流水太大时，升级会非常慢
                                                  <<"alter table cey_u_hungbill  add column int_rate decimal(12,2)  default 0 "//流水太大时，升级会非常慢
                                                  ///打印机加蜂鸣次数
                                                  <<"alter table cey_bt_kitchen_print  add column int_xinye_beep_count int default 4"//打印机新烨打印机的蜂鸣
                                                  ///原料配比表
                                                  <<
                                                  "CREATE TABLE IF NOT EXISTS material_bt_dish("
                                                  "`int_flowID` int(11) NOT NULL AUTO_INCREMENT COMMENT '流水号',"
                                                  "`ch_dishno` varchar(7) NOT NULL COMMENT '编号',"
                                                  "`ch_itemno` varchar(7) NOT NULL COMMENT '原料编号',"
                                                  "`num_num` decimal(12,2) DEFAULT '0.00' COMMENT '原料数量',"
                                                  " PRIMARY KEY (`int_flowID`)"
                                                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
                                                  <<SQL_ch_yun_already_keep("material_bt_dish")
                                                  <<QString("create table del_%1 like %1; ").arg("material_bt_dish")
                                                  ///原料配比流水表
                                                  <<
                                                  "CREATE TABLE IF NOT EXISTS material_u_orderdish("
                                                  "`int_flowID`	int(11)  NOT NULL AUTO_INCREMENT COMMENT '流水号',"
                                                  "`int_order_flowID`	int(11) NOT NULL COMMENT '主商品流水',"
                                                  "`ch_dishno`	varchar(7) NOT NULL COMMENT '原料',"
                                                  "`num_num`	decimal(12,3) default '0.00'  COMMENT '原料点单数量',"
                                                  "`num_back`	decimal(12,3) default '0.00'  COMMENT '原料退货数量',"
                                                  "`num_rate`	decimal(12,3) default '0.00'  COMMENT '原料主料比率2',"
                                                  " PRIMARY KEY (`int_flowID`)"
                                                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"

                                                  ///原料销售更新库存的触发器-update
                                                  <<SQL_ch_yun_already_keep("material_bt_dish")
                                                  ///采购申请表
                                                  <<
                                                  "CREATE TABLE `material_purchase_apply` ("
                                                  "`BILL_NO` varchar(64) NOT NULL COMMENT '单据编号',"
                                                  "`APPLY_PEOPLE` varchar(32) DEFAULT NULL COMMENT '申请人',"
                                                  "`APPLY_TIME` datetime COMMENT '申请时间',"
                                                  "`CH_STATE` varchar(1) DEFAULT NULL COMMENT '审核状态',"
                                                  "`REMARK` varchar(256) DEFAULT NULL COMMENT '备注',"
                                                  "`DATE` datetime COMMENT '表单时间',"
                                                  "`TITLE` varchar(255) DEFAULT NULL,"
                                                  "PRIMARY KEY (`BILL_NO`)"
                                                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='采购申请表';"


                                                  ///原料采购申请明细
                                                  <<
                                                  "CREATE TABLE `material_purchase_apply_detail` ("
                                                  "`int_flowID` int(11) NOT NULL AUTO_INCREMENT,"
                                                  "`BILL_NO` varchar(64) NOT NULL COMMENT '单据编号',"
                                                  "`MATERIAL_CODE` varchar(32) DEFAULT NULL COMMENT '原料编号',"
                                                  "`MATERIAL_NAME` varchar(32) DEFAULT NULL COMMENT '原料名称',"
                                                  "`FORMAT` varchar(64) DEFAULT NULL COMMENT '规格',"
                                                  "`UNIT_CODE` varchar(16) DEFAULT NULL COMMENT '入库单位',"
                                                  "`UNIT_NAME` varchar(32) DEFAULT NULL COMMENT '单位名称',"
                                                  "`APPLY_NUM` decimal(16,2) DEFAULT NULL COMMENT '申请数量',"
                                                  "`DATE` datetime,"
                                                  "PRIMARY KEY (`int_flowID`)"
                                                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='原料采购申请明细';"


                                                  ///原料采购入库单
                                                  <<
                                                  "CREATE TABLE `material_purchase_in_storage` ("
                                                  "`BILL_NO` varchar(64) NOT NULL COMMENT '编号',"
                                                  "`ORG_CODE` varchar(64) DEFAULT NULL COMMENT '入库总店',"
                                                  "`ORG_NAME` varchar(64) DEFAULT NULL COMMENT '入库总店',"
                                                  "`PEOPLE` varchar(32) DEFAULT NULL COMMENT '入库人',"
                                                  "`REMARK` varchar(128) DEFAULT NULL COMMENT '备注',"
                                                  "`CH_STATE` varchar(1) DEFAULT NULL COMMENT '入库状态',"
                                                  "`DATE` datetime,"
                                                  "`TITLE` varchar(255) DEFAULT NULL,"
                                                  "PRIMARY KEY (`BILL_NO`)"
                                                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='原料采购入库单';"


                                                  ///原料采购入库单明细
                                                  <<
                                                  "CREATE TABLE `material_purchase_in_storage_detail` ("
                                                  "`int_flowID` int(11) NOT NULL AUTO_INCREMENT,"
                                                  "`BILL_NO` varchar(64) NOT NULL COMMENT '单据编号',"

                                                  "`MATERIAL_CODE` varchar(32) DEFAULT NULL COMMENT '原料编号',"
                                                  "`MATERIAL_NAME` varchar(32) DEFAULT NULL COMMENT '原料名称',"
                                                  "`FORMAT` varchar(64) DEFAULT NULL COMMENT '规格',"
                                                  "`UNIT_CODE` varchar(16) DEFAULT NULL COMMENT '入库单位',"
                                                  "`UNIT_NAME` varchar(32) DEFAULT NULL COMMENT '单位名称',"

                                                  "`APPLY_NUM` decimal(16,2) DEFAULT NULL COMMENT '入库数量',"
                                                  "`APPLY_PRICE` decimal(16,2) DEFAULT NULL COMMENT '入库价格',"
                                                  "`DATE` datetime,"
                                                  "PRIMARY KEY (`int_flowID`)"
                                                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='原料采购入库单明细';"


                                                  ///报损申请表
                                                  <<
                                                  "CREATE TABLE `material_loss_apply` ("
                                                  "`BILL_NO` varchar(64) NOT NULL COMMENT '单据编号',"
                                                  "`ORG_CODE` varchar(64) DEFAULT NULL COMMENT '受理总店',"
                                                  "`ORG_NAME` varchar(64) DEFAULT NULL,"
                                                  "`PEOPLE` varchar(32) DEFAULT NULL COMMENT '审核人',"
                                                  "`REMARK` varchar(256) DEFAULT NULL COMMENT '备注',"
                                                  "`CH_STATE` varchar(1) DEFAULT NULL COMMENT '审核状态',"
                                                  "`DATE` datetime COMMENT '表单时间',"
                                                  "`TITLE` varchar(255) DEFAULT NULL,"
                                                  "PRIMARY KEY (`BILL_NO`)"
                                                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='报损申请表';"


                                                  ///报损申请明细
                                                  <<
                                                  "CREATE TABLE `material_loss_apply_detail` ("
                                                  "`int_flowID` int(11) NOT NULL AUTO_INCREMENT,"
                                                  "`BILL_NO` varchar(64) NOT NULL COMMENT '单据编号',"

                                                  "`MATERIAL_CODE` varchar(32) DEFAULT NULL COMMENT '原料编号',"
                                                  "`MATERIAL_NAME` varchar(32) DEFAULT NULL COMMENT '原料名称',"
                                                  "`FORMAT` varchar(64) DEFAULT NULL COMMENT '规格',"
                                                  "`UNIT_CODE` varchar(16) DEFAULT NULL COMMENT '入库单位',"
                                                  "`UNIT_NAME` varchar(32) DEFAULT NULL COMMENT '单位名称',"

                                                  "`APPLY_NUM` decimal(16,2) DEFAULT NULL COMMENT '出库',"
                                                  "`APPLY_PRICE` decimal(16,2) DEFAULT NULL COMMENT '入库价格',"
                                                  "`DATE` datetime,"
                                                  "PRIMARY KEY (`int_flowID`)"
                                                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='报损申请明细';"

                                                  ///更新云同步原料部分
                                                  <<"replace into cey_web_table_version values( 'material_bt_dish', 'int_flowID', 'dishItemList', 'intFlowID', 'dishItemVersion',ifnull((select t.int_version from cey_web_table_version t where t.vch_sqltablename = 'material_bt_dish'),0) )"
                                                  ///库存添加【是否上传字段】
                                                  << SQL_ch_yun_already_keep("cey_st_material_num")
                                                  )
                                              );

    //3.3
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  ///数据库版本号的第三列
                                                  << "alter table cey_sys_sqlversion add column sqlversion_sub  int(5) default 0 ;"
                                                  <<
                                                  ///厨打单独一张表
                                                  "CREATE TABLE `cey_u_orderdish_print_tr` ("
                                                  "`int_orderflow` int(11) NOT NULL, "
                                                  "`vch_plan_k3o3_id` varchar(12) , "
                                                  "`vch_plan_k3o3_state` varchar(6), "
                                                  "`dt_operdate` datetime, "
                                                  "`ch_billno`	varchar(15), "
                                                  "`ch_dishno`	varchar(7), "
                                                  "PRIMARY KEY (`int_orderflow`)"
                                                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT ;"
                                                  ");"
                                                  <<"ALTER TABLE `cey_u_orderdish_print_tr` ADD INDEX `idx_cey_u_orderdish_print_tr_2` (`ch_billno` ) ;"
                                                  <<"ALTER TABLE `cey_u_orderdish_print_tr` ADD INDEX `idx_cey_u_orderdish_print_tr_3` (`dt_operdate` ) ;"
                                                  <<"ALTER TABLE `cey_u_orderdish_print_tr` ADD INDEX `idx_cey_u_orderdish_print_tr_4` (`vch_plan_k3o3_state` ) ;"

                                                  ///cey_u_orderdish_log
                                                  <<"alter table cey_u_orderdish_log change column num_back_cur num_back_cur DECIMAL(12, 3)  default 0 ;"
                                                  ///厨打单位宽度
                                                  <<" insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_spinBox_unit_width','15') ;"
                                                  ///厨打单位行数
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_spinBox_unit_row','1') ;"
                                                  ///厨打单总计倍高倍宽
                                                  <<"insert into cey_sys_parameter(vch_parameter, vch_value) value('kt_checkBox_ch_tailtotal_scale','11') ;"
                                                  ///商品信息,第二名称
                                                  <<" alter table cey_bt_dish add column vch_dishname_2 VARCHAR(60); "
                                                  ///厨打添加打印商品第一\二名称
                                                  <<" alter table cey_bt_kitchen_plan add column vch_dishname_No  varchar(30);"
                                                  ///用于局域网下,多机 流水号递增//AUTO_INCREMENT辅助作用
                                                  "  CREATE TABLE IF NOT EXISTS `cey_u_master_help` ("
                                                  "  `int_flowID` int(11) NOT NULL AUTO_INCREMENT, "
                                                  "  `ch_billno` varchar(15) , "
                                                  "  `date` date DEFAULT NULL,"
                                                  "  PRIMARY KEY (`int_flowID`),"
                                                  "  UNIQUE KEY `Idx_cey_u_master_help` (`ch_billno`),"
                                                  "  KEY `Idx_cey_u_master_help_2` (`date`)"
                                                  "  ) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT ;"

                                                  ///新的分单
                                                  <<"alter table cey_bt_table add column ch_billno_1 varchar(15) DEFAULT NULL;"
                                                  <<"alter table cey_bt_table add column ch_billno_2 varchar(15) DEFAULT NULL;"
                                                  <<"alter table cey_bt_table add column ch_billno_3 varchar(15) DEFAULT NULL;"
                                                  <<"alter table cey_bt_table add column ch_billno_4 varchar(15) DEFAULT NULL;"
                                                  <<"alter table cey_bt_table add column ch_billno_5 varchar(15) DEFAULT NULL;"
                                                  <<"alter table cey_bt_table add column ch_billno_6 varchar(15) DEFAULT NULL;"

                                                  <<"alter table cey_bt_table add column ch_billno_7 varchar(15) DEFAULT NULL;"
                                                  <<"alter table cey_bt_table add column ch_billno_8 varchar(15) DEFAULT NULL;"


                                                  <<"alter table cey_bt_table add column ch_lockflag_1 varchar(1) DEFAULT NULL;"
                                                  <<"alter table cey_bt_table add column ch_lockflag_2 varchar(1) DEFAULT NULL;"
                                                  <<"alter table cey_bt_table add column ch_lockflag_3 varchar(1) DEFAULT NULL;"
                                                  <<"alter table cey_bt_table add column ch_lockflag_4 varchar(1) DEFAULT NULL;"
                                                  <<"alter table cey_bt_table add column ch_lockflag_5 varchar(1) DEFAULT NULL;"
                                                  <<"alter table cey_bt_table add column ch_lockflag_6 varchar(1) DEFAULT NULL;"

                                                  <<"alter table cey_bt_table add column ch_lockflag_7 varchar(1) DEFAULT NULL;"
                                                  <<"alter table cey_bt_table add column ch_lockflag_8 varchar(1) DEFAULT NULL;"

                                                  //remove tr_android_cey_u_table_update
                                                  <<"drop trigger tr_android_cey_u_table_update;"
                                                  <<"drop trigger tr_android_cey_u_table_insert;"

                                                  ///原料数量精确到3位,以及索引
                                                  <<" alter table material_bt_dish change column num_num num_num DECIMAL(12, 3)  default 0;"
                                                  <<"ALTER TABLE `material_bt_dish` ADD INDEX index_name (`ch_dishno` ) ;"
                                                  <<"ALTER TABLE `cey_bt_dish` ADD INDEX index_name (`ch_groupno` ) ;"

                                                  ///商品\大类\小类 添加颜色
                                                  <<"alter table `cey_bt_dish` add column vch_color varchar(10) DEFAULT NULL;"
                                                  <<"alter table `cey_bt_dish_type` add column vch_color varchar(10) DEFAULT NULL;"  //#665588
                                                  <<"alter table `cey_bt_dish_subtype` add column vch_color varchar(10) DEFAULT NULL;"   //#665588

                                                  ///添加外带标志
                                                  <<" alter table cey_u_orderdish add column ch_waidai_flag  varchar(1) default '' ;"
                                                  ///参数设置添加云端是否上传标志
                                                  << SQL_ch_yun_already_keep("cey_sys_parameter")

                                                  ///由于挂账取款时的bug,此处对挂账金额做平衡
                                                  <<"update t_m_curamount set num_hangamount = (select sum(num_remain) from t_m_hang_balance where t_m_hang_balance.vch_memberno = t_m_curamount.vch_memberno) ;"

                                                  ///酒吧----------未清台
                                                  <<"CREATE TABLE IF NOT EXISTS cey_bt_table_bar("
                                                  "`int_flowID` int(11) NOT NULL AUTO_INCREMENT, "
                                                  "`ch_tableno` varchar(4) NOT NULL,"
                                                  "`int_div_id` int(4) NOT NULL,"
                                                  "`ch_billno` varchar(15) NOT NULL,"
                                                  "PRIMARY KEY (`int_flowID`), "
                                                  "UNIQUE KEY `Idx_cey_u_orderdish` (`ch_tableno`, `int_div_id`, `ch_billno`)"
                                                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"

                                                  ///酒吧----------取酒
                                                  <<"CREATE TABLE IF NOT EXISTS cey_bt_table_bar_wine("
                                                  "`int_flowID` int(11) NOT NULL AUTO_INCREMENT, "
                                                  "`ch_dishno` varchar(7) NOT NULL,"
                                                  "`num_dish` decimal(12, 2) NOT NULL,"

                                                  "`vch_handle` varchar(20) NOT NULL,"
                                                  "`vch_guest` varchar(50) NOT NULL,"
                                                  "`vch_operID` varchar(6) NOT NULL,"
                                                  "`dt_operdate` DATETIME NOT NULL,"
                                                  "`dt_enddate` DATE NOT NULL,"
                                                  "`vch_memo` varchar(100), "

                                                  "PRIMARY KEY (`int_flowID`), "
                                                  "KEY `idx_cey_bt_table_bar_wine` (`vch_handle`),"
                                                  "KEY `idx_cey_bt_table_bar_wine_2` (`vch_guest`),"
                                                  "KEY `idx_cey_bt_table_bar_wine_3` (`vch_operID`),"
                                                  "KEY `idx_cey_bt_table_bar_wine_4` (`dt_operdate`),"
                                                  "KEY `idx_cey_bt_table_bar_wine_5` (`dt_enddate`)"
                                                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
                                                  )
                                              );

    //旧系统下，将不会在这边更新数据看
    //3.4
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()));
    //3.5
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()));

    //3.6
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()));
    //3.7
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()));
    //3.8
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()));
    //3.9
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()));
    //4.0
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()));
    //4.1
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  << "update cey_sys_sqlversion set sqlversion_sub = 19 where sqlversion = 4.1"
                                                  )
                                              );
    //4.2
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  << "alter table cey_u_checkout_master  add column `ch_member_belong_state` varchar(1) "
                                                  << "alter table cey_u_checkout_master  add column `ch_yun_payno` varchar(128)  "
                                                  << "alter table cey_bt_dish  add column `ch_count` varchar(1) default 'N'  "
                                                  << "alter table cey_bt_dish  add column `num_count_weight` decimal(12, 3) default 0  "
                                                  << "alter table cey_sys_operator  add column `num_shift_cash_register`  decimal(12, 2) default 0 "
                                                  << "alter table cey_sys_operator  add column `ch_shift_config_flag`  varchar(1) default 'N' "
                                                  <<
                                                  " CREATE TABLE `cey_sys_lan_terminal` ("
                                                  " `vch_terminal` VARCHAR (64) NOT NULL,"
                                                  " `int_id` int UNIQUE NOT NULL,"
                                                  "  PRIMARY KEY (`vch_terminal`)"
                                                  " )ENGINE = INNODB DEFAULT CHARSET = utf8 ROW_FORMAT = COMPACT;"
                                                  << "alter table cey_sys_lan_terminal  add column `ch_billno`  varchar(15) default '' "
                                                  << " alter table cey_u_handover_master  add column `vch_memo2`  varchar(1023) default ''  "
                                                  <<  " delete from cey_bt_dish_type_oper where ifnull(ch_state, '') <> 'Y'; "
                                                  << " alter table cey_sys_operator  add column `num_deduct_rate`  decimal(10,2) default 0;  "
                                                  << " alter table cey_u_checkout_master  drop column `ch_dish_typeno`"
                                                  << " alter table cey_bt_dish_type  drop column `num_deduct_rate`"
                                                  << " alter table cey_bt_dish_type_oper  drop column `ch_state` "
                                                  << " drop trigger tr_android_cey_u_table_update "
                                                  << " drop trigger tr_android_cey_u_table_insert;"
                                                  << " alter table cey_u_table  add column  `vch_qr_code_sn`  VARCHAR(32) default '' ;"
                                                  << " alter table cey_u_orderdish  add column  `vch_qr_code_sn`  VARCHAR(32) default '' ;"
                                                  << " alter table cey_bt_dish  add column  `ch_qr_code_online`  VARCHAR(1) default '' ;"
                                                  << " alter table cey_bt_dish  add column  `int_rate_2`  DECIMAL(12,2) default 0 ;"
                                                  << " alter table cey_bt_dish  add column  `int_rate_3`  DECIMAL(12,2) default 0 ;"
                                                  << " alter table cey_bt_kitchen_print  drop column  `vch_printport`   ;"
                                                  << " alter table cey_bt_kitchen_print  drop column  `vch_printother`   ;"
                                                  << " alter table cey_bt_kitchen_print  add column  `ch_kitchen_view`  varchar(1) default 'N' ;"
                                                  )
                                              );
    //4.3
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA());
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA());
    //4.5
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA());
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA());
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA());
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA());
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA());
    //5.0
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA());
    //之后的数据库升级部分，在这里填写


    ///   num_num - num_back > 0 出库，< 0入库
    ///material_u_orderdish_insert
    ///cey_u_orderdish_insert

    /// new.num_num - old.num_back > 0 入库， < 0 出库
    ///material_u_orderdish_update
    ///cey_u_orderdish_update
    MySQL_sqlversion_data_content_list.append(MySQL_SQLVERSION_DATA(
                                                  QStringList()
                                                  <<
                                                  ///tr_material_u_orderdish_insert
                                                  "       DROP TRIGGER IF EXISTS tr_material_u_orderdish_insert;\n"

                                                  "       CREATE TRIGGER tr_material_u_orderdish_insert AFTER INSERT ON material_u_orderdish FOR EACH ROW \n"

                                                  "       BEGIN\n"

                                                  "       DECLARE flowId integer;\n"
                                                  "       DECLARE dishno varchar(7);\n"
                                                  "       DECLARE numnum numeric(10, 3);\n"
                                                  "       DECLARE materialno varchar(7);\n"
                                                  "       DECLARE intflowId integer;\n"
                                                  "       DECLARE initqty numeric(16,4);\n"
                                                  "       DECLARE initprice numeric(16,4);\n"
                                                  "       DECLARE remainqty numeric(16,4);\n"
                                                  "       DECLARE remainprice numeric(16,4);\n"
                                                  "       DECLARE itemflag char(1);\n"

                                                  "       set flowId = NEW.int_flowId;\n"
                                                  "       set dishno = NEW.ch_dishno;\n"
                                                  "       set numnum = NEW.num_num - NEW.num_back;\n"

                                                  "       set itemflag = 'N';\n"
                                                  "       set itemflag = (select ifnull(item_flag,'N') from cey_bt_dish where ch_dishno = dishno);\n"

                                                  "       set materialno = dishno;\n"

                                                  "        IF (materialno<>'' and itemflag = 'Y') THEN\n"

                                                  "       set intflowId = (select ifnull(max(int_flowId),0) from cey_st_material_flow where ch_materialno = materialno);\n"

                                                  "       IF (intflowId = 0) THEN\n"
                                                  "          set initqty = 0;\n"
                                                  "       ELSE\n"
                                                  "          set initqty = (select remain_qty from cey_st_material_flow where int_flowId = intflowId);\n"
                                                  "       END IF;\n"

                                                  "      IF (intflowId = 0) THEN\n"
                                                  "          set initprice = 0;\n"
                                                  "      ELSE\n"
                                                  "          set initprice = (select remain_price from cey_st_material_flow where int_flowId = intflowId);\n"
                                                  "      END IF;\n"

                                                  "        set remainqty  = initqty - numnum ;\n"

                                                  "     IF (remainqty = 0) THEN\n"
                                                  "         set remainprice = 0;\n"
                                                  "     ELSE\n"
                                                  "          set remainprice = initprice;\n"
                                                  "       END IF;\n"

                                                  ////////////////////////////////////////////////////////////////////////////////////////原料
                                                  "           insert cey_st_material_flow (ch_materialno, dt_operdate, sheettype, int_flag,\n"
                                                  "              num_num, sheet_price, cost_price, remain_qty, remain_price, ch_sheetno )\n"
                                                  "        values (materialno,now(), 'YL', if(numnum < 0, -1, 1), abs(numnum), 0, remainprice, remainqty, remainprice, LEFT(flowId,20));\n"


                                                  "      IF exists (select 1 from cey_st_material_num where ch_materialno = materialno) THEN\n"
                                                  "            update cey_st_material_num set num_num = remainqty, cost_price = remainprice\n"
                                                  "          where ch_materialno = materialno;\n"
                                                  "       ELSE\n"
                                                  "       insert into cey_st_material_num (ch_materialno, num_num, cost_price)\n"
                                                  "      values (materialno, remainqty, remainprice);\n"
                                                  "      END IF;\n"

                                                  "       END IF;\n"

                                                  "       END\n"

                                                  <<
                                                  ///

                                                  "    DROP TRIGGER IF EXISTS tr_material_u_orderdish_update;\n"
                                                  "    CREATE TRIGGER tr_material_u_orderdish_update AFTER UPDATE ON material_u_orderdish FOR EACH ROW \n"
                                                  "      BEGIN\n"
                                                  "       DECLARE m_done int default 0;\n"
                                                  "     DECLARE flowId integer;\n"
                                                  "     DECLARE dishno varchar(7);\n"
                                                  "     DECLARE numnum numeric(10, 3);\n"
                                                  "  DECLARE materialno varchar(7);\n"
                                                  "    DECLARE intflowId integer;\n"
                                                  "       DECLARE initqty numeric(16,4);\n"
                                                  "      DECLARE initprice numeric(16,4);\n"
                                                  "     DECLARE remainqty numeric(16,4);\n"
                                                  "     DECLARE remainprice numeric(16,4);\n"
                                                  "       DECLARE itemflag char(1);\n"


                                                  "    IF(NEW.int_flowId = OLD.int_flowId and (NEW.num_back <> OLD.num_back or NEW.num_num <> OLD.num_num ) ) THEN\n"

                                                  "        set flowId = NEW.int_flowId;\n"
                                                  "      set dishno = NEW.ch_dishno;\n"
                                                  "      set numnum = OLD.num_num - OLD.num_back - (NEW.num_num - NEW.num_back);\n"

                                                  "     set itemflag = 'N';\n"
                                                  "        set itemflag = (select ifnull(item_flag,'N') from cey_bt_dish where ch_dishno = dishno);\n"
                                                  "         set materialno = dishno;\n"

                                                  "        IF (materialno<>'' and itemflag = 'Y')  THEN\n"

                                                  "    set intflowId = (select ifnull(max(int_flowId),0) from cey_st_material_flow where ch_materialno = materialno);\n"

                                                  "       IF (intflowId = 0) THEN\n"
                                                  "             set initqty = 0;\n"
                                                  "    ELSE\n"
                                                  "         set initqty = (select remain_qty from cey_st_material_flow where int_flowId = intflowId);\n"
                                                  "      END IF;\n"

                                                  "        IF (intflowId = 0) THEN\n"
                                                  "          set initprice = 0;\n"
                                                  "    ELSE\n"
                                                  "      set initprice = (select remain_price from cey_st_material_flow where int_flowId = intflowId);\n"
                                                  "       END IF;\n"

                                                  "       set remainqty = initqty + numnum ;\n"

                                                  "        IF (remainqty = 0)  THEN\n"
                                                  "       set remainprice = 0;\n"
                                                  "      ELSE\n"
                                                  "       set remainprice = initprice;\n"
                                                  "        END IF;\n"
                                                  ////////////////////////////////////////////////////////////////////////////////////////原料

                                                  "           insert cey_st_material_flow (ch_materialno, dt_operdate, sheettype, int_flag,\n"
                                                  "                     num_num, sheet_price, cost_price, remain_qty, remain_price, ch_sheetno )\n"
                                                  "          values (materialno,now(), 'YL', if(numnum < 0, -1, 1), abs(numnum), 0, remainprice, remainqty, remainprice, left(flowId,20));\n"

                                                  "     IF exists (select 1 from cey_st_material_num where ch_materialno = materialno) THEN\n"
                                                  "       update cey_st_material_num set num_num = remainqty, cost_price = remainprice\n"
                                                  "      where ch_materialno = materialno;\n"
                                                  "          ELSE\n"
                                                  "           insert into cey_st_material_num (ch_materialno, num_num, cost_price)\n"
                                                  "              values (materialno, remainqty, remainprice);\n"
                                                  "      END IF;\n"

                                                  "     END IF;\n"

                                                  "          END IF;\n"

                                                  "   END\n"
                                                  <<
                                                  ///cey_u_taxsale_master
                                                  "drop table cey_u_taxsale_master"
                                                  <<
                                                  "CREATE TABLE IF NOT EXISTS cey_u_taxsale_master("
                                                  "`ch_billno` varchar(15) NOT NULL,"
                                                  "`ch_payno` varchar(15) DEFAULT NULL,"
                                                  "`ch_tax_mode` varchar(1) DEFAULT NULL ,"
                                                  "`ch_tax_type` varchar(1) DEFAULT NULL,"

                                                  "`num_before` DECIMAL(10,2) DEFAULT 0  comment '税前金额' ,"
                                                  "`num_cur` DECIMAL(10,2) DEFAULT 0  comment '税1',"
                                                  "`num_cur_2` DECIMAL(10,2) DEFAULT 0,"
                                                  "`num_cur_3` DECIMAL(10,2) DEFAULT 0,"

                                                  "`ch_fix_flag` varchar(1) DEFAULT NULL  comment '是否固定税率',"
                                                  "`int_fix_tax` DECIMAL(10,2) DEFAULT 0  comment '固定税率1',"
                                                  "`int_fix_tax_2` DECIMAL(10,2) DEFAULT 0,"
                                                  "`int_fix_tax_3` DECIMAL(10,2) DEFAULT 0,"

                                                  "`ch_fix_service` varchar(1) DEFAULT NULL comment '是否固定服务费' ,"
                                                  "`int_fix_service` DECIMAL(10,2) DEFAULT 0 comment '固定服务费',"

                                                  "`dt_operdate` datetime DEFAULT NULL,"
                                                  "PRIMARY KEY (`ch_billno`),"
                                                  "KEY `idx_cey_u_taxsale_master_1` (`dt_operdate`), "
                                                  "KEY `idx_cey_u_taxsale_master_2` (`ch_payno`)"
                                                  ") ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT ;"
                                                  )
                                              );
    return;
}

void public_sql::PUBLIC_SQL_INIT_DEL()
{
}

bool public_sql::fastfd_init_UNZIP()
{
    //解压对应的商品包
#ifdef Q_OS_UNIX
    if(false == QFile("fastfd_localdata.zip").exists()) {
        public_sql::MySQL_info=public_sql::err_path_arg.arg(QObject::tr("失败")).arg("not find  fastfd_localdata.zip");
        return false;
    }
    QProcess myProcess;
    myProcess.start(QString("rm -rf %1").arg(lds::localdataPath));
    if(myProcess.waitForFinished()) {
        myProcess.start("unzip fastfd_localdata.zip -d ./userdata");
        if(myProcess.waitForFinished()) {
            qApp->setStyleSheet(public_sql::stylesheetData(lds::sysconf->value("system_setting/skin_name", "obsidian").toString()));
            public_sql::MySQL_info=public_sql::err_path_arg.arg(QObject::tr("操作成功")).arg("unzip  fastfd_localdata");
            return true;
        }
    }
    public_sql::MySQL_info=public_sql::err_path_arg.arg(QObject::tr("失败")).arg(myProcess.errorString());
    return false;
    //! 解压对应的商品包
#endif
    return true;
}

void public_sql::MySQL_PARAM_INIT()
{
    islocal=lds::sysconf->value("public_sql/islocal", true).toBool();
    localdbHostName=lds::sysconf->value("public_sql/localdbHostName", "127.0.0.1").toString();
    localdbDatabaseName=lds::sysconf->value("public_sql/localdbDatabaseName","hddpos").toString();
    localdbUserName=lds::sysconf->value("public_sql/localdbUserName","root").toString();

#ifdef LINPOS_DEBUG
    localdbPassword=lds::sysconf->value("public_sql/localdbPassword","111111").toString();
#endif
#ifdef LINPOS_OS6
    localdbPassword=lds::sysconf->value("public_sql/localdbPassword","888888").toString();
#endif
#ifdef LINPOS_WIN
    localdbPassword=lds::sysconf->value("public_sql/localdbPassword","123456").toString();
#endif

    dbHostName=lds::sysconf->value("public_sql/dbHostName").toString();
    dbDatabaseName=lds::sysconf->value("public_sql/dbDatabaseName", "hddpos").toString();
    dbUserName=lds::sysconf->value("public_sql/dbUserName", "root").toString();
    dbPassword=lds::sysconf->value("public_sql/dbPassword").toString();
}

void public_sql::MySQL_PARAM_SAVE()
{
    lds::sysconf->setValue("public_sql/dbHostName", dbHostName);
    lds::sysconf->setValue("public_sql/dbDatabaseName", dbDatabaseName);
    lds::sysconf->setValue("public_sql/dbUserName", dbUserName);
    lds::sysconf->setValue("public_sql/dbPassword", dbPassword);

    lds::sysconf->setValue("public_sql/islocal", islocal);
    lds::sysconf->setValue("public_sql/localdbHostName", localdbHostName);
    lds::sysconf->setValue("public_sql/localdbDatabaseName", localdbDatabaseName);
    lds::sysconf->setValue("public_sql/localdbUserName", localdbUserName);
    lds::sysconf->setValue("public_sql/localdbPassword", localdbPassword);
}

//-1 连接错误       0数据库不存在       1成功
bool public_sql::CONNECT_MySQL(lds_messagebox_loading_show *loading)
{
    //1
    QSqlDatabase db = lds_query_thread::getThreadDbAndCreat();
    QString hddpos = public_sql::getdbDatabaseName();
    QString use_hddpos = "use " +hddpos +";";
    QString create_database_hddpos = "create database  " +hddpos +";";
    QString drop_database_hddpos = "drop database  " +hddpos +";";
    QString mysqlinstallpath;
    QString msg;
    db.setHostName(public_sql::getdbHostName());
    db.setUserName(public_sql::getdbUserName());
    db.setPassword(public_sql::getdbPassword());
    // MySQL connection
    if(db.hostName().isEmpty()
            //            || db.databaseName().isEmpty()
            || db.userName().isEmpty()
            ) {
        public_sql::MySQL_info=public_sql::err_path_arg.arg(QObject::tr("数据库连接失败")).arg(QObject::tr("有参数为空"));
        return false;
    }

    //open
#ifdef Q_OS_WIN
    if(false == db_open(db)) {
        if(public_sql::islocal) {
            if(!PUBLIC_MYSQL_INSTALL(loading, msg)) {
                public_sql::MySQL_info = public_sql::err_path_arg.arg(QObject::tr("操作失败")).arg(msg);
                return false;
            }
            if(false == db_open(db)) {
                public_sql::MySQL_info = public_sql::err_path_arg.arg(QObject::tr("数据库连接失败")).arg("ERROR "+ QString::number(db.lastError().number()) + ":" + db.lastError().text());
                return false;
            }
        }
    }
#else
    if(false == db_open(db)) {
        public_sql::MySQL_info = public_sql::err_path_arg.arg(QObject::tr("数据库连接失败")).arg("ERROR "+ QString::number(db.lastError().number()) + ":" + db.lastError().text());
        return false;
    }
#endif

    //win_mysql_local_install_path
    lds_query_db query(db);
    msg = "";
#ifdef Q_OS_WIN
    query.execSelect("select @@basedir as basePath from dual");
    if(false == query.next()) {
        public_sql::MySQL_info=public_sql::err_path_arg.arg(QObject::tr("数据库连接失败")).arg("ERROR "+ QString::number(db.lastError().number()) + ":" + db.lastError().text());
        return false;
    }
    mysqlinstallpath = QDir::fromNativeSeparators(query.recordValue(0).toString());
    if(QDir(mysqlinstallpath).exists()) {
        lds::sysconf->setValue("public_sql/win_mysql_local_install_path", mysqlinstallpath);
    }
#endif

    //hddpos 不存在
    if(!query.execSelect(use_hddpos)) {
        fastfd_init_UNZIP();
        goto DB_INIT;
    }
    //基础表 不存在
    if(!query.execSelect(" desc cey_bt_dish ")) {
        msg = "," + QObject::tr("不重置本地文件");
        goto DB_INIT;
    }
    goto DB_OPEN;
DB_INIT:
    if(false == islocal) {
        public_sql::MySQL_info = QObject::tr("非本地数据库, 无法初始化");
        return false;
    }
    query.execCreate(drop_database_hddpos);
    query.execCreate(create_database_hddpos);
    query.execSelect(use_hddpos);
    public_sqlSet::privileges(&query);

    if(saas_pay_detail::isSaaSAA()) {//普通版
#ifdef Q_OS_WIN
        lds::sysconf->setValue("system_setting/bofanglujing", qApp->applicationDirPath()+"/"+"fastfd_localdata/image_dish_dir_2");
#endif
        loading->setText(QObject::tr("首次使用，正在还原数据库") + msg);
#ifdef QT_SCPOS_SUPPORT
        if(false == public_sql::MySQL_restore(public_sql::show_chinese_sql_file_dir + "/" +  public_sql::show_chinese_sql_file)) {
#else
        if(false == public_sql::MySQL_restore(public_sql::show_english_sql_file_dir + "/" +  public_sql::show_english_sql_file)) {
#endif
            public_sql::MySQL_info = QObject::tr("操作失败");
            return false;
        }
    } else {//专业版
        loading->setText(QObject::tr("首次使用，正在还原数据库") + msg);
        if(false == public_sql::MySQL_restore(public_sql::show_empty_sql_file_dir + "/" +  public_sql::show_empty_sql_file)) {
            public_sql::MySQL_info = QObject::tr("操作失败");
            return false;
        }
    }

DB_OPEN:

    //  setDatabaseName
    //  This must be done before the connection is opened
    db.setDatabaseName(hddpos);
    db_close(db);
    if(false == db_open(db)) {
        public_sql::MySQL_info=public_sql::err_path_arg.arg(QObject::tr("数据库连接失败")).arg(QObject::tr("打开失败") + db.lastError().text());
        return false;
    }
    //
    if(false == query.execSelect("select now();")) {
        public_sql::MySQL_info=public_sql::err_path_arg.arg(QObject::tr("数据库连接失败")).arg(QObject::tr("打开失败") + QString::number(__LINE__));
        return false;
    }

    public_sql::MySQL_info=public_sql::err_path_arg.arg(QObject::tr("数据库连接成功") + ":").arg(public_sql::islocal?QObject::tr("本地") : QObject::tr("服务器"));

    return true;
}

int public_sql::MySQL_PARAME_SET_EXEC(QWidget *parent, const QString &title)
{
    lds_dialog_input inputdialog(parent);
    inputdialog.ui->label->setText(QObject::tr("密钥") + "(***7):");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText("7777");
#endif
    if(title.isEmpty()) {
        inputdialog.setWindowTitle(QObject::tr("请输入密钥"));
    } else {
        inputdialog.setWindowTitle(title);
    }
    if(QDialog::Accepted==transparentCenterDialog(&inputdialog).exec()) {
        if(inputdialog.ui->lineEdit->text() != "7777") {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("密码错误"));
            return QDialog::Rejected;
        }
        public_sqlSet dialog(parent);
        dialog.setWindowTitle(QObject::tr("数据库设置"));
        return transparentCenterDialog(&dialog).exec();
    }
    return QDialog::Rejected;
}

QString public_sql::getdbHostName()
{
    return islocal?localdbHostName:dbHostName;
}

QString public_sql::getdbDatabaseName()
{
#ifdef QT_DEBUG
    return "hddpos";
#endif
    return islocal?localdbDatabaseName:dbDatabaseName;
}

QString public_sql::getdbUserName()
{
    return islocal?localdbUserName:dbUserName;
}

QString public_sql::getdbPassword()
{
    return islocal?localdbPassword:dbPassword;
}

void public_sql::setdbHostName(const QString &db_host_name)
{
    if(islocal) {
        localdbHostName = db_host_name;
    } else {
        dbHostName = db_host_name;
    }
}

void public_sql::setdbDatabaseName(const QString &db_database_name)
{
    if(islocal) {
        localdbDatabaseName = db_database_name;
    } else {
        dbDatabaseName = db_database_name;
    }
}

void public_sql::setdbUserName(const QString &db_usr_name)
{
    if(islocal) {
        localdbUserName = db_usr_name;
    } else {
        dbUserName = db_usr_name;
    }
}

void public_sql::setdbPassword(const QString &db_pwd)
{
    if(islocal) {
        localdbPassword = db_pwd;
    } else {
        dbPassword = db_pwd;
    }
}

QStringList public_sql::MySQL_hddpos_tablelist_order()
{
    QStringList rets;
    rets << "t_m_member";
    rets << "t_m_curamount";

    rets << "cey_bt_dish";
    rets << "cey_bt_dish_subtype";
    rets << "cey_bt_dish_type";
    rets << "cey_bt_unit";

    lds_query_hddpos query;
    query.execSelect("  SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA='hddpos' ; ");
    while(query.next()) {
        if(rets.contains(query.recordValue(0).toString())) {

        } else {
            rets << query.recordValue(0).toString();
        }
    }
    return rets;
}

//!压缩备份 mysqldump -h主机名  -P端口 -u用户名 -p密码 (–database) 数据库名 > 文件名.sql
bool public_sql::MySQL_backup(const QString &fileName, const QStringList &tablelist)
{
#ifdef Q_OS_WIN
    QString win_mysql_local_install_path = lds::sysconf->value("public_sql/win_mysql_local_install_path").toString();
    if(win_mysql_local_install_path.count() == 0) {
        MySQL_info= "win_mysql_local_install_path is null";
        return false;
    }
#endif
    //2
    MySQL_info.clear();
    //3
    //获取备份 指令
    QString ip = getdbHostName();
    QString username =getdbUserName();
    QString password = getdbPassword();
    QString databaseName = getdbDatabaseName();

    QString program;
#ifdef Q_OS_UNIX
    //!非压缩备份
    program = "mysqldump";
#else
    program = QString("\"%1\"")
            .arg(win_mysql_local_install_path+"/bin/mysqldump");
#endif

    if(ip != "127.0.0.1") {
        program += " -h" + ip;
    }
    program += " -u"+username;
    if(!password.isEmpty()) {
        program += " -p" + password;
    }
    program += " " + databaseName + " " + tablelist.join(" ");

    //开始备份，提交备份信息
    QString errstring;
    QEventLoop eloop;
    QFuture<void> future = QtConcurrent::run(PUBLIC_EXCUTE_PROCESS,
                                             &eloop,
                                             program,
                                             fileName,
                                             QString(),
                                             &errstring
                                             );
    eloop.exec();
    //4
    if(errstring.isEmpty()) {
        MySQL_info=err_path_arg.arg(QObject::tr("备份成功")).arg(fileName);
        return true;
    }

    MySQL_info=err_path_arg.arg(QObject::tr("备份失败")).arg(fileName);
    return false;
}

//!压缩还原 gunzip < backupfile.sql.gz | mysql -uusername -ppassword databasename
bool public_sql::MySQL_restore(const QString &fileName, bool rebuidDB)
{
#ifdef Q_OS_WIN
    QString win_mysql_local_install_path = lds::sysconf->value("public_sql/win_mysql_local_install_path").toString();
    if(win_mysql_local_install_path.count() == 0) {
        MySQL_info= "win_mysql_local_install_path is null";
        return false;
    }
#endif
    if(QFile::exists(fileName)) {

    } else {//
        MySQL_info=err_path_arg.arg(QObject::tr("文件不存在")).arg(fileName);
        return false;
    }//
    QString path = fileName;
    QString ip = getdbHostName();
    QString username =getdbUserName();
    QString password = getdbPassword();
    QString databaseName = getdbDatabaseName();
    QString program;
    lds_query_hddpos  query;

    ///rebuild database
    if(rebuidDB) {
        query.execAlter("drop database " + databaseName);
        query.execAlter("create database " + databaseName);
    }
    ///~rebuild database

    ///QSqlDatabase::close
    foreach(const QString &dbname, QSqlDatabase::connectionNames()) {
        QSqlDatabase::database(dbname).close();
    }
    MySQL_info.clear();
    ///~QSqlDatabase::close

    //3
#ifdef Q_OS_UNIX
    //!非压缩还原
    program = "mysql ";
#else
    program = QString("\"%1\" ")
            .arg(win_mysql_local_install_path+"/bin/mysql");
#endif
    if(ip != "127.0.0.1") {
        program += " -h" + ip;
    }
    program += " -u"+username;
    if(!password.isEmpty()) {
        program += " -p" + password;
    }
    program += " " + databaseName;

    //开始还原，提交备份信息
    QString errstring;
    QEventLoop eloop;
    QFuture<void> future = QtConcurrent::run(PUBLIC_EXCUTE_PROCESS,
                                             &eloop,
                                             program,
                                             QString(),
                                             path,
                                             &errstring
                                             );
    eloop.exec();
    //4
    if(errstring.isEmpty()) {
        MySQL_info=err_path_arg.arg(QObject::tr("还原成功")).arg(fileName);
        return true;
    }
    MySQL_info=err_path_arg.arg(QObject::tr("还原失败")).arg(fileName);
    return false;
}

QString public_sql::makedir(const QString &dir)
{
    if(QDir().exists(dir))return "dir is existed";

    if(QDir().mkdir(dir)) return "dir create success";

    return "dir create failure";
}

QString public_sql::copyfile(const QString &filefrom, const QString &fileto)
{
    QString errstring;
    if(lds::resCopyFile(filefrom, fileto, errstring)) {
        return "success";
    }
    return errstring;
}

QString public_sql::remove_copyfile(const QString &filefrom, const QString &fileto)
{

    if(QFile(fileto).exists()) {
        removefile(fileto);
    }
    if(QFile::copy(filefrom,  fileto)) {
        QFile::setPermissions(fileto, QFile::Permission(0x777));
        return "file copy success";
    }
    return "file create failure";
}

bool public_sql::removefile(const QString &filename)
{
    return QFile(filename).remove();
}

QObject *public_sql::save_login_object()
{
    return save_login;
}

QString public_sql::get_logo_path()
{
    return lds::sysconf->value("public_sql/login_logo", ":/image/widgets_pic/logo1.png").toString();
}

QString public_sql::get_print_line()
{
    return "_gs_print_line_";
}

void public_sql::systranslate()
{
    //翻译
    public_Dialog::retranslateSystem(lds::get_soft_curent_language_first());
}

void public_sql::syswindowicon()
{
#ifdef Q_OS_UNIX
    qApp->setWindowIcon(QIcon(":/image/logo/fastfd.png"));
#else
    qApp->setWindowIcon(QIcon(":/image/logo/fastfd.ico"));
#endif
}

void public_sql::locale_print_file_init()
{
}

QByteArray public_sql::stylesheetData(const QString &key)
{
    QFile file(QString("userdata/settings/skin/fastfd_%1.qss").arg(key));
    if(file.open(QFile::ReadOnly)) {
        qDebug() << file.errorString();
        return file.readAll();
    }
    return QByteArray();
}

bool public_sql::syslimitcheck(QWidget *parent)
{
    forever{
        QDate dt_limit,dt_cur;
        w_sys_manage_sqlcopy_soft_limit2::limit_date(dt_limit, dt_cur);

#ifdef Q_OS_WIN
        public_encryption_dog dog;
        QString dog_id;
        public_encryption_dog::ERROR_CODE err = dog.conductlock(dog_id);
        if(err == public_encryption_dog::ERROR_CODE_NOERR) {
            dt_limit = qMax(dt_cur.addDays(30), dt_limit);
            public_sql::fastfd_limit_conf->setValue("machine/"+lds::terminalCode, dt_limit);
            return true;
        }
#endif

        int limit_day = dt_cur.daysTo(dt_limit);
        QString info;
        if(limit_day >30)
            return true;//!启动，过期时间超过10

        if(limit_day <= 30 && limit_day > 0) {
            info = QObject::tr("软件试用期还剩%1天，请输入验证密钥或插入加密狗").arg(limit_day);
            switch(lds_messagebox::question(parent, MESSAGE_TITLE_VOID, info, QObject::tr("输入验证密钥"), QObject::tr("插入加密狗"), QObject::tr("继续"))) {
            case 0://输入验证密钥
            {
                w_sys_manage_sqlcopy_soft_limit2 dialog(parent);
                dialog.setWindowTitle(info);
                transparentCenterDialog(&dialog).exec();
            }
                break;
            case 1://插入加密狗
                break;
            case 2://继续
                return true;
            }
        }
        if(limit_day  <= 0) {
            info = QObject::tr("软件超过试用期，请输入验证密钥或插入加密狗");
            switch(lds_messagebox::question(parent, MESSAGE_TITLE_VOID, info, QObject::tr("输入验证密钥"), QObject::tr("插入加密狗"), QObject::tr("取消"))) {
            case 0://输入验证密钥
            {
                w_sys_manage_sqlcopy_soft_limit2 dialog(parent);
                dialog.setWindowTitle(info);
                transparentCenterDialog(&dialog).exec();
            }
            case 1://插入加密狗
                break;
            case 2://取消
                return false;
            }
        }
    }
    return true;
    //#ifdef Q_OS_WIN
    //    return true; //window下不做判断，只判断是否插入加密狗
    //    //加密狗检测
    //    //    {
    //    //        //#ifdef QT_DEBUG
    //    //        //        settings.setValue("window_soft_limit/canuse", true);
    //    //        //#endif
    //    //        while(1) {
    //    //            //若是没有激活的话
    //    //            int remainday;
    //    //            if((remainday=(settings.value("window_soft_limit/date").toDate().daysTo(sqldt.date())))>30) {
    //    //                lds_messagebox::information(parent, MESSAGE_TITLE_VOID,
    //    //                                            QObject::tr("软件使用已经超过30天，请插入加密狗以便继续使用"));
    //    //                return false;
    //    //            } else {
    //    //                if(remainday < 0) {
    //    //                    lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,
    //    //                                            QObject::tr("最后一次激活时间大于机器当前时间"));
    //    //                    return false;
    //    //                }
    //    //                //软件在使用期内
    //    //                if(0!=lds_messagebox::question(parent, MESSAGE_TITLE_VOID,
    //    //                                               QObject::tr("剩余")+QString::number(30-remainday)+QObject::tr("天"))) {
    //    //                    break;
    //    //                }
    //    //            }
    //    //            break;
    //    //        }
    //    //    }
    //#else
    //    if(false == w_sys_manage_sqlcopy_soft_limit::static_soft_limit_check(parent)) {
    //        return false;
    //    }
    //#endif
}

bool public_sql::sysstart(public_loginDialog *login, lds_messagebox_loading_show *loading)
{
    //版本号
    login->setversion(fastfd_update::gs_soft_version);
    //登录界面
    login->init();
    if(false == login->connectdb_init_check(loading)) {
        return false;
    }
    //全局指针保存
    lds::setwflagFrameless(login);
    login->resize(lds::MAIN_WINDOW_SIZE);
    login->move(0, 0);
    login->setWindowTitle(QObject::tr("用户登录"));
#if (defined QT_DEBUG) || (defined Q_OS_WIN)
#else
    login->move(0,0);
#endif

    return true;
}

void public_sql::syssetcode()
{
    //设置编号
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//中文可用//包含头文件
    QTextCodec::setCodecForLocale(codec);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#else
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#endif
}

void public_sql::arg_forbid_debug(int argc, char *argv[])
{
    lds::arg_forbid_debug(argc, argv);
}

///
/// \brief public_sql::cmd_ps_pid
/// \param pids return value
/// \param psFilter 用于检索时的filter           pos/fastf
/// \param exactFilter 用于精确匹配的filter  pos/fastfdd
///
void public_sql::cmd_ps_pid(QStringList &pids, const QString &psFilter, const QString &exactFilter)
{
    lds::cmd_ps_pid(pids, psFilter, exactFilter);
}

QString public_sql::getTablenoByChbillno(const QString &ch_billno)
{
    lds_query_hddpos  query;
    query.execSelect(QString("select ch_tableno from cey_u_table where ch_billno = '%1' ")
                     .arg(ch_billno));
    query.next();
    return query.recordValue("ch_tableno").toString();
}

float public_sql::getTotalBy_ch_billno(const QString &ch_billno)
{
    //赠送、taocai
    lds_query_hddpos  query;
    query.execSelect(QString("select sum( if(ch_presentflag = 'Y',  0, (num_num-num_back) * num_price *0.01*int_discount)+ num_price_add ) from cey_u_orderdish where ch_billno = '%1' ")
                     .arg(ch_billno));
    query.next();
    return query.recordValue(0).toFloat();
}

QString public_sql::getTableName(const QString &ch_tableno)
{
    lds_query_hddpos  query;
    query.execSelect(QString(" select vch_tablename from cey_bt_table where ch_tableno = '%1' ")
                     .arg(ch_tableno));
    query.next();
    return query.recordValue("vch_tablename").toString();
}

void public_sql::action_sql_debug()
{
    lds_query_hddpos  query;
    query.execSelect("select * FROM cey_web_table_version;");
    while(query.next()) {
        action_sql_debug_piece(query.recordValue("vch_sqltablename").toString());
    }
    action_sql_debug_piece("cey_u_checkout_detail");
    action_sql_debug_piece("cey_u_checkout_master");
    action_sql_debug_piece("cey_u_orderdish");
    action_sql_debug_piece("cey_u_orderdish_log");
    action_sql_debug_piece("cey_u_service_low");

    action_sql_debug_piece("t_m_deposit");
    action_sql_debug_piece("t_m_point");
    action_sql_debug_piece("t_m_pay");
    action_sql_debug_piece("cey_u_togo");
    action_sql_debug_piece("cey_u_table");
}

void public_sql::action_sql_debug_piece(const QString &tablename)
{
    QString p0, p1;
    QString update_p0;
    lds_query_hddpos  query0;
    query0.execSelect(QString("show columns from %1").arg(tablename));
    while(query0.next()) {
        if(tablename == "cey_bt_dish"
                && (query0.recordValue(0).toString() == "vch_kitchen_out_id1"
                    || query0.recordValue(0).toString() == "vch_kitchen_out_id2"
                    || query0.recordValue(0).toString() == "vch_kitchen_out_id3"
                    || query0.recordValue(0).toString() == "vch_kitchen_print_id1"
                    || query0.recordValue(0).toString() == "vch_kitchen_print_id2"
                    || query0.recordValue(0).toString() == "vch_kitchen_print_id3"
                    || query0.recordValue(0).toString() == "ch_kitchen_out_flag"
                    || query0.recordValue(0).toString() == "ch_kitchen_print_flag")
                ) continue;
        if(query0.recordValue(0).toString() == "ch_yun_already_keep") continue;
        p0 += query0.recordValue(0).toString() +",";
        p1 += ":"+query0.recordValue(0).toString() +",";

        update_p0 += query0.recordValue(0).toString() + "="+":"+query0.recordValue(0).toString()+",";
    }
    p0.chop(1);
    p1.chop(1);
    update_p0.chop(1);

    //    qDebug() << ("insert-"+tablename) << "," << QString("insert into %1(%2) values(%3)")
    //                .arg(tablename)
    //                .arg(p0)
    //                .arg(p1);
    //    qDebug() << ("update-"+tablename) << ","<< QString("update %1 set %2 ")
    //                .arg(tablename)
    //                .arg(update_p0);
    //
}


//=========================
#include "ui_public_sqlset_dialog.h"
#include <QFontDialog>
#include "public_logindialog.h"

public_sqlSet::public_sqlSet(QWidget *parent):
    QDialog(parent),
    ui(new Ui_public_sqlset_Dialog)
{
    ui->setupUi(this);
    switch_server_local(public_sql::islocal);
    ui->checkBox->setChecked(lds::sysconf->value("public_sql/islocal", true).toBool());

    //! toolbar
    ui->widget->setup(QStringList() /*<< QObject::tr("重置本地文件路径")*/<< QObject::tr("远程权限") << QObject::tr("确定")<< QObject::tr("退出"), Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("确定")), SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->checkBox,SIGNAL(toggled(bool)), this, SLOT(switch_server_local(bool)));
    connect(ui->widget->index_widget(QObject::tr("远程权限")), SIGNAL(clicked()),this,SLOT(enable_remote_MYSQL_connect()));
}

bool public_sqlSet::privileges(QString *errstring)
{
    lds_query_hddpos  query;
    if(privileges(&query)) {
        return true;
    }
    if(errstring)
        *errstring = query.recordError();
    return false;
}

bool public_sqlSet::privileges(lds_query_db *query)
{
    //    query->execDirect("grant all privileges on *.* to root@\"%\" identified by \".\";"
    //                      "flush privileges;");

    if(query->execDirect(QString(" grant all privileges on *.* to root@'%' identified by '%1'; flush privileges; ").arg(public_sql::getdbPassword()))) {
        return true;
    }
    return false;
}

void public_sqlSet::took()
{
    if(public_sql::islocal) {
        public_sql::localdbHostName = ui->lineEdit_hostname->text();
        public_sql::localdbDatabaseName = ui->lineEdit_dbname->text();
        public_sql::localdbUserName = ui->lineEdit_usrname->text();
        public_sql::localdbPassword = ui->lineEdit_password->text();
    } else {
        public_sql::dbHostName = ui->lineEdit_hostname->text();
        public_sql::dbDatabaseName = ui->lineEdit_dbname->text();
        public_sql::dbUserName = ui->lineEdit_usrname->text();
        public_sql::dbPassword = ui->lineEdit_password->text();
    }
    public_sql::MySQL_PARAM_SAVE();

    this->accept();
}

void public_sqlSet::toexit()
{
    this->reject();
}

void public_sqlSet::switch_server_local(bool flag)
{
    public_sql::islocal = flag;
    ui->lineEdit_hostname->setText(public_sql::getdbHostName());
    ui->lineEdit_dbname->setText(public_sql::getdbDatabaseName());
    ui->lineEdit_usrname->setText(public_sql::getdbUserName());
    ui->lineEdit_password->setText(public_sql::getdbPassword());

    ui->lineEdit_hostname->setEnabled(!flag);
    ui->lineEdit_dbname->setEnabled(!flag);

#ifdef Q_OS_UNIX
    ui->lineEdit_usrname->setEnabled(!flag);
#endif
}

void public_sqlSet::tohddpos_yaoqianshu()
{
    //conduct outer exe
    QProcess p;
    p.start("\""+lds::localdataPath +"/MyApplication"+"\"");
    p.waitForFinished(-1);
    p.close();
}

void public_sqlSet::enable_remote_MYSQL_connect()
{
    QString errstring;
    if(privileges(&errstring)) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        return;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败")+errstring);
}


//===================
WX_POLLING_thread::WX_POLLING_thread(QObject *parent):
    lds_thread("无线点菜机", parent)
{
}

WX_POLLING_thread::~WX_POLLING_thread()
{
}


void WX_POLLING_thread::run()
{

    w_sys_manage_wirelessordermonitoring_wx  wx;
    wx.enableUDP();
#ifdef QT_DEBUG
    //    QTimer timer;
    //    connect(&timer, SIGNAL(timeout()), &wx, SLOT(DC()));
    //    timer.start(1000);
#endif
    this->exec();
}

void public_sql::OPER_KEY_MODEL_INIT()
{
    public_short_cut_map::OPER_KEY_MODEL_INIT();
}

//-1 rejct\ 0 errstring \1 success
int public_sql::get_god_password(QWidget *parent, const QString &title, QString *errstring)
{
    lds_dialog_input inputdialog(parent);
    inputdialog.ui->label->setText(QObject::tr("密码") + ":");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(title);
    if(QDialog::Accepted==transparentCenterDialog(&inputdialog).exec()) {
        if(inputdialog.ui->lineEdit->text()!=n_func::f_get_godpassword()) {
            if(errstring) *errstring = QObject::tr("密码错误");
            return 0;
        }
        return 1;
    }
    if(errstring) *errstring = QObject::tr("手动取消");
    return -1;
}

bool public_sql::get_password(QWidget *parent, const QString &title, QString &password)
{
    lds_dialog_input inputdialog(parent);
    inputdialog.ui->label->setText(QObject::tr("密码") + ":");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(title);
    if(QDialog::Accepted==transparentCenterDialog(&inputdialog).exec()) {
        password = inputdialog.ui->lineEdit->text();
        return true;
    }
    return false;
}

void public_sql::softUpdate()
{
#ifdef Q_OS_WIN
    wchar_t version[256];
    int max = fastfd_update::gs_soft_version.toWCharArray(version);
    version[max] = 0;

    ::ShellExecute(0,
                   (LPCWSTR)L"runas",
                   (LPCWSTR)L"fastfd_updated.exe",
                   (LPCWSTR)version,
                   (LPCWSTR)0,
                   SW_SHOWNORMAL);
#else
    qDebug() << "gs_soft_version:" << fastfd_update::gs_soft_version;
    QProcess::startDetached(qApp->applicationDirPath()+"/fastfd_updated",
                            QStringList() << fastfd_update::gs_soft_version, qApp->applicationDirPath());

#endif
}

bool public_sql::discount_off_mode()
{
    return fexpandmain_model_sqltablemodel::discount_off_mode();
}

double public_sql::discount_get(QWidget *parent)
{
    return 0;
}

bool public_sql::can_discount_limit(QWidget *parent, int int_discount, const QString &title, QString &discount_desc)
{
    QString lds_operid = lds::gs_operid;
    lds_dialog_input inputdialog(parent);
    lds_query_hddpos  query;
    int ldec_opermindiscount;
    QString errstring;

right_try:
    if(lds_operid == "0000") {
        if(discount_off_mode()) {//折让
            ldec_opermindiscount = 100;
        } else {//折扣
            ldec_opermindiscount = 0;
        }
    } else {
        query.execSelect(QString("select int_discount from cey_sys_operator where vch_operID ='%1' and ch_flag = 'Y' ").arg(lds::gs_operid));
        query.next();
        ldec_opermindiscount = query.recordValue("int_discount").toInt();
    }

    if(discount_off_mode()) {//折让
        if(int_discount > ldec_opermindiscount) {
            discount_desc = title + "," + QObject::tr("超出营业员折扣上限")+QString(":%1%>%2%").arg(int_discount).arg(ldec_opermindiscount);
            goto right_upgrade;
        }
    } else {
        if(int_discount < ldec_opermindiscount) {
            discount_desc = title + "," + QObject::tr("超出营业员折扣下限")+QString(":%1%<%2%").arg(int_discount).arg(ldec_opermindiscount);
            goto right_upgrade;
        }
    }
    goto right_ok;
right_upgrade:
    if(1 == lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, discount_desc, QObject::tr("提升权限"), QObject::tr("取消") )) {
        goto right_cancel;
    }
    inputdialog.showSecondLineEdt();
    inputdialog.setWindowTitle(QObject::tr("提升权限"));
    inputdialog.ui->label->setText(QObject::tr("用户名") + ":");
    inputdialog.ui->label_2->setText(QObject::tr("密码") + ":");
    inputdialog.ui->lineEdit_2->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText("0000");
    inputdialog.ui->lineEdit_2->setText(n_func::f_get_godpassword());
#endif
    if(QDialog::Accepted != transparentCenterDialog(&inputdialog).exec()) {
        goto right_cancel;
    }
    lds_operid = inputdialog.ui->lineEdit->text();
    errstring = n_func::f_confirm_user_pwd(lds_operid, inputdialog.ui->lineEdit_2->text());
    if(errstring.count() > 0) {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, errstring);
        goto right_cancel;
    }
    goto right_try;

right_ok:
    return true;
right_cancel:
    discount_desc = QObject::tr("手动退出");
    return false;
}

bool public_sql::localContainIp(const QString &ip)
{
    return QNetworkInterface::allAddresses().indexOf(QHostAddress(ip)) != -1;
}

void public_sql::doublescreensenddataByconf(const lds::DOUBLE_DATA &ddata)
{
    lds_scr_qt::doublescreensenddataByconf(ddata);
}

int public_sql::tableColumnCount(const QString &tablename)
{
    lds_query_hddpos q;
    int count = 0;
    q.execSelect("desc " + tablename);
    while(q.next())
        count ++;
    return count;
}

void public_sql::drawTextForArabic(QPainter *painter, QRect rect, int align, const QString &text)
{
    drawTextForArabic(painter, QRectF(rect), align, text);
}

void public_sql::drawTextForArabic(QPainter *painter, QRectF rect, int align, const QString &text)
{
    int ar_begin = -1;
    int ar_end = 0;
    for(int k = 0; k < text.count(); k ++) {
        if(text[k].direction() == QChar::DirAL) {
            if(ar_begin == -1)
                ar_begin = k;
            ar_end = k;
        }
    }

    if(ar_begin != -1) {
        rect.adjust(0, 0, 0, 10);
        QRectF r2 = rect;
        QString part;
        if(align & Qt::AlignLeft)
        {
            qreal xoff = 0;

            QRectF r1, r3;
            part = text.mid(0, ar_begin); r1 = rect;xoff += printArabicWidth(painter, part);
            part = text.mid(ar_begin, ar_end - ar_begin + 1); r2 = rect.translated(xoff, 0);xoff += printArabicWidth(painter, part);
            part = text.mid(ar_end + 1); r3 = rect.translated(xoff, 0);xoff += printArabicWidth(painter, part);

            painter->drawText(r1, align | Qt::AlignVCenter, text.mid(0, ar_begin));
            painter->drawText(r3, align | Qt::AlignVCenter, text.mid(ar_end + 1));

            r2 = QRectF(r2.x(), r2.y(), r3.x() - r2.x(), r2.height());

            part = text.mid(ar_begin, ar_end - ar_begin + 1);
            QFont f0 = painter->font();
            QFont sys_font = painter->font();
            sys_font.setFamily(qApp->font().family());
            QFontMetrics sys_font_m(sys_font);

            if(sys_font_m.width(part) > r2.width()) {
                sys_font.setPointSizeF(1.0 * r2.width() / sys_font_m.width(part)* sys_font.pointSizeF());
            }
            painter->setFont(sys_font);
#ifdef QT_DEBUG
            //        painter->drawRect(r2);
#endif
            painter->drawText(r2, align | Qt::AlignTop, part);
            painter->setFont(f0);
            return;
        }
    }
#ifdef QT_DEBUG
    //    painter->drawRect(rect);
#endif
    painter->drawText(rect, align | Qt::AlignVCenter, text);
}

int public_sql::printArabicWidth(QPainter *painter, const QString &text)
{
    QFontMetrics fm(painter->font());
    return qMax(fm.width(QString(text.count(), ' ')), 0);//fm.width(text));
}

bool public_sql::ObjContain(QObject *o, const QString &name)
{
    if(o == 0)
        return false;
    return o->findChild<QWidget*>(name) != 0
            || o->objectName() == name;
}

int public_sql::func_dec_price()
{
    return lds_query_hddpos::selectValue(" select vch_value from cey_sys_parameter where vch_parameter = 'bp_fmt_price_dec' ").toInt();
}

int public_sql::func_dec_num()
{
    return lds_query_hddpos::selectValue(" select vch_value from cey_sys_parameter where vch_parameter = 'bp_fmt_num_dec' ").toInt();
}

int public_sql::func_dec_amt()
{
    return lds_query_hddpos::selectValue(" select vch_value from cey_sys_parameter where vch_parameter = 'bp_fmt_amount_dec' ").toInt();
}

bool public_sql::candoPerday(const QDateTime &dt_cur, const QTime &time_do, int secs_off, const QString &key)
{
    if(dt_cur.isNull() || time_do.isNull())
        return false;
    QDateTime dt_do = QDateTime(dt_cur.date(), time_do);
    QString sys_key = key + dt_do.toString(yyyyMMdd);
    if(lds::systemValue(sys_key, false).toBool() == false) {
        if(qAbs(dt_cur.secsTo(dt_do)) <= secs_off) {//相差一分钟
            lds::setSystemValue(sys_key, true);
            return true;
        }
    }
    return false;
}

TelBox_thread_event::TelBox_thread_event(QObject *parent) : lds_thread_event(parent)
{
}

info_tel TelBox_thread_event::readTel()
{
    Reader_Tel tel;
    info_tel t;
    QString errstring;
    if(false == tel.open(&errstring)) {
        QMetaObject::invokeMethod(public_sql::save_login_object(),
                                  "signal_telNoComingState",Qt::QueuedConnection,Q_ARG(QString, QObject::tr("操作失败") +"\t" + errstring)  );
        return t;
    }
    t = tel.getTelInfo();
    if(false == t.isvalid) {
        QMetaObject::invokeMethod(public_sql::save_login_object(),
                                  "signal_telNoComingState",Qt::QueuedConnection,Q_ARG(QString, QObject::tr("操作成功") +"\t" + t.errstring)  );
    } else {
        QMetaObject::invokeMethod(public_sql::save_login_object(),
                                  "signal_telNoComingState",Qt::QueuedConnection,Q_ARG(QString, QObject::tr("操作成功") +"\tcallerID:" + t.callerID)  );
    }

    return t;
}

void TelBox_thread_event::toExec()
{
    info_tel t = readTel();

    ///////////////////////////////////////////////////////////////////////////////////
    if(t.isvalid) {
        ///////////////////
        QFile file(lds::localdataPath + "/TelNoComing.txt");
        //check file existed
        if(false == file.exists()) {
            file.open(QFile::WriteOnly);
            file.close();
        }
        //check tel existed
        if(false == file.open(QFile::ReadOnly)) {
            qDebug()<<__LINE__ << __FILE__ << file.errorString();
            return;
        }
        QByteArray content;
        bool isfind = false;
        while(!file.atEnd()) {
            QByteArray line = file.readLine();
            QString line1 = line;
            QString callerID = line1.split(",").value(0).trimmed();
            if(callerID == t.callerID) {
                line = QByteArray().append(callerID + "," + QDateTime::currentDateTime().toString("MM-dd hh:mm") + "\n");
                isfind = true;
            }
            content += line;
        }
        if(false == isfind) {
            content += t.callerID + "," + QDateTime::currentDateTime().toString("MM-dd hh:mm") + "\n";
        }
        file.close();
        //append
        if(false == file.open(QFile::WriteOnly)) {
            qDebug()<<__LINE__ << __FILE__ << file.errorString();
            return;
        }
        file.write(content);
        file.close();
        /////////////////////
        QMetaObject::invokeMethod(public_sql::save_login_object(),
                                  "toTelNoComing",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, t.callerID)
                                  );
    }
}

TelBox_thread::TelBox_thread(QObject *parent)
    : lds_thread("电话盒子", parent)
{

}

void TelBox_thread::run()
{

    TelBox_thread_event event;
    execEvent(&event);
}
