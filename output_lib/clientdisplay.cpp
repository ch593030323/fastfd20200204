#include "clientdisplay.h"
#include "clientdisplay_no.h"
#include "printer_pos_serial.h"
#include <QtDebug>

QByteArray ClientDisplay::DingHeng_flag;
QByteArray ClientDisplay::DingHeng_num;
QByteArray ClientDisplay::DingHeng_price;
QByteArray ClientDisplay::DingHeng_total;

static void setSerialportPairty(QString *port, ParityType parity)
{
    QStringList path = port->split(",");
    while(path.count() < 3)
        path << "";
    path[2] = QString::number(parity);
    *port = path.join(",");
}

static bool contain_zn(const QString &text) {
    QByteArray p1;
    if(text.trimmed().isEmpty()) return false;

    p1.append(text);
    return p1.length() != text.length();
}

ClientDisplay::ClientDisplay(const QString &type)
{
    _type =  type;
}

ClientDisplay::ClientDisplay()
{
}

void ClientDisplay::setType(const QString &type)
{
    _type = type;
}

QString ClientDisplay::getType()
{
    return _type;
}

ClientDisplay::~ClientDisplay()
{
}

bool ClientDisplay::writeCom(int sqr, double value, const QString &devpath, const QString &name)
{
    QMap<int, double> map;
    map.insert(sqr, value);
    return writeCom(map, devpath, name);
}

bool ClientDisplay::writeCom(const QMap<int, double> &map, const QString &devpath, const QString &name)
{
    if(devpath.isEmpty())
        return false;

    err0.clear();
    Printer_POS_SERIAL printer;
    bool f = true;
    switch(creat_clientdisplay_ledtypes().indexOf(_type)) {
    case 3://LED-8-WIN
        setSerialportPairty((QString*)&devpath, PAR_SPACE);
    case 0://LED-8
        for(QMap<int, double>::const_iterator k = map.begin(); k != map.end(); k ++) {
            int sqr = k.key();
            double value = k.value();
            f = ClientDisplay_NO::writeCom(sqr, QString().sprintf("%.02f", value),devpath);
        }
        break;
    case 1://VFD220
        if(false == printer.tryOpen(devpath)) {
            f = false;
            err0 = printer.lastError();
        } else {
            printer.setBaudRate(9600); //波特率必须时9600 才可以通信
            printer.writeHex("0C");//22. 清除屏幕的所有字符

            for(QMap<int, double>::const_iterator k = map.begin(); k != map.end(); k ++) {
                int sqr = k.key();
                double value = k.value();
                switch(sqr) {
                case 1://单价 PRICE
                    if(!contain_zn(name) )
                        printer.writedirect(QByteArray().append(name));
                    printer.writeHex("1F240102");//12.  x  y光标移到指定的位置 [1FH][24H]x  y;1H<=x<=14H，01H<=y<=02H
                    //                printer.writedirect(QByteArray("PRICE:").append(QString().sprintf("%.02f",in.toDouble())));
                    printer.writedirect(QByteArray().append(QString().sprintf("%.02f",value)));
                    break;
                case 2://总计 TATOL
                    printer.writedirect(QByteArray().append(name));
                    printer.writeHex("1F240102");//12.  x  y光标移到指定的位置 [1FH][24H]x  y;1H<=x<=14H，01H<=y<=02H
                    printer.writedirect(QByteArray("TATOL:").append(QString().sprintf("%.02f",value)));
                    break;
                case 3://收款 CASH
                    printer.writedirect(QByteArray().append(name));
                    printer.writeHex("1F240102");//12.  x  y光标移到指定的位置 [1FH][24H]x  y;1H<=x<=14H，01H<=y<=02H
                    printer.writedirect(QByteArray("CASH:").append(QString().sprintf("%.02f",value)));
                    break;
                case 4://找零 CHANGE
                    printer.writedirect(QByteArray().append(name));
                    printer.writeHex("1F240102");//12.  x  y光标移到指定的位置- [1FH][24H]x  y;1H<=x<=14H，01H<=y<=02H
                    printer.writedirect(QByteArray("CHANGE:").append(QString().sprintf("%.02f",value)));
                    break;
                }
            }
        }
        break;
    case 2://DingHeng
        if(false == printer.tryOpen(devpath)) {
            f = false;
            err0 = printer.lastError();
        } else {
            printer.setBaudRate(9600);
            QByteArray DingHeng_ins;
            //FF 0E   20   31 30 30 30 30   30 30 32 30 30   30 30 32 30 30 30
            for(QMap<int, double>::const_iterator k = map.begin(); k != map.end(); k ++) {
                int sqr = k.key();
                double value = k.value();
                switch(sqr) {
                case 5://重量
                    //0x31 0x30 0x30 0x30 0x30
                    if(value < 0)
                        DingHeng_flag = "-";
                    else
                        DingHeng_flag = " ";
                    DingHeng_num = QString().sprintf("%05d", qAbs(qRound(value * 1000))).toLocal8Bit();
                    DingHeng_ins  = DinghengDishIns();
                    break;
                case 1://单价 PRICE
                    DingHeng_price = QString().sprintf("%05d", qAbs(qRound(value * 100))).toLocal8Bit();
                    DingHeng_ins  = DinghengDishIns();
                    break;
                case 2://总计 TATOL
                    DingHeng_total = QString().sprintf("%06d", qAbs(qRound(value * 100))).toLocal8Bit();
                    DingHeng_ins  = DinghengDishIns();
                    break;

                case 3://收款 CASH
                    DingHeng_ins = QByteArray::fromHex("1B74FB0E") + QString().sprintf("%06d", qRound(value * 100)).right(6).toLocal8Bit();
                    break;

                case 4://找零 CHANGE
                    DingHeng_ins = QByteArray::fromHex("1B73FD0E") + QString().sprintf("%06d", qRound(value * 100)).right(6).toLocal8Bit();
                    break;
                }
            }
            printer.writedirect(DingHeng_ins);
        }
        break;
    }
    return f;
}

bool ClientDisplay::cleanCom(const QString &devpath, const QString &/*baudrate*/)
{
    err0.clear();
    Printer_POS_SERIAL printer;
    bool f = false;
    switch(creat_clientdisplay_ledtypes().indexOf(_type)) {
    case 3://LED-8-WIN
        setSerialportPairty((QString*)&devpath, PAR_SPACE);
    case 0://LED-8
        f = ClientDisplay_NO::cleanCom(devpath);
        break;
    case 1://VFD220
        if((f = printer.tryOpen(devpath))) {
            printer.setBaudRate(9600);
            printer.writeHex("0C");
        } else {
            err0 = printer.lastError();
        }
        break;
    case 2://DingHeng
        if((f = printer.tryOpen(devpath))) {
            printer.setBaudRate(9600);
            printer.writeHex("1B0C");
        } else {
            err0 = printer.lastError();
        }
        break;
    }

    return f;
}

bool ClientDisplay::startShowinfo(const QString &text, const QString &devpath)
{
    bool f = false;
    switch(creat_clientdisplay_ledtypes().indexOf(_type)) {
    case 3://LED-8-WIN
        setSerialportPairty((QString*)&devpath, PAR_SPACE);
    case 0://LED-8
        f = true;
        break;
    case 1://VFD220
    {
        err0.clear();
        Printer_POS_SERIAL printer;
        if((f=printer.tryOpen(devpath))) {
            printer.setBaudRate(9600); //波特率必须时9600 才可以通信
            printer.writeHex("0C");//22. 清除屏幕的所有字符
            printer.writedirect(QByteArray().append(text));
        } else {
            err0 = printer.lastError();
        }
    }
        break;
    case 2://DingHeng
        f = true;
        break;
    }

    return f;
}

QString ClientDisplay::lastError()
{
    return ClientDisplay_NO::lastError() + err0;
}

QByteArray ClientDisplay::DinghengDishIns()
{
    return QByteArray::fromHex("FF0E") +
            DingHeng_flag.rightJustified(1, ' ', true) +
            DingHeng_num.rightJustified(5, '0', true) +
            DingHeng_price.rightJustified(5, '0', true) +
            DingHeng_total.rightJustified(6, '0', true);
}

QStringList ClientDisplay::creat_clientdisplay_ledtypes()
{
    static QStringList clientdisplay_ledtypes;
    if(clientdisplay_ledtypes.isEmpty()) {
        clientdisplay_ledtypes << "LED-8"<<"VFD220" << "DingHeng" << "LED-8-WIN";
    }
    return clientdisplay_ledtypes;
}
