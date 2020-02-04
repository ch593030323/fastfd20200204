#ifndef W_BT_DISH_BACK_H
#define W_BT_DISH_BACK_H

#include <QObject>
#include <QStringList>
#include <QDateTime>
#include "vch_print_memodata.h"

struct num_backData{
    qlonglong int_order_flow_id;
    mutable qlonglong int_quit_flow_id;
    float num_back_add;

    QString vch_back_operid;
    QDateTime dt_back_operdate;
    QString vch_reason;

    vch_print_memoData vch_print_memo;
    vch_print_memoData vch_print_memo_remain;
    void print() const;
    static QStringList int_quit_flow_id_list(const QList<num_backData> &list);
};

class w_bt_dish_back : public QObject
{
    Q_OBJECT
public:
    explicit w_bt_dish_back(QObject *parent = 0);

    static bool static_toquit_1_only(QString &errstring, const num_backData &d);
    static bool static_toquit_1_suit(QString &errstring,
                                     qlonglong int_flowid, double &dishnum_back_total, const QString &vch_reason,
                                     const QString &operid, const QDateTime &dt, QList<num_backData> &d_list);
    static bool static_toquit_1_suit_list(QString &errstring,
                                         qlonglong int_flowid, double &dishnum_back_total, const QString &vch_reason,
                                         const QString &operid, const QDateTime &dt, QList<num_backData> &d_list);

};

#endif // W_BT_DISH_BACK_H
