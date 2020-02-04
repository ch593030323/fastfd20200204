#ifndef W_SYS_MANAGE_WIRELESSORDERMONITORING_H
#define W_SYS_MANAGE_WIRELESSORDERMONITORING_H

#include <QDialog>
#include <QUdpSocket>
#include "lds_query_hddpos.h"
#include <QMap>
#if defined (Q_OS_WIN)
#include "windows.h"
#endif
#include "fexpandmain_model_sqltablemodel_data.h"

///扫码棒，仅限于window下使用

class w_sys_manage_wirelessordermonitoring_box;
#define _TXT_jz_txt w_sys_manage_wirelessordermonitoring_box::_TXT_jz_txt_str()
#define _TXT_dcj_txt w_sys_manage_wirelessordermonitoring_box::_TXT_dcj_txt_str()
#define _TXT_jdmc_txt w_sys_manage_wirelessordermonitoring_box::_TXT_jdmc_txt_str()
#define _TXT_cpb_txt w_sys_manage_wirelessordermonitoring_box::_TXT_cpb_txt_str()
#define _TXT_cplbb_txt w_sys_manage_wirelessordermonitoring_box::_TXT_cplbb_txt_str()
#define _TXT_bfmcb_txt w_sys_manage_wirelessordermonitoring_box::_TXT_bfmcb_txt_str()
#define _TXT_tclyb_txt w_sys_manage_wirelessordermonitoring_box::_TXT_tclyb_txt_str()
#define _TXT_khyqb_txt w_sys_manage_wirelessordermonitoring_box::_TXT_khyqb_txt_str()
/*
1. 状态，设置方法为基站背后的小开关1,朝上为正常工作状态，朝下为硬测试状态。
3. 基站的信道号0-7，设置方法为基站背后的小开关234，其对照关系见基站底部的对照表特别注意：基站后面的小开关变动后需要重新上电才会起作用。使用WX中的测试功能进行测试时（一般叫软测试）小开关1朝上。
*/

class QStandardItemModel;
class Ui_w_sys_manage_wirelessordermonitoring_dialog;
//plsql
class w_sys_manage_wirelessordermonitoring : public QDialog
{
    Q_OBJECT
public:
    static QString checkdata(const QString &data);
    static void initdata(QWidget *parent, const QString &dir);
public:
    explicit w_sys_manage_wirelessordermonitoring(QWidget *parent = 0);
    void initjizhan();
    void initdiancaiji();

    bool savejizhan();
    bool savediancaiji();

    static bool startjz(QWidget *parent);//启动
    static void showjz();//显示
    static void closejz();//关闭
private slots:
    void todel();
    void todel2();
    void tonew();
    void tonew2();
    void toexit();
    void toinit();
    void tosave();

    void toshowjz();
    void tostartjz();
    void tosavestart(bool f);
    void towxclose();
private:
    Ui_w_sys_manage_wirelessordermonitoring_dialog *ui;
    QStandardItemModel *standmodel;
    QStandardItemModel *standmodel_2;
    static QString dir;
};

//class w_sys_manage_wirelessordermonitoring_wx:public QObject{
//    Q_OBJECT
//public:
//    explicit w_sys_manage_wirelessordermonitoring_wx(const QString &db, QObject *parent = 0);
//    QString _db;
//signals:
//    void signalwxcurstepinfo(const QString &info);
//public slots:
//    void toKJ(const QByteArray &request);//开机
//    void toDL(const QByteArray &request);//登录
//    void toKT(const QByteArray &request);//开台
//    void toCT(const QByteArray &request);//撤台
//    void toDC(const QByteArray &request);//点菜
//    void toTC(const QByteArray &request);//退货
//    void toDYD(const QByteArray &request);//打印结账单
//    void toKXHZ(const QByteArray &request);//空闲餐台汇总
//    void toLBKX(const QByteArray &request);//按类别查空闲
//    void toTHKX(const QByteArray &request);//按台号查空闲toTHKX
//    void toZDCX(const QByteArray &request);//  账单查询

//    void toZZCC(const QByteArray &request);//  整桌催起
//    void toCLCC(const QByteArray &request);//  商品类别催起
//    void toCPCC(const QByteArray &request);//  商品催起

//    //KXHZ 002"  //空闲餐台汇总
//    //"LBKX 002"  //按类别查空闲
//    //THKX 002"  //按台号查空闲

////    "ZZCC 002" "1001   " 1578    整桌催起
////   "CLCC 002" "1001    01        " 1578   商品类别
////    "CPCC 002" "1001    10010" 1578   商品

//    //整桌催起(ZZCC)、按商品催起(CPCC)

//private:
//    bool tocheckoperidRight(lds_query_hddpos  &query,const QString &dcjno_3, const QString &rightno = "");
//public slots:
//    void processPendingDatagrams();

//    void basestationT(const QByteArray &request, const QHostAddress &host, quint16 port);
////    void basestationT2();
////    void basestationT3();
////    void basestationT4();

////    void basestationT5();
////    void basestationT6();
////    void basestationT7();
////    void basestationT8();
//public:
//    QString lxcode_4;//类型码
//    QString dcjno_3;//点菜机机
//    QString jqsbh_10;//机器设备号
//    QString operid_4;//工号
//    QString opername_8;//服务员姓名
//    QString operpwd_8;//密码
//    QString tableno_4;//台号
//    QString tablepersonnum_2;//客人数量
//    QString tableservant_5;//服务员号
//    QString tableserial_3;//账单流水号

//    QString dir;
//    QMap<QString, QString> mapkv;
//    QMap<QString, QString>::iterator mapitr;
//    QUdpSocket udpsocket;
//};

class w_sys_manage_wirelessordermonitoring_box : public QObject {
    Q_OBJECT
public:
    enum androidFlag{
        SUCCESS = 0x12,
        HAS_DIVED = 0x13,
        DB_ERROR = 0x14,
        QUITNUM_MORE_THAN_TOTAL = 0x15,
        TABLE_NOT_OPEN = 0x16,

    };
    static void writeLog(const QString &/*byte*/);
public:
    explicit w_sys_manage_wirelessordermonitoring_box(QObject *parent = 0);

//    QString lxcode_4;//类型码
//    QString dcjno_3;//点菜机机
//    QString jqsbh_10;//机器设备号
//    QString operid_4;//工号
    QMap<QString, QString> dcj_opid_map;

//    QString opername_8;//服务员姓名
    QString operpwd_8;//密码
//    QString tableno_4;//台号
//    QString tablepersonnum_2;//客人数量
//    QString tableservant_5;//服务员号
//    QString tableserial_3;//账单流水号
public slots:
    void KJ(int baseNo);//开机
    void DL(int baseNo);//点菜机登录
    void DL10();//管理软件登录
    void SJZB11();//数据下载准备，发送11
    void KT(int baseNo, const QString err = "");//开台
    void CT(int baseNo);//撤台
    void DC(int baseNo, const QString err = "");//点菜
    void DC();
    void TC(int baseNo);//退货
    void DXFD(int baseNo);//打印结账单
    void KXHZ(int baseNo);//空闲餐台汇总
    void LBKX(int baseNo);//按类别查空闲
    void THKX(int baseNo);//按台号查空闲toTHKX
    void ZDCX(int baseNo);//  账单查询

    void ZZCC(int baseNo);//  整桌催起
    void CLCC(int baseNo);//  商品类别催起
    void CPCC(int baseNo);//  商品催起
public:
    static QString _TXT_jz_txt_str();
    static QString _TXT_dcj_txt_str();
    static QString _TXT_jdmc_txt_str();
    static QString _TXT_cpb_txt_str();
    static QString _TXT_cplbb_txt_str();
    static QString _TXT_bfmcb_txt_str();
    static QString _TXT_tclyb_txt_str();
    static QString _TXT_khyqb_txt_str();

    bool writeback(const QByteArray &d, int baseNo);
    bool quitdish(QString &errstring, const QString &ch_billno, const QString &ch_dishno, double dishnum_back_total, const QString &vch_reason, const QString &operid, QList<num_backData> &d_list);

protected:
    virtual void after(int baseNo, bool f, const QString &info) = 0;
};

class w_sys_manage_wirelessordermonitoring_wx_bar : public w_sys_manage_wirelessordermonitoring_box{
    Q_OBJECT
public:
    explicit w_sys_manage_wirelessordermonitoring_wx_bar(QObject *parent = 0);
    virtual ~w_sys_manage_wirelessordermonitoring_wx_bar();
    unsigned long  wxhandle;

public slots:
    void tonative(unsigned long  lParam);
public:
    void basestationT1();
protected:
    void after(int baseNo, bool f, const QString &info);
    virtual void KT(int baseNo);//开台
    virtual void DC(int baseNo);//点菜
    bool stepf;
    QString stepinfo;
};

///嵌入下的基站
class w_sys_manage_wirelessordermonitoring_wx:public w_sys_manage_wirelessordermonitoring_box{
    Q_OBJECT
public:
    explicit w_sys_manage_wirelessordermonitoring_wx(QObject *parent = 0);
    void enableUDP();
    void enableWindowSLOT();
private slots:
    void processPendingDatagrams();
    void basestationTUDP(const QByteArray &request, const QHostAddress &host, quint16 port);
    void basestationTAndroid(const QByteArray &request, const QHostAddress &host, quint16 port);
    void basestationTDCJ(int baseNo);

protected:
    void after(int baseNo, bool f, const QString &info);
private:
    void record_wx_request(const QByteArray &d);
    QByteArray request_tmp;
    QUdpSocket udpsocket;

    //OS_WIN
signals:
    void signalNative(unsigned long lParam);
public slots:
    void toNative(unsigned long lParam);
public:
    static unsigned long  wxhandle;
    //~OS_WIN
};
#endif // W_SYS_MANAGE_WIRELESSORDERMONITORING_H
