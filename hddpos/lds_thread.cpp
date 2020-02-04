#include "lds_thread.h"
#include <QtDebug>
#include <QEventLoop>
#include <QTimer>
#include <QSqlDatabase>
#include <QApplication>
#include <QFile>
#include <QSqlQuery>
#include "lds_query_thread.h"

lds_thread::lds_thread(const QString &title, QObject *parent) :
    QThread(parent)
{
    this->title = title;
    busy_state = false;
}

lds_thread::~lds_thread()
{
    this->quit();
    this->wait();
}

bool lds_thread::tryTrigger()
{
    if(isBusy()) {
        qDebug() << title + "------------BUSY";
        return false;
    }
    emit signalTrigger();
    return true;
}

void lds_thread::ToTryTrigger()
{
    tryTrigger();
}

void lds_thread::msecSleep(unsigned long msec)
{
    QThread::msleep(msec);
}

void lds_thread::secSleep(unsigned long sec)
{
    QThread::sleep(sec);
}
int lds_thread::execEvent(lds_thread_event *event)
{
    connect(this,SIGNAL(signalTrigger()),event, SLOT(toExecBusy()));//若是正常打印，则会跳过
    connect(event, SIGNAL(signal_busy(bool)), this,SLOT(toSetBusy(bool)));

    return lds_thread::exec();
}

int lds_thread::exec()
{
    int r = QThread::exec();

    QString dbname = lds_query_thread::getThreadDbname();
    if(QSqlDatabase::connectionNames().contains(dbname)) {
        QSqlDatabase::removeDatabase(dbname);
    }
    return r;
}

bool lds_thread::isBusy()
{
    return busy_state;
}

void lds_thread::toSetBusy(bool flag)
{
    busy_state = flag;
}

void lds_thread_event::toExecBusy()
{
    emit signal_busy(true);
    toExec();
    emit signal_busy(false);
}
