#ifndef W_SCR_DISH_MAIN_QR_CODE_H
#define W_SCR_DISH_MAIN_QR_CODE_H

#include <QDialog>
#include <QStandardItemModel>
#include <QDateTime>
#include "lds_model_sqlstandardmodel.h"
#include <QNetworkRequest>
#include <QList>
#include "vch_print_memodata.h"

class cJSON;

#define QrCodeMasterList QList<w_scr_dish_main_qr_code::QrCodeMaster>
#define QrCodeDetailList QList<w_scr_dish_main_qr_code::QrCodeDetail>
#define QrCodeMasterDetailsList QList<w_scr_dish_main_qr_code::QrCodeMasterDetails>
#define QrCode_cey_u_orderdishList QList<w_scr_dish_main_qr_code::QrCode_cey_u_orderdish>

class w_scr_dish_main_qr_code : public QObject
{
    Q_OBJECT
public:
    static const QString billTypeAVailable;
    static const QString billTypeRCV;
    static const QString billTypePaid;
    static const QString billTypeQuickOrder;
    static const QString billTypeYunAddDish;

    static QString billTypeDesc(const QString &billtype);
    //
    struct QrCodeMaster{
        QString sn;
        QString orgName;
        double mount;
        QString tableNo;
        QString state;
        QDateTime date;

        void print() const;
    };
    static bool QrCodeMasterGreaterThan(const QrCodeMaster &s1, const QrCodeMaster &s2);
    static bool QrCodeMasterLessThan(const QrCodeMaster &s1, const QrCodeMaster &s2);


    static int indexof(const QrCodeMasterList &masters, const QString &sn);
    //
    struct QrCodeDetail{
        QString sn;
        QString nickname;
        QString dishNo;
        double number;
        double price;

        QByteArray zf;
        double mount;
        QByteArray tc;
        int flag; //0正常， -1退货, 1 本地加菜, 11云端加菜
        QDateTime date;

        void print() const;
    };
    static bool QrCodeDetailGreaterThan(const QrCodeDetail &s1, const QrCodeDetail &s2);

    struct QrCode_cey_u_orderdish {
        QString ch_dishno;
        double num_num;
        double num_price;
        vch_print_memoData vch_print_memo;
        double num_total;

        QString ch_suitflag;
        QString vch_operid;
        QString ch_suitno;
        QString vch_qr_code_sn;
        int flag; //0正常， -1退货, 1 本地加菜, 11云端加菜

        QDateTime dt_operdate;
    };

    struct QrCodePay {//05微信， 06支付宝
        QrCodePay():
            num_cost(0),
            num_round(0),
            num_cash(0),
            //
            alipay(0),
            wechatpay(0),
            couponpay(0),

            memberpay(0),
            memberNo(""),

            num_money(0),
            num_change(0)
        {}


        double num_cost;//应收
        double num_round;//抹零
        double num_cash;//实收
        //
        double alipay;//支付宝
        double wechatpay;//微信
        double couponpay;//优惠券

        double memberpay;//云会员
        QString memberNo;//云会员编号
        //
        double num_money;//现金
        double num_change;//找零

        //
        QString information();
        void print();
    };

    struct QrCodeMasterDetails{
        QrCodeMaster master;
        QrCodeDetailList details;
    };

    static QrCodeMasterList qr_code_master_get(const QString &ch_tableno, const QString &state, bool *ok = 0);
    static QrCodeMasterList qr_code_master_get(const QString &state,  bool *ok = 0);
    static QrCodeMasterList qr_code_master_get(const QString &state, const QStringList &include_sn,  bool *ok = 0);
    static QrCodeMasterList qr_code_bill_get_quick_order_asc(const QString &min_sn, const QList<QString> &except_sn, bool &ok);
    static bool qr_code_bill_get_yun_add_dish_asc(const QString &min_sn, QByteArray &readall, QString &errstring);

    static void qr_code_list_print(const QrCodeMasterList &list);

    static QrCodeDetailList qr_code_detail_get(const QString &sn);
    static void qr_code_code_resolve_detail(cJSON *item, QrCodeDetail &d);
    static void qr_code_code_resolve_master(cJSON *item, QrCodeMaster &d);

    static void qr_code_list_print(const QrCodeDetailList &list);

    static bool qr_code_unique_sn(QString &dish_sn, QString &errstring);

    static bool qr_code_bill_do_CANCEL(const QString &sn, QString &errstring);
    static bool qr_code_bill_do_RCV(const QString &sn, QString &errstring);
    static bool qr_code_has_paid(const QString &sn, QrCodePay &qr_code_pay_data, QString &errstring);

    static bool qr_code_bill_do_pay(const QString &ch_tableno, const QString &ch_billno_R, const QrCodePay &qr_code_pay_data, QString &ch_payno,
                                                QString &errstring);
    static void qr_code_bill_do_pay_print(const QString &ch_billno_R);
    static bool qr_code_bill_do_pay_transaction(const QString &ch_tableno, const QString &ch_billno_R, const QrCodePay &paydetail,
                                                QString &ret_errstring);
    static bool qr_code_bill_cashpay(const QString &ch_payno, const QString &sn, double numTotal, double numPay, QString &errstring);

    static bool qr_code_can_order(const QString &ch_tableno, QString &errstring);

    static void orderdish_generate(const QrCodeDetailList &detail_list, QrCode_cey_u_orderdishList &retvalue);
    static void orderdish_generate(const QString &sn, QrCode_cey_u_orderdishList & retvalue);
    static bool orderdish_is_valid(const QrCode_cey_u_orderdishList &value_list, QString &null_ch_dishno);

    static bool orderdish_bill_create_do_RCV_transaction(const QrCodeMaster &master, const QrCode_cey_u_orderdishList &value_list, QString &errstring);
    static bool orderdish_bill_create(const QrCodeMaster &master, const QrCode_cey_u_orderdishList &value_list, QString &ch_billno, QString &errstring);
    static bool orderdish_bill_append(const QString &ch_billno, const QrCode_cey_u_orderdishList &value_list, QString &errstring);

    static bool orderdish_dish_quit_transaction(const QString &ch_billno, const QString &dish_sn, QString &errstring);

    static bool qr_code_bill_do_QUIT(const QString &master_sn, const QString &dish_sn, QString &errstring);

    static bool qr_code_bill_do_DISHAPPEND(const QString &master_sn, const QString &dish_sn, const QString &tableNo, const QString &dishNo, double number, const QVariant &zf, const QVariant &tc, const QString &typeNo, QString &errstring);

    static bool qr_code_bill_member_flow_upload(const QString &ch_payno, const w_scr_dish_main_qr_code::QrCodePay &qr_code_pay_data, QString &errstring);

    static bool qr_code_bill_quick_order_do_OK_transaction(const QrCodeMaster &master, QString &errstring);
public:
    w_scr_dish_main_qr_code();
};

#endif // W_SCR_DISH_MAIN_QR_CODE_H
