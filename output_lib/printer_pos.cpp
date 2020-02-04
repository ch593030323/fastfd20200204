#include "printer_pos.h"
#include <QtDebug>
#include "printer_pos_serial.h"
#include "printer_pos_net.h"
#include "printer_pos_vidpid.h"
#include "printer_pos_parallel.h"
#include "printer_pos_win_pos80.h"
#include <QSettings>

//静态函数的定义，顺序依赖于pro或者pri里的cpp的定义顺序
QstandarditemmodelManager Printer_POS::class_name_model;
Printer_POS::Printer_POS()
{
}

Printer_POS::~Printer_POS()
{
}

QSharedPointer<Printer_POS> Printer_POS::get_pos_printer_by_path(const QString &path)
{
    if(printer_0::comPortIsValid(path)) {
        return QSharedPointer<Printer_POS>( QSharedPointer<Printer_POS>(new Printer_POS_SERIAL));
    }
    if(path.contains("usb")) {//u口
        Printer_POS_SERIAL *p = new Printer_POS_SERIAL;
        p->setType(Printer_POS_SERIAL::USB);
        return QSharedPointer<Printer_POS>(p);
    }
    if(path.contains("&")) { //vidpid
        return QSharedPointer<Printer_POS>(new Printer_POS_VIDPID);
    }
    if(path.count(".")==3) {//网口
        return QSharedPointer<Printer_POS>(new Printer_POS_NET);
    }
    if(path == Printer_POS_PARALLEL::printPort()) {
        return QSharedPointer<Printer_POS>(new Printer_POS_PARALLEL);
    }
    if(path == printer_0::POS80) {
        return QSharedPointer<Printer_POS>(new Printer_POS_win_POS80);
    }
    return QSharedPointer<Printer_POS>();
}

bool Printer_POS::printer_write_bylanguage(const QString &path, const blockIns &ins, const QString &languageKey, QString &errstring)
{
    QSharedPointer<Printer_POS> printer = get_pos_printer_by_path(path);
    if(printer.isNull()) {
        errstring = "PRINT_IS_NULL";
        return false;
    }
    if(false == printer->tryOpen(path)) {
        errstring = printer->lastError();
        return false;
    }
    printer_write_bylanguage(printer, ins, languageKey);
    return true;
}

void Printer_POS::printer_write_bylanguage(QSharedPointer<Printer_POS> &printer, const blockIns &ins, const QString &languageKey)
{
    foreach(const pieceIns &p, ins) {
        printer->writeHex(p.first);
        printer->write(p.second, languageKey);
    }
}

bool Printer_POS::isNetPrinter()
{
    return port().count(".") == 3;
}

Printer_POS_auto_destroy::Printer_POS_auto_destroy(Printer_POS *printer)
{
    this->printer = printer;
}

Printer_POS_auto_destroy::~Printer_POS_auto_destroy()
{
    if(printer)
        delete printer;
}
