#include "printer_label_postek.h"
////注册
static printer_label_Virtual_Register<printer_label_postek>
r(QList<QStandardItem*>() << new QStandardItem("printer_label_postek"));
////
//博思得标签打印机
/*

*/
static QByteArray SOH = QByteArray::fromHex(QByteArray().append("01"));
static QByteArray STX = QByteArray::fromHex(QByteArray().append("02"));
static QByteArray CR = QByteArray::fromHex(QByteArray().append("0D"));
static QByteArray LF = QByteArray::fromHex(QByteArray().append("0A"));
static QByteArray FILENAME = "\"standalone\"";

static QMap<QString, QString> textfont;
static QMap<QString, QString> onebartype;
static QMap<QString, QString> twobartype;
printer_label_postek::printer_label_postek()
{
}

printer_label_postek::~printer_label_postek()
{
}

QString printer_label_postek::name()
{
    return "POSTEK";
}

QMap<QString, QString> printer_label_postek::print_bar_code_map()
{
    static QMap<QString, QString> map;
    if(map.isEmpty()) {
        map.insert("Code 128 UCC (shipping container code)", "0");
        map.insert("Code 128 AUTO", "1");
        map.insert("Code 128 subset A", "1A");
        map.insert("Code 128 subset B", "1B");
        map.insert("Code 128 subset C", "1C");

        map.insert("UCC/EAN", "1E");
        map.insert("Interleaved 2 of 5", "2");
        map.insert("Interleaved 2 of 5 with check sum digit)", "2C");
        map.insert("Interleaved 2 of 5 with human readable check digit", "2D");
        map.insert("German Postcode", "2G");

        map.insert("Matrix 2 of 5", "2M");
        map.insert("UPC Interleaved 2 of 5", "2U");
        map.insert("Code 3 of 9", "3");
        map.insert("Code 3 of 9 with check sum digit", "3C");
        map.insert("Extended Code 3 of 9", "3E");

        map.insert("Extended Code 3 of 9 with check sum digit", "3F");
        map.insert("Code93", "9");
        map.insert("EAN-13", "E30");
        map.insert("EAN-13 2 digit add-on", "E32");
        map.insert("EAN-13 5 digit add-on", "E35");

        map.insert("EAN-8", "E80");
        map.insert("EAN-8 2 digit add-on", "E82");
        map.insert("EAN-8 5 digit add-on", "E-85");
        map.insert("Codabar", "K");
        map.insert("Postnet", "P");

        map.insert("UPC-A", "UA0");
        map.insert("UPC-A 2 digit add-on", "UA2");
        map.insert("UPC-A 5 digit add-on", "UA5");
        map.insert("UPC-E", "UE0");
        map.insert("UPC-E 2 digit add-on", "UE2");

        map.insert("UPC-E 5 digit add-on ", "UE5");
    }
    return map;
}

QMap<QString, QString> printer_label_postek::print_qr_code_map()
{
    static QMap<QString, QString> map;
    if(map.isEmpty()) {
        map.insert("Maxi Code", "M");
        map.insert("PDF-417", "P");
        map.insert("DataMatrix ECC200", "DX");
        map.insert("DataMatrix ECC000 （*）", "D0");
        map.insert("DataMatrix ECC050 （*）", "D1");

        map.insert("DataMatrix ECC080 （*）", "D2");
        map.insert("DataMatrix ECC100 （*）", "D3");
        map.insert("DataMatrix ECC140 （*）", "D4");
        map.insert("QR", "QR");
        map.insert("汉信码", "HX");
    }
    return map;
}

QMap<QString, QString> printer_label_postek::print_text_map()
{
    static QMap<QString, QString> map;
    if(map.isEmpty()) {
        map.insert("西文字体1", "1");
        map.insert("西文字体2","2");
        map.insert("西文字体3","3");
        map.insert("西文字体4","4");
        map.insert("西文字体5","5");
        map.insert("24点阵中文字体","6");
    }
    return map;
}

QStringList printer_label_postek::support_printer_type()
{
    return QStringList() << "POSTEK Q8/200";
}

double printer_label_postek::value_translate(double value, unitType value_unit, dpiType value_dpi)
{
    return valueTranslate(value, value_unit, value_dpi, dots, DPI200);
}

QString printer_label_postek::toPrint(const QString &in, int times, int height, int gap)
{
    QByteArray b;
    b += "N"+CR+LF;
    b += "ZB"+CR+LF;//set print direction
    b += "FS" + FILENAME+CR+LF;
    b += in;
    b += "FE"+CR+LF;
    b += QString("Q%1,%2").arg(height).arg(gap)+CR+LF;
    b += "FR" + FILENAME+CR+LF;
    b += QString("W%1").arg(times)+CR+LF;
    b += "FK" + FILENAME+CR+LF;

    return QString(b);
}


QString printer_label_postek::drawText(const QString &data, int x, int y, const QString &font, printer_label_postek::ROTATE rotate, int xbiger, int ybiger, const QString &textprinttype)
{
    QString str;
    foreach(const QString &d, data.split("\n", QString::SkipEmptyParts)) {
        str += QString("T%1,%2,%3,%4,%5,%6,%7,\"%8\"")
                .arg(x)
                .arg(y)
                .arg(rotate)
                .arg(font)

                .arg((xbiger%25) > 0 ? xbiger % 25 : 1)
                .arg((ybiger%25) > 0 ? ybiger % 25 : 1)
                .arg(textprinttype)
                .arg(d)+CR+LF;
        y += line_char_height * ybiger;
    }
    return str;
}


QString printer_label_postek::drawOneCode(const QString &data, int x, int y, const QString &onebartype, int height, printer_label_postek::ROTATE rotate, int wide_width,  int narrow_width, const QString &onebarprinttype)
{
    return QString("B%1,%2,%3,%4,%5,%6,%7,%8,\"%9\"")
            .arg(x)
            .arg(y)
            .arg(rotate)
            .arg(onebartype)

            .arg(narrow_width)
            .arg(wide_width)
            .arg(height)
            .arg(onebarprinttype)

            .arg(data)+CR+LF
            ;
}

QString printer_label_postek::drawTwoCode(const QString &data, int x, int y, const QString &bartype, int maxwidth, int maxheight, printer_label_postek::ROTATE rotate, int bigger, printer_label_postek::MODEL twobarmodel, printer_label_postek::RECOVERY recovery, printer_label_postek::MASK mask)
{
    return QString("b%1,%2,%3,%4,%5,o%6,r%7,m%8,g%9,s%10,\"%11\"")
            .arg(x)
            .arg(y)
            .arg(bartype)
            .arg(maxwidth)
            .arg(maxheight)

            .arg(rotate)
            .arg(bigger)
            .arg(twobarmodel)
            .arg(recovery)

            .arg(mask)
            .arg(data)+CR+LF
            ;
}

QString printer_label_postek::drawLineXOR(const QPoint &postion
                                          , int width
                                          , int height)
{
    //LE50,30,100,10 CR LF
    return QString("LE%1,%2,%3,%4")
            .arg(postion.x())// 直线 x
            .arg(postion.y())// 直线 y
            .arg(width)//直线宽
            .arg(height) + CR + LF;//直线高
}

QString printer_label_postek::drawLineOR(const QPoint &postion
                                         , int width
                                         , int height)
{
    //LO50,30,100,10 CR LF
    return QString("LO%1,%2,%3,%4")
            .arg(postion.x())// 直线 x
            .arg(postion.y())// 直线 y
            .arg(width)//直线宽
            .arg(height) + CR + LF;//直线高
}

QString printer_label_postek::drawLineSlash(const QPoint &beginpos
                                            ,  int bold
                                            ,  const QPoint &endpos)
{
    //LS50,30,10,100,80 CR LF
    return QString("LS%1,%2,%3,%4,%5")
            .arg(beginpos.x())// 直线 x
            .arg(beginpos.y())// 直线 y
            .arg(bold)//直线宽
            .arg(endpos.x())// 直线 x
            .arg(endpos.y()) + CR + LF;
}

QString printer_label_postek::drawLineWhite(const QPoint &postion
                                            , int width
                                            , int height)
{
    //LW100,20,5,110 CR LF
    return QString("LW%1,%2,%3,%4")
            .arg(postion.x())// 直线 x
            .arg(postion.y())// 直线 y
            .arg(width)//直线宽
            .arg(height) + CR + LF;//直线高
}

QString printer_label_postek::drawRectage(const QPoint &beginpos
                                          ,  int bold
                                          ,  const QPoint &endpos)
{
    //X50,120,5,250,150  CR LF
    return QString("X%1,%2,%3,%4,%5")
            .arg(beginpos.x())// 直线 x
            .arg(beginpos.y())// 直线 y
            .arg(bold)//直线宽
            .arg(endpos.x())// 直线 x
            .arg(endpos.y()) + CR + LF;
}
