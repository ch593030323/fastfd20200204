#include "w_sys_manage_cloudsync.h"
//u云同步
#include "lds_messagebox.h"
#include <QSqlError>
#include <QTimer>
#include "cjson_help.h"
#include <QProcess>
#include <QtDebug>
#include "lds.h"
#include "lds.h"
#include "lds_query_hddpos.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "public_sql.h"
#include "n_func.h"
#include "w_sys_manage_cloudsync_transfer.h"

QMap<QString, QString> w_sys_manage_cloudsync::yun_MemberParameter;
QMap<QString, QString> w_sys_manage_cloudsync::yun_MemberReadcard;
QMap<QString, QString> w_sys_manage_cloudsync::yun_MemberType;

QSharedPointer<cJSON> w_sys_manage_cloudsync::json_yun_parameter;
QSharedPointer<cJSON> w_sys_manage_cloudsync::json_yun_readcard;
QSharedPointer<cJSON> w_sys_manage_cloudsync::json_yun_membertype;
QSharedPointer<cJSON> w_sys_manage_cloudsync::json_yun_readcard_select;
bool w_sys_manage_cloudsync::is_match_local_time=false;

/**
 * 构造函数
 */
w_sys_manage_cloudsync::w_sys_manage_cloudsync()
{
}

w_sys_manage_cloudsync::~w_sys_manage_cloudsync()
{
}

httpDataAutoDel w_sys_manage_cloudsync::get_yun_httppos(QWidget *parent, const QString &http_key, const QByteArray &param, const QMap<QString, QString> &headInfo, QString &retparam, const QString &msgtitle, bool ispop, bool timecheck, int mesc)
{
    httpDataAutoDel ret_json_err;
    w_sys_manage_cloudsync_transfer transfer;
    yunTime ydt;
    QString errstring;
    int check_result;
    if(timecheck
            &&!is_match_local_time
            &&1 != (check_result = check_local_time(parent, ydt, &errstring))) {
        //-1云端失败 0时间不符 1时间相符
        if(-1 == check_result) {
            ret_json_err.message = errstring;
        }
        if(0 == check_result) {
            ret_json_err.message =
                    QObject::tr("软件未进行时间校对,可以通过以下操作校对") + "\n"
                    + "1." + QObject::tr("云同步数据设置，启动云端，重启") + "\n"
                    + "2." + QObject::tr("云同步数据设置，时间校对");
        }
    } else {
        ret_json_err = transfer.sendPost2(http_key, param, headInfo,retparam, mesc);
    }
    //! 获取map
    if(200 != ret_json_err.httpStatus) {
        if(parent && ispop) {
            QString msgerr=QString("httpStatus:") + QString::number(ret_json_err.httpStatus) + ", " + msgtitle+"\n"+ret_json_err.message;
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, msgerr);
        }
    }
end:
    return ret_json_err;
}

httpDataAutoDel w_sys_manage_cloudsync::get_yun_httppos(QWidget *parent, const QString &http_key, const QByteArray &param, const QMap<QString, QString> &headInfo, const QString &msgtitle, bool ispop, bool timecheck, int mesc)
{
    QString empty;
    return get_yun_httppos(parent, http_key, param, headInfo, empty, msgtitle, ispop, timecheck, mesc);
}

httpDataAutoDel w_sys_manage_cloudsync::get_yun_httpget(QWidget *parent, const QString &http_key, const QMap<QString, QString> &headInfo, QString &retparam, const QString &msgtitle, bool ispop, bool timecheck, int mesc)
{
    httpDataAutoDel ret_json_err;
    w_sys_manage_cloudsync_transfer transfer;
    yunTime ydt;
    QString msg;
    int check_result;
    if(timecheck
            &&!is_match_local_time
            &&1 != (check_result = check_local_time(parent, ydt, &msg))) {
        if(-1 == check_result) {
            ret_json_err.message = msg;
        }
        if(0 == check_result) {
            ret_json_err.message =
                    QObject::tr("软件未进行时间校对,可以通过以下操作校对") + "\n"
                    + "1." + QObject::tr("云同步数据设置，启动云端，重启") + "\n"
                    + "2." + QObject::tr("云同步数据设置，时间校对");
        }
    } else {
        //! 获取map
        ret_json_err = transfer.sendGet(http_key, headInfo,retparam, mesc);
    }
    if(ret_json_err.httpStatus != 200) {
        QString msgerr=msgtitle+"\n" +ret_json_err.message;
        if(parent && ispop) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, msgerr);
        } else {
        }
    }
end:
    return ret_json_err;
}

httpDataAutoDel w_sys_manage_cloudsync::get_yun_httpget(QWidget *parent, const QString &http_key, const QMap<QString, QString> &headInfo, const QString &msgtitle, bool ispop, bool timecheck, int mesc)
{
    QString empty;
    return get_yun_httpget(parent, http_key, headInfo, empty, msgtitle, ispop, timecheck, mesc);
}

QString w_sys_manage_cloudsync::print_yun(cJSON *print)
{
    if(print==NULL) {
        return "";
    }

    char* arrayDate = cJSON_Print(print);
    QString param=QString(arrayDate);
    free(arrayDate);
    return param;
}

void w_sys_manage_cloudsync::printalljson_child(cJSON *print)
{
    cJSON *tmp=print;
    while(tmp) {
        tmp=tmp->child;
    }
}

QString w_sys_manage_cloudsync::get_yun_member_parameter(QWidget *parent,const QString &key, bool *ok)
{
    if(ok) *ok = false;
    if(json_yun_parameter.isNull()) {
        QMap<QString, QString> headInfo;

        //headInfo.insert("machineCode","");
        headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
        headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
        headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
        QString httpkey = http_www_yqspos_com +"/cvs/member/parameter-rest";
        httpDataAutoDel httpd = get_yun_httpget(parent, httpkey, headInfo, QObject::tr("获取云会员参数"));
        json_yun_parameter = httpd.json;
    }

    if(json_yun_parameter.isNull())
        return "";
    if(ok) *ok = true;

    return cJSON_help::getcJSONvalue(json_yun_parameter, key.toLocal8Bit()).toString();
}

bool w_sys_manage_cloudsync::get_yun_member_parameter(const QString &key, QString &retValue, QString &errstring)
{
    if(json_yun_parameter.isNull()) {
        QByteArray readall;
        int httpcode;
        QString httpkey = http_www_yqspos_com +"/cvs/member/parameter-rest";
        QNetworkRequest request;
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");
        request.setRawHeader("terminalCode", lds::terminalCode.toLocal8Bit());
        request.setUrl(httpkey);

        if(false == lds_network::managerGet(request, readall, errstring, httpcode)) {
            return false;
        }
        if(200 != httpcode) {
            errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
            return false;
        }
        json_yun_parameter = cJSON_help::createSharedPtr(readall);
    }

    retValue = cJSON_help::getcJSONvalue(json_yun_parameter, key.toLocal8Bit()).toString();
    return true;
}

/*
  "vch_memberno": "Y10004",
  "vch_member": "10004",
  "ch_typeno": "11",
  "dt_birthday": null,
  "vch_handtel": "13324569874",
  "vch_tel": "",

  "dt_limit": "2015-07-31 12:00:00",
  "ch_state": "1",
  "num_limit": null,
  "ch_cardflag": "Y",
  "vch_cardno": "10004",
  "int_basedata": 5,

  "vch_typename": "优惠卡1",
  "ch_favourable": "2",
  "ch_pricekind": null,
  "int_discount": 10,
  "ch_calc": "N",
  "ch_calc_type": "1",

  "int_calc_unit": 1,
  "ch_area_flag": "0",
  "int_backcash_rate": 0,
  "int_card_baseamount": 50,
  "password": null,
  "num_amount": 200,

  "num_hangamount": 0,
  "num_point": 0,
  "vch_operID": null,
  "dt_operdate": null,
  "ch_attribute": "",
  "ch_payno": "",

  "ch_paymodeno": "",
  "num_pay": 0,
  "ch_balanceno": "",

  "int_checkflow": ""
*/
QString w_sys_manage_cloudsync::get_yun_member_readcard(QWidget *parent, const QString cardNo, const QString &operId, const QString &key, bool &ok, const QString &def)
{
    QString ret;
    if(json_yun_readcard.isNull()
            || (json_yun_readcard !=NULL && cJSON_help::getcJSONvalue(json_yun_readcard, "vch_memberno").toString() != cardNo)) {
        json_yun_readcard.clear();

        QByteArray param = cJSON_help::mapTocJSONByteArray("cardNo", cardNo, "operId", operId);

        //获取map
        QMap<QString, QString> headInfo;

        //headInfo.insert("machineCode","");
        headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
        headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
        headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
        QString http_key=http_www_yqspos_com +"/cvs/member/dukFuk-rest";
        httpDataAutoDel httpd = get_yun_httppos(parent, http_key, param, headInfo,QObject::tr("云会员读卡"));
        json_yun_readcard =  httpd.json;
    }

    if(json_yun_readcard.isNull()) {
        ok = false;
        return "";
    }

    if(cJSON_help::array_count(json_yun_readcard) > 0) {
        ret = cJSON_help::getcJSONvalue(json_yun_readcard, key.toLocal8Bit()).toString();
    }
    ok = true;
    return ret;
}

bool w_sys_manage_cloudsync::get_yun_member_readcard(const QString &cardNo, const QString &operId, const QString &key, QVariant &retValue, QString &retErrstring)
{
    if(json_yun_readcard.isNull()
            || (cJSON_help::getcJSONvalue(json_yun_readcard, "vch_memberno").toString() != cardNo)) {
        json_yun_readcard.clear();

        QByteArray param = cJSON_help::mapTocJSONByteArray("cardNo", cardNo, "operId", operId);

        //获取map
        QMap<QString, QString> headInfo;

        //headInfo.insert("machineCode","");
        headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
        headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
        headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
        QString http_key=http_www_yqspos_com +"/cvs/member/dukFuk-rest";
        httpDataAutoDel httpd = get_yun_httppos(0, http_key, param, headInfo,QObject::tr("云会员读卡"));
        json_yun_readcard =  httpd.json;

        if(httpd.httpStatus == 200) {

            retValue = cJSON_help::getcJSONvalue(json_yun_readcard, key.toLocal8Bit());
            return true;
        }
        retErrstring = httpd.message;
        return false;
    }

    retValue = cJSON_help::getcJSONvalue(json_yun_readcard, key.toLocal8Bit());
    return true;
}

bool w_sys_manage_cloudsync::get_yun_member_readcard(const QString &cardNo, const QString &operId, const QString &key, QString &retValue, QString &retErrstring)
{
    QVariant d;
    bool f = get_yun_member_readcard(cardNo, operId, key, d, retErrstring);
    retValue = d.toString();
    return f;
}

bool w_sys_manage_cloudsync::get_yun_member_readcard(const QString &cardNo, const QString &operId, const QString &key, int &retValue, QString &retErrstring)
{
    QVariant d;
    bool f = get_yun_member_readcard(cardNo, operId, key, d, retErrstring);
    retValue = d.toInt();
    return f;
}

bool w_sys_manage_cloudsync::get_yun_member_maxorderno(const QString &flag, QString &maxOrderNo, QString &errstring)
{
    QByteArray param;
    QMap<QString, QString> headInfo;

    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    headInfo.insert("flag",flag);
    QString http_key=http_www_yqspos_com +"/cvs/member/maxOrderNo-rest";
    httpDataAutoDel root = get_yun_httppos(0, http_key, param, headInfo, QObject::tr("获取最大订单号"), false);
    if(root.httpStatus != 200) {
        errstring = root.message;
        return false;
    }
    maxOrderNo = cJSON_help::getcJSONvalue(root.json, "maxOrderNo").toString();
    qDebug() << "maxOrderNo" << maxOrderNo;
    return true;
}


bool w_sys_manage_cloudsync::get_yun_member_maxmasterno(const QString &flag, QString &maxMasterNo, QString &errstring)
{
    QByteArray param;
    QMap<QString, QString> headInfo;

    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    headInfo.insert("flag",flag);
    QString http_key=http_www_yqspos_com +"/cvs/member/maxMasterNo-rest";
    httpDataAutoDel root = get_yun_httppos(0, http_key, param, headInfo,QObject::tr("获取最大支付号"), false);
    if(root.httpStatus != 200) {
        errstring = root.message;
        return false;
    }
    maxMasterNo = cJSON_help::getcJSONvalue(root.json, "maxMasterNo").toString();
    qDebug() << "maxMasterNo" << maxMasterNo;
    return true;
}

/*
  "cvsMemberType": {
    "sn": "5",
    "chTypeno": "11",
    "vchTypename": "优惠卡1",
    "chAttribute": "2",
    "chFavourable": "2",
    "chPricekind": null,
    "intDiscount": 10,
    "chProjectno": null,
    "chCalc": "N",
    "chCalcType": "1",
    "intCalcUnit": 1,
    "chAreaFlag": "0",
    "intBackcashRate": 0,
    "intCardBaseamount": 50,
    "orgCode": "000000060001",
    "date": "2015-07-02 18:27:22",
    "chAttributeStr": "充值卡",
    "chFavourableStr": "固定折扣",
    "idStr": "5",
    "operate": null
},
"message":"ok"*/
QString w_sys_manage_cloudsync::get_yun_member_membertype(QWidget *parent,const QString &typeNo, const QString &key, bool &ok, const QString &def)
{
    if(json_yun_membertype.isNull()
            || typeNo != cJSON_help::getcJSONvalue(cJSON_help::getcJSON(json_yun_membertype, "cvsMemberType"), "chTypeno").toString()
            ) {
        QByteArray param = cJSON_help::mapTocJSONByteArray("typeNo", typeNo);
        //获取map
        QMap<QString, QString> headInfo;

        //headInfo.insert("machineCode","");
        headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
        headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
        headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
        QString http_key = http_www_yqspos_com +"/cvs/member/getMemberType-rest";

#ifdef QT_DEBUG
        parent->objectName();
#endif
        httpDataAutoDel httpd = get_yun_httppos(parent, http_key, param, headInfo,QObject::tr("获取云会员类型"));//, def, "cvsMemberType"
        json_yun_membertype = httpd.json;
    }

    if(json_yun_membertype.isNull()) {
        ok = false;
        return "";
    }

    ok = true;

    return cJSON_help::getcJSONvalue(cJSON_help::getcJSON(json_yun_membertype,"cvsMemberType"), key.toLocal8Bit()).toString();
}

bool w_sys_manage_cloudsync::get_yun_member_membertype(const QString &typeNo, const QString &key, QString &retValue, QString &retErrstring)
{
    if(json_yun_membertype.isNull()
            || (typeNo !=cJSON_help::getcJSONvalue(cJSON_help::getcJSON(json_yun_membertype, "cvsMemberType"), "chTypeno").toString())
            ) {
        QByteArray param = cJSON_help::mapTocJSONByteArray("typeNo", typeNo);
        QMap<QString, QString> headInfo;

        //headInfo.insert("machineCode","");
        headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
        headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
        headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
        QString http_key=http_www_yqspos_com +"/cvs/member/getMemberType-rest";

        httpDataAutoDel httpd = get_yun_httppos(0, http_key, param, headInfo,QObject::tr("获取云会员类型"));//, def, "cvsMemberType"
        json_yun_membertype = httpd.json;

        if(httpd.httpStatus == 200) {
            retValue = cJSON_help::getcJSONvalue(cJSON_help::getcJSON(json_yun_membertype,"cvsMemberType"), key.toLocal8Bit()).toString();
            return true;
        }
        retErrstring = httpd.message;
        return false;
    }

    retValue = cJSON_help::getcJSONvalue(cJSON_help::getcJSON(json_yun_membertype,"cvsMemberType"), key.toLocal8Bit()).toString();
    return true;
}

bool w_sys_manage_cloudsync::get_yun_member_membertypelist(QWidget *parent, QMap<QString, QVariant> &retlist, QString *errstring)
{
    QMap<QString, QString> headInfo;

    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString httpkey = http_www_yqspos_com +"/cvs/member/memberTypeList-rest";

    httpDataAutoDel httpd = get_yun_httpget(parent, httpkey, headInfo, QObject::tr("获取会员类别列表"));
    if(httpd.json == NULL) {
        if(errstring)
            *errstring = httpd.message;
        return false;
    }

    for(int i =0, gcount=cJSON_help::array_count(httpd.json);i <gcount;i++) {
        cJSON *pItem = cJSON_help::_cJSON_GetArrayItem(httpd.json, i);
        retlist.insert(
                    cJSON_help::getcJSONvalue(pItem,"chTypeno").toString(),
                    cJSON_help::getcJSONvalue(pItem,"vchTypename").toString());
    }
    return true;
}

bool w_sys_manage_cloudsync::get_yun_member_membertypelist(QMap<QString, QVariant> &retlist, QString &errstring)
{
    QMap<QString, QString> headInfo;

    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString httpkey = http_www_yqspos_com +"/cvs/member/memberTypeList-rest";

    httpDataAutoDel httpd = get_yun_httpget(0, httpkey, headInfo, QObject::tr("获取会员类别列表"));
    if(httpd.json == NULL) {
        errstring = httpd.message;
        return false;
    }

    for(int i =0, gcount=cJSON_help::array_count(httpd.json);i <gcount;i++) {
        cJSON *pItem = cJSON_help::_cJSON_GetArrayItem(httpd.json, i);
        retlist.insert(
                    cJSON_help::getcJSONvalue(pItem,"chTypeno").toString(),
                    cJSON_help::getcJSONvalue(pItem,"vchTypename").toString());
    }
    return true;
}

yunpayData w_sys_manage_cloudsync::upload_yun_pay_info(QWidget *parent, member_up_yun &muy, const QString &ch_payno)
{

    cJSON *root = cJSON_CreateObject();
    cJSON *subarray;
    //会员x4
    if(!muy.memberPointListList.isEmpty())
    {
        cJSON_AddItemToObject(root, "memberPointList", subarray = cJSON_CreateArray());
        foreach(const memberPointList& p, muy.memberPointListList)
            cJSON_AddItemToArray(subarray,  muy.make_JSON_Array(p, n_func::f_get_sysparm("orgCode")));
    }
    if(!muy.memberPayList.isEmpty())
    {
        cJSON_AddItemToObject(root, "memberPay", subarray=cJSON_CreateArray());
        foreach(const memberPay& p, muy.memberPayList)
            cJSON_AddItemToArray(subarray,  muy.make_JSON_Array(p, n_func::f_get_sysparm("orgCode")));
    }
    if(!muy.memberCuramountList.isEmpty())
    {
        cJSON_AddItemToObject(root, "memberCuramount", subarray=cJSON_CreateArray());
        foreach(const memberCuramount& p, muy.memberCuramountList)
            cJSON_AddItemToArray(subarray,  muy.make_JSON_Array(p, n_func::f_get_sysparm("orgCode")));
    }
    if(!muy.memberDepositList.isEmpty()) {
        cJSON_AddItemToObject(root, "memberDepositList", subarray=cJSON_CreateArray());
        foreach(const memberDeposit& p, muy.memberDepositList)
            cJSON_AddItemToArray(subarray,  muy.make_JSON_Array(p, n_func::f_get_sysparm("orgCode")));
    }

    //付款记录 x5
    QString ch_billno_local;
    lds_query_hddpos  query;
    query.execSelect(QString("select * from cey_u_checkout_master where ch_payno = '%1' ; ").arg(ch_payno));
    query.next();
    ch_billno_local = query.recordValue("ch_billno").toString();
    //checkOutMaster
    query.execSelect(QString("select * from cey_u_checkout_master where ch_payno = '%1' ; ").arg(ch_payno));
    query.next();
    cJSON_AddItemToObject(root, "checkOutMaster",  muy.q_u_get_piece_cey_u_checkout_master(query, n_func::f_get_sysparm("orgCode")));
    //orderMaster
    query.execSelect(QString("select * from cey_u_master where ch_billno = '%1' ; ").arg(ch_billno_local));
    query.next();
    cJSON_AddItemToObject(root, "orderMaster",  muy.q_u_get_piece_cey_u_master(query, n_func::f_get_sysparm("orgCode")));
    //tableRecordsConsumption
    query.execSelect(QString("select * from cey_u_table where ch_payno = '%1' ; ").arg(ch_payno));
    query.next();
    cJSON_AddItemToObject(root, "tableRecordsConsumption",  muy.q_u_get_piece_cey_u_table(query, n_func::f_get_sysparm("orgCode")));
    //orderDish
    query.execSelect(QString("select * from cey_u_orderdish where ch_payno = '%1' ; ").arg(ch_payno));
    cJSON_AddItemToObject(root, "orderDish", subarray = cJSON_CreateArray());
    while(query.next()) {
        cJSON_AddItemToArray(subarray,  muy.q_u_get_piece_cey_u_orderdish(query, n_func::f_get_sysparm("orgCode")));
    }
    //checkOutDetail
    query.execSelect(QString("select * from cey_u_checkout_detail where ch_payno = '%1' ; ").arg(ch_payno));
    cJSON_AddItemToObject(root, "checkOutDetail", subarray = cJSON_CreateArray());
    while(query.next()) {
        cJSON_AddItemToArray(subarray,  muy.q_u_get_piece_cey_u_checkout_detail(query, n_func::f_get_sysparm("orgCode")));
    }

    //
    QByteArray param = cJSON_help::getcJSONprint(root);
    cJSON_Delete(root);
    root=NULL;

    //开始向云端插入
    //获取map
    QMap<QString, QString> headInfo;
    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    yunpayData ret;
    QString http_key=http_www_yqspos_com +"/cvs/member/fuk-rest";
    QString retparam;
#ifdef QT_DEBUG
    parent->objectName();
#endif
    httpDataAutoDel receive_json = get_yun_httppos(parent, http_key, param, headInfo, retparam,QObject::tr("向云端提交云会员信息"));
    ret.code = cJSON_help::getcJSONvalue(receive_json.json, "code").toString();
    ret.maxOrderNo = cJSON_help::getcJSONvalue(receive_json.json, "maxOrderNo").toString();
    ret.message = cJSON_help::getcJSONvalue(receive_json.json, "message").toString();

    return ret;
}

bool w_sys_manage_cloudsync::get_yun_member_recharge(QWidget *parent,const QString &cardNo, const QString &operId, const QString &chongzje, const QString &shisje, const QString &zhiffs)
{
    QByteArray param;
    QMap<QString, QString> headInfo;

    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString httpkey = http_www_yqspos_com +"/cvs/member/chongz-rest";
    param = cJSON_help::mapTocJSONByteArray(ldsVariantMap()
                                            .append("cardNo", cardNo)
                                            .append("operId", operId)
                                            .append("chongzje", chongzje)
                                            .append("shisje", shisje)
                                            .append("zhiffs", zhiffs)
                                            );

#ifdef QT_DEBUG
    parent->objectName();
#endif
    httpDataAutoDel receive_json=get_yun_httppos(parent, httpkey, param, headInfo, QObject::tr("连锁会员充值"));

    return receive_json.httpStatus == 200;
}

bool w_sys_manage_cloudsync::get_yun_member_blankcard(QWidget *parent,const QString &cardNo, const QString &tuikje, const QString &zhiffs)
{
    QByteArray param;
    QMap<QString, QString> headInfo;

    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString httpkey = http_www_yqspos_com +"/cvs/member/tuik-rest";
    param = cJSON_help::mapTocJSONByteArray(ldsVariantMap()
                                            .append("cardNo", cardNo)
                                            .append("tuikje", tuikje)
                                            .append("orgCode", n_func::f_get_sysparm("orgCode"))
                                            .append("zhiffs", zhiffs)
                                            );

#ifdef QT_DEBUG
    parent->objectName();
#endif
    httpDataAutoDel receive_json=get_yun_httppos(parent, httpkey, param, headInfo, QObject::tr("退卡"));
    return receive_json.httpStatus == 200;
}

bool w_sys_manage_cloudsync::get_yun_member_fetch(QWidget *parent,const QString &cardNo, const QString &operId, const QString &qukje, const QString &zhiffs)
{
    QMap<QString, QString> headInfo;

    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString httpkey = http_www_yqspos_com +"/cvs/member/quk-rest";
    QByteArray param = cJSON_help::mapTocJSONByteArray(ldsVariantMap()
                                                       .append("cardNo", cardNo)
                                                       .append("operId", operId)
                                                       .append("qukje", qukje)
                                                       .append("zhiffs", zhiffs)
                                                       );

#ifdef QT_DEBUG
    parent->objectName();
#endif
    httpDataAutoDel receive_json=get_yun_httppos(parent, httpkey, param, headInfo, QObject::tr("取款"));
    return receive_json.httpStatus == 200;
}

bool w_sys_manage_cloudsync::get_yun_member_save(QWidget *parent,const membernewparam &value)
{
    QMap<QString, QString> headInfo;

    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString httpkey = http_www_yqspos_com +"/cvs/member/save-rest";
    QByteArray param = cJSON_help::mapTocJSONByteArray(ldsVariantMap()
                                                       .append("miChCardFlag", value.miChCardFlag)
                                                       .append("miChState", value.miChState)
                                                       .append("miChStateStr", value.miChStateStr)
                                                       .append("miChTypeNo", value.miChTypeNo)

                                                       .append("miDtBirthday", value.miDtBirthday)
                                                       .append("miDtLimit", value.miDtLimit)
                                                       .append("miDtOperDate", value.miDtOperDate)
                                                       .append("miDtSendTime", value.miDtSendTime)
                                                       .append("idStr", value.idStr)

                                                       .append("miIntBasedata", value.miIntBasedata)
                                                       .append("miIntBasedataStr", value.miIntBasedataStr)
                                                       .append("numLimit", value.numLimit)
                                                       .append("operate", value.operate)

                                                       .append("orgCode", value.orgCode)
                                                       .append("remark", value.remark)
                                                       .append("miVchAddress", value.miVchAddress)
                                                       .append("miVchCardNo", value.miVchCardNo)
                                                       .append("miVchHandTel", value.miVchHandTel)

                                                       .append("miVchId", value.miVchId)
                                                       .append("miVchMember", value.miVchMember)
                                                       .append("miVchMemberNo", value.miVchMemberNo)
                                                       .append("miVchOperId", value.miVchOperId)
                                                       .append("miVchPassword", value.miVchPassword)

                                                       .append("miVchTel", value.miVchTel)
                                                       );

#ifdef QT_DEBUG
    parent->objectName();
#endif
    httpDataAutoDel receive_json=get_yun_httppos(parent, httpkey, param, headInfo, QObject::tr("新增会员卡"));
    return receive_json.httpStatus == 200;
}

bool w_sys_manage_cloudsync::get_yun_member_save(const membernewparam &value, QString &errstring)
{
    QByteArray readall;
    int httpcode;
    QString httpkey = http_www_yqspos_com +"/cvs/member/save-rest";
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");
    request.setRawHeader("terminalCode", lds::terminalCode.toLocal8Bit());
    request.setUrl(httpkey);

    QByteArray param = cJSON_help::mapTocJSONByteArray(ldsVariantMap()
                                                       .append("miChCardFlag", value.miChCardFlag)
                                                       .append("miChState", value.miChState)
                                                       .append("miChStateStr", value.miChStateStr)
                                                       .append("miChTypeNo", value.miChTypeNo)

                                                       .append("miDtBirthday", value.miDtBirthday)
                                                       .append("miDtLimit", value.miDtLimit)
                                                       .append("miDtOperDate", value.miDtOperDate)
                                                       .append("miDtSendTime", value.miDtSendTime)
                                                       .append("idStr", value.idStr)

                                                       .append("miIntBasedata", value.miIntBasedata)
                                                       .append("miIntBasedataStr", value.miIntBasedataStr)
                                                       .append("numLimit", value.numLimit)
                                                       .append("operate", value.operate)

                                                       .append("orgCode", value.orgCode)
                                                       .append("remark", value.remark)
                                                       .append("miVchAddress", value.miVchAddress)
                                                       .append("miVchCardNo", value.miVchCardNo)
                                                       .append("miVchHandTel", value.miVchHandTel)

                                                       .append("miVchId", value.miVchId)
                                                       .append("miVchMember", value.miVchMember)
                                                       .append("miVchMemberNo", value.miVchMemberNo)
                                                       .append("miVchOperId", value.miVchOperId)
                                                       .append("miVchPassword", value.miVchPassword)

                                                       .append("miVchTel", value.miVchTel)
                                                       );

    if(false == lds_network::managerPost(request, param, readall, errstring, httpcode)) {
        return false;
    }
    if(200 != httpcode) {
        errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
        return false;
    }
    return true;
}

bool w_sys_manage_cloudsync::get_yun_member_send(QWidget *parent,const QString &cardNo)
{
    QString orgCode;
    QMap<QString, QString> headInfo;

    orgCode = n_func::f_get_sysparm("orgCode");
    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode", orgCode);
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString httpkey = http_www_yqspos_com +"/cvs/member/fak-rest";
    QByteArray param = cJSON_help::mapTocJSONByteArray(ldsVariantMap()
                                                       .append("cardNo", cardNo)
                                                       .append("orgCode", orgCode)
                                                       );

#ifdef QT_DEBUG
    parent->objectName();
#endif
    httpDataAutoDel receive_json=get_yun_httppos(parent, httpkey, param, headInfo, QObject::tr("云会员发卡"));
    return 200 == receive_json.httpStatus;
}

bool w_sys_manage_cloudsync::get_yun_member_send(const QString &cardNo, QString &errstring)
{
    QByteArray readall;
    int httpcode;
    QString httpkey = http_www_yqspos_com +"/cvs/member/fak-rest";
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");
    request.setRawHeader("terminalCode", lds::terminalCode.toLocal8Bit());
    request.setUrl(httpkey);
    QString orgCode = n_func::f_get_sysparm("orgCode");

    QByteArray param = cJSON_help::mapTocJSONByteArray(ldsVariantMap()
                                                       .append("cardNo", cardNo)
                                                       .append("orgCode", orgCode)
                                                       );

    if(false == lds_network::managerPost(request, param, readall, errstring, httpcode)) {
        return false;
    }
    if(200 != httpcode) {
        errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
        return false;
    }
    return true;
}

bool w_sys_manage_cloudsync::get_yun_member_select_pre100(QWidget *parent, const QString &filter, QList<yunMemberSelectInfo > &selects)
{
    QString httpkey;
    QMap<QString, QString> headInfo;
    int rowcount;
getyunmlist:
    if(json_yun_readcard_select.isNull()) {
        httpkey = http_www_yqspos_com +"/cvs/member/memberCardList-rest";
        headInfo.clear();
        headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
        headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
        headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
        headInfo.insert("param",filter);//!开始会检索所有的，后面会按照条件来检索
        httpDataAutoDel httpd = get_yun_httpget(parent, httpkey, headInfo, QObject::tr("检索云端会员"));
        json_yun_readcard_select = httpd.json;
    }
    // 访问失败
    if(json_yun_readcard_select.isNull())
        return false;
    rowcount = cJSON_help::array_count(json_yun_readcard_select);
    //! 固定是前五个
    for(int row = 0; row < rowcount && selects.count() <= 5/*最多显示5个*/; row++) {
        cJSON *pItem = cJSON_help::_cJSON_GetArrayItem(json_yun_readcard_select, row);
        yunMemberSelectInfo select;
        select.mname = cJSON_help::getcJSONvalue(pItem, "miVchMember").toString();
        select.mhandtel =  cJSON_help::getcJSONvalue(pItem, "miVchHandTel").toString();
        select.mcardno = cJSON_help::getcJSONvalue(pItem, "miVchMemberNo").toString();

        if(select.mhandtel.contains(filter)
                ||select.mcardno.contains(filter)) {
            selects.append(select);
        }
    }
    //获取数据为空或者云端返回为空
    if(selects.isEmpty() || rowcount == 0) {
        json_yun_readcard_select.clear();
        //云端返回不为空则重新获取
        if(rowcount > 0)  {
            goto getyunmlist;
        }
    }
    return rowcount > 0;
}

bool w_sys_manage_cloudsync::get_yun_member_INFO(const QString &cardNo, membernewparam &arg0, QString &errstring)
{
    QString httpkey = http_www_yqspos_com +"/cvs/member/getCvsMemberInfoByCard-rest";
    QByteArray param = cJSON_help::mapTocJSONByteArray(ldsVariantMap()
                                                       .append("cardNo", cardNo)
                                                       );
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");
    request.setRawHeader("terminalCode", lds::terminalCode.toLocal8Bit());
    request.setUrl(httpkey);

    QByteArray readall;
    int httpcode;
    if(false == lds_network::managerPost(request, param, readall, errstring, httpcode)) {
        return false;
    }
    if(200 != httpcode) {
        errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
        return false;
    }
    cJSON_help json(readall);

    //===
    arg0.idStr = cJSON_help::getcJSONvalue(json, "idStr").toString();
    arg0.miChCardFlag = cJSON_help::getcJSONvalue(json, "miChCardFlag").toString();
    arg0.miChState = cJSON_help::getcJSONvalue(json, "miChState").toString();
    arg0.miChStateStr = cJSON_help::getcJSONvalue(json, "miChStateStr").toString();

    arg0.miChTypeNo = cJSON_help::getcJSONvalue(json, "miChTypeNo").toString();
    arg0.miDtBirthday = cJSON_help::getcJSONvalue(json, "miDtBirthday").toString();
    arg0.miDtLimit = cJSON_help::getcJSONvalue(json, "miDtLimit").toString();
    arg0.miDtOperDate = cJSON_help::getcJSONvalue(json, "miDtOperDate").toString();
    arg0.miDtSendTime = cJSON_help::getcJSONvalue(json, "miDtSendTime").toString();

    arg0.miIntBasedata = cJSON_help::getcJSONvalue(json, "miIntBasedata").toLongLong();
    arg0.miIntBasedataStr = cJSON_help::getcJSONvalue(json, "miIntBasedataStr").toString();
    arg0.miVchAddress = cJSON_help::getcJSONvalue(json, "miVchAddress").toString();
    arg0.miVchCardNo = cJSON_help::getcJSONvalue(json, "miVchCardNo").toString();
    arg0.miVchHandTel = cJSON_help::getcJSONvalue(json, "miVchHandTel").toString();

    arg0.miVchId = cJSON_help::getcJSONvalue(json, "miVchId").toString();
    arg0.miVchMember = cJSON_help::getcJSONvalue(json, "miVchMember").toString();
    arg0.miVchMemberNo = cJSON_help::getcJSONvalue(json, "miVchMemberNo").toString();
    arg0.miVchOperId = cJSON_help::getcJSONvalue(json, "miVchOperId").toString();
    arg0.miVchPassword = cJSON_help::getcJSONvalue(json, "miVchPassword").toString();

    arg0.miVchTel = cJSON_help::getcJSONvalue(json, "miVchTel").toString();
    arg0.numLimit = cJSON_help::getcJSONvalue(json, "numLimit").toLongLong();
    arg0.operate = cJSON_help::getcJSONvalue(json, "operate").toString();
    arg0.orgCode = cJSON_help::getcJSONvalue(json, "orgCode").toString();

    arg0.remark = cJSON_help::getcJSONvalue(json, "remark").toString();
    //===
    qDebug() << __FILE__ << __LINE__ ;
    qDebug() << json.jsonValue();
    return true;
}

bool w_sys_manage_cloudsync::put_yun_member_point_add_sub(QWidget *parent, const QString &memberno, const QString &type, const QString &payno, float point, const QString &memo, const QString &operid)
{
    QMap<QString, QString> headInfo;

    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString httpkey = http_www_yqspos_com +"/cvs/member/memberTypeList-rest";
    QByteArray param = cJSON_help::mapTocJSONByteArray(ldsVariantMap()
                                                       .append("memberno", memberno)
                                                       .append("type", type)
                                                       .append("payno", payno)
                                                       .append("point", point)
                                                       .append("memo", memo)
                                                       .append("operid", operid)
                                                       );
#ifdef QT_DEBUG
    parent->objectName();
#endif
    httpDataAutoDel receive_json=get_yun_httppos(parent, httpkey, param, headInfo, QObject::tr("会员积分充值"));
    if(receive_json.json == NULL) return false;

    return true;
}

bool w_sys_manage_cloudsync::member_mobileNo_verify(const QString &telephone, const QString &code, QString &errstring)
{
    QNetworkAccessManager m;
    QEventLoop loop;
    QObject::connect(&m, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply *reply;
    QNetworkRequest request;
    QByteArray readall;
    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
    request.setRawHeader("telephone",  telephone.toLocal8Bit());
    request.setRawHeader("code",  code.toLocal8Bit());

#if defined(QT_DEBUG )
    request.setRawHeader("terminalCode",  lds::terminalCode.toLocal8Bit());
    request.setUrl(QUrl("http://192.168.1.81/cvs/member/sendCode-rest"));
#else
    request.setRawHeader("terminalCode",  lds::terminalCode.toLocal8Bit());
    request.setUrl(QUrl(http_www_yqspos_com +"/cvs/member/sendCode-rest"));
#endif
    reply = m.get(request);
    loop.exec();
    readall = reply->readAll();
    qDebug() <<  "url" << request.url();
    qDebug() << "terminalCode:" << request.rawHeader("terminalCode");
    qDebug() << "code:" << request.rawHeader("code");
    qDebug() << "telephone:" << request.rawHeader("telephone");
    qDebug() << "readall:" << readall;
    if(200 == reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()) {
        if("200" == cJSON_help::getcJSONvalue(readall, "code").toString()) {
            return true;
        }
    }
    errstring = cJSON_help::getcJSONvalue(readall, "msg").toString();
    return false;
}

bool w_sys_manage_cloudsync::get_yun_coupon(QDate &useDate, double &money, double &use, const QString &orderSn, QString &errstring)
{
    QByteArray readall;
    QNetworkAccessManager m;
    QEventLoop loop;
    connect(&m,SIGNAL(finished(QNetworkReply*)),&loop,SLOT(quit()));

    QNetworkRequest request;
    request.setRawHeader("terminalCode", lds::terminalCode.toLocal8Bit());
    request.setRawHeader("userCouponSn", orderSn.toLocal8Bit());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");
    request.setUrl(QUrl("http://115.28.155.165/qryqs/couponRest/getCoupon-rest"));

    QNetworkReply *reply = m.post(request, "");
    loop.exec();
    readall = reply->readAll();
    if(200 == reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()) {
        useDate = QDate::fromString(cJSON_help::getcJSONvalue(readall, "useDate").toString(),"yyyy-MM-dd");
        money = cJSON_help::getcJSONvalue(readall, "money").toDouble();
        use = cJSON_help::getcJSONvalue(readall, "use").toDouble();

        qDebug() << readall;
        qDebug() << useDate;
        qDebug() << money;
        qDebug() << use;
        return true;
    }
    errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
    return false;
}

bool w_sys_manage_cloudsync::verify_yun_coupon(const QString &orderSn, QString &errstring)
{
    QByteArray readall;
    QNetworkAccessManager m;
    QEventLoop loop;
    connect(&m,SIGNAL(finished(QNetworkReply*)),&loop,SLOT(quit()));

    QNetworkRequest request;
    request.setRawHeader("terminalCode", lds::terminalCode.toLocal8Bit());
    request.setRawHeader("userCouponSn", orderSn.toLocal8Bit());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");
    request.setUrl(QUrl("http://115.28.155.165/qryqs/couponRest/usedCoupon-rest"));

    QNetworkReply *reply = m.post(request, "");
    loop.exec();
    readall = reply->readAll();
    if(200 == reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()) {
        return true;
    }
    errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
    return false;
}

w_sys_manage_cloudsync::CvsMemberOperaInfo w_sys_manage_cloudsync::getCvsMemberOperaInfo(QWidget *parent, const QString &operId, const QString &dateStart, const QString &dateEnd, bool &ok)
{
    CvsMemberOperaInfo ret;
    QMap<QString, QString> headInfo;
    ok = false;

    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QByteArray param = cJSON_help::mapTocJSONByteArray(ldsVariantMap()
                                                       .append("operId", operId)
                                                       .append("dateStart", dateStart)
                                                       .append("dateEnd", dateEnd)
                                                       );
    QString httpkey = http_www_yqspos_com +"/cvs/member/getCvsMemberOperaInfo-rest";
    httpDataAutoDel receive_json=get_yun_httppos(parent, httpkey, param, headInfo, QObject::tr("获取会员操作信息"), false);
    if(receive_json.json == NULL) return ret;
    ret.rechargeAmount = cJSON_help::getcJSONvalue(receive_json.json, "rechargeAmount").toDouble();
    ret.realRechargeAmount = cJSON_help::getcJSONvalue(receive_json.json, "realRechargeAmount").toDouble();
    ret.withdrawalAmount = cJSON_help::getcJSONvalue(receive_json.json, "withdrawalAmount").toDouble();
    ret.backCardAmount = cJSON_help::getcJSONvalue(receive_json.json, "backCardAmount").toDouble();
    cJSON * rechargeDetail = cJSON_help::getcJSON(receive_json.json, "rechargeDetail");
    for(int k = 0, count = cJSON_help::array_count(rechargeDetail); k < count; k ++) {
        ret.rechargeDetail.append(CvsMemberOperaInfoRecharge{
                                      cJSON_help::getArrayValue(rechargeDetail, k, "code").toString(),
                                      cJSON_help::getArrayValue(rechargeDetail, k, "amount").toDouble()
                                  });
    }
    qDebug() <<  receive_json.jsonToString();
    ok = true;
    return ret;
}

void w_sys_manage_cloudsync::json_free()
{
    json_yun_parameter.clear();
    json_yun_readcard.clear();
    json_yun_membertype.clear();
    json_yun_readcard_select.clear();
}
//-1云端失败 0本地校对失败 1校对成功 2无需校对//
int w_sys_manage_cloudsync::match_local_time(QWidget *parent)
{
    //获取map
    //2
    QString errstring;
    yunTime ydt;
    is_match_local_time = false;
    //-1云端失败 0时间不符 1时间相符
    int ret = check_local_time(parent, ydt, &errstring);
    if(ret == -1) {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,  QObject::tr("操作失败") + "\n"+ errstring);//QObject::tr("请检测云端设置是否正确或者网线是否拔出"));
        return -1;//-1云端失败
    }
    if(ret == 1) {
        return 2;//2无需校对
    }
    if(ret == 0) {//0时间不符
        if(public_sql::getdbHostName() == "127.0.0.1"
                || public_sql::localContainIp(public_sql::getdbHostName())
                ) {

        } else {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,  QObject::tr("服务器非本机服务器,请在服务器所在机器上校对时间"));
            return -1;//-1云端失败
        }
        if(0 == lds_messagebox::question(parent, MESSAGE_TITLE_VOID,  QObject::tr("数据库时间和云端时间不同步,是否校对"), QObject::tr("校对"), QObject::tr("取消"))) {
            //
            bool success = true;
            //再获取一次时间
            bool ok;
            ydt = w_sys_manage_cloudsync::get_yun_srvdt(parent, ok);
            if(!ok) return -1;//-1云端失败

            FILE *fd = popen(QString("date -s '%1' ").arg(ydt.time.toString("yyyy-MM-dd hh:mm:ss")).toLocal8Bit(), "r");
            if(fd < NULL) success = false;
            pclose(fd);
            fd = 0;

            if(success) {
                fd = popen("hwclock -w", "r");
                if(fd < NULL)success = false;
                pclose(fd);
                fd = 0;
            }

            //
            if(success) {
                is_match_local_time = true;
                lds_messagebox::information(parent, MESSAGE_TITLE_VOID, QObject::tr("同步成功"), true);
                return 1;
            } else {
                lds_messagebox::information(parent, MESSAGE_TITLE_VOID, QObject::tr("同步失败"));
            }
        }
    }
    return 0;
}

//-1云端失败 0时间不符 1时间相符
int w_sys_manage_cloudsync::check_local_time(QWidget *parent, yunTime &retydt, QString *msg)
{
    //3
    bool ok;
    //"------------begin----------------";
    retydt = w_sys_manage_cloudsync::get_yun_srvdt(parent, ok,msg);
    QDateTime local_dt = QDateTime::currentDateTime();
    // "------------end----------------";
    if(!ok) {
        return -1;//云端异常
    }
    //4
    if(qAbs(local_dt.secsTo(retydt.time)) > 6 * 60 * 60) {//时间不能超过6个小时
        if(msg)  *msg = local_dt.toString("yyyy-MM-dd hh:mm:ss") +"-->" + retydt.time.toString("yyyy-MM-dd hh:mm:ss");
        return 0;//时间不相符
    }
    is_match_local_time = true;
    return 1;//时间相符
}

bool w_sys_manage_cloudsync::get_yun_member_checkbillnopay(QWidget *parent,const QString &billno, const QString &memberno)
{
    QString ls_pay;
    //获取map
    QMap<QString, QString> headInfo;

    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    headInfo.insert("memberNo",memberno);
    headInfo.insert("billNo",billno);

    QString http_key=http_www_yqspos_com +"/cvs/member/checkBillNoPay-rest";
    httpDataAutoDel root = get_yun_httpget(parent, http_key, headInfo, "Member had paid");

    if(root.json.isNull()) return false;
    if(cJSON_help::array_count(root.json) > 0) {
        ls_pay = cJSON_help::getcJSONvalue(root.json, "pay").toString();
    }

    return ls_pay == "true";
}
//获取、服务器时间，会跳过时间校验
yunTime w_sys_manage_cloudsync::get_yun_srvdt(QWidget *parent, bool &ok, QString *errstring)
{
    yunTime ydt;
    ok = false;
    //获取map
    QMap<QString, QString> headInfo;

    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));

    QString http_key = http_www_yqspos_com +"/cvs/member/getServerTime-rest";
    httpDataAutoDel httpd = get_yun_httpget(parent, http_key, headInfo,QObject::tr("本地同步云端时间"),false, false);//允许弹出警告窗口、不会进行时间校验
    qDebug() << http_key;
    if(200 == httpd.httpStatus) {
        if(cJSON_help::array_count(httpd.json) > 0) {
            ydt.time = QDateTime::fromString(cJSON_help::getcJSONvalue(httpd.json, "time").toString(), "yyyy-MM-dd hh:mm:ss");
            ydt.printCheck = cJSON_help::getcJSONvalue(httpd.json, "printCheck").toString();
            ydt.printContext = cJSON_help::getcJSONvalue(httpd.json, "printContext").toString();
            ok = true;
        }
    }
    if(false == ok) {
        if(errstring) *errstring = httpd.message;
    }
    return ydt;
}

void member_up_yun::clearallList()
{   
    memberPointListList.clear();
    memberPayList.clear();
    memberCuramountList.clear();
    memberDepositList.clear();
}

int member_up_yun::finditem(const memberCuramount &o)
{
    for(int k = 0; k < memberCuramountList.count(); k++) {
        if(memberCuramountList[k].vchMemberno == o.vchMemberno)
            return k;
    }
    return -1;
}

cJSON *member_up_yun::q_u_get_piece_cey_u_checkout_master(lds_query_hddpos  &query, const QString &orgCode)
{
    cJSON *item = w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_checkout_master(query,  orgCode);
    cJSON_help::replaceItemValue(item, "comChPayNo", ch_payno);
    cJSON_help::replaceItemValue(item, "comChBillNo", ch_billno);
    return item;
}

cJSON *member_up_yun::q_u_get_piece_cey_u_master(lds_query_hddpos  &query, const QString &orgCode)
{
    cJSON *item = w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_master(query,  orgCode);
    cJSON_help::replaceItemValue(item, "omChBillNo", ch_billno);
    return item;
}

cJSON *member_up_yun::q_u_get_piece_cey_u_table(lds_query_hddpos  &query, const QString &orgCode)
{
    cJSON *item = w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_table(query,  orgCode);
    cJSON_help::replaceItemValue(item, "chPayno", ch_payno);
    cJSON_help::replaceItemValue(item, "chBillno", ch_billno);
    return item;
}

cJSON *member_up_yun::q_u_get_piece_cey_u_orderdish(lds_query_hddpos  &query, const QString &orgCode)
{
    cJSON *item = w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_orderdish(query,  orgCode);
    cJSON_help::replaceItemValue(item, "bdChPayNo", ch_payno);
    cJSON_help::replaceItemValue(item, "bdChBillNo", ch_billno);
    return item;
}

cJSON *member_up_yun::q_u_get_piece_cey_u_checkout_detail(lds_query_hddpos  &query, const QString &orgCode)
{
    cJSON *item = w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_checkout_detail(query,  orgCode);
    cJSON_help::replaceItemValue(item, "codChPayNo", ch_payno);
    return item;
}


cJSON *member_up_yun::make_JSON_Array(const memberPointList &p, const QString &orgCode)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_help::_cJSON_AddValueToObject(root, "mpIntFlowId", p.mpIntFlowId);
    cJSON_help::_cJSON_AddValueToObject(root, "mpVchMemberNo", p.mpVchMemberNo);
    cJSON_help::_cJSON_AddValueToObject(root, "mpChType", p.mpChType);
    cJSON_help::_cJSON_AddValueToObject(root, "mpChPayNo", p.mpChPayNo);
    cJSON_help::_cJSON_AddValueToObject(root, "mpNumPoint", p.mpNumPoint);
    cJSON_help::_cJSON_AddValueToObject(root, "mpVchMemo", p.mpVchMemo);
    cJSON_help::_cJSON_AddValueToObject(root, "mpVchOperId", p.mpVchOperId);
    cJSON_help::_cJSON_AddValueToObject(root, "mpDtOperDate", p.mpDtOperDate);

    cJSON_help::_cJSON_AddValueToObject(root, "orgCode", orgCode);
    cJSON_help::_cJSON_AddValueToObject(root, "terminalCode", lds::terminalCode.toLocal8Bit());

    return root;
}

cJSON *member_up_yun::make_JSON_Array(const memberPay &p, const QString &orgCode)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_help::_cJSON_AddValueToObject(root, "intFlowid", p.intFlowid);
    cJSON_help::_cJSON_AddValueToObject(root, "mpVchMemberNo", p.mpVchMemberNo);
    cJSON_help::_cJSON_AddValueToObject(root, "mpChPayNo", p.mpChPayNo);
    cJSON_help::_cJSON_AddValueToObject(root, "mpChPayModeNo", p.mpChPayModeNo);
    cJSON_help::_cJSON_AddValueToObject(root, "mpNumAmount", p.mpNumAmount);
    cJSON_help::_cJSON_AddValueToObject(root, "mpNumPay", p.mpNumPay);
    cJSON_help::_cJSON_AddValueToObject(root, "mpVchOperId", p.mpVchOperId);
    cJSON_help::_cJSON_AddValueToObject(root, "mpDtOperDate", p.mpDtOperDate);
    cJSON_help::_cJSON_AddValueToObject(root, "mpChBalanceNo", p.mpChBalanceNo);
    cJSON_help::_cJSON_AddValueToObject(root, "mpIntCheckFlow", p.mpIntCheckFlow);

    cJSON_help::_cJSON_AddValueToObject(root, "orgCode", orgCode);
    cJSON_help::_cJSON_AddValueToObject(root, "terminalCode", lds::terminalCode.toLocal8Bit());

    return root;
}

cJSON *member_up_yun::make_JSON_Array(const memberCuramount &p, const QString &orgCode)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_help::_cJSON_AddValueToObject(root, "vchMemberno", p.vchMemberno);
    cJSON_help::_cJSON_AddValueToObject(root, "numAmount", p.numAmount);
    cJSON_help::_cJSON_AddValueToObject(root, "numHangamount", p.numHangamount);
    cJSON_help::_cJSON_AddValueToObject(root, "numPoint", p.numPoint);
    cJSON_help::_cJSON_AddValueToObject(root, "vchOperId", p.vchOperId);
    cJSON_help::_cJSON_AddValueToObject(root, "dtOperdate", p.dtOperdate);
    cJSON_help::_cJSON_AddValueToObject(root, "chBranchno", p.chBranchno);

    cJSON_help::_cJSON_AddValueToObject(root, "orgCode", orgCode);
    cJSON_help::_cJSON_AddValueToObject(root, "terminalCode", lds::terminalCode.toLocal8Bit());

    return root;
}

cJSON *member_up_yun::make_JSON_Array(const memberDeposit &p, const QString &orgCode)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_help::_cJSON_AddValueToObject(root, "mdIntFlowId", p.mdIntFlowId);
    cJSON_help::_cJSON_AddValueToObject(root, "mdVchMemberNo", p.mdVchMemberNo);
    cJSON_help::_cJSON_AddValueToObject(root, "mdNumDeposit", p.mdNumDeposit);
    cJSON_help::_cJSON_AddValueToObject(root, "mdNumRealaMount", p.mdNumRealaMount);
    cJSON_help::_cJSON_AddValueToObject(root, "mdChDepositMode", p.mdChDepositMode);
    cJSON_help::_cJSON_AddValueToObject(root, "mdChPayMode", p.mdChPayMode);
    cJSON_help::_cJSON_AddValueToObject(root, "mdVchOperId", p.mdVchOperId);
    cJSON_help::_cJSON_AddValueToObject(root, "dtOperDate", p.dtOperDate);
    cJSON_help::_cJSON_AddValueToObject(root, "mdVchMemo", p.mdVchMemo);
    cJSON_help::_cJSON_AddValueToObject(root, "mdRemainAmt", p.mdRemainAmt);
    cJSON_help::_cJSON_AddValueToObject(root, "comChPayNo", p.comChPayNo);

    cJSON_help::_cJSON_AddValueToObject(root, "orgCode", orgCode);
    cJSON_help::_cJSON_AddValueToObject(root, "terminalCode", lds::terminalCode.toLocal8Bit());

    return root;
}

httpDataAutoDel::httpDataAutoDel()
{

}

httpDataAutoDel::httpDataAutoDel(cJSON *json)
{
    this->json = cJSON_help::createSharedPtr(json);
}

QByteArray httpDataAutoDel::jsonToByteArray()
{
    QByteArray ret;
    if(json.data() ) {
        ret = cJSON_help::getcJSONprint(json.data());
    }
    return ret;
}

QString httpDataAutoDel::jsonToString()
{
    return jsonToByteArray();
}


w_sys_manage_cloudsync::CvsMemberOperaInfo::CvsMemberOperaInfo()
{
    init();
}

void w_sys_manage_cloudsync::CvsMemberOperaInfo::print()
{
    qDebug() << "rechargeAmount" << rechargeAmount;
    qDebug() << "realRechargeAmount" << realRechargeAmount;
    qDebug() << "withdrawalAmount" << withdrawalAmount;
    qDebug() << "backCardAmount" << backCardAmount;
    for(int k = 0; k < rechargeDetail.count(); k ++) {
        qDebug() << __LINE__ <<  "rechargeDetail/amount" << rechargeDetail[k].amount;
        qDebug() << __LINE__ <<  "rechargeDetail/code" << rechargeDetail[k].code;
    }
}

void w_sys_manage_cloudsync::CvsMemberOperaInfo::init()
{
    rechargeAmount = 0;//充值金额
    realRechargeAmount = 0;//充值实收
    rechargeDetail.clear();
    withdrawalAmount = 0;//取款金额
    backCardAmount = 0;//退卡金额
}
