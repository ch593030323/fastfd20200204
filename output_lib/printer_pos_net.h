#ifndef PRINTER_POS_NET_H
#define PRINTER_POS_NET_H

#include <QTcpSocket>
#include <QUdpSocket>
#include "printer_pos.h"


/**
example:
        Printer_POS_NET printer;
        if(printer.tryconnect("192.168.1.201", 9100)) {
            foreach(pieceIns ins, printIns) {
                printer.writedirect(QByteArray::fromHex(ins.first));
                printer.writedirect(printer_0::trans_UTF82GBK(ins.second));
            }
        } else {
            //printer.errorString();
        }
        printer.closeconnect();
*/
class Printer_POS_NET : public Printer_POS
{
public:
    struct MutexState{
        MutexState();
        MutexState(QMutex *m);
        void lock();
        void unlock();

        QSharedPointer<QMutex> mutex;
        bool is_locked;
    };

    static QMap<QString, MutexState >  ip_mutex_map;
    static void ip_mutex_lock(const QString &ip);
    static void ip_mutex_unlock(const QString &ip);
public:
    Printer_POS_NET();
    virtual ~Printer_POS_NET();
    //virtual
    qint64 writedirect(const QByteArray & byte);
    virtual qint64 write(const QString & str, QString languageKey);
    virtual qint64 writeHex(const QString & hex);
    virtual bool tryOpen(const QString &ip_port);
    virtual bool isOpen();
    virtual void close();
    virtual QString lastError();
    virtual QString port();
    virtual bool waitForReadyReadv(int msecs);
    virtual QByteArray readAll();
    //~virtual
    bool waitForReadyRead(int msecs = 30000);
private:
    QString _ip_port;

    QTcpSocket *tcp;
};


#endif // PRINTER_POS_NET_H
