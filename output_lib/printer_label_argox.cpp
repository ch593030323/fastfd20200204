#include "printer_label_argox.h"
////注册
static printer_label_Virtual_Register<printer_label_argox>
r(QList<QStandardItem*>() << new QStandardItem("printer_label_argox"));
////

//立象 标签打印机
static QByteArray SOH = QByteArray::fromHex(QByteArray().append("01"));
static QByteArray STX = QByteArray::fromHex(QByteArray().append("02"));
static QByteArray CR = QByteArray::fromHex(QByteArray().append("0a"));
static QByteArray FILENAME = QByteArray::fromHex(QByteArray().append("3132330d0a"));

printer_label_argox::printer_label_argox()
{
    bartype_map = print_bar_code_map();

//    凯士条码秤
}


printer_label_argox::~printer_label_argox()
{
}

QString printer_label_argox::name()
{
    return "ARGOX";
}

QMap<QString, QString> printer_label_argox::print_bar_code_map()
{
    static QMap<QString, QString> map;
    if(!map.isEmpty()) return map;

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
    map.insert("MAXICODE", "U");
    map.insert("DATAMATRIX", "W");
    map.insert("QR<AUTO FORMAT>", "W1d");
    map.insert("QR<MANUAL FORMAT>", "W1D");

    map.insert("AZTEC BARCODE", "W1f");
    map.insert("RSS", "W1K");
    map.insert("MIRCO PDF41", "W1z");
    map.insert("PDF-417", "Z");

    return map;
 }

QMap<QString, QString> printer_label_argox::print_qr_code_map()
{
    static QMap<QString, QString> map;
    return map;
}

QMap<QString, QString> printer_label_argox::print_text_map()
{
    static QMap<QString, QString> map;
    return map;
}

QStringList printer_label_argox::support_printer_type()
{
    return QStringList();
}

double printer_label_argox::value_translate(double value, unitType value_unit, dpiType value_dpi)
{
    return valueTranslate(value, value_unit, value_dpi, mm, DPI200);
//    return value;
}

QString printer_label_argox::DrawBar(QString data
                              ,  printer_label_argox_ROTATE orientation
                              ,  QString bartype, int widebarwidth
                              ,  int narrowbarwidth
                              ,  int barcodeheight
                              ,  int y
                              ,  int x
                               )
{
    return QString("%1%2%3%4%5%6%7%8\n")
            .arg(orientation)
            .arg(bartype)
            .arg(widebarwidth % 10)
            .arg(narrowbarwidth % 10)

            .arg(QString().sprintf("%03d",barcodeheight % 1000))
            .arg(QString().sprintf("%04d",y % 10000))
            .arg(QString().sprintf("%04d",x % 10000))
            .arg(data);
}

QByteArray printer_label_argox::toprint(const QString &in, int piece, int widthpixel,  int heightpixel)
{
    QByteArray b = STX + "L\n"; //Enter label formatting state
    b += QByteArray().append(QString("D%1%2\n")  //[1][1]sets width and height pixel
            .arg(widthpixel % 10)
            .arg(heightpixel % 10)
                           );
    b += QByteArray().append(in);
    b += "E\n";//ends the job and exit from label formatting mode

    QByteArray bt;
    while(piece >= 1 && --piece) {
        bt += b;
    }
    return bt;
}
