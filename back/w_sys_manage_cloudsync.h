#ifndef W_SYS_MANAGE_CLOUDSYNC_H
#define W_SYS_MANAGE_CLOUDSYNC_H

#include <QDialog>
#include <QVariant>
#include <QString>
#include <QMap>
#include "lds_query_hddpos.h"
#include "lds_network.h"

//云端的
//支付号 中餐 快餐 =======  F
//订单号 中餐         =======  Z
//订单号 快餐         =======  K
//外卖             -cey_u_togo_guest     C[ch_guestid]
//                     -cey_u_master           Q[ch_billno]
//                     -cey_u_table               Q[ch_billno]
//                     -cey_u_togo                T[ch_togono] Q[ch_billno]
//                     -cey_u_orderdish       Q[ch_billno]

//软件本身
//支付号 中餐 快餐 ======= P
//订单号 中餐         =======  B
//订单号 快餐         ======= Q
//外卖             -cey_u_togo_guest     C[ch_guestid]
//                     -cey_u_master           Q[ch_billno]
//                     -cey_u_table               Q[ch_billno]
//                     -cey_u_togo                W[ch_togono] Q[ch_billno]
//                     -cey_u_orderdish       Q[ch_billno]
struct orderMaster{
    //
    QString omChBillNo;
    QString omChState;
    QString omVchOperId;
    QString omDtOperDate;
    QString omVchOperId2;
    QString omDtOperDate2;
    QString omVchMemo;
    QString orgCode;
};
struct checkOutMaster{
    checkOutMaster():
        comNumCost(0)
      , comNumDiscount(0)
      , comNumPresent(0)
      , comNumService(0)
      , comNumLowCost(0)
      , comNumBlotout(0)
      , comNumFree(0)
      , comNumCash(0)
      , comNumOther(0)
      , comNumRun(0)
      , comNumRate(0)
    {}
    //
    QString comChPayNo;
    QString comChBillNo;
    QString comChTableNo;
    float comNumCost;
    float comNumDiscount;
    float comNumPresent;
    float comNumService;
    float comNumLowCost;
    float comNumBlotout;
    float comNumFree;
    QString comVchFreeOperId;
    float comNumCash;
    float comNumOther;
    float comNumRun;
    QString comVchRunOperId;
    QString comChState;
    QString comVchOperId;
    QString comDtOperDate;
    QString comVchOperId2;
    QString comDtOperDate2;
    QString comVchMemberNo;
    QString comChType;
    QString orgCode;
    float comNumRate;
};
struct memberPointList{
    memberPointList():
        mpIntFlowId(0)
      ,mpNumPoint(0) {}

    float mpIntFlowId;
    QString mpVchMemberNo;
    QString mpChType;
    QString mpChPayNo;
    float mpNumPoint;
    QString mpVchMemo;
    QString mpVchOperId;
    QDateTime mpDtOperDate;

    QString orgCode;
};
struct memberPay{
    memberPay():
        intFlowid(0),
        mpNumAmount(0),
        mpNumPay(0),
        mpIntCheckFlow(0)
    {}

    float intFlowid;
    QString mpVchMemberNo;
    QString mpChPayNo;
    QString mpChPayModeNo;
    float mpNumAmount;
    float mpNumPay;
    QString mpVchOperId;
    QDateTime mpDtOperDate;
    QString mpChBalanceNo;
    float mpIntCheckFlow;
    QString orgCode;
};
struct memberCuramount{
    memberCuramount():
        numAmount(0),
        numHangamount(0),
        numPoint(0)
    {}

    QString vchMemberno;
    float numAmount;
    float numHangamount;
    float numPoint;
    QString vchOperId;
    QDateTime dtOperdate;
    QString chBranchno;
    QString orgCode;
};

struct tableRecordsConsumption{
    tableRecordsConsumption():
        intPerson(0),
        numCost(0),
        numTransService(0)
    {}

    QString chBillno;
    QString chTableno;
    float intPerson;
    QString vchMemo;
    QString chState;
    QString chPayno;
    QString vchOperId;
    QString dtOperdate;
    QString vchOperId2;
    QString dtOperdate2;
    QString chOrderState;
    float numCost;
    QString vchWaiter;
    float numTransService;
    QString orgCode;
};
struct orderDish{
    orderDish():
        bdIntFlowId(0),
        bdIntId(0),
        bdNumPrice(0),
        bdNumPriceOrg(0),
        bdNumPriceAdd(0),
        bdNumNum(0),
        bdNumBack(0),
        bdIntDiscount(0)
    {}

    float bdIntFlowId;
    QString bdChBillNo;
    QString bdChTableNo;
    float bdIntId;
    QString bdChDishNo;
    float bdNumPrice;
    float bdNumPriceOrg;
    float bdNumPriceAdd;
    float bdNumNum;
    float bdNumBack;
    QString bdVchBackOperId;
    QString bdDtBackOperDate;
    float bdIntDiscount;
    QString bdVchDisOperId;
    QString bdDtDisOperDate;
    QString btVchMemberNo;
    QString bdChConsult;
    QString bdChPrintFlag;
    QString bdVchPrintMemo;
    QString dcChSuitFlag;
    QString dcChSuitNo;
    QString bdChSpecialFlag;
    QString chPresentFlag;
    QString bdVchPreOperId;
    QString bdDtPreOperDate;
    QString bdVchOperId;
    QString bdDtOperDate;
    QString bdChTogoNo;
    QString bdChPayNo;
    QString bdChOutFlag;
    QString orgCode;
};
struct checkOutDetail{
    checkOutDetail():
        codIntFlowId(0),
        codNumPayAmount(0),
        codNumChange(0),
        codNumRealAmount(0),
        codNumFace(0),
        codNumCardRemain(0),
        codNumFaceNum(0)
    {}
    float codIntFlowId;
    QString codChPayNo;
    QString codChPayModeNo;
    float codNumPayAmount;
    float codNumChange;
    float codNumRealAmount;
    float codNumFace;
    QString codVchVoucherNo;
    QString codVchMemo;
    QString codVchOperId;
    QString codDtOperDate;
    float codNumCardRemain;
    float codNumFaceNum;

    QString orgCode;
};
struct memberDeposit{
    memberDeposit():
        mdIntFlowId(0),
        mdNumDeposit(0),
        mdNumRealaMount(0),
        mdRemainAmt(0)
    {}

    float mdIntFlowId;
    QString mdVchMemberNo;
    float mdNumDeposit;
    float mdNumRealaMount;
    QString mdChDepositMode;
    QString mdChPayMode;
    QString mdVchOperId;
    QDateTime dtOperDate;
    QString mdVchMemo;
    float mdRemainAmt;
    QString comChPayNo;
    QString orgCode;
};

struct membernewparam{
    membernewparam():
        numLimit(0),
        miIntBasedata(0)
    {}

//    QString nosn;
    QString miVchMemberNo;
    QString miVchMember;
    QString miChTypeNo;
    QString miVchId;

    QString miDtBirthday;
    QString miVchTel;
    QString miVchHandTel;
    QString miVchAddress;
    QString miDtLimit;

    QString miChState;
    QString miChStateStr;
    qlonglong numLimit;
    QString miChCardFlag;
    QString miVchCardNo;

    QString miVchPassword;
    QString miVchOperId;
    QString miDtOperDate;
    qlonglong miIntBasedata;
    QString miIntBasedataStr;

    QString miDtSendTime;
    QString orgCode;
    QString remark;
    QString idStr;

    QString operate;
};

//会员接口检索
struct yunMemberSelectInfo{
    QString mname;
    QString mhandtel;
    QString mcardno;
};

//云端时间返回参数：时间、是否打印云端尾语，尾语
struct yunTime{
    QDateTime time;
    QString printCheck;
    QString printContext;
};
struct yunpayData{
    QString code;
    QString message;
    QString maxOrderNo;
};
class httpDataAutoDel{
public:
    httpDataAutoDel();
    httpDataAutoDel(cJSON *json);
    //
    QByteArray jsonToByteArray();
    QString jsonToString();

    //
    QSharedPointer<cJSON> json;
    int httpStatus;//success:200
    QString message;
    QString code;
};

class member_up_yun{

public :
    member_up_yun():
        isrun(false) {}
public:
    QList<memberPointList> memberPointListList;
    QList<memberPay> memberPayList;
    QList<memberCuramount> memberCuramountList;
    QList<memberDeposit> memberDepositList;
    bool isrun;
    QString ch_payno;
    QString ch_billno;
    void clearallList();

public:
    ///x4 会员
    void appitem(const memberPointList &o) {memberPointListList.append(o);}
    void appitem(const memberPay &o) {memberPayList.append(o);}
    void appitem(const memberCuramount &o) {memberCuramountList.append(o);}
    void appitem(const memberDeposit &o) {memberDepositList.append(o);}
    int finditem(const memberCuramount &o);

    cJSON *q_u_get_piece_cey_u_checkout_master(lds_query_hddpos  &query, const QString &orgCode);
    cJSON *q_u_get_piece_cey_u_master(lds_query_hddpos  &query, const QString &orgCode);
    cJSON *q_u_get_piece_cey_u_table(lds_query_hddpos  &query, const QString &orgCode);
    cJSON *q_u_get_piece_cey_u_orderdish(lds_query_hddpos  &query, const QString &orgCode);
    cJSON *q_u_get_piece_cey_u_checkout_detail(lds_query_hddpos  &query, const QString &orgCode);

    cJSON *make_JSON_Array(const memberPointList &p, const QString &orgCode);
    cJSON *make_JSON_Array(const memberPay &p, const QString &orgCode);
    cJSON *make_JSON_Array(const memberCuramount &p, const QString &orgCode);
    cJSON *make_JSON_Array(const memberDeposit &p, const QString &orgCode);

};

/**
16、连锁会员付款
http://192.168.1.81/cvs/member/fuk-rest
method: POST
head orgCode:000000060002
    head userName:admin
    head password:123456
    head flag:Q

    参数
{
"orderMaster":{"sn":null,"omChBillNo":null,"omChState":null,"omVchOperId":null,"omDtOperDate":null,"omVchOperId2":null,"omDtOperDate2":null,"omVchMemo":null,"orgCode":null,"date":null},

"checkOutMaster":{"sn":null,"comChPayNo":null,"comChBillNo":null,"comChTableNo":null,"comNumCost":null,"comNumDiscount":null,"comNumPresent":null,"comNumService":null,"comNumLowCost":null,"comNumBlotout":null,"comNumFree":null,"comVchFreeOperId":null,"comNumCash":null,"comNumOther":null,"comNumRun":null,"comVchRunOperId":null,"comChState":null,"comVchOperId":null,"comDtOperDate":null,"comVchOperId2":null,"comDtOperDate2":null,"comVchMemberNo":null,"comChType":null,"orgCode":null,"date":null,"comNumRate":null},

"memberPointList":[
{"sn":null,"mpIntFlowId":null,"mpVchMemberNo":null,"mpChType":null,"mpChPayNo":null,"mpNumPoint":null,"mpVchMemo":null,"mpVchOperId":null,"mpDtOperDate":null,"orgCode":null,"date":null},
{"sn":null,"mpIntFlowId":null,"mpVchMemberNo":null,"mpChType":null,"mpChPayNo":null,"mpNumPoint":null,"mpVchMemo":null,"mpVchOperId":null,"mpDtOperDate":null,"orgCode":null,"date":null}
],

"memberPay":[
{"sn":null,"intFlowid":null,"mpVchMemberNo":null,"mpChPayNo":null,"mpChPayModeNo":null,"mpNumAmount":null,"mpNumPay":null,"mpVchOperId":null,"mpDtOperDate":null,"mpChBalanceNo":null,"mpIntCheckFlow":null,"orgCode":null,"date":null},
{"sn":null,"intFlowid":null,"mpVchMemberNo":null,"mpChPayNo":null,"mpChPayModeNo":null,"mpNumAmount":null,"mpNumPay":null,"mpVchOperId":null,"mpDtOperDate":null,"mpChBalanceNo":null,"mpIntCheckFlow":null,"orgCode":null,"date":null}
],

"memberCuramount":[
{"sn":null,"vchMemberno":null,"numAmount":null,"numHangamount":null,"numPoint":null,"vchOperId":null,"dtOperdate":null,"chBranchno":null,"orgCode":null,"date":null},
{"sn":null,"vchMemberno":null,"numAmount":null,"numHangamount":null,"numPoint":null,"vchOperId":null,"dtOperdate":null,"chBranchno":null,"orgCode":null,"date":null}
],

"tableRecordsConsumption":{"sn":null,"chBillno":null,"chTableno":null,"intPerson":null,"vchMemo":null,"chState":null,"chPayno":null,"vchOperId":null,"dtOperdate":null,"vchOperId2":null,"dtOperdate2":null,"chOrderState":null,"numCost":null,"vchWaiter":null,"numTransService":null,"orgCode":null,"date":null},

"orderDish":[
{"sn":null,"bdIntFlowId":null,"bdChBillNo":null,"bdChTableNo":null,"bdIntId":null,"bdChDishNo":null,"bdNumPrice":null,"bdNumPriceOrg":null,"bdNumPriceAdd":null,"bdNumNum":null,"bdNumBack":null,"bdVchBackOperId":null,"bdDtBackOperDate":null,"bdIntDiscount":null,"bdVchDisOperId":null,"bdDtDisOperDate":null,"btVchMemberNo":null,"bdChConsult":null,"bdChPrintFlag":null,"bdVchPrintMemo":null,"dcChSuitFlag":null,"dcChSuitNo":null,"bdChSpecialFlag":null,"chPresentFlag":null,"bdVchPreOperId":null,"bdDtPreOperDate":null,"bdVchOperId":null,"bdDtOperDate":null,"bdChTogoNo":null,"bdChPayNo":null,"bdChOutFlag":null,"orgCode":null,"date":null},
{"sn":null,"bdIntFlowId":null,"bdChBillNo":null,"bdChTableNo":null,"bdIntId":null,"bdChDishNo":null,"bdNumPrice":null,"bdNumPriceOrg":null,"bdNumPriceAdd":null,"bdNumNum":null,"bdNumBack":null,"bdVchBackOperId":null,"bdDtBackOperDate":null,"bdIntDiscount":null,"bdVchDisOperId":null,"bdDtDisOperDate":null,"btVchMemberNo":null,"bdChConsult":null,"bdChPrintFlag":null,"bdVchPrintMemo":null,"dcChSuitFlag":null,"dcChSuitNo":null,"bdChSpecialFlag":null,"chPresentFlag":null,"bdVchPreOperId":null,"bdDtPreOperDate":null,"bdVchOperId":null,"bdDtOperDate":null,"bdChTogoNo":null,"bdChPayNo":null,"bdChOutFlag":null,"orgCode":null,"date":null}
 ],

"checkOutDetail":[
{"sn":null,"codIntFlowId":null,"codChPayNo":null,"codChPayModeNo":null,"codNumPayAmount":null,"codNumChange":null,"codNumRealAmount":null,"codNumFace":null,"codVchVoucherNo":null,"codVchMemo":null,"codVchOperId":null,"codDtOperDate":null,"codNumCardRemain":null,"codNumFaceNum":null,"orgCode":null,"date":null},
{"sn":null,"codIntFlowId":null,"codChPayNo":null,"codChPayModeNo":null,"codNumPayAmount":null,"codNumChange":null,"codNumRealAmount":null,"codNumFace":null,"codVchVoucherNo":null,"codVchMemo":null,"codVchOperId":null,"codDtOperDate":null,"codNumCardRemain":null,"codNumFaceNum":null,"orgCode":null,"date":null}
],

"memberDeposit":{"sn":null,"mdIntFlowId":null,"mdVchMemberNo":null,"mdNumDeposit":null,"mdNumRealaMount":null,"mdChDepositMode":null,"mdChPayMode":null,"mdVchOperId":null,"dtOperDate":null,"mdVchMemo":null,"mdRemainAmt":null,"orgCode":null,"date":null}

}

                                          返回结果

                                          200
                                        {
                                            "message": "ok"
                                        }

                                                         550
                 message : 错误信息
                                                         */

/**
    cJSON *root,*fmt;
    root=cJSON_CreateObject();
    cJSON_AddItemToObject(root, "name", cJSON_CreateString("Jack (\"Bee\") Nimble"));
    cJSON_AddItemToObject(root, "format", fmt=cJSON_CreateObject());
    cJSON_AddStringToObject(fmt,"type",     "rect");
    cJSON_AddNumberToObject(fmt,"width",        1920);
    cJSON_AddNumberToObject(fmt,"height",       1080);
    cJSON_AddFalseToObject (fmt,"interlace");
    cJSON_AddNumberToObject(fmt,"frame rate",   24);


派生到我的代码片

    {
        "name": "Jack (\"Bee\") Nimble",
        "format": {
            "type":       "rect",
            "width":      1920,
            "height":     1080,
            "interlace":  false,
            "frame rate": 24
        }
    }


    cJSON *root;root=cJSON_CreateObject();
    cJSON_AddItemToObject(root, "name", cJSON_CreateString("Jack (\"Bee\") Nimble"));

    cJSON *name1;cJSON_AddItemToObject(root, "name1", name1=cJSON_CreateObject());
    cJSON_AddStringToObject(name1, "name2", "h1");
    cJSON_AddStringToObject(name1, "name3", "h1");


    cJSON *fmt;cJSON_AddItemToObject(root, "format", fmt=cJSON_CreateArray());
    cJSON *fmt1;cJSON_AddItemToObject(fmt, "fmt1", fmt1=cJSON_CreateObject());
    cJSON_AddStringToObject(fmt1,"fmt2",     "rect");
    cJSON_AddStringToObject(fmt1,"fmt3",     "rect");
    cJSON *fmt2;cJSON_AddItemToObject(fmt, "fmt1", fmt2=cJSON_CreateObject());
    cJSON_AddStringToObject(fmt2,"fmt4",     "rect");
    cJSON_AddStringToObject(fmt2,"fmt5",     "rect");
    //cJSON_Print(root);
*/

class w_sys_manage_cloudsync : public QObject
{
    Q_OBJECT
public:
    explicit w_sys_manage_cloudsync();
    ~w_sys_manage_cloudsync();

public:
    //! 会员部分
    static httpDataAutoDel get_yun_httppos(QWidget *parent, const QString &http_key,
                                                                    const QByteArray &param,
                                                                    const QMap<QString, QString> &headInfo,
                                                                    QString &retparam, //返回json数据
                                                                    const QString &msgtitle,
                                                                    bool ispop=true,
                                                                    bool timecheck=true,
                                                                    int mesc = 20*60*1000);
    static httpDataAutoDel get_yun_httppos(QWidget *parent, const QString &http_key,
                                                                    const QByteArray &param,
                                                                    const QMap<QString, QString> &headInfo,
                                                                    const QString &msgtitle,
                                                                    bool ispop=true,
                                                                    bool timecheck=true,
                                                                    int mesc = 20*60*1000);

    static httpDataAutoDel get_yun_httpget( QWidget *parent,const QString &http_key,
                                  const QMap<QString, QString> &headInfo,
                                  QString &retparam,//返回json数据
                                  const QString &msgtitle,
                                  bool ispop=true,
                                  bool timecheck=true,
                                  int mesc = 20*60*1000
            );
    static httpDataAutoDel get_yun_httpget( QWidget *parent,const QString &http_key,
                                  const QMap<QString, QString> &headInfo,
                                  const QString &msgtitle,
                                  bool ispop=true,
                                  bool timecheck=true,
                                  int mesc = 20*60*1000
            );
    ///////////////////////////////////////////////////////
public:

    static QString print_yun(cJSON *print);
    static void printalljson_child(cJSON *print);
    //获取会员参数
    static QString get_yun_member_parameter(QWidget *parent, const QString &key, bool *ok= 0);
    static bool get_yun_member_parameter(const QString &key, QString &retValue, QString &errstring);
    //获取会员读卡
    static QString get_yun_member_readcard(QWidget *parent, const QString cardNo, const QString &operId, const QString &key, bool &ok, const QString &def="");

    static bool  get_yun_member_readcard(const QString &cardNo, const QString &operId, const QString &key, QVariant &retValue, QString &retErrstring);
    static bool  get_yun_member_readcard(const QString &cardNo, const QString &operId, const QString &key, QString &retValue, QString &retErrstring);
    static bool  get_yun_member_readcard(const QString &cardNo, const QString &operId, const QString &key, int &retValue, QString &retErrstring);
    //获取会员最大订单号/*获取单号，中餐是Z；快餐是F*/
    static bool get_yun_member_maxorderno(const QString &flag, QString &maxOrderNo,QString &errstring);
    //获取会员最大支付号
    static bool get_yun_member_maxmasterno(const QString &flag, QString &maxMasterNo, QString &errstring);
    //获取会员类型
    static QString get_yun_member_membertype(QWidget *parent, const QString &typeNo, const QString &key, bool &ok, const QString &def="");
    static bool  get_yun_member_membertype(const QString &typeNo, const QString &key, QString &retValue, QString &retErrstring);

    //获取会员类型
    static bool get_yun_member_membertypelist(QWidget *parent, QMap<QString, QVariant> &retlist, QString *errstring = 0);
    static bool get_yun_member_membertypelist(QMap<QString, QVariant> &retlist, QString &errstring);
    //提交关于会员的信息
    static yunpayData upload_yun_pay_info(QWidget *parent, member_up_yun &muy, const QString &ch_payno );
    //提交关于会员的充值
    static bool get_yun_member_recharge(QWidget *parent, const QString &cardNo, const QString &operId, const QString &chongzje, const QString &shisje, const QString &zhiffs);
    //提交关于会员的退卡
    static bool get_yun_member_blankcard(QWidget *parent, const QString &cardNo, const QString &tuikje, const QString &zhiffs);
    //提交关于会员的取款
    static bool get_yun_member_fetch(QWidget *parent, const QString &cardNo, const QString &operId, const QString &qukje, const QString &zhiffs);
    //提交关于会员的新增
    static bool get_yun_member_save(QWidget *parent, const membernewparam &value);
    static bool get_yun_member_save(const membernewparam &value, QString &errstring);
    //提交关于会员的新增
    static bool get_yun_member_send(QWidget *parent, const QString &cardNo);
    static bool get_yun_member_send(const QString &cardNo, QString &errstring);
    //根据条件检索会员
    static bool get_yun_member_select_pre100(QWidget *parent, const QString &filter/*卡号和手机号*/,QList<yunMemberSelectInfo > &selects);
    //根据条件检索会员
    static bool get_yun_member_INFO(const QString &cardNo, membernewparam &arg0, QString &errstring);
    //云会员增减积分
    static bool put_yun_member_point_add_sub(QWidget *parent, const QString &memberno, const QString &type, const QString &payno, float point, const QString &memo, const QString &operid);
    //会员手机验证
    static bool member_mobileNo_verify(const QString &telephone, const QString &code, QString &errstring);
    //优惠券信息
    static bool get_yun_coupon(QDate &useDate, double &money, double &use,  const QString &orderSn, QString &errstring);
    //优惠券验证
    static bool verify_yun_coupon(const QString &orderSn, QString &errstring);


    //会员充值,取款,退卡相关金额
    struct CvsMemberOperaInfoRecharge{
        QString code;
        double amount;
    };

    struct CvsMemberOperaInfo{
        CvsMemberOperaInfo();
        double rechargeAmount;//充值金额
        double realRechargeAmount;//充值实收
        QList<CvsMemberOperaInfoRecharge> rechargeDetail;
        double withdrawalAmount;//取款金额
        double backCardAmount;//退卡金额
        void print();
        void init();
    };
    static CvsMemberOperaInfo  getCvsMemberOperaInfo(QWidget *parent,const QString &operId,const QString &dateStart, const QString &dateEnd, bool &ok);

    //释放json
    static void json_free();
    //会员付款核实
    static bool get_yun_member_checkbillnopay(QWidget *parent,const QString &billno, const QString &memberno);
    static yunTime get_yun_srvdt(QWidget *parent, bool &ok, QString *errstring = 0);
    //! 匹配系统时间是否和 云时间相同
    ///*-1 异常, 0 失败, 1成功校对*/
    static int match_local_time(QWidget *parent);
    static int check_local_time(QWidget *parent, yunTime &retydt, QString *msg = 0);
public:
    static bool is_match_local_time;

private:
    QDialog *dialog;
    static QMap<QString, QString> yun_MemberParameter;
    static QMap<QString, QString> yun_MemberReadcard;
    static QMap<QString, QString> yun_MemberType;

    // 这儿对c的指针要做专门处理制度json_free负责释放
    static QSharedPointer<cJSON> json_yun_parameter;
    static QSharedPointer<cJSON> json_yun_readcard;
    static QSharedPointer<cJSON> json_yun_membertype;
    static QSharedPointer<cJSON> json_yun_readcard_select;
};

#endif // W_SYS_MANAGE_CLOUDSYNC_H
