#ifndef PRINTER_0_H
#define PRINTER_0_H

#include <QString>
#include <QStandardItem>
#include <QQueue>
#include <QEventLoop>
#include <QProcess>
#include <QFile>
#include <QMap>
#include <QVariant>
#include <QThread>

typedef QPair<QByteArray, QByteArray> pieceIns;
typedef QQueue<pieceIns> blockIns;

class printer_0
{
public:
    printer_0();

    static QString  trans_CODEC2UTF8(const QByteArray &in, const QByteArray &codec);
    //gbk to utf-8
    static QString  trans_GBK2UFT8(const QByteArray &in);

    static QByteArray  trans_u2code(const QString &in, const QByteArray &codec);
    //utf-8 to gbk
    static QByteArray  trans_UTF82GBK(const QString &in);
    //
    static QByteArray trans_u2codepage(const QString &str, const QString &languageKey, const QByteArray &defCode);

//    static QByteArray  trans_u2codepage(const QString &u_data, QByteArray (*codepage_trans)(const QString &, const QString &), const QString &codepage_language);

    static QStringList  f_scan_usbPort(bool firstIsEmpty = false);
    static bool usbPortIsValid(const QString &port);
    static const QString POS80;

    static QStringList  f_scan_ttyPort(bool firstIsEmpty = false);
    static bool comPortIsValid(const QString &port);
    //汉字转区位码 : utf-8 to 区位码
    //只支持汉字不包括 ：阿拉伯数字和字母
    //字母 只包括ascii下的 !到 } 之间打字符，包括 数字，大小写字母，以及大部分符号
    static QByteArray  hanzi2quweima(const QString &hanzi);


    static QByteArray _alignRight_fixedLen_int2bcd(const QString &data, int len, char c);
    static QByteArray _alignRight_fixedLen_int2hex(const QString &data, int len, char c);
    static QByteArray _alignLeft_fixedLen_byte2byte(const QByteArray &data, int len, char c);
    static QByteArray _alignRight_fixedLen_byte2byte(const QByteArray &data, int len, char c);
    static  char _2xor(const QByteArray &data);
    static QByteArray bytecutBybeginEnd(const QByteArray &val,const QByteArray &begin, const QByteArray &end);

    static pieceIns draw_qr_code(const QByteArray &hex/*=QByteArray("1B40 1B6101")*/, const QString &code);//获取二维码，打印指令
    static QByteArray draw_bar_code(const QString &code);
    static QStringList creat_normal_printpathtypes();
};

class QstandarditemmodelManager{
public:
    QstandarditemmodelManager();
    ~QstandarditemmodelManager();
    QStandardItemModel* mCreator();
private:
    QStandardItemModel *p;
};


class printer_0_thread : public QThread{
public:
    explicit printer_0_thread(QObject *parent = 0) : QThread(parent) {}
    ~printer_0_thread() {}

    //
    static void msecSleep(unsigned long msec);
    static void secSleep(unsigned long sec);
    //
};

#endif // PRINTER_0_H
