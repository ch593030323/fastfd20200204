#ifndef PRINTER_POS_PARALLEL_H
#define PRINTER_POS_PARALLEL_H
#include <stdio.h>
#include "printer_pos.h"

class Printer_POS_PARALLEL : public Printer_POS
{
public:
    static QString printPort();
public:
    Printer_POS_PARALLEL();
    virtual ~Printer_POS_PARALLEL();

    //virtual
    virtual qint64 writedirect(const QByteArray & byte);
    virtual qint64 write(const QString & str, QString languageKey);
    virtual qint64 writeHex(const QString & hex);
    virtual bool tryOpen(const QString &ipport);
    virtual bool isOpen();
    virtual void close();
    virtual QString lastError();
    virtual QString port();
    virtual bool waitForReadyReadv(int msecs);
    virtual QByteArray readAll();
    //~virtual

private:
    QString m_error;
    QString m_port;
    bool _open;
    FILE *m_file;
};

#endif // PRINTER_POS_PARALLEL_H
