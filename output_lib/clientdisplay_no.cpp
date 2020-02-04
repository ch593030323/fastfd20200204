#include "clientdisplay_no.h"
#include "printer_pos_serial.h"
#include <QtDebug>

//static char send_content[] = {0x1b, 0x51, 0x41};
//static char send_sqr[] = {0x1b,0x73};
QString ClientDisplay_NO::err0;
ClientDisplay_NO::ClientDisplay_NO()
{
}

ClientDisplay_NO::~ClientDisplay_NO()
{
}

bool ClientDisplay_NO::writeCom(int sqr, const QString &i, const QString &devpath)
{
    Printer_POS_SERIAL printer;
    if(!printer.tryOpen(devpath)) {
        err0 = printer.lastError();
        return false;
    }
    printer.setBaudRate(2400); //波特率必须时2400 才可以通信
    QByteArray in;
    QByteArray s1,s2;
    in.append(i);
    //1 2 3 4
    //小于零的按零算 ，大于4的按五算 ，其他为1 2 3 4 ，0 5 为清空，及没有灯点亮
    sqr = qMax(0,qMin(5,sqr));//sqr < 0? 0 : sqr > 4 ?  5 : sqr;
    char m[] = {0x1b, 0x51, 0x41};
    s1 = QByteArray(m, sizeof(m)) + in + (char)0x0d;
    //5. ESC  Q  A  d1d2d3…dn  CR送显示数据命令
    printer.writedirect(s1);
    //6. ESC  s  n设置 “单价”、“总计”、“收款”、“找零”字符显示状态命令
    char m1[] = {0x1b,0x73};
    s2 = QByteArray(m1, sizeof(m1)) + (char)(0x30 + sqr);
    printer.writedirect(s2);
    return true;
}

bool ClientDisplay_NO::cleanCom(const QString &devpath)
{
    Printer_POS_SERIAL printer;
    if(!printer.tryOpen(devpath)) {
        err0 = printer.lastError();
        return false;
    }
    printer.setBaudRate(2400); //波特率必须时2400 才可以通信

    char m[] = { 0x1b, 0x40 };
    printer.writedirect(QByteArray(m, sizeof(m)));
    return true;
}

QString ClientDisplay_NO::lastError()
{
    return err0;
}
