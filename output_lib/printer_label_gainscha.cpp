#include "printer_label_gainscha.h"

#include <QtDebug>
////注册
static printer_label_Virtual_Register<printer_label_gainscha>
r(QList<QStandardItem*>() << new QStandardItem("printer_label_gainscha"));
////
//SOUND 3,200 蜂鸣 3次，每次间隔200ms
/**
Barcode type
128
128M
EAN128
25
25C

39
39C
93
EAN13
EAN13+2

EAN13+5
EAN8
EAN8+2
EAN8+5
CODA

POST
UPCA
UPCA+2
UPCA+5
UPCE

UPCE+2
UPCE+5
CPOST
MSI
MSIC

PLESSY
ITF14
EAN14
*/
static QMap<QString, QString> textfont;
static QMap<QString, QString> onebartype;
static QMap<QString, QString> twobartype;

printer_label_gainscha::printer_label_gainscha()
{
    //        speical_PRINTSIZE.insert("40x30", QSize(42.48, 32.48));
}

printer_label_gainscha::~printer_label_gainscha()
{

}

QString printer_label_gainscha::name()
{
    return "Gainscha";
}

QMap<QString, QString> printer_label_gainscha::print_bar_code_map()
{
    static QMap<QString, QString> map;
    if(map.isEmpty()) {
        map.insert("Code 128 AUTO", "128");
        map.insert("Code 128 MANUAL", "128M");
        map.insert("Code 128 AUTO", "EAN128");
        map.insert("Interleaved 2 of 5", "25");
        map.insert("Interleaved 2 of 5 with check digital", "25C");

        map.insert("Code 39", "39");
        map.insert("Code 39 with check digital", "39C");
        map.insert("93", "93");
        map.insert("EAN 13", "EAN13");
        map.insert("EAN 13 with 2 digits add-on", "EAN13+2");

        map.insert("EAN 13 with 5 digits add-on", "EAN13+5");
        map.insert("EAN 8", "EAN8");
        map.insert("EAN 8 with 2 digits add-on", "EAN8+2");
        map.insert("EAN 8 with 5 digits add-on", "EAN8+5");
        map.insert("Codabar", "CODA");

        map.insert("Postnet", "CODA");
        map.insert("UPC-A", "UPCA");
        map.insert("UPC-A with 2 digits add-on", "UPCA+2");
        map.insert("UPC-A with 5 digits add-on", "UPCA+5");
        map.insert("UPC-E", "UPCE");

        map.insert("UPC-E with 2 digits add-on", "UPCE+2");
        map.insert("UPC-E with 5 digits add-on", "UPCE+5");
        map.insert("China post code", "CPOST");
        map.insert("MSI code", "MSI");
        map.insert("MSI code with check digital", "MSIC");

        map.insert("PLESSEY code", "PLESSY");
        map.insert("ITF 14 code", "ITF14");
        map.insert("EAN 14 code", "EAN14");
    }
    return map;
}

QMap<QString, QString> printer_label_gainscha::print_qr_code_map()
{
    static QMap<QString, QString> map;
    if(map.isEmpty()) {
    }
    return map;
}

QMap<QString, QString> printer_label_gainscha::print_text_map()
{
    static QMap<QString, QString> map;
    if(map.isEmpty()) {
        map.insert("TSS24.BF2", "TSS24.BF2");
    }
    return map;
}

QStringList printer_label_gainscha::support_printer_type()
{
    return QStringList() << QObject::tr("佳博打印机");
}

double printer_label_gainscha::value_translate(double value, unitType value_unit, dpiType value_dpi)
{
    return valueTranslate(value, value_unit, value_dpi, mm, DPI200);
}

QString printer_label_gainscha::toPrint(const QString &in, int piece, float height, float gap, float width, const QString &unit)
{
    Q_UNUSED(gap)

    return QString(
                "SIZE %1 %6,%2 %6\n"
                "GAP %3 mm,0 mm\n"
                "REFERENCE 0,0\n"
                "SPEED %5\n"
                "DENSITY %4\n"
                "SET PEEL OFF\n"
                "SET CUTTER OFF\n"
                "SET PARTIAL_CUTTER OFF\n"
                "SET TEAR ON\n"
                "DIRECTION 1\n"
                "SHIFT 0\n"
                "OFFSET 0 mm\n"
                "CLS\n"//清楚image的缓存
                "%7PRINT %8,1\n"
                )
            .arg(width).arg(height/*+gap*/)
            .arg(2)
            .arg("8")//density
            .arg("4.0") //speed
            .arg(unit)
            .arg(in)
            .arg(piece)
            ;

}


QString printer_label_gainscha::drawText(const QString &data, int x, int y, const QString &font, const QSize &size)
{
    QString str;
    foreach(const QString &d, data.split("\n", QString::SkipEmptyParts)) {
        str += QString("TEXT %1,%2,\"%3\",%4,%5,%6,\"%7\"\n")
                .arg(x)
                .arg(y)
                .arg(font)
                .arg(0)//旋转角度
                .arg(size.width())
                .arg(size.height())//水平，垂直放大陪数
                .arg(d);//打印内容
        y += line_char_height * size.height();
    }
    return str;
}

QString printer_label_gainscha::drawOneCode(const QString &data, int x, int y, const QString &bartype, int height, printer_label_gainscha::ROTATE rotate, int widebarwidth, int narrowbarwidth)
{
    return QString("BARCODE %1,%2,\"%3\",%4,%5,%6,%7,%8,\"%9\"\n")
            .arg(x)
            .arg(y)
            .arg(bartype)//对于的条形码的种类，这儿只写一种，其他的请查阅文档
            .arg(height)//条形码的高度
            .arg(1)//条形码的可视属性，1为可见，0为不可见
            .arg(rotate)//旋转，这儿默认旋转为不旋转
            .arg(widebarwidth).arg(narrowbarwidth)//宽窄比例
            .arg(data);//内容
    //"BARCODE 0,60,\"128M\",20,1,0,2,2,\"!104!096ABCD!101EFGH\"\n"//条形码ok
}

QString printer_label_gainscha::drawTwoCode(const QString &data, int x, int y)
{
    return QString("QRCODE %1,%2,%3,%4,%5,%6,%7,%8,\"%9\"\n")
            .arg(x)
            .arg(y)
            .arg("L")//错误纠错能力等级
            .arg(7)//cell width

            .arg("M")//手动生成编号
            .arg(0)//不选转
            .arg("M1")//扩大版本
            .arg("S0")//mask 0

            .arg(QString("B0012%1").arg(data));//B0012是字节模式
    //QRCODE 100,10,L,7,M,0,M1,S1,"ATHE FIRMWARE HAS BEEN UPDATED"
}

