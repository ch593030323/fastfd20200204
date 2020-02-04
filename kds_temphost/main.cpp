#include "mainwindow.h"
#include <QApplication>
#include<QtGui>
#include "lds_query_thread.h"
#include "kds_public.h"
#include "lds.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QString hostname = "192.168.1.188";
    QString password = "888888";
    QSqlDatabase hddpos = QSqlDatabase::addDatabase("QMYSQL", "hddpos");
    hddpos.setDatabaseName("hddpos");
    hddpos.setHostName(hostname);
    hddpos.setUserName("root");
    hddpos.setPassword(password);
    qDebug() << hddpos.open();


    QSqlDatabase hddpos_kds = lds_query_thread::getThreadDbAndCreat();//QSqlDatabase::addDatabase("QMYSQL");
    hddpos_kds.setHostName(hostname);
    hddpos_kds.setUserName("root");
    hddpos_kds.setPassword(password);
    qDebug() << hddpos_kds.open();
    if(QSqlError::NoError != hddpos_kds.exec("use hddpos_kds").lastError().type()) {
        hddpos_kds.exec("create database hddpos_kds");
    }
    hddpos_kds.exec("use hddpos");

    QSqlQuery query = hddpos.exec("show tables");
    while(query.next()) {
        hddpos_kds.exec(QString("create table %1 like hddpos.%1").arg(query.record().value(0).toString()));
    }
    //
    QSettings conf(lds::localdataPath +"/fastfd.ini", QSettings::IniFormat);
    kds_public::setSysTextCodec();
    lds::sysconf = &conf;
    lds::LDS_INIT(lds::func_dec_price_def, lds::func_dec_num_def, lds::func_dec_amt_def);
    app.setStyleSheet(kds_public::stylesheetData());
    MainWindow w;
    w.show();
    return app.exec();
}
