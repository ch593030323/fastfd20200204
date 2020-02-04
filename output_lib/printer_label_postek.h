#ifndef PRINTER_LABEL_POSTEK_H
#define PRINTER_LABEL_POSTEK_H

//博思得 标签打印机
//【0】 codetype 1 2 3 4 5 6 A - Z
//     字体类型：西文字体1 西文字体2 西文字体3 西文字体4 西文字体5 中文字体[6]
//【1】 textprinttype  N R
//     打印类型    白底黑字 黑底白字
//【3】 onebartype 0 1 1A 1B 1C 1E 2 2C 2D 2G 2M 2U 3 3C 3E 3F 9 E30 E32 E35 E80 E82 E-85
//                K P UA0 UA2 UE0 UE2 UE5
//     一维码类型  这儿暂时用E80
//【4】 onebarprinttype N B
//     一维码打印类型     不打印条码下面打认可识别文字 打印条码下面的人可识别文字
//【5】 twobarmodel 0 1 2 3 4
//     二维码模式选择 数字模式 数字字母模式 字节模式0-255 中国汉字模式 混合模式
//【6】 一维码只能识别ABCD四个字符，其他的无法识别
//【7】 串口波特率设置 9600 才会起效
//【8】 纸张长度一般定义为102，宽度32（包含了gap）
#include <QMap>
#include <QSize>
#include <QPoint>
#include <QString>
#include "printer_label.h"

class  printer_label_postek : public printer_label
{
public:
    enum ROTATE{
        ROTATE_0   = 0,
        ROTATE_90  = 1,
        ROTATE_180 = 2,
        ROTATE_270 = 3
    };
    enum RECOVERY{
        RECOVERY_L  = 0,
        RECOVERY_M  = 1,
        RECOVERY_Q1 = 2,
        RECOVERY_H1 = 3
    };
    enum MASK{
        MASK_000  = 0,
        MASK_001  = 1,
        MASK_010  = 2,
        MASK_011  = 3,

        MASK_100  = 4,
        MASK_101  = 5,
        MASK_110  = 6,
        MASK_111  = 7,

        MASK_AUTO = 8
    };

    enum MODEL{
        MODEL_Digital  = 0,
        MODEL_Digital_Letter =  1,
        MODEL_Byte = 2,
        MODEL_Chinese = 3,
        MODEL_Mixed = 4

    };
    //        QStringList

public:
    printer_label_postek();
    virtual ~printer_label_postek();
    /////////////////
    virtual QString print_form(const QString &content, double /*width*/, double height, double gap, int printcount) {
        return toPrint(content, printcount, value_translate(height), value_translate(gap));
    }
    virtual QString print_text(const QString &content, double x, double y, const QString &font, int xbigger, int ybigger) {
        return drawText(content, value_mm_2_dpi200(x), value_mm_2_dpi200(y), font, ROTATE_0, xbigger, ybigger);
    }
    virtual QString print_bar_code(const QString &content, double x, double y, double height, const QString &bar_type, int wide_width, int narrow_width) {
        return drawOneCode(content, value_mm_2_dpi200(x), value_mm_2_dpi200(y), bar_type,value_mm_2_dpi200 (height), ROTATE_0, wide_width, narrow_width);
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
    QString toPrint(const QString &in                               //内容
                    , int times = 1                                 //次数
            , int height = 0                                        //每单高度
            , int gap = 0);                                         //在拼接成的指令的基础上，补充并打印

    QString drawText(
            const QString &data = "BC 1"                            //获取打印文字指令
            , int x = 280
            , int y = 180
            , const QString &font = "6"                             //字体选择

            , ROTATE rotate = ROTATE_0                            //旋转
            , int xbiger = 1                                        //水平放大倍数1-24
            , int ybiger = 1                                        //垂直放大倍数1-24
            , const QString &textprinttype = "N"                    //文本是否反相，N表示不反相；R表示反相。
            );
    QString drawOneCode(
            const QString &data = "0123456789"                      //获取打印文字指令
            , int x = 20
            , int y = 20
            , const QString &onebartype = "E80"                     //一维码类型
            , int height = 41                                       //高度

            , ROTATE rotate = ROTATE_0                              //旋转
            , int wide_width = 3                                    //宽边宽
            , int narrow_width = 2                                  //窄边宽
            , const QString &onebarprinttype = "B"                  //一维码格式 B表示人可识别；N表示人不可识别
            );

    QString drawTwoCode(
            const QString &data = "ABCabc12345"                      //获取打印文字指令
            , int x = 200
            , int y = 200
            , const QString &bartype = "QR"                         //二维码类型

            , int maxwidth = 0                                      //最大宽度
            , int maxheight = 0                                     //最大高度
            , ROTATE rotate = ROTATE_0             //旋转
            , int bigger = 5                                        //放大倍数
            , MODEL twobarmodel = MODEL_Byte         //字节模式
            , RECOVERY recovery = RECOVERY_L         //纠错模式L
            , MASK mask = MASK_000                   //掩码000
            );
    QString drawLineXOR(const QPoint &postion = QPoint(50, 30)      //坐标
            , int width = 100                                       //宽度
            , int height = 10);                                     //高度

    QString drawLineOR(const QPoint &postion = QPoint(50, 30)
            , int width = 100
            , int height = 10);
    QString drawLineSlash(const QPoint &beginpos = QPoint(50, 30)
            , int bold = 10
            , const QPoint &endpos = QPoint(100, 80));
    QString drawLineWhite(const QPoint &postion  = QPoint(100, 20)
            , int width = 5
            , int height = 110 );
    QString drawRectage(const QPoint &beginpos  = QPoint(50, 120)
            , int bold = 5
            , const QPoint &endpos  = QPoint(250, 150));
    /* O[D,P,L,C,N]<CR><LF>
    设置打印机为热敏印状态
    设置打印机为连续送纸状态
    设置打印机为打印一张标签后，暂停等待用户确定再打印下一张标签
    设置打印机为安装切刀状态
    设置打印机为安装剥纸器
*/
};
#endif // PRINTER_LABEL_POSTEK_H
