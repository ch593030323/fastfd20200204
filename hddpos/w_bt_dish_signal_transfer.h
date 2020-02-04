#ifndef W_BT_DISH_SIGNAL_TRANSFER_H
#define W_BT_DISH_SIGNAL_TRANSFER_H

#include <QObject>
#include<QSharedPointer>
#include<QVariantList>

class w_bt_dish_signal_transfer : public QObject
{
    Q_OBJECT
public:
    static QObject *wxpolling;
    static QObject *ttpolling;
    static QObject *kitchenpolling;
    static QObject *kitchen_print_polling;
    static QObject *yun_version_diff_polling;
    static QObject *yun_flow_upload_polling;
    static QObject *isoft_upload_polling;
    static QObject *fengze_upload_polling;
    static QObject *restful_upload_polling;
    static QObject *liaoning_inventory_keep_polling;
    static QObject *keepdata_time_basedata_polling;
    static QObject *qt_recipt_print_polling;
    static QObject *qt_display_print_polling;
    static QObject *tel_box_polling;
    static QObject *ogg_play_polling;
    static QObject *api_malaysia_hourly_polling;

    public:
    template<class T>
    static T *createObj(QObject *parent, QObject * & other) {
            T *obj = new T(parent);
            other = obj;
            return obj;
    }
    static void check();
public:
    explicit w_bt_dish_signal_transfer(QObject *parent = 0);
};

#endif // W_BT_DISH_SIGNAL_TRANSFER_H
