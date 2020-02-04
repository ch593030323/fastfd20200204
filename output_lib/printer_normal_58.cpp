#include "printer_normal_58.h"

////注册
static Printer_Normal_virtual_Register<Printer_Normal_58> r(QList<QStandardItem*>() << new QStandardItem("Printer_Normal_58"));
////

Printer_Normal_58::Printer_Normal_58()
{
}

void Printer_Normal_58::tran(blockIns &bins, int xinye_beep)
{
    Q_UNUSED(xinye_beep)
    Q_UNUSED(bins)
}

QString Printer_Normal_58::name()
{
    return "HDD";
}
