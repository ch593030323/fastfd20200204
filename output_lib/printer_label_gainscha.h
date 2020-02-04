#ifndef PRINTER_LABEL_GAINSCHA_H
#define PRINTER_LABEL_GAINSCHA_H
//佳博标签打印

//toPrint(const QString &in,int piece=1, int page=1,const QSize &s=QSize(40,30),int density=15,int speed=4);
//指定打印的份数，指定每份打印的张数，设置卷纸的长宽，浓度，速度
//【0】 支持usb和串口
//【1】 默认单位为 mm
//【2】 串口波特率为9600
//【3】 中文支持需要下载字库到打印机 这儿窗口里需要加字体打名称，方便在软件里添加

#include <QMap>
#include <QSize>
#include <QRect>
#include <QString>
#include <QObject>
#include "printer_label.h"

//打印 单位mm
class printer_label_gainscha : public printer_label
{
public:
    enum ROTATE{
        ROTATE_0   = 0,
        ROTATE_90  = 1,
        ROTATE_180 = 2,
        ROTATE_270 = 3
    };

public:
    explicit printer_label_gainscha();
    virtual ~printer_label_gainscha();
    /////////////////
    virtual QString print_form(const QString &content, double width, double height, double gap, int printcount) {
        return toPrint(content, printcount, value_translate(height), value_translate(gap), value_translate(width));
    }
    virtual QString print_text(const QString &content, double x, double y, const QString &font, int xbigger, int ybigger) {
        return drawText(content, value_mm_2_dpi200(x), value_mm_2_dpi200(y), font, QSize(xbigger, ybigger));
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

    virtual QString toPrint(const QString &in,
                            int piece = 1,
                            float height = 32.48,
                            float gap = 0,
                            float width = 10.2,
                            const QString &unit="mm");//在拼接成的指令的基础上，补充并打印

    QString drawText(
            const QString &data = "BC 1"                //打印内容
            , int x = 20
            , int y = 20
            , const QString &font = "TSS24.BF2"           //打印字体

            , const QSize &size = QSize(1,1)            //倍高倍宽
            );

    QString drawOneCode(
            const QString &data = "0123456789"          //打印内容
            , int x = 20
            , int y = 20
            , const QString &bartype = "128M"           //一维码类型
            , int height = 41
            , printer_label_gainscha::ROTATE rotate = ROTATE_0     //[1]orientation,1 represents for portrait

            , int widebarwidth = 0                      //宽边宽
            , int narrowbarwidth = 0                    //窄边宽

            );

    QString drawTwoCode(
            const QString &data = "0123456789"          //打印内容
            , int x = 20
            , int y = 20
            );
};


#endif // PRINTER_LABEL_GAINSCHA_H
