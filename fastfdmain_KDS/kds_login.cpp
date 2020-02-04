#include "kds_login.h"
#include "ui_kds_login.h"
#include "kds.h"
#include "lds_messagebox.h"
#include "lds_query_db.h"
#include "lds.h"
#include "lds_lan_terminal.h"
#include <QSqlError>

kds_login::kds_login(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::kds_login)
{
    ui->setupUi(this);
    ui->lineEdit_db_host_name->setText(lds::sysconf->value("kds_login/db_host_name").toString());
    ui->lineEdit_vch_operid->setText(lds::sysconf->value("kds_login/vch_operid").toString());
#ifdef QT_DEBUG
    ui->lineEdit_vch_operid->setText("0000");
#endif
    ui->checkBox_auto_login->setChecked(lds::sysconf->value("kds_login/toauto_login", false).toBool());

    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_login, SIGNAL(clicked()), this, SLOT(tologin()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_12, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_10, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_15, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_7, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_8, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_9, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_clear, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->checkBox_auto_login, SIGNAL(clicked()), this, SLOT(toauto_login()));
}

kds_login::~kds_login()
{
    delete ui;
}

void kds_login::toexit()
{
    emit signalExit();
}

void kds_login::tologin()
{
    login();
}

bool kds_login::login()
{
    QString errstring;
    QString ls_operid = ui->lineEdit_vch_operid->text().trimmed();
    QString db_host_name = ui->lineEdit_db_host_name->text().trimmed();
    QString vch_operid = ui->lineEdit_vch_operid->text().trimmed();
    QSqlDatabase db = QSqlDatabase::database(QSqlDatabase::defaultConnection, false);
    lds::sysconf->setValue("kds_login/db_host_name", db_host_name);
    lds::sysconf->setValue("kds_login/vch_operid", vch_operid);
    //db_open
    if(!db.isOpen()) {
        db.setHostName(db_host_name);
#if defined(QT_DEBUG)
        db.setPassword("111111");
        db.setPassword("888888");
#elif defined(Q_OS_LINUX)
        db.setPassword("888888");
#elif defined(Q_OS_WIN32)
        db.setPassword("123456");
#endif
        db.setUserName("root");
        db.setDatabaseName("hddpos");
        if(!kds::db_open(db)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, db.lastError().text());
            return false;
        }
        if(!lds_lan_terminal::regedit(lds::terminalCode, errstring)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
            return false;
        }
    }

    bool ch_auto_login = kds::cey_bt_kitchen_view_data("ch_auto_login").toString() == "Y";
    if(ch_auto_login != lds::sysconf->value("kds_login/toauto_login", false).toBool()) {
        lds::sysconf->setValue("kds_login/toauto_login", ch_auto_login);
        ui->checkBox_auto_login->setChecked(ch_auto_login);
        return false;
    }

    lds_query_db query;
    //ls_operid
    if(ls_operid.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("用户名为空"));
        return false;
    }
    //
    if(ls_operid == "0000") {
        goto end;
    }
    query.execSelect(QString("select vch_password, ch_flag from cey_sys_operator  where vch_operID = '%1' ")
                     .arg(ls_operid));
    if(!query.next()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作员不存在"));
        return false;
    }
    if(query.recordValue("ch_flag").toString() != "Y") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作员未生效"));
        return false;
    }
end:
    kds::cey_bt_kitchen_view_update("ch_auto_login", ui->checkBox_auto_login->isChecked() ? "Y" : "N");
    kds::cey_bt_kitchen_view_update("vch_operID", vch_operid);
    lds::sysconf->setValue("kds_login/toauto_login", ui->checkBox_auto_login->isChecked());
    emit signalLogin();
    return true;
}

void kds_login::tonum()
{
    QLineEdit *l = qobject_cast<QLineEdit*>(this->focusWidget());
    QPushButton *b = qobject_cast<QPushButton*>(this->sender());
    if(l) {
        if(b->text() == "C") {
            l->clear();
        } else {
            l->insert(b->text());
        }
    }
}

void kds_login::toauto_login()
{
//    lds::sysconf->setValue("kds_login/toauto_login", ui->checkBox_auto_login->isChecked());
}
