#ifndef ELECTRONICSCALE_KAIFENG_H
#define ELECTRONICSCALE_KAIFENG_H

//说明
//【PLU编号】0000～9999 共一万个
//【商品代码】商品代码 以bcd为标准；有对齐；长度为小于5，超出的部分不会写到条码秤里：111235 ->11235
//【商品名称】商品名称 以gbk为标准；转换后的指令需要左对齐；长度0～24个汉字；
//【商品单价】单价计算：0010 0000 = 1000 = 4096/100 = 40.96元/kg；小端模式输入
//【称重方式】0称重；1定价；2定重定价
//【有效日期】测试中没有打印出来，但是可以设置
//【标识号】
//【商店店名】最多12个汉字，或25个字母
//【条码秤型号】1.厦门顶尖 条码标签秤(LS2CX系列)；2.厦门吉景LP-16；3凯丰电子秤
//【条码格式】0-11【0】
//【0】
//0：pos + 金额；
//1：pos + 重量；
//2：pos+单价；
//3：pos；
//4：pos + 重量+金额（舍分）
//5：pos + 金额（舍分）；
//6：pos + 重量（5） + 金额（不舍分）
//7：pos + 金额（舍分） + 重量
//8：pos + 金额（不舍分） + 重量
//9：1 + 重量 + pos
//10：编号 + 单价 + 重量
//11：编号 + 重量 + 单价
//出厂设置 【菜单】【5】【2】【12345678】【确定】
//ip:1.87
#include <QByteArray>
#include <QStringList>
#include <QMap>

class    ElectronicScale_KAIFENG
{
public:
    ElectronicScale_KAIFENG();
    static QStringList scaleName();
    static QMap<int, QString> WeighType();
    static QStringList BarType();

    static QByteArray clearPLU();
    static QByteArray clearPLU_back();

    static QByteArray downloadPLUINFO(const QString &plu,
                                      const QString &goodsname,
                                      const QString &barcode,
                                      const QString &price,//4096 = 40.96元/kg
                                      const QString &wtype,
                                      const QString &validate
                                      );
    static QByteArray downloadPLUINFO_back();

    static QByteArray setShopName(const QString &shop_name);
    static QByteArray setShopName_back();

    static QByteArray setShopPrefix(const QString &pre, const QByteArray &sysparam);
    static QByteArray setShopPrefix_back();

    static QByteArray getSysParam();
    static QByteArray getSysParam_back();
    static bool barcode_scale_resolve(QString &barcode, QString &price, QString &weight, QString &amount, const QString &code, const QString &type);
    static QString barcode_default_port();
private:
    static QByteArray test();
    static QMap<int, QString> _wtype;
    static QStringList _btype;
    static QStringList _name;
};
#endif // ELECTRONICSCALE_KAIFENG_H
