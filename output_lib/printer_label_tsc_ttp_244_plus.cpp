#include "printer_label_tsc_ttp_244_plus.h"
////注册
static printer_label_Virtual_Register<printer_label_tsc_ttp_244_plus>
r(QList<QStandardItem*>() << new QStandardItem("printer_label_tsc_ttp_244_plus"));
////
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
printer_label_tsc_ttp_244_plus::printer_label_tsc_ttp_244_plus()
{
}

printer_label_tsc_ttp_244_plus::~printer_label_tsc_ttp_244_plus()
{

}

QString printer_label_tsc_ttp_244_plus::name()
{
    return "TSC TTP-244 PLUS";
}

QMap<QString, QString> printer_label_tsc_ttp_244_plus::print_bar_code_map()
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

QMap<QString, QString> printer_label_tsc_ttp_244_plus::print_qr_code_map()
{
    static QMap<QString, QString> map;
    if(map.isEmpty()) {

    }
    return map;
}

QMap<QString, QString> printer_label_tsc_ttp_244_plus::print_text_map()
{
    static QMap<QString, QString> map;
    if(map.isEmpty()) {
        map.insert("font1", "1");
        map.insert("font2", "2");
        map.insert("font3", "3");
        map.insert("font4", "4");
        map.insert("font5", "5");

        map.insert("font6", "6");
        map.insert("font7", "7");
        map.insert("font8", "8");
    }
    return map;
}

QStringList printer_label_tsc_ttp_244_plus::support_printer_type()
{
    return QStringList() << "TSC TTP-244 PLUS";
}

double printer_label_tsc_ttp_244_plus::value_translate(double value, unitType value_unit, dpiType value_dpi)
{
    return valueTranslate(value, value_unit, value_dpi, mm, DPI200);
}

QByteArray printer_label_tsc_ttp_244_plus::lineEnd()
{
    return "\r\n";
    return "\n";
}

QByteArray printer_label_tsc_ttp_244_plus::GAP(int vgap)
{
    return "";
    return "GAP " + QByteArray::number(vgap) + " mm, 2 mm\n";
}


QString printer_label_tsc_ttp_244_plus::toPrint(const QString &in, int piece, double height, int gap, double width)
{
    return QString(
                "SIZE %1 mm,%2 mm"+lineEnd()+
                GAP(gap) +
                "DENSITY %3"+lineEnd()+
                "SPEED %4"+lineEnd()+
                "REFERENCE 1,1"+lineEnd()+
                "DIRECTION 1"+lineEnd()+
                "CLS"+lineEnd()//清楚image的缓存
                )
            .arg(width, 0, 'f', 2).arg(height, 0, 'f', 2)
            .arg(15)//density
            .arg(4) //speed
            +in+
            QString("PRINT %1,1"+lineEnd())
            .arg(piece);
}


QString printer_label_tsc_ttp_244_plus::drawText(const QString &data, int x, int y, const QString &font, const QSize &size)
{
    QString str;
    foreach(const QString &d, data.split("\n", QString::SkipEmptyParts)) {
        str += QString("TEXT %1,%2,\"%3\",%4,%5,%6,\"%7\""+lineEnd())
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

QString printer_label_tsc_ttp_244_plus::drawOneCode(const QString &data, int x, int y, const QString &bartype, int height, printer_label_tsc_ttp_244_plus::ROTATE rotate, int widebarwidth, int narrowbarwidth)
{
    return QString("BARCODE %1,%2,\"%3\",%4,%5,%6,%7,%8,\"%9\""+lineEnd())
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

QString printer_label_tsc_ttp_244_plus::drawTwoCode(const QString &data, int x, int y)
{
    return QString("QRCODE %1,%2,%3,%4,%5,%6,%7,%8,\"%9\""+lineEnd())
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
