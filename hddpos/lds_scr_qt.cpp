#include "lds_scr_qt.h"

lds_scr_qt::lds_scr_qt()
{
}

bool lds_scr_qt::material_insert(lds_query_hddpos  &query, const QString &ch_billno, int int_id)
{
    bool f = true;//默认是没有设置原料的
    //原料----点菜流水号
    query.execSelect(QString("SELECT int_flowid, num_num, num_back, ch_dishno from cey_u_orderdish where ch_billno = '%1' and int_id = %2 ")
                     .arg(ch_billno)
                     .arg(int_id));
    if(query.next()) {
        QString int_flowid = query.recordValue("int_flowid").toString();
        QString ch_dishno = query.recordValue("ch_dishno").toString();
        double num_num = query.recordValue("num_num").toDouble();
        double num_back = query.recordValue("num_back").toDouble();

        query.execSelect(QString("SELECT int_order_flowID from material_u_orderdish where int_order_flowID = %1 ")
                         .arg(int_flowid));
        if(query.next()) {
            f = query.execUpdate("material_u_orderdish",
                                 "num_num", qrtFunc(QString::number(num_num)+" * num_rate"),
                                 "num_back", qrtFunc(QString::number(num_back)+" * num_rate"),
                                 QString(" int_order_flowID = %1  ").arg(int_flowid));

        } else {

            f = query.execInsert("material_u_orderdish",
                                 QString("select %1 as `int_order_flowID`, "
                                         "ch_itemno as `ch_dishno`, "
                                         "num_num as `num_rate`, "
                                         "num_num * %2 as `num_num`, "
                                         "num_num * %3 as `num_back` from material_bt_dish where ch_dishno = '%4' ")
                                 .arg(int_flowid)
                                 .arg(num_num)
                                 .arg(num_back)
                                 .arg(ch_dishno)
                                 );
        }
    }

    return f;
}

bool lds_scr_qt::guqing_update(const QString &ch_dishno, double off)
{
    if(off == 0) return true;

    lds_query_hddpos  query;
    query.execSelect(QString("select ch_guqing_flag from cey_bt_dish where ch_dishno = '%1' ").arg(ch_dishno));
    if(query.next()) {
        QString ch_guqing_flag = query.recordValue("ch_guqing_flag").toString();
        if("Y" == ch_guqing_flag) {
            if(!query.execUpdate("cey_bt_dish", "num_guqing_num", qrtFunc("num_guqing_num - "+QString::number(off)), qrtEqual("ch_dishno", ch_dishno))) {
                return false;
            }
        }
    }
    return true;
}

void lds_scr_qt::doublescreensenddataByconf(const lds::DOUBLE_DATA &ddata)
{
    //不启用副屏、
    if(lds::sysconf->value("system_setting/fuping_mode", -1).toInt() == -1) {
        return;
    }
    //非关闭 && 不显示商品信息
    if(ddata.udpKey != lds::udp_close_request
            &&false == lds::sysconf->value("system_setting/dd_show_dish_list").toBool()) {
        return;
    }
    lds::doublescreensenddata(ddata);
}
