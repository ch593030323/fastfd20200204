#ifndef W_SYS_MANAGE_SENDEMAIL_H
#define W_SYS_MANAGE_SENDEMAIL_H

#include <QDialog>
#include <QMap>

class SmtpClient;
class lds_messagebox_loading_show;

namespace Ui {
class w_sys_manage_sendEmail;
}

class w_sys_manage_sendEmail_obj : public QObject {

    Q_OBJECT
public:
    static const QMap<QString ,QString> getServerList();
private:
    static QMap<QString, QString> map;
public:
    w_sys_manage_sendEmail_obj(QObject *parent = 0);
    ~w_sys_manage_sendEmail_obj();
    bool open(QString &errstring);
    bool open(
            const QString &sys_email_usr,
            const QString &sys_email_pwd,
            QString &errstring);
    bool sendAttachment(
            const QString &title,
            const QString &filePath,
            QString &errstring);
    bool sendAttachment(
            const QString &sys_email_receiver,
            const QString &title,
            const QString &filePath,
            QString &errstring);
    bool sendText(
            const QString &sys_email_receiver,
            const QString &title,
            const QString &text,
            QString &errstring);
    void close();
private:
    SmtpClient *smtp;
};

class w_sys_manage_sendEmail : public QDialog
{
    Q_OBJECT
public:
    explicit w_sys_manage_sendEmail(QWidget *parent = 0);
    ~w_sys_manage_sendEmail();

private slots:
    void took();
    void tocancel();
    void totest();
    void tocompany();
private:
    QString lastSplit(const QString &str, const QString &split_str);
private:
    Ui::w_sys_manage_sendEmail *ui;
};

#endif // W_SYS_MANAGE_SENDEMAIL_H
