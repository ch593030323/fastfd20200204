#include "electroniccountprice_kaifeng.h"
#include "printer_pos_serial.h"
#include <QTimer>
#include <QtDebug>

ElectronicCountPrice_kaifeng::ElectronicCountPrice_kaifeng(QObject *parent) :
    QObject(parent)
{
    printer = new Printer_POS_SERIAL;
    static Printer_POS_SERIAL_Thread_scale t_scale;
    t = &t_scale;
    t->_printer= printer;
    t->isbreak= false;
}

ElectronicCountPrice_kaifeng::~ElectronicCountPrice_kaifeng()
{
    if(t->isRunning()) {
        t->isbreak = true;
        t->wait();
    }
    delete printer;
}

void ElectronicCountPrice_kaifeng::setSendData(const QByteArray &data)
{
    t->senddata = data;
}

QByteArray ElectronicCountPrice_kaifeng::sendData()
{
    return t->senddata;
}

bool ElectronicCountPrice_kaifeng::tryStart_qs(const QString &path, int baudrate)
{
//#ifdef QT_DEBUG

//    if(!printer->tryOpen("/dev/ttyS0,19200,0,8,1")) return false;
//    t->torestart();

//    return true;
//#endif
    if(!printer->tryOpen(printer->pathBaudRate(path, baudrate))) return false;
    t->torestart();

    return true;
}

void ElectronicCountPrice_kaifeng::stop()
{
    t->tostop();
    printer->close();
}

void ElectronicCountPrice_kaifeng::restart()
{
}

QString ElectronicCountPrice_kaifeng::lastError()
{
    return printer->lastError();
}

void ElectronicCountPrice_kaifeng::setBaudrate(const QString &baudrate)
{
    printer->setBaudRate(baudrate.toInt());
}

const Printer_POS_SERIAL_Thread_scale *ElectronicCountPrice_kaifeng::threader()
{
    return t;
}

//===============================================

void Printer_POS_SERIAL_Thread_scale::run()
{
    if(_printer->isOpen()) {
        _printer->writedirect(senddata);
    }
    int waits=0;
    forever
    {
        if(isbreak) //手动停止
            break;
        if(_printer->isOpen()) {
            if(_printer->bytesAvailable()>0) {//有数据来则直接读取，并发送，并再次获取
                QByteArray b=_printer->readAll();
                emit this->receiveData(b);
                _printer->writedirect(senddata);
                waits = 0;
            } else {//没有数据来则轮询10次后，再次发送
                if((waits++)>10) {
                    waits=0;
                    _printer->writedirect(senddata);
                }
            }
        }
        this->msleep(100);
        emit timeout();
    }
}

void Printer_POS_SERIAL_Thread_scale::updatestate()
{
    isstarted = true;
}

//一次开始，但是假如后来打senddata 更新的话，会线程失效
Printer_POS_SERIAL_Thread_scale::Printer_POS_SERIAL_Thread_scale(QObject *parent)
    : QThread(parent)
{
    isstarted = false;
    connect(this,SIGNAL(started()),this,SLOT(updatestate()));
}

Printer_POS_SERIAL_Thread_scale::~Printer_POS_SERIAL_Thread_scale()
{
}

void Printer_POS_SERIAL_Thread_scale::tostart()
{
    isbreak = false;
    start();
    //    waitForStarted();
    //If the thread is already running, this function does nothing.
}

void Printer_POS_SERIAL_Thread_scale::torestart()
{
    tostop();
    tostart();
}

void Printer_POS_SERIAL_Thread_scale::tostop()
{
    isbreak = true;
    wait();
    //This function will return true if the thread has finished.
    //It also returns true if the thread has not been started yet.
    //If time is ULONG_MAX (the default), then the wait will never timeout
}

bool Printer_POS_SERIAL_Thread_scale::waitForStarted(int msecs)
{
    int count = msecs/100;
    for(int k = 0; k < count && false == isstarted; k++) {
        printer_0_thread::msecSleep(100);
    }
    return isstarted;
}
