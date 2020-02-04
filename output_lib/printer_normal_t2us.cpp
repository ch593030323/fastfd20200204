#include "printer_normal_t2us.h"
////注册
static Printer_Normal_virtual_Register<Printer_Normal_T2US> r(QList<QStandardItem*>() << new QStandardItem("Printer_Normal_T2US"));
////
Printer_Normal_T2US::Printer_Normal_T2US()
    : vidpid("1659&8963")
{
//    vidpid="0x067B&0x2303";
}
void Printer_Normal_T2US::tran(blockIns &bins, int xinye_beep)
{
    Q_UNUSED(xinye_beep)
    Q_UNUSED(bins)
}

QString Printer_Normal_T2US::name()
{
    return "MINI58";
}
