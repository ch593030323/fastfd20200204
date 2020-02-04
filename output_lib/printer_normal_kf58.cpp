#include "printer_normal_kf58.h"
////注册
static Printer_Normal_virtual_Register<Printer_Normal_KF58> r(QList<QStandardItem*>() << new QStandardItem("Printer_Normal_KF58"));
////
Printer_Normal_KF58::Printer_Normal_KF58()
{
}

void Printer_Normal_KF58::tran(blockIns &bins, int xinye_beep)
{
    Q_UNUSED(xinye_beep)
    for(int c = 0, c_count = bins.count(); c < c_count; c ++) {//清除
        bins[c].first.clear();
    }
    if(bins.count() > 0) {
        bins[0].first="1B6300";
        bins << pieceIns("20200a20200a20200a","");//出纸
    }
}

QString Printer_Normal_KF58::name()
{
    return "KF58";
}
