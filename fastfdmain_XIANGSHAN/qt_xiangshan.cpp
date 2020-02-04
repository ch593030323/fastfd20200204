#include "qt_xiangshan.h"
#include <QtDebug>
const QByteArray qt_XIANGSHAN::tcp_request_type_HELLO           = "tcp_request_type_HELLO";
const QByteArray qt_XIANGSHAN::tcp_request_type_get_parameter   = "tcp_request_type_get_parameter";
const QByteArray qt_XIANGSHAN::tcp_request_type_set_all         = "tcp_request_type_set_fupingtcp_request_type_set_scaletcp_request_type_set_ccb";
const QByteArray qt_XIANGSHAN::tcp_request_type_set_fuping      = "tcp_request_type_set_fuping";
const QByteArray qt_XIANGSHAN::tcp_request_type_set_scale       = "tcp_request_type_set_scale";
const QByteArray qt_XIANGSHAN::tcp_request_type_set_ccb         = "tcp_request_type_set_ccb";
const QByteArray qt_XIANGSHAN::tcp_request_type_pic_transfer    = "tcp_request_type_pic_transfer";
const QByteArray qt_XIANGSHAN::tcp_request_type_zip_update      = "tcp_request_type_zip_update";
const QByteArray qt_XIANGSHAN::tcp_request_type_pic_list        = "tcp_request_type_pic_list";
const QByteArray qt_XIANGSHAN::tcp_request_type_pic_del         = "tcp_request_type_pic_del";
const QByteArray qt_XIANGSHAN::tcp_request_type_pic_shop_info   = "tcp_request_type_pic_shop_info";

const QByteArray qt_XIANGSHAN::tcp_request_state_OK             = "tcp_request_state_OK";
const QByteArray qt_XIANGSHAN::tcp_request_state_ERROR          = "tcp_request_state_ERROR";

const QString qt_XIANGSHAN::tcp_request_type_name(const QByteArray &type)
{
    if(type == tcp_request_type_HELLO) {
        return "hello";
    }
    if(type == tcp_request_type_get_parameter) {
        return QObject::tr("获取参数");
    }
    if(type == tcp_request_type_set_all) {
        return QObject::tr("设置所有参数");
    }
    if(type == tcp_request_type_set_fuping) {
        return QObject::tr("设置界面参数");
    }
    if(type == tcp_request_type_set_scale) {
        return QObject::tr("设置计价秤参数");
    }
    if(type == tcp_request_type_set_ccb) {
        return QObject::tr("设置龙支付参数");
    }
    if(type == tcp_request_type_pic_transfer) {
        return QObject::tr("传送图片");
    }
    if(type == tcp_request_type_zip_update) {
        return QObject::tr("升级包升级");
    }
    if(type == tcp_request_type_pic_list) {
        return QObject::tr("图片列表");
    }
    if(type == tcp_request_type_pic_del) {
        return QObject::tr("图片删除");
    }
    if(type == tcp_request_type_pic_shop_info) {
        return QObject::tr("店铺图片");
    }
    qDebug() << "No type matched to name";
    return "";
}

qt_XIANGSHAN::tcp_request_data qt_XIANGSHAN::tcp_request_output(const QByteArray &byte)
{
    tcp_request_data d;
    int end = 0;
    int begin = 0;
    end = byte.indexOf('\t', begin);
    d.type = byte.mid(begin, end - begin); begin = end + QByteArray("\t").length();
    end = byte.indexOf('\t', begin);
    d.state = byte.mid(begin, end - begin); begin = end + QByteArray("\t").length();
    end = byte.indexOf('\t', begin);
    d.msg = byte.mid(begin, end - begin); begin = end + QByteArray("\t").length();
    end = byte.indexOf('\t', begin);

    d.fuping_size = byte.mid(begin, end - begin); begin = end + QByteArray("\t").length();
    end = byte.indexOf('\t', begin);
    d.fuping_rotate = byte.mid(begin, end - begin); begin = end + QByteArray("\t").length();
    end = byte.indexOf('\t', begin);
    d.fuping_skin = byte.mid(begin, end - begin); begin = end + QByteArray("\t").length();
    end = byte.indexOf('\t', begin);
    d.scale_port = byte.mid(begin, end - begin); begin = end + QByteArray("\t").length();
    end = byte.indexOf('\t', begin);
    d.scale_bundrate = byte.mid(begin, end - begin); begin = end + QByteArray("\t").length();

    end = byte.indexOf('\t', begin);
    d.ccb_BRANCHID = byte.mid(begin, end - begin); begin = end + QByteArray("\t").length();
    end = byte.indexOf('\t', begin);
    d.ccb_MERCHANTID = byte.mid(begin, end - begin); begin = end + QByteArray("\t").length();
    end = byte.indexOf('\t', begin);
    d.ccb_POSID = byte.mid(begin, end - begin); begin = end + QByteArray("\t").length();
    end = byte.indexOf('\t', begin);
    d.ccb_PUB = byte.mid(begin, end - begin); begin = end + QByteArray("\t").length();
    end = byte.indexOf('\t', begin);
    d.ccb_QUPWD = byte.mid(begin, end - begin); begin = end + QByteArray("\t").length();
    end = byte.indexOf('\t', begin);
    d.shop_info = byte.mid(begin, end - begin); begin = end + QByteArray("\t").length();

    end = byte.indexOf('\t', begin);
    d.pic_info_list = byte.mid(begin, end - begin); begin = end + QByteArray("\t").length();
    end = byte.indexOf('\t', begin);
    d.pic_info_del_file_name = byte.mid(begin, end - begin); begin = end + QByteArray("\t").length();

    end = byte.indexOf('\t', begin);
    d.pic_name = byte.mid(begin, end - begin); begin = end + QByteArray("\t").length();
    end = byte.indexOf('\t', begin);
    d.pic_size = byte.mid(begin, end - begin).toLongLong(); begin = end + QByteArray("\t").length();
    end = byte.indexOf('\t', begin);
    d.pic_byte = byte.mid(begin);

    return d;
}

QByteArray qt_XIANGSHAN::tcp_request_input(const qt_XIANGSHAN::tcp_request_data &data)
{
    QByteArray r;
    r += data.type + "\t";
    r += data.state + "\t";
    r += data.msg + "\t";

    r += data.fuping_size + "\t";
    r += data.fuping_rotate + "\t";
    r += data.fuping_skin + "\t";
    r += data.scale_port + "\t";
    r += data.scale_bundrate + "\t";

    r += data.ccb_BRANCHID + "\t";
    r += data.ccb_MERCHANTID + "\t";
    r += data.ccb_POSID + "\t";
    r += data.ccb_PUB + "\t";
    r += data.ccb_QUPWD + "\t";

    r += data.shop_info + "\t";

    r += data.pic_info_list + "\t";
    r += data.pic_info_del_file_name + "\t";


    r += data.pic_name + "\t";
    r += QByteArray::number(data.pic_size) + "\t";
    r += data.pic_byte;

    return r;
}

void qt_XIANGSHAN::tcp_request_data_print(const qt_XIANGSHAN::tcp_request_data &data)
{
    qDebug() << "type:" << data.type;
    qDebug() << "state:" << data.state;
    qDebug() << "msg:" << data.msg;
    qDebug() << "fuping_size:" << data.fuping_size;
    qDebug() << "fuping_rotate:" << data.fuping_rotate;
    qDebug() << "fuping_skin:" << data.fuping_skin;
    qDebug() << "scale_port:" << data.scale_port;
    qDebug() << "scale_bundrate:" << data.scale_bundrate;

    qDebug() << "ccb_BRANCHID:" << data.ccb_BRANCHID;
    qDebug() << "ccb_MERCHANTID:" << data.ccb_MERCHANTID;
    qDebug() << "ccb_POSID:" << data.ccb_POSID;
    qDebug() << "ccb_PUB:" << data.ccb_PUB;
    qDebug() << "ccb_QUPWD:" << data.ccb_QUPWD;
    qDebug() << "shop_info:" << data.shop_info.data();

    qDebug() << "pic_info_list:" << data.pic_info_list;
    qDebug() << "pic_info_del_file_name:" << data.pic_info_del_file_name;
    qDebug() << "pic_name:" << data.pic_name;
    qDebug() << "pic_size:" << data.pic_size;
}

void qt_XIANGSHAN::tcp_request_test()
{
    qt_XIANGSHAN::tcp_request_data d;
    d.type = "type";
    d.state = "state";
    d.msg = "msg";

    d.fuping_size = "fuping_size";
    d.fuping_rotate = "fuping_rotate";
    d.fuping_skin = "fuping_skin";
    d.scale_port = "scale_port";
    d.scale_bundrate = "scale_bundrate";

    d.ccb_BRANCHID = "ccb_BRANCHID";
    d.ccb_MERCHANTID = "ccb_MERCHANTID";
    d.ccb_POSID = "ccb_POSID";
    d.ccb_PUB = "ccb_PUB";
    d.ccb_QUPWD = "ccb_QUPWD";

    d.shop_info = "shop_info";
    d.pic_info_list = "pic_info_list";
    d.pic_info_del_file_name = "pic_info_del_file_name";

    d.pic_name = "pic_name";
    d.pic_size = 1204;
    d.pic_byte = "pic_byte";//pix_byte必须保证在最后，可能会很大
    qt_XIANGSHAN::tcp_request_data_print(qt_XIANGSHAN::tcp_request_output(qt_XIANGSHAN::tcp_request_input(d)));
}


qt_XIANGSHAN::qt_XIANGSHAN(QObject *parent)
    : QObject(parent)
{

}
