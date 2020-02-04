#ifdef LINPOS_OS6
#include <QDBusInterface>
#include "frminputnew.h"
#endif
#include "lds_application.h"
#include "w_sys_manage_sqlcopy_soft_limit2_for_language.h"
#include "public_sql.h"
#include "lds.h"
#include "w_sys_manage_outer_settings.h"
#include <QProcess>
#include "public_logindialog.h"
#include "saas_guide2.h"
#include "fastfd_update.h"
#include "saas_guide_direct.h"
#include "saas_guide_hard_test.h"
#include "business_interface_malayisa_hourly_pos.h"
#include "label_img_header.h"
#include "w_scr_dish_pay_widget_service_manual.h"
#include "w_scr_dish_pay_widget.h"

int main(int argc, char *argv[])
{
    lds_application app(argc, argv);
    QString errstring;
    //新系统 且 unix 且 发布
    //linpos 新系统2017
#ifdef LINPOS_OS6
    frmInputNew::Instance();
#endif
    public_sql::    syssetcode();                           //设置编号
    //配置文件
    QSettings conf(lds::localdataPath +"/fastfd.ini", QSettings::IniFormat);
    lds::sysconf = &conf;
    public_sql::    syswindowicon();
    public_sql::    locale_print_file_init();
    app.setStyleSheet(public_sql::stylesheetData(lds::sysconf->value("system_setting/skin_name", "obsidian").toString()));
    if(QString("ar,uy").contains(lds::get_soft_curent_language_first()))
    {
        app.setLayoutDirection(Qt::RightToLeft);
    }
    //    app.setLayoutDirection(Qt::RightToLeft);

    if(lds::sysconf->value("system_setting/screen_primary_size").isNull()) {
        if(!w_sys_manage_outer_settings::supportDesktopSize()) {
            w_sys_manage_outer_settings::setSystemSize(0);
        }
    }

    switch(lds::LDS_INIT(public_sql::func_dec_price, public_sql::func_dec_num, public_sql::func_dec_amt, &errstring)) {
    case lds::resultError:
        lds_messagebox::warning(0, MESSAGE_TITLE_VOID, errstring);
        return 0;
    case lds::resultInfo:
        lds_messagebox::information(0, MESSAGE_TITLE_VOID, errstring);
        break;
    default:
        break;
    }
    lds::reportModelMapValue("");
    public_sql::    systranslate();                         //翻译
    //~配置文件
    //! 0-----------------判断软件是否正常运行
#ifdef Q_OS_UNIX

    //fastfddlist fastfdlist
    QStringList fastfddlist;
    QStringList fastfdlist;
    //相比SoftPos少一个s，以免后面匹配Pos时，多一个
    public_sql::cmd_ps_pid(fastfdlist, "pos/SoftPo", "pos/SoftPos");
    //相比fastfdd少一个d，以免后面匹配Pos时，多一个
    public_sql::cmd_ps_pid(fastfddlist, "pos/fastfd", "pos/fastfdd");

    if(fastfdlist.count() > 1 || fastfddlist.count() >= 1) {//软件软已经启动，并且本次启动fastfdlist>1；副屏启动时
        if(1 == QMessageBox::critical(0, MESSAGE_TITLE_VOID, QObject::tr("软件已启动") +":"+"self"+QString::number(app.applicationPid())+"fastfdlist:"+fastfdlist.join("|")+"fastfddlist:"+fastfddlist.join("|"), QObject::tr("关闭软件"),QObject::tr("退出"))) {
            return 0;
        }

        QProcess p;
        //fastfdd异常的话，全部杀死
        foreach(const QString &pid, fastfddlist) {
            p.start("kill -9 "+pid);
            p.waitForReadyRead(5*1000);
            p.close();
        }
        //fastfd异常的话，全部杀死
        foreach(const QString &pid, fastfdlist) {
            if(pid == QString::number(app.applicationPid())) {//本身最后杀死
                continue;
            }
            p.start("kill -9 "+pid);
            p.waitForReadyRead(5*1000);
            p.close();
        }
        //杀死本身
        p.start("kill -9 "+QString::number(app.applicationPid()));
        p.waitForReadyRead(5*1000);
        p.close();
        return 0;
    }
#endif

    //! 2-----------------icon设置

    public_sql::    arg_forbid_debug(argc, argv);               //debug模式，获取usr password debug等信息
    //! 4-----------------软件 初始化
    public_sql::    PUBLIC_SQL_INIT();                      //数据库的初始化
    public_sql::    MySQL_PARAM_INIT();                     //mysql的一些初始化
    //! 4-----------------软件到期自检

    while(false == w_sys_manage_sqlcopy_soft_limit2_for_language::checkLimit(lds::get_soft_curent_language_first(), lds::get_soft_curent_language_second(), 0)) {
        if(0 == lds_messagebox::warning(0, MESSAGE_TITLE_VOID, QObject::tr("恢复默认"), QObject::tr("确定"), QObject::tr("取消"))) {
            lds::set_soft_curent_language("zh");
            lds::set_soft_curent_language_second("en");
            public_sql::    systranslate();
            break;
        }
    }

    public_loginDialog login;
    //! 5-----------------登录界面
    public_sql::save_login = &login;
    {
        lds_messagebox_loading_show loading(0, "");
        loading.show_delay();
        //#ifdef QT_DEBUG
        //#else
        if(false == saas_pay_detail::isSaaS()) {//非saas
            loading.setText("软件到期自检");
            if(false == public_sql::syslimitcheck(0))
                return 0;               //软件到期自检
        }
        //#endif

        //新系统
        //! 5 软件版本号
        fastfd_update::setSoftVersiontCyPath(
            #if defined(QT_SCPOS_SUPPORT)
                    "SC"
            #else
                    "V"
            #endif
                    + public_sql::get_MySQL_sqlversion()/*数据库版本*/+"64."/*软件版本*/+"."+"200119"/*修改日期*/
                    );
        //登陆界面ddd
        //! 6-----------------启动
        if(false == public_sql::    sysstart(&login, &loading)) {                       //软件启动
            return 0;
        }
        //saas
        {
            //! 6 SaaS
            if(saas_pay_detail::isSaaS())
            {
#ifdef QT_DEBUG
#else
                //                saaslimitcheck
                loading.setText(QObject::tr("软件到期自检"), 100);
                if(false == saas_pay_detail::saaslimitcheck(0))
                {
                    saas_guide2 d(new saas_guide_hard_test, public_sql::save_login);
                    if(QDialog::Rejected == transparentCenterDialog(&d).exec()) {
                        return 0;
                    }
                    return 0;               //软件到期自检
                }
#endif
            }

            //    软件激活
            if(saas_pay_detail::isSaaS()) {//第二次判断是否为saas
                if(saas_pay_detail::saas_type() == "")
                {
                    saas_guide2 d(new saas_guide_direct(0, public_sql::save_login));
                    if(QDialog::Rejected == transparentCenterDialog(&d).exec()) {
                        return 0;
                    }
                }
                if(saas_pay_detail::saas_type() == "") {
                    lds_messagebox::warning(0, MESSAGE_TITLE_VOID, QObject::tr("软件未激活"));
                    return 0;
                }
                //广告
#ifdef QT_NO_DEBUG
                if(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_is_run", false).toBool()) {
                    loading.setText(QObject::tr("云端广告下载"), 100);
                    saas_pay_detail::saas_update_adv(lds::localdataPath +"/image_double_screen_dir",  &loading);
                }
#endif
            }
        }
    }

    //! 7-----------------若是启动双屏
    if(lds::sysconf->value("system_setting/fuping_mode", -1).toInt() != -1) {
        //开始
        QString cmd_exec =app.applicationDirPath()+"/fastfdd";
        QStringList cmd_arg;
        cmd_arg << "-dp" + lds::sysconf->value("system_setting/bofanglujing").toString();
        cmd_arg << "-ap" + lds::localdataPath + "/image_double_screen_dir";
        cmd_arg << "-sd" + QString(lds::sysconf->value("system_setting/dd_show_dish_list", true).toBool() ? "1" : "0");
        cmd_arg << "-sn" + lds::sysconf->value("system_setting/skin_name", "obsidian").toString();
        cmd_arg << "-ps" + lds::sysconf->value("system_setting/screen_primary_size").toString();
        cmd_arg << "-ss" + lds::sysconf->value("system_setting/bofangg_rate").toString();

        cmd_arg << "-ty" + lds::sysconf->value("system_setting/fuping_mode", -1).toString();
        cmd_arg << "-la" + lds::sysconf->value("system_setting/bofang_layout_dir", 0).toString();
        cmd_arg << "-sx" + QString::number(lds::FULL_WINDOW_SIZE.width());
        cmd_arg << "-fp" + QString() + "userdata/settings/language/" + lds::get_soft_curent_language_first() + ".ttf";

        QString  arg_display = QString("-display Transformed:Rot%1:LinuxFb:/dev/fb1:1 -nomouse -nokeyboard -qws")
                .arg(lds::sysconf->value("system_setting/bofangg_rote", 0).toInt());//为逆时针旋转
        qDebug() << cmd_arg;

#ifdef LINPOS_DEBUG
        system(QString(cmd_exec + " " + cmd_arg.join(" ")).toLocal8Bit() + "&");
#endif
#ifdef LINPOS_OS6
        system(QString(cmd_exec + " " + arg_display + " " + cmd_arg.join(" ")).toLocal8Bit() + "&");
#endif
#ifdef LINPOS_OS7
        system(QString(cmd_exec + " " + cmd_arg.join(" ")).toLocal8Bit() + "&");
#endif
#ifdef LINPOS_WIN
        QProcess::startDetached(cmd_exec, cmd_arg);
#endif
    }
//    w_scr_dish_pay_widget_service_manual_obj d;
//    w_scr_dish_pay_widget_service_manual_obj::PayInterData m;
//    m.num_cost = 15;
//    m.num_service = 20;
//    m.enable_service = true;
//    m.num_pay_low = 100.85;
//    m.num_lowcost =65.85;
//    m.enable_lowcost = true;
//    m.num_paid = 0;
//    m.num_realreceive = 101;
//    m.num_money = 101;
//    m.num_blotout = -0.15;//这里的和付款界面反的!~
//    d.setPayInterData(m);
//    d.print();
//    qDebug()<< __LINE__ << d.isCorrect();
//    d.setMoney(1);
//    d.print();
//    qDebug()<< __LINE__ << d.isCorrect();


//    w_scr_dish_pay_widget_service_manual dialog(0);
//    dialog.initData(m);
//    return dialog.exec();
    return public_sql::save_login->exec();
    bool ret = app.exec();
    return ret;
}

//db_startup_base.sql


