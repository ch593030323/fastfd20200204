#ifndef LDS_DD_H
#define LDS_DD_H

#include <QObject>
#include <QPair>

class lds_dd {
public:
    lds_dd() {}

     static const qint16 SYS_DOUBLE_SCREEN_PORT;
     static const qint16 SYS_WX_PORT;
     static const qint16 SYS_QRCODE_ORDER_PORT;

     static const QString udp_insert_request;
     static const QString udp_update_request;
     static const QString udp_delete_request;
     static const QString udp_delete_all_request;
     static const QString udp_change_skin_request;

     static const QString udp_title_request;
     static const QString udp_pay_request;
     static const QString udp_paying_request;
     static const QString udp_rb_code_request_begin;
     static const QString udp_rb_code_request_end;
     static const QString udp_close_request;
     static const QString udp_m_hlabels_request;
     static const QString udp_scale_init_request;
     static const QString udp_scale_update_request;
     static const QString udp_font_update_request;


    struct tableviewDATA{
        QString vch_dishname;//名称
        QString ch_dishno;//编码
        QString dish_price;//单价
        QString dish_num;//数量
        QString dish_total;//合计
        QString vch_other;//做法特价等其他描述
        QString ch_suitflag;//套菜标志
    };
    struct DOUBLE_DATA{
        // insert update delete
        //title
        //pay
        //rb_code
        //close
        //
        QString udpKey;

        //insert update delete
        int row;
        int count;
        tableviewDATA items;//dishno dishname dishprice dishnum dishtotal

        //title
        QString title_num;//{"name":"数量", "value":"12.6"}
        QString title_total;//{"name":"金额", "value":"12.6"}
//        QString sum_total_desc;//{"name":"金额", "value":"12.6"}

        //pay
        QString p_desc;//表头
        QString p_receive_desc;//应收
        QString p_receive;//应收
        QString p_pay_desc;//已付
        QString p_pay;//已付
        QString p_change_desc;//找零
        QString p_change;//找零

        //rb_code
        QString rb_title;
        QString rb_title_wechat;
        QString rb_title_alipay;
        QString rb_wechat;
        QString rb_alipay;
        QString rb_ch_paymodeno;//05只显示微信、26只显示支付宝

        //m_hlabels, splic ,
        QString m_hlabels;//tableview的表头，4.3吋下生效，只能四列，如【名称,价格,数量,小计】

        //parentPID
        qint64 parentPID;//主程序的pid

        //scale
        QString scale_keys;
        QString scale_titles;
        QString scale_suffixs;
        QString scale_numbers;
        //skin
        QString skin_name;
        //pay_master_json
        QString paying_desc;//表头
        QString paying_master;//{"no":"custume", "name":"消费", "value":"12.56"}
        QString paying_detail;//{"no":"00", "name":"现金", "value":"15.00"}
        //font_path
        QString font_path;
    };
    static void DOUBLE_DATA_input(QString &d, const DOUBLE_DATA &data);
    static void DOUBLE_DATA_output(const QString &d, DOUBLE_DATA &data);
    //客户端
    static void doublescreensenddata(const DOUBLE_DATA &ddata);//客户端
    static void doublescreensenddata(const QByteArray &d);//客户端
};

#endif // LDS_DD_H
