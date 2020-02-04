#include "lds_query_thread.h"
#include <QStringList>
#include <QSqlQuery>
#include <QtDebug>

const Qt::HANDLE lds_query_thread::MAIN_THREAD_ID = QThread::currentThreadId();
static int SQL_THREAD_COUNT = 0;

lds_query_thread::lds_query_thread(QObject *parent) :
    QThread(parent)
{
}

QString lds_query_thread::getThreadDbname()
{

    if(MAIN_THREAD_ID == QThread::currentThreadId()) {
        return QSqlDatabase::defaultConnection;
    }
    return QString::number((unsigned long)QThread::currentThreadId());
}

//!约定的主线程的mysql数据库别名为：空
//!约定的子线程的mysql数据库别名为：线程号
//!约定的主线程的其他数据库别名为：非空、非defconnectionname
//!约定的子线程的其他数据库别名为：线程号+"-"+connectionname
//! 本函数值获取mysql数据库的别名
//!若一个项目连接n个的数据库时，则线程最多也是n个数据库别名，命名方式：线程号+数据库别名
//!mysql数据库别名必须为空
QString lds_query_thread::getThreadDbnameAndCreat()
{
    return getThreadDbAndCreat().databaseName();
}

QSqlDatabase lds_query_thread::getThreadDbAndCreat()
{
    //子线程
    //1
    QString dbname = getThreadDbname();
    QSqlDatabase db;
    //3
    if(!QSqlDatabase::connectionNames().contains(dbname)) {
        if(QSqlDatabase::defaultConnection == dbname) {
            db = QSqlDatabase::addDatabase("QMYSQL", dbname);
            qDebug() << __LINE__ << __FUNCTION__ << "addDatabase dbname:" << dbname << ++SQL_THREAD_COUNT;
        } else {
            db = QSqlDatabase::cloneDatabase(QSqlDatabase::database(QSqlDatabase::defaultConnection), dbname);//Does nothing if other is an invalid database
            qDebug() << __LINE__ << __FUNCTION__ << "cloneDatabase dbname:" << dbname << ++SQL_THREAD_COUNT;
        }
    } else {
        db = QSqlDatabase::database(dbname, false);
    }

    return db;
}

QSqlDatabase lds_query_thread::getThreadDbAndOpen()//第一次操作数据库时,是需要false 的
{
    QSqlDatabase db = getThreadDbAndCreat();
    if(false == db.isOpen()) {
        db.open();
        QSqlQuery query(db);
        query.exec(
                    "SET sql_mode = 'STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION';"
                    );
    }
    return db;
}

void lds_query_thread::msecSleep(unsigned long msec)
{
    QThread::msleep(msec);
}

void lds_query_thread::secSleep(unsigned long sec)
{
    QThread::sleep(sec);
}
