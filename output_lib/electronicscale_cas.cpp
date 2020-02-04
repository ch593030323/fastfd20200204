#include "electronicscale_cas.h"
#include "printer_0.h"

QMap<int, QString> ElectronicScale_CAS::_wtype;
QStringList ElectronicScale_CAS::_btype;

ElectronicScale_CAS::ElectronicScale_CAS()
{
}

ElectronicScale_CAS::~ElectronicScale_CAS()
{
}

QByteArray ElectronicScale_CAS::clearPLU(const QString &plu, const QString &dept_no)
{

    QByteArray ba;
    ba += "C43F13,";
    ba += int2numhex_0(dept_no,2);
    ba += int2numhex_0(plu, 6);
    ba += "\n";
    //    ba += "C43F12";
    //    ba += int2numhex_0(1,2);                   //默认部门号是 1
    //    ba += "\n";

    return ba;
}

QByteArray ElectronicScale_CAS::downloadPLUINFO(const QString &plu, const QString &dept_no
                                               ,  const QString &barcode,const QString &goodsname,const QString &price, const QString &baozhiqi, const QString &plu_type)
{
    //    "192.168.1.161",20304;
    //    0-3<< "PLU编号" << "商品代码" << "单价" << "称重方式"
    //    4-7<< "特殊信息1" << "特殊信息2" << "特殊信息3" << "有效日期"
    //    8-11<< "店名号" << "部门号" << "数字代号" << "皮重"
    //    12-15<< "标签号" << "是否打折" << "1时间段开始" << "1时间段结束"

    //    16-19<< "1时间段折扣" << "2时间段开始" << "2时间段结束" << "2时间段折扣"
    //    20-23<< "3时间段开始" << "3时间段结束" << "3时间段折扣" << "4时间段开始"
    //    24-27<< "4时间段结束" << "4时间段折扣" << "商品名1" << "商品名2"
    //    28<< "商品名3");


    //? 标识号， 或者店号不知道
    //    ?称重方式 和大华的不同 称重方式 1称重； 2数量；3个
    //? 部门号有什么用；和店号有什么区别
    QByteArray ba = "W02A"                                   //W是写入，02A是PLU信息

            + int2numhex_0(plu,5)+","                        // 0 PLU序号
            + int2numhex_0(dept_no,2) +"L"                         // 部门号
            ;
    QByteArray body;
    body += addInstruct_digit(11,'L',6,barcode);             // 1商品代码
    body += addInstruct_str(10,'S',40,goodsname);            // 26商品名
    body += addInstruct_digit(6,'L',7,price);                // 2单价(/kg)
    body += addInstruct_digit(16,'L',4,baozhiqi);            // 7有效日期
    body += addInstruct_digit(4,'M',1,plu_type);             // 3称重方式 1称重； 2数量；3个


    ba +=   int2numhex_0(body.size(),4) + ":"//计算内容大小c
            + body
            + printer_0::_2xor(body)
            ;

    return ba;

}

QByteArray ElectronicScale_CAS::addInstruct_str(int identify, char type, int maxsize, const QString &content)
{
    QByteArray b0 = byte2numbyte(printer_0::trans_UTF82GBK(content),maxsize);
    QByteArray b = "F=";
    b += int2numhex_0(identify,2).toUpper();   //标识号
    b += ".";

    b += int2numhex_0(type,2).toUpper();       //类别
    b += ",";

    b += int2numhex_0(b0.size(),4).toUpper();  //内容
    b += ":";

    b += b0;

    return b;//20596072;
}

QByteArray ElectronicScale_CAS::addInstruct_digit(int identify, char type, int maxsize, const QString &content)
{
    QByteArray b0, b1;
    b1 = byte2numbyte(QByteArray().append(content), maxsize);
    b1 = QByteArray::fromHex(QByteArray::number(b1.toInt(), 16));
    //小端 字节
    foreach(char c, b1)
        b0.push_front(c);

    QByteArray b = "F=";
    b += int2numhex_0(identify,2).toUpper();   //标识号
    b += ".";

    b += int2numhex_0(type,2).toUpper();       //类别
    b += ",";

    b += int2numhex_0(b0.size(),4).toUpper();  //内容
    b += ":";

    b += b1;

    return b;
}
QByteArray ElectronicScale_CAS::int2numhex_0(const QString &begin, int num)
{
    return int2numhex_0(begin.toInt(),num);
}

QByteArray ElectronicScale_CAS::int2numhex_0(int begin, int num)
{
    QByteArray b = QByteArray::number(begin,16);

    int other = num - b.length();

    if(other > 0)
        return QByteArray(other, '0') + b;
    else
        return b.right(num);
}

QByteArray ElectronicScale_CAS::byte2numbyte(const QByteArray &byte, int num)
{
    int other = num - byte.length();

    if(other > 0)
        return byte;
    else
        return byte.right(num);
}

QMap<int, QString> ElectronicScale_CAS::WeighType()
{
    if(_wtype.isEmpty()) {
        _wtype.insert(1, QObject::tr("称重") + "(1)");
        _wtype.insert(2, QObject::tr("数量") +"(2)");
        _wtype.insert(3, QObject::tr("个")+ "(3)");
    }

    return _wtype;
}

QStringList ElectronicScale_CAS::BarType()
{
    //D:部门 ;I:项目代码 ;P:价格 ;C:总价条码校验位 ;U:单价 ;W:重量 ;
    if(_btype.isEmpty()) {
        _btype << QObject::tr("DDIIIIIPPPPPC")
                  //                                     << QObject::tr("DIIIIIIPPPPPC")
               << QObject::tr("DDIIIIIPPPPPCUUUUU")
               << QObject::tr("DDIIIIIPPPPPCWWWWW");

    }
    return _btype;
}

bool ElectronicScale_CAS::barcode_scale_resolve(QString &barcode, QString &price, QString &weight, QString &amount, const QString &code, const QString &type)
{
    bool ret = false;
    int index = BarType().indexOf(type);
    switch(index) {
    case 0:
        barcode = code.mid(2, 5);
        amount = code.mid(7, 5);
        ret = true;
        break;
    case 1:
        barcode = code.mid(2, 5);
        amount = code.mid(7, 5);
        price = code.mid(12 + 1, 5);
        ret = true;
        break;
    case 2:
        barcode = code.mid(2, 5);
        amount = code.mid(7, 5);
        weight = code.mid(12 + 1, 5);
        ret = true;
        break;
    }
    return ret;
}

QString ElectronicScale_CAS::barcode_default_port()
{
    return "";
}

QStringList ElectronicScale_CAS::scaleName()
{
    return QStringList() << QObject::tr("上海凯士 CL5200");
}
