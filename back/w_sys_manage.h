#ifndef W_SYS_MANAGE_H
#define W_SYS_MANAGE_H

#include <QObject>
#include "lds_query_hddpos.h"

#include <QVariant>
#include <QStringList>
#include <QStandardItemModel>
#include <QtDebug>
#include "backheader.h"

class w_sys_manage : public QObject
{
    Q_OBJECT
public:
    explicit w_sys_manage(QObject *parent = 0);

    static QStringList get_flilename_path(const QString &path, const QStringList &filter=QStringList());//返回 一系列 文件名
    static QString set_flilename_path(const QString &flilename, const QString &path);//返回错入信息

    static cJSON *q_get_piece_table(const QString &table_name, lds_query_hddpos  &query, const QString &orgCode, int state);
    static void q_put_piece_table(const QString &table_name, qrtVariantPairList &key_value_list, cJSON *pItem);
    ///基础资料
    //1
    static cJSON *q_bt_get_piece_cey_bt_dish(lds_query_hddpos  &query, const QString &orgCode, int state);//-1删除、 0正常使用
    static void q_bt_put_piece_cey_bt_dish(qrtVariantPairList &key_value_list, cJSON *pItem);
    //2
    static cJSON *q_bt_get_piece_cey_bt_dish_subtype(lds_query_hddpos  &query, const QString &orgCode, int state);//-1删除、 0正常使用
    static void q_bt_put_piece_cey_bt_dish_subtype(qrtVariantPairList &key_value_list, cJSON *pItem);
    //3
    static cJSON *q_bt_get_piece_cey_bt_dish_type(lds_query_hddpos  &query, const QString &orgCode, int state);//-1删除、 0正常使用
    static void q_bt_put_piece_cey_bt_dish_type(qrtVariantPairList &key_value_list, cJSON *pItem);
    //4
    static cJSON *q_bt_get_piece_cey_bt_dish_suit(lds_query_hddpos  &query, const QString &orgCode, int state);//-1删除、 0正常使用
    static void q_bt_put_piece_cey_bt_dish_suit(qrtVariantPairList &key_value_list, cJSON *pItem);
    //5
    static cJSON *q_bt_get_piece_cey_bt_suit_change(lds_query_hddpos  &query, const QString &orgCode, int state);//-1删除、 0正常使用
    static void q_bt_put_piece_cey_bt_suit_change(qrtVariantPairList &key_value_list, cJSON *pItem);
    //6
    static cJSON *q_bt_get_piece_cey_bt_unit(lds_query_hddpos  &query, const QString &orgCode, int state);//-1删除、 0正常使用
    static void q_bt_put_piece_cey_bt_unit(qrtVariantPairList &key_value_list, cJSON *pItem);
    //7
    static cJSON *q_bt_get_piece_cey_bt_table(lds_query_hddpos  &query, const QString &orgCode, int state);//-1删除、 0正常使用
    static void q_bt_put_piece_cey_bt_table(qrtVariantPairList &key_value_list, cJSON *pItem);
    //8
    static cJSON *q_bt_get_piece_cey_bt_table_type(lds_query_hddpos  &query, const QString &orgCode, int state);//-1删除、 0正常使用
    static void q_bt_put_piece_cey_bt_table_type(qrtVariantPairList &key_value_list, cJSON *pItem);
    //9
    static cJSON *q_bt_get_piece_cey_bt_table_area(lds_query_hddpos  &query, const QString &orgCode, int state);//-1删除、 0正常使用
    static void q_bt_put_piece_cey_bt_table_area(qrtVariantPairList &key_value_list, cJSON *pItem);
    //10
    static cJSON *q_bt_get_piece_cey_bt_cp_dish(lds_query_hddpos  &query, const QString &orgCode, int state);//-1删除、 0正常使用
    static void q_bt_put_piece_cey_bt_cp_dish(qrtVariantPairList &key_value_list, cJSON *pItem);
    //11
    static cJSON *q_bt_get_piece_cey_bt_cp_memo(lds_query_hddpos  &query, const QString &orgCode, int state);//-1删除、 0正常使用
    static void q_bt_put_piece_cey_bt_cp_memo(qrtVariantPairList &key_value_list, cJSON *pItem);
    //12
    static cJSON *q_bt_get_piece_cey_bt_cp_type(lds_query_hddpos  &query, const QString &orgCode, int state);//-1删除、 0正常使用
    static void q_bt_put_piece_cey_bt_cp_type(qrtVariantPairList &key_value_list, cJSON *pItem);
    //13
    static cJSON *q_bt_get_piece_cey_bt_dish_warn(lds_query_hddpos  &query, const QString &orgCode, int state);//-1删除、 0正常使用
    static void q_bt_put_piece_cey_bt_dish_warn(qrtVariantPairList &key_value_list, cJSON *pItem);
    //14
    static cJSON *q_bt_get_piece_cey_bt_paymode(lds_query_hddpos  &query, const QString &orgCode, int state);//-1删除、 0正常使用
    static void q_bt_put_piece_cey_bt_paymode(qrtVariantPairList &key_value_list, cJSON *pItem);
    //15
    static cJSON *q_bt_get_piece_material_bt_dish(lds_query_hddpos  &query, const QString &orgCode, int state);//-1删除、 0正常使用
    static void q_bt_put_piece_material_bt_dish(qrtVariantPairList &key_value_list, cJSON *pItem);

    //16
    static cJSON *q_bt_get_piece_promotion();//-1删除、 0正常使用
    static int q_bt_get_piece_promotion(cJSON *item);//-1删除、 0正常使用
    static bool q_bt_put_piece_promotion(cJSON *pItem);

    ///会员
    //1
    static cJSON *q_m_get_piece_t_m_member_type(lds_query_hddpos  &query, const QString &orgCode, int state);//-1删除、 0正常使用
    static void q_m_put_piece_t_m_member_type(qrtVariantPairList &key_value_list, cJSON *pItem);
    //2
    static cJSON *q_m_get_piece_t_m_member(lds_query_hddpos  &query, const QString &orgCode, int state);//-1删除、 0正常使用
    static void q_m_put_piece_t_m_member(qrtVariantPairList &key_value_list, cJSON *pItem);
    //3
    static cJSON *q_m_get_piece_t_m_gift(lds_query_hddpos  &query, const QString &orgCode, int state);//-1删除、 0正常使用
    static void q_m_put_piece_t_m_gift(qrtVariantPairList &key_value_list, cJSON *pItem);
    //4

    static cJSON *q_m_get_piece_t_m_curamount(lds_query_hddpos  &query, const QString &orgCode, int state);//-1删除、 0正常使用
    static void q_m_put_piece_t_m_curamount(qrtVariantPairList &key_value_list, cJSON *pItem);
    //5
    static cJSON *q_m_get_piece_t_m_parameter(lds_query_hddpos  &query, const QString &orgCode);//-1删除、 0正常使用
    static void q_m_put_piece_t_m_parameter(qrtVariantPairList &key_value_list, cJSON *pItem);

};

#endif // W_SYS_MANAGE_H
