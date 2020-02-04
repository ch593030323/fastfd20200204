#ifndef ELECTRONICSCALE_RONGTA_H
#define ELECTRONICSCALE_RONGTA_H


#include <QMap>
#include <QString>
#include <QStandardItemModel>
class ElectronicScale_RONGTA
{
public:
    struct pluData{
        pluData() {}
        inline pluData(const QString &_name,const QString &_barcode,
                       const QString &_price,
                       const QString &_packagetype,
                       const QString &_plucode,const QString &_flag, int _validday, int _barcodetype, int _unitcode):
            name(_name),//商品名称
            barcode(_barcode),//生鲜号
            price(_price),//单价
            validday(_validday),//有效时间
            packagetype(_packagetype),//包装类型

            Tareweight("0"),//皮重默认是0
            plucode(_plucode),//货号
            flag(_flag),//部门号
            barcodetype(_barcodetype),//条码编号
            discount("0"),
            unitcode(_unitcode)//单位
        {}
        QString name;// = "apple";
        QString barcode;// = "900012";
        QString price;// = "100";//1.00
        int validday;// = "15";//1.00
        QString packagetype;// = "300";//300kg

        QString Tareweight;// = "3000";//3kg
        QString plucode;// = "10001";
        QString flag;// = "21";
        int barcodetype;// = "21";
        QString discount;
        int unitcode;

    };
public:
    ElectronicScale_RONGTA();
    static QByteArray clearPLU();//清PLU记录

    static QByteArray downloadPLUINFO(int n/*包号>=1*/,
                                      const pluData & data1,
                                      const pluData & data2,
                                      const pluData & data3,
                                      const pluData & data4);
    static QString scaleName();
    static QMap<int, QString> WeighType();
    static QMap<int, QString> UnitType();
    static QMap<int, QString> BarType();
    static bool barcode_scale_resolve(QString &barcode, QString &price, QString &weight, QString &amount, const QString &code, const QString &type);
    static QString barcode_default_port();

    static int unitcode;//外部固定的单位编号
    static int barcodetype;//外部固定的条码类型
public:

    static QByteArray creatPlu(const pluData & data);
    static QByteArray creatPlu(const QList<pluData> &datas);
    static QByteArray creatHotkey(const QStringList &barcodes);//最多84x3个热键
    static QByteArray creatClearPlu();//最多84x3个热键
    static QByteArray creatfinished();
    static QMap<int, QString> _wtype;
    static QMap<int, QString> _btype;
    static QMap<int, QString> _utype;
    static QString _name;
};

#endif // ELECTRONICSCALE_RONGTA_H
