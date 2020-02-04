#include "printer_0.h"
#include "electronicscale_kaifeng.h"
#include "printer_0.h"

QMap<int, QString> ElectronicScale_KAIFENG::_wtype;
QStringList ElectronicScale_KAIFENG::_btype;
QStringList ElectronicScale_KAIFENG::_name;

ElectronicScale_KAIFENG::ElectronicScale_KAIFENG()
{

}

QStringList ElectronicScale_KAIFENG::scaleName()
{
    if(_name.isEmpty()) {
        _name << QObject::tr("凯丰条码秤");
    }
    return _name;
}

QMap<int, QString> ElectronicScale_KAIFENG::WeighType()
{
    if(_wtype.isEmpty()) {
        _wtype.insert(0, QObject::tr("称重") + "(0)");
        _wtype.insert(1, QObject::tr("定价") + "(1)");
        _wtype.insert(2, QObject::tr("定重定价") + "(2)");
    }
    return _wtype;
}

QStringList ElectronicScale_KAIFENG::BarType()
{
    if(_btype.isEmpty()) {
        _btype << QObject::tr("前缀 + 店内码 + 金额");
        _btype << QObject::tr("前缀 + 店内码 + 重量");
        _btype << QObject::tr("前缀 + 店内码 + 单价");
        _btype << QObject::tr("前缀 + 店内码");
        //        _btype << QObject::tr("前缀 + 店内码 + 重量(5)+ 金额(舍分)");

        //        _btype << QObject::tr("前缀 + 店内码 + 金额(舍分)");
        //        _btype << QObject::tr("前缀 + 店内码 + 重量(5) + 金额(不舍分)");
        //        _btype << QObject::tr("前缀 + 店内码 + 金额(舍分) + 重量");
        //        _btype << QObject::tr("前缀 + 店内码 + 金额(不舍分) + 重量");
        //        _btype << QObject::tr("1 + 重量 + 前缀 + 店内码");

        //        _btype << QObject::tr("编号 + 单价 + 重量(5)");
        //        _btype << QObject::tr("编号 + 重量(5) + 单价量");

    }
    return _btype;
}
bool ElectronicScale_KAIFENG::barcode_scale_resolve(QString &barcode, QString &price, QString &weight, QString &amount, const QString &code, const QString &type)
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
        weight = code.mid(7, 5);
        ret = true;
        break;
    case 2:
        barcode = code.mid(2, 5);
        price = code.mid(7, 5);
        ret = true;
        break;
    case 3:
        barcode = code.mid(2, 5);
        ret = true;
        break;
    }
    return ret;
}

QString ElectronicScale_KAIFENG::barcode_default_port()
{
    return "";
}
QByteArray ElectronicScale_KAIFENG::clearPLU()
{
    uchar c[] = {0x03,0xff,0x42,0xff};
    return QByteArray((char*)c, sizeof(c));
}

QByteArray ElectronicScale_KAIFENG::clearPLU_back()
{
    uchar c[] = {0x03,0xff,0x42,0xff};
    return QByteArray((char*)c, sizeof(c));
}

QByteArray ElectronicScale_KAIFENG::downloadPLUINFO(const QString &plu, const QString &goodsname, const QString &barcode, const QString &price, const QString &wtype, const QString &validate)
{
    //! 生成data
    QByteArray b;
//    uchar name[] =// 商品名称 以gbk为标准；转换后的指令需要左对齐；长度0～12个汉字；
//            /*24*/{0xc4,0xe3,0xba,0xc3, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00};
//    char plucode[] =//商品代码 以bcd为标准；有对齐；长度为小于5，超出的部分不会写到条码秤里：111235 ->11235
//            /*6*/{0x00,0x00,0x00,0x01, 0x12,0x35};
    char padding[] =
            /*2*/{0x00,0x00};
//    char unitprice[] =//单价计算：0010 0000 = 1000 = 4096/100 = 40.96元/kg；小端模式输入
//            /*4*/{0x00,0x10,0x00,0x00};
    b += /*name*/ printer_0::_alignLeft_fixedLen_byte2byte(printer_0::trans_UTF82GBK(goodsname), 24, char(0))+
            /*plucode*/printer_0::_alignRight_fixedLen_int2bcd(barcode, 6, char(0))+
            QByteArray(padding,sizeof(padding));
    {
        /*unitprice*/
        float v = price.toFloat();
        QByteArray p3;
        QByteArray p2 = QByteArray::fromHex(QByteArray::number(int(v),16));
        p2 = printer_0::_alignRight_fixedLen_byte2byte(p2,4,char(0));
        foreach(char c, p2)
            p3.push_front(c);
        b += p3;
    }

    char weight[] =// 定重定价的单位重量； 在设置秤重方式时 用到
            /*4*/{0x00,0x00,0x10,0x00};
    char discount[] =//折扣
            /*4*/{0x00,0x00,0x00,0x00};
//    char pluno[] =//price lookup
//            /*3*/{0x00,0x00,0x03};
    char padding_2[] =
            /*1*/{0x00};
    b += QByteArray(weight,sizeof(weight))+QByteArray(discount,sizeof(discount))+
            /*pluno*/printer_0::_alignRight_fixedLen_int2bcd(plu,3, char(0))+
            QByteArray(padding_2,sizeof(padding_2));

//    char validate0[] =//保质期
//            /*2*/{0x00,0x10};
    char tare[] =//皮重 默认为0（以g为单位）
            /*2*/{0x00,0x00};
//    char type[] =//称重类型：0称重，1定价，2定重定价
//            /*1*/{0x00};
    char datatype[] =//0 4都没有打印
            /*1*/{0x04};
    b += /*validate0*/printer_0::_alignRight_fixedLen_int2hex(validate,2,char(0))
            +QByteArray(tare,sizeof(tare))+
            /*type*/printer_0::_alignRight_fixedLen_int2hex(wtype,1,char(0)) +QByteArray(datatype,sizeof(datatype));

    char message[] =//说明文，暂时没用
            /*1*/{0x00};
    char pricechangeable[] =//是否可变价 缺省
            /*1*/{0x00};
    char distype[] =
            /*1*/{0x00};
    char padding_3[] =
            /*7*/{0x00,0x00,0x00,0x00, 0x00,0x00,0x00};
    b += QByteArray(message,sizeof(message))+QByteArray(pricechangeable,sizeof(pricechangeable))+QByteArray(distype,sizeof(distype))+QByteArray(padding_3,sizeof(padding_3));
    //! 生成data


    //! 获取下载plu
    QByteArray ba_data = b;
    int data_length = ba_data.length();
    QByteArray part1,part2,ins;

    /*2*/uchar stx[] = {0x03, 0xff};
    /*2*/uchar command[] = {0x43,0xff};
    /*4*/char paramete[]={0x00,0x00,0x00,0x00};
    /*4*/char ecr_id[] = {0x01,0x00,0x00,0x00};
    /*2*/char length[2];
    length[1] = data_length >> 8 &0xff;
    length[0] = data_length &0xff;

    part1 = QByteArray((char*)stx,sizeof(stx)) + QByteArray((char*)command,sizeof(command)) + QByteArray(paramete,sizeof(paramete)) + QByteArray(ecr_id,sizeof(ecr_id)) +
            QByteArray(length,sizeof(length));
    /*2*/char crc[2]; crc[0] = printer_0::_2xor(part1); crc[1] = 0;

    /*n*/QByteArray &data = ba_data;
    /*2*/char crc_2[2]; crc_2[0] = printer_0::_2xor(data); crc_2[1] = 0;

    part2 = QByteArray(crc,sizeof(crc)) + data + QByteArray(crc_2,sizeof(crc_2));
    ins = part1 + part2;
    //! 获取下载plu

    return ins;
}

QByteArray ElectronicScale_KAIFENG::downloadPLUINFO_back()
{
    uchar c[] = {0x03, 0xff, 0x43, 0xff};
    return QByteArray((char*)c, sizeof(c));
}

QByteArray ElectronicScale_KAIFENG::setShopName(const QString &shop_name)
{
    char idx[] =
    {0x01};
//    uchar name[] =
//    {0xc4,0xe3,0xba,0xc3, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00};
    char padding[] =
    {0x00,0x00,0x00,0x00, 0x00,0x00};



    QByteArray ba_data =
            QByteArray(idx,sizeof(idx)) +
            printer_0::_alignLeft_fixedLen_byte2byte(printer_0::trans_UTF82GBK(shop_name),25, char(0)) +
            QByteArray(padding,sizeof(padding));
    int data_length = ba_data.length();
    QByteArray part1,part2,ins;

    /*2*/uchar stx[] = {0x03, 0xff};
    /*2*/uchar command[] = {0x46,0xff};
    /*4*/char paramete[]={0x00,0x00,0x00,0x00};
    /*4*/char ecr_id[] = {0x02,0x00,0x00,0x00};
    /*2*/char length[2];
    length[1] = data_length >> 8 &0xff;
    length[0] = data_length &0xff;

    part1 = QByteArray((char*)stx,sizeof(stx)) + QByteArray((char*)command,sizeof(command)) + QByteArray(paramete,sizeof(paramete)) + QByteArray(ecr_id,sizeof(ecr_id)) +
            QByteArray(length,sizeof(length));
    /*2*/char crc[2]; crc[0] = printer_0::_2xor(part1); crc[1] = 0;

    /*n*/QByteArray &data = ba_data;
    /*2*/char crc_2[2]; crc_2[0] = printer_0::_2xor(data); crc_2[1] = 0;

    part2 = QByteArray(crc,sizeof(crc)) + data + QByteArray(crc_2,sizeof(crc_2));
    ins = part1 + part2;

    return ins;
}

QByteArray ElectronicScale_KAIFENG::setShopName_back()
{
    uchar c[] = {0x03,0xff,0x46,0xff};
    return QByteArray((char*)c, sizeof(c));
}

QByteArray ElectronicScale_KAIFENG::setShopPrefix(const QString &pre, const QByteArray &sysparam)
{
    QByteArray ba_data = sysparam.mid(16,31).replace(17,2,QByteArray().append(pre).rightJustified(2,'0').right(2));
    int data_length = ba_data.length();
    QByteArray part1,part2,ins;

    /*2*/uchar stx[] = {0x03, 0xff};
    /*2*/uchar command[] = {0x41,0xff};
    /*4*/char paramete[]={0x00,0x00,0x00,0x00};
    /*4*/char ecr_id[] = {0x02,0x00,0x00,0x00};
    /*2*/char length[2];
    length[1] = data_length >> 8 &0xff;
    length[0] = data_length &0xff;

    part1 = QByteArray((char*)stx,sizeof(stx)) + QByteArray((char*)command,sizeof(command)) + QByteArray(paramete,sizeof(paramete)) + QByteArray(ecr_id,sizeof(ecr_id)) +
            QByteArray(length,sizeof(length));
    /*2*/char crc[2]; crc[0] = printer_0::_2xor(part1); crc[1] = 0;

    /*n*/QByteArray &data = ba_data;
    /*2*/char crc_2[2]; crc_2[0] = printer_0::_2xor(data); crc_2[1] = 0;

    part2 = QByteArray(crc,sizeof(crc)) + data + QByteArray(crc_2,sizeof(crc_2));
    ins = part1 + part2;

    return ins;
}

QByteArray ElectronicScale_KAIFENG::setShopPrefix_back()
{
    uchar c[] = {0x03,0xff,0x41,0xff};
    return QByteArray((char*)c, sizeof(c));
}

QByteArray ElectronicScale_KAIFENG::getSysParam()
{
    uchar c[] = {0x03,0xff,0x81,0xff};
    return QByteArray((char*)c, sizeof(c));
}

QByteArray ElectronicScale_KAIFENG::getSysParam_back()
{
    uchar c[] = {0x03,0xff,0x81,0xff};
    return QByteArray((char*)c, sizeof(c));
}

QByteArray ElectronicScale_KAIFENG::test()
{
    uchar c[] = {0x03,0xff,0x30,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x08,0x00,0xc4,0x00,
                0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,
                0xff};
    return QByteArray((char*)c, sizeof(c));
}


