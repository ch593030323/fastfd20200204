#ifndef PRINTER_NORMAL_80_OLD_H
#define PRINTER_NORMAL_80_OLD_H

#include "printer_normal.h"

class Printer_Normal_80_old : public Printer_Normal
{
public:
    Printer_Normal_80_old();
    virtual void tran(blockIns &bins, int xinye_beep);
    virtual QString name();

    virtual QByteArray init();//1b40
    virtual QByteArray scale(char c);//0x00默认 0x01倍高 0x10倍宽 x11倍高倍宽
    virtual QByteArray align(char c);//0x00居中 0x10左对齐 0x01右对齐
    virtual QByteArray gonum(char c);//0x01-0x05 打印并走纸
};

#endif // PRINTER_NORMAL_80_OLD_H
