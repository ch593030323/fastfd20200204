#ifndef PUBLIC_MAINPRINTER_H
#define PUBLIC_MAINPRINTER_H


#include <QObject>
#include <QStandardItemModel>
#include <QMap>
#include "printer_0.h"
#include "lds_query_hddpos.h"

/*
    public_mainprinter printer;
    QByteArray b;
    b += printer.drawText("1B40 1B6101 1C5701", "交 班 对 账\n");//清除缓存 居中 2x2x
    b += printer.drawText("1B40 1B6101 1C5700", "交 班 对 账\n");//清除缓存 居中 1x1x
    b += printer.drawText("1B40", "");//清除缓存

    b += printer.drawText("0a", "Left", Qt::AlignLeft, 20);
    b += printer.drawText("", "Left", Qt::AlignLeft, 20);
    b += printer.drawText("0a", "LeftLeft", Qt::AlignLeft, 20);
    b += printer.drawText("", "LeftLeftLeft", Qt::AlignLeft, 20);

    b += printer.drawText("0a", "Center", Qt::AlignCenter, 20);
    b += printer.drawText("", "Center", Qt::AlignCenter, 20);
    b += printer.drawText("0a", "CenterCenter", Qt::AlignCenter, 20);
    b += printer.drawText("", "CenterCenterCenter", Qt::AlignCenter, 20);

    b += printer.drawText("0a", "Right", Qt::AlignRight, 20);
    b += printer.drawText("", "Right", Qt::AlignRight, 20);
    b += printer.drawText("0a", "RightRight", Qt::AlignRight, 20);
    b += printer.drawText("", "RightRightRight", Qt::AlignRight, 20);

    b += printer.drawTextEnd("1B6405 1D5601", "");//走纸5x， 切纸

    printer.asynchPrint(printer.setPrintCount(b, 1), "192.168.1.172:9100");
*/

/*
    printer->asynchPrint(printer->setPrintCount(printer->drawLogo("1B40 1B6101", "userdata/settings/image/logo.png"), 1),"/dev/ttyS0" );//清除缓存 居中 pixmap路径
*/

/*
    printer->asynchPrint(printer->setPrintCount(printer->drawQR_Code("","www.baidu.com"), 1),"/dev/ttyS0" );
*/

/*
    printer->asynchPrint(printer->setPrintCount(printer->drawoneBarcode("","123456789"), 1),"/dev/ttyS0" );

*/
class public_mainprinter : public QObject
{
    Q_OBJECT
public:
    explicit public_mainprinter(QObject *parent = 0);
    ~public_mainprinter();
    pieceIns drawText(const QByteArray &hex=QByteArray(""), const QString &text=QString(""), int textwidth=-1,Qt::Alignment textflag=Qt::AlignLeft);//获取文本，打印指令
    blockIns drawText(const QByteArray &hex, const QString &text, int textwidth, Qt::Alignment textflag, int blanks, const QString &text2);
    blockIns drawText(const QByteArray &hex, const QString &text, const QByteArray &hex2, int textwidth=-1,Qt::Alignment textflag=Qt::AlignLeft);
    blockIns drawTextEnd(const QByteArray &hex, const QString &text);
    pieceIns drawLogo(const QByteArray &hex, const QString &logopath);//获取logo，打印指令
    pieceIns drawQR_Code(const QByteArray &hex/*=QByteArray("1B40 1B6101")*/, const QString &code);//获取二维码，打印指令
    pieceIns drawoneBarcode(const QByteArray &hex/*=QByteArray("1B40 1B6101")*/, const QString &code);//获取二维码，打印指令

    blockIns setPrintCount(const blockIns &text, int printcount=1);//获取打印次数，打印指令

//    bool asynchPrint(const blockIns &printIns, const QString &path, bool isblock=false);//异步打印
//    bool asynchPrintWithoutresolve(blockIns printIns, const QString &path, bool isblock=false);//异步打印
//    bool synchPrint(const blockIns &printIns, const QString &path);//同步打印
//    bool synchPrintWithoutresolve(const blockIns &printIns, const QString &path);//同步打印

    void write(const blockIns &printIns);
    void write(const blockIns &printIns, const QString &path);
    void writeHex(const QByteArray &hex, const QString &path);
public slots:
    void sendError(const QString &err, const QString &title);
//signals:
//    void errorString(const QString &err, const QString &title);
};
#endif // PUBLIC_MAINPRINTER_H
