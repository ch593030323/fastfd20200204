#include "printer_normal_80_old.h"
#include <QtDebug>

Printer_Normal_80_old::Printer_Normal_80_old()
{
}


void Printer_Normal_80_old::tran(blockIns &bins, int xinye_beep)
{
    Q_UNUSED(xinye_beep)
    Q_UNUSED(bins)
}

QString Printer_Normal_80_old::name()
{
    return "";
}

QByteArray Printer_Normal_80_old::init()
{
    QByteArray r = QByteArray::fromHex("1B40");
    return r.toHex();
}

QByteArray Printer_Normal_80_old::scale(char c)
{
    /*
4． ESC ! n 设置所有字符打印方式
格式： ASCII：ESC ! n
十进制：[27][33]n
十六进制：[1BH][21H]n
说明：
0≤n≤255。
．综合性的字符打印方式设置命令，用于选择打印字符是否以倍宽和倍高打印。
默认值
n=0，即字符不放大；
n=16，即字符以倍高打印；
n=32 ，即字符以倍宽打印；
n=48，即字符以倍宽和倍高打印。使用“ESC DC4”命令后无效。
*/
    QByteArray r;
    switch(c) {
    case 0x00:
        r = QByteArray::fromHex("1b2100");
        break;
    case 0x01:
        r = QByteArray::fromHex("1b2110");
        break;
    case 0x10:
        r = QByteArray::fromHex("1b2120");
        break;
    case 0x11:
        r = QByteArray::fromHex("1b2130");
        break;
    }

    return r.toHex();
}

QByteArray Printer_Normal_80_old::align(char c)
{
    QByteArray r;
    switch(c) {
    case 0x00://中
        r = QByteArray::fromHex("1B6101");
        break;
    case 0x01://右
        r = QByteArray::fromHex("1B6102");
        break;
    case 0x10://左
        r = QByteArray::fromHex("1B6100");
        break;
    }
    return r.toHex();
}

QByteArray Printer_Normal_80_old::gonum(char c)
{
    Q_UNUSED(c)
    /*
11． ESC J n  打印并走纸n点行
格式： ASCII： ESC J n
十进制： [27][74] n
十六进制： [1BH][4AH] n
说明：
．打印行缓冲器里的内容，并向前走纸n点行。0≤n≤255。
．该命令只本行打印有效，不改变ESC 2，ESC 3命令设置的行间距值。
*/
    QByteArray r;
//    r = QByteArray::fromHex("1b4a")+c;
    return r.toHex();
}
