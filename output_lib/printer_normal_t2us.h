#ifndef PRINTER_NORMAL_T2US_H
#define PRINTER_NORMAL_T2US_H

#include "printer_normal.h"

class  Printer_Normal_T2US : public Printer_Normal
{
public:
    Printer_Normal_T2US();
    virtual void tran(blockIns &bins, int xinye_beep);
    virtual QString name();

    const QString vidpid;
};

#endif // PRINTER_NORMAL_T2US_H
