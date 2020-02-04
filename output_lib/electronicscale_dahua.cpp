#include "electronicscale_dahua.h"
#include <QFile>
#include "printer_0.h"
#include <QtDebug>

QMap<int, QString> ElectronicScale_DAHUA::_wtype;
QStringList ElectronicScale_DAHUA::_btype;

static QString str2numstr(const QString &str, int num)
{
    int other = num - str.length();

    if(other > 0)
        return QString(other, '0') + str;
    else
        return str.right(num);
}


//清除PLU 及记录商品条码打信息，再重置时有用
QByteArray ElectronicScale_DAHUA::clearPLU()
{
    return QByteArray("!OIA")+ QByteArray::fromHex("0D0A");
}

//清除报表 如日销售报表等
void ElectronicScale_DAHUA::clearREPORT()
{
    //printer->writedirect(QByteArray("!OHA")+ QByteArray::fromHex("0D0A"));
}

//设置系统参数 如秤号 打印浓度 店号 条码格式 条码类型 开钱箱驱动开关 等
void ElectronicScale_DAHUA::setSYSTEM()
{
    //printer->writedirect(QByteArray("!0O010500010201010100000100010000000000000000000001")+ QByteArray::fromHex("0D0A"));
}

void ElectronicScale_DAHUA::setTEXTINFO()
{
}

//设置热键 01 - 142 142个热键，直接点击 = 点击条码 + 点击PLU
//热键可以重复设置
//0L00A 第一层设置后面 0003 代表PLU号，依次类推；
//PLU号 固定长度为4为，最大为4000
void ElectronicScale_DAHUA::setSHORTCUT()
{
    QByteArray().append(
                QString(
                    "!0L00A000300020003000400050006000700080009001000110012001300140015001600170018001900200021002200230024002500260027002800290030003100320033003400350036B"
                    ) + QByteArray::fromHex("0D0A")
                +                         QString(
                    "!0L01A003700380039004000410042004300440045004600470048004900500051005200530054005500560057005800590060006100620063006400650066006700680069007000710072B"
                    ) + QByteArray::fromHex("0D0A")
                +                         QString(
                    "!0L02A007300740075007600770078007900800081008200830084008500860087008800890090009100920093009400950096009700980099010001010102010301040105010601070108B"
                    ) + QByteArray::fromHex("0D0A")
                +                         QString(
                    "!0L03A010901100111011201130114011501160117011801190120012101220123012401250126012701280129013001310132013301340135013601370138013901400141014200000000B"
                    ) + QByteArray::fromHex("0D0A")
                );
}

void ElectronicScale_DAHUA::setLABEL(ElectronicScale_LABEL labeltype)
{

    QString filename;
    switch(labeltype) {
    case LABEL_40x30:
        filename = ":/icon/40x30";
        break;
    case LABEL_56x40:
        filename = ":/icon/56x40";
        break;
    default:
        break;
    }

    QFile file(filename);
    if(!file.open(QFile::ReadOnly)) {
        qDebug() << file.errorString();
        return;
    }
    QByteArray b = file.readAll();
    file.close();

    //printer->writedirect(b + QByteArray::fromHex("0D0A"));

}

QString ElectronicScale_DAHUA::downloadPre()
{
    return "!0V";
}

QByteArray ElectronicScale_DAHUA::downloadAfter()
{
        return QByteArray::fromHex("0D0A");
}

/*
"!0V
0001                            [4]PLU序号
A                               A
2281080                         [7]商品编号 商品编号不能超过5位，超过打将会丢失!
002000                          [6]单价 分/kg 02000 20元/kg
0                               [1]称重方式 0 称重； 2 计件； 2定重
00 00 00                        [2][2][2]特殊信息1 特殊信息2 特殊信息3
008 03 00                       [3][2][2]有效期 店名号 部门号
0000000000000 00000 00          [13][5][2]13位数字代号 皮重 标签号
00                              [2]是否打折 00 不按时间段调价，手动不可以调单价; 01 按时间段调价，手动不可以调单价;02 按时间段调价，手动可以调单价
00 00 00                        [2][2][2] 第一时间段起始时间 第一时间段结束时间 折扣
00 00 00                        [2][2][2] 第二时间段起始时间 第二时间段结束时间 折扣
00 00 00                        [2][2][2] 第三时间段起始时间 第三时间段结束时间 折扣
00 00 00                        [2][2][2] 第四时间段起始时间 第四时间段结束时间 折扣
B
186642525027                    商品1区位码
C
186642525028                    商品2区位码
D
186642525028                    商品3区位码
E"

*/
QByteArray ElectronicScale_DAHUA::downloadPLUINFO(QStandardItemModel *model, int row)
{
    const  int max[26] = {  4, 7, 6,1, 2,2, 2 //7
                           , 3, 2, 2,13,5,2, 2 //7
                           , 2, 2, 2,2, 2,2, 2 //7
                           , 2, 2, 2,2, 2};    //5
    //QString s0 = "!0V";
    QString s0 = downloadPre();

//    int rr = 0;
    for(int c = 0; c < 26; c++) {
        if(c == 1) s0 += "A";
        s0 += str2numstr(model->item(row, c)->text(), max[c]);
    }
    s0 += "B";
    s0 += printer_0::hanzi2quweima(model->item(row, 26)->text());
    s0 += "C";
    s0 += printer_0::hanzi2quweima(model->item(row, 27)->text());
    s0 += "D";
    s0 += printer_0::hanzi2quweima(model->item(row, 28)->text());
    s0 += "E";

    return QByteArray().append(s0) + downloadAfter();//QByteArray::fromHex("0D0A");
}


QByteArray ElectronicScale_DAHUA::setLABEL_40x30_step1()
{
    return QByteArray(
                "!0T00A56300308150301290000000000000316220000000000000000000000000000000000000317190315250000000000000000000000000000000000000000000000000000000000000300110600000007"+ QByteArray::fromHex("0D0A")
                );
}

QByteArray ElectronicScale_DAHUA::setLABEL_40x30_step2()
{
    return QByteArray(
                "!0R00A000000030122030119030125000000000000000000000000000000000000000000000000000000000000030819000000000000000000030719030722000000031119000000000000000000000000000000000000000000000000000000000000"+ QByteArray::fromHex("0D0A")
                );
}
QMap<int, QString> ElectronicScale_DAHUA::WeighType()
{
    if(_wtype.isEmpty()) {
        _wtype.insert(0, QObject::tr("称重(0)"));
        _wtype.insert(1, QObject::tr("计件(1)"));
        _wtype.insert(2, QObject::tr("定重(2)"));
    }

    return _wtype;
}
QStringList ElectronicScale_DAHUA::BarType()
{
    //F:前导码; W:商品代码;E:金额;C:正校验码;N:重量;
    if(_btype.isEmpty()) {
        _btype << QObject::tr("FFWWWWWEEEEEC")
               << QObject::tr("FFWWWWWNNNNNC")
               << QObject::tr("FFWWWWWEEEEENNNNNC")
               << QObject::tr("FFWWWWWNNNNNEEEEEC");
    }
    return _btype;
}

bool ElectronicScale_DAHUA::barcode_scale_resolve(QString &barcode, QString &price, QString &weight, QString &amount, const QString &code, const QString &type)
{
    Q_UNUSED(price)
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
        amount = code.mid(7, 5);
        weight = code.mid(12, 5);
        ret = true;
        break;
    case 3:
        barcode = code.mid(2, 5);
        weight = code.mid(7, 5);
        amount = code.mid(12, 5);
        ret = true;
        break;
    }
    return ret;
}

QString ElectronicScale_DAHUA::barcode_default_port()
{
    return "";
}

QStringList ElectronicScale_DAHUA::scaleName()
{
    return QStringList() << QObject::tr("上海大华 TM");
}
//清PLU记录
//"0iaa0
// " "30696161300d0a03"
//清报表记录
// "0ha
// " "3068610a0d0a03"
//系统参数
//"0o01
//" "306f30310d0a03"
//热键设置
//"0l00
//" "306c30300d0a03"
//标签设置
// "0t00
//" "307430300d0a03"
//下载商品数据
//0v
