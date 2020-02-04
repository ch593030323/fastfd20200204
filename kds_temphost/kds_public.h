#ifndef KDS_PUBLIC_H
#define KDS_PUBLIC_H

#include <QObject>

class kds_public : public QObject
{
    Q_OBJECT
public:
    explicit kds_public(QObject *parent = 0);
    struct kds_order_item{
        QString no;
        QString name;
        double num;
        double price;
    };
    struct kds_order_dish{
        int  row;
        QString no;
        QString name;
        double num;
        QString type_no;
        QList<kds_order_item> item_list;
    };
    struct kds_order_content{
        QString bill_no;
        QString bill_serial;
        QString bill_time;
        QString bill_memo;
        QString table_no;
        QString table_name;
        QList<kds_order_dish> dish_list;
    };
    static QString kds_order_content2Json(const kds_order_content &d);
    //
    struct kds_return_item{
        QString no;
        double num;
    };
    struct kds_return_dish{
        int  row;
        double num;
        QList<kds_return_item> item_list;
    };
    struct kds_return_content{
        QString bill_no;
        QString bill_reason;
        QList<kds_return_dish> dish_list;
    };
    static QString kds_return_content2Json(const kds_return_content &d);

    static void setSysTextCodec();
    static QByteArray stylesheetData();
};

#endif // KDS_PUBLIC_H
