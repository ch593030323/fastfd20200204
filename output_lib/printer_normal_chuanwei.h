#ifndef PRINTER_NORMAL_CHUANWEI_H
#define PRINTER_NORMAL_CHUANWEI_H

#include "printer_normal.h"

class Printer_Normal_CHUANWEI : public Printer_Normal
{
public:
    Printer_Normal_CHUANWEI();
    virtual void tran(blockIns &bins, int xinye_beep);
    virtual QString name();
};

#endif // PRINTER_NORMAL_CHUANWEI_H
