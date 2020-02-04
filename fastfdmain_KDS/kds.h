#ifndef KDS_H
#define KDS_H

#include <QObject>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSize>
#include <QVariant>
#include <QTranslator>
#include "lds_ogg_play.h"
#include"lds_const_var.h"
#include"w_bt_dish_flag.h"
#include"lds_model_sqltablemodel_m.h"
#include"lds_query_db.h"
#include <QSqlRecord>
#include <QPainter>
#include <QElapsedTimer>
class cJSON_path;
class kds : public QObject
{
    Q_OBJECT
public:
    struct billPlanDetail{
        QString bill_no;
        QString vch_plan_id;
        QStringList flow_id_list;
    };
public:
    kds(QObject *parent = 0);
    static QByteArray stylesheetData();
    static int func_dec_price();
    static int func_dec_num();
    static int func_dec_amt();

    static QString color_prepared;
    static QString color_prepared_warn;
    static QString color_cooking;
    static QString color_finished;
    static QString color_token;
    static QString color_cancel;
    static QString color_state(int state, const QDateTime &dt);

    static QString print_flag_cook;
    static QMap<QString, QTranslator*> translator_map;

    static QSize xstringToSize(const QString &sizestr, const QString &defSize);
    static QSize xstringToSize(const QString &sizestr, const QSize &defSize);
    static QString sizeToxstring(const QSize &size);

    static void recordFill(const QSqlRecord &record, qrtVariantPairList &d);
    static QVariant selectValue(const QString &sql);
    static QVariant selectValue(const QString &sql, const QString &arg0);
    static QVariant selectValue(const QString &sql, int arg0);
    static bool  selectHasNext(const QString &sql);
    static bool  selectHasNext(const QString &sql, const QString &arg0);
    static bool  selectHasNext(const QString &sql, int arg0);
    static void writeLog(const QString &message);
    static bool is_fastfdfood_kitchen();
    static bool is_fastfdfood_service();
    static bool is_restaurant_kitchen();
    static bool is_restaurant_service();

    static const QString log_file_path;

    static lds_ogg_play_thread *mediaPlayer;
    static QStringList numStringToOggName(const QString &num_string);

    static QString FONT_NUM_Family();

    static void setTranslator(const  QString &la);
    static QTranslator *getTranslator(QString la);

    static const QString kdsdd_request_setTitle;
    static const QString kdsdd_request_setCooking;
    static const QString kdsdd_request_setService;
    static const QString kdsdd_request_setCookingService;
    static const QString kdsdd_request_close;
    static const QString kdsdd_request_size;
    struct kdsddData{
        QString type;
        QStringList list_cooking;
        QStringList list_service;
        QString size;

        void print() const;
        static kdsddData create(const QString &type);
    };
    static void kdsdd_datagram_make(const kdsddData &dd, QByteArray &datagram);
    static void kdsdd_datagram_resolve(kdsddData &dd, const QByteArray &datagram);
    static void kdsdd_send(const kdsddData &dd);
    static void kdsdd_send_type(const QString &type);
    static void kdsdd_send_scree_size(const QString &size);
    static void kds_second_screen_read(QMap<QString, QString> &map);
    static void kds_second_screen_translate();


    static QDateTime currentDateTime();
    //
    static bool update_print_kt_dish(const QString & flow_id, int target_state, QString &errstring, bool *via_cook = 0);
    static bool updateTable(const QString &tablename, const qrtVariantPairList &key_value_list, const QString &where_sql, QString *errstring = 0);

    static bool pos_kdsdd_existed();
    static void pos_kdsdd_start();

    static bool softCanStart();
public:
    static QList<QRectF> paintTableBrief(QPainter *painter, const QByteArray &json, QRectF rect);
    static QList<QRectF> paintTableBrief(QPainter *painter, const QModelIndex &index, QRectF rect);
    static QPainterPath  sawtoothPath(QRectF r, int count);
    static bool paint_path_select_next(cJSON_path *p);

    static lds_model_sqltablemodel_m* model;
    static void kds_model_setdata(const QString &table_name, const QString &key, const QVariant &value);
    static QVariant kds_model_data(const QString &table_name, const QString &key, const QVariant &def = QVariant());

    static QString f_get_sysparm(const QString &as_parameter, const QString &def = QString());

    static QVariant cey_bt_kitchen_view_data(const QString &field_name);
    static bool cey_bt_kitchen_view_update(const QString &field_name, const QVariant &value);
    static bool cey_bt_kitchen_view_update_service(const QString &field_name, const QVariant &value);
    static void cey_bt_kitchen_view_clear();
    static void voice_play();
    static QElapsedTimer voice_play_timer;
    static double cey_u_orderdish_num_dish(const QString &flow_id, bool clear = false);

    static QString sql_plan_service_in();
    static QString sql_string_equal(int value);
    static QString sql_string_equal(const QString & value);
    static QString sql_plan_kitchen_equal();
public:
    static QFont fontPixelSize(const QFont &font, int pixel);
    static QFont fontScaled(const QFont &font, qreal scale);
    static QFont fontFamily(const QFont &font, const QString &family, qreal scale = 1.0);
    static QRectF rectPaintTime(const QRectF &rect, QPainter *painter);
    static int flowIdRight(const QRectF &rect);
    static int sawtoothRight(const QRectF &rect);

    static bool db_open(QSqlDatabase &db);
    static void db_close(QSqlDatabase &db);
    //
    static QByteArray btn_detail_json(const QString &bill_no, const QString &vch_plan_id, int less_than_state);
    static QByteArray btn_detail_json(const QString &bill_no, const QString &vch_plan_id, int less_than_state, int id);
    static QByteArray btn_detail_json(const QList<billPlanDetail> &bill_flow_id_list);
    //
    static void print_cook(const QString &flow_id);

private:
    static QString _FONT_NUM_Family;
    static QString cur_language;
};

#endif // KDS_H
