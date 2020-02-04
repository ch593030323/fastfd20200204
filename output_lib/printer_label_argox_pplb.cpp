#include "printer_label_argox_pplb.h"
#include <QtDebug>
////注册
static printer_label_Virtual_Register<printer_label_argox_PPLB>
r(QList<QStandardItem*>() << new QStandardItem("printer_label_argox_PPLB"));
////
/**
1.支持的一维码格式
0       Code 128 UCC (shipping container code)
1       Code 128 auto
1A      Code 128 subset A
1B      Code 128 subset B
1C      Code 128 subset C
1E      UCC/EAN
2       Interleaved 2 of 5
2C      Interleaved 2 of 5 with check sum digit
2D      Interleaved 2 of 5 with human readable checkdigit
2G      German Postcode
2M      Matrix 2 of 5
2U      UPC Interleaved 2 of 5
3       Code 3 of 9
3C      Code 3 of 9 with check sum digit
9       Code 93
E30     EAN-13
E32     EAN-13 2 digit add-on
E35     EAN-13 5 digit add-on
E80     EAN-8
E82     EAN-8 2 digit add-on
E85     EAN-8 5 digit add-on
K       Codabar
P       Postnet
UA0     UPC-A
UA2     UPC-A 2 digit add-on
UA5     UPC-A 5 digit add-on
UE0     UPC-E
UE2     UPC-E 2 digit add-on
UE5     UPC-E 5 digit add-on
*/

/**
2.支持的二维码格式
A       Aztec-Specific Options
M       Maxi Code
P       PDF-417
D       Data Matrix
Q or QR QR Code
*/
static QMap<QString, QString> onebartype;
static QMap<QString, QString> twobartype;
static QMap<QString, QString> textfont;

printer_label_argox_PPLB::printer_label_argox_PPLB()
{
}

printer_label_argox_PPLB::~printer_label_argox_PPLB()
{
}

QString printer_label_argox_PPLB::name()
{
    return "argox_PPLB";
}

QMap<QString, QString> printer_label_argox_PPLB::print_bar_code_map()
{
    static QMap<QString, QString> map;
    if(map.isEmpty()) {
        map.insert("Code 128 UCC (shipping container code)", "0");
        map.insert("Code 128 auto)", "1");
        map.insert("Code 128 subset A", "1A");
        map.insert("Code 128 subset B", "1B");
        map.insert("Code 128 subset C", "1C");

        map.insert("UCC/EAN", "1E");
        map.insert("Interleaved 2 of 5", "2");
        map.insert("Interleaved 2 of 5 with check sum digit", "2C");
        map.insert("Interleaved 2 of 5 with human readable check digit", "2D");
        map.insert("German Postcode", "2G");

        map.insert("Matrix 2 of 5", "2M");
        map.insert("UPC Interleaved 2 of 5", "2U");
        map.insert("Code 3 of 9", "3");
        map.insert("Code 3 of 9 with check sum digit", "3C");
        map.insert("Code 93", "9");

        map.insert("EAN-13", "E30");              //EAN码的结构为12+p、7+p、p为校验位；长度不能小于12，7：小于不打印 大于截取
        map.insert("EAN-13 2 digit add-on", "E32");
        map.insert("EAN-13 5 digit add-on", "E35");
        map.insert("EAN-8", "E80");
        map.insert("EAN-8 2 digit add-on", "E82");

        map.insert("EAN-8 5 digit add-on", "E85");
        map.insert("Codabar", "K");
        map.insert("Postnet", "P");
        map.insert("UPC-A", "UA0");
        map.insert("UPC-A 2 digit add-on", "UA2");

        map.insert("UPC-A 5 digit add-on", "UA5");
        map.insert("UPC-E", "UE0");
        map.insert("UPC-E 2 digit add-on", "UE2");
        map.insert("UPC-E 5 digit add-on", "UE5");
    }
    return map;
}

QMap<QString, QString> printer_label_argox_PPLB::print_qr_code_map()
{
    static QMap<QString, QString> map;
    if(map.isEmpty()) {

        map.insert("Aztec-Specific Options", "A");
        map.insert("Maxi Code", "M");
        map.insert("PDF-417", "P");
        map.insert("Data Matrix", "D");

        map.insert("QR Code", "Q"/*Q or QR*/);
    }
    return map;
}

QMap<QString, QString> printer_label_argox_PPLB::print_text_map()
{
    static QMap<QString, QString> map;
    if(map.isEmpty()) {
        map.insert("font1", "1");
        map.insert("font2", "2");
        map.insert("font3", "3");
        map.insert("font4", "4");
        map.insert("font5", "5");

        map.insert("Tranditional Chinese font", "7");
        map.insert("Chinese font", "8");
        map.insert("Korean font1", "9");
        map.insert("Korean font2", "10");
        map.insert("Japanese font1", "11");
        map.insert("Japanese font2", "12");
    }
    return map;
}

QStringList printer_label_argox_PPLB::support_printer_type()
{
    return QStringList() << "ARGOX CP-2140" << "GODEX G500-U";
}

double printer_label_argox_PPLB::value_translate(double value, unitType value_unit, dpiType value_dpi)
{
    return valueTranslate(value, value_unit, value_dpi, dots, DPI200);
}

QString printer_label_argox_PPLB::toPrint(const QString &data, int pieces, int darkness)
{

    return QString("N\n%1\nD%2\n%3P%4\n")
            .arg("ZB")//set print direction
            .arg(darkness)// Darkness. Acceptable values ranges from 0 to 15. The default     darkness value is 8.
            .arg(data)//每行自动加的\n
            .arg(pieces)
            ;
}

QString printer_label_argox_PPLB::drawOneCode(const QString &data, int x, int y, const QString &bartype, int height, printer_label_argox_PPLB::ROTATE orientation, int narrow_width, int wide_width, const QString &isprint)
{
    return QString("B%1,%2,%3,%4,%5,%6,%7,%8,\"%9\"\n")
            .arg(x)
            .arg(y)
            .arg(orientation)
            .arg(bartype)

            .arg(narrow_width)
            .arg(wide_width)
            .arg(height)
            .arg(isprint)

            .arg(data)

            ;
}

QString printer_label_argox_PPLB::drawText(const QString &data, int x, int y, const QString &font, printer_label_argox_PPLB::ROTATE orientation, int horizontalscale, int verticalscale, const QString &texttype)
{
    QString str;
    foreach(const QString &d, data.split("\n", QString::SkipEmptyParts)) {
        str += QString("A%1,%2,%3,%4,%5,%6,%7,\"%8\"\n")
                .arg(x)
                .arg(y)
                .arg(orientation)
                .arg(font)

                .arg(horizontalscale)
                .arg(verticalscale)
                .arg(texttype)
                .arg(d)
                ;
        y += line_char_height * verticalscale;
    }
    return str;
}

QString printer_label_argox_PPLB::drawTwoCode(const QString &data, int x, int y, const QString &bartype)
{
    return QString("b%1,%2,%3,\"%4\"\n")
            .arg(x)
            .arg(y)
            .arg(bartype)
            .arg(data)
            ;
}

quint16 printer_label_argox_PPLB::get_vid()
{
    return 0x1664;
}

quint16 printer_label_argox_PPLB::get_pid()
{
    return 0x081b;
}
