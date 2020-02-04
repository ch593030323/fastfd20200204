#ifndef PRINTER_LABEL_TSC_TTP_244_PLUS_H
#define PRINTER_LABEL_TSC_TTP_244_PLUS_H

#include <QObject>
#include <QMap>
#include <QSize>
#include "printer_label.h"
//打印 单位mm
class printer_label_tsc_ttp_244_plus : public printer_label
{
public:
    enum ROTATE{
        ROTATE_0   = 0,
        ROTATE_90  = 1,
        ROTATE_180 = 2,
        ROTATE_270 = 3
    };
public:
    explicit printer_label_tsc_ttp_244_plus();
    virtual ~printer_label_tsc_ttp_244_plus();
    /////////////////
    // from printer_label
    virtual QString print_form(const QString &content, double width, double height, double gap, int printcount) {
        return toPrint(content, printcount, value_translate(height), value_translate(gap), value_translate(width));
    }
    // from printer_label
    virtual QString print_text(const QString &content, double x, double y, const QString &font, int xbigger, int ybigger) {
        return drawText(content, value_mm_2_dpi200(x), value_mm_2_dpi200(y), font, QSize(xbigger, ybigger));
    }
    // from printer_label
    virtual QString print_bar_code(const QString &content, double x, double y, double height, const QString &bar_type, int wide_width, int narrow_width) {
        return drawOneCode(content, value_mm_2_dpi200(x), value_mm_2_dpi200(y), bar_type, value_mm_2_dpi200(height), ROTATE_0, wide_width, narrow_width);
    }
    // from printer_label
    virtual QString print_qr_code(const QString &content, double x, double y) {
        return drawTwoCode(content, value_mm_2_dpi200(x), value_mm_2_dpi200(y));
    }
    // from printer_label
    virtual QString name();

    // from printer_label
    virtual QMap<QString, QString> print_bar_code_map();
    virtual  QMap<QString, QString> print_qr_code_map();
    virtual  QMap<QString, QString> print_text_map();

    // from printer_label
    virtual  QStringList support_printer_type();
    virtual double value_translate(double value, unitType value_unit = mm, dpiType value_dpi = DPI200);
    //from self
    virtual QByteArray lineEnd();
    virtual QByteArray GAP(int vgap);
    //////////////////~

    QString toPrint(const QString &in, int piece = 1, double height = 40, int gap = 0, double width = 10);//在拼接成的指令的基础上，补充并打印

    virtual QString drawText(
            const QString &data = "BC 1"                //打印内容
            , int x = 20
            , int y = 20
            , const QString &font = "FONT001"           //打印字体

            , const QSize &size = QSize(1,1)            //倍高倍宽
            );

    QString drawOneCode(
            const QString &data = "0123456789"          //打印内容
            , int x = 20
            , int y = 20
            , const QString &bartype = "128M"           //一维码类型
            , int height = 41
            , printer_label_tsc_ttp_244_plus::ROTATE rotate = ROTATE_0     //[1]orientation,1 represents for portrait

            , int widebarwidth = 0                      //宽边宽
            , int narrowbarwidth = 0                    //窄边宽

            );
    QString drawTwoCode(
            const QString &data = "0123456789"          //打印内容
            , int x = 20
            , int y = 20
            );
};

#endif // PRINTER_LABEL_TSC_TTP_244_PLUS_H
