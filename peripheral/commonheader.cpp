#include "commonheader.h"
#include "electronicscale_cas.h"
#include "electronicscale_dahua.h"
#include "electronicscale_ysheng.h"
#include "electronicscale_djian.h"
#include "electronicscale_kaifeng.h"
#include "printer_0.h"
#include <QSettings>
#include "printer_label_postek.h"
#include "printer_label_argox_pplb.h"
#include "printer_label_argox_ppla.h"
#include "printer_label_tsc_ttp_244_plus.h"
#include "printer_label_gainscha.h"
#include "printer_pos_serial.h"
#include "printer_pos_vidpid.h"
#include "printer_pos_net.h"
#include <QtCore/qmath.h>
#include "printer_normal_80.h"
#include "electronicscale_rongta.h"
#include "clientdisplay.h"
#include "lds.h"
#include "printer_0.h"

QStringList CommonHeader::normal_printtypes;
QStringList CommonHeader::normal_print_instypes;
QStringList CommonHeader::normal_printpathtypes;

QStringList CommonHeader::pricecout_scaletypes;

QStringList CommonHeader::clientdisplay_ledtypes;

QStringList CommonHeader::printtypes;
QStringList CommonHeader::printpathtypes;

QStringList CommonHeader::barcode_scaletypes;

QMap<int, QString> CommonHeader::barcode_scale_weightype;

QStandardItemModel CommonHeader::label_vidpid_model;

CommonHeader::CommonHeader()
{
}

CommonHeader::~CommonHeader()
{
}

QStringList CommonHeader::creat_printtypes()
{
    if(printtypes.isEmpty()) {
        printtypes  << QObject::tr("POSTEK Q8/200") << QObject::tr("TSC TTP-244 PLUS")
                    << QObject::tr("SC2100(商超专用)") << QObject::tr("ARGOX CP-2140")
                    << ("GP-3120TN")
                    << ("GP-3150TIN")
                    << ("GP-2120TF")
                    << ("GP-9034T")
                    << ("GODEX G500-U")
                    << ("GP-1224T")
                    << QObject::tr("汉印(HPRT)")
                       ;
    }
    return printtypes;
}

QStandardItemModel &CommonHeader::creat_print_vidpidmodel()
{
    if(label_vidpid_model.rowCount()==0) {
        label_vidpid_model.setHorizontalHeaderLabels(QStringList()<<QObject::tr("标签打印机名称")<<QObject::tr("打印机类型")<<QObject::tr("VIDPID")<<QObject::tr("描述"));
        label_vidpid_model.appendRow(QList<QStandardItem*>()
                                     <<new QStandardItem("p1")
                                     <<new QStandardItem("GP-3120TN")
                                     <<new QStandardItem("0471&0017")
                                     <<new QStandardItem("")
                                     );
        //USB\VID_6868&PID_0500&REV_0100

        label_vidpid_model.appendRow(QList<QStandardItem*>()
                                     <<new QStandardItem("p1")
                                     <<new QStandardItem("GP-2120TF")
                                     <<new QStandardItem("6868&0500")
                                     <<new QStandardItem("")
                                     );
    }
    return label_vidpid_model;
}

QStringList CommonHeader::creat_printpathtypes()
{
    /**
USB TO  f_scan_usbPort
串口哦 TO  f_scan_ttyPort
*/
    if(printpathtypes.isEmpty()) {
        printpathtypes << QObject::tr("USB") << QObject::tr("串口")<< QObject::tr("网口");
    }
    return printpathtypes;
}

QMap<int, QString> CommonHeader::creat_baudrates()
{
        return Printer_POS_SERIAL::creat_baudrates();
}

QString CommonHeader::getAboutLabelPrinter()
{
    return
            "POSTEK Q8/200 请使用【24点阵中文字体】\n"
            "TSC TTP-244 PLUS 需要下载字体才可以支持中文\n"
            "SC2100(商超专用) 暂时不支持中文\n"
            "ARGOX CP-2140 需要下载字体才可以支持中文\n"
            "系列 请使用【TSS24.BF2】\n"
            "GODEX G500-U 请使用【Chinese font】\n"
            ;
}

QStringList CommonHeader::creat_clientdisplay_ledtypes()
{
    if(clientdisplay_ledtypes.isEmpty()) {
        clientdisplay_ledtypes = ClientDisplay::creat_clientdisplay_ledtypes();
    }
    return clientdisplay_ledtypes;
}

QStringList CommonHeader::creat_normal_printpathtypes()
{
    return printer_0::creat_normal_printpathtypes();

}

QStringList CommonHeader::creat_barcode_scaletypes()
{
    if(barcode_scaletypes.isEmpty()) {
        barcode_scaletypes << QObject::tr("上海大华 TM") << QObject::tr("上海凯士 CL5200")
                           << QObject::tr("上海精涵友声 TM") << QObject::tr("厦门顶尖LS2CX")
                           << QObject::tr("凯丰条码秤") << ElectronicScale_RONGTA::scaleName()
                              //                      #ifdef QT_DEBUG
                           << QObject::tr("上海大华 TM")+"_EN" << QObject::tr("上海大华 TM")+"_07"
                              //                      #endif
                              ;
    }
    return barcode_scaletypes;
}


QMap<int, QString> CommonHeader::creat_barcode_scale_weightype(const QString &type)
{
    barcode_scale_weightype.clear();
    switch(creat_barcode_scaletypes().indexOf(type)) {
    case 0://上海大华 TM-15H
        barcode_scale_weightype = ElectronicScale_DAHUA::WeighType();
        break;
    case 1://凯士 CL5200
        barcode_scale_weightype = ElectronicScale_CAS::WeighType();
        break;
    case 2://友声
        barcode_scale_weightype = ElectronicScale_YSHENG::WeighType();
        break;
    case 3://厦门顶尖LS2CX
        barcode_scale_weightype = ElectronicScale_DJIAN::WeighType();
        break;
    case 4://凯丰条码秤
        barcode_scale_weightype = ElectronicScale_KAIFENG::WeighType();
        break;
    case 5://容大RLS1000
        barcode_scale_weightype = ElectronicScale_RONGTA::WeighType();
        break;
    }
    return barcode_scale_weightype;
}

QString CommonHeader::creat_barcode_scale_netport_init(const QString &type)
{
    QString value;
    switch(creat_barcode_scaletypes().indexOf(type)) {
    case 0://上海大华 TM-15H
        value = "4001";
        break;
    case 1://凯士 CL5200
        value = "20304";
        break;
    case 2://友声
        value = "33581";
        break;
    case 3://厦门顶尖LS2CX
        value="6275";
        break;
    case 4://凯丰条码秤
        value = "5100";
        break;
    case 5://容大RLS1000
        value = ElectronicScale_RONGTA::barcode_default_port();
        break;
    }
    return value;
}

QStringList CommonHeader::creat_barcode_scalebarcodetype(const QString &type)
{
    QStringList value;
    switch(creat_barcode_scaletypes().indexOf(type)) {
    case 0://上海大华 TM-15H
        value = ElectronicScale_DAHUA::BarType();
        break;
    case 1://凯士 CL5200
        value = ElectronicScale_CAS::BarType();
        break;
    case 2://友声
        value = ElectronicScale_YSHENG::BarType();
        break;
    case 3://厦门顶尖LS2CX
        value = ElectronicScale_DJIAN::BarType();
        break;
    case 4://凯丰条码秤
        value = ElectronicScale_KAIFENG::BarType();
        break;
    case 5://容大RLS1000
        value = ElectronicScale_RONGTA::BarType().values();
        break;
    }
    return value;
}

bool CommonHeader::barcode_scale_resolve(QString &barcode, QString &price, QString &weight, QString &amount, QString code)
{
    QString flag=lds::sysconf->value("barscale/scaleflag").toString();
    QString type=lds::sysconf->value("barscale/scalebartype").toString();
    int precision_amount = lds::sysconf->value("barscale/precision_amount", 2).toInt();
    int precision_weight = lds::sysconf->value("barscale/precision_weight", 3).toInt();
    int precision_price = lds::sysconf->value("barscale/precision_price", 2).toInt();
    if(flag.count() == 1)  {
        flag = "0" + flag;
        code ="0" + code;
    }
    bool f = code.startsWith(flag)&&
            (ElectronicScale_DAHUA::barcode_scale_resolve(barcode,price,weight,amount,code,type)
             || ElectronicScale_CAS::barcode_scale_resolve(barcode,price,weight,amount,code,type)
             || ElectronicScale_YSHENG::barcode_scale_resolve(barcode,price,weight,amount,code,type)
             || ElectronicScale_DJIAN::barcode_scale_resolve(barcode,price,weight,amount,code,type)
             || ElectronicScale_KAIFENG::barcode_scale_resolve(barcode,price,weight,amount,code,type)
             || ElectronicScale_RONGTA::barcode_scale_resolve(barcode,price,weight,amount,code,type)
             );

    if(f) {
        bool ok;
        int tmp;
        tmp = price.toInt(&ok);
        if(ok) {
            price = QString::number(tmp/qPow(10,precision_price));//
        }

        tmp = weight.toInt(&ok);
        if(ok) {
            weight = QString::number(tmp/qPow(10,precision_weight));//);
        }

        tmp = amount.toInt(&ok);
        if(ok) {
            amount = QString::number(tmp/qPow(10,precision_amount));//);
        }
    }

    return f;
}
