#include "w_sys_manage_outer_pay_set_mt_smart_cate_up.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/rand.h>
#include <openssl/objects.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include "lds_model_sqltablemodel.h"
#include "lds_query_hddpos.h"

#define ORDER_TRANS_row_count 36
#define ORDER_TRANS_column_count 5

const QByteArray order_trans_pay_mode_model[ORDER_TRANS_row_count][ORDER_TRANS_column_count] = {
    {"00","现金","1","现金支付"},
    {"03","银行卡","2","银行卡记账"},
    {"","","3","美团团购验券"},
    {"","","4","大众点评验券"},
    {"","","5","优惠买单验券"},
    {"","","6","糯米团购券"},
    {"","","7","微信扫码枪","美团收银在线支付"},
    {"","","8","支付宝扫码枪","美团收银在线支付"},
    {"","","9","美团外卖"},
    {"","","10","饿了么外卖"},
    {"","","11","百度外卖"},
    {"","","12","会员卡记账"},
    {"","","13","一卡通"},
    {"","","14","免单"},
    {"09","美团","15","美团团购记账"},
    {"","","16","大众点评记账"},
    {"ZZ","优惠","17","优惠买单记账"},
    {"05","微信支付","18","微信记账"},
    {"06","支付宝支付","19","支付宝记账"},
    {"02","充值卡","20","会员储值卡"},
    {"10","云会员卡","20","会员储值卡"},
    {"","","21","挂账"},
    {"","","22","银行卡刷卡"},
    {"","","23","Apple pay"},
    {"","","24","顾客微信自助扫码","美团收银在线支付"},
    {"","","25","顾客支付宝自助扫码","美团收银在线支付"},
    {"","","26","台卡支付","美团收银在线支付"},
    {"04","代金券","99","其他"},
    {"07","百度糯米","99","其他"},
    {"08","大众点评","99","其他"},
    {"11","会员挂账","99","其他"},
    {"AA","摇钱树支付","99","其他"},
    {"CC","龙支付","99","其他"},
    {"DD","微支付","99","其他"},
    {"YY","自动抹零","99","其他"},
    {"","","1000","自定义"}
};

QString w_sys_manage_outer_pay_set_mt_smart_cate_up::getcJSONString(cJSON *json, const char *string)
{
    cJSON *o = cJSON_GetObjectItem(json, string);
    if(o != 0)
        return o->valuestring;
    return "";
}

w_sys_manage_outer_pay_set_mt_smart_cate_up::w_sys_manage_outer_pay_set_mt_smart_cate_up(const QByteArray &signKey, const QByteArray &developerId, const QByteArray &appAuthToken, const QByteArray &ePoiId, const QByteArray &businessId, QObject *parent) :
    signKey(signKey),
    developerId(developerId),
    appAuthToken(appAuthToken),
    ePoiId(ePoiId),
    businessId(businessId),
    QObject(parent)
{
}

QByteArray w_sys_manage_outer_pay_set_mt_smart_cate_up::order_trans(const QString &ch_payno)
{
    QByteArray payTypeCode;
    QByteArray payTypeName;
    lds_query_hddpos  query;
    cJSON_object orderTrans;
    cJSON *transList =cJSON_CreateArray();
    cJSON_AddItemToObject(orderTrans.json, "orderId", cJSON_CreateString(ch_payno.toLocal8Bit()));
    cJSON_AddItemToObject(orderTrans.json, "transList", transList);

    query.execSelect(QString("select int_flowid, ch_paymodeno, dt_operdate, num_realamount - num_change as payamount from cey_u_checkout_detail where ch_payno = '%1' ").arg(ch_payno));
    while(query.next()) {
        order_trans_payCode(query.recordValue("ch_paymodeno").toByteArray(), payTypeCode, payTypeName);

        cJSON *item =cJSON_CreateObject();
        cJSON_AddItemToArray(transList, item);

        cJSON_AddItemToObject(item, "transId", cJSON_CreateString(query.recordValue("int_flowid").toByteArray()));
        cJSON_AddItemToObject(item, "payAmount", cJSON_CreateNumber(query.recordValue("payamount").toDouble()));
        cJSON_AddItemToObject(item, "payTypeCode", cJSON_CreateString(payTypeCode));
        cJSON_AddItemToObject(item, "payTypeName", cJSON_CreateString(payTypeName));
        cJSON_AddItemToObject(item, "payTime", cJSON_CreateNumber(query.recordValue("dt_operdate").toDateTime().toMSecsSinceEpoch()));
    }
    //
    QMap<QByteArray, QByteArray> map;
    map.insert("appAuthToken", appAuthToken);
    map.insert("charset", "UTF-8");
    map.insert("timestamp", QByteArray::number(QDateTime::currentMSecsSinceEpoch()));
    map.insert("businessId", businessId);//1团购、2外卖 businessId暂时只支持1或3（商家绑定团购或闪惠)
    map.insert("sign", "");
    map.insert("orderTrans", cJSON_Print(orderTrans.json));

    mapPrint(map);

    return QByteArray("http://api.open.cater.meituan.com/restaurant/order/trans") + "?" + create_url_path(map);
}

QByteArray w_sys_manage_outer_pay_set_mt_smart_cate_up::menu_category()
{
    cJSON_array param;
    {
        cJSON *item =cJSON_CreateObject();
        cJSON_AddItemToArray(param.json, item);

        cJSON_AddItemToObject(item, "cateId", cJSON_CreateString("rc"));
        cJSON_AddItemToObject(item, "name", cJSON_CreateString("热菜"));
        cJSON_AddItemToObject(item, "rank", cJSON_CreateString("1"));
        cJSON_AddItemToObject(item, "upperCateId", cJSON_CreateString("cx"));
    }
    //
    QMap<QByteArray, QByteArray> map;
    map.insert("appAuthToken", appAuthToken);
    map.insert("charset", "UTF-8");
    map.insert("timestamp", QByteArray::number(QDateTime::currentMSecsSinceEpoch()));
    map.insert("sign", "");
    map.insert("param", cJSON_Print(param.json));
    mapPrint(map);

    return QByteArray("http://api.open.cater.meituan.com/restaurant/menu/category") + "?" + create_url_path(map);

}

QByteArray w_sys_manage_outer_pay_set_mt_smart_cate_up::menu_dish()
{
    cJSON_array param;
    {
        {
            cJSON *item =cJSON_CreateObject();
            cJSON_AddItemToArray(param.json, item);
            cJSON_AddItemToObject(item, "spuId", cJSON_CreateString("001"));
            cJSON_AddItemToObject(item, "name", cJSON_CreateString("土豆"));
            cJSON_AddItemToObject(item, "engName", cJSON_CreateString("potato floss"));
            cJSON_AddItemToObject(item, "desc", cJSON_CreateString("土豆丝描述"));
            cJSON_AddItemToObject(item, "cateId", cJSON_CreateString("1"));
            cJSON_AddItemToObject(item, "rank", cJSON_CreateString("1"));

            cJSON *labelList =cJSON_CreateArray();
            cJSON_AddItemToObject(item, "labelList", labelList);
            {
                cJSON *item =cJSON_CreateObject();
                cJSON_AddItemToArray(labelList, item);
                cJSON_AddItemToObject(item, "labelId", cJSON_CreateString("ladu"));
                cJSON_AddItemToObject(item, "labelValue", cJSON_CreateString("3"));
            }


            cJSON_AddItemToObject(item, "valid", cJSON_CreateString("1"));

            cJSON *dishSkuList =cJSON_CreateArray();
            cJSON_AddItemToObject(item, "dishSkuList", dishSkuList);
            {
                cJSON *item =cJSON_CreateObject();
                cJSON_AddItemToArray(dishSkuList, item);
                cJSON_AddItemToObject(item, "skuId", cJSON_CreateString("A001"));
                cJSON_AddItemToObject(item, "unit", cJSON_CreateString("碗"));
                cJSON_AddItemToObject(item, "spec", cJSON_CreateString("大份"));
                cJSON_AddItemToObject(item, "cookMethod", cJSON_CreateString("清蒸"));
                cJSON_AddItemToObject(item, "price", cJSON_CreateString("3200"));
                cJSON_AddItemToObject(item, "memberPrice", cJSON_CreateString("2600"));

                item =cJSON_CreateObject();
                cJSON_AddItemToArray(dishSkuList, item);
                cJSON_AddItemToObject(item, "skuId", cJSON_CreateString("A002"));
                cJSON_AddItemToObject(item, "unit", cJSON_CreateString("碗"));
                cJSON_AddItemToObject(item, "spec", cJSON_CreateString("小份"));
                cJSON_AddItemToObject(item, "cookMethod", cJSON_CreateString("红烧"));
                cJSON_AddItemToObject(item, "price", cJSON_CreateString("3000"));
                cJSON_AddItemToObject(item, "memberPrice", cJSON_CreateString("2400"));
            }
        }
    }
    //
    QMap<QByteArray, QByteArray> map;
    map.insert("appAuthToken", appAuthToken);
    map.insert("charset", "UTF-8");
    map.insert("timestamp", QByteArray::number(QDateTime::currentMSecsSinceEpoch()));
    map.insert("sign", "");
    map.insert("param", cJSON_Print(param.json));
    qDebug() << map.value("param").data();

    return QByteArray("http://api.open.cater.meituan.com/restaurant/menu/dish") + "?" + create_url_path(map);

}

QByteArray w_sys_manage_outer_pay_set_mt_smart_cate_up::menu_combo()
{
    //param
    cJSON_array param;
    {
        //item
        cJSON *item =cJSON_CreateObject();
        cJSON_AddItemToArray(param.json, item);
        {
            cJSON_AddItemToObject(item, "comboId", cJSON_CreateString("001"));
            cJSON_AddItemToObject(item, "name", cJSON_CreateString("套餐A"));
            cJSON_AddItemToObject(item, "price", cJSON_CreateString("1200"));
            cJSON_AddItemToObject(item, "stock", cJSON_CreateString("1"));
            cJSON_AddItemToObject(item, "valid", cJSON_CreateString("1"));
            cJSON_AddItemToObject(item, "type", cJSON_CreateString("0"));


            //skuList
            cJSON *skuList =cJSON_CreateArray();
            cJSON_AddItemToObject(item, "skuList", skuList);
            {
                cJSON_AddItemToArray(skuList, cJSON_CreateString("001"));
                cJSON_AddItemToArray(skuList, cJSON_CreateString("002"));
                cJSON_AddItemToArray(skuList, cJSON_CreateString("003"));
            }//~skuList
        }//~item
    }//~param
    //
    QMap<QByteArray, QByteArray> map;
    map.insert("appAuthToken", appAuthToken);
    map.insert("charset", "UTF-8");
    map.insert("timestamp", QByteArray::number(QDateTime::currentMSecsSinceEpoch()));
    map.insert("sign", "");
    map.insert("param", cJSON_Print(param.json));
    mapPrint(map);

    return QByteArray("http://api.open.cater.meituan.com/restaurant/menu/combo") + "?" + create_url_path(map);

}

QByteArray w_sys_manage_outer_pay_set_mt_smart_cate_up::menu_dishimage(const QString &spuId)
{
    QMap<QByteArray, QByteArray> map;
    map.insert("appAuthToken", appAuthToken);
    map.insert("charset", "UTF-8");
    map.insert("timestamp", QByteArray::number(QDateTime::currentMSecsSinceEpoch()));
    map.insert("sign", "");
    map.insert("spuId", spuId.toLocal8Bit());
    mapPrint(map);

    return QByteArray("http://api.open.cater.meituan.com/restaurant/menu/dishimage") + "?" + create_url_path(map);

}

QByteArray w_sys_manage_outer_pay_set_mt_smart_cate_up::menu_order()
{
    //orderInfo
    cJSON_object orderInfo;
    {
        cJSON_AddItemToObject(orderInfo.json, "orderId", cJSON_CreateString("24345676543"));
        cJSON_AddItemToObject(orderInfo.json, "orderCreateTime", cJSON_CreateString("1528707768611"));
        cJSON_AddItemToObject(orderInfo.json, "orderSettlePrice", cJSON_CreateString("8900"));
        cJSON_AddItemToObject(orderInfo.json, "peopleCount", cJSON_CreateString("2"));
        cJSON_AddItemToObject(orderInfo.json, "orderRemark", cJSON_CreateString("黄焖鸡很好吃"));

        //comboList
        cJSON *comboList =cJSON_CreateArray();
        cJSON_AddItemToObject(orderInfo.json, "comboList", comboList);
        {
            {
                cJSON *item =cJSON_CreateObject();
                cJSON_AddItemToArray(comboList, item);
                cJSON_AddItemToObject(item, "comboId", cJSON_CreateString("001"));
                cJSON_AddItemToObject(item, "comboName", cJSON_CreateString("黄焖鸡套餐"));
                cJSON_AddItemToObject(item, "comboPrice", cJSON_CreateString("3000"));
                cJSON_AddItemToObject(item, "comboCount", cJSON_CreateString("1"));

                cJSON *skuIdList =cJSON_CreateArray();
                cJSON_AddItemToObject(item, "skuIdList", skuIdList);
                {
                    cJSON_AddItemToArray(skuIdList, cJSON_CreateString("1001"));
                    cJSON_AddItemToArray(skuIdList, cJSON_CreateString("1002"));
                    cJSON_AddItemToArray(skuIdList, cJSON_CreateString("1003"));
                }

                cJSON_AddItemToObject(item, "comboRemark", cJSON_CreateString(""));
            }
            {
                cJSON *item =cJSON_CreateObject();
                cJSON_AddItemToArray(comboList, item);
                cJSON_AddItemToObject(item, "comboId", cJSON_CreateString("002"));
                cJSON_AddItemToObject(item, "comboName", cJSON_CreateString("XXX套餐"));
                cJSON_AddItemToObject(item, "comboPrice", cJSON_CreateString("5000"));
                cJSON_AddItemToObject(item, "comboCount", cJSON_CreateString("1"));

                cJSON *skuIdList =cJSON_CreateArray();
                cJSON_AddItemToObject(item, "skuIdList", skuIdList);
                {
                    cJSON_AddItemToArray(skuIdList, cJSON_CreateString("1009"));
                }

                cJSON_AddItemToObject(item, "comboRemark", cJSON_CreateString(""));
            }
        }//~comboList

        //dishList
        cJSON *dishList =cJSON_CreateArray();
        cJSON_AddItemToObject(orderInfo.json, "dishList", dishList);
        {
            {
                cJSON *item =cJSON_CreateObject();
                cJSON_AddItemToArray(dishList, item);
                cJSON_AddItemToObject(item, "skuId", cJSON_CreateString("1004"));
                cJSON_AddItemToObject(item, "spuId", cJSON_CreateString("5002"));
                cJSON_AddItemToObject(item, "dishName", cJSON_CreateString("小鸡炖蘑菇"));
                cJSON_AddItemToObject(item, "skuCount", cJSON_CreateString("1"));
                cJSON_AddItemToObject(item, "skuPrice", cJSON_CreateString("5900"));
                cJSON_AddItemToObject(item, "dishRemark", cJSON_CreateString(""));
            }
            {
                cJSON *item =cJSON_CreateObject();
                cJSON_AddItemToArray(dishList, item);
                cJSON_AddItemToObject(item, "skuId", cJSON_CreateString("1005"));
                cJSON_AddItemToObject(item, "spuId", cJSON_CreateString("5003"));
                cJSON_AddItemToObject(item, "dishName", cJSON_CreateString("烤鱼"));
                cJSON_AddItemToObject(item, "skuCount", cJSON_CreateString("3.5"));
                cJSON_AddItemToObject(item, "skuPrice", cJSON_CreateString("9900"));
                cJSON_AddItemToObject(item, "dishRemark", cJSON_CreateString(""));
            }
        }//~dishList
    }//~orderInfo

    QMap<QByteArray, QByteArray> map;
    map.insert("appAuthToken", appAuthToken);
    map.insert("charset", "UTF-8");
    map.insert("timestamp", QByteArray::number(QDateTime::currentMSecsSinceEpoch()));
    map.insert("sign", "");
    map.insert("orderInfo", cJSON_Print(orderInfo.json));
    mapPrint(map);

    return QByteArray("http://api.open.cater.meituan.com/restaurant/menu/order") + "?" + create_url_path(map);

}

QByteArray w_sys_manage_outer_pay_set_mt_smart_cate_up::table_poi_heartbeat()
{
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();

    //param
    cJSON_array param;
    {
        {
            //item
            cJSON *item = cJSON_CreateObject();
            cJSON_AddItemToArray(param.json, item);
            cJSON_AddItemToObject(item, "ePoiId", cJSON_CreateString("0000231"));

            cJSON *equipmentList = cJSON_CreateArray();
            cJSON_AddItemToObject(item, "equipmentList", equipmentList);
            {
                {
                    cJSON *item = cJSON_CreateObject();
                    cJSON_AddItemToArray(equipmentList, item);
                    cJSON_AddItemToObject(item, "equipmentId", cJSON_CreateString("001"));
                    cJSON_AddItemToObject(item, "heartbeatTime", cJSON_CreateNumber(timestamp));
                }
            }
        }

        {
            //item
            cJSON *item = cJSON_CreateObject();
            cJSON_AddItemToArray(param.json, item);
            cJSON_AddItemToObject(item, "ePoiId", cJSON_CreateString("0000232"));

            cJSON *equipmentList = cJSON_CreateArray();
            cJSON_AddItemToObject(item, "equipmentList", equipmentList);
            {
                {
                    cJSON *item = cJSON_CreateObject();
                    cJSON_AddItemToArray(equipmentList, item);
                    cJSON_AddItemToObject(item, "equipmentId", cJSON_CreateString("001"));
                    cJSON_AddItemToObject(item, "heartbeatTime", cJSON_CreateNumber(timestamp));
                }
            }
        }
    }//~param
    //
    QMap<QByteArray, QByteArray> map;
    map.insert("appAuthToken", appAuthToken);
    map.insert("charset", "UTF-8");
    map.insert("timestamp", QByteArray::number(QDateTime::currentMSecsSinceEpoch()));
    map.insert("sign", "");
    map.insert("developerId", developerId);
    map.insert("param", cJSON_Print(param.json));
    mapPrint(map);

    return QByteArray("http://heartbeat.meituan.com/table/poi/heartbeat") + "?" + create_url_path(map);

}

QByteArray w_sys_manage_outer_pay_set_mt_smart_cate_up::table_base_info()
{
    cJSON_array param;
    {
        {
            cJSON *item =cJSON_CreateObject();
            cJSON_AddItemToArray(param.json, item);

            cJSON_AddItemToObject(item, "tableCode", cJSON_CreateString("001"));
            cJSON_AddItemToObject(item, "tableName", cJSON_CreateString("美团外卖1台"));
            cJSON_AddItemToObject(item, "areaDesc", cJSON_CreateString("靠窗"));
            cJSON_AddItemToObject(item, "typeDesc", cJSON_CreateString("普通包间"));
            cJSON_AddItemToObject(item, "minPeople", cJSON_CreateString("4"));
            cJSON_AddItemToObject(item, "maxPeople", cJSON_CreateString("8"));
            cJSON_AddItemToObject(item, "labelName", cJSON_CreateString("宝宝座"));
            cJSON_AddItemToObject(item, "roomCost", cJSON_CreateString("80"));
            cJSON_AddItemToObject(item, "fixedServiceCost", cJSON_CreateString("100"));
            cJSON_AddItemToObject(item, "ratioServiceCost", cJSON_CreateString("10.00"));
        }
        {
            cJSON *item =cJSON_CreateObject();
            cJSON_AddItemToArray(param.json, item);

            cJSON_AddItemToObject(item, "tableCode", cJSON_CreateString("002"));
            cJSON_AddItemToObject(item, "tableName", cJSON_CreateString("美团外卖2台"));
            cJSON_AddItemToObject(item, "areaDesc", cJSON_CreateString("靠窗"));
            cJSON_AddItemToObject(item, "typeDesc", cJSON_CreateString("普通包间"));
            cJSON_AddItemToObject(item, "minPeople", cJSON_CreateString("4"));
            cJSON_AddItemToObject(item, "maxPeople", cJSON_CreateString("8"));
            cJSON_AddItemToObject(item, "labelName", cJSON_CreateString("宝宝座"));
            cJSON_AddItemToObject(item, "roomCost", cJSON_CreateString("80"));
            cJSON_AddItemToObject(item, "fixedServiceCost", cJSON_CreateString("100"));
            cJSON_AddItemToObject(item, "ratioServiceCost", cJSON_CreateString("10.00"));
        }
    }
    //
    QMap<QByteArray, QByteArray> map;
    map.insert("appAuthToken", appAuthToken);
    map.insert("charset", "UTF-8");
    map.insert("timestamp", QByteArray::number(QDateTime::currentMSecsSinceEpoch()));
    map.insert("sign", "");
    map.insert("param", cJSON_Print(param.json));
    mapPrint(map);

    return QByteArray("http://api.open.cater.meituan.com/restaurant/table/base/info") + "?" + create_url_path(map);

}

QByteArray w_sys_manage_outer_pay_set_mt_smart_cate_up::table_dynamic_info()
{
    cJSON_array param;
    {
        {
            cJSON *item =cJSON_CreateObject();
            cJSON_AddItemToArray(param.json, item);

            cJSON_AddItemToObject(item, "tableCode", cJSON_CreateString("001"));
            cJSON_AddItemToObject(item, "useStatus", cJSON_CreateString("0"));
            cJSON_AddItemToObject(item, "subUseStatus", cJSON_CreateString(""));
        }
        {
            cJSON *item =cJSON_CreateObject();
            cJSON_AddItemToArray(param.json, item);

            cJSON_AddItemToObject(item, "tableCode", cJSON_CreateString("002"));
            cJSON_AddItemToObject(item, "useStatus", cJSON_CreateString("1"));
            cJSON_AddItemToObject(item, "subUseStatus", cJSON_CreateString("11"));        }
    }
    //
    QMap<QByteArray, QByteArray> map;
    map.insert("appAuthToken", appAuthToken);
    map.insert("charset", "UTF-8");
    map.insert("timestamp", QByteArray::number(QDateTime::currentMSecsSinceEpoch()));
    map.insert("sign", "");
    map.insert("param", cJSON_Print(param.json));
    mapPrint(map);

    return QByteArray("http://api.open.cater.meituan.com/restaurant/table/dynamic/info") + "?" + create_url_path(map);

}

QByteArray w_sys_manage_outer_pay_set_mt_smart_cate_up::delete_batchDelete()
{
    //    1	combo_id	删除指定套餐	删除整条套餐信息
    //    2	cate_id	删除指定分类	删除整条分类信息
    //    3	sku_id	删除指定规格	删除整条规格信息
    //    4	spu_id	删除指定商品	删除整条商品信息
    //    5	table_id	删除指定桌台	删除整条桌台信息
    //    6	spu_id	删除指定图片	仅删除该SPU下的图片信息

    cJSON_array delIds;
    {
        cJSON_AddItemToArray(delIds.json, cJSON_CreateString("61428442"));
        cJSON_AddItemToArray(delIds.json, cJSON_CreateString("56139881"));
        cJSON_AddItemToArray(delIds.json, cJSON_CreateString("56139860"));
        cJSON_AddItemToArray(delIds.json, cJSON_CreateString("56139885"));
        cJSON_AddItemToArray(delIds.json, cJSON_CreateString("56139917"));
        cJSON_AddItemToArray(delIds.json, cJSON_CreateString("56139947"));
        cJSON_AddItemToArray(delIds.json, cJSON_CreateString("56139935"));
        cJSON_AddItemToArray(delIds.json, cJSON_CreateString("56139848"));
    }
    //
    QMap<QByteArray, QByteArray> map;
    map.insert("appAuthToken", appAuthToken);
    map.insert("charset", "UTF-8");
    map.insert("timestamp", QByteArray::number(QDateTime::currentMSecsSinceEpoch()));
    map.insert("sign", "");
    map.insert("delIds", cJSON_Print(delIds.json));
    map.insert("delType", "4");
    mapPrint(map);

    return QByteArray("http://api.open.cater.meituan.com/restaurant/delete/batchDelete") + "?" + create_url_path(map);

}

QByteArray w_sys_manage_outer_pay_set_mt_smart_cate_up::create_url_path(const QMap<QByteArray, QByteArray> &map)
{
    /*
     * https://developer.meituan.com/openapi#3.2
     * 计算sha1签名时，需要以utf-8的编码转换byte流，否则可能导致含中文参数的签名计算不正确
     * 请求参数中有中文时，中文需要经过url编码，但计算签名时不需要
     */
    QByteArray prepare_url;
    QByteArray prepare_sign = signKey;
    for(QMap<QByteArray, QByteArray>::const_iterator k = map.begin(); k != map.end(); k++) {
        //sign不参与签名
        if(k.key() == ("sign"))
            continue;
#ifdef Q_OS_UNIX
        prepare_url += k.key() + "=" + k.value() + "&";
#endif
#ifdef Q_OS_WIN
        prepare_url += k.key().toPercentEncoding() + "=" + k.value() + "&";
#endif
        prepare_sign += k.key() + k.value();
    }

    QByteArray sigret;
    sigret.resize(20);//SHA1:20,SHA256:32
    SHA1((unsigned char*)prepare_sign.data(), prepare_sign.length(), (unsigned char*)sigret.data());
    sigret = sigret.toHex().toLower();

    return prepare_url + "sign=" + sigret;
}

void w_sys_manage_outer_pay_set_mt_smart_cate_up::mapPrint(const QMap<QByteArray, QByteArray> &map)
{
    for(QMap<QByteArray, QByteArray>::const_iterator k = map.begin(); k != map.end(); k++) {
        qDebug() << k.key() << ":" << k.value().data();
    }
}

void w_sys_manage_outer_pay_set_mt_smart_cate_up::order_trans_payCode(const QByteArray &ch_paymodeno, QByteArray &payTypeCode, QByteArray &payTypeName)
{
    for(int row = 0; row < ORDER_TRANS_row_count; row ++) {
        //0:paymodeno,1:paymodename,2:payTypeCode,3:payTypeName,4:payTypeExplain
        if(order_trans_pay_mode_model[row][0] == ch_paymodeno) {
            payTypeCode = order_trans_pay_mode_model[row][2];
            payTypeName = order_trans_pay_mode_model[row][3];
            break;
        }
    }
    if(payTypeCode.length() == 0) {
        payTypeCode = "99";//其他
        payTypeName = "其它";
    }
}

bool w_sys_manage_outer_pay_set_mt_smart_cate_up::post_image(const QString &url, const QString &name, const QString &image_path, QString &errstring)
{
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart imagePart;
    QFileInfo info(image_path);
    QFile *file = new QFile(info.filePath(), multiPart);
    QEventLoop loop;
    QNetworkAccessManager manager;
    QNetworkRequest request;
    QScopedPointer<QNetworkReply> reply;

    //imagePart
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/" + info.suffix()));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data;name=\"" + name + "\";filename=\"" + info.baseName() + "\""));
    // we cannot delete the file now, so delete it with the multiPart
    if(false == file->open(QIODevice::ReadOnly)) {
        errstring = file->errorString();
        return false;
    }
    imagePart.setBodyDevice(file);
    multiPart->append(imagePart);
    //~imagePart

    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    reply.reset(manager.post(request, multiPart));
    multiPart->setParent(reply.data());
    loop.exec();
    if(200 != reply.data()->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()) {
        errstring = reply.data()->errorString();
        return false;
    }
    cJSON_object obj(reply->readAll());

    if(0 != obj.json) {
        if(QString("ok") == getcJSONString(obj.json, "data")) {
            return true;
        }
        errstring = getcJSONString(obj.json, "message");
        return false;
    }
    errstring = "JSON IS NULL";
    return false;
}

bool w_sys_manage_outer_pay_set_mt_smart_cate_up::post_url(const QString &url, QString &errstring)
{
    QEventLoop loop;
    QNetworkAccessManager manager;
    QNetworkRequest request;
    QScopedPointer<QNetworkReply> reply;

    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    reply.reset(manager.post(request, ""));
    loop.exec();
    if(200 != reply.data()->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()) {
        errstring = reply.data()->errorString();
        return false;
    }
    cJSON_object obj(reply->readAll());
    qDebug() << cJSON_Print(obj.json);

    if(0 != obj.json) {
        if(QString("ok") == getcJSONString(obj.json, "data")) {
            return true;
        }
        errstring = getcJSONString(obj.json, "message");
        return false;
    }
    errstring = "JSON IS NULL";
    return false;
}
