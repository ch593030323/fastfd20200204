#ifndef PRINTER_LABEL_ARGOX_H
#define PRINTER_LABEL_ARGOX_H

//立象标签打印机
//【0】drawOneBar（） 这儿打y值不能为过小（如0；1） 否则会进纸多行
//【1】支持中文的话，同样需要依靠软件来实现下载
#include <QMap>
#include <QString>
#include "printer_label.h"

class  printer_label_argox : public printer_label
{
public:
    enum printer_label_argox_ROTATE{
        ROTATE_0   = 1,
        ROTATE_90  = 2,
        ROTATE_180 = 3,
        ROTATE_270 = 4
    };
public:
    printer_label_argox();
    virtual ~printer_label_argox();
    /////////////////
    virtual QString print_form(const QString &content, double width, double height, double /*gap*/, int printcount) {
        return toprint(content, printcount, value_translate(width), value_translate(height));
    }
    virtual QString print_text(const QString &/*content*/, double /*x*/, double /*y*/, const QString &/*font*/, int /*xbigger*/, int /*ybigger*/) {
        return "";
    }
    virtual QString print_bar_code(const QString &content, double x, double y, double height, const QString &bar_type, int wide_width, int narrow_width) {
        return DrawBar(content, ROTATE_0, bar_type, wide_width, narrow_width, value_mm_2_dpi200(height), value_mm_2_dpi200(y), value_mm_2_dpi200(x));
    }
    virtual QString print_qr_code(const QString &/*content*/, double /*x*/, double /*y*/) {
        return "";
    }
    virtual QString name();

    virtual QMap<QString, QString> print_bar_code_map();
    virtual  QMap<QString, QString> print_qr_code_map();
    virtual  QMap<QString, QString> print_text_map();

    virtual  QStringList support_printer_type();
    virtual double value_translate(double value, unitType value_unit = mm, dpiType value_dpi = DPI200);
    //////////////////~

    QByteArray toprint(const QString &in, int piece, int widthpixel, int heightpixel); //[1][1]Sets width and height pixel.
    inline QString drawOneBar(QString data, int x, int y, QString bartype) {
        return DrawBar(data
                       , ROTATE_0
                       , bartype
                       , 0, 0
                       , 0
                       , y
                       , x);
    }

    QMap<QString, QString> bartype_map;

    //protected:
    /*打印条码，内容只有条码和条码内容*/
    QString DrawBar(QString data = "BC 1"                              //[nn..]data
            , printer_label_argox_ROTATE orientation = ROTATE_0              //[1]orientation,1 represents for portrait
            , QString bartype = "A"                                 //[A]bar code type
            , int widebarwidth = 0
            , int narrowbarwidth = 0          //[5][2]wide bar width;narrow bar width
            , int barcodeheight = 0                                 //[000]bar code height ,000 stands for default height.
            , int y = 20                                            //[0020]y coord.
            , int x = 0                                             //[0020]x coord.

            );
};

#endif // PRINTER_LABEL_ARGOX_H
