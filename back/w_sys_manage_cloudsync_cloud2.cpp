#include "w_sys_manage_cloudsync_cloud2.h"
#include "ui_w_sys_manage_cloudsync_cloud2.h"
#include "lds.h"
#include "w_sys_manage_cloudsync.h"
#include "lds_messagebox.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "saas_pay_detail.h"
#include <QFile>
#include "public_sql.h"
#include "saas_guide.h"
#include "saas_guide2.h"
#include <QtDebug>
#include "saas_guide_activateterminal.h"
#include "w_sys_manage_cloudsync_with.h"
#include "n_func.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"

w_sys_manage_cloudsync_cloud2::w_sys_manage_cloudsync_cloud2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_cloudsync_cloud2)
{
    ui->setupUi(this);

    ui->pushButton_check_data_updownload->setCheckedByIniN("w_sys_manage_cloudsync_with_time/cloud_yun_is_run", false);
    ui->pushButton_check_yun_member->setCheckedByIniN("w_sys_manage_cloudsync_with_time/cloud_yun_member", false);
    ui->pushButton_check_local_member->setCheckedByIniN("w_sys_manage_cloudsync_with_time/cloud_no_use_local_member", false);

    ui->pushButton_saas_type->setTitleKey(QObject::tr("版本"),  saas_pay_detail::saasDesc());
    ui->pushButton_machine_id->setTitleKey(QObject::tr("机器号"),  lds::terminalCode);//机器号\n4567498746479
    ui->pushButton_machine_id->disableClicked();
    ui->pushButton_host->setTitleKey(QObject::tr("网址"), n_func::f_get_sysparm("cloud_host", "www.yqspos.com"));

    connect(ui->pushButton_data_upload, SIGNAL(clicked()),this,SLOT(to_data_upload()));
    connect(ui->pushButton_data_download, SIGNAL(clicked()),this,SLOT(to_data_download()));
    connect(ui->pushButton_data_downupload, SIGNAL(clicked()),this,SLOT(to_data_updownload()));
    connect(ui->pushButton_flow_upload, SIGNAL(clicked()),this,SLOT(to_flow_upload()));
    connect(ui->pushButton_timecheck, SIGNAL(clicked()),this,SLOT(to_timecheck()));
    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(to_exit()));
    connect(ui->pushButton_saas_type, SIGNAL(clicked()),this,SLOT(to_saas_update()));
    connect(ui->pushButton_host, SIGNAL(clicked()),this,SLOT(tochangeHost()));
}

w_sys_manage_cloudsync_cloud2::~w_sys_manage_cloudsync_cloud2()
{
    delete ui;
}

void w_sys_manage_cloudsync_cloud2::exec_keepdata_byconf(QWidget *parent)
{
    if(w_sys_manage_cloudsync_with_time_running::q_bt_get_ready_total_byconf()) {
        data_updown(parent);
    }
}

void w_sys_manage_cloudsync_cloud2::data_updown(QWidget *parent)
{
    QString errstring;
    lds_messagebox_loading_show loading(parent, QObject::tr("上传商品"));
    loading.show_delay();
    if(w_sys_manage_cloudsync_with::data_upload(&loading, false, errstring)) {
    } else {
        lds_messagebox::warning(parent,MESSAGE_TITLE_VOID, errstring);
        return;
    }

    loading.setTile(QObject::tr("下载商品"));
    if(w_sys_manage_cloudsync_with::data_download(&loading, false, errstring)) {
    } else {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    lds_messagebox::information(parent, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
}

void w_sys_manage_cloudsync_cloud2::to_data_upload()
{
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确定上传"), QObject::tr("确定"), QObject::tr("取消"))) {
        return;
    }
    QString errstring;
    lds_messagebox_loading_show loading(this, QObject::tr("上传商品"));
    loading.show_delay();
    if(w_sys_manage_cloudsync_with::data_upload(&loading, true, errstring)) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
    } else {
        lds_messagebox::warning(this,MESSAGE_TITLE_VOID, errstring);
    }
}

void w_sys_manage_cloudsync_cloud2::to_data_download()
{
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("本操作将会清空本地所有基础资料"), QObject::tr("确定"), QObject::tr("取消"))) {
        return;
    }
    QString errstring;
    lds_messagebox_loading_show loading(this, QObject::tr("下载商品"));
    loading.show_delay();
    if(w_sys_manage_cloudsync_with::data_download(&loading, true, errstring)) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
    } else {
        lds_messagebox::warning(this,MESSAGE_TITLE_VOID, errstring);
    }
}

void w_sys_manage_cloudsync_cloud2::to_data_updownload()
{
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("数据同步"), QObject::tr("确定"), QObject::tr("取消"))) {
        return;
    }
    data_updown(this);
}

void w_sys_manage_cloudsync_cloud2::to_flow_upload()
{
    switch(lds_messagebox::question(this,MESSAGE_TITLE_VOID, QObject::tr("流水上传"), QObject::tr("全部流水上传"), QObject::tr("未上传流水上传"), QObject::tr("取消"))) {
    case 0:
    {
        if(1 == lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("全部流水将置为未上传状态，流水越多花时间越多"), QObject::tr("继续"), QObject::tr("取消"))) {
            return;
        }
        lds_messagebox_loading_show loading(this, QObject::tr("上传流水"));
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

void w_sys_manage_cloudsync_cloud2::to_timecheck()
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

void w_sys_manage_cloudsync_cloud2::to_exit()
{
    this->reject();
}

void w_sys_manage_cloudsync_cloud2::to_saas_update()
{
    if(saas_pay_detail::isSaaSFA20()) {
        //saas_guide_pay
        saas_guide2 d(new saas_guide_pay_update, this);
        if(QDialog::Rejected == transparentCenterDialog(&d, false).exec()) {
            return ;
        }
        ///从云端读取终端权限、确认升级成功
        httpDataAutoDel httpd = saas_guide::getTerminal();
        if(200 == httpd.httpStatus
                && "21" == saas_pay_detail::saas_type())
        {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            return ;
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
        return;
    }

    if(saas_pay_detail::isSaaSFA21()
            || saas_pay_detail::isSaaSAA21()) {
        //saas_guide_pay
        saas_guide2 d(new saas_guide_pay, this);
        if(QDialog::Rejected == transparentCenterDialog(&d, false).exec()) {
            return ;
        }
        ///从云端读取终端权限、确认升级成功
        httpDataAutoDel httpd = saas_guide::getTerminal();
        if(200 == httpd.httpStatus
                && "21" == saas_pay_detail::saas_type())
        {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            return ;
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
        return;
    }

    if(saas_pay_detail::isSaaSAA()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, ui->pushButton_saas_type->text(),  QObject::tr("升级专业版"), QObject::tr("SaaS验证"), QObject::tr("取消"))) {
        case 0:
            AA_saas_update();
            break;
        case 1:
            AA_verify();
            break;
        case 2:
            return;
        }
    }
}

void w_sys_manage_cloudsync_cloud2::tochangeHost()
{
    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(QObject::tr("密码") + ":");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted != transparentCenterDialog(&inputdialog).exec()) {
        return;
    }
    if(inputdialog.ui->lineEdit->text() != n_func::f_get_godpassword()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("密码错误"));
        return;
    }

    lds_dialog_input dialog(this);
    dialog.ui->label->setText("");
    dialog.ui->lineEdit->setText(n_func::f_get_sysparm("cloud_host", "www.yqspos.com"));
    dialog.hideKey();
    dialog.setWindowTitle(QObject::tr("网址"));
    forever {
        if(QDialog::Accepted != transparentCenterDialog(&dialog).exec()) {
            return;
        }
        if(dialog.ui->lineEdit->text().trimmed().endsWith("/")) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("不能以/结尾"));
            continue;
        }
        if(dialog.ui->lineEdit->text().trimmed().endsWith("\\")) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("不能以\\结尾"));
            continue;
        }
        break;
    }
    n_func::f_set_sysparm("cloud_host", dialog.ui->lineEdit->text().trimmed());

    ui->pushButton_host->setTitleKey(QObject::tr("网址"), n_func::f_get_sysparm("cloud_host", "www.yqspos.com"));
}

void w_sys_manage_cloudsync_cloud2::AA_verify()
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

void w_sys_manage_cloudsync_cloud2::AA_saas_update()
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
