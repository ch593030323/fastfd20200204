#include "printer_normal.h"
#include "printer_normal_80.h"
#include "printer_normal_80_old.h"
#include "printer_pos.h"
#include <QScopedPointer>
#include "bmp_info.h"

//静态函数的定义，顺序依赖于pro或者pri里的cpp的定义顺序

QstandarditemmodelManager Printer_Normal::class_name_model;
Printer_Normal::Printer_Normal()
{

}

void Printer_Normal::tran(blockIns &bins, int xinye_beep)
{
    Q_UNUSED(xinye_beep)
    Q_UNUSED(bins)
}

QString Printer_Normal::name()
{
    return "";
}

QByteArray Printer_Normal::init()
{
    QByteArray r = QByteArray::fromHex("1B40");
    return r.toHex();
}

QByteArray Printer_Normal::scale(char c)
{
    QByteArray r = QByteArray::fromHex("1D21")+c;
    return r.toHex();
}

QByteArray Printer_Normal::align(char c)
{
    QByteArray r;
    switch(c) {
    case 0x00://中
        r = QByteArray::fromHex("1B6101");
        break;
    case 0x01://右
        r = QByteArray::fromHex("1B6102");
        break;
    case 0x10://左
        r = QByteArray::fromHex("1B6100");
        break;
    }
    return r.toHex();
}

QByteArray Printer_Normal::bold(char c)
{
    //01加粗 00取消加粗
    QByteArray r;
    switch(c) {
    case 0x00://取消加粗
        r = QByteArray::fromHex("1B4500");
        break;
    case 0x01://加粗
        r = QByteArray::fromHex("1B4501");
        break;
    }
    return r.toHex();
}

QByteArray Printer_Normal::gonum(char c)
{
    QByteArray r =  QByteArray::fromHex("1D56")+c;
    return r.toHex();
}

QByteArray Printer_Normal::pic_code(const QString &filepath)
{
    QByteArray b0;
    Bmp_info bmpmaker;
    bmpmaker.Pic2printIns(filepath);
    bmpmaker.makeIns(b0);
    return b0;
}

QByteArray Printer_Normal::bar_code(const QString &code)
{
    return printer_0::draw_bar_code(code);
}

QByteArray Printer_Normal::qr_code(const QString &code)
{
    //编号转成GBK
    QByteArray content = printer_0::trans_UTF82GBK(code);
    //b
    qint16 lenght = content.length() + 3;//315030 三位
    char size0 = lenght & 0xff;//取低8位
    char size1 = lenght >> 8;//取高8位
    QByteArray pLH;
    pLH += size0;
    pLH += size1;
    //b0
    QByteArray b0;
    b0+="1B6101";
    b0+="1D286B0300314308";
    b0+="1d286b";
    b0 += pLH.toHex();
    b0 += "315030";
    b0 += content.toHex();
    b0+="1D286B0300315130";

    return b0;
}

Printer_Normal *Printer_Normal::byName(const QString &fname, QString &class_name)
{
    Printer_Normal *printtype = 0;
    QByteArray typeName;
    int typeID;
    for(int row = 0, rowcount = class_name_model.mCreator()->rowCount(); row < rowcount ;row++) {
        typeName = class_name_model.mCreator()->index(row, 0).data().toByteArray();
        typeID = QMetaType::type(typeName);
        if(typeID != 0) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            printtype = static_cast<Printer_Normal *>(QMetaType::create(typeID));
#else
            printtype = static_cast<Printer_Normal *>(QMetaType::construct(typeID));
#endif
            if(printtype && printtype->name() == fname) {
                class_name = typeName;
                break;
            }
            //
            if(printtype) {delete printtype; printtype = 0;}
        }
    }

    return printtype;
}

QByteArray Printer_Normal::static_qr_code(const QString &print_class_name, const QString &qr_code)
{
    int  typeID = QMetaType::type(print_class_name.toLocal8Bit());
    if(typeID != 0) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        QScopedPointer<Printer_Normal>printtype((Printer_Normal *)QMetaType::create(typeID));
#else
        QScopedPointer<Printer_Normal>printtype((Printer_Normal *)QMetaType::construct(typeID));
#endif
        //
        return printtype->qr_code(qr_code);
    }
    return "";
}

//Printer_Normal::Normal_INS_TYPE
void Printer_Normal::resolve(int print_instype, QString print_class_name, int paperwidth, int xinye_beep_count, blockIns &ins)
{
    //
    QByteArray line;
    if(paperwidth == 0) paperwidth = 32;
    line = QByteArray(paperwidth, '-');
    //
    reSort(ins);
    //
    Printer_Normal *insprinter = 0;
    switch(print_instype) {
    case Enhance://增强型ESC/POS打印机
        insprinter = new Printer_Normal_80;
        break;
    case Standard://标准型ESC/POS打印机
        insprinter = new Printer_Normal_80_old;
        break;
    default://增强型ESC/POS打印机
        insprinter = new Printer_Normal_80;
        break;
    }
    for(int index = 0, count = ins.count(); index < count; index++) {
        if(ins_first_has_image(ins[index].first)) {
            continue;
        }
        QList<QByteArray> plist = ins[index].first.split(',');
        ins[index].first.clear();
        foreach(const QByteArray &p, plist) {
            QByteArray pins = p.mid(1);
            char c = 0x00;
            if(pins == "00") {
                c = 0x00;
            } else if(pins == "01") {
                c = 0x01;
            } else if(pins == "10") {
                c = 0x10;
            } else if(pins == "11") {
                c = 0x11;
            }
            //32
            if(p.startsWith("i")) {//初始化
                ins[index].first += insprinter->init();
            } else if(p.startsWith("a")) {//对齐方式
                ins[index].first += insprinter->align(c);
            } else if(p.startsWith("s")) {//倍高倍宽
                ins[index].first += insprinter->scale(c);
            } else if(p.startsWith("-")) {//分隔线
//                ins[index].first.clear();
                ins[index].second = line+"\n";
            } else if(p.startsWith("b")) {//分隔线
                ins[index].first +=insprinter->bold(c);
            } else {
                ins[index].first += p;//默认
            }
        }
    }

    //4
    for(int index = 0, count = ins.count(); index < count; index++) {
        // a, ""
        //xx, "dd" == axx, "dd"
        if(ins[index].second.isEmpty()) {
            if( (index+1) < count) {
                ins[index + 1].first = ins[index].first + ins[index + 1].first;
                ins[index].first = "";
            }
        }
    }
    //
    reTran(ins, print_class_name, xinye_beep_count);
}

const QMap<QString, QVariant> &Printer_Normal::get_normal_print_map_ref()
{
    static QMap<QString, QVariant> map_normal_pint;
    if(map_normal_pint.isEmpty()) {
        for(int row = 0, rowcount = Printer_Normal::class_name_model.mCreator()->rowCount();
            row < rowcount ;row++) {
            QByteArray typeName = Printer_Normal::class_name_model.mCreator()->index(row, 0).data().toByteArray();
            int typeId = QMetaType::type(typeName);
            if (typeId != 0) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
                Printer_Normal *p =  static_cast<Printer_Normal *>(QMetaType::create(typeId));
#else
                Printer_Normal *p =  static_cast<Printer_Normal *>(QMetaType::construct(typeId));
#endif
                if(p) {
                    map_normal_pint.insert(typeName, p->name());
                    delete p;
                }
            }
        }
    }
    return map_normal_pint;
}

const QMap<QString, QVariant> &Printer_Normal::get_normal_ins_map_ref()
{
    static QMap<QString, QVariant> map_normal_ins;
    if(map_normal_ins.isEmpty()) {
        map_normal_ins.insert(QString::number(Enhance), QObject::tr("增强型ESC/POS打印机"));
        map_normal_ins.insert(QString::number(Standard), QObject::tr("标准型ESC/POS打印机"));
    }
    return map_normal_ins;
}

bool Printer_Normal::toprint(int print_instype, QString print_class_name, int paperwidth, int xinye_beep_count, const QString &path, blockIns ins, int times, QString *errstring)
{
    resolve(print_instype, print_class_name, paperwidth, xinye_beep_count, ins);
    return toprint(path, ins, times, errstring);
}

bool Printer_Normal::toprint(const QString &path, const blockIns &ins, int times, QString *errstring)
{
    //
    QSharedPointer<Printer_POS> printer = Printer_POS::get_pos_printer_by_path(path);
    if(printer.isNull()) {
        if(errstring) *errstring = "PRINTER_IS_NULL";
        return false;
    }
    //
    if(!printer->tryOpen(path)) {
        if(errstring) *errstring = "PRINTER_OPEN_ERROR";
        return false;
    }
    //
    toprint(printer, ins, times);
    printer->close();
    return true;
}

bool Printer_Normal::toprint(QSharedPointer<Printer_POS> printer, const blockIns &ins, int times)
{
    while(times >= 1 && times--) {
        foreach(const pieceIns &pins,ins) {
            printer->writeHex(pins.first);
            printer->writedirect(pins.second);
        }
    }
    return true;
}

void Printer_Normal::reSort(blockIns &ins)
{
    if(ins.count() > 0
            && false == ins[0].first.contains("i"))  {
        ins.prepend(pieceIns("i", ""));
    }

    qDebug() << __FILE__ << __LINE__ << ins;
    //3 QPair(",,a00","AA"),QPair(",s01,a00","AA") - >QPair("a00","AA"),QPair("s01","AA")
    for(int k = 0; k < ins.count(); k++) {
        if(ins_first_has_image(ins[k].first)) {
            continue;
        }
        QList<QByteArray> plist = ins[k].first.split(',');
        foreach(const QByteArray &p, plist) {
            for(int k2 = k + 1; k2 < ins.count(); k2++) {
                if(ins[k2].first.contains("i")) {//初始化
                    break;
                }
                if(ins[k2].first.contains("-")) {//初始化
                    break;
                }
                if(ins[k2].first.isEmpty()) {//空
                    break;
                }
                if(ins_first_has_image(ins[k2].first)) {//空
                    break;
                }
                if(false == ins[k2].first.contains(p)
                        &&ins[k2].first.contains(p.mid(0, 1))) {//检索s00时,没有预到s00,反而遇到s01
                    break;
                }
                //
                ins[k2].first.replace(p, "");//打印logo乱码
                ins[k2].first.replace(" ", "");//避免 "   ", ""的情况
                while(ins[k2].first.count(",,") > 0) {//避免,,,的情况
                    ins[k2].first.replace(",,", ",");
                }
                while(ins[k2].first.startsWith(",")) {//避免开头,,,的情况
                    ins[k2].first.remove(0, 1);
                }
                while(ins[k2].first.endsWith(",")) {//避免开头,,,的情况
                    ins[k2].first.chop(1);
                }
            }
        }
    }

    //4 QPair("a10",""),   QPair("s01","12.36") -> QPair("a10,s01","12.36")
    for(int k = 0; k < ins.count() - 1; k ++) {
        if(ins[k].second.count() == 0) {
            if(ins[k].first.contains("-"))
                continue;
            if(ins_first_has_image(ins[k].first))
                continue;
            if(ins_first_has_image(ins[k + 1].first))
                continue;
            ins[k + 1].first = ins[k].first +  "," + ins[k + 1].first;
            ins.removeAt(k);
            k --;
        }
    }

    //5  i,a10,s00,a00,s11 => i,a00,s11
    for(int k = 0; k < ins.count(); k ++) {
        if(ins[k].first.count() == 0)
            continue;
        if(ins[k].first.contains("-"))
            continue;
        if(ins_first_has_image(ins[k].first))
            continue;
        QList<QByteArray> plist = ins[k].first.split(',');
        QMap<QByteArray,  QByteArray> pmap;
        foreach(const QByteArray &p, plist) {
            pmap.insert(p.left(1), p);
        }
        ins[k].first = "";
        foreach(const QByteArray &p, pmap) {
            if(p == "i")
                ins[k].first = "i," + ins[k].first;
            else
                ins[k].first += p + ",";
        }
        ins[k].first.chop(1);
    }
}

void Printer_Normal::reTran(blockIns &ins, const QString &printer_class_name, int xinye_beep_count)
{
    int  typeID = QMetaType::type(printer_class_name.toLocal8Bit());
    if(typeID != 0) {
        QSharedPointer<Printer_Normal> printtype;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        printtype = QSharedPointer<Printer_Normal>(static_cast<Printer_Normal *>(QMetaType::create(typeID)));
#else
        printtype = QSharedPointer<Printer_Normal>(static_cast<Printer_Normal *>(QMetaType::construct(typeID)));
#endif
        printtype->tran(ins, xinye_beep_count);
    }
}

bool Printer_Normal::ins_first_has_image(const QByteArray &byte)
{
    if(byte.count() == 0)
        return false;
    foreach(const QString &b, byte.split(',')) {
        if(b.count() > 3)
            return true;
    }
    return false;
}
