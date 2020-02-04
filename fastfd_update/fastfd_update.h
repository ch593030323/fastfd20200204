#ifndef FASTFD_UPDATE_H
#define FASTFD_UPDATE_H

#include <QDialog>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QTimer>
#include "lds_const_var.h"

namespace Ui {
class fastfd_update;
}

class fastfd_update : public QDialog
{

    Q_OBJECT
public:
    class pushbutton_auto_enable {
    public:
        pushbutton_auto_enable(QPushButton *p) : _p(p) {_p->setEnabled(false);}
        ~pushbutton_auto_enable() {_p->setEnabled(true);}
        QPushButton *_p;
    };
    //在线升级程序 -ve[软件版本] -pa[ftp 相对路径] -zn[解压程序] -zp[解压密码]
    static void setSoftVersiontCyPath(const QString &gs_soft_version);
    static void setSoftVersiontCyPath(const QString &gs_soft_version, const QString &ftp_path, const QString &zip_name, const QString &zip_password, const QString &app_name);

    static lds_const_var<QString> gs_soft_version;
    static lds_const_var<QString> cy_path;
    static lds_const_var<QString> unzip_str;
    static lds_const_var<QString> unzip_password;
    static lds_const_var<QString> app_name;
public:
    explicit fastfd_update(QWidget *parent = 0);
    ~fastfd_update();
    bool check_need_update(QString *errstring = 0);
    int check_state_update(QString *errstring = 0);


    static bool getNewVersion(QString &version, bool *isdownload, QString *errstring);
public slots:
    void tocheck_down();
protected:
    void closeEvent(QCloseEvent *e);
    void showEvent(QShowEvent *e);
private slots:
    void toprogress(qint64 bytesReceived, qint64 bytesTotal);

    void todownload();
    void tocancel();

private:
    bool tocheckversion(QString *errstring = 0);

private:
    Ui::fastfd_update *ui;
    QByteArray md5;
    bool checkstate;
    bool isdownload;
};

class fastfd_update_downstate : public QObject{
    Q_OBJECT
public:
    fastfd_update_downstate(bool *_isDW) ;
    ~fastfd_update_downstate();
    void startTimer(int msec);
    bool isOverTime();
private slots:
    void toCheck();
public:
    QEventLoop loop;
    QTimer *timer;
private:
    bool *isDW;
    bool isovertime;
};

#endif // FASTFD_UPDATE_H
