#ifndef ELECTRONICSCALE_CAS_H
#define ELECTRONICSCALE_CAS_H
/*
W02A                    W是写入，02A是PLU信息
【2】00004,                  [5]PLU序号

【1】01                      [2]部门号
L0198:                  [4]datasize  0x198 = 408(后面的字节数)

【3】F=0A.53.0006:BANANA     [2][2][4][n..] PLU类型；4；M；1（最大size） ；从计重，计数，件数中选择PLU类型

【4】F=0A.53.0006:BANANA     [2][2][4][n..] 名称1；10；S；40（最大size）

【5】F=0A.53.0006:BANANA     [2][2][4][n..] 名称2；30；S；40（最大size）

【6】F=0A.53.0006:BANANA     [2][2][4][n..] 名称3；31；S；40（最大size）

【7】F=0A.53.0006:BANANA     [2][2][4][n..] 组编号；9；W；2（最大size）

【8】F=0A.53.0006:BANANA     [2][2][4][n..] 外部条形码；49；S；20（最大size）

【9】F=0A.53.0006:BANANA     [2][2][4][n..] 标签号；80；W；2（最大size）

【10】F=0A.53.0006:BANANA     [2][2][4][n..] 附加标签号；81；W；2（最大size）

【11】F=0A.53.0006:BANANA     [2][2][4][n..] 原产地编号；55；W；3（最大size）//origin no.

【12】F=0A.53.0006:BANANA     [2][2][4][n..] 固定重量；100；L；6（最大size）//fixed weight

【13】F=0A.53.0006:BANANA     [2][2][4][n..] 预固定；3；S；2（最大size）//prefix

【14】F=0A.53.0006:BANANA     [2][2][4][n..] 商品代码；11；L；6（最大size）

【15】F=0A.53.0006:BANANA     [2][2][4][n..] 使用固定价格类型；26；B；1（最大size）

【16】F=0A.53.0006:BANANA     [2][2][4][n..] 件数；14；W；3（最大size）   //pieces

【17】F=0A.53.0006:BANANA     [2][2][4][n..] 数量单位编号；15；B；1（最大size） //qty unit no

【18】F=0A.53.0006:BANANA     [2][2][4][n..] （单）价格；6；L；7（最大size）

【19】F=0A.53.0006:BANANA     [2][2][4][n..] 特殊价格；91；L；7（最大size）

【20】F=0A.53.0006:BANANA     [2][2][4][n..] 税号；8；B；1（最大size） //tax no.

【21】F=0A.53.0006:BANANA     [2][2][4][n..] 皮重；13；L；6（最大size）
【21】F=0A.53.0006:BANANA     [2][2][4][n..] 皮重/皮重编号；12；B；（最大size）

【22】F=0A.53.0006:BANANA     [2][2][4][n..] %皮重；24；W；5（最大size）

【23】F=0A.53.0006:BANANA     [2][2][4][n..] 皮重%限额；23；W；5（最大size） //tare % limit

【24】F=0A.53.0006:BANANA     [2][2][4][n..] 条形码 编号；85；W；2（最大size）

【25】F=0A.53.0006:BANANA     [2][2][4][n..] 条形码2 编号；86；W；2（最大size）

【26】F=0A.53.0006:BANANA     [2][2][4][n..] 图像编号；36；B；1（最大size） //picture no.

【27】F=0A.53.0006:BANANA     [2][2][4][n..] 生产日期；20；W；3（最大size）

【28】F=0A.53.0006:BANANA     [2][2][4][n..] 包装日期；18；W；3（最大size）

【29】F=0A.53.0006:BANANA     [2][2][4][n..] 包装时间；19；B；2（最大size）

【30】F=0A.53.0006:BANANA     [2][2][4][n..] 保质期；16；L；4（最大size）//sell by date

【31】F=0A.53.0006:BANANA     [2][2][4][n..] 出售时间；17；B；2（最大size）sell by time

【32】F=0A.53.0006:BANANA     [2][2][4][n..] 制作日期；22；W；4（最大size）//cookbydate

【33】F=0A.53.0006:BANANA     [2][2][4][n..] 原料编号；25；W；4（最大size）//ingredient no.

【34】F=0A.53.0006:BANANA     [2][2][4][n..] 可追溯编号；35；W；3（最大size）//tracebility no.

【35】F=0A.53.0006:BANANA     [2][2][4][n..] 奖金；50；W；3（最大size）

【36】F=0A.53.0006:BANANA     [2][2][4][n..] 营养元素编号；70；W；3（最大size）//mutrifact no.？？？？

【37】F=0A.53.0006:BANANA     [2][2][4][n..] 销售信息编号；90；B；2（最大size）//sales msg no.

【38】F=0A.53.0006:BANANA     [2][2][4][n..] 部门PLU参数；71；W；2（最大size）//reference plu dept

【39】F=0A.53.0006:BANANA     [2][2][4][n..] PLU编号参数；69；L；5（最大size）//reference plu no.

【40】F=0A.53.0006:BANANA     [2][2][4][n..] 双部分PLU；64；W；2（最大size） //coupled plu1 dept

【41】F=0A.53.0006:BANANA     [2][2][4][n..] 双PLU编号；68；L；5（最大size） //coupled plu1 no.

【42】F=0A.53.0006:BANANA     [2][2][4][n..] #关联PLU；60；B；1（最大size） // #of linkplu

。
。
。
6                       [1]内容部分的xor
*/

/*cas 条码的长度最大为6个，超过为空，不超过没变化*/
/*条码格式设置在CL-works里->[1]->[表1]添加条码->选择类型；DDIIIIIPPPPPC（部门号-项目代码-总金额-校验位）*/
/*不同的部门，需要在1830 修改对应的部门号 才可以 查看plu信息*/
/*设置标签时，需要修改标签号一致才能下载到下位机里*/
/*进入菜单的步骤是：menu+011+menu*/
/*peel-of ：走纸即可解决*/
/*称重方式 == plu类型 ：称重，数量，个*/
#include <QStandardItemModel>
#include <QMap>

class ElectronicScale_CAS
{
public:
    explicit ElectronicScale_CAS();

    ~ElectronicScale_CAS();

    static QByteArray clearPLU(const QString &plu, const QString &dept_no);//清PLU记录

    static QByteArray downloadPLUINFO(const QString &plu
                                       , const QString &dept_no
                                       , const QString &barcode
                                       , const QString &goodsname
                                       , const QString &price
                                       , const QString &baozhiqi
                                      , const QString &plu_type);//下载PLU信息

    static QStringList scaleName();
    static QMap<int, QString> WeighType();
    static QStringList BarType();
    static bool barcode_scale_resolve(QString &barcode, QString &price, QString &weight, QString &amount, const QString &code, const QString &type);
    static QString barcode_default_port();
private:
    static QMap<int, QString> _wtype;
    static QStringList _btype;
    static QByteArray addInstruct_str(int identify,char type,int maxsize,const QString &content);
    static QByteArray addInstruct_digit(int identify,char type,int maxsize,const QString &content);

    static QByteArray int2numhex_0(const QString &begin, int num);
    static QByteArray int2numhex_0(int begin, int num);

    static QByteArray byte2numbyte(const QByteArray &byte, int num);
};

#endif // ELECTRONICSCALE_CAS_H
