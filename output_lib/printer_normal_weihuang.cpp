#include "printer_normal_weihuang.h"
#include <QtDebug>
////注册
static Printer_Normal_virtual_Register<Printer_Normal_WEIHUANG> r(QList<QStandardItem*>() << new QStandardItem("Printer_Normal_WEIHUANG"));
////

Printer_Normal_WEIHUANG::Printer_Normal_WEIHUANG()
{
}

void Printer_Normal_WEIHUANG::tran(blockIns &bins, int xinye_beep)
{
    Q_UNUSED(xinye_beep)
    Q_UNUSED(bins)
}

QString Printer_Normal_WEIHUANG::name()
{
    return "HDD_MINI2";
}

QByteArray Printer_Normal_WEIHUANG::qr_code(const QString &code)
{
    //编号转成GBK
    QByteArray content = printer_0::trans_UTF82GBK(code);
    //b
    qint16 lenght = content.length() + 2;//315030 三位
    char size0 = lenght & 0xff;//取低8位
    char size1 = lenght >> 8;//取高8位
    QByteArray pLH;
    pLH += size0;
    pLH += size1;
    //b0
    QByteArray b0;
    b0 += "1b6d01";//浓度
    b0 +="1D2831";//固定
    b0 += pLH.toHex();
    b0 += "0000";//v、e ：QR码版本号、ECC纠错级别
    b0 += content.toHex();

    return b0;
}
