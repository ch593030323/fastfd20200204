#ifndef COMMONHEADER_H
#define COMMONHEADER_H

/*
需要做的外设 至少包括一下：
计价秤     凯丰 凯士
条码秤     托利多 大华   凯士条码秤
A4打印机   爱普生 630K  富士通
条码打印机  立象214    postek    tsc244
客显       1种
盘点机     奥巴马
扫描枪     测试项  平台不停的读，若是黑屏就做串口

【软件型号】                   【软件类型】                 【说明】
SC5890(商超专用)               58打印机                    包含串口，usb，网口

HDD80(商超专用)                80打印机 POS80              包含串口，usb，网口

凯丰(商超专用)                  计价秤                     包含串口(交叉线)一公一公；清除+6键->设置模式 4-9600-p(4是应答模式，累计切换)；

凯士PR PLUS                   计价秤                      包含串口(交叉线)一公一母；9600-8-1-n ；长按“.”切换通信状态(start 开始，stop 关闭)；数据格式：0A0DXXXXXX2020XXXXXX2020XXXXXXX；c键->设置模式(1不发送，2连续，3稳定发送)

SC9540(商超专用)               条码阅读器 RS232 阅读器      包含usb

?                            客显                      包含串口

上海大华 TM-15H                条码秤                     包含串口(暂时不用)和网口，0称重；1计件；2定重             ；测试port 4001

?托利多                       条码秤

凯士 CL5200                    条码秤                    包含串口，并口，usb，网口等，暂时用网口；1称重；2数量；3个   ；测试port 20304

postek Q8/200                 条码打印机                 包含串口(9600)和usb !高度必须指定才能正确打印dots；对高度要求很高，可以说没有自带的自校验，无论是条码和吊牌都需要严格设定长宽

TSC TTP-244 PLUS              条码打印机                 包含串口(9600)和usb，tsc打打印指令和佳博的类似，!高度和宽度(为整个打印纸的宽度)，单位为mm

SC2100(商超专用)               条码打印机                 包含串口(9600)和并口(暂时不用)，PPLA打印指令，中文暂不支持支持

ARGOX CP-2140                 条码打印机                 包括串口(9600)和并口(暂时不用)和usb，PPLB打印指令，

? 盘点机(暂时不做)
? A4等其他打印机(暂时不做)
*/

/*
？ 店名号 从配置文件里读取，这儿默认为01；以后再添加
  店名号改成 标识符
*/

/*
1.打印机类型和字体，一维码的字体打匹配通过打印机类型来识别，添加打印机务必添加对于打字体
2.字体软件下载打需要自定义，没找到，则直接将key写入配置文件
*/

/*
   tsc_ttp_244_plus字体需要从window  DialogTool 下载点阵字体
*/

/*
小票打印机                normal_print
计价秤                   pricecount_scale
电子秤                   barcode_scale
顾客显示器                clientdisplay_led
开钱箱                   normal_print（group）
条码秤                   label_scale
标价签打印机              label_print
扫描枪                   scan_gun
磁卡读卡器                magnetic_reader
*/
/*
外设设置中的标价签按打印设置，打印份数也会对标价签起作用，切换模式时注意
*/

/*条码秤标志符为2位*/
/*凯士地址：上海凯士电子有限公司*//*松江卖新路448号*//*联系电话021-57680559*/

//blockIns :打印指令
//pieceIns :first is writeHex();second is write();

//友中衡 置零是不能超过量程的-+2%，否则会置零失败
/*!
1.点菜宝：服务端  -->串口通信-->基站   点菜宝
2.点菜宝每隔15s听一下
3.发送失败后，再继续发送3次
科城打印机 21 5665 1313 +拨601
*/
/**
GP-3120TN
VIDPID:0471&0017

*/
#include <QString>
#include <QPair>
#include <QQueue>
#include <QObject>
#include <QSize>
#include <QPoint>
#include <QRect>
#include <QStandardItemModel>
#include "printer_0.h"


class Printer_POS;
class Printer_Normal;
class  CommonHeader
{
public:
    explicit CommonHeader();
    ~CommonHeader();

    //normal_print
    static QStringList creat_normal_printpathtypes();


    //clientdisplay_led
    static QStringList creat_clientdisplay_ledtypes();

    //label_print
    static QStringList creat_printtypes();
    static QStandardItemModel &creat_print_vidpidmodel();
    static QStringList creat_printpathtypes();
    static QMap<int, QString> creat_baudrates();
    static QString getAboutLabelPrinter();
    //barcode scale
    static QStringList creat_barcode_scaletypes();
    static QMap<int, QString> creat_barcode_scale_weightype(const QString &type);
    static QString creat_barcode_scale_netport_init(const QString &type);
    static QStringList creat_barcode_scalebarcodetype(const QString &type);
    static bool barcode_scale_resolve(//解析 条码秤一段条码
            QString &barcode, QString &price, QString &weight,
            QString &amount,
            QString code);
private:
    //normal_print
    static QStringList normal_printtypes;
    static QStringList normal_print_instypes;
    static QStringList normal_printpathtypes;

    //pricecout_scale
    static QStringList pricecout_scaletypes;

    //clientdisplay_led
    static QStringList clientdisplay_ledtypes;

    //label_print
    static QStringList printtypes;
    static QStringList printpathtypes;
    static QStandardItemModel label_vidpid_model;

    //barcode scale
    static QStringList barcode_scaletypes;
    static QMap<int, QString> barcode_scale_weightype;

};

#endif // COMMONHEADER_H
