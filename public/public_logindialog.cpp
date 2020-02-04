#include "public_logindialog.h"
#include <QDebug>
#include "n_func.h"
#include "backheader.h"
#include "w_m_member.h"
#include "lds_query_hddpos.h"
#include <QTimer>
#include <QPixmap>
#include "printer_0.h"
#include "w_scr_dish_restaurant_dialog.h"
#include "w_i_inventory.h"
#include "w_sys_manage.h"
#include "lds_messagebox.h"
#include "backheader.h"
#include <QInputDialog>
#include <QDir>
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "w_sys_manage_sqlcopy.h"
#include <QSqlError>
#include "w_bt_dish_kitchenprinter.h"
#include "public_printerr_dialog.h"
#include "w_scr_dish.h"
#include <QProcess>
#include "w_sys_manage_wirelessordermonitoring.h"
#include "w_scr_dish.h"
#include "w_bt_dish_kitchenprinter.h"
#include "w_bt_dish_table.h"
#include "lds.h"
#include "getfilepath.h"
#include "restauranttableview.h"
#include "ui_login_dialog.h"
#include "w_bt_dish_kitchen_dish.h"
#include "fexpandmain_2.h"
#include <QSqlDriver>
#include <QPointer>
#include <QHostInfo>
#include <QTcpServer>
#include "lds_roundeddialog_rect_align.h"
#include "back_view_dialog.h"
#include "w_scr_dish_pay_widget.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "w_sys_manage_outer_pay_set.h"
#include "w_bt_dish_kitchen_dish_state.h"
#include "w_bt_dish.h"
#include "business_data_upload_isoft.h"
#include "w_scr_dish_pay_widget.h"
#include "lds_getdishavailable.h"
#include "fastfd_update.h"
#include "pointerautodel.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "w_scr_dish_main_view_800x600.h"
#include "w_scr_dish_main_table_800x600.h"
#include "w_bt_dish_kitchenprinter_printerset.h"
#include "saas_guide.h"
#include "printer_pos_serial.h"
#include "w_scr_dish.h"
#include "business_documents_query.h"
#include "lds_ogg_play.h"
#include "w_sys_manage_outer_pay_set_fengze.h"
#include "w_scr_dish_main_view_scale.h"
#include "w_sys_manage_outer_pay_set_locale_wizard.h"
#include "printer_pos_net.h"
#include "lock_screen.h"
#include "business_data_upload_restful.h"
#include "business_interface_malayisa_hourly_pos.h"
#include "lds_lan_terminal.h"
#include "lds_query_thread.h"
#include "w_bt_dish_signal_transfer.h"

public_loginDialog::public_loginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_login_Dialog),
    info_sqr(0),
    sql_isopen(false)
{
    ui->setupUi(this);
    lds::setwflagFrameless(this);
    sql_heart_beat = 0;
    wxpolling = 0;
    ttpolling = 0;
    kitchenpolling = 0;
    QPixmap p(lds::sysconf->value("public_loginDialog/logo").toString());
    if(!p.isNull()) {
        ui->toolButton_logo->setIcon(p);
    } else {
        ui->toolButton_logo->setIcon(QPixmap(":/image/logo_top.png"));
    }
    ui->pushButton_can_update->setText("");
    ui->label_info->setText("");
#ifdef Q_OS_WIN
    wx_OS_WIN.enableWindowSLOT();
#endif

    lds::showWidget(ui->label_2, ui->lineEdit_pwd, lds::sysconf->value("system_setting/login_with_pwd","0").toString() == "1");

    runtimer = new QTimer(this);
    runtimer->setInterval(1000);
    connect(runtimer,SIGNAL(timeout()),this,SLOT(runFrameupdate()));
    connect(ui->pushButton_cp_SoftPos, SIGNAL(clicked()), this, SLOT(to_cp_SoftPos()));
}

public_loginDialog::~public_loginDialog()
{
    delete ui;
}

void public_loginDialog::setversion(const QString &text)
{
    //软件版本号
    qDebug() << "软件版本号:" << text;
    ui->label_version->setText(fastfd_update::gs_soft_version);
}

void public_loginDialog::init(const QString &u, const QString &p)
{
    //信号连接
    connect(ui->toolButton_dbset,SIGNAL(clicked()),this,SLOT(todbset()));
    connect(ui->pushButton_close,SIGNAL(clicked()),this,SLOT(goto_close()));
    connect(ui->pushButton_pre,SIGNAL(clicked()),this,SLOT(togoto_pre()));
    connect(ui->pushButton_back,SIGNAL(clicked()),this,SLOT(togoto_back()));
    connect(ui->lineEdit_usr,SIGNAL(returnPressed()),this,SLOT(tousr2pwd()));
    connect(ui->lineEdit_pwd,SIGNAL(returnPressed()),ui->pushButton_pre,SLOT(animateClick()));
    connect(ui->toolButton_logo,SIGNAL(clicked()),this,SLOT(toupdatelogo()));
    //! after
    ui->lineEdit_usr->setFocus();
    //

#ifdef QT_DEBUG
    ui->lineEdit_usr->setText("0000");
    ui->lineEdit_pwd->setText(n_func::f_get_godpassword());
#endif
    if(!p.isEmpty()) {
        ui->lineEdit_pwd->setText(p);
    }
    if(!u.isEmpty()) {
        ui->lineEdit_usr->setText(u);
    }
}

void public_loginDialog::set_num()
{

}

void public_loginDialog::goto_back()
{
    if(false == backheader::operate_right_check_code(this, "0244")) {//后台
        return;
    }
    if(this->isVisible()) {
        this->hide();
    }

    int retexec = -1;
    {
        back_view_dialog dialog;
        dialog.move(0,0);
        retexec = dialog.exec();
    }
    switch(retexec) {
    case public_sql::Rejected:
        goto_close();
        break;
    case public_sql::Accepted:
        goto_pre();
        break;
    }
}

void public_loginDialog::goto_pre()
{
    if(false == backheader::operate_right_check_code(this, "0241")) {//前台
        return;
    }
    if(this->isVisible()) {
        this->hide();
    }
    int retexec = 0;
#ifdef QT_SCPOS_SUPPORT
    {
        lds_query_hddpos  query;
        query.execSelect(QString("select ch_billno from cey_sys_lan_terminal where ch_billno<> '' and vch_terminal = '%1' ").arg(lds::terminalCode));
        if(query.next()) {
            if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("快餐上次反结账未完成"), QObject::tr("继续"), QObject::tr("取消"))) {
                business_documents_query::doUnCheck(lds_scr_table::Q000, query.recordValue("ch_billno").toString());
            }
        }
        if(lds::sysconf->value("w_sys_manage_outer_pay_set/scale_led_mode", false).toBool()) {
            w_scr_dish_main_view_scale dialog(0);
            dialog.move(0, 0);
            retexec = dialog.exec();
        } else {
            w_scr_dish_main_view_800x600 dialog(0);
            dialog.move(0, 0);
            retexec = dialog.exec();
        }
    }
#else
    switch(lds::sysconf->value("system_setting/yingyemoshi", 0).toInt()) {
    case 0:
    {
        lds_query_hddpos  query;
        query.execSelect(QString("select ch_billno from cey_sys_lan_terminal where ch_billno<> '' and vch_terminal = '%1' ").arg(lds::terminalCode));
        if(query.next()) {
            if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("快餐上次反结账未完成"), QObject::tr("继续"), QObject::tr("取消"))) {
                business_documents_query::doUnCheck(lds_scr_table::Q000, query.recordValue("ch_billno").toString());
            }
        }

        if(lds::sysconf->value("w_sys_manage_outer_pay_set/scale_led_mode", false).toBool()) {
            w_scr_dish_main_view_scale dialog(0);
            dialog.move(0, 0);
            retexec = dialog.exec();
        } else {
            w_scr_dish_main_view_800x600 dialog(0);
            dialog.move(0, 0);
            retexec = dialog.exec();
        }
    }
        break;
    case 1:
    case 2:
    {
        w_scr_dish_restaurant_dialog fastd(0);
        fastd.move(0,0);
        //开启基站，启动成功返回true
        if(lds::sysconf->value("w_sys_manage_wirelessordermonitoring/start", false).toBool()) {
            w_sys_manage_wirelessordermonitoring::startjz(this);
        }
        retexec = fastd.exec();
    }
        break;
    default:
        return;
    }
#endif

    switch(retexec) {
    case public_sql::Rejected:
        goto_close();
        break;
    case public_sql::Accepted:
        goto_back();
        break;
    case public_sql::GoFast:
        //yingyemoshi_pre 用于酒吧模式,切换快餐模式下,后还能转回酒吧模式
        lds::sysconf->setValue("system_setting/yingyemoshi_pre", lds::sysconf->value("system_setting/yingyemoshi", 0));
        lds::sysconf->setValue("system_setting/yingyemoshi", 0);
        goto_pre();
        break;
    case public_sql::GoRestaurant:
        lds::sysconf->setValue("system_setting/yingyemoshi", lds::sysconf->value("system_setting/yingyemoshi_pre", 1).toInt());
        goto_pre();
        break;
    }
}

bool public_loginDialog::goto_prerestaurant(const QString &ch_tableno, const QString &ch_billno)
{
    lds_scr_table::cey_bt_table_lock(ch_tableno, 0);

    w_scr_dish_main_table_800x600 dialog(ch_tableno, ch_billno, 0);
    dialog.move(0, 0);
    if(QDialog::Accepted ==dialog.exec()) {//没有付款或付款失败/取消
        lds_scr_table::cey_bt_table_update(ch_tableno, "", 0);
    }
    lds_scr_table::cey_bt_table_unlock(ch_tableno, 0);
    return true;
}

void public_loginDialog::togoto_back()
{
    QString errlogin=toTryLogin();
    if(errlogin.isEmpty()) {
        goto_back();
    } else {
        updateinfo(errlogin);
    }
}

void public_loginDialog::togoto_pre()
{
    QString errlogin=toTryLogin();
    if(errlogin.isEmpty()) {
        goto_pre();
    } else {
        updateinfo(errlogin);
    }
}

void public_loginDialog::updateinfo(const QString &errinfo)
{
    if(errinfo.isEmpty()) return;
    ui->label_info->setText(QString::number(info_sqr++)+errinfo);
}

void public_loginDialog::connectdb_init(lds_messagebox_loading_show *loading)
{
    //连接数据库
    loading->setText(QObject::tr("正在连接数据库..."), 1);
    sql_isopen = public_sql::CONNECT_MySQL(loading);
    updateinfo(public_sql::MySQL_info);
    if(false == sql_isopen) return;

    QString errstring;
    lds_query_hddpos  query;
    loading->setText(QObject::tr("正在数据库升级..."));
    //升级数据库
    if(lds::arg_d_value.toInt() == 0) {
        query.execSelect("select sqlversion from cey_sys_sqlversion order by sqlversion desc");//从大到小
        query.next();
        loading->setText(QObject::tr("数据库升级"));
        lds_query_hddpos::tran_saction();
        public_sql::MySQL_sqlversion_update(query.recordValue("sqlversion").toString(), errstring, loading);
        public_sql::Other_sql_update(this);//不会影响前他升级
        lds_query_hddpos::com_mit();
    }
    loading->setText(QObject::tr("初始化..."));
    //若是sd卡，备份
    lds::sysconf->setValue("w_sys_manage_sqlcopy/radioButton_2", QDir("/mnt/sdcard").exists());//强制根据sd卡是否插入，判断是否备份
    if(lds::sysconf->value("w_sys_manage_sqlcopy_backup_strategy/sql_start_backup", false).toBool()) {
        if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID,  QObject::tr("开机备份") +"\n"+ QObject::tr("是否备份数据库?"), QObject::tr("确定"), QObject::tr("取消"))) {
            w_sys_manage_sqlcopy::static_tobackup(this);
        }
    }

    //!厨打单内容的生成
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static bool hasother = true;
    if(false == hasother) goto end;
    hasother = false;
    // 局域网连接数
    if(saas_pay_detail::isSaaS()) {
    }
    //vfd开机客显
    public_sql::display_led.setType(lds::sysconf->value("system_setting/kexianleixing").toString());
    public_sql::display_led.startShowinfo(lds::sysconf->value("system_setting/kexian_vfd_start").toString(),
                                          lds::sysconf->value("system_setting/kexianduankou").toString());

    //!线程启动
    threadInit();
    //!线程启动
    goto end;
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("连接云端失败"));
end:
    return;
}

void public_loginDialog::showprinterr(const QString &errstring, const QString &title, int error_id, const QString &int_print_id)
{
    static QPointer<public_printerr_Dialog> dialog = 0;
    if(dialog ) return;//如果控件存在则不在显示

    dialog = new public_printerr_Dialog(qApp->activeWindow());
    dialog->setFixedSize(250, 50);
    dialog->raise();
    dialog->setContent(errstring, title);
    QRect expend_rect = QRect(qApp->desktop()->availableGeometry().topLeft(),  lds::MAIN_WINDOW_SIZE);
    QPoint pos =  lds::sysconf->value("public_printerr/pos").toPoint();
    qDebug() << "expend_rect:" << expend_rect;
    qDebug() << "pos:" << pos;
    if(!pos.isNull() && expend_rect.contains(QRect(pos, dialog->size()))) {

    } else {
        pos = QPoint(lds::MAIN_WINDOW_SIZE.width() - dialog->width(), qApp->desktop()->availableGeometry().y());
    }
    dialog->move(pos);
    dialog->show();

    if(public_sql::error_kitchen == error_id) {
        emit signal_kitchen_print_error(int_print_id);
    }
}

QWidget *public_loginDialog::getcur_validparent()
{
    return qApp->activeWindow();
}

void public_loginDialog::to_sql_heart_beat()
{
    //数据库定时备份
    QDateTime sqldt = n_func::f_get_sysdatetime();
    if(!sqldt.isValid()) {
        if(!lds_query_thread::getThreadDbAndCreat().open() ) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID,  QObject::tr("数据库连接失败"));
        }
    }
    //开启定时备份&&时间范围内
    if(lds::sysconf->value("w_sys_manage_sqlcopy_backup_strategy/sql_time_backup", true).toBool()) {
        if(public_sql::candoPerday(sqldt,
                                   lds::sysconf->value("w_sys_manage_sqlcopy_backup_strategy/sql_time_backup_value", QTime(10,0,0)).toTime(),
                                   60,
                                   "sql_time_backup_")) {
            if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID,  QObject::tr("定时备份") +"\n"+ QObject::tr("是否备份数据库?"), QObject::tr("确定"), QObject::tr("取消"), "", "", true)) {
                w_sys_manage_sqlcopy::static_tobackup(this);
            }
        }
    }
    //开启定时上传&&时间范围内
    if(n_func::f_get_sysparm("hourly_pos_interface_enable") == "1") {
        if(n_func::f_get_sysparm("api_hourly_pos_machine") == lds::terminalCode) {
            if(public_sql::candoPerday(sqldt,
                                       QTime::fromString(n_func::f_get_sysparm("api_hourly_pos_time"), "hh:mm:ss"),
                                       60,
                                       "api_hourly_pos_")) {
                lds_messagebox::information(this, MESSAGE_TITLE_VOID,  "Hour pos interface upload business data", true);
                public_sql::save_login->api_malaysia_hourly_polling->ToTryTrigger();
            }
        }
    }
}

void public_loginDialog::to_everytime_to_print_failed_kitchen_bill()
{
    if(lds::terminalCode == n_func::f_get_sysparm("kitchen_service_terminal")) {//当厨打服务ip和本机ip相同是才会厨打
        kitchenpolling->ToTryTrigger();
        kitchen_print_polling->ToTryTrigger();
    }
    //播放支付demo
    static QList<double> demolist;
    static int demolist_index = 0;
    if(ogg_polling) {
        if(demolist.isEmpty()) {
            demolist << 66.66 << 12345.65 <<  125.88;
        }
        if(false == lds_ogg_play::isPlaying()) {
            ogg_play_polling->play("userdata/settings/ogg/", demolist[demolist_index]);
            demolist_index ++;
            demolist_index = demolist_index % demolist.count();
        }
    }
}

void public_loginDialog::goto_close()
{
    //开始关闭
    lds::DOUBLE_DATA ddata;
    ddata.udpKey = lds::udp_close_request;
    lds::doublescreensenddata(ddata);
    //关闭基站
    w_sys_manage_wirelessordermonitoring::closejz();
    //关闭二维码点餐
    QUdpSocket udpSocket;
    QByteArray byte = "quit";
    udpSocket.writeDatagram(byte.data(),
                            byte.size(),
                            QHostAddress::LocalHost,
                            lds::SYS_QRCODE_ORDER_PORT);

    this->reject();
}


void public_loginDialog::todbset()
{
    if(QDialog::Accepted == public_sql::MySQL_PARAME_SET_EXEC(this)) {
        lds_messagebox_loading_show loading(this, "");
        loading.show_delay();
        if(false == connectdb_init_check(&loading)) {
            this->reject();
        }
    }
}

void public_loginDialog::toversionCheck()
{
}

void public_loginDialog::toupload_polling()
{
    w_scr_dish_pay_widget::toYunPaydataUploadWithTransaction(this, "P17030422400001", QDateTime::fromString("2017-03-04 11:33:04", "yyyy-MM-dd hh:mm:ss"));
}

void public_loginDialog::to_cp_SoftPos()
{
    QString size_x = QString("%1x%2").arg(this->width()).arg(this->height());
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, size_x);
    return;
#ifdef Q_OS_UNIX
    switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID,  "","本地U盘", "指定ftp服务器", "取消")) {
    case 0://本地U盘
    {
        QString readall;
        QByteArray d;
        d.resize(2014);
        FILE *fd = popen("find /media -name SoftPos","r");
        if(fd == NULL) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, "popen error");
            return ;
        }
        fread(d.data(), d.size() - 1, 1,  fd);
        pclose(fd);
        readall = d;

        QStringList u_path_list = readall.split("\n", QString::SkipEmptyParts);
        //
        QDialog dialog(this);
        QComboBox combobox(&dialog);
        QPushButton btnOk("确定", &dialog);
        QPushButton btnCancel("取消", &dialog);
        dialog.resize(500, 100);
        combobox.setGeometry(10, 10, 480, 50);
        btnOk.setGeometry(320, 70, 80, 30);
        btnCancel.setGeometry(410, 70, 80, 30);
        combobox.addItems(u_path_list);
        connect(&btnOk, SIGNAL(clicked()), &dialog, SLOT(accept()));
        connect(&btnCancel, SIGNAL(clicked()), &dialog, SLOT(reject()));
        dialog.setWindowTitle("cp SoftPos");
        if(QDialog::Accepted == dialog.exec()) {
            if(combobox.currentText().contains("SoftPos")) {
                if(0 == system(" mv /usr/desk/pos/SoftPos /usr/desk/pos/SoftPos1")) {
                    if(0 == system(("cp " + combobox.currentText().toLocal8Bit() +" /usr/desk/pos/SoftPos").data())) {
                        if(0 == system(" rm /usr/desk/pos/SoftPos1")) {
                            if(0 == system("chmod +x /usr/desk/pos/SoftPos")) {
                                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, "success");
                                hideall_and_back_login();
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
        break;
    case 1://指定tcp服务器
    {
        lds_dialog_input dialog(this);
        dialog.showSecondLineEdt();
        dialog.ui->label->setText("usrname");
        dialog.ui->label_2->setText("password");
        dialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
        dialog.ui->lineEdit_2->setEchoMode(QLineEdit::Password);
        if(QDialog::Accepted == dialog.exec()) {
            lds_messagebox_loading_show loading(this, "", "1");
            loading.show_delay();
            QUrl url;
            url.setUrl("ftp://115.28.155.165/cyTest/SoftPos");
            url.setUserName(dialog.ui->lineEdit->text());
            url.setPassword(dialog.ui->lineEdit_2->text());

            QNetworkRequest request;
            request.setUrl(url);

            QByteArray datagram;
            QEventLoop loop;
            QNetworkAccessManager m;
            QNetworkReply *reply =  m.get(request);
            connect(reply,SIGNAL(downloadProgress(qint64,qint64)), &loading,  SLOT(updateProgress(qint64,qint64)));
            connect(&m, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
            loop.exec();

            if(reply->error() == QNetworkReply::NoError) {
                datagram = reply->readAll();

                QFile file("/usr/desk/temp_softpos");
                file.open(QFile::WriteOnly);
                file.write(datagram);
                file.close();
                //
                if(0 == system(" mv /usr/desk/pos/SoftPos /usr/desk/pos/SoftPos1")) {
                    if(0 == system(("cp /usr/desk/temp_softpos /usr/desk/pos/SoftPos"))) {
                        if(0 == system(" rm /usr/desk/pos/SoftPos1")) {
                            if(0 == system("chmod +x /usr/desk/pos/SoftPos")) {
                                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, "success");
                                hideall_and_back_login();
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
        break;
    case 2://取消
        break;
    }
#endif
}

void public_loginDialog::tousr2pwd()
{
    if(lds::widgetIsShow(ui->lineEdit_pwd))
        ui->lineEdit_pwd->setFocus();
    else
        ui->pushButton_pre->click();
}

void public_loginDialog::toTelNoComing(const QString &telNo)
{
    emit signal_TelNoComing(telNo);
}

void public_loginDialog::hideall_and_back_login(const QString &p, const QString &u)
{
    QString _p=p;
    QString _u=u;

    if(p.isEmpty() && u.isEmpty()) {
        _u=lds::gs_operid;
        _p=lds::gs_password;
    }

    Printer_POS_SERIAL::SERIAL_PORT_MUTEX_COM_MAP_CLOSE_ALL();
    qApp->closeAllWindows();
    goto_close();
    QProcess::startDetached(qApp->applicationFilePath(), QStringList()<<("-p"+_p)<<("-u"+_u));
}

void public_loginDialog::hideall_and_back_login()
{
    hideall_and_back_login(lds::gs_password,lds::gs_operid);
}

bool public_loginDialog::connectdb_init_check(lds_messagebox_loading_show *loading)
{
    //判断系统盘是否满了
#ifdef LINPOS_OS6
    QString path;
    if(path.isEmpty()) {
        path = "/userdata";
        if(lds_getdishAvailable::availableSpaceMb(path) >= 10) {
            path.clear();
        }
    }
    if(path.isEmpty()) {
        path = "/usr/desk";
        if(lds_getdishAvailable::availableSpaceMb(path) >= 10) {
            path.clear();
        }
    }
    if(!path.isEmpty()) {
        switch(lds_messagebox::critical(this, MESSAGE_TITLE_VOID,  path + QObject::tr("空间不足，请及时清理"), QObject::tr("打开文件管理器"), QObject::tr("继续"), QObject::tr("退出"))) {
        case 0:
            QProcess::startDetached("/usr/desk/file_copy", QStringList(),
                                    "/usr/desk"
                                    );
            break;
        case 1:
            break;
        case 2:
            return false;
        }
    }
#endif
    //～判断系统盘是否满了

    QString errstring;
    //连接数据库
    connectdb_init(loading);
    while(!sql_isopen) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, public_sql::MySQL_info);
        if(QDialog::Accepted == public_sql::MySQL_PARAME_SET_EXEC(this)) {
            connectdb_init(loading);
        } else {
            return false;
        }
    }

    if(sql_isopen) {
        lds_query_hddpos  query;
#ifdef Q_OS_WIN
        //window下， 去掉group by 时， select 只能 针对 group 的内容
        query.execDirect(
                    " SET sql_mode = 'STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION';"
                    );
#endif
        loading->setText("BACKUP db_startup_base.sql");
        public_sql::MySQL_backup("userdata/fastfd_localdata/db_startup_base.sql", QStringList() <<   "t_m_member" << "t_m_curamount" << "cey_bt_dish"  << "cey_bt_dish_subtype" << "cey_bt_dish_type" << "cey_bt_unit");

        const QString country = n_func::f_get_sysparm("locale_country_name");
        if(lds::get_soft_curent_language_first() != _CN_ && country.count() == 0) {
            w_sys_manage_outer_pay_set_locale_wizard d(this);
            if(QDialog::Accepted != transparentCenterDialog(&d).exec()) {
            }
        }

        //主机时间和最后营业时间相差大于30天
        query.execSelect("select max(dt_operdate) from cey_u_checkout_master ");
        if(query.next())
        {
            QDateTime master_max_dt =  query.recordValue(0).toDateTime();
            QDateTime sql_curdt = n_func::f_get_sysdatetime();
            if(master_max_dt.isValid() && qAbs(sql_curdt.daysTo(master_max_dt)) > 30)
            {
                if(1 == lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("主机时间和最后营业时间相差大于30天")+"\n"+QObject::tr("最后营业时间")+":\n"+master_max_dt.toString("yyyy-MM-dd hh:mm:ss")+"\n"+QObject::tr("主机时间")+":\n"+sql_curdt.toString("yyyy-MM-dd hh:mm:ss")+"\n"+QObject::tr("请核实")
                                                ,  QObject::tr("继续"), QObject::tr("取消") )) {
                    //手动取消
                    return false;
                }
            }
        }

        query.execSelect(" select min(dt_operdate) from cey_u_checkout_master  ");
        //~主机时间和最后营业时间相差大于30天
        if(!lds_lan_terminal::regedit(lds::terminalCode, errstring)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
            return false;
        }
        //
        query.execSelect(QString(" select int_id from cey_sys_lan_terminal where vch_terminal = '%1' ")
                         .arg(lds::terminalCode));
        query.next();
        lds::LOCAL_TERMINAL_INDEX_FF.setOther(QString().sprintf("%02X",  query.record().value("int_id").toInt()));
        //~联机多台及其根据机器号不重单付款
        //SaaS 最大连接数
        //        if(saas_pay_detail::isSaaS())
        //        {
        //            query.execInsert("t_st_material_templet",
        //                             qrtVariantPairList()
        //                             << qrtVariantPair("ch_templetname", lds::termainal));
        //            query.execSelect("select count(0) from t_st_material_templet");
        //            query.next();
        //            if(query.recordValue(0).toInt() > saas_pay_detail::saas_lan_num())
        //            {
        //                lds_messagebox::warning(this, MESSAGE_TITLE_VOID,
        //                                        QObject::tr("终端号") + ":" + lds::terminalCode + "\n" +
        //                                        QObject::tr("超过最大连接数") + "\n" +
        //                                        QObject::tr("局域网机器个数") + ":" +QString::number(saas_pay_detail::saas_lan_num()));
        //                return false;
        //            }
        //        }

        //华润接口
        upload_ftp_business(loading);
        //协信接口
        upload_xiexin_business(loading);
    }

    return true;
}

#ifdef LINPOS_OS7
void public_loginDialog::resizeEvent(QResizeEvent *e)
{
    lds::resetWINDOW_SIZE(this->size());
    QDialog::resizeEvent(e);
}
#endif

void public_loginDialog::threadInit()
{
    //无线点菜机
    wxpolling = w_bt_dish_signal_transfer::createObj<WX_POLLING_thread>(this, w_bt_dish_signal_transfer::wxpolling);
    wxpolling->start();
    //科传接口上传
    ttpolling = w_bt_dish_signal_transfer::createObj<w_scr_dish_TECH_TRANS_thread>(this, w_bt_dish_signal_transfer::ttpolling);
    ttpolling->start();
    //厨房打印
    kitchenpolling = w_bt_dish_signal_transfer::createObj<w_bt_dish_kitchen_dish_thread>(this, w_bt_dish_signal_transfer::kitchenpolling);
    kitchenpolling->start();
    //打印机状态判断
    kitchen_print_polling = w_bt_dish_signal_transfer::createObj<w_bt_dish_kitchen_dish_print_state_thread>(this, w_bt_dish_signal_transfer::kitchen_print_polling);
    //    kitchen_print_polling->start();
    //判断云端和本地版本号差异
    yun_version_diff_polling = w_bt_dish_signal_transfer::createObj<w_sys_manage_cloudsync_with_time_check_state_thread>(this, w_bt_dish_signal_transfer::yun_version_diff_polling);
    yun_version_diff_polling->start();
    //流水上传
    yun_flow_upload_polling = w_bt_dish_signal_transfer::createObj<w_sys_manage_cloudsync_with_time_flow_upload_thread>(this, w_bt_dish_signal_transfer::yun_flow_upload_polling);
    //星摩尔接口
    isoft_upload_polling = w_bt_dish_signal_transfer::createObj<business_data_upload_isoft_thread>(this, w_bt_dish_signal_transfer::isoft_upload_polling);
    isoft_upload_polling->start();
    //丰泽接口
    fengze_upload_polling = w_bt_dish_signal_transfer::createObj<w_sys_manage_outer_pay_set_fengze_thread>(this, w_bt_dish_signal_transfer::fengze_upload_polling);
    fengze_upload_polling->start();
    //实时上传-基础资料
    keepdata_time_basedata_polling = w_bt_dish_signal_transfer::createObj<w_sys_manage_cloudsync_with_time_basedata_thread>(this, w_bt_dish_signal_transfer::keepdata_time_basedata_polling);
    keepdata_time_basedata_polling->start();
    //结算线程打印
    qt_recipt_print_polling = w_bt_dish_signal_transfer::createObj<w_bt_dish_Kitchenprinter_printer_thread>(this, w_bt_dish_signal_transfer::qt_recipt_print_polling);
    qt_recipt_print_polling->start();
    //客显
    qt_display_print_polling = w_bt_dish_signal_transfer::createObj<w_bt_dish_Kitchenprinter_display_thread>(this, w_bt_dish_signal_transfer::qt_display_print_polling);
    qt_display_print_polling->start();
    //客显
    tel_box_polling = w_bt_dish_signal_transfer::createObj<TelBox_thread>(this, w_bt_dish_signal_transfer::tel_box_polling);
    tel_box_polling->start();
    //malaysia 接口上传
    restful_upload_polling = w_bt_dish_signal_transfer::createObj<business_data_upload_restful_thread>(this, w_bt_dish_signal_transfer::restful_upload_polling);
    restful_upload_polling->start();
    //音乐
    ogg_play_polling = w_bt_dish_signal_transfer::createObj<lds_ogg_play_thread>(this, w_bt_dish_signal_transfer::ogg_play_polling);
    connect(ogg_play_polling, SIGNAL(signal_error(QString,QString)), this, SLOT(showprinterr(QString, QString)));
    ogg_play_polling->start();
    //辽宁顶有鸣
    liaoning_inventory_keep_polling = w_bt_dish_signal_transfer::createObj<w_scr_dish_liaoning_inventory_keep_thread>(this, w_bt_dish_signal_transfer::liaoning_inventory_keep_polling);
    liaoning_inventory_keep_polling->start();
    //api_malaysia_hourly_polling
    api_malaysia_hourly_polling = w_bt_dish_signal_transfer::createObj<business_interface_malayisa_hourly_pos_thread>(this, w_bt_dish_signal_transfer::api_malaysia_hourly_polling);
    api_malaysia_hourly_polling->start();
    //
#ifdef QT_DEBUG
    w_bt_dish_signal_transfer::check();
#endif
}

#ifdef Q_OS_WIN
bool public_loginDialog::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG* msg = reinterpret_cast<MSG*>(message);
    if (msg->message == 1280)
    {
        emit wx_OS_WIN.signalNative(ulong(msg->lParam));
        return true;
    }
    return false;
}
#endif

QString public_loginDialog::toTryLogin()
{
    //ue_ok (none) returns (none)
    QString ls_operid;
    QString ls_password;
    QString ls_error;
    if(sql_isopen) {
    } else {
        return public_sql::MySQL_info;
    }
    //Has been Shielded.
    ls_operid = ui->lineEdit_usr->text().trimmed();
    ls_password = ui->lineEdit_pwd->text().trimmed();
    if(lds::sysconf->value("system_setting/login_with_pwd","0").toString() == "1")
        ls_error = n_func::f_confirm_user_pwd(ls_operid, ls_password);
    else
        ls_error = n_func::f_confirm_user(ls_operid);
    if(!ls_error.isEmpty()) {
        return ls_error;
    }
    lds::gs_operid.setOther(ls_operid);
    lds::gs_opername.setOther(n_func::f_get_opername(ls_operid));
    lds::gs_password.setOther(ls_password);


    ////////////////////////////////////
    public_short_cut_map::OPER_KEY_MODEL_INIT();
    runStart();

    ///二维码点餐
    if(n_func::f_get_sysparm("qr_code_order_mode","0") == "1") {
        QString cmd_exec = "pos/qrcode";
        QStringList cmd_arg =  QStringList()
                << "-us" + lds::gs_operid
                << "-un" + lds::gs_opername
                << "-pw" + lds::gs_password
                << "-dd" + QSqlDatabase::database().databaseName()
                << "-du" + QSqlDatabase::database().userName()
                << "-dh" + QSqlDatabase::database().hostName()
                << "-dp" + QSqlDatabase::database().password()
                << "-sk" + lds::sysconf->value("system_setting/skin_name", "obsidian").toString()
                   ;

        system(QString(cmd_exec + " " + cmd_arg.join(" ")).toLocal8Bit() + "&");
    }
    return "";
}

void public_loginDialog::toprint_activesize() {
    QWidget *d = qApp->activeWindow();
    if(d)qDebug() <<__FUNCTION__<< d << d->size();
}

void public_loginDialog::toupdatelogo()
{
    QString filepath = getFilePath::getOpenFileName(this, QObject::tr("选择图片"),
                                                    lds::sysconf->value("public_loginDialog/logo").toString());
    if(!filepath.isEmpty()) {
        QPixmap p(filepath);
        if(!p.isNull()) {
            ui->toolButton_logo->setIcon(p);
            lds::sysconf->setValue("public_loginDialog/logo", filepath);
        } else {
            ui->toolButton_logo->setIcon(QPixmap(":/image/logo_top.png"));
            lds::sysconf->setValue("public_loginDialog/logo", ":/image/logo_top.png");
        }
    }
}

void public_loginDialog::upload_ftp_business(lds_messagebox_loading_show *loading)
{
    //ftp营业数据上传
    //tabwidget选中了FTP接口
    if(n_func::f_get_sysparm("ftptp_use") == "1") {
        loading->setText(QObject::tr("正在营业数据上传..."));
        QDate cur_date=n_func::f_get_sysdatetime().date();
        QDate last_update_date = QDate::fromString(n_func::f_get_sysparm("ftptp_lastdate", cur_date.toString("yyyy-MM-dd")), "yyyy-MM-dd");
        QString ls_err;

        while(last_update_date < cur_date) {
            QList<w_scr_dish::HUARUN_BUSINESS_DATA> datalist;
            w_scr_dish::GET_HUARUN_BUSINESS_DATA(last_update_date, datalist, ls_err);
            int ret = w_scr_dish::HUARUN_BUSINESS_DATA_UPLOAD(w_sys_manage_outer_pay_set_ftp::FtpAddress
                                                              , n_func::f_get_sysparm("ftptp_usr")
                                                              , n_func::f_get_sysparm("ftptp_pwd")
                                                              , datalist
                                                              );
            ls_err = w_scr_dish::huarunInfo_error(ret) + ":" + last_update_date.toString("yyyy-MM-dd");

            if(w_scr_dish::huarun_ftp_server_success == ret
                    || w_scr_dish::huarun_local_business_is_empty == ret) {
                loading->setText(QObject::tr("正在营业数据上传...")+last_update_date.toString("yyyy-MM-dd"));
                last_update_date = last_update_date.addDays(1);
                ls_err = "";
                continue;
            }
            if(w_scr_dish::huarun_ftp_file_exists == ret) {
                if(0 == lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QObject::tr("正在营业数据上传") +"\n"+  ls_err+","+QObject::tr("是否继续上传"), QObject::tr("确认"), QObject::tr("取消"))) {
                    loading->setText(ls_err + "," +last_update_date.toString("yyyy-MM-dd"));
                    last_update_date = last_update_date.addDays(1);
                    continue;
                }
                ls_err = "";
            }
            break;
        }
        if(!ls_err.isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QObject::tr("正在营业数据上传") + "\n" +  ls_err);
        }
        //
        n_func::f_set_sysparm("ftptp_lastdate", last_update_date.toString("yyyy-MM-dd"), "FTP LAST UPLOAD DATE");
    }
    //!ftp营业数据上传
}

void public_loginDialog::upload_xiexin_business(lds_messagebox_loading_show *loading)
{
    if(n_func::f_get_sysparm("xiexin_use") == "1")
    {
        //数据校对
        loading->setText(QObject::tr("正在营业数据上传..."));
        QDate cur_date=n_func::f_get_sysdatetime().date();
        QDate last_update_date = QDate::fromString(n_func::f_get_sysparm("xiexin_lastdate", cur_date.toString("yyyy-MM-dd")), "yyyy-MM-dd");
        QString ls_err;
        /**
    */
#ifdef QT_DEBUG
        last_update_date = QDate(2017,1,23);
        cur_date = QDate(2017,1,24);
#endif

        while(last_update_date < cur_date) {
            int ret = w_scr_dish::XIEXIN_BUSINESS_DATA_UPLOAD(last_update_date, &ls_err);
            ///协信营业数据上传: 0 失败 1 成功 2没有营业数据
            if(0 != ret) {
                last_update_date = last_update_date.addDays(1);
                loading->setText(QObject::tr("正在营业数据上传...")+last_update_date.toString("yyyy-MM-dd"));
                continue;
            }
            if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID,  last_update_date.toString("yyyy-MM-dd") + "\n"+ ls_err, QObject::tr("继续"), QObject::tr("取消"))) {
                last_update_date = last_update_date.addDays(1);
                continue;
            }
            break;
        }
        //
        n_func::f_set_sysparm("xiexin_lastdate", last_update_date.toString("yyyy-MM-dd"), "XIEXIN LAST UPLOAD DATE");
    }
}

void public_loginDialog::runStart()
{
    if(runtimer->isActive()) {
        qDebug() << __LINE__ << __FUNCTION__;
        return;
    }

    //厨打5、数据库连接30、中餐刷新10、微信点餐15
    runRestaurantTimeout = 10;
    runKitchenTimeout = 5;
    runHeartTimeout = 30;
    runTelBoxTimeout = 3;

#ifdef QT_DEBUG
    runKitchenTimeout = 1;
    runHeartTimeout = 2;
#endif
    runstep = 0;
    runtimer->start();


    {
        lds::DOUBLE_DATA ddata;
        ddata.udpKey = lds::udp_m_hlabels_request;
        ddata.m_hlabels = QObject::tr("名称") +","+ QObject::tr("价格") +","+ QObject::tr("数量") +","+ QObject::tr("金额");
        ddata.parentPID =qApp->applicationPid();
        public_sql::doublescreensenddataByconf(ddata);
    }
}

void public_loginDialog::runFrameupdate()
{
    if(runstep >= 1000) {
        runstep = 0;
    }
    runstep ++;
    //中餐刷新
    int value;
    if(lds::isqAppActiveWindow("w_scr_dish_restaurant_dialog")) {// public_sql::ObjContain(qApp->activeWindow(), "w_scr_dish_restaurant_dialog")){
        value = runstep%(runRestaurantTimeout+1);
        emit signalRestaurantUpdate(value, runRestaurantTimeout);
    }

    //厨打
    value = runstep%(runKitchenTimeout+1);
    if(value == runKitchenTimeout) {
        to_everytime_to_print_failed_kitchen_bill();
    }

    //数据库心跳
    value = runstep%(runHeartTimeout+1);
    if(value == runHeartTimeout) {
        to_sql_heart_beat();
    }


    //电话盒子
    value = runstep % (runTelBoxTimeout + 1);
    if(value == runTelBoxTimeout) {
        if(lds::sysconf->value("system_setting/read_tel_enable", false).toBool()) {
            tel_box_polling->ToTryTrigger();
        }
    }

    //signalRestrant_auto_lock
    int timeout_auto_lock = 15;//30s触发一次
#ifdef QT_DEBUG
    timeout_auto_lock = 5;
#endif
    value = runstep%(timeout_auto_lock+1);

    if(lds::qAppContaniWidget("w_scr_dish_restaurant_dialog")
            || lds::qAppContaniWidget("w_scr_dish_main_table_800x600")) {
        if(false == lds::isqAppActiveWindow("lock_screen")) {
            if(value == timeout_auto_lock) {
                if(n_func::f_get_sysparm("restrant_auto_lock_flag", "0") == "1") {
                    if(public_sql::restrant_auto_lock_flag) {
                        QTimer::singleShot(100, this, SLOT(restrant_auto_lock_unique()));
                    }
                    public_sql::restrant_auto_lock_flag = true;
                }
            }
        }
    }

    //1s发送一次
    emit signalTimeout_1s();
    //马来西亚 hourly pos上传接口
    //    qDebug() << lds::systemValue(QDate::currentDate().toString(yyyyMMdd), 0) << qAbs(QTime::currentTime().secsTo(QTime::fromString(n_func::f_get_sysparm("api_hourly_pos_time"), "hh:mm:ss")));
    if(lds::systemValue(QDate::currentDate().toString(yyyyMMdd), 0) == 0
            && qAbs(QTime::currentTime().secsTo(QTime::fromString(n_func::f_get_sysparm("api_hourly_pos_time"), "hh:mm:ss"))) < 60) {
        lds::setSystemValue(QDate::currentDate().toString(yyyyMMdd), 1);
    }
}

void public_loginDialog::restrant_auto_lock_unique()
{
    static int k = 0;
    if(k == 1)
        return;
    k = 1;
    lock_screen dialog(this);
    transparentCenterDialog(&dialog).exec();
    k = 0;
}
