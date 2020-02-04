#include "w_sys_manage_sqlcopy_soft_limit2_for_language.h"
#include "ui_w_sys_manage_sqlcopy_soft_limit2_for_language.h"
#include "lds.h"
#include <QClipboard>
#include "lds_messagebox.h"
#include "public_sql.h"
#include "getfilepath.h"

const QString w_sys_manage_sqlcopy_soft_limit2_for_language::group_name = "strange_language";

bool w_sys_manage_sqlcopy_soft_limit2_for_language::isValid()
{
    if(false == public_sql::fastfd_limit_conf->value(group_name + "/" + lds::terminalCode).isValid()) {
        public_sql::fastfd_limit_conf->setValue(group_name + "/" + lds::terminalCode, "0");
    }
    return public_sql::fastfd_limit_conf->value(group_name + "/" + lds::terminalCode, "0").toString() == "1";
}

QDate w_sys_manage_sqlcopy_soft_limit2_for_language::deadDate()
{
    if(false == public_sql::fastfd_limit_conf->value(group_name + "/" + lds::terminalCode + "_remain").isValid()) {
        public_sql::fastfd_limit_conf->setValue(group_name + "/" + lds::terminalCode + "_remain", QDate::currentDate().addMonths(1));
    }
    return public_sql::fastfd_limit_conf->value(group_name + "/" + lds::terminalCode + "_remain").toDate();
}

QStringList w_sys_manage_sqlcopy_soft_limit2_for_language::freeLanuageList()
{
    return QStringList() << "zh" << "en";
}

bool w_sys_manage_sqlcopy_soft_limit2_for_language::currentLanuageIsFree()
{
    return currentLanuageIsFree(lds::get_soft_curent_language_first(), lds::get_soft_curent_language_second());
}

bool w_sys_manage_sqlcopy_soft_limit2_for_language::currentLanuageIsFree(const QString &first, const QString &second)
{
    QStringList free_language = w_sys_manage_sqlcopy_soft_limit2_for_language::freeLanuageList();
    return free_language.contains(first)
            && free_language.contains(second);
}

bool w_sys_manage_sqlcopy_soft_limit2_for_language::checkLimit(const QString &first, const QString &second, QWidget *parent)
{
    if(false == isValid()) {
        if(false == currentLanuageIsFree(first, second)) {
            //激活小语种
            if(false == public_sql::fastfd_limit_conf->value(group_name + "/" + lds::terminalCode + "_remain").isValid()) {
                switch(lds_messagebox::information(parent, MESSAGE_TITLE_VOID, QObject::tr("中英文外的语言需要输入注册码激活"),
                                                   QObject::tr("注册"), QObject::tr("继续使用"), QObject::tr("取消"))) {
                case 0:{
                    w_sys_manage_sqlcopy_soft_limit2_for_language dialog(parent);
                    if(QDialog::Rejected == transparentCenterDialog(&dialog).exec()) {
                        return false;
                    }
                } break;
                case 1:{//试用
                    QDate dead = deadDate();
                    lds_messagebox::information(parent, MESSAGE_TITLE_VOID, QObject::tr("截止日期") + ":" + dead.toString(yyyyMMdd));
                }break;
                case 2://取消
                    return false;
                }
            } else {
                //试用阶段的
                QDate cur = QDate::currentDate();
                QDate dead = deadDate();
                int dead_off = cur.daysTo(dead);
                if(dead_off >= 0) {
                    while(0 == lds_messagebox::information(parent, MESSAGE_TITLE_VOID, QObject::tr("中英文外的语言需要输入注册码激活") + "\n" +
                                                           QObject::tr("当前日期") + ":" + cur.toString(yyyyMMdd) + "\n" +
                                                           QObject::tr("截止日期") + ":" + dead.toString(yyyyMMdd) + "\n" +
                                                           QObject::tr("剩余") + ":" + QString::number(dead_off) + QObject::tr("天"),
                                                           QObject::tr("注册"), QObject::tr("继续使用"))) {
                        w_sys_manage_sqlcopy_soft_limit2_for_language dialog(parent);
                        if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
                            break;
                        }
                    }
                } else {
                    //试用过期
                    lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("中英文外的语言需要输入注册码激活") + "\n" +
                                            QObject::tr("当前日期") + ":" + cur.toString(yyyyMMdd) + "\n" +
                                            QObject::tr("截止日期") + ":" + dead.toString(yyyyMMdd) + "\n" +
                                            QObject::tr("剩余") + ":" + QObject::tr("不足"),
                                            QObject::tr("注册"));
                    w_sys_manage_sqlcopy_soft_limit2_for_language dialog(parent);
                    if(QDialog::Rejected == transparentCenterDialog(&dialog).exec()) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

w_sys_manage_sqlcopy_soft_limit2_for_language::w_sys_manage_sqlcopy_soft_limit2_for_language(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_sqlcopy_soft_limit2_for_language)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("激活"));

    ui->lineEdit_hardware_id->setText(lds::terminalCode);
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->toolButton_copy, SIGNAL(clicked()), this, SLOT(toTerminalCodeCopy()));
    connect(ui->toolButton_paste, SIGNAL(clicked()), this, SLOT(topaste()));
    connect(ui->pushButton_read, SIGNAL(clicked()), this, SLOT(toread()));

#ifdef QT_DEBUG
    QString code = group_name +"/" + lds::terminalCode + "=1";
    QString en = lds::of_str_encrypt(code);
#endif
}

w_sys_manage_sqlcopy_soft_limit2_for_language::~w_sys_manage_sqlcopy_soft_limit2_for_language()
{
    delete ui;
}

void w_sys_manage_sqlcopy_soft_limit2_for_language::took()
{
    //strange_language:A2825645874
    QString code = ui->lineEdit_auth_code->text().trimmed();
    code = QByteArray::fromHex(code.toLocal8Bit());
    code = lds::of_str_decrypt(code);
    qDebug() << code;
    qDebug() << group_name;
    if(!code.startsWith(group_name)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, "group_name is unmatched");
        return;
    }

    qDebug() << group_name + "/" + lds::terminalCode;
    if(!code.startsWith(group_name + "/" + lds::terminalCode)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, "TerminalCode is unmatched");
        return;
    }
    QString value = code.mid(group_name.length() + 1 + lds::terminalCode.length() + 1, 1);
    value = "1";
    //    if(value != "1") {
    //        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, "Value is exception:" + value);
    //        return;
    //    }

    public_sql::fastfd_limit_conf->setValue(group_name + "/" + lds::terminalCode, value);
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
    this->accept();
}

void w_sys_manage_sqlcopy_soft_limit2_for_language::tocancel()
{
    this->reject();
}

void w_sys_manage_sqlcopy_soft_limit2_for_language::toTerminalCodeCopy()
{
    qApp->clipboard()->setText(lds::terminalCode);
}

void w_sys_manage_sqlcopy_soft_limit2_for_language::topaste()
{
    ui->lineEdit_auth_code->setText(qApp->clipboard()->text());
}

void w_sys_manage_sqlcopy_soft_limit2_for_language::toread()
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
