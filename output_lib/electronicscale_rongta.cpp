#include "electronicscale_rongta.h"
#include <QtDebug>
#include "printer_0.h"

QMap<int, QString> ElectronicScale_RONGTA::_wtype;
QMap<int, QString> ElectronicScale_RONGTA::_btype;
QMap<int, QString> ElectronicScale_RONGTA::_utype;
QString ElectronicScale_RONGTA::_name;
int ElectronicScale_RONGTA::unitcode;
int ElectronicScale_RONGTA::barcodetype;


ElectronicScale_RONGTA::ElectronicScale_RONGTA()
{

}

QByteArray ElectronicScale_RONGTA::clearPLU()
{
    return creatClearPlu();
}

QByteArray ElectronicScale_RONGTA::downloadPLUINFO(int n, const ElectronicScale_RONGTA::pluData &data1, const ElectronicScale_RONGTA::pluData &data2, const ElectronicScale_RONGTA::pluData &data3, const ElectronicScale_RONGTA::pluData &data4)
{
    QByteArray retvalue;
    retvalue += (char)0x01;//cmd
    retvalue += char((n-1)%256);
    retvalue += char((n-1)/256);
    retvalue += creatPlu(data1);
    retvalue += creatPlu(data2);
    retvalue += creatPlu(data3);
    retvalue += creatPlu(data4);

    return retvalue;

}

QString ElectronicScale_RONGTA::scaleName()
{
    if(_name.isEmpty()) {
        _name = QObject::tr("容大RLS1000");
    }
    return _name;
}

QMap<int, QString> ElectronicScale_RONGTA::WeighType()
{

    //    0x00
    //    0x45
    //    0x85
    //    0xca	0x包装类型 误差
    //        正常   0%
    //        定重   5%
    //        定价   5%
    //    定重定价  10%
    //00正常, 称重方式：正常(0x00)、定重(0x45)、定价(0x85)、定重定价(0xca)
    if(_wtype.isEmpty()) {
        _wtype.insert(0x00, QObject::tr("正常(0x00)"));
        _wtype.insert(0x45, QObject::tr("定重(0x45)"));
        _wtype.insert(0x85, QObject::tr("定价(0x85)"));
        _wtype.insert(0xca, QObject::tr("定重定价(0xca)"));
    }
    return _wtype;
}

QMap<int, QString> ElectronicScale_RONGTA::UnitType()
{
    if(_utype.isEmpty()) {
        _utype.insert(0x01, "g");
        _utype.insert(0x04, "kg");
        _utype.insert(0x07, "500g");
    }
    return _utype;
}

QMap<int, QString> ElectronicScale_RONGTA::BarType()
{
    //D部门号、I货号、P总价、W重量、C校验和
    if(_btype.isEmpty()) {
        _btype.insert(2, "DDIIIIIPPPPPC");
        _btype.insert(7, "DDIIIIIWWWWWC");
        _btype.insert(94, "DDIIIIIPPPPPWWWWWC");//这边是容大秤的说明书bug
    }
    return _btype;
}

bool ElectronicScale_RONGTA::barcode_scale_resolve(QString &barcode, QString &price, QString &weight, QString &amount, const QString &code, const QString &type)
{
    Q_UNUSED(price)
    bool ret = false;
    int index = BarType().values().indexOf(type);
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
    case 2://
        barcode = code.mid(2, 5);
        amount = code.mid(7, 5);
        weight = code.mid(12, 5);
        ret = true;
        break;
    }
    return ret;
}

QString ElectronicScale_RONGTA::barcode_default_port()
{
    return "5001";
}

QByteArray ElectronicScale_RONGTA::creatPlu(const pluData &data)
{
    int discount = 0;
    int packageweight = 0;
    QByteArray plu_block;
    plu_block += printer_0::trans_UTF82GBK(data.name).leftJustified(36, 0x00, true);//品名，ascii码
    plu_block += QByteArray::fromHex(data.barcode.toLocal8Bit()).rightJustified(3, 0x00, true);//生鲜码,3字节BCD
    plu_block += (char)0x00;//对应的标签模板索引
    plu_block += QByteArray().append(char(discount%256)).leftJustified(1, 0x00, true);//折扣，BIN码;0a-10折 1e-30折 32-50折 50-80折
    plu_block += QByteArray::fromHex(data.flag.toLocal8Bit()).rightJustified(1, 0x00, true);//所属部门，BCD
    plu_block += QByteArray().append((char)data.barcodetype).leftJustified(1, 0x00, true);//(char)0x20;//条码类型，BIN码， 默认是32号条码
    plu_block += QByteArray::fromHex(data.price.toLocal8Bit()).rightJustified(4, 0x00, true);//单价，4字节，BCD
    plu_block += (char)data.unitcode ;//计价单位，BIN码（上位机要载入相应的单位，才可以正常打印出来）,以实际秤的单价序号为标准

//    保鲜天数，BIN码 左对齐
    plu_block += char(data.validday&0x00ff);
    plu_block += char((data.validday>>8)&0x00ff);

    plu_block += (char)0x01;//信息1，BIN码
    plu_block += (char)0x01;//信息2，BIN码
    plu_block += QByteArray().append((char)packageweight).rightJustified(3, 0x00, true);//包装重量，3字节BCD码
    plu_block += QByteArray().append(char(data.packagetype.toInt())).leftJustified(1, 0x00, true);//包装类型BIN码, 00正常, 称重方式：正常(0x00)、定重(0x45)、定价(0x85)、定重定价(0xca)
    plu_block += QByteArray::fromHex(data.Tareweight.toLocal8Bit()).rightJustified(3, 0x00, true);//皮重
    plu_block += QByteArray::fromHex(data.plucode.toLocal8Bit()).rightJustified(5, 0x00, true);//货号,5字节BCD

    return plu_block;
}

QByteArray ElectronicScale_RONGTA::creatPlu(const QList<pluData> &datas)
{
    /*
第n包 是 0x[(n-1)%256][(n-1)/256]
*/
    QByteArray retvalue;
    retvalue += (char)0x01;//cmd
    int blockid = 0;
    for(int begin = 0, count = datas.count();begin < count; begin++) {
        if(0 == begin % 4) {//包地址
            blockid ++;
            retvalue += char((blockid-1)%256);
            retvalue += char((blockid-1)/256);
        }
        retvalue += creatPlu(datas[begin]);
    }
    return retvalue;
}

QByteArray ElectronicScale_RONGTA::creatHotkey(const QStringList &barcodes)
{
    /*
     * 第1个包地址：0xf803
第2个包地址：0xf903
第3个包地址：0xfa03
*/

    QByteArray retvalue;
    retvalue += (char)0x02;//cmd
    for(int begin = 0, count = barcodes.count();begin < count ;begin++) {
        if(0 == begin) {//第一块开始地址
            retvalue += (char)0xf8;
            retvalue += (char)0x03;
        } else if(84 == begin) {//第二块开始地址
            retvalue += (char)0xf9;
            retvalue += (char)0x03;
        } else if(84*2 == begin) {//第三块开始地址
            retvalue += (char)0xfa;
            retvalue += (char)0x03;
        } else if(begin >= 84*3) {//超出范围
            break;
        }

        //开始拼接
        retvalue += QByteArray::fromHex(barcodes[begin].toLocal8Bit()).rightJustified(3, 0x00, true);
    }
    return retvalue;
}

QByteArray ElectronicScale_RONGTA::creatClearPlu()
{
    QByteArray retvalue;
    retvalue += (char)0x0e;//cmd
    retvalue += (char)0x04;
    retvalue += (char)0x00;

    retvalue += (char)0x00;
    retvalue += (char)0x04;
    retvalue += QByteArray(254, (char)0x00);
    return retvalue;
}

QByteArray ElectronicScale_RONGTA::creatfinished()
{
    QByteArray retvalue;
    retvalue += char(0x0e);
    retvalue += char(0x02);
    retvalue += char(0x00);
    return retvalue;
}

