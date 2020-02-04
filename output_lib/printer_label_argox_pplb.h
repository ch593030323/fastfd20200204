#ifndef PRINTER_LABEL_ARGOX_PPLB_H
#define PRINTER_LABEL_ARGOX_PPLB_H
//【0】 字体的倍高倍宽支持字母 如0-9； A(10)-O(24)
//标签打印例子：100 39 0 2 mm 200dpi
#include <QMap>
#include <QString>
#include <QSize>
#include <stdio.h>
#include "printer_label.h"

class printer_label_argox_PPLB : public printer_label
{
public:
    enum ROTATE{
        ROTATE_0   = 0,
        ROTATE_90  = 1,
        ROTATE_180 = 2,
        ROTATE_270 = 3
    };
    static quint16 get_vid();
    static quint16 get_pid();

    printer_label_argox_PPLB();
    virtual ~printer_label_argox_PPLB();
    /////////////////
    virtual QString print_form(const QString &content, double /*width*/, double /*height*/, double /*gap*/, int printcount) {
        return toPrint(content, printcount);
    }
    virtual QString print_text(const QString &content, double x, double y, const QString &font, int xbigger, int ybigger) {
        return drawText(content, value_mm_2_dpi200(x), value_mm_2_dpi200(y), font, ROTATE_0, xbigger, ybigger);
    }
    virtual QString print_bar_code(const QString &content, double x, double y, double height, const QString &bar_type, int wide_width, int narrow_width) {
        return drawOneCode(content, value_mm_2_dpi200(x), value_mm_2_dpi200(y), bar_type, value_mm_2_dpi200(height), ROTATE_0, narrow_width, wide_width);
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
    //    N\n
    //    B20,20,0,E80,3,3,41,B,"0123459"\n
    //    P1\n

    QString toPrint(const QString &data, int pieces = 1, int darkness = 8);                //打印内容， 打印次数
    QString drawOneCode(
            const QString &data = "0123459"
            , int x = 20
            , int y = 20
            , const QString &bartype = "E80"                              //一维码类型
            , int height = 41                                             //条码高度

            , printer_label_argox_PPLB::ROTATE orientation = ROTATE_0           //方向指示
            , int narrow_width =3                                         //窄边宽
            , int wide_width = 3                                          //宽边宽
            , const QString &isprint = "B"                                //其实就是是否打印：N is not text is printed;B is readable is printed
            );
    QString drawText(
            const QString &data = "BC 1"
            , int x = 20
            , int y = 20
            , const QString &font = "966"                                 // 1 - 5表示字体的大小；a-z表示需要自己下载字体；966是通过软件下载好的字体

            , printer_label_argox_PPLB::ROTATE orientation = ROTATE_0           //方向指示
            , int horizontalscale = 1                                     //水平放大倍数
            , int verticalscale = 1                                       //垂直放大倍数
            , const QString &texttype = "N"                               //文本是否反相，N表示不反相；R表示反相。
            );
    QString drawTwoCode(
            const QString &data = "BC 1"
            , int x = 20
            , int y = 20
            , const QString &bartype = "Q"                                //这儿暂时只支持Q
            );

};
#endif // PRINTER_LABEL_ARGOX_PPLB_H
