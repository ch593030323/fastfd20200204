#ifndef ELECTRONICSCALE_YSHENG_H
#define ELECTRONICSCALE_YSHENG_H

//【0】"UPL\tINF\t\r\n" 获取条码秤信息
//【1】2|010010|000035 20为标志位
//【3】默认项目条码格式是 BBEEEEELLLLL(B全局标志位 E货物编号 L销售总价)
//【4】新版 PLU编号 1～9 999 999
//【5】货号 0～5位  5 默认设置中 超出五位不打印
//【6】1代表计重，2代表计件(设置为重量：【0】【1】默认计重单位【2】默认计数单位【3】kg千克计重单位【4】g克计重单位【5】ton吨计重单位【6】ib英镑计重单位【7】500g市斤计重单位【8】100g百克计重单位)
//【7】价格 (1200,3) = 1.200；但是小数保留2位；(有效数据，小数点位置)
//【8】保质期 26有是否允许打印 和 31保质期时间
//【9】商品码 这儿用GBK格式来，条码秤编号，可以通过【步骤0】获取详细信息
//ip:1.192 编程 + 42 +确认
#include <QMap>
#include <QString>
#include <QStandardItemModel>

class ElectronicScale_YSHENG
{
public:
    explicit ElectronicScale_YSHENG();
    inline static QByteArray DL_f() {
        return QByteArray("DOWNLOAD");
    }
    inline static QByteArray CL_f() {
        return QByteArray("clear");
    }
    inline static QByteArray end() {
        return QByteArray("UPL\tTIM\t\r\n");
    }
    inline static QByteArray end_ans() {
        char c[] = {13,10,68,87};
        return QByteArray(c, sizeof(c));
    }

    static QByteArray clearPLU();//清PLU记录

    static QByteArray downloadPLUINFO(QStandardItemModel *model, int row);//下载PLU信息
    //    static QByteArray setLABEL_40x30_step1();
    //    static QByteArray setLABEL_40x30_step2();
    static QStringList scaleName();
    static QMap<int, QString>WeighType();
    static QStringList BarType();
    static bool barcode_scale_resolve(QString &barcode, QString &price, QString &weight, QString &amount, const QString &code, const QString &type);
    static QString barcode_default_port();
private:
    static QMap<int, QString> _wtype;
    static QStringList _btype;

    static QByteArray _tx1(const QByteArray &data);
    static QByteArray _tx2(const QByteArray &data);
    static QByteArray _tx7(const QByteArray &data);

    static QString _price(const QString &data);
};

#endif // ELECTRONICSCALE_YSHENG_H
