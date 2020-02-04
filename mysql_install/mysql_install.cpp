#include "mysql_install.h"
#include "JlCompress.h"
#include "quazip.h"
#include <QtDebug>
#include <QProcess>
#include <QSettings>
#include <QDir>
#include "windows.h"
#include <QApplication>

/*
卸载MySQL数据库：管理员打开cmd，切换到mysql 的安装路径/bin里，mysqld -remove
因为MySQL的密码，数据库内容会保存在原路径下，假如在其他盘符安装MySQL时，删除原MySQL的源文件
问安装的MySQL密码为：123456
*/
QString trans_CODEC2UTF8(const QByteArray &in, const QByteArray &codec)
{
    QTextCodec *c = QTextCodec::codecForName(codec);
    return c->toUnicode(in);
}

QByteArray trans_u2code(const QString &in, const QByteArray &codec)
{
    QTextCodec *c = QTextCodec::codecForName(codec);
    QByteArray r = c->fromUnicode(in);
    return r;
}

QString trans_GBK2UFT8(const QByteArray &in)
{
    return trans_CODEC2UTF8(in, "GBK");
}

QByteArray trans_UTF82GBK(const QString &in)
{
    QTextCodec *c = QTextCodec::codecForName("GBK");
    return c->fromUnicode(in);
}

mysql_install_obj::mysql_install_obj(QObject *parent)
    : QObject(parent)
{
    this->messageBar = 0;
    getWindowVersion(sysname, sysbit);

    QString root = qApp->applicationDirPath();
    while(!root.endsWith(":"))
        root.chop(1);
    QString mysql_filename;
    if(sysbit == 64) {
        mysql_filename = "mysql-5.7.18-winx64";
    }
    if(sysbit == 32) {
        mysql_filename = "mysql-5.7.18-win32";
    }
    //
    ProgramFiles_path = root + "/Program Files";
    MySQL_path = ProgramFiles_path + "/" + mysql_filename;
    MySQL_zip_filepath = qApp->applicationDirPath() + "/quazip/" + mysql_filename + ".zip";
    MySQL_rd_bat = MySQL_path + "/rd.bat";
}

mysql_install_obj::MySQL_STATE mysql_install_obj::MySQLCheck()
{
    QByteArray r;
    QString errstring;
    if(!cmd("sc query", QStringList(), errstring, &r)) {
        qDebug() << errstring;
        return MySQL_STATE_ERROR;
    }
    QMap<QString, mysql_install_obj::MySQL_STATE> serviceState;
    QBuffer buff(&r);
    buff.open(QIODevice::ReadOnly);
    while(!buff.atEnd()) {
        QString line = buff.readLine();
        QString line_start = "DISPLAY_NAME: ";
        if(line.startsWith(line_start)
                && line.mid(line_start.length()).startsWith("mysql", Qt::CaseInsensitive)) {
            serviceState.insert(line.mid(line_start.length()).trimmed(), MySQLCheck(buff));
        }
    }
    if(serviceState.count() > 0) {
        return serviceState.values().first();
    }
    for(QMap<QString, mysql_install_obj::MySQL_STATE>::iterator k = serviceState.begin(); k != serviceState.end(); k ++) {
        qDebug() << "MySQL Server List:" << k.key() << k.value();
    }
    return MySQL_STATE_UNEXISTED;
}

mysql_install_obj::MySQL_STATE mysql_install_obj::MySQLCheck(QBuffer &buff)
{
    while(!buff.atEnd()) {
        QByteArray line = buff.readLine();
        if(line.contains("STATE")) {
            if(line.trimmed().endsWith("RUNNING")) {
                return MySQL_STATE_RUNNING;
            }
            return MySQL_STATE_STOPPED;
        }
    }
    return MySQL_STATE_UNEXISTED;
}

bool mysql_install_obj::unzip(QString fileCompressed, QString dir)
{
    showMessage("unzip:" + fileCompressed);
    return JlCompress::extractDir(fileCompressed, dir).count() != 0;
}

void mysql_install_obj::getWindowVersion(QString &sysname, int &sysbit)
{
    typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process;
    BOOL bIsWow64 = FALSE;
    fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress( GetModuleHandle(TEXT("kernel32")),"IsWow64Process");
    if (NULL != fnIsWow64Process)
    {
        fnIsWow64Process(GetCurrentProcess(),&bIsWow64);
    }
    if(bIsWow64)
        sysbit = 64;// = "64位";
    else
        sysbit = 32;//"32位";

    sysname = QSysInfo::prettyProductName();
}

void mysql_install_obj::setMessageBar(QObject *messageBar, const QByteArray &messageBarFunc)
{
    this->messageBar = messageBar;
    this->messageBarFunc = messageBarFunc;
}

bool mysql_install_obj::install(QString &info)
{
    bool f = MySQLInstall(info);
    if(!f)
        return false;
    return MySQLCheck() == MySQL_STATE_RUNNING;
}

bool mysql_install_obj::uninstall(QString &info)
{
    if(!QDir(MySQL_path).exists()) {
        info = MySQL_path + "is not existed \n when to unintall other mysql server maybe have that error";
        return false;
    }
    if(!QFile::exists(MySQL_rd_bat)) {
        QFile file(MySQL_rd_bat);
        file.open(QFile::WriteOnly);
        file.write("rd/s/q \"" + MySQL_path.toLocal8Bit() + "\"");
        file.close();
    }
    if(!cmd("net stop mysql", QStringList(), info)) {
        return false;
    }
    if(!cmd(MySQL_path + "/bin/mysqld", QStringList() << "-remove", info)) {
        return false;
    }
    if(!cmd("cmd", QStringList() << "/c" << MySQL_rd_bat, info)) {
        return false;
    }
    return true;
}

bool mysql_install_obj::cmd(QString program, const QStringList &args, QString &errstring, QByteArray *readall)
{
    showMessage(program + " " + args.join(" "));
    QProcess p;
    QByteArray reply;
    if(program.contains("Program Files"))
        program = "\"" + program + "\"";
    if(args.count() == 0)
        p.start(program);
    else
        p.start(program, args);
    if(!p.waitForFinished(-1)) {
        errstring = p.errorString();
        return false;
    }
    reply = p.readAll();
    if(readall)
        *readall = reply;

    qDebug() << reply.data();
    return true;
}

bool mysql_install_obj::MySQLInstall(QString &info)
{
    QString errstring;
    MySQL_STATE state;
    //check
    state = MySQLCheck();
    if(MySQL_STATE_RUNNING == state) {
        info = "mysql is running";
        return true;
    }

    if(MySQL_STATE_STOPPED == state) {
        if(!cmd("net start mysql", QStringList(), errstring)){
            info = "net start mysql failed";
            return false;
        }
        info = "net start mysql successfully";
        return true;
    }
    if(MySQL_STATE_UNEXISTED == state) {
        //unzip
        if(!QDir(MySQL_path).exists()) {
            if(!unzip(MySQL_zip_filepath, ProgramFiles_path)) {
                info = "mysql unzip failed";
                return false;
            }
        }
        if(!QFile::exists(MySQL_path + "/my.ini")) {
            QSettings conf(MySQL_path + "/my.ini", QSettings::IniFormat);
            conf.setValue("mysqld/basedir", MySQL_path);
            conf.setValue("mysqld/datadir", MySQL_path + "/data");
            conf.setValue("mysqld/port", 3306);
            conf.sync();
        }
        //install
        if(!cmd(MySQL_path + "/bin/mysqld", QStringList() << "-install", errstring)) {
            info = "mysql install failed";
            return false;
        }
        if(!cmd(MySQL_path + "/bin/mysqld", QStringList() << "--initialize-insecure", errstring)) {
            info = "mysql init failed";
            return false;
        }
        if(!cmd("net start mysql", QStringList(), errstring)) {
            info = "net start mysql failed";
            return false;
        }
        if(!cmd(MySQL_path + "/bin/mysqladmin", QStringList() << "-u" << "root" << "password" << "123456", errstring)) {
            info = "mysql init password failed";
            return false;
        }
        info = "install successfully";
        return true;
    }

    info = "unknown error";
    return false;
}

void mysql_install_obj::showMessage(const QString &text)
{
    QMetaObject::invokeMethod(messageBar, messageBarFunc, Qt::AutoConnection, Q_ARG(QString,text));
}

