#ifndef QT_XIANGSHAN_H
#define QT_XIANGSHAN_H

#include <QObject>

class qt_XIANGSHAN : public QObject
{
    Q_OBJECT
public:
    static const QByteArray tcp_request_type_HELLO;
    static const QByteArray tcp_request_type_get_parameter;
    static const QByteArray tcp_request_type_set_all;
    static const QByteArray tcp_request_type_set_fuping;
    static const QByteArray tcp_request_type_set_scale;
    static const QByteArray tcp_request_type_set_ccb;
    static const QByteArray tcp_request_type_pic_transfer;
    static const QByteArray tcp_request_type_zip_update;
    static const QByteArray tcp_request_type_pic_list;
    static const QByteArray tcp_request_type_pic_del;
    static const QByteArray tcp_request_type_pic_shop_info;

    static const QString tcp_request_type_name(const QByteArray &type);

    static const QByteArray tcp_request_state_OK;
    static const QByteArray tcp_request_state_ERROR;
    struct tcp_request_data{
        QByteArray type;
        QByteArray state;
        QByteArray msg;

        QByteArray fuping_size;
        QByteArray fuping_rotate;
        QByteArray fuping_skin;
        QByteArray scale_port;
        QByteArray scale_bundrate;

        QByteArray ccb_BRANCHID;
        QByteArray ccb_MERCHANTID;
        QByteArray ccb_POSID;
        QByteArray ccb_PUB;
        QByteArray ccb_QUPWD;

        QByteArray shop_info;

        QByteArray pic_info_list;
        QByteArray pic_info_del_file_name;

        QByteArray pic_name;
        qlonglong  pic_size;
        QByteArray pic_byte;//pix_byte必须保证在最后，可能会很大
    };
    static tcp_request_data tcp_request_output(const QByteArray &byte);
    static QByteArray tcp_request_input(const tcp_request_data &data);
    static void tcp_request_data_print(const tcp_request_data &data);
    static void tcp_request_test();

public:
    qt_XIANGSHAN(QObject *parent = 0);
};

#endif // QT_XIANGSHAN_H
