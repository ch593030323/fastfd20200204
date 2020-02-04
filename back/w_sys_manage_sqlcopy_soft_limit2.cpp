#include "w_sys_manage_sqlcopy_soft_limit2.h"
#include "ui_w_sys_manage_sqlcopy_soft_limit2.h"
#include <QSettings>
#include "public_sql.h"
#include "lds_messagebox.h"
#include "getfilepath.h"
#include <QClipboard>

void w_sys_manage_sqlcopy_soft_limit2::limit_date(QDate &dt_limit, QDate &dt_cur)
{
    dt_limit = public_sql::fastfd_limit_conf->value("machine/"+lds::terminalCode, QDate()).toDate();
    qDebug() << __FILE__ << public_sql::fastfd_limit_conf->fileName();
    dt_cur = QDate::currentDate();
    if(dt_limit.isNull()) {
#if defined(Q_OS_LINUX)
        dt_limit = dt_cur.addYears(5);
#elif defined(Q_OS_WIN)
        dt_limit = dt_cur.addDays(30);
#endif
        public_sql::fastfd_limit_conf->setValue("machine/"+lds::terminalCode, dt_limit);
    }
}

w_sys_manage_sqlcopy_soft_limit2::w_sys_manage_sqlcopy_soft_limit2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_sqlcopy_soft_limit2)
{
    ui->setupUi(this);
    QDate dt_limit,dt_cur;
    limit_date(dt_limit, dt_cur);

    ui->lineEdit_hardware_id->setText(lds::terminalCode);
    ui->lineEdit_time_cur->setText(dt_cur.toString(yyyyMMdd));
    ui->lineEdit_time_limit->setText(dt_limit.toString(yyyyMMdd));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_read, SIGNAL(clicked()), this, SLOT(toread()));
    connect(ui->toolButton_copy, SIGNAL(clicked()), this, SLOT(toTerminalCodeCopy()));
    connect(ui->toolButton_paste, SIGNAL(clicked()), this, SLOT(topaste()));

#ifdef QT_DEBUG
    qDebug() << lds::of_str_encrypt(lds::terminalCode + QDateTime::currentDateTime().addDays(100).toString("yyyyMMdd")).toLocal8Bit().toHex().toUpper();
#endif
}

w_sys_manage_sqlcopy_soft_limit2::~w_sys_manage_sqlcopy_soft_limit2()
{
    delete ui;
}

void w_sys_manage_sqlcopy_soft_limit2::took()
{
    QString code = ui->lineEdit_auth_code->text().trimmed();
    code = QByteArray::fromHex(code.toLocal8Bit());
    code = lds::of_str_decrypt(code);
    QDate new_dt = QDate::fromString(code.right(8), "yyyyMMdd");
    code.chop(8);
#ifdef QT_DEBUG
    code = lds::terminalCode;
#endif
    if(lds::terminalCode != code) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("机器号不匹配"));
        return;
    }
//    if(QString("w_sys_manage_sqlcopy_soft_limit2_for_customer") == this->metaObject()->className()) {

//    } else {
//        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("到期时间") + ":" + new_dt.toString(yyyyMMdd), QObject::tr("确定"), QObject::tr("取消"))) {
//            return;
//        }
//    }
    public_sql::fastfd_limit_conf->setValue("machine/"+lds::terminalCode, new_dt);
    ui->lineEdit_time_limit->setText(new_dt.toString(yyyyMMdd));
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
    this->accept();
}

void w_sys_manage_sqlcopy_soft_limit2::tocancel()
{
    this->reject();
}

void w_sys_manage_sqlcopy_soft_limit2::toTerminalCodeCopy()
{
    qApp->clipboard()->setText(ui->lineEdit_hardware_id->text());
}

void w_sys_manage_sqlcopy_soft_limit2::topaste()
{
    ui->lineEdit_auth_code->setText(qApp->clipboard()->text());
}

void w_sys_manage_sqlcopy_soft_limit2::toread()
{
    bool ok;
    QString errstring;
    QString defpath;
#ifdef QT_DEBUG
    defpath = qApp->applicationDirPath();
#else
    defpath = "/usr/desk";
#endif
    //读取文本
    QString filepath = getFilePath::getOpenFileName(this, QObject::tr("导入文件"), defpath);
    if(filepath.isEmpty()) {
        return;
    }
    QByteArray readall = lds::fileOpenReadAll(filepath, &ok, &errstring);
    if(false == ok) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    //
    ui->lineEdit_auth_code->setText(readall.trimmed());
}


w_sys_manage_sqlcopy_soft_limit2_for_customer::w_sys_manage_sqlcopy_soft_limit2_for_customer(QWidget *parent)
    : w_sys_manage_sqlcopy_soft_limit2(parent)
{
    lds::hideWidget(ui->label);
    lds::hideWidget(ui->lineEdit_time_limit);
    lds::hideWidget(ui->label_3);
    lds::hideWidget(ui->lineEdit_time_cur);
    resize(600, 100);
}
