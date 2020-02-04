#ifndef PRINTER_NORMAL_WEIHUANG_H
#define PRINTER_NORMAL_WEIHUANG_H

#include "printer_normal.h"

class Printer_Normal_WEIHUANG : public Printer_Normal
{
public:
    Printer_Normal_WEIHUANG();
    virtual void tran(blockIns &bins, int xinye_beep);
    virtual QString name();

    virtual QByteArray qr_code(const QString &code);//获取二维码，打印指令
};

#endif // PRINTER_NORMAL_WEIHUANG_H
