#include "w_sys_manage_cloudsync_with.h"
#include "cJSON.h"
#include "cjson_help.h"
#include "w_sys_manage.h"
#include "lds.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include <QVariant>
#include <QNetworkReply>
#include "cjson_help.h"

QMap<QString, int> w_sys_manage_cloudsync_with::map;

w_sys_manage_cloudsync_with::w_sys_manage_cloudsync_with(QObject *parent) :
    QObject(parent)
{
}

bool w_sys_manage_cloudsync_with::data_upload(lds_messagebox_loading_show *loading, bool skip_ch_yun_already_keep_Y, QString &errstring)
{
    map.clear();
    //
    int count = 500;// = 0的话会死循环
    QString table_name;
    QString table_name_yun;
    QString sql;
    lds_query_hddpos  query;
    QNetworkAccessManager m;
    QEventLoop loop;
    connect(&m,SIGNAL(finished(QNetworkReply*)),&loop,SLOT(quit()));

    ///基础资料
    if(skip_ch_yun_already_keep_Y) {
        sql = QString(" select * from %1");
    } else {
        sql = QString(" select * from %1 where ifnull(ch_yun_already_keep,'') <> 'Y' ");
    }
    query.execSelect("select * from cey_web_table_version ");
    while(query.next()) {
        table_name = query.recordValue("vch_sqltablename").toString();
        table_name_yun = query.recordValue("vch_sqltablename_yun").toString();

        if(false == upload_base_table_while(loading, m, loop, table_name, table_name_yun, sql.arg(table_name), 0, count, errstring)) {
            qDebug() << __LINE__ << "errstring:" << errstring;
            return false;
        }
        if(false == upload_base_table_while(loading, m, loop, table_name, table_name_yun, sql.arg( "del_" + table_name), -1, count, errstring)) {
            qDebug() << __LINE__ << "errstring:" << errstring;
            return false;
        }
    }
    ///会员参数
    sql = QString(" select * from %1");
    table_name = "t_m_parameter";
    table_name_yun = "memberParameterList";
    if(false == upload_base_table_while(loading, m, loop, table_name, table_name_yun, sql.arg(table_name), 0, count, errstring)) {
        qDebug() << __LINE__ << "errstring:" << errstring;
        return false;
    }
    ///促销
    table_name = "cey_sys_parameter";
    table_name_yun = "dishSysParameter";
    if(false == upload_promotion_table_while(loading, m, loop, table_name, table_name_yun, "", 0, count, errstring)) {
        qDebug() << __LINE__ << "errstring:" << errstring;
        return false;
    }

    ///库存
    sql = QString(" select * from %1");
    table_name = "cey_st_material_num";
    table_name_yun = "dishInfoList";
    if(false == upload_material_table_while(loading, m, loop, table_name, table_name_yun, sql.arg(table_name), 0, count, errstring)) {
        qDebug() << __LINE__ << "errstring:" << errstring;
        return false;
    }
    //
    return true;
}

bool w_sys_manage_cloudsync_with::data_download(lds_messagebox_loading_show *loading, bool down_all, QString &errstring)
{
    QNetworkRequest request;
    QByteArray readall;
    QNetworkReply *reply;
    lds_query_hddpos  query;
    QString table_name;
    QString table_name_yun;
    QString  key;
    QString  key_yun;
    QNetworkAccessManager m;
    QEventLoop loop;
    connect(&m,SIGNAL(finished(QNetworkReply*)),&loop,SLOT(quit()));

    lds_query_hddpos::tran_saction();
    if(down_all) {
        loading->setText(QObject::tr("清空本地商品"));
        query.execSelect("select vch_sqltablename from cey_web_table_version");
        while(query.next()) {
            lds_query_hddpos::deleteTable(query.recordValue("vch_sqltablename").toString(), "");
        }
        lds_query_hddpos::updateTable("cey_web_table_version", qrtVariantPairList()
                               << qrtVariantPair("int_version", 0), "");
    }
    cJSON_help json_version(w_sys_manage_cloudsync_with_time_running::q_bt_get_table_version_array());
    //
    request.setUrl(QString(http_www_yqspos_com +"/arm/dish-base/get-rest"));
    request.setRawHeader("version", json_version.jsonValue().simplified());
    request.setRawHeader("terminalCode", lds::terminalCode.toLocal8Bit());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");
    loading->setText(QObject::tr("获取商品"));
    qDebug() << "url:" << request.url();
    qDebug() << "version" << request.rawHeader("version");
    qDebug() << "terminalCode" << request.rawHeader("terminalCode");
    qDebug() << __LINE__ << "get";
    qDebug() << request.url();
    reply = m.get(request);
    loop.exec();
    readall = reply->readAll();
    qDebug() << "readall:" << readall;
    if(200 != reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()) {
        lds_query_hddpos::roll_back();
        errstring = "GET_BASE_FROM_SERVER_FAILED:" + cJSON_help::getcJSONvalue(readall, "message").toString();
        return false;
    }

    cJSON_help root(readall);
    ///
    query.execSelect("select * from cey_web_table_version ");
    while(query.next()) {
        table_name = query.recordValue("vch_sqltablename").toString();
        table_name_yun = query.recordValue("vch_sqltablename_yun").toString();
        key = query.recordValue("vch_sqltablename_key_name").toString();
        key_yun = query.recordValue("vch_sqltablename_yun_key_name").toString();
        QStringList key_yun_list = key_yun.split(",");
        QStringList key_list = key.split(",");
        loading->setText(QObject::tr("基础资料") + ":" + table_name);

        cJSON *array =  cJSON_help::getcJSON(root, table_name_yun.toLocal8Bit());
        for(int k =0, count = cJSON_GetArraySize(array); k< count; k++) {
            cJSON *item = cJSON_GetArrayItem(array, k);
            int state = cJSON_help::getcJSONvalue(item,"state").toInt();
            QString arg;
            for(int k = 0; k < key_list.count(); k++) {
                arg += QString("%1 = '%2' and ")
                        .arg(key_list.value(0))
                        .arg(cJSON_help::getcJSONvalue(item, key_yun_list.value(k).toLocal8Bit()).toString());
            }
            arg.chop(4);

            if(-1 == state) {//删除
                lds_query_hddpos::deleteTable(table_name, arg);
            }
            if(0 == state) {//更新
                qrtVariantPairList key_value_list;
                w_sys_manage::q_put_piece_table(table_name, key_value_list, item);
                if(lds_query_hddpos::selectHasNext(QString("select 0 from %1 where %2").arg(table_name).arg(arg))) {
                    lds_query_hddpos::updateTable(table_name, key_value_list, arg);
                } else {
                    lds_query_hddpos::insertTable(table_name, key_value_list);
                }
            }
        }
    }
    ///会员参数
    {
        loading->setText(QObject::tr("会员参数"));
        table_name = "t_m_parameter";
        table_name_yun = "memberParameterList";
        cJSON *array =  cJSON_help::getcJSON(root, table_name_yun.toLocal8Bit());
        lds_query_hddpos::deleteTable(table_name, "");

        for(int k =0; k< cJSON_GetArraySize(array); k++) {
            cJSON *item = cJSON_GetArrayItem(array, k);
            qrtVariantPairList key_value_list;
            w_sys_manage::q_put_piece_table(table_name, key_value_list, item);
            lds_query_hddpos::insertTable(table_name, key_value_list);
        }
    }
    ///促销
    {
        loading->setText(QObject::tr("促销"));
        table_name = "cey_sys_parameter";
        table_name_yun = "dishSysParameter";
        cJSON *item =  cJSON_help::getcJSON(root, table_name_yun.toLocal8Bit());
        w_sys_manage::q_bt_put_piece_promotion(item);
    }
    ///版本号
    loading->setText(QObject::tr("版本号"));
    if(false == w_sys_manage_cloudsync_with_time_running::q_bt_put_table_version(root)) {
        lds_query_hddpos::roll_back();
        errstring = "TABLE_VERSION_UPDATE_FAILED";
        return false;
    }
    ///更新组号
    query.execUpdate("cey_bt_dish", "ch_groupno", qrtFunc("ch_dishno"), " ifnull(ch_groupno, '') = '' ");
    //
    if(false == w_sys_manage_cloudsync_with_time_running::q_bt_put_ch_yun_already_keep_Y()) {
        lds_query_hddpos::roll_back();
        errstring = "TABLE_STATE_UPDATE_FAILED";
        return false;
    }
    //
    query.execInsertDuplicateUpdate("cey_bt_paymode", "ch_paymodeno", qrtVariantPairList() << qrtVariantPair("ch_paymodeno","00") << qrtVariantPair("vch_paymodename","现金") << qrtVariantPair("ch_IDflag","N") << qrtVariantPair("ch_faceflag","N") << qrtVariantPair("ch_changeflag","Y") << qrtVariantPair("ch_incomeflag","Y") << qrtVariantPair("ch_key","A") << qrtVariantPair("vch_arg4","N"));
    query.execInsertDuplicateUpdate("cey_bt_paymode", "ch_paymodeno", qrtVariantPairList() << qrtVariantPair("ch_paymodeno","02") << qrtVariantPair("vch_paymodename","充值卡") << qrtVariantPair("ch_IDflag","Y") << qrtVariantPair("ch_faceflag","N") << qrtVariantPair("ch_changeflag","N") << qrtVariantPair("ch_incomeflag","Y") << qrtVariantPair("ch_key","C") << qrtVariantPair("vch_arg4","N"));
    query.execInsertDuplicateUpdate("cey_bt_paymode", "ch_paymodeno", qrtVariantPairList() << qrtVariantPair("ch_paymodeno","03") << qrtVariantPair("vch_paymodename","银行卡") << qrtVariantPair("ch_IDflag","Y") << qrtVariantPair("ch_faceflag","N") << qrtVariantPair("ch_changeflag","N") << qrtVariantPair("ch_incomeflag","Y") << qrtVariantPair("ch_key","Y") << qrtVariantPair("vch_arg4","N"));
    query.execInsertDuplicateUpdate("cey_bt_paymode", "ch_paymodeno", qrtVariantPairList() << qrtVariantPair("ch_paymodeno","04") << qrtVariantPair("vch_paymodename","代金券") << qrtVariantPair("ch_IDflag","Y") << qrtVariantPair("ch_faceflag","N") << qrtVariantPair("ch_changeflag","N") << qrtVariantPair("ch_incomeflag","Y") << qrtVariantPair("ch_key","E") << qrtVariantPair("vch_arg4","N"));
    query.execInsertDuplicateUpdate("cey_bt_paymode", "ch_paymodeno", qrtVariantPairList() << qrtVariantPair("ch_paymodeno","05") << qrtVariantPair("vch_paymodename","微信支付") << qrtVariantPair("ch_IDflag","N") << qrtVariantPair("ch_faceflag","N") << qrtVariantPair("ch_changeflag","N") << qrtVariantPair("ch_incomeflag","Y") << qrtVariantPair("ch_key","W") << qrtVariantPair("vch_arg4","N"));
    query.execInsertDuplicateUpdate("cey_bt_paymode", "ch_paymodeno", qrtVariantPairList() << qrtVariantPair("ch_paymodeno","06") << qrtVariantPair("vch_paymodename","支付宝支付") << qrtVariantPair("ch_IDflag","N") << qrtVariantPair("ch_faceflag","N") << qrtVariantPair("ch_changeflag","N") << qrtVariantPair("ch_incomeflag","Y") << qrtVariantPair("ch_key","") << qrtVariantPair("vch_arg4","N"));
    query.execInsertDuplicateUpdate("cey_bt_paymode", "ch_paymodeno", qrtVariantPairList() << qrtVariantPair("ch_paymodeno","07") << qrtVariantPair("vch_paymodename","百度糯米") << qrtVariantPair("ch_IDflag","N") << qrtVariantPair("ch_faceflag","N") << qrtVariantPair("ch_changeflag","N") << qrtVariantPair("ch_incomeflag","Y") << qrtVariantPair("ch_key","") << qrtVariantPair("vch_arg4","T"));
    query.execInsertDuplicateUpdate("cey_bt_paymode", "ch_paymodeno", qrtVariantPairList() << qrtVariantPair("ch_paymodeno","08") << qrtVariantPair("vch_paymodename","大众点评") << qrtVariantPair("ch_IDflag","N") << qrtVariantPair("ch_faceflag","N") << qrtVariantPair("ch_changeflag","N") << qrtVariantPair("ch_incomeflag","Y") << qrtVariantPair("ch_key","") << qrtVariantPair("vch_arg4","T"));
    query.execInsertDuplicateUpdate("cey_bt_paymode", "ch_paymodeno", qrtVariantPairList() << qrtVariantPair("ch_paymodeno","09") << qrtVariantPair("vch_paymodename","美团") << qrtVariantPair("ch_IDflag","N") << qrtVariantPair("ch_faceflag","N") << qrtVariantPair("ch_changeflag","N") << qrtVariantPair("ch_incomeflag","Y") << qrtVariantPair("ch_key","") << qrtVariantPair("vch_arg4","T"));
    query.execInsertDuplicateUpdate("cey_bt_paymode", "ch_paymodeno", qrtVariantPairList() << qrtVariantPair("ch_paymodeno","10") << qrtVariantPair("vch_paymodename","云会员卡") << qrtVariantPair("ch_IDflag","N") << qrtVariantPair("ch_faceflag","N") << qrtVariantPair("ch_changeflag","N") << qrtVariantPair("ch_incomeflag","Y") << qrtVariantPair("ch_key","") << qrtVariantPair("vch_arg4","N"));
    query.execInsertDuplicateUpdate("cey_bt_paymode", "ch_paymodeno", qrtVariantPairList() << qrtVariantPair("ch_paymodeno","11") << qrtVariantPair("vch_paymodename","会员挂账") << qrtVariantPair("ch_IDflag","N") << qrtVariantPair("ch_faceflag","N") << qrtVariantPair("ch_changeflag","N") << qrtVariantPair("ch_incomeflag","N") << qrtVariantPair("ch_key","") << qrtVariantPair("vch_arg4","N"));
    query.execInsertDuplicateUpdate("cey_bt_paymode", "ch_paymodeno", qrtVariantPairList() << qrtVariantPair("ch_paymodeno","AA") << qrtVariantPair("vch_paymodename","摇钱树支付") << qrtVariantPair("ch_IDflag","N") << qrtVariantPair("ch_faceflag","N") << qrtVariantPair("ch_changeflag","N") << qrtVariantPair("ch_incomeflag","Y") << qrtVariantPair("ch_key","") << qrtVariantPair("vch_arg4","N"));
    query.execInsertDuplicateUpdate("cey_bt_paymode", "ch_paymodeno", qrtVariantPairList() << qrtVariantPair("ch_paymodeno","CC") << qrtVariantPair("vch_paymodename","龙支付") << qrtVariantPair("ch_IDflag","N") << qrtVariantPair("ch_faceflag","N") << qrtVariantPair("ch_changeflag","N") << qrtVariantPair("ch_incomeflag","Y") << qrtVariantPair("ch_key","") << qrtVariantPair("vch_arg4","N"));
    query.execInsertDuplicateUpdate("cey_bt_paymode", "ch_paymodeno", qrtVariantPairList() << qrtVariantPair("ch_paymodeno","DD") << qrtVariantPair("vch_paymodename","微支付") << qrtVariantPair("ch_IDflag","N") << qrtVariantPair("ch_faceflag","N") << qrtVariantPair("ch_changeflag","N") << qrtVariantPair("ch_incomeflag","Y") << qrtVariantPair("ch_key","") << qrtVariantPair("vch_arg4","N"));
    query.execInsertDuplicateUpdate("cey_bt_paymode", "ch_paymodeno", qrtVariantPairList() << qrtVariantPair("ch_paymodeno","YY") << qrtVariantPair("vch_paymodename","自动抹零") << qrtVariantPair("ch_IDflag","N") << qrtVariantPair("ch_faceflag","N") << qrtVariantPair("ch_changeflag","N") << qrtVariantPair("ch_incomeflag","N") << qrtVariantPair("ch_key"," ") << qrtVariantPair("vch_arg4","N"));
    query.execInsertDuplicateUpdate("cey_bt_paymode", "ch_paymodeno", qrtVariantPairList() << qrtVariantPair("ch_paymodeno","ZZ") << qrtVariantPair("vch_paymodename","优惠") << qrtVariantPair("ch_IDflag","N") << qrtVariantPair("ch_faceflag","N") << qrtVariantPair("ch_changeflag","N") << qrtVariantPair("ch_incomeflag","N") << qrtVariantPair("ch_key"," ") << qrtVariantPair("vch_arg4","N"));

    lds_query_hddpos::com_mit();

    return true;
}

bool w_sys_manage_cloudsync_with::upload_base_table_while(lds_messagebox_loading_show *loading, QNetworkAccessManager &m, QEventLoop &loop, const QString &table_name, const QString &table_name_yun, const QString &sql, int state, int count, QString &errstring)
{
    QNetworkRequest request;
    bool has_next = true;
    lds_query_hddpos  q;
    QByteArray readall;
    QNetworkReply *reply;

    request.setUrl(QString(http_www_yqspos_com +"/arm/dish-base/add-rest"));
    request.setRawHeader("terminalCode", lds::terminalCode.toLocal8Bit());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");
    q.execSelect(sql);
    while(has_next) {
        cJSON_help json(upload_base_table(table_name, table_name_yun, q, state, count, has_next));

        loading->setText(QObject::tr("基础资料") + ":" +table_name + ":" + processMapValue(table_name, state));

        qDebug() << __LINE__ << "post";
        qDebug() << request.url();
        qDebug() << cJSON_help::getcJSONprint(json);
        qDebug() << lds::terminalCode;
        qDebug() << sql;

        reply = m.post(request, cJSON_help::getcJSONprint(json));
        loop.exec();
        readall = reply->readAll();
        if(200 != reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()) {
            errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
            qDebug() <<__LINE__ << errstring;
            return false;
        }
    }
    return true;
}

cJSON *w_sys_manage_cloudsync_with::upload_base_table(const QString &table_name, const QString &table_name_yun, lds_query_hddpos  &query, int state, int count, bool &has_next)
{
    //
    cJSON *json = upload_base_json_create(table_name_yun);
    //
    cJSON *item = cJSON_GetObjectItem(json, table_name_yun.toLocal8Bit());
    int c = upload_data_query(item, count, has_next, table_name, query,  state, w_sys_manage::q_get_piece_table);
    //
    debugProcessMap(table_name, state, c);
    return json;
}

cJSON *w_sys_manage_cloudsync_with::upload_base_json_create(const QString &table_name_yun)
{
    cJSON *json = cJSON_CreateObject();
    if(table_name_yun == "dishPracticeList")  cJSON_AddItemToObject(json, "dishPracticeList", cJSON_CreateArray());
    if(table_name_yun == "dishPracticeMemoList")  cJSON_AddItemToObject(json, "dishPracticeMemoList", cJSON_CreateArray());
    if(table_name_yun == "dishPracticeTypeList")  cJSON_AddItemToObject(json, "dishPracticeTypeList", cJSON_CreateArray());
    if(table_name_yun == "dishInfoList")  cJSON_AddItemToObject(json, "dishInfoList", cJSON_CreateArray());
    if(table_name_yun == "dishSubTypeList")  cJSON_AddItemToObject(json, "dishSubTypeList", cJSON_CreateArray());

    if(table_name_yun == "dishSuitList")  cJSON_AddItemToObject(json, "dishSuitList", cJSON_CreateArray());
    if(table_name_yun == "dishTypeList")  cJSON_AddItemToObject(json, "dishTypeList", cJSON_CreateArray());
    if(table_name_yun == "dishGuQingList")  cJSON_AddItemToObject(json, "dishGuQingList", cJSON_CreateArray());
    if(table_name_yun == "payModeList")  cJSON_AddItemToObject(json, "payModeList", cJSON_CreateArray());
    if(table_name_yun == "dishSuitChangeList")  cJSON_AddItemToObject(json, "dishSuitChangeList", cJSON_CreateArray());

    if(table_name_yun == "dishTableList")  cJSON_AddItemToObject(json, "dishTableList", cJSON_CreateArray());
    if(table_name_yun == "dishTableAreaList")  cJSON_AddItemToObject(json, "dishTableAreaList", cJSON_CreateArray());
    if(table_name_yun == "dishTableTypeList")  cJSON_AddItemToObject(json, "dishTableTypeList", cJSON_CreateArray());
    if(table_name_yun == "dishUnitList")  cJSON_AddItemToObject(json, "dishUnitList", cJSON_CreateArray());
    if(table_name_yun == "dishItemList")  cJSON_AddItemToObject(json, "dishItemList", cJSON_CreateArray());

    if(table_name_yun == "memberCuramountList")  cJSON_AddItemToObject(json, "memberCuramountList", cJSON_CreateArray());
    if(table_name_yun == "memberGiftList")  cJSON_AddItemToObject(json, "memberGiftList", cJSON_CreateArray());
    if(table_name_yun == "memberInfoList")  cJSON_AddItemToObject(json, "memberInfoList", cJSON_CreateArray());
    if(table_name_yun == "memberTypeList")  cJSON_AddItemToObject(json, "memberTypeList", cJSON_CreateArray());
    if(table_name_yun == "memberParameterList")  cJSON_AddItemToObject(json, "memberParameterList", cJSON_CreateArray());

    if(table_name_yun == "dishSysParameter")  cJSON_AddItemToObject(json, "dishSysParameter", cJSON_CreateObject());

    return json;
}

bool w_sys_manage_cloudsync_with::upload_material_table_while(lds_messagebox_loading_show *loading, QNetworkAccessManager &m, QEventLoop &loop, const QString &table_name, const QString &table_name_yun, const QString &sql, int state, int count, QString &errstring)
{
    QNetworkRequest request;
    bool has_next = true;
    lds_query_hddpos  q;
    QByteArray readall;
    QNetworkReply *reply;

    request.setUrl(QString(http_www_yqspos_com +"/arm/dish-base/update-rest"));
    request.setRawHeader("terminalCode", lds::terminalCode.toLocal8Bit());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");
    q.execSelect(sql);
    while(has_next) {
        cJSON_help  json(upload_material_table(table_name, table_name_yun, q, state, count, has_next));

        loading->setText(QObject::tr("库存") + ":" +table_name + ":" + processMapValue(table_name, state));
        qDebug() << __LINE__ << "post";
        qDebug() << request.url();
        reply = m.post(request, cJSON_help::getcJSONprint(json));
        loop.exec();
        readall = reply->readAll();
        if(200 != reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()) {
            errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
            return false;
        }
    }
    return true;
}

cJSON *w_sys_manage_cloudsync_with::upload_material_table(const QString &table_name, const QString &table_name_yun, lds_query_hddpos  &query, int state, int count, bool &has_next)
{
    cJSON *json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "dishInfoList", cJSON_CreateArray());

    //
    cJSON *item = cJSON_GetObjectItem(json, "dishInfoList");
    //
    int c = upload_data_query(item, count, has_next, table_name, query,  state, q_material_get_piece_table);
    //
    debugProcessMap(table_name, state, c);

    return json;
}

bool w_sys_manage_cloudsync_with::upload_promotion_table_while(lds_messagebox_loading_show *loading, QNetworkAccessManager &m, QEventLoop &loop, const QString &table_name, const QString &table_name_yun, const QString &sql, int state, int count, QString &errstring)
{
    QNetworkRequest request;
    lds_query_hddpos  q;
    QByteArray readall;
    QNetworkReply *reply;
    bool has_next = true;

    request.setUrl(QString(http_www_yqspos_com +"/arm/dish-base/update-rest"));
    request.setRawHeader("terminalCode", lds::terminalCode.toLocal8Bit());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");

    cJSON_help json(upload_promotion_table(table_name, table_name_yun, q, state, count, has_next));

    loading->setText(QObject::tr("促销") + ":" +table_name + ":" + processMapValue(table_name, state));
    qDebug() << __LINE__ << "post";qDebug() << request.url();
    reply = m.post(request, cJSON_help::getcJSONprint(json));
    loop.exec();
    readall = reply->readAll();
    if(200 != reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()) {
        errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
        return false;
    }
    return true;
}

cJSON *w_sys_manage_cloudsync_with::upload_promotion_table(const QString &table_name, const QString &table_name_yun, lds_query_hddpos  &query, int state, int count, bool &has_next)
{
    cJSON *json = upload_base_json_create(table_name_yun);
    cJSON *item = cJSON_GetObjectItem(json, table_name_yun.toLocal8Bit());
    int c = w_sys_manage::q_bt_get_piece_promotion(item);
    //
    debugProcessMap(table_name, state, c);

    return json;
}

bool w_sys_manage_cloudsync_with::upload_m_parameter_table_while(lds_messagebox_loading_show *loading, QNetworkAccessManager &m, QEventLoop &loop, const QString &table_name, const QString &table_name_yun, const QString &sql, int state, int count, QString &errstring)
{
    QNetworkRequest request;
    bool has_next = true;
    lds_query_hddpos  q;
    QByteArray readall;
    QNetworkReply *reply;

    request.setUrl(QString(http_www_yqspos_com +"/arm/dish-base/update-rest"));
    request.setRawHeader("terminalCode", lds::terminalCode.toLocal8Bit());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");
    q.execSelect(sql);
    while(has_next) {
        cJSON_help json(upload_m_parameter_table(table_name, table_name_yun, q, state, count, has_next));

        loading->setText(QObject::tr("会员参数") + ":" +table_name + ":" + processMapValue(table_name, state));
        qDebug() << __LINE__ << "post";qDebug() << request.url();
        reply = m.post(request, cJSON_help::getcJSONprint(json));
        loop.exec();
        readall = reply->readAll();
        if(200 != reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()) {
            errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
            return false;
        }
    }
    return true;
}

cJSON *w_sys_manage_cloudsync_with::upload_m_parameter_table(const QString &table_name, const QString &table_name_yun, lds_query_hddpos  &query, int state, int count, bool &has_next)
{
    cJSON *json = cJSON_CreateArray();
    cJSON_AddItemToObject(json, "dishInfoList", cJSON_CreateArray());

    //
    cJSON *item = cJSON_GetObjectItem(json, "dishInfoList");
    //
    int c = upload_data_query(item, count, has_next, table_name, query,  state, q_material_get_piece_table);
    //
    debugProcessMap(table_name, state, c);

    return json;
}

int w_sys_manage_cloudsync_with::upload_data_query(cJSON *item, int count, bool &has_next, const QString &table_name, lds_query_hddpos  &query, int state, cJSON *(*func)(const QString &, lds_query_hddpos  &, const QString &, int))
{
    int cur = 0;
    while(count > 0 && query.next()) {
        cJSON_AddItemToArray(item, func(table_name, query, "", state));
        cur ++;
        count --;
    }
    has_next = query.next();
    query.previous();
    return cur;
}

cJSON *w_sys_manage_cloudsync_with::q_material_get_piece_table(const QString &table_name, lds_query_hddpos  &query, const QString &orgCode, int state)
{
    cJSON *item = cJSON_CreateObject();

    cJSON_help::_cJSON_AddValueToObject(item, "diChDishNo", query.recordValue("ch_materialno"));
    cJSON_help::_cJSON_AddValueToObject(item, "inventory", query.recordValue("num_num"));
    cJSON_help::_cJSON_AddValueToObject(item, "orgCode", orgCode);

    return item;
}

void w_sys_manage_cloudsync_with::debugProcessMap(const QString &table_name, int state, int add)
{
    QString table_name_state = table_name + ":" +QString::number(state);
    map.insert(table_name_state, map.value(table_name_state, 0) + add);
    qDebug() << "table_name_state:" << table_name << processMapValue(table_name, state);
}

QString w_sys_manage_cloudsync_with::processMapValue(const QString &table_name, int state)
{
    QString table_name_state = table_name + ":" +QString::number(state);
    return QString::number(map.value(table_name_state));
}
