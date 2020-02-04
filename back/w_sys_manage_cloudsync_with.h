#ifndef W_SYS_MANAGE_CLOUDSYNC_WITH_H
#define W_SYS_MANAGE_CLOUDSYNC_WITH_H

#include <QObject>
#include <QMap>
#include "lds_query_hddpos.h"
#include "cJSON.h"
#include <QNetworkAccessManager>
#include <QEventLoop>
#include "lds_messagebox.h"

class w_sys_manage_cloudsync_with : public QObject
{
    Q_OBJECT
public:
    explicit w_sys_manage_cloudsync_with(QObject *parent = 0);

    static bool data_upload(lds_messagebox_loading_show *loading, bool skip_ch_yun_already_keep_Y, QString &errstring);
    static bool data_download(lds_messagebox_loading_show *loading,bool down_all, QString &errstring);

private:
    //
    //基础资料 x 19
    static bool upload_base_table_while(lds_messagebox_loading_show *loading, QNetworkAccessManager &m, QEventLoop &loop,
                                        const QString &table_name,  const QString &table_name_yun, const QString &sql, int state, int count,
                                        QString &errstring);
    static cJSON *upload_base_table(
            const QString &table_name, const QString &table_name_yun, lds_query_hddpos  &query,  int state, int count, bool &has_next);

    static cJSON *upload_base_json_create(const QString &table_name_yun);
    //库存 x 1
    static bool upload_material_table_while(lds_messagebox_loading_show *loading, QNetworkAccessManager &m, QEventLoop &loop,
                                            const QString &table_name,  const QString &table_name_yun, const QString &sql, int state, int count,
                                            QString &errstring);
    static cJSON *upload_material_table(
            const QString &table_name, const QString &table_name_yun, lds_query_hddpos  &query,  int state, int count, bool &has_next);

    //促销 x 1
    static bool upload_promotion_table_while(lds_messagebox_loading_show *loading, QNetworkAccessManager &m, QEventLoop &loop,
                                            const QString &table_name,  const QString &table_name_yun, const QString &sql, int state, int count,
                                            QString &errstring);
    static cJSON *upload_promotion_table(
            const QString &table_name, const QString &table_name_yun, lds_query_hddpos  &query,  int state, int count, bool &has_next);
    //会员参数
    static bool upload_m_parameter_table_while(lds_messagebox_loading_show *loading, QNetworkAccessManager &m, QEventLoop &loop,
                                               const QString &table_name,  const QString &table_name_yun, const QString &sql, int state, int count,
                                               QString &errstring);
    static cJSON *upload_m_parameter_table(
            const QString &table_name, const QString &table_name_yun, lds_query_hddpos  &query,  int state, int count, bool &has_next);
    //
    static int upload_data_query(cJSON *item, int count, bool &has_next,
                                 const QString &table_name, lds_query_hddpos  &query, int state,
                                 cJSON *(*func)(const QString &, lds_query_hddpos  &, const QString &, int ));
    //
    static cJSON *q_material_get_piece_table(const QString &table_name, lds_query_hddpos  &query, const QString &orgCode, int state);
    static void debugProcessMap(const QString &table_name, int state, int add);
    static QString processMapValue(const QString &table_name, int state);
    //
    static QMap<QString, int> map;
};

#endif // W_SYS_MANAGE_CLOUDSYNC_WITH_H
