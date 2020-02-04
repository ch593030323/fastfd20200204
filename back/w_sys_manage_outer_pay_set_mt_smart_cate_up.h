#ifndef W_SYS_MANAGE_OUTER_PAY_SET_MT_SMART_CATE_UP_H
#define W_SYS_MANAGE_OUTER_PAY_SET_MT_SMART_CATE_UP_H

#include <QObject>
#include <QtNetwork>
#include <QNetworkRequest>
#include "cJSON.h"

class w_sys_manage_outer_pay_set_mt_smart_cate_up : public QObject
{
    Q_OBJECT
public:
    struct cJSON_object{
        cJSON_object():json(cJSON_CreateObject()) {}
        cJSON_object(const QByteArray &byte):json(cJSON_Parse(byte)) {}
        ~cJSON_object() {cJSON_Delete(json);}
        cJSON *json;
    };
    struct cJSON_array{
        cJSON_array():json(cJSON_CreateArray()) {}
        ~cJSON_array() {cJSON_Delete(json);}
        cJSON *json;
    };
    static QString getcJSONString(cJSON *json, const char *string);
public:
    explicit w_sys_manage_outer_pay_set_mt_smart_cate_up(
            const QByteArray &signKey,
            const QByteArray &developerId,
            const QByteArray &appAuthToken,
            const QByteArray &ePoiId,
            const QByteArray &businessId,
            QObject *parent = 0);

    //【1】http://api.open.cater.meituan.com/restaurant/order/trans
    QByteArray order_trans(const QString &ch_payno);

    //【2】http://api.open.cater.meituan.com/restaurant/menu/category
    QByteArray menu_category();

    //【3】http://api.open.cater.meituan.com/restaurant/menu/dish
    QByteArray menu_dish();

    //【4】http://api.open.cater.meituan.com/restaurant/menu/combo
    QByteArray menu_combo();

    //【5】http://api.open.cater.meituan.com/restaurant/menu/dishimage
    QByteArray menu_dishimage(const QString &spuId);

    //【6】http://api.open.cater.meituan.com/restaurant/menu/order
    QByteArray menu_order();

    //【7】http://heartbeat.meituan.com/table/poi/heartbeat
    QByteArray table_poi_heartbeat();

    //【8】http://api.open.cater.meituan.com/restaurant/table/base/info
    QByteArray table_base_info();

    //【9】http://api.open.cater.meituan.com/restaurant/table/dynamic/info
    QByteArray table_dynamic_info();

    //【10】http://api.open.cater.meituan.com/restaurant/delete/batchDelete
    QByteArray delete_batchDelete();

public:
    bool post_image(const QString &url, const QString &name, const QString &image_path, QString &errstring);
    bool post_url(const QString &url, QString &errstring);
private:
    QByteArray create_url_path(const QMap<QByteArray, QByteArray> &map);
    void mapPrint(const QMap<QByteArray, QByteArray> &map);
    void order_trans_payCode(const QByteArray &ch_paymodeno, QByteArray &payTypeCode, QByteArray &payTypeName);

private:
    const QByteArray signKey;
    const QByteArray developerId;
    const QByteArray appAuthToken;
    const QByteArray ePoiId;
    const QByteArray businessId;

};

#endif // W_SYS_MANAGE_OUTER_PAY_SET_MT_SMART_CATE_UP_H
