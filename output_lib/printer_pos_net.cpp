#include "printer_pos_net.h"
#include <QTcpSocket>
#include <QStringList>
#include <QHostAddress>
#include "printer_0.h"
#include <QTimer>


QMap<QString, Printer_POS_NET::MutexState > Printer_POS_NET::ip_mutex_map;

void Printer_POS_NET::ip_mutex_lock(const QString &ip)
{
    if(ip_mutex_map.find(ip) == ip_mutex_map.end())
        ip_mutex_map.insert(ip, MutexState(new QMutex));
    ip_mutex_map[ip].lock();
    qDebug() << __FILE__ << "lock state" << __LINE__ << "lock";
}

void Printer_POS_NET::ip_mutex_unlock(const QString &ip)
{
    qDebug() << __FILE__ << "lock state" << __LINE__ << "unlock";
    if(ip_mutex_map.find(ip) == ip_mutex_map.end())
        ip_mutex_map.insert(ip, MutexState(new QMutex));
    ip_mutex_map[ip].unlock();
}

Printer_POS_NET::Printer_POS_NET()
{
    tcp = new QTcpSocket;
}

Printer_POS_NET::~Printer_POS_NET()
{
    close();
    delete tcp;
}

bool Printer_POS_NET::isOpen()
{
    return tcp->state() == QAbstractSocket::ConnectedState;
}

qint64 Printer_POS_NET::writedirect(const QByteArray &byte)
{
    return tcp->write(byte);
}

qint64 Printer_POS_NET::write(const QString & str, QString languageKey)
{
    qDebug() <<__LINE__ << str;
    return writedirect(printer_0::trans_u2codepage(str, languageKey, "GBK"));
}

qint64 Printer_POS_NET::writeHex(const QString &hex)
{
    return writedirect(QByteArray::fromHex(hex.toLocal8Bit()));
}

bool Printer_POS_NET::tryOpen(const QString &ip_port)
{
    ip_mutex_lock(ip_port);

    _ip_port = ip_port;
    tcp->connectToHost(ip_port.split(":").value(0), ip_port.split(":").value(1, "9100").toInt());
    tcp->waitForConnected(10000);
    return tcp->state() == QTcpSocket::ConnectedState;
}

void Printer_POS_NET::close()
{
    tcp->disconnectFromHost();
    tcp->waitForDisconnected(10000);
    ip_mutex_unlock(_ip_port);
    qDebug() << tcp->state();
}

QString Printer_POS_NET::lastError()
{
    return tcp->errorString();
}

QString Printer_POS_NET::port()
{
    return _ip_port;
}

bool Printer_POS_NET::waitForReadyReadv(int msecs)
{
    return waitForReadyRead(msecs);
}

QByteArray Printer_POS_NET::readAll()
{
    return tcp->readAll();
}

bool Printer_POS_NET::waitForReadyRead(int msecs)
{
    return tcp->waitForReadyRead(msecs);
}


Printer_POS_NET::MutexState::MutexState()
{

}

Printer_POS_NET::MutexState::MutexState(QMutex *m)
{
    is_locked = false;
    mutex = QSharedPointer<QMutex>(m);
}

void Printer_POS_NET::MutexState::lock()
{
    mutex->lock();
    is_locked = true;
}

void Printer_POS_NET::MutexState::unlock()
{
    if(is_locked) {
        qDebug() << __FILE__ << "lock state" << __LINE__ << "real unlock";
        mutex->unlock();
    }
    is_locked = false;
}
