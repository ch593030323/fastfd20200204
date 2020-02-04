#include "public_mainprinter.h"
#include "printer_0.h"
#include "qextserialenumerator.h"
#include "lds.h"
#include "public_sql.h"
#include <QDebug>
#include "bmp_info.h"
#include "printer_normal_80.h"
#include "n_func.h"
#include "public_ins_maker.h"
#include "printer_pos.h"
#include "backheader.h"


public_mainprinter::public_mainprinter(QObject *parent) :
    QObject(parent)
{
}

public_mainprinter::~public_mainprinter()
{}


pieceIns public_mainprinter::drawText(const QByteArray &hex, const QString &text, int textwidth, Qt::Alignment textflag)
{
    int scale_h = 1;
    QList<QByteArray> hexs = hex.split(',');
    foreach(const QByteArray &h,  hexs) {
        if(h.startsWith('s')) {
            int s = h.mid(1,1).toInt();
            if(s != 0) {
                scale_h = s + 1;
                textwidth = textwidth / scale_h;// s00, s11, s22, s33 -> 不放大、x2、x3、x4
            }
            break;
        }
    }

    //    pieceIns pret;
    QByteArray ret;
    //【0】    文本长度过长会破坏格式
    ret=text.toLocal8Bit();
    int actualtextwidth=lds::size_g_of_u(text);//ret.length();
    char blank = ' ';
    if(actualtextwidth < textwidth) {//文本实际宽度，小于文本预宽度
        switch(textflag) {
        case Qt::AlignCenter://居中
            ret=QByteArray((textwidth-actualtextwidth)/2, blank) + ret + QByteArray((textwidth-actualtextwidth)/2, blank);
            break;
        case Qt::AlignRight://右对齐
            ret=QByteArray((textwidth-actualtextwidth), blank) + ret;
            break;
        case Qt::AlignLeft://左对齐
            ret=ret + QByteArray((textwidth-actualtextwidth), blank);
            break;
        }
        //最后统一按左对齐处理
    }

    return pieceIns(hex, ret);
}

//若是后面追加\n, 则判断之前的指令，后‘ ’清掉
blockIns public_mainprinter::drawText(const QByteArray &hex, const QString &text, int textwidth, Qt::Alignment textflag,  int blanks, const QString &text2)
{
    //1
    blockIns ins;
    ins.append(drawText(hex, text,textwidth,textflag));

    //2
    QByteArray text0 = ins.last().second;
    if(text2 == "\n") {
        for(int count = text0.count() - 1 ; count >= 0; count --) {
            if(text0.at(count) == ' ' ) {
                text0.remove(count, 1);
                continue;
            }
            break;
        }
    }
    ins[ins.count() - 1].second = text0;

    //3
    ins.append(drawText("", QString(blanks,QChar(' '))+text2));
    return ins;
}

blockIns public_mainprinter::drawText(const QByteArray &hex, const QString &text, const QByteArray &hex2, int textwidth, Qt::Alignment textflag)
{
    blockIns ins;
    ins.append(drawText(hex, text,textwidth,textflag));
    ins.append(drawText(hex2));
    return ins;
}

blockIns public_mainprinter::drawTextEnd(const QByteArray &hex, const QString &text)
{
    Q_UNUSED(text)
    blockIns ins;

    ins.append(drawText(hex));

    if(n_func::f_get_sysparm("yun_bill_bottom_enable") == "1") {
        ins.append(drawText("", n_func::f_get_sysparm("yun_bill_bottom")));
    }

    return ins;
}

pieceIns public_mainprinter::drawLogo(const QByteArray &hex, const QString &logopath)
{
    QByteArray b0;
    Bmp_info bmpmaker;
    bmpmaker.Pic2printIns(logopath);
    bmpmaker.makeIns(b0);
    return pieceIns(hex+b0, "");
}

pieceIns public_mainprinter::drawQR_Code(const QByteArray &hex, const QString &code)
{
    return public_ins_maker::draw_qr_code(hex, code);
}

pieceIns public_mainprinter::drawoneBarcode(const QByteArray &hex, const QString &code)
{
    return public_ins_maker::oneBarcode(hex, code);
}


blockIns public_mainprinter::setPrintCount(const blockIns &text, int printcount)
{
    blockIns b;
    while(printcount >= 1 && printcount--) {
        b+=text;
    }
    return b;
}

void public_mainprinter::write(const blockIns &printIns)
{
    write(printIns, lds::sysconf->value("system_setting/printerpath").toString());
}

//bool public_mainprinter::asynchPrint(const blockIns &printIns, const QString &path, bool isblock)
//{
//    lds_query_hddpos  query;
//    blockIns ins = printIns;
//    public_ins_maker::resolve(path, query, ins, lds::get_soft_curent_language_first());
//    return asynchPrintWithoutresolve(ins, path, isblock);
//}

//bool public_mainprinter::asynchPrintWithoutresolve(blockIns printIns, const QString &path, bool isblock)
//{
//    Q_UNUSED(isblock)
//    QString error;
//    //2
//    if(Printer_Normal::toprint(path, printIns, 1, &error)) {
//        return true;
//    }
//    if(!error.isEmpty()) sendError(path + ":" + error, QObject::tr("打印信息"));
//    return false;
//}

//bool public_mainprinter::synchPrint(const blockIns &printIns, const QString &path)
//{
//    return asynchPrint(printIns, path, true);
//}

//bool public_mainprinter::synchPrintWithoutresolve(const blockIns &printIns, const QString &path)
//{
//    return asynchPrintWithoutresolve(printIns, path, true);
//}

void public_mainprinter::write(const blockIns &printIns, const QString &path)
{
    QSharedPointer<Printer_POS> printer = Printer_POS::get_pos_printer_by_path(path);
    if(printer.isNull()) {
        sendError(path + ":" + "PRINTER_IS_NULL", QObject::tr("打印信息"));
        return;
    }
    if(false == printer->tryOpen(path)) {
        sendError(path + ":" + printer->lastError(), QObject::tr("打印信息"));
        return;
    }
    foreach(const pieceIns &p, printIns) {
        printer->writeHex(p.first);
        printer->write(p.second, lds::get_soft_curent_language_first());
    }
}

void public_mainprinter::writeHex(const QByteArray &hex, const QString &path)
{
    QSharedPointer<Printer_POS> printer = Printer_POS::get_pos_printer_by_path(path);
    if(printer.isNull()) {
        sendError(path + ":" + "PRINTER_IS_NULL", QObject::tr("打印信息"));
        return;
    }
    if(false == printer->tryOpen(path)) {
        sendError(path + ":" + printer->lastError(), QObject::tr("打印信息"));
        return;
    }
    printer->writeHex(hex);
}

void public_mainprinter::sendError(const QString &err, const QString &title)
{
    backheader::sendError(err, title);
}
