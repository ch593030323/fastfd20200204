#ifndef MYSQL_INSTALL_H
#define MYSQL_INSTALL_H

#include <QObject>
#include <QByteArray>
#include <QBuffer>


namespace Ui {
class mysql_install;
}

class mysql_install_obj : public QObject {
    Q_OBJECT
public:
    enum MySQL_STATE{
        MySQL_STATE_RUNNING,
        MySQL_STATE_STOPPED,
        MySQL_STATE_UNEXISTED,
        MySQL_STATE_ERROR
    };

public:
    mysql_install_obj(QObject *parent = 0);

    MySQL_STATE MySQLCheck();
    MySQL_STATE MySQLCheck(QBuffer &buff);

    bool unzip(QString fileCompressed, QString dir);

    void getWindowVersion(QString &sysname, int &sysbit);

    void setMessageBar(QObject *messageBar, const QByteArray &messageBarFunc);

    bool install(QString &info);

    bool uninstall(QString &info);

public:
    bool cmd(QString program, const QStringList &args, QString &errstring, QByteArray *readall = 0);

    bool MySQLInstall(QString &info);

    void showMessage(const QString &text);

public:
    QString sysname;
    int sysbit;
    QString MySQL_path;
    QString MySQL_zip_filepath;
    QString MySQL_rd_bat;
    QString ProgramFiles_path;
    QObject *messageBar;
    QByteArray messageBarFunc;
};

#endif // MYSQL_INSTALL_H
