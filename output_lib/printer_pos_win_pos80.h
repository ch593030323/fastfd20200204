#ifndef PRINTER_POS_WIN_POS80_H
#define PRINTER_POS_WIN_POS80_H
#include "printer_pos.h"
#ifdef Q_OS_WIN
#include <windows.h>
#endif

class Printer_POS_win_POS80 : public Printer_POS
{
public:
    Printer_POS_win_POS80();
    ~Printer_POS_win_POS80();

    //virtual
    virtual qint64 writedirect(const QByteArray & byte);
    virtual qint64 write(const QString & str, QString languageKey);//if n have write then n is return; if n == -1 then error
    virtual qint64 writeHex(const QString &hex);//if n have write then n is return; if n == -1 then error
    virtual bool tryOpen(const QString &port);
    virtual bool isOpen();
    virtual void close();
    virtual QString lastError();
    virtual QString port();
    virtual bool waitForReadyReadv(int msecs);
    virtual QByteArray readAll();
    //~virtual

private:
#ifdef Q_OS_WIN
    HANDLE hPrinter;
#endif
    QString errstring;
    bool _open;
    QString _port;
};

#endif // PRINTER_POS_WIN_POS80_H
