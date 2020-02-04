#include "fastfd_update.h"
#include "ui_fastfd_update.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QTimer>
#include <QProcess>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QtDebug>
#include <QCloseEvent>

 lds_const_var<QString> fastfd_update::gs_soft_version;
 lds_const_var<QString> fastfd_update::cy_path;
 lds_const_var<QString> fastfd_update::unzip_str;
 lds_const_var<QString> fastfd_update::unzip_password;
 lds_const_var<QString> fastfd_update::app_name;

void fastfd_update::setSoftVersiontCyPath(const QString &gs_soft_version)
{
    fastfd_update::gs_soft_version.setOther(gs_soft_version);
    fastfd_update::app_name.setOther("SoftPos");

    if(gs_soft_version.startsWith("V")) {//普通版本

        QString sqlVersion = gs_soft_version.left(4);//V4.0

        fastfd_update::cy_path.setOther(sqlVersion.compare("V4.0") >=0 ? "cyNew" : "cy");

        fastfd_update::unzip_str.setOther(sqlVersion.compare("V4.0") >=0 ? "unzip" : "punzip");

        fastfd_update::unzip_password.setOther(sqlVersion.compare("V4.0") >=0 ? "linpos.scpos.com" :  "www.scpos.com");
    }

    if(gs_soft_version.startsWith("SC")) {//商超
        fastfd_update::cy_path.setOther("scpos");

        fastfd_update::unzip_str.setOther("unzip") ;

        fastfd_update::unzip_password.setOther("linpos.scpos.com");
    }
    if(gs_soft_version.startsWith("SZ")) {//苏州
        fastfd_update::cy_path.setOther("suZhou");

        fastfd_update::unzip_str.setOther("unzip");

        fastfd_update::unzip_password.setOther("linpos.scpos.com");
    }
}

void fastfd_update::setSoftVersiontCyPath(const QString &gs_soft_version, const QString &ftp_path, const QString &zip_name, const QString &zip_password, const QString &app_name)
{
    fastfd_update::gs_soft_version.setOther(gs_soft_version);
    fastfd_update::cy_path.setOther(ftp_path);
    fastfd_update::unzip_str.setOther(zip_name);
    fastfd_update::unzip_password.setOther(zip_password);
    fastfd_update::app_name.setOther(app_name);

    qDebug() << fastfd_update::gs_soft_version;
    qDebug() << fastfd_update::cy_path;
    qDebug() << fastfd_update::unzip_str;
    qDebug() << fastfd_update::unzip_password;
    qDebug() << fastfd_update::app_name;
}

fastfd_update::fastfd_update(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fastfd_update)
{
    ui->setupUi(this);
    checkstate = false;
    isdownload = false;
    ui->progressBar->setFormat("%p%(%v/%m)");
    ui->progressBar->setValue(0);
    ui->lineEdit_cur_version->setEnabled(false);
    ui->lineEdit_newest_version->setEnabled(false);
    ui->lineEdit_cur_version->setText(fastfd_update::gs_soft_version);

    connect(ui->pushButton_update,SIGNAL(clicked()),this,SLOT(todownload()));
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(tocancel()));
}

fastfd_update::~fastfd_update()
{
    delete ui;
}

bool fastfd_update::check_need_update(QString *errstring)
{
    if(!tocheckversion(errstring)) {
        return false;
    }
    if(ui->lineEdit_cur_version->text() >= ui->lineEdit_newest_version->text()) {
        if(errstring) *errstring = QObject::tr("已经是最新版本");
        return false;
    }
    return true;
}

//0:error、1:可以升级、2:最新版
int fastfd_update::check_state_update(QString *errstring)
{
    if(!tocheckversion(errstring)) {
        return 0;
    }
    qDebug() << "cur" << ui->lineEdit_cur_version->text() <<  "latest" << ui->lineEdit_newest_version->text();

    if(ui->lineEdit_cur_version->text() >= ui->lineEdit_newest_version->text()) {
        if(errstring) *errstring = QObject::tr("已经是最新版本");
        return 2;
    }
    //可以升级
    return 1;
}

void fastfd_update::tocheck_down()
{
    if(!fastfd_update::gs_soft_version.isEmpty()) {
        tocheckversion();
        todownload();
    }
}

void fastfd_update::closeEvent(QCloseEvent *e)
{
#ifdef QT_DEBUG
    QDialog::closeEvent(e);
    return;
#endif
    if(isdownload) e->ignore();
    else
        QDialog::closeEvent(e);
}

void fastfd_update::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
}

void fastfd_update::toprogress(qint64 bytesReceived, qint64 bytesTotal)
{
    bytesReceived = bytesReceived;
    bytesTotal = bytesTotal;
    if(ui->progressBar->maximum() != bytesTotal)
        ui->progressBar->setMaximum(bytesTotal);
    ui->progressBar->setValue(bytesReceived);

    //更新下载状态
    if(!isdownload)
        isdownload = true;
}

bool fastfd_update::tocheckversion(QString *errstring)
{
    ui->progressBar->setValue(0);
    QString version;
    if(getNewVersion(version, &isdownload, errstring)) {
        ui->lineEdit_newest_version->setText(version.trimmed());
        return true;
    }
    return false;
}

void fastfd_update::todownload()
{
    QString errstring;
    QFile file;
    QByteArray      appArray;
    QProcess p;
    QString newupdatefilename;
    QString newupdatefilepath;
    QString targetpath;
    QString file_suffix;
#if defined(QT_DEBUG) || defined(Q_OS_WIN)
    targetpath = qApp->applicationDirPath();
#else
    targetpath = "/usr/desk";
#endif
    pushbutton_auto_enable d1(ui->pushButton_cancel);
    pushbutton_auto_enable d2(ui->pushButton_update);


    if(ui->lineEdit_cur_version->text().trimmed().isEmpty()) {
        errstring = QObject::tr("获取本地版本号失败");
        goto end;
    }

    if(ui->lineEdit_cur_version->text() >= ui->lineEdit_newest_version->text()) {
        errstring = QObject::tr("已经是最新版本");
        goto end;
    }

    //获取最大版本号
    if(ui->lineEdit_newest_version->text().isEmpty()) {
        errstring = ui->label_newest_version->text() + "ISEMPTY";
        goto end;
    }
    //开始下载

#if defined(Q_OS_WIN)
    file_suffix = ".exe";
#else
    file_suffix = ".zip";
#endif

#if defined(QT_DEBUG) || defined(Q_OS_WIN)
    newupdatefilename = ui->lineEdit_newest_version->text() + file_suffix;
    newupdatefilepath = qApp->applicationDirPath() + "/" + ui->lineEdit_newest_version->text() + file_suffix;
#else
    newupdatefilename = ui->lineEdit_newest_version->text() + file_suffix;
    newupdatefilepath = "/userdata/" + ui->lineEdit_newest_version->text() + file_suffix;
#endif

    {
        QUrl url;
        url.setUrl("ftp://115.28.155.165/" +fastfd_update::cy_path + "/" + newupdatefilename);
        url.setUserName("cz");
        url.setPassword("111111");
        QNetworkRequest request;
        request.setUrl(url);

        QNetworkAccessManager networkAccessManager;
        /*计时*/fastfd_update_downstate overtimecheck(&isdownload);
        overtimecheck.startTimer(5000);
        connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), &overtimecheck.loop, SLOT(quit()));
        QNetworkReply *reply = networkAccessManager.get(request);
        connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(toprogress(qint64,qint64)));
        overtimecheck.loop.exec(QEventLoop::ExcludeUserInputEvents);
        /*超时*/if(overtimecheck.isOverTime()) { reply->abort();  errstring = QObject::tr("网络超时"); goto end; }

        if(reply->error() != QNetworkReply::NoError) {
            errstring = "net work error";
            qDebug() << reply->errorString();
            goto end;
        }
        appArray = reply->readAll();
    }
    //解压完成
    file.setFileName(newupdatefilepath);
    if(!file.open(QFile::WriteOnly)) {
        errstring = file.errorString();
        goto end;
    }
    file.write(appArray);
    file.close();

#if defined(Q_OS_WIN)
    if(!QFile::remove(app_name + ".exe")) {
        errstring = file.errorString();
        goto end;
    }
    if(!file.rename(app_name + ".exe")) {
        errstring = file.errorString();
        goto end;
    }
#endif

#ifdef Q_OS_UNIX
    //解压文件 直接重写，密码解压
    //unzip -o -P linpos.scpos.com %3 -d %4
    p.start(QString("%1 -o -P %2 %3 -d %4")
            .arg(fastfd_update::unzip_str)
            .arg(fastfd_update::unzip_password)
            .arg(newupdatefilepath)
            .arg(targetpath));
    if(!p.waitForFinished(-1)) {
        errstring = QObject::tr("操作超时") + "\n" + p.errorString();
        goto end;
    }
    //删除源文件
    if(!file.remove()) {
        errstring = file.errorString();
        goto end;
    }
    system("chmod +x /usr/desk/install.sh");
    system("/usr/desk/install.sh");

    system("chmod +x /usr/desk/pos/" + app_name.toLocal8Bit());
    system("chmod +x /usr/desk/pos/fastfd_updated");
#endif
    //升级成功
    QMessageBox::information(this, QObject::tr("提示"), QObject::tr("升级成功"));
#if defined(QT_DEBUG) || defined(Q_OS_WIN)
    QProcess::startDetached(qApp->applicationDirPath()+"/" + app_name, QStringList(), qApp->applicationDirPath());
#else
    QProcess::startDetached("/usr/desk/pos/" + app_name, QStringList(), "/usr/desk");
#endif
    this->reject();
    return;
end:
    QMessageBox::warning(this, QObject::tr("提示"), errstring);
    return;
}

void fastfd_update::tocancel()
{
    this->reject();
}

bool fastfd_update::getNewVersion(QString &version, bool *isdownload, QString *errstring)
{
    QUrl url;
    url.setUrl("ftp://115.28.155.165/" +fastfd_update::cy_path + "/version.txt");
    url.setUserName("cz_r");
    url.setPassword("111111");
    QNetworkRequest request;
    request.setUrl(url);
    QNetworkAccessManager networkAccessManager;
    /*计时*/fastfd_update_downstate overtimecheck(isdownload);
    overtimecheck.startTimer(5000);
    connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), &overtimecheck.loop, SLOT(quit()));
    QNetworkReply *reply = networkAccessManager.get(request);
    overtimecheck.loop.exec(QEventLoop::ExcludeUserInputEvents);
    /*超时*/if(overtimecheck.isOverTime()) { reply->abort();  if(errstring) *errstring = QObject::tr("网络超时"); goto end; }

    if(reply->error() != QNetworkReply::NoError) {
        if(errstring) *errstring = reply->errorString();
        goto end;
    }
    version  = reply->readAll();
    version  = version.trimmed();
    return true;
end:
    return false;
}

fastfd_update_downstate::fastfd_update_downstate(bool *_isDW)
    : isDW(_isDW)
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(toCheck()));

    *isDW = false;
    isovertime = false;
}

fastfd_update_downstate::~fastfd_update_downstate()
{
    *isDW = false;
}

void fastfd_update_downstate::startTimer(int msec)
{
    timer->setSingleShot(true);
    timer->start(msec);
}

bool fastfd_update_downstate::isOverTime()
{
    return isovertime;
}

void fastfd_update_downstate::toCheck()
{
    if(*isDW) {

    } else {
        loop.quit();
        isovertime = true;
    }
}
