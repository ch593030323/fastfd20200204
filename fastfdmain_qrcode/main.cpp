#include <QApplication>
#include <QLabel>
#include <QtDebug>
#include "public_logindialog.h"
#include "backheader.h"
#include "w_i_inventory.h"
#include "w_sys_manage.h"
#include "w_bt_dish.h"
#include "w_bt_dish_kitchenprinter.h"
#include "lds.h"
#include "alipay_help.h"
#include "mainwindow.h"
#include "s610pos.h"
#include "lds_calendarwidget.h"
#include "w_sys_manage_wirelessordermonitoring.h"
#include "lds.h"
#include "lds_messagebox.h"
#include <QProcess>
#include "barscale_main_dialog.h"
#include "ui_w_m_member_sendcard_dialog.h"
#include "printer_pos_serial.h"
#include "w_bt_dish_base.h"
#include "lds_roundeddialog_rect_align.h"
#include "public_pushbutton_numkey.h"
#include "electronicscale_rongta.h"
#include "system_setting_mobile_max_connect.h"
#include "printer_pos_net.h"
#include "fexpandmain_2.h"
#include "back_view_dialog.h"
#include "ui_w_scr_dish_restaurant_dialog.h"
#include "w_scr_dish_restaurant_dialog.h"
#include "back_view_dialog.h"
#include "ui_back_view_dialog.h"
#include <QStringList>
#include "w_scr_dish_pay_widget.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "lds_toolbar_button.h"
#include "w_scr_dish_restaurant_table_select.h"
#include "w_rpt_report_frame_dishsale.h"
#include "w_rpt_report_frame_cashflow.h"
#include "w_rpt_report_frame_cash.h"
#include "w_bt_dish_kitchen_dish_state.h"
#include "w_sys_manage_outer_settings.h"
#include "w_rpt_report_frame_dishsalerank.h"
#include "colordialog.h"
#include "saas_pay_detail.h"
#include "w_sys_manage_cloudsync_cloud2.h"
#include "colordialog.h"
#include "saas_guide.h"
#include "w_scr_dish_pay_widget.h"
#include "w_scr_dish_pay_dialog.h"
#include "saas_guide_hard_test.h"
#include "w_inventory_input.h"
#include "saas_guide_hard_test.h"
#include "w_sys_manage_cloudsync_transfer.h"
#include "saas_guide_direct.h"
#include "saas_guide2.h"
#include "saas_guide_pay.h"
#include "saas_guide_num_key.h"
#include "printer_pos_vidpid.h"
#include "saas_guide_init_soft.h"
#include "fastfd_update.h"
#include "public_showimages.h"
#include "w_scr_dish_main_bar_cancel_win.h"
#ifdef LINPOS_OS6
#include <QDBusInterface>
#include "frminputnew.h"
#endif
#include "w_inventory_draw.h"
#include "saas_guide_pay_rbcode.h"
#include "commonheader.h"
#include "w_bt_dish_cookset.h"
#include "w_scr_dish_main_table_800x600.h"
#include "w_scr_dish_phone_pay.h"
#include "transparentcenterdialog.h"
#include "w_scr_dish_pay_kitchen_quit_request.h"
#include "w_scr_dish_main_view_scale.h"
#include "w_sys_manage_cloudsync_with.h"
#include "fupay_help.h"
#include "w_scr_dish_main_qr_code_over_view_cash_pay.h"
#include "w_bt_dish_kitchenprinter_printerset.h"
#include "w_scr_dish_main_qr_code_over_view.h"
#include <QSqlError>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    QMap<QString, QString> argMap;
#ifdef QT_DEBUG
    argMap.insert("-us", "0000");
    argMap.insert("-un", "0000");
    argMap.insert("-pw", "0000");

    argMap.insert("-dd", "hddpos");
    argMap.insert("-dh", "192.168.1.136");
    argMap.insert("-du", "root");
    argMap.insert("-dp", "888888");
    argMap.insert("-sk", "obsidian");
#endif
    for(int k = 1; k < app.argc(); k ++) {
        argMap.insert(app.arguments().value(k).left(3), app.arguments().value(k).mid(3));
    }

    QString errstring;
    //新系统 且 unix 且 发布
#ifdef LINPOS_OS6
    frmInputNew::Instance();
#endif
    public_sql::    syssetcode();                           //设置编号
    //配置文件
    QSettings conf(lds::localdataPath +"/fastfd.ini", QSettings::IniFormat);
    lds::sysconf = &conf;
    public_sql::    systranslate();                         //翻译
    public_sql::    syswindowicon();
    app.setStyleSheet(public_sql::stylesheetData(argMap.value("-sk", "obsidian")));

    switch(lds::LDS_INIT(&errstring)) {
    case lds::resultError:
        lds_messagebox::warning(0, MESSAGE_TITLE_VOID, errstring);
        return 0;
    case lds::resultInfo:
        lds_messagebox::information(0, MESSAGE_TITLE_VOID, errstring);
        break;
    }

    ///
    //fastfddlist fastfdlist
    QStringList fastfdlist;
    //相比SoftPos少一个s，以免后面匹配Pos时，多一个
    public_sql::cmd_ps_pid(fastfdlist, "pos/qrcod", "pos/qrcode");
    if(fastfdlist.count() > 1) {

        if(1 == QMessageBox::critical(0, MESSAGE_TITLE_VOID, QObject::tr("软件已启动") +":"+"self"+QString::number(app.applicationPid())+"fastfdlist:"+fastfdlist.join("|"), QObject::tr("关闭软件"),QObject::tr("退出"))) {
            return 0;
        }
        QProcess p;
        //fastfdd异常的话，全部杀死
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
    ///~


    lds::gs_operid.setOther(argMap.value("-us"));//usr
    lds::gs_opername.setOther(argMap.value("-un"));//usrname
    lds::gs_password.setOther(argMap.value("-pw"));//password

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setDatabaseName(argMap.value("-dd"));
        db.setHostName(argMap.value("-dh"));
        db.setUserName( argMap.value("-du"));
        db.setPassword(argMap.value("-dp"));
        db.setConnectOptions("MYSQL_OPT_RECONNECT=1;");
        if(!db.open()) {
            lds_messagebox::warning(0, MESSAGE_TITLE_VOID, db.lastError().text());
            return 0;
        }

        lds_query_hddpos  query;
        query.execSelect(QString(" select int_id from cey_sys_lan_terminal where vch_terminal = '%1' ")
                         .arg(lds::terminalCode));
        query.next();
        lds::LOCAL_TERMINAL_INDEX_FF.setOther(QString().sprintf("%02X",  query.record().value("int_id").toInt()));
        qDebug() << lds::LOCAL_TERMINAL_INDEX_FF;
    }

    lds::reportModelMapValue("");
    public_sql::    arg_forbid_debug(argc, argv);               //debug模式，获取usr password debug等信息

    public_loginDialog login;
    login.qt_recipt_print_polling = new w_bt_dish_Kitchenprinter_printer_thread(&login);
    login.qt_recipt_print_polling->start();

    w_scr_dish_main_qr_code_over_view dialog(0);
    dialog.setWindowFlags(dialog.windowFlags() | Qt::FramelessWindowHint);
    dialog.setGeometry(QRect(QPoint(0, 0), lds::MAIN_WINDOW_SIZE));
    dialog.exec();

    int ret = app.exec();

    return ret;
}
