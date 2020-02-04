#ifndef PRINTER_LABEL_ARGOX_PPLA_H
#define PRINTER_LABEL_ARGOX_PPLA_H
//立象标签打印机
//【0】drawOneBar（） 这儿打y值不能为过小（如0；1） 否则会进纸多行
//【1】支持中文的话，同样需要依靠软件来实现下载
//【2】    //暂时不能用 QString drawTwoCode(

//打印机设置
//条码EXAMPLE   400      12
//              0
//              2
//              dots
//              200dpi
#include <QMap>
#include <QString>
#include <QMap>
#include <QSize>
#include "printer_label.h"

class printer_label_argox_PPLA : public printer_label
{
public:

    enum ROTATE{
        ROTATE_0   = 1,
        ROTATE_90  = 2,
        ROTATE_180 = 3,
        ROTATE_270 = 4
    };
public:
    printer_label_argox_PPLA();
    virtual ~printer_label_argox_PPLA();
    /////////////////
    virtual QString print_form(const QString &content, double width, double height, double /*gap*/, int printcount) {
        return toPrint(content, printcount, value_translate(width), value_translate(height));
    }
    virtual QString print_text(const QString &content, double x, double y, const QString &font, int xbigger, int ybigger) {
        return drawText(content, value_mm_2_dpi200(x), value_mm_2_dpi200(y), font, ROTATE_0, xbigger, ybigger);
    }
    virtual QString print_bar_code(const QString &content, double x, double y, double height, const QString &bar_type, int wide_width, int narrow_width) {
        return drawOneCode(content, value_mm_2_dpi200(x), value_mm_2_dpi200(y), bar_type, value_mm_2_dpi200(height), ROTATE_0, wide_width, narrow_width);
    }
    virtual QString print_qr_code(const QString &content, double x, double y) {
        return drawTwoCode(content, value_mm_2_dpi200(x), value_mm_2_dpi200(y));
    }
    virtual QString name();

    virtual QMap<QString, QString> print_bar_code_map();
    virtual  QMap<QString, QString> print_qr_code_map();
    virtual  QMap<QString, QString> print_text_map();

    virtual  QStringList support_printer_type();
    virtual double value_translate(double value, unitType value_unit = mm, dpiType value_dpi = DPI200);
    //////////////////~
    QByteArray toPrint(
            const QString &in
            , int piece = 1
            , int widthpixel = 1, int heightpixel = 1               //[1][1]Sets width and height pixel.
            );
    QString drawOneCode(
            const QString &data = "BC 1"                            //[nn..]data
            , int x = 0                                             //[0000]x coord.
            , int y = 20                                            //[0020]y coord.
            , const QString &bartype = "A"                          //[A]bar code type
            , int height = 0                                 //[000]bar code height ,000 stands for default height.

            , printer_label_argox_PPLA::ROTATE orientation = ROTATE_0     //[1]orientation,1 represents for portrait
            , int widebarwidth = 0, int narrowbarwidth = 0          //[5][2]wide bar width;narrow bar width

            );
    QString drawText(
            const QString &data = "BC 1"                            //[nn..]data
            , int x = 0                                             //[0000]x coord.
            , int y = 20                                            //[0020]y coord.
            , const QString &font = "2"                             //[1]font type

            , printer_label_argox_PPLA::ROTATE orientation = ROTATE_0     //[1]orientation,1 represents for portrait
            , int widthmutiplier = 1
            , int heightmultiplier = 1                              //
            , int barcodeheight = 1                                 //[1]bar code height is ignored in case of text

            );
    //暂时不能用
    QString drawTwoCode(
            const QString &data = "BC 1"
            , int x = 0                                             //[0000]x coord.
            , int y = 20                                            //[0020]y coord.
            , const QString &bartype = "W1d"                        //[WID]bar code type

            , int value = 0                                         //[000] ineffective; must bu numeric
            , printer_label_argox_PPLA::ROTATE orientation = ROTATE_0     //[1]orientation,1 represents for portrait
            , int verticalmultiplier = 4                            //[4]
            , int horizontalmultiplier = 4                          //[4]
            );
};


#endif // PRINTER_LABEL_ARGOX_PPLA_H
