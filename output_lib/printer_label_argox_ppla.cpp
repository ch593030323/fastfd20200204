#include "printer_label_argox_ppla.h"
////注册
static printer_label_Virtual_Register<printer_label_argox_PPLA>
r(QList<QStandardItem*>() << new QStandardItem("printer_label_argox_PPLA"));
////

/**
1.支持的一维码格式
Code 39、
UPC-A、
UPC-E、
Code 128 subset
A/B/C、
EAN-13、
EAN-8、
HBIC、
Codabar、
Plessey、
UPC2、
UPC5、
Code 93、
Postnet、
UCC/EAN-128、
UCC/EAN-128 K-MART、
UCC/EAN-128 Random
weight、
Telepen、
FIM、
Interleaved 2 of 5
(Standard/with modulo 10 checksum/ with human
readable check digit/ with modulo 10 checksum &
shipping bearer bars) 、
GS1 Data bar (RSS)
*/

/**
2.支持的二维码格式
MaxiCode、PDF417、Data Matrix (ECC 200
only) 、QR code、Composite Codes、
Aztec Barcode 、Micro PDF417
*/

/**
3.软件字型
需要通过font utility 下载字型
*/

/**
4.通信
Argox CP
打印机系列提供
9
针电子工业协会
(EIA)
标准的
RS-232
接口、
USB
接口、并口和以太网口。不同的接口选项适合
不同的用途。

CP-2140, CP-2140Z, CP-3140L
：并口埠
, USB
口和串口
CP-2140E, CP-3140E, CP-3140LE
：以太网口
, USB
口和串口
*/

/**
5.
*/
static QMap<QString, QString> onebartype;
static QMap<QString, QString> twobartype;
static QMap<QString, QString> textfont;

static QByteArray SOH = QByteArray::fromHex(QByteArray().append("01"));
static QByteArray STX = QByteArray::fromHex(QByteArray().append("02"));
static QByteArray CR = QByteArray::fromHex(QByteArray().append("0a"));
static QByteArray FILENAME = QByteArray::fromHex(QByteArray().append("3132330d0a"));

printer_label_argox_PPLA::printer_label_argox_PPLA()
{
}

printer_label_argox_PPLA::~printer_label_argox_PPLA()
{
}

QString printer_label_argox_PPLA::name()
{
    return "argox_PPLA";
}

QMap<QString, QString> printer_label_argox_PPLA::print_bar_code_map()
{
    static QMap<QString,  QString> map;
    if(map.isEmpty()) {
        map.insert("3 OF 9", "A");
        map.insert("UPC-A", "B");
        map.insert("UPC-E", "C");
        map.insert("INTERLEADVED 2 OF 5", "D");
        map.insert("CODE 128", "E");

        map.insert("EAN-13", "F");
        map.insert("EAN-8", "G");
        map.insert("HBIC", "H");
        map.insert("CODA BAR", "I");
        map.insert("WITH CHECKSUM", "J");

        map.insert("PLESSEY", "K");
        map.insert("125 WITH CHECKSUM & BEARER", "L");
        map.insert("UPC2", "M");
        map.insert("UPC5", "N");
        map.insert("CODE 93", "O");

        map.insert("POSTNET", "P");
        map.insert("UCC/EAN 128", "Q");
        map.insert("UCC/EAN 128K", "R");
        map.insert("UCC/EAN CODE 128 RANDOM WEIGHT", "S");
        map.insert("TELEPEN", "T");

        map.insert("FIM", "V");
    }
    return map;
}

QMap<QString, QString> printer_label_argox_PPLA::print_qr_code_map()
{
    static QMap<QString,  QString> map;
    if(map.isEmpty()) {
        map.insert("MAXICODE", "U");
        map.insert("DATAMATRIX", "W");
        map.insert("QR<AUTO FORMAT>", "W1d");
        map.insert("QR<MANUAL FORMAT>", "W1D");

        map.insert("RSS", "W1k");
        map.insert("AZTEC BARCODE(variabel length)", "W1f");
        map.insert("AZTEC BARCODE(specified length/byte count)", "W1F");
        map.insert("MIRCO PDF417", "W1z");

        map.insert("PDF-417", "z");
    }
    return map;
}

QMap<QString, QString> printer_label_argox_PPLA::print_text_map()
{
    static QMap<QString,  QString> map;
    if(map.isEmpty()) {
        map.insert("font0", "0");
        map.insert("font1", "1");
        map.insert("font2", "2");
        map.insert("font3", "3");

        map.insert("font4", "4");
        map.insert("font6", "6");
        map.insert("font7", "7");
        map.insert("font8", "8");
    }
    return map;
}

QStringList printer_label_argox_PPLA::support_printer_type()
{
    return QStringList() << QObject::tr("SC2100(商超专用)");
}

double printer_label_argox_PPLA::value_translate(double value, unitType value_unit, dpiType value_dpi)
{
    return valueTranslate(value, value_unit, value_dpi, dots, DPI200);
}

QByteArray printer_label_argox_PPLA::toPrint(const QString &in, int piece, int widthpixel, int heightpixel)
{
    QString b = STX + "L\n";    //Enter label formatting state
    b += QString("D%1%2\n")     //[1][1]sets width and height pixel
            .arg(widthpixel % 10)
            .arg(heightpixel % 10);
    b += in;
    b += "E\n";                 //ends the job and exit from label formatting mode

    QByteArray bt;
    while(piece >= 1 && --piece) {
        bt += b;
    }
    return bt;
}

QString printer_label_argox_PPLA::drawOneCode(const QString &data, int x, int y, const QString &bartype, int height, printer_label_argox_PPLA::ROTATE orientation, int widebarwidth, int narrowbarwidth)
{
    return QString("%1%2%3%4%5%6%7%8\n")
            .arg(orientation)
            .arg(bartype)
            .arg(widebarwidth % 10)
            .arg(narrowbarwidth % 10)

            .arg(QString().sprintf("%03d",height % 1000))
            .arg(QString().sprintf("%04d",y % 10000))
            .arg(QString().sprintf("%04d",x % 10000))
            .arg(data);
}

QString printer_label_argox_PPLA::drawText(const QString &data, int x, int y, const QString &font, printer_label_argox_PPLA::ROTATE orientation, int widthmutiplier, int heightmultiplier, int barcodeheight)
{
    QString str;
    foreach(const QString &d, data.split("\n", QString::SkipEmptyParts)) {
        str += QString("%1%2%3%4%5%6%7%8\n")
                .arg(orientation)
                .arg(font)
                .arg(widthmutiplier % 10)
                .arg(heightmultiplier % 10)

                .arg(QString().sprintf("%03d",barcodeheight % 1000))
                .arg(QString().sprintf("%04d",y % 10000))
                .arg(QString().sprintf("%04d",x % 10000))
                .arg(d)
                ;
        y += line_char_height * heightmultiplier;
    }
    return str;
}

QString printer_label_argox_PPLA::drawTwoCode(const QString &data, int x, int y, const QString &bartype, int value, printer_label_argox_PPLA::ROTATE orientation, int verticalmultiplier, int horizontalmultiplier)
{
    return QString("%1%2%3%4%5%6%7%8\n")
            .arg(orientation)
            .arg(bartype)
            .arg(horizontalmultiplier % 10)
            .arg(verticalmultiplier % 10)

            .arg(QString().sprintf("%03d",value % 1000))
            .arg(QString().sprintf("%04d",y % 10000))
            .arg(QString().sprintf("%04d",x % 10000))
            .arg(data)

            ;
}
