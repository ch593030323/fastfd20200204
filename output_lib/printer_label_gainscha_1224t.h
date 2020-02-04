#ifndef PRINTER_LABEL_GAINSCHA_1224T_H
#define PRINTER_LABEL_GAINSCHA_1224T_H

#include "printer_label_tsc_ttp_244_plus.h"

class printer_label_gainscha_1224t : public printer_label_tsc_ttp_244_plus
{
public:
    printer_label_gainscha_1224t();
    virtual ~printer_label_gainscha_1224t();

    /////////////////
    virtual QString print_form(const QString &/*content*/, double /*width*/, double /*height*/, double /*gap*/, int /*printcount*/) {
        return "";
    }
    virtual QString print_text(const QString &content, double x, double y, const QString &font, int xbigger, int ybigger) {
        return drawText(content, value_mm_2_dpi200(x), value_mm_2_dpi200(y), font, QSize(xbigger, ybigger));
    }
    virtual QString print_bar_code(const QString &/*content*/, double /*x*/, double /*y*/, double /*height*/, const QString &/*bar_type*/, int /*wide_width*/, int /*narrow_width*/) {
        return "";
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
    ///
    virtual QString drawText(
            const QString &data = "BC 1"                //打印内容
            , int x = 20
            , int y = 20
            , const QString &font = "TSS24.BF2"           //打印字体

            , const QSize &size = QSize(1,1)            //倍高倍宽
            );
};
#endif // PRINTER_LABEL_GAINSCHA_1224T_H
