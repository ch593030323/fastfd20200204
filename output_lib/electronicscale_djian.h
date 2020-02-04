#ifndef ELECTRONICSCALE_DJIAN_H
#define ELECTRONICSCALE_DJIAN_H
//说明
/**
常工

    资料如附件，操作步骤如下：
1）您那需要对LSX条码秤升级新版本到：LSX_Chinese_V6.050，
2）然后在秤下位机菜单中打开：系统设置（长按此按键3s）->左右切换->参数设置->是否启用简单变价协议?  选择是否生效,（我司程序默认关闭，您的应用选择打开）
3）将附件中的：变价协议.doc参考范例，给Linux应用软件开发工程师，加进你们的应用软件中，来操作我司的秤

以上如有疑问，请来电，我会安排工程师与您对接
//=====================================================================
通信端口 6275，UDP通信

数据区：

1．起始符：0xCF

2．生鲜号3位：BCD

3．品名40位+1位结束符：BIN

4．单价4位：BCD

5．单位1位：BIN

6．预留15位：BIN

7．重复2~6 总长不超过1024字节



单位：

ID  Unit

0  50g

1  g

2  10g

3  100g

4  kg

5  oz

6  Lb

7  500g

8  600g

9  PCS(g)

10  PCS(kg)

11  PCS(oz)

12  PCS(Lb)

例：

       变价单品1：生鲜号 900123 品名 APPLE 价格 1234(下位机默认两位小数点 即为12.34) 单位 kg

       变价单品2：生鲜号 950012 品名 BEER 价格 550(下位机默认两位小数点 即为5.50)单位 PCS_KG



则输出：
CF 90 01 23 41 50 50 4C 45 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 12 34 04 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
CF 90 01 23 41 50 50 4C 45 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 12 34 04 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
     95 00 12 42 45 41 52 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 05 50 0A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
*/

#include <QByteArray>
#include <QStringList>
#include <QMap>

class  ElectronicScale_DJIAN
{
public:
    ElectronicScale_DJIAN();
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
    static QByteArray downloadPLUINFO_back(const QString &barcode);

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

#endif // ELECTRONICSCALE_DJIAN_H
