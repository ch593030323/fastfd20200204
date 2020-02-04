#include "w_sys_manage_sendemail.h"
#include "ui_w_sys_manage_sendemail.h"
#include "n_func.h"
#include "smtpmime.h"
#include "lds_messagebox.h"
#include "w_sys_manage_companyinformation.h"
#include "transparentcenterdialog.h"

QMap<QString, QString> w_sys_manage_sendEmail_obj::map;

const QMap<QString, QString> w_sys_manage_sendEmail_obj::getServerList()
{
    if(map.count() == 0) {
        map.insert("@qq.com", "smtp.qq.com,465,SSL");
        map.insert("@sina.com", "smtp.sina.com,25,TCP");
        map.insert("@sina.cn", "smtp.sina.cn,25,TCP");
        map.insert("@gmail.com", "smtp.gmail.com,465,SSL");
        //        map.insert("@hotmail.com", "smtp.hotmail.com,465,SSL");
        //        map.insert("@yahoo.com", "smt.yahoo.com,25,TCP");
    }
    return map;
}

w_sys_manage_sendEmail::w_sys_manage_sendEmail(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_sendEmail)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("邮箱"));
    ui->comboBox->addItems(w_sys_manage_sendEmail_obj::getServerList().keys());

    QString sys_email_usr = n_func::f_get_sysparm("sys_email_usr");
    QString sys_email_pwd = n_func::f_get_sysparm("sys_email_pwd");
    QString sys_email_receiver = n_func::f_get_sysparm("sys_email_receiver");

    ui->lineEdit_pwd->setText(sys_email_pwd);
    ui->lineEdit_usr->setText(sys_email_usr.mid(0, sys_email_usr.indexOf("@")));
    ui->lineEdit_receiver->setText(sys_email_receiver);
    ui->comboBox->setcomtext(sys_email_usr.mid(sys_email_usr.indexOf("@")));
    ui->pushButton_title->setText(lds_query_hddpos::selectValue("select vch_company from cey_sys_company").toString());

    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_test, SIGNAL(clicked()), this, SLOT(totest()));
    connect(ui->pushButton_title, SIGNAL(clicked()), this, SLOT(tocompany()));
}

w_sys_manage_sendEmail::~w_sys_manage_sendEmail()
{
    delete ui;
}

void w_sys_manage_sendEmail::took()
{
    QString sys_email_pwd = ui->lineEdit_pwd->text().trimmed();
    QString sys_email_usr = ui->lineEdit_usr->text().trimmed() + ui->comboBox->currentText();
    QString sys_email_receiver = ui->lineEdit_receiver->text().trimmed();

    n_func::f_set_sysparm("sys_email_pwd", sys_email_pwd);
    n_func::f_set_sysparm("sys_email_usr", sys_email_usr);
    n_func::f_set_sysparm("sys_email_receiver", sys_email_receiver);

    this->accept();
}

void w_sys_manage_sendEmail::tocancel()
{
    this->reject();
}

void w_sys_manage_sendEmail::totest()
{
    QString sys_email_pwd = ui->lineEdit_pwd->text().trimmed();
    QString sys_email_usr = ui->lineEdit_usr->text().trimmed() + ui->comboBox->currentText();
    QString sys_email_receiver = ui->lineEdit_receiver->text().trimmed();

    if(sys_email_pwd == "") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, "sys_email_pwd is null");
        return;
    }
    if(sys_email_usr.indexOf("@") == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, "sys_email_usr is null");
        return;
    }
    if(sys_email_receiver == "") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, "sys_email_receiver is null");
        return;
    }

    lds_messagebox_loading_show loading(this, MESSAGE_TITLE_VOID, ui->pushButton_test->text());
    loading.show_delay();
    QString errstring;
    w_sys_manage_sendEmail_obj obj;
    if(false == obj.open(sys_email_usr, sys_email_pwd, errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    if(false == obj.sendText(sys_email_receiver, ui->pushButton_title->text(), "THIS IS TEST", errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }

    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("发送成功"));
    obj.close();
    return;
}

void w_sys_manage_sendEmail::tocompany()
{
    w_sys_manage_companyinformation dialog(this);
    dialog.setWindowTitle(QObject::tr("公司信息"));
    transparentCenterDialog(&dialog).exec();
    ui->pushButton_title->setText(lds_query_hddpos::selectValue("select vch_company from cey_sys_company").toString());
}

QString w_sys_manage_sendEmail::lastSplit(const QString &str, const QString &split_str)
{
    int k = str.lastIndexOf(split_str);
    return str.mid(k + split_str.length());
}

w_sys_manage_sendEmail_obj::w_sys_manage_sendEmail_obj(QObject *parent) : QObject(parent)
{
    smtp = new SmtpClient;
}

w_sys_manage_sendEmail_obj::~w_sys_manage_sendEmail_obj()
{
    close();
    delete smtp;
}

bool w_sys_manage_sendEmail_obj::open(QString &errstring)
{
    QString sys_email_usr = n_func::f_get_sysparm("sys_email_usr");
    QString sys_email_pwd = n_func::f_get_sysparm("sys_email_pwd");
    return open(sys_email_usr, sys_email_pwd, errstring);
}

bool w_sys_manage_sendEmail_obj::open(const QString &sys_email_usr, const QString &sys_email_pwd, QString &errstring)
{
    QString _a_com = sys_email_usr.mid(sys_email_usr.indexOf("@"));

    QStringList server_info_list = getServerList().value(_a_com).split(",");
    QString url_server = server_info_list.value(0);
    int url_port = server_info_list.value(1).toInt();
    bool url_SSL = server_info_list.value(2) == "SSL";

    //smtp
    smtp->setHost(url_server);
    smtp->setPort(url_port);
    smtp->setConnectionType(url_SSL ? SmtpClient::SslConnection : SmtpClient::TcpConnection);
    smtp->setUser(sys_email_usr);
    smtp->setPassword(sys_email_pwd);
    if(!smtp->connectToHost()) {
        errstring = "connectToHost failed\n" + smtp->getResponseText();
        return false;
    }
    if(!smtp->login()) {
        errstring = "login failed\n" + smtp->getResponseText();
        return false;
    }
    return true;
}

bool w_sys_manage_sendEmail_obj::sendAttachment(const QString &title, const QString &filePath, QString &errstring)
{
    QString sys_email_receiver = n_func::f_get_sysparm("sys_email_receiver");
    return sendAttachment(sys_email_receiver, title, filePath, errstring);
}

bool w_sys_manage_sendEmail_obj::sendAttachment(const QString &sys_email_receiver, const QString &title, const QString &filePath, QString &errstring)
{
    MimeMessage message;
    //sender
    message.setSender(new EmailAddress(smtp->getUser()));
    //receiver
    foreach(const QString &receiver, sys_email_receiver.split(";", QString::SkipEmptyParts)) {
        message.addRecipient(new EmailAddress(receiver));
    }
    //title
    message.setSubject(title);
    //file
    foreach(const QString &file, filePath.split(";", QString::SkipEmptyParts)) {
        message.addPart(new MimeAttachment(new QFile(file, this)));
    }
    if(!smtp->sendMail(message)) {
        errstring = "sendMail failed\n" + smtp->getResponseText();
        return false;
    }
    return true;
}

bool w_sys_manage_sendEmail_obj::sendText(const QString &sys_email_receiver, const QString &title, const QString &text, QString &errstring)
{
    MimeMessage message;
    //sender
    message.setSender(new EmailAddress(smtp->getUser()));
    //receiver
    foreach(const QString &receiver, sys_email_receiver.split(";", QString::SkipEmptyParts)) {
        message.addRecipient(new EmailAddress(receiver));
    }
    //title
    message.setSubject(title);
    //text
    MimeHtml txt;
    txt.setText(text);
    message.addPart(&txt);
    if(!smtp->sendMail(message)) {
        errstring = "sendMail failed\n" + smtp->getResponseText();
        return false;
    }
    return true;
}

void w_sys_manage_sendEmail_obj::close()
{
    smtp->quit();
}
