#ifndef PRINTER_NORMAL_XINYE_H
#define PRINTER_NORMAL_XINYE_H

#include "printer_normal.h"

class  Printer_Normal_XINYE  : public Printer_Normal
{
public:
    Printer_Normal_XINYE();
    virtual void tran(blockIns &bins, int xinye_beep);
    virtual QString name();
};

#endif // PRINTER_NORMAL_XINYE_H
