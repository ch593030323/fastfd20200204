#ifndef ELECTRONICSCALE_DAHUA_H
#define ELECTRONICSCALE_DAHUA_H
//【0】函数必须定义为指针，否则无效
//【1】目前能用打函数有：clearPLU downloadPLUINFO 连个函数
//【2】软件功能只支持上面两种功能
//【3】条码秤手动功能： 置零； 折扣，以及允许手动折扣；自动打印（计重，计数）；条码类型设置（1F+6W+C..1F+6W+5E+C..）；设置日期；设置钱箱驱动开关；设置连续打印；设置快捷键；初始化ip和设置ip；
//【4】 【3】中功能也可以通过软件修改；设置标签软件修改，手动修改都可以
//【5】 如何调整精度: 功能键39706->点击去皮11次->02 即修改精度
//FF 店号 默认自带打就是FFMMMMM EEEEEC
//ip:1.160
#include <QStandardItemModel>

class ElectronicScale_DAHUA
{
public:
    enum ElectronicScale_LABEL{
        LABEL_40x30 = 0,
        LABEL_56x40
    };

    explicit ElectronicScale_DAHUA() {}

    ~ElectronicScale_DAHUA() {}

    static QByteArray clearPLU();//清PLU记录

    QByteArray downloadPLUINFO(QStandardItemModel *model, int row);//下载PLU信息

    static QByteArray setLABEL_40x30_step1();
    static QByteArray setLABEL_40x30_step2();

    static QStringList scaleName();
    static QMap<int, QString> WeighType();
    static QStringList BarType();
    static bool barcode_scale_resolve(//解析 条码秤一段条码
            QString &barcode, QString &price, QString &weight,
            QString &amount,
            const QString &code, const QString &type);
    static QString barcode_default_port();

    void clearREPORT();//清报表记录

    void setSYSTEM();//系统参数

    void setTEXTINFO();//文本信息

    void setSHORTCUT();//热键设置

    void setLABEL(ElectronicScale_LABEL labeltype = LABEL_40x30);//标签设置

    virtual QString downloadPre();

    virtual QByteArray downloadAfter();
private:
    static QMap<int, QString> _wtype;
    static QStringList _btype;
};

#endif // ELECTRONICSCALE_DAHUA_H
