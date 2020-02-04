#include "w_scr_dish_main_qr_code.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "lds.h"
#include "cjson_help.h"
#include "cJSON.h"
#include <QtDebug>
#include "lds_messagebox.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "fexpandmain_model_sqltablemodel_data.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "public_sql.h"
#include "w_scr_dish_restaurant_dialog.h"
#include "fexpandmain_2.h"
#include "w_bt_dish.h"
#include "public_printby_ch_billno.h"
#include "w_scr_dish_pay_widget.h"
#include "lds_network.h"

const QString w_scr_dish_main_qr_code::billTypeAVailable = "1";
const QString w_scr_dish_main_qr_code::billTypeRCV = "2";
const QString w_scr_dish_main_qr_code::billTypePaid = "3";
const QString w_scr_dish_main_qr_code::billTypeQuickOrder = "billTypeQuickOrder";
const QString w_scr_dish_main_qr_code::billTypeYunAddDish = "billTypeYunAddDish";

static int w_scr_dish_main_qr_REQUEST_COUNT = 0;
QString w_scr_dish_main_qr_code::billTypeDesc(const QString &billtype)
{
    if(billTypeAVailable == billtype) return QObject::tr("新的订单");
    if(billTypeRCV == billtype) return QObject::tr("已经确定");
    if(billTypePaid == billtype) return QObject::tr("已经支付");
    return "";
}

bool w_scr_dish_main_qr_code::QrCodeMasterGreaterThan(const w_scr_dish_main_qr_code::QrCodeMaster &s1, const w_scr_dish_main_qr_code::QrCodeMaster &s2)
{
    return s1.sn.toInt() > s2.sn.toInt();
}

bool w_scr_dish_main_qr_code::QrCodeMasterLessThan(const w_scr_dish_main_qr_code::QrCodeMaster &s1, const w_scr_dish_main_qr_code::QrCodeMaster &s2)
{
    return s1.sn.toInt() < s2.sn.toInt();
}

int w_scr_dish_main_qr_code::indexof(const QrCodeMasterList &masters, const QString &sn)
{
    for(int k = 0; k < masters.count(); k++) {
        if(masters[k].sn == sn) {
            return k;
        }
    }
    return -1;
}

bool w_scr_dish_main_qr_code::QrCodeDetailGreaterThan(const QrCodeDetail &s1, const QrCodeDetail &s2)
{
    return s1.sn.toInt() > s2.sn.toInt();
}

QrCodeMasterList w_scr_dish_main_qr_code::qr_code_master_get(const QString &ch_tableno, const QString &state, bool *ok)
{
    QrCodeMasterList qr_data_list;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
#if defined(QT_DEBUG ) && 0
    request.setRawHeader("terminalCode",  "FA1101");
    request.setUrl(QUrl("http://192.168.1.81/dishREST/queryUserOrders-rest"));
#else
    request.setRawHeader("terminalCode",  lds::terminalCode.toLocal8Bit());
    request.setUrl(QUrl("http://115.28.155.165/qryqs/dishREST/queryUserOrders-rest"));
#endif
    request.setRawHeader("tableNo", ch_tableno.toLocal8Bit());
    request.setRawHeader("state", state.toLocal8Bit());

    qDebug() << request.url() << ++w_scr_dish_main_qr_REQUEST_COUNT;

    QByteArray readall;
    int httpcode;
    QString errstring;

    if(ok) *ok = false;
    if(lds_network::managerGet(request, readall, errstring, httpcode)) {
        if(200 == httpcode) {
            if(ok) *ok = true;
            cJSON_help root(readall);
            w_scr_dish_main_qr_code::QrCodeMaster qr_data;
            qr_data.orgName = cJSON_help::getcJSONvalue(root, "orgName").toString();
            cJSON *array = cJSON_help::getcJSON(root, "data");
            for(int k = 0; k < cJSON_help::array_count(array); k ++) {
                cJSON *item = cJSON_GetArrayItem(array, k);
                qr_code_code_resolve_master(item, qr_data);
                qr_data_list << qr_data;
            }
        }
    }
    return qr_data_list;
}

QrCodeMasterList w_scr_dish_main_qr_code::qr_code_master_get(const QString &state, bool *ok)
{
    QrCodeMasterList masters;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
#if defined(QT_DEBUG ) && 0
    request.setRawHeader("terminalCode",  "FA1101");
    request.setUrl(QUrl("http://192.168.1.81/dishREST/queryUserOrders-rest"));
#else
    request.setRawHeader("terminalCode",  lds::terminalCode.toLocal8Bit());
    request.setUrl(QUrl("http://115.28.155.165/qryqs/dishREST/queryUserOrders-rest"));
#endif
    request.setRawHeader("state", state.toLocal8Bit());

    qDebug() << request.url() << ++w_scr_dish_main_qr_REQUEST_COUNT;
    QByteArray readall;
    QString errstring;
    int httpcode;

    if(ok) *ok = false;
    if(lds_network::managerGet(request, readall, errstring, httpcode)) {
        if(200 == httpcode) {
            if(ok) *ok = true;
            cJSON_help root(readall);
            w_scr_dish_main_qr_code::QrCodeMaster master;
            master.orgName = cJSON_help::getcJSONvalue(root, "orgName").toString();
            cJSON *array = cJSON_help::getcJSON(root, "data");
            for(int k = 0; k < cJSON_help::array_count(array); k ++) {
                cJSON *item = cJSON_GetArrayItem(array, k);
                qr_code_code_resolve_master(item, master);

                masters << master;
            }
        }
    }
    return masters;
}

QrCodeMasterList w_scr_dish_main_qr_code::qr_code_master_get(const QString &state, const QStringList &include_sn, bool *ok)
{
    QrCodeMasterList masters1 = qr_code_master_get(state, ok);
    QrCodeMasterList masters2;
    for(int k = 0; k < include_sn.count(); k++) {
        int index = indexof(masters1, include_sn[k]);
        if(index >= 0) {
            masters2 << masters1[index];
        }
    }

    return masters2;
}

QrCodeMasterList w_scr_dish_main_qr_code::qr_code_bill_get_quick_order_asc(const QString &min_sn, const QList<QString> &except_sn, bool &ok)
{
    QrCodeMasterList masters;
    QString errstring;
    QByteArray readall;
    int httpcode;
    QNetworkRequest request;
#if defined(QT_DEBUG) & 0
    request.setUrl(QString("http://192.168.1.81/qryqs/dishREST/queryPayOrders-rest"));
#else
    request.setUrl(QString("http://115.28.155.165/qryqs/dishREST/queryPayOrders-rest"));
#endif
    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
    request.setRawHeader("terminalCode", lds::terminalCode.toLocal8Bit());
    request.setRawHeader("sn", min_sn.toLocal8Bit());

    ok = false;
    if(false == lds_network::managerGet(request, readall, errstring, httpcode)) {
        return masters;
    }
    if(200 != httpcode) {
        errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
        return masters;
    }
    ok = true;

    //
    cJSON_help obj(readall);
    QString orgName = cJSON_help::getcJSONvalue(obj, "orgName").toString();
    cJSON *array = cJSON_help::getcJSON(obj, "data");
    for(int k = 0; k < cJSON_help::array_count(array); k++) {
        cJSON *item = cJSON_GetArrayItem(array, k);
        QrCodeMaster master;
        w_scr_dish_main_qr_code::qr_code_code_resolve_master(item, master);
        master.state = w_scr_dish_main_qr_code::billTypeQuickOrder;
        master.orgName = orgName;

        if(!except_sn.contains(master.sn)) {
            masters << master;
        }
    }

    //云端默认是小到大排序
    //    qSort(masters.begin(), masters.end(), w_scr_dish_main_qr_code::QrCodeMasterLessThan);
    return masters;
}

bool w_scr_dish_main_qr_code::qr_code_bill_get_yun_add_dish_asc(const QString &min_sn, QByteArray &readall, QString &errstring)
{
    int httpcode;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
#if defined(QT_DEBUG ) && 0
    request.setRawHeader("terminalCode",  "FA1101");
    request.setUrl(QUrl("192.168.1.81/qryqs/dishREST/queryOrdersAddDishs-rest"));
#else
    request.setRawHeader("terminalCode",  lds::terminalCode.toLocal8Bit());
    request.setUrl(QUrl("http://115.28.155.165/qryqs/dishREST/queryOrdersAddDishs-rest"));
#endif
    request.setRawHeader("sn", min_sn.toLocal8Bit());

    if(false == lds_network::managerGet(request, readall, errstring, httpcode)) {
        return false;
    }
    if(200 != httpcode) {
        errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
        return false;
    }
    return true;
}

void w_scr_dish_main_qr_code::qr_code_list_print(const QList<w_scr_dish_main_qr_code::QrCodeMaster> &list)
{
    for(int k = 0; k < list.count(); k ++) {
        qDebug() << "sn:" << list[k].sn;
        qDebug() << "date:" << list[k].date;
        qDebug() << "orgName:" << list[k].orgName;
        qDebug() << "mount:" << list[k].mount;
        qDebug() << "state:" << list[k].state;
    }
}

QrCodeDetailList w_scr_dish_main_qr_code::qr_code_detail_get(const QString &sn)
{
    QrCodeDetailList qr_data_list;
    QNetworkRequest request;
    QByteArray readall;
    QString errstring;
    int httpcode;

    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
    request.setRawHeader("orderSn",  sn.toLocal8Bit());
#if defined(QT_DEBUG ) && 0
    request.setRawHeader("terminalCode",  "FA1101");
    request.setUrl(QUrl("http://192.168.1.81/dishREST/getOrderDishInfo-rest"));
#else
    request.setRawHeader("terminalCode",  lds::terminalCode.toLocal8Bit());
    request.setUrl(QUrl("http://115.28.155.165/qryqs/dishREST/getOrderDishInfo-rest"));
#endif

    if(lds_network::managerGet(request, readall, errstring, httpcode)) {
        if(200 == httpcode) {
            cJSON_help  root(readall.data());
            cJSON *array = cJSON_help::getcJSON(root, "data");
            for(int k = 0; k < cJSON_help::array_count(array); k ++) {
                cJSON *item = cJSON_GetArrayItem(array, k);
                w_scr_dish_main_qr_code::QrCodeDetail qr_data;
                qr_code_code_resolve_detail(item, qr_data);
                qr_data_list << qr_data;
            }
        }
    }
    return qr_data_list;
}

void w_scr_dish_main_qr_code::qr_code_code_resolve_detail(cJSON *item, w_scr_dish_main_qr_code::QrCodeDetail &d)
{
    d.sn = cJSON_help::getcJSONvalue(item, "sn").toString();
    d.dishNo = cJSON_help::getcJSONvalue(item, "dishNo").toString();
    d.mount = cJSON_help::getcJSONvalue(item, "mount").toDouble();
    d.nickname = cJSON_help::getcJSONvalue(item, "nickname").toString();
    d.number = cJSON_help::getcJSONvalue(item, "number").toDouble();
    d.price = cJSON_help::getcJSONvalue(item, "price").toDouble();

    d.tc = cJSON_help::getcJSONvalue(item, "tc").toByteArray();
    d.zf = cJSON_help::getcJSONvalue(item, "zf").toByteArray();

    d.flag = cJSON_help::getcJSONvalue(item, "flag").toInt();
    d.date = cJSON_help::getcJSONvalue(item, "date").toDateTime();

    qDebug() << __LINE__ ;
    cJSON_help::jsonPrint(item);
}

void w_scr_dish_main_qr_code::qr_code_code_resolve_master(cJSON *item, w_scr_dish_main_qr_code::QrCodeMaster &d)
{
    d.date = cJSON_help::getcJSONvalue(item, "date").toDateTime();
    d.mount = cJSON_help::getcJSONvalue(item, "mount").toDouble();
    d.sn = cJSON_help::getcJSONvalue(item, "sn").toString();
    d.tableNo = cJSON_help::getcJSONvalue(item, "tableNo").toString();
    d.state = cJSON_help::getcJSONvalue(item, "state").toString();
}

void w_scr_dish_main_qr_code::qr_code_list_print(const QrCodeDetailList &list)
{
    for(int k = 0; k < list.count(); k ++) {
        list[k].print();
    }
}

bool w_scr_dish_main_qr_code::qr_code_unique_sn(QString &dish_sn, QString &errstring)
{
    QNetworkRequest request;
    QByteArray readall;
    int httpcode;
#if defined(QT_DEBUG ) && 0
    request.setUrl(QUrl("http://192.168.1.81/dishREST/getNextOrderSn-rest"));
#else
    request.setUrl(QUrl("http://115.28.155.165/qryqs/dishREST/getNextOrderSn-rest"));
#endif
    qDebug() << request.url() << ++w_scr_dish_main_qr_REQUEST_COUNT;
    if(false == lds_network::managerPost(request, "", readall, errstring, httpcode)) {
        return false;
    }
    if(200 != httpcode) {
        errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
        return false;
    }
    dish_sn = cJSON_help::getcJSONvalue(readall, "sn").toString();
    return true;
}

bool w_scr_dish_main_qr_code::qr_code_bill_do_CANCEL(const QString &sn, QString &errstring)
{
    QByteArray readall;
    int httpcode;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
    request.setRawHeader("orderSn",  sn.toLocal8Bit());//已下单
#if defined(QT_DEBUG ) && 0
    request.setRawHeader("terminalCode",  "FA1101");
    request.setUrl(QUrl("http://192.168.1.81/dishREST/cannelOrderDishInfoByOrderNo-rest"));
#else
    request.setRawHeader("terminalCode",  lds::terminalCode.toLocal8Bit());
    request.setUrl(QUrl("http://115.28.155.165/qryqs/dishREST/cannelOrderDishInfoByOrderNo-rest"));
#endif
    qDebug() << request.url() << ++w_scr_dish_main_qr_REQUEST_COUNT;
    if(false == lds_network::managerGet(request, readall, errstring, httpcode)) {
        return false;
    }
    if(200 != httpcode) {
        errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
        return false;
    }
    return true;
}

bool w_scr_dish_main_qr_code::qr_code_bill_do_RCV(const QString &sn, QString &errstring)
{
    QByteArray readall;
    int httpcode;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
    request.setRawHeader("orderSn",  sn.toLocal8Bit());//已下单
#if defined(QT_DEBUG ) && 0
    request.setRawHeader("terminalCode",  "FA1101");
    request.setUrl(QUrl("http://192.168.1.81/dishREST/commitOrderDishInfoByOrderNo-rest"));
#else
    request.setRawHeader("terminalCode",  lds::terminalCode.toLocal8Bit());
    request.setUrl(QUrl("http://115.28.155.165/qryqs/dishREST/commitOrderDishInfoByOrderNo-rest"));
#endif
    qDebug() << request.url() << ++w_scr_dish_main_qr_REQUEST_COUNT;
    if(false == lds_network::managerGet(request, readall, errstring, httpcode)) {
        return false;
    }
    if(200 != httpcode) {
        errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
        return false;
    }
    return true;
}

bool w_scr_dish_main_qr_code::qr_code_has_paid(const QString &sn, w_scr_dish_main_qr_code::QrCodePay &qr_code_pay_data, QString &errstring)
{
    QByteArray readall;
    int httpcode;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
    request.setRawHeader("orderSn",  sn.toLocal8Bit());//已下单
#if defined(QT_DEBUG ) & 0
    request.setRawHeader("terminalCode",  lds::terminalCode.toLocal8Bit());
    request.setUrl(QUrl("http://192.168.1.81/dishREST/getOrderDishInfoByOrderNo-rest"));
#else
    request.setRawHeader("terminalCode",  lds::terminalCode.toLocal8Bit());
    request.setUrl(QUrl("http://115.28.155.165/qryqs/dishREST/getOrderDishInfoByOrderNo-rest"));
#endif
    qDebug() << request.url() << ++w_scr_dish_main_qr_REQUEST_COUNT;

    if(false == lds_network::managerGet(request, readall, errstring, httpcode)) {
        return false;
    }
    cJSON_help root(readall);
    if(200 != httpcode) {
        errstring = cJSON_help::getcJSONvalue(root, "message").toString();
        return false;
    }

    cJSON *d = cJSON_help::getcJSON(root, "data");
    if(billTypePaid != cJSON_help::getcJSONvalue(d, "state").toString()) {
        errstring = "NOT PAID TYPE";
        return false;
    }

    cJSON *data = cJSON_help::getcJSON(root, "data");
    cJSON *paydetail = cJSON_help::getcJSON(root, "paydetail");
    qr_code_pay_data.num_cost = cJSON_help::getcJSONvalue(data, "mount").toDouble();
    qr_code_pay_data.num_cash = qr_code_pay_data.num_cost;

    for(int k = 0; k < cJSON_GetArraySize(paydetail); k++) {
        cJSON *item = cJSON_GetArrayItem(paydetail, k);
        qr_code_pay_data.alipay += cJSON_help::getcJSONvalue(item, "alipay").toDouble();
        qr_code_pay_data.couponpay += cJSON_help::getcJSONvalue(item, "couponpay").toDouble();
        qr_code_pay_data.wechatpay += cJSON_help::getcJSONvalue(item, "wechatpay").toDouble();
        qr_code_pay_data.memberpay += cJSON_help::getcJSONvalue(item, "memberpay").toDouble();
        qr_code_pay_data.memberNo += cJSON_help::getcJSONvalue(item, "memberNo").toString();
    }
    qr_code_pay_data.print();
    return true;
}

bool w_scr_dish_main_qr_code::qr_code_bill_do_pay(const QString &ch_tableno, const QString &ch_billno_R, const w_scr_dish_main_qr_code::QrCodePay &qr_code_pay_data, QString &ch_payno, QString &errstring)
{
    QDateTime curdt = n_func::f_get_sysdatetime();
    lds_query_hddpos  query;
    int sqr = 0;
    double num_point = 0;
    QByteArray readall;

    //判断是否有追加菜品
    if(w_scr_dish_main_qr_code::qr_code_bill_get_yun_add_dish_asc(n_func::f_get_sysparm("qr_code_yun_add_dish_cur_sn", "0"), readall, errstring)) {
        cJSON_help obj(readall);
        cJSON *array = cJSON_help::getcJSON(obj, "data");
        int data_count = cJSON_help::array_count(array);
        for(int k = 0; k < data_count; k++) {
            cJSON *item = cJSON_GetArrayItem(array, k);
            if(ch_tableno == cJSON_help::getcJSONvalue(item, "tableNo").toString()) {
                errstring = QObject::tr("云端商品未添加");
                return false;
            }
        }
    }
    //更新qr_code_quick_order_cur_sn，因为已付款订单和其他订单不可能同时存在，所以
    QString vch_qr_code_sn = lds_query_hddpos::selectValue(query, QString(" select vch_qr_code_sn from cey_u_table where ch_billno = '%1' ").arg(ch_billno_R)).toString();
    QString qr_code_quick_order_cur_sn_old = n_func::f_get_sysparm("qr_code_quick_order_cur_sn");
    QString qr_code_quick_order_cur_sn_cur;
    if(qr_code_quick_order_cur_sn_old.toInt() > vch_qr_code_sn.toInt())
        qr_code_quick_order_cur_sn_cur = qr_code_quick_order_cur_sn_old;
    else
        qr_code_quick_order_cur_sn_cur = vch_qr_code_sn;

    if(false == n_func::f_set_sysparm("qr_code_quick_order_cur_sn", qr_code_quick_order_cur_sn_cur, "", errstring)) {
        return false;
    }

    ch_payno = lds_scr_table::get_max_ch_payno_add1();

    //cancel table
    if(lds_scr_table::cey_bt_table_update(ch_tableno, "", sqr, &errstring)) {
        if(query.execUpdate("cey_u_orderdish", qrtVariantPairList()
                            <<qrtVariantPair("ch_payno", ch_payno)
                            <<qrtVariantPair("vch_memberno", qr_code_pay_data.memberNo)
                            ,  qrtEqual("ch_billno",ch_billno_R), &errstring)) {
            //checkout_master
            if(query.execInsert("cey_u_checkout_master",qrtVariantPairList()
                                << qrtVariantPair("ch_billno", ch_billno_R)
                                << qrtVariantPair("ch_tableno", ch_tableno)
                                << qrtVariantPair("num_cost", qr_code_pay_data.num_cost)
                                << qrtVariantPair("num_discount", 0)
                                << qrtVariantPair("num_present", 0)

                                << qrtVariantPair("num_service", 0)
                                //                                        << qrtVariantPair("vch_service_desc", "")
                                << qrtVariantPair("num_lowcost", 0)
                                << qrtVariantPair("num_blotout", qr_code_pay_data.num_round)
                                << qrtVariantPair("num_run", 0)

                                << qrtVariantPair("vch_run_operID", "")
                                << qrtVariantPair("ch_state", "Y")
                                << qrtVariantPair("vch_operID", lds::gs_operid)
                                << qrtVariantPair("dt_operdate", curdt)
                                << qrtVariantPair("vch_operID2", "")

                                << qrtVariantPair("num_rate", 0)
                                << qrtVariantPair("ch_payno", ch_payno)
                                << qrtVariantPair("vch_memberno", qr_code_pay_data.memberNo)
                                << qrtVariantPair("ch_member_belong_state",  qr_code_pay_data.memberNo.length() > 0 ? "Y" : "")
                                << qrtVariantPair("ch_yun_payno", "")

                                << qrtVariantPair("num_cash", qr_code_pay_data.num_cash),  &errstring)) {
                //check detail alipay
                if(qr_code_pay_data.alipay == 0 ||query.execInsert("cey_u_checkout_detail",qrtVariantPairList()
                                                                   << qrtVariantPair("ch_payno", ch_payno)
                                                                   << qrtVariantPair("ch_paymodeno", "06")//支付宝
                                                                   << qrtVariantPair("num_payamount", qr_code_pay_data.alipay)
                                                                   << qrtVariantPair("num_change", 0)
                                                                   << qrtVariantPair("num_realamount", qr_code_pay_data.alipay)

                                                                   << qrtVariantPair("num_face", 0)
                                                                   << qrtVariantPair("vch_operID", lds::gs_operid)
                                                                   << qrtVariantPair("dt_operdate", curdt)
                                                                   << qrtVariantPair("num_card_remain", 0)
                                                                   << qrtVariantPair("vch_voucherno", "")

                                                                   << qrtVariantPair("num_face_Num", 0), &errstring))  {
                    //check detail couponpay
                    if(qr_code_pay_data.couponpay == 0 ||query.execInsert("cey_u_checkout_detail",qrtVariantPairList()
                                                                          << qrtVariantPair("ch_payno", ch_payno)
                                                                          << qrtVariantPair("ch_paymodeno", "04")//优惠券
                                                                          << qrtVariantPair("num_payamount", qr_code_pay_data.couponpay)
                                                                          << qrtVariantPair("num_change", 0)
                                                                          << qrtVariantPair("num_realamount", qr_code_pay_data.couponpay)

                                                                          << qrtVariantPair("num_face", 0)
                                                                          << qrtVariantPair("vch_operID", lds::gs_operid)
                                                                          << qrtVariantPair("dt_operdate", curdt)
                                                                          << qrtVariantPair("num_card_remain", 0)
                                                                          << qrtVariantPair("vch_voucherno", "")

                                                                          << qrtVariantPair("num_face_Num", 0), &errstring))  {
                        //check detail wechatpay
                        if(qr_code_pay_data.wechatpay == 0 ||query.execInsert("cey_u_checkout_detail", qrtVariantPairList()
                                                                              << qrtVariantPair("ch_payno", ch_payno)
                                                                              << qrtVariantPair("ch_paymodeno", "05")//微信
                                                                              << qrtVariantPair("num_payamount", qr_code_pay_data.wechatpay)
                                                                              << qrtVariantPair("num_change", 0)
                                                                              << qrtVariantPair("num_realamount", qr_code_pay_data.wechatpay)

                                                                              << qrtVariantPair("num_face", 0)
                                                                              << qrtVariantPair("vch_operID", lds::gs_operid)
                                                                              << qrtVariantPair("dt_operdate", curdt)
                                                                              << qrtVariantPair("num_card_remain", 0)
                                                                              << qrtVariantPair("vch_voucherno", "")

                                                                              << qrtVariantPair("num_face_Num", 0), &errstring))  {
                            //check detail memberpay
                            if(qr_code_pay_data.memberpay == 0 ||query.execInsert("cey_u_checkout_detail", qrtVariantPairList()
                                                                                  << qrtVariantPair("ch_payno", ch_payno)
                                                                                  << qrtVariantPair("ch_paymodeno", "10")//云会员
                                                                                  << qrtVariantPair("num_payamount", qr_code_pay_data.memberpay)
                                                                                  << qrtVariantPair("num_change", 0)
                                                                                  << qrtVariantPair("num_realamount", qr_code_pay_data.memberpay)

                                                                                  << qrtVariantPair("num_face", 0)
                                                                                  << qrtVariantPair("vch_operID", lds::gs_operid)
                                                                                  << qrtVariantPair("dt_operdate", curdt)
                                                                                  << qrtVariantPair("num_card_remain", 0)
                                                                                  << qrtVariantPair("vch_voucherno", qr_code_pay_data.memberNo)

                                                                                  << qrtVariantPair("num_face_Num", 0), &errstring))  {
                                if(w_scr_dish_pay_widget::get_member_point(ch_payno, num_point, errstring)) {
                                    if(num_point == 0 || query.execInsert("t_m_point",qrtVariantPairList()
                                                                          << qrtVariantPair("vch_memberno", qr_code_pay_data.memberNo)
                                                                          << qrtVariantPair("ch_type", 3)
                                                                          << qrtVariantPair("ch_payno", ch_payno)
                                                                          << qrtVariantPair("num_point", num_point)
                                                                          << qrtVariantPair("vch_memo", "")

                                                                          << qrtVariantPair("vch_operID", lds::gs_operid)
                                                                          << qrtVariantPair("dt_operdate", curdt), &errstring))  {
                                        //check detail num_money
                                        if(qr_code_pay_data.num_money == 0 ||query.execInsert("cey_u_checkout_detail",qrtVariantPairList()
                                                                                              << qrtVariantPair("ch_payno", ch_payno)
                                                                                              << qrtVariantPair("ch_paymodeno", "00")//现金
                                                                                              << qrtVariantPair("num_payamount", qr_code_pay_data.num_money)
                                                                                              << qrtVariantPair("num_change", qr_code_pay_data.num_change)
                                                                                              << qrtVariantPair("num_realamount", qr_code_pay_data.num_money - qr_code_pay_data.num_change)

                                                                                              << qrtVariantPair("num_face", 0)
                                                                                              << qrtVariantPair("vch_operID", lds::gs_operid)
                                                                                              << qrtVariantPair("dt_operdate", curdt)
                                                                                              << qrtVariantPair("num_card_remain", 0)
                                                                                              << qrtVariantPair("vch_voucherno", "")

                                                                                              << qrtVariantPair("num_face_Num", 0), &errstring))  {
                                            //u_table
                                            if(query.execUpdate("cey_u_table","ch_payno", ch_payno, qrtEqual("ch_billno", ch_billno_R),  &errstring)) {
                                                //u_orderdish
                                                if(query.execUpdate("cey_u_orderdish","ch_payno", ch_payno, qrtEqual("ch_billno", ch_billno_R),  &errstring)) {
                                                    if(qr_code_pay_data.memberpay == 0 ||qr_code_bill_member_flow_upload(ch_payno, qr_code_pay_data, errstring)) {
                                                        return true;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

void w_scr_dish_main_qr_code::qr_code_bill_do_pay_print(const QString &ch_billno_R)
{
    //打印结账单
    if(lds::sysconf->value("system_setting/maidanzhangdan").toString() == "1") {
        public_printby_ch_billno print_ch_billno(
                    ch_billno_R,
                    lds::sysconf->value("system_setting/maidan_language").toString());
        print_ch_billno.toPayPrint(
                    print_ch_billno.defpath(),
                    n_func::f_get_sysparm("bp_count", "1").toInt(),
                    QStringList()/* label_insert_int_flowid_list */,
                    public_printby_ch_billno::payNormal
                    );
    }
}

bool w_scr_dish_main_qr_code::qr_code_bill_do_pay_transaction(const QString &ch_tableno, const QString &ch_billno_R, const QrCodePay &paydetail, QString &ret_errstring)
{
    qDebug() << __LINE__ ; lds_query_hddpos::tran_saction();
    QString ch_payno;
    if(qr_code_bill_do_pay(ch_tableno, ch_billno_R, paydetail, ch_payno,  ret_errstring)) {
        lds_query_hddpos::com_mit();
        qr_code_bill_do_pay_print(ch_billno_R);
        return true;
    }
    lds_query_hddpos::roll_back();
    return false;
}

bool w_scr_dish_main_qr_code::qr_code_bill_cashpay(const QString &ch_payno, const QString &sn, double numTotal, double numPay, QString &errstring)
{
    QByteArray readall;
    int httpcode;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
    request.setRawHeader("terminalCode",  lds::terminalCode.toLocal8Bit());
    request.setRawHeader("orderSn",  sn.toLocal8Bit());//已下单
    request.setRawHeader("numTotal",  QByteArray::number(numTotal));
    request.setRawHeader("numPay",  QByteArray::number(numPay));
    request.setRawHeader("outTradeNo",  ch_payno.toLocal8Bit());
    request.setUrl(QUrl("http://115.28.155.165/qryqs/dishREST/xianJinPay-rest"));

    if(false == lds_network::managerGet(request, readall, errstring, httpcode)) {
        return false;
    }
    if(200 != httpcode) {
        errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
        return false;
    }
    return true;
}

bool w_scr_dish_main_qr_code::qr_code_can_order(const QString &ch_tableno, QString &errstring)
{
    lds_query_hddpos  query;
    query.execSelect(QString(" select " + restraurantDelegate::sql_ch_billnos + ", ch_billno from cey_bt_table where ch_tableno = '%1' ").arg(ch_tableno));
    query.next();
    QString ch_billnos = query.recordValue(0).toString();
    QString ch_billno = query.recordValue(1).toString();

    //已经分单
    if(restraurantDelegate::ch_billno_is_div(ch_billnos)) {
        errstring = QObject::tr("餐桌已经分单");
        return false;
    }
    if(ch_billno.length() > 0) {
        errstring = QObject::tr("餐桌已开台");
        return false;
    }
    //    //点菜记录
    //    if(lds_query_hddpos::selectValue(QString(" select count(0) from cey_u_orderdish where ch_billno = '%1' ")
    //                              .arg(ch_billno)).toInt() > 0) {
    //        errstring = QObject::tr("已有点菜记录");
    //        return false;
    //    }
    return true;
}

void w_scr_dish_main_qr_code::orderdish_generate(const QrCodeDetailList &detail_list, QrCode_cey_u_orderdishList &retvalue)
{
    QString ch_suitflag;
    QString ch_dishno;
    lds_query_hddpos  query;
    QMap<QString, QString> suit_origin_replace_map;
    for(int k = 0; k < detail_list.count(); k ++) {
        int index = retvalue.count();
        retvalue.append((w_scr_dish_main_qr_code::QrCode_cey_u_orderdish) {
                            detail_list[k].dishNo,
                            detail_list[k].number,
                            detail_list[k].price,
                            vch_print_memoData(),
                            detail_list[k].mount,
                            "N",
                            detail_list[k].nickname,
                            "",
                            detail_list[k].sn,
                            detail_list[k].flag,
                            detail_list[k].date
                        }
                        );
        w_scr_dish_main_qr_code::QrCode_cey_u_orderdish &d= retvalue[index];

        //vch_print_memo
        {
            httpDataAutoDel zf = cJSON_Parse(detail_list[k].zf.data());
            for(int k = 0; k < cJSON_help::array_count(zf.json); k ++ ) {
                d.vch_print_memo.duplicate(query.db, cJSON_help::getItemValue(zf.json, k).toString());
            }
        }
        //cook
        {
            suit_origin_replace_map.clear();
            httpDataAutoDel tc = cJSON_Parse(detail_list[k].tc.data());
            for(int k = 0; k < cJSON_help::array_count(tc.json); k ++ ) {
                QString replaced = cJSON_help::getItemValue(tc.json, k).toString();
                suit_origin_replace_map.insert(replaced.split(",").value(0), replaced.split(",").value(1));
            }
        }
        ch_suitflag = query.selectValue(QString(" select ch_suitflag from cey_bt_dish where ch_dishno = '%1' ").arg(d.ch_dishno)).toString();
        //suit
        if(ch_suitflag == "Y") {
            d.ch_suitflag = "P";
            query.execSelect(QString(" SELECT a.int_flowId, a.ch_suitno, a.ch_dishno, a.num_num FROM cey_bt_dish_suit a, cey_bt_dish b  where a.ch_dishno = b.ch_dishno and ch_suitno = '%1'; ")
                             .arg(d.ch_dishno));
            while(query.next()) {
                ch_dishno = query.recordValue("ch_dishno").toString();
                retvalue.append((w_scr_dish_main_qr_code::QrCode_cey_u_orderdish) {
                                    suit_origin_replace_map.value(ch_dishno, ch_dishno),
                                    d.num_num * query.recordValue("num_num").toDouble(),
                                    0,
                                    d.vch_print_memo,
                                    0,
                                    "Y",
                                    d.vch_operid,
                                    d.ch_dishno,
                                    d.vch_qr_code_sn,
                                    d.flag,
                                    d.dt_operdate
                                });
            }
            d.vch_print_memo.clear();
        }
    }
}

void w_scr_dish_main_qr_code::orderdish_generate(const QString &sn, QrCode_cey_u_orderdishList &retvalue)
{
    QrCodeDetailList detail_list = qr_code_detail_get(sn);
    qSort(detail_list.begin(), detail_list.end(), QrCodeDetailGreaterThan);
    return orderdish_generate(detail_list, retvalue);
}

bool w_scr_dish_main_qr_code::orderdish_is_valid(const QrCode_cey_u_orderdishList &value_list, QString &null_ch_dishno)
{
    lds_query_hddpos  query;
    foreach(const QrCode_cey_u_orderdish &p, value_list) {
        qDebug() << __FILE__ << __LINE__ << p.ch_dishno;
        if(lds_query_hddpos::selectValue(query, QString(" select ch_dishno from cey_bt_dish where ch_dishno = '%1' ").arg(p.ch_dishno)).isNull()) {
            null_ch_dishno = p.ch_dishno;
            return false;
        }
    }
    return true;
}

bool w_scr_dish_main_qr_code::orderdish_bill_create_do_RCV_transaction(const QrCodeMaster &master, const QrCode_cey_u_orderdishList &value_list, QString &errstring)
{
    qDebug() << __LINE__ ; lds_query_hddpos::tran_saction();
    QString ch_billno;
    if(orderdish_bill_create(master, value_list, ch_billno, errstring)) {
        if(qr_code_bill_do_RCV(master.sn, errstring)) {
            lds_query_hddpos::com_mit();
            //消费单
            if(lds::sysconf->value("system_setting/luodanzongdan").toString() == "1") {
                public_printby_ch_billno printer(ch_billno);
                printer.toprint_preprint(lds::sysconf->value("system_setting/printerpath").toString(), QObject::tr("消费单"));
            }
            return true;
        }
    }
    lds_query_hddpos::roll_back();
    return false;
}

bool w_scr_dish_main_qr_code::orderdish_bill_create(const w_scr_dish_main_qr_code::QrCodeMaster &master, const QrCode_cey_u_orderdishList &value_list, QString &ch_billno, QString &errstring)
{
    lds_query_hddpos  query;
    ch_billno = lds_scr_table::get_max_ch_billno_add1("R");
    if(lds_scr_table::insert_cey_u_master_Create_serial_no(ch_billno, &errstring)) {
        if(query.execInsert("cey_u_table",qrtVariantPairList()
                            << qrtVariantPair("ch_billno", ch_billno)
                            << qrtVariantPair("ch_tableno", master.tableNo)
                            << qrtVariantPair("int_person", lds_query_hddpos::selectValue(QString("select int_person from cey_bt_table where ch_tableno = '%1' ").arg(master.tableNo)))
                            << qrtVariantPair("ch_state", 1)
                            << qrtVariantPair("vch_operID", lds::gs_operid)

                            << qrtVariantPair("dt_operdate", master.date)
                            << qrtVariantPair("ch_order_state", 0)
                            << qrtVariantPair("vch_waiter", "")
                            << qrtVariantPair("vch_memo", master.orgName)

                            << qrtVariantPair("vch_qr_code_sn", master.sn)
                            << qrtVariantPair("num_cost", master.mount), &errstring
                            )) {
            if(lds_scr_table::cey_u_taxsale_master_insert_table(query, ch_billno)) {
                if(lds_scr_table::cey_bt_table_update(master.tableNo, ch_billno, 0, &errstring)) {
                    if(w_scr_dish_restaurant_dialog::cey_bt_table_bar_replace(master.tableNo, 0, ch_billno, &errstring)) {
                        if(orderdish_bill_append(ch_billno, value_list, errstring)) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool w_scr_dish_main_qr_code::orderdish_bill_append(const QString &ch_billno, const QrCode_cey_u_orderdishList &value_list, QString &errstring)
{
    lds_query_hddpos  query;
    fexpandmain_model_sqltablemodel_data tablemodel;
    int int_id;
    QString vch_plan_k3o3_state;
    QString vch_plan_k3o3_id;
    w_bt_dish::kitchenState  s0 = w_bt_dish::KITCHEN_MIN_DEF;
    int row0;

    tablemodel.setTable("cey_u_orderdish");
    tablemodel.setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel.select(ch_billno);
    //
    if(tablemodel.rowCount() != tablemodel.rowCountOp(lds_model_sqltablemodel::OpInsert)) {
        s0 = w_bt_dish::KITCHEN_JIACAI;
    }

    //
    query.execSelect(QString("select max(int_id) from cey_u_orderdish where ch_billno  = '%1' ")
                     .arg(ch_billno));
    query.next();
    int_id = query.recordValue(0).toInt();

    foreach(const QrCode_cey_u_orderdish &d, value_list) {
        int_id ++;
        //guqing
        errstring.clear();
        if(!fexpandmain_2::guqing_check(errstring, d.ch_dishno, d.num_num)) {
            goto CANCEL;
        }
        if(!fexpandmain_2::guqing_update(d.ch_dishno, d.num_num)) {
            errstring = QObject::tr("数据库错误")+QString::number(__LINE__);
            goto CANCEL;
        }
        //tablemodel
        row0 = tablemodel.rowCount();
        tablemodel.insertRow(row0);
        w_bt_dish::get_vch_plan_k3o3(vch_plan_k3o3_state, vch_plan_k3o3_id, d.ch_dishno, s0);
        tablemodel.model_data_set(row0, "ch_billno", ch_billno);
        tablemodel.model_data_set(row0, "ch_tableno", tablemodel.get_cey_u_table_value(ch_billno, "ch_tableno"));//master.tableNo);
        tablemodel.model_data_set(row0, "int_id", int_id);
        tablemodel.model_data_set(row0, "ch_dishno", d.ch_dishno);
        tablemodel.model_data_set(row0, "num_price", d.num_price);

        tablemodel.model_data_set(row0, "num_price_org", d.num_price);
        tablemodel.model_data_set(row0, "num_price_add", d.num_total - d.num_num * d.num_price);
        tablemodel.model_data_set(row0, "num_num", d.num_num);
        tablemodel.model_data_set(row0, "num_back", 0);
        tablemodel.model_data_set(row0, "int_discount", 100);

        tablemodel.model_data_set(row0, "ch_suitflag", d.ch_suitflag);
        tablemodel.model_data_set(row0, "ch_suitno", d.ch_suitno);
        tablemodel.model_data_set(row0, "vch_operID", lds::gs_operid);
        tablemodel.model_data_set(row0, "dt_operdate", d.dt_operdate);
        tablemodel.model_data_set(row0, "vch_print_memo", d.vch_print_memo.sqlValue());

        tablemodel.model_data_set(row0, "vch_plan_k3o3_id", vch_plan_k3o3_id);
        tablemodel.model_data_set(row0, "vch_plan_k3o3_state", vch_plan_k3o3_state);

        tablemodel.model_data_set(row0, "ch_presentflag", "N");
        tablemodel.model_data_set(row0, "ch_specialflag", "N");
        tablemodel.model_data_set(row0, "int_rate",  tablemodel.get_ch_dishno_rate(ch_billno, d.ch_dishno));
        tablemodel.model_data_set(row0, "vch_qr_code_sn", d.vch_qr_code_sn);
    }

    if(tablemodel.try_commit_data(errstring)) {
        goto COMMIT;
    }
CANCEL:
    tablemodel.removeRows(0, tablemodel.rowCount());
    return false;
COMMIT:
    return true;
}

bool w_scr_dish_main_qr_code::orderdish_dish_quit_transaction(const QString &ch_billno, const QString &dish_sn, QString &errstring)
{
    QString master_sn;
    lds_query_hddpos  query;
    query.execSelect(QString(" SELECT vch_qr_code_sn FROM cey_u_table where ch_billno = '%1'; ").arg(ch_billno));
    query.next();
    master_sn = query.recordValue("vch_qr_code_sn").toString();

    query.execSelect(QString(" select int_flowid, num_num - num_back as num_dish from cey_u_orderdish where ch_billno = '%1' and vch_qr_code_sn = '%2' ")
                     .arg(ch_billno)
                     .arg(dish_sn));
    query.next();
    qlonglong int_flowid = query.recordValue("int_flowid").toLongLong();
    double num_dish = query.recordValue("num_dish").toDouble();
    //
    QList<num_backData> d_list;
    lds_query_hddpos::tran_saction();
    if(w_bt_dish_back::static_toquit_1_suit(errstring, int_flowid, num_dish, "", lds::gs_operid, query.curDateTime(), d_list)) {
        if(qr_code_bill_do_QUIT(master_sn, dish_sn, errstring)) {
            lds_query_hddpos::com_mit();
            return true;
        }
    }
    lds_query_hddpos::roll_back();
    //0
    return false;
}

bool w_scr_dish_main_qr_code::qr_code_bill_do_QUIT(const QString &master_sn, const QString &dish_sn, QString &errstring)
{
    QByteArray readall;
    QNetworkRequest request;
    int httpcode;
    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
#if defined(QT_DEBUG ) && 0
    request.setRawHeader("terminalCode",  "FA1101");
    request.setUrl(QUrl("http://192.168.1.81/dishREST/cancelOrderDishInfo-rest"));
#else
    request.setRawHeader("terminalCode",  lds::terminalCode.toLocal8Bit());
    request.setUrl(QUrl("http://115.28.155.165/qryqs/dishREST/cancelOrderDishInfo-rest"));
#endif
    QByteArray param = cJSON_help::mapTocJSONByteArray("orderSn", master_sn, "dishSn", dish_sn);
    qDebug() << request.url() << ++w_scr_dish_main_qr_REQUEST_COUNT;

    if(false == lds_network::managerPost(request, param, readall, errstring, httpcode)) {
        return false;
    }
    if(200 != httpcode) {
        errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
        return false;
    }
    return true;
}

bool w_scr_dish_main_qr_code::qr_code_bill_do_DISHAPPEND(const QString &master_sn, const QString &dish_sn, const QString &tableNo, const QString &dishNo, double number, const QVariant &zf, const QVariant &tc, const QString &typeNo, QString &errstring)
{
    QByteArray readall;
    int httpcode;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
#if defined(QT_DEBUG ) && 0
    request.setRawHeader("terminalCode",  "FA1101");
    request.setUrl(QUrl("http://192.168.1.81/dishREST/addOrderDishInfo-rest"));
#else
    request.setRawHeader("terminalCode",  lds::terminalCode.toLocal8Bit());
    request.setUrl(QUrl("http://115.28.155.165/qryqs/dishREST/addOrderDishInfo-rest"));
#endif

    cJSON_help json(cJSON_Object);
    cJSON *dish_data = cJSON_CreateObject();
    cJSON_help::_cJSON_AddValueToObject(json, "orderSn", master_sn);
    cJSON_help::_cJSON_AddItemToObject(json, "data", cJSON_CreateArray());
    cJSON_help::_cJSON_AddItemToObject(cJSON_help::getcJSON(json, "data"), "", dish_data);
    cJSON_help::_cJSON_AddValueToObject(dish_data, "tableNo", tableNo);
    cJSON_help::_cJSON_AddValueToObject(dish_data, "dishNo", dishNo);
    cJSON_help::_cJSON_AddValueToObject(dish_data, "number", number);
    cJSON_help::_cJSON_AddValueToObject(dish_data, "zf", zf);
    cJSON_help::_cJSON_AddValueToObject(dish_data, "tc", tc);
    cJSON_help::_cJSON_AddValueToObject(dish_data, "typeNo", typeNo);
    cJSON_help::_cJSON_AddValueToObject(dish_data, "sn", dish_sn);

    QByteArray param = cJSON_help::getcJSONprint(json);

    if(false == lds_network::managerPost(request, param, readall, errstring, httpcode)) {
        return false;
    }
    if(200 != httpcode) {
        errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
        return false;
    }
    return true;
}

bool w_scr_dish_main_qr_code::qr_code_bill_member_flow_upload(const QString &ch_payno, const QrCodePay &qr_code_pay_data, QString &errstring)
{
    lds_query_hddpos  query;
    cJSON_help json(cJSON_Object);
    cJSON *array = 0;

    QString dateyyMMdd = n_func::f_get_sysdatetime().toString("yyMMdd");

    QDateTime payDateTime;
    QString ch_billno_Y;
    w_sys_manage_cloudsync::get_yun_member_maxorderno("K"+dateyyMMdd, ch_billno_Y, errstring);
    ch_billno_Y = "K"+dateyyMMdd  + QString().sprintf("%05d",ch_billno_Y.right(5).toInt()+1);

    QString ch_billno_LOCAL;
    QString ch_payno_Y;
    w_sys_manage_cloudsync::get_yun_member_maxmasterno("F" +dateyyMMdd , ch_payno_Y, errstring);
    ch_payno_Y = "F"+dateyyMMdd  + QString().sprintf("%05d",ch_payno_Y.right(5).toInt()+1);

    QMap<QString, QVariant> keyValueMap;
    keyValueMap.insert("ch_billno", ch_billno_Y);
    keyValueMap.insert("ch_payno", ch_payno_Y);

    //cey_u_checkout_master
    if(false == query.execUpdate("cey_u_checkout_master", "ch_yun_payno", ch_payno_Y, qrtEqual("ch_payno", ch_payno), &errstring)) {
        return false;
    }
    query.execSelect(QString(" select * from cey_u_checkout_master where ch_payno = '%1' ").arg(ch_payno));
    query.next();
    ch_billno_LOCAL = query.recordValue("ch_billno").toString();
    payDateTime = query.recordValue("dt_operdate").toDateTime();
    cJSON_help::_cJSON_AddItemToObject(json, "checkOutMaster",w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_checkout_master(query, "", keyValueMap));

    //cey_u_master
    query.execSelect(QString(" select * from cey_u_master where ch_billno = '%1' ").arg(ch_billno_LOCAL));
    query.next();
    cJSON_help::_cJSON_AddItemToObject(json, "orderMaster", w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_master(query, "", keyValueMap));

    //t_m_point
    array = cJSON_CreateArray();
    query.execSelect(QString(" select * from t_m_point where ch_payno = '%1' ").arg(ch_payno));
    while(query.next()) {
        cJSON_AddItemToArray(array,w_sys_manage_cloudsync_with_time_running::q_u_get_piece_t_m_point(query, "", keyValueMap));
    }
    cJSON_help::_cJSON_AddItemToObject(json, "memberPointList", array);

    //t_m_pay
    array = cJSON_CreateArray();
    cJSON_AddItemToArray(array,cJSON_help::mapTocJSON(ldsVariantMap()
                                                      .append("mpVchMemberNo", qr_code_pay_data.memberNo)
                                                      .append("mpChPayNo", ch_payno_Y)
                                                      .append("mpChPayModeNo", "10")
                                                      .append("mpNumPay", qr_code_pay_data.memberpay)
                                                      .append("mpVchOperId", lds::gs_operid)

                                                      .append("mpNumAmount", 0)
                                                      .append("mpDtOperDate", payDateTime)
                                                      .append("terminalCode", lds::terminalCode)
                                                      ));
    cJSON_help::_cJSON_AddItemToObject(json, "memberPay", array);

    //cey_u_table
    query.execSelect(QString(" select * from cey_u_table where ch_payno = '%1' ").arg(ch_payno));
    query.next();
    cJSON_help::_cJSON_AddItemToObject(json, "tableRecordsConsumption",w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_table(query, "", keyValueMap));

    //cey_u_orderdish
    array = cJSON_CreateArray();
    query.execSelect(QString(" select * from cey_u_orderdish where ch_payno = '%1' ").arg(ch_payno));
    while(query.next()) {
        cJSON_AddItemToArray(array,w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_orderdish(query, "", keyValueMap));
    }
    cJSON_help::_cJSON_AddItemToObject(json, "orderDish", array);

    //cey_u_checkout_detail
    array = cJSON_CreateArray();
    query.execSelect(QString(" select * from cey_u_checkout_detail where ch_payno = '%1' ").arg(ch_payno));
    while(query.next()) {
        cJSON_AddItemToArray(array,w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_checkout_detail(query, "", keyValueMap));
    }
    cJSON_help::_cJSON_AddItemToObject(json, "checkOutDetail", array);

    //t_m_deposit 二维码点菜的会员余额以云端为准，本地不存在充值记录
    cJSON_help::_cJSON_AddItemToObject(json, "memberDeposit", cJSON_CreateArray());

    //
    QByteArray readall;
    int httpcode;
    QNetworkRequest request;
#if defined(QT_DEBUG)
    request.setUrl(QString("http://192.168.1.105/cvs/member/subMemberFlow-rest"));
#else
    request.setUrl(http_www_yqspos_com +"/cvs/member/subMemberFlow-rest");
#endif
    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
    request.setRawHeader("terminalCode", lds::terminalCode.toLocal8Bit());
    request.setRawHeader("memberNo", qr_code_pay_data.memberNo.toLocal8Bit());

    if(false == lds_network::managerPost(request, json.jsonValue(), readall, errstring, httpcode)) {
        return false;
    }
    if(200 != httpcode) {
        errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
        return false;
    }
    if("ok" != cJSON_help::getcJSONvalue(readall, "message").toString()) {
        errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
        return false;
    }
    return true;
}

bool w_scr_dish_main_qr_code::qr_code_bill_quick_order_do_OK_transaction(const w_scr_dish_main_qr_code::QrCodeMaster &master, QString &errstring)
{
    //
    QString ch_billno;
    QString ch_payno;
    QrCodePay qr_code_pay_data;
    QrCode_cey_u_orderdishList detail_list;
    qDebug() << __LINE__ ; lds_query_hddpos::tran_saction();
    if(qr_code_has_paid(master.sn, qr_code_pay_data, errstring)) {
        orderdish_generate(master.sn, detail_list);
        if(orderdish_bill_create(master, detail_list, ch_billno, errstring)) {
            if(qr_code_bill_do_pay(master.tableNo, ch_billno, qr_code_pay_data, ch_payno, errstring)) {
                lds_query_hddpos::com_mit();

                qr_code_bill_do_pay_print(ch_billno);
                return true;
            }
        }
    }
    lds_query_hddpos::roll_back();
    return true;
}

w_scr_dish_main_qr_code::w_scr_dish_main_qr_code() : QObject(0)
{

}

QString w_scr_dish_main_qr_code::QrCodePay::information()
{
    QString msg =
            QObject::tr("应收") + ":" + QString().sprintf("%.02f", num_cost) + "\n";
    if(num_round != 0)
        msg += QObject::tr("抹零") + ":" + QString().sprintf("%.02f", num_round) + "\n";
    msg += QObject::tr("实收") + ":" + QString().sprintf("%.02f", num_cash) + "\n";

    if(alipay != 0)
        msg += QObject::tr("支付宝支付") + ":" + QString().sprintf("%.02f", alipay) + "\n";
    if(wechatpay != 0)
        msg += QObject::tr("微信支付") + ":" + QString().sprintf("%.02f", wechatpay) + "\n";
    if(couponpay != 0)
        msg += QObject::tr("优惠券") + ":" + QString().sprintf("%.02f", couponpay) + "\n";
    if(memberpay != 0) {
        msg += QObject::tr("会员支付") + ":" + QString().sprintf("%.02f", memberpay) + "\n";
        msg += QObject::tr("会员编号") + ":" + memberNo + "\n";
    }

    if(num_money != 0)
        msg += QObject::tr("现金支付") + ":" + QString().sprintf("%.02f", num_money) + "\n";
    if(num_change != 0)
        msg += QObject::tr("找零") + ":" + QString().sprintf("%.02f", num_change) + "\n";
    return msg;
}

void w_scr_dish_main_qr_code::QrCodePay::print()
{
    qDebug() << "num_cost" << num_cost;
    qDebug() << "num_round" << num_round;
    qDebug() << "num_cash" << num_cash;


    qDebug() << "alipay" << alipay;
    qDebug() << "couponpay" << couponpay;
    qDebug() << "wechatpay" << wechatpay;
    qDebug() << "memberpay" << memberpay;


    qDebug() << "num_money" << num_money;
    qDebug() << "num_change" << num_change;
}


void w_scr_dish_main_qr_code::QrCodeMaster::print() const
{
    qDebug() << "sn:" << sn;
    qDebug() << "orgName:" << orgName;
    qDebug() << "mount:" << mount;
    qDebug() << "tableNo:" << tableNo;
    qDebug() << "state:" << state;

    qDebug() << "date:" << date;
}


void w_scr_dish_main_qr_code::QrCodeDetail::print() const
{
    qDebug() << "sn:" << sn;
    qDebug() << "nickname:" << nickname;
    qDebug() << "dishNo:" << dishNo;
    qDebug() << "number:" << number;
    qDebug() << "price:" << price;

    qDebug() << "zf:" << zf;
    qDebug() << "mount:" << mount;
    qDebug() << "tc:" << tc;
    qDebug() << "flag:" << flag;
    qDebug() << "date:" << date;
}
