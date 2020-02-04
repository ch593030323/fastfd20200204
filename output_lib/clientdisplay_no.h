#ifndef CLIENTDISPLAY_NO_H
#define CLIENTDISPLAY_NO_H

//客显 型号未知
//【0】tryOpen(/dev/ttyS0)
//【1】ClientDisplay_NO().writeCom(2, "123.6");
#include <QStringList>
#include <QObject>
class Printer_POS_SERIAL;

class  ClientDisplay_NO
{
public:
    explicit ClientDisplay_NO();
    ~ClientDisplay_NO();
    //写串口
    static bool writeCom(int sqr, const QString &in, const QString &devpath);
    static bool cleanCom(const QString &devpath);
    static QString lastError();
    static QString err0;
};
#endif // CLIENTDISPLAY_NO_H
