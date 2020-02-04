#include "w_bt_dish_back.h"
#include <QtDebug>
#include "lds_query_hddpos.h"
#include "w_bt_dish.h"

void num_backData::print() const
{
#ifdef QT_DEBUG
    qDebug() << "int_quit_flow_id" << int_quit_flow_id;
    qDebug() << "int_order_flow_id" << int_order_flow_id;
    qDebug() << "num_back_add" << num_back_add;

    qDebug() << "vch_back_operid" << vch_back_operid;
    qDebug() << "dt_back_operdate" << dt_back_operdate;
    qDebug() << "vch_reason" << vch_reason;

    qDebug() << "vch_reason" << vch_print_memo.sqlValue();
    qDebug() << "vch_print_memo_remain" << vch_print_memo_remain.sqlValue();
#endif
}

QStringList num_backData::int_quit_flow_id_list(const QList<num_backData> &list)
{
    QStringList int_flow_id_list;
    foreach(const num_backData &d, list) {
        int_flow_id_list << QString::number(d.int_quit_flow_id);
    }
    return int_flow_id_list;
}

w_bt_dish_back::w_bt_dish_back(QObject *parent) :
    QObject(parent)
{
}

bool w_bt_dish_back::static_toquit_1_only(QString &errstring, const num_backData &d)
{
    lds_query_hddpos  query;
    QString ch_dishno;
    QString ch_suitflag;
    double num_price_add;
    double num_price_add_P;
    QString vch_plan_k3o3_id;
    QString state;
    query.execSelect(QString("select ch_dishno, ch_suitflag, num_price_add, vch_plan_k3o3_id  from cey_u_orderdish where int_flowid = '%1' ")
                     .arg(d.int_order_flow_id));
    query.next();
    ch_dishno = query.recordValue("ch_dishno").toString();
    ch_suitflag = query.recordValue("ch_suitflag").toString();
    vch_plan_k3o3_id = query.recordValue("vch_plan_k3o3_id").toString();

    num_price_add = d.vch_print_memo_remain.total();
    if(ch_suitflag == "Y") num_price_add = 0;
    if(ch_suitflag == "P") num_price_add_P = query.recordValue("num_price_add").toDouble();


    for(int k = 0; k < 6; k++) {
        if(vch_plan_k3o3_id.mid(k*2, 2) == "00") {
            state += "1";
        } else {
            state += w_bt_dish::k2o3_stae2hex_str(w_bt_dish::KITCHEN_QUIT);
        }
    }
    if(query.execUpdate("cey_u_orderdish", qrtVariantPairList()
                        << qrtVariantPair("num_back", qrtFunc("num_back + "+QString::number(d.num_back_add)))
                        << qrtVariantPair("vch_back_operid", d.vch_back_operid)
                        << qrtVariantPair("dt_back_operdate", d.dt_back_operdate)
                        << qrtVariantPair("vch_print_memo", d.vch_print_memo_remain.sqlValue())
                        << qrtVariantPair("num_price_add", num_price_add),
                        qrtEqual("int_flowid", d.int_order_flow_id))) {
        //更新log
        num_price_add = d.vch_print_memo.total();
        if(ch_suitflag == "Y")
            num_price_add = 0;
        if(ch_suitflag == "P")
            num_price_add = num_price_add_P;
        if(query.execInsert("cey_u_orderdish_log", qrtVariantPairList()
                            << qrtVariantPair("int_orderflow", d.int_order_flow_id)
                            << qrtVariantPair("ch_type", 1)
                            << qrtVariantPair("vch_operID", lds::gs_operid)
                            << qrtVariantPair("dt_operdate", d.dt_back_operdate)
                            << qrtVariantPair("vch_reason", d.vch_reason)

                            << qrtVariantPair("num_back_cur", d.num_back_add)
                            << qrtVariantPair("vch_plan_k3o3_state", state)
                            << qrtVariantPair("vch_print_memo", d.vch_print_memo.sqlValue(-1))
                            << qrtVariantPair("num_price_add", num_price_add))) {
            query.execSelect("SELECT LAST_INSERT_ID();");
            query.next();
            d.int_quit_flow_id = query.recordValue(0).toLongLong();

            query.execSelect(QString("SELECT num_num, num_back from cey_u_orderdish where int_flowid = %1 ")
                             .arg(d.int_order_flow_id));
            query.next();
            double num_num = query.recordValue("num_num").toDouble();
            double num_back = query.recordValue("num_back").toDouble();
            if(query.execUpdate("material_u_orderdish",
                                "num_num", qrtFunc(QString::number(num_num)+" * num_rate"),
                                "num_back", qrtFunc(QString::number(num_back)+" * num_rate"),
                                QString(" int_order_flowID = %1  ").arg(d.int_order_flow_id))) {
                return true;
            }
        }
    }
    errstring = query.recordError();
    return false;
}


//dishnum_back_total、d_map 同步更新
bool w_bt_dish_back::static_toquit_1_suit(QString &errstring, qlonglong int_flowid, double &dishnum_back_total, const QString &vch_reason, const QString &operid, const QDateTime &dt, QList<num_backData> &d_list)
{
    if(!static_toquit_1_suit_list(errstring, int_flowid, dishnum_back_total, vch_reason, operid, dt, d_list)) {
        return false;
    }
    for(int k = 0; k < d_list.count(); k ++) {
        if(!static_toquit_1_only(errstring, d_list[k])) {
            return false;
        }
    }
    return true;

    //        lds_query_hddpos  query;
    //        lds_query_hddpos  query2;
    //        QString ch_suitflag;
    //        QList<qlonglong> int_flowid_list;
    //        QString ch_dishno;
    //        QByteArray vch_print_memo;
    //        double back_num_P=dishnum_back_total;
    //        double dish_num;
    //        double back_num_P_org;

    //        if(dishnum_back_total <= 0) {
    //            errstring = QObject::tr("退货数量为0");
    //            goto failure;
    //        }
    //        query.execSelect(QString("select int_flowid, ch_suitflag, ch_dishno, num_num-num_back as dish_num from cey_u_orderdish where int_flowid = %1 ").arg(int_flowid));
    //        if(query.next()) {
    //            int_flowid_list.clear();
    //            int_flowid_list.append(query.recordValue("int_flowid").toLongLong());
    //            ch_suitflag = query.recordValue("ch_suitflag").toString();
    //            ch_dishno = query.recordValue("ch_suitflag").toString();
    //            back_num_P_org = query.recordValue("dish_num").toDouble();

    //            if(ch_suitflag == "P") {
    //                while(query2.execSelect(QString("select int_flowid, ch_suitflag from cey_u_orderdish where int_flowid = %1 + 1 and ch_suitflag = 'Y' ")
    //                                        .arg(int_flowid_list.last()))) {
    //                    if(query2.next()) {
    //                        int_flowid_list.append(query2.recordValue("int_flowid").toLongLong());
    //                    } else {
    //                        break;
    //                    }
    //                }
    //            }

    //            foreach(int id,  int_flowid_list) {
    //                query2.execSelect(QString("select int_flowid, num_num - num_back, ch_suitflag, ch_dishno, vch_print_memo from cey_u_orderdish where int_flowid = %1 ")
    //                                  .arg(id));
    //                query2.next();
    //                num_backData d;
    //                ch_suitflag = query2.recordValue("ch_suitflag").toString();
    //                vch_print_memo = query2.recordValue("vch_print_memo").toByteArray();
    //                dish_num = query2.recordValue(1).toDouble();

    //                d.int_flowid = id;
    //                d.num_back_add = dish_num;
    //                d.dt_back_operdate = dt;
    //                d.vch_back_operid = operid;
    //                d.vch_reason = vch_reason;
    //                d.vch_print_memo.select(vch_print_memo);//套菜全退

    //                //主商品或普通商品
    //                if(d.num_back_add <= dishnum_back_total) {
    //                } else {
    //                    d.num_back_add = dishnum_back_total;
    //                }
    //                //子套菜
    //                if(ch_suitflag == "Y") {
    //                    if(back_num_P_org == 0) {
    //                        errstring = QObject::tr("操作失败");
    //                        goto failure;
    //                    }
    //                    d.num_back_add = back_num_P/back_num_P_org * dish_num;
    //                    //                d.vch_print_memo = "";
    //                }


    //                if(d_list)
    //                    d_list->append(d);
    //                if(!fexpandmain_2::static_toquit_1_only(errstring, d)) {
    //                    goto failure;
    //                }

    //                //主商品或普通商品
    //                dishnum_back_total -= d.num_back_add;
    //                //子套菜
    //                if(ch_suitflag == "Y") {
    //                    dishnum_back_total += d.num_back_add;
    //                }
    //            }
    //            return true;
    //        }
    //        errstring = QObject::tr("商品没有找到");
    //failure:
    //        return false;
}

bool w_bt_dish_back::static_toquit_1_suit_list(QString &errstring, qlonglong int_flowid, double &dishnum_back_total, const QString &vch_reason, const QString &operid, const QDateTime &dt, QList<num_backData> &d_list)
{
    lds_query_hddpos  query;
    lds_query_hddpos  query2;
    QString ch_suitflag;
    QList<qlonglong> int_flowid_list;
    QString ch_dishno;
    QByteArray vch_print_memo;
    double back_num_P=dishnum_back_total;
    double dish_num;
    double back_num_P_org;
    d_list.clear();

    if(dishnum_back_total <= 0) {
        errstring = QObject::tr("退货数量为0");
        goto failure;
    }
    query.execSelect(QString("select int_flowid, ch_suitflag, ch_dishno, num_num-num_back as dish_num from cey_u_orderdish where int_flowid = %1 ").arg(int_flowid));
    if(query.next()) {
        int_flowid_list.clear();
        int_flowid_list.append(query.recordValue("int_flowid").toLongLong());
        ch_suitflag = query.recordValue("ch_suitflag").toString();
        ch_dishno = query.recordValue("ch_suitflag").toString();
        back_num_P_org = query.recordValue("dish_num").toDouble();

        if(ch_suitflag == "P") {
            while(query2.execSelect(QString("select int_flowid, ch_suitflag from cey_u_orderdish where int_flowid = %1 + 1 and ch_suitflag = 'Y' ")
                                    .arg(int_flowid_list.last()))) {
                if(query2.next()) {
                    int_flowid_list.append(query2.recordValue("int_flowid").toLongLong());
                } else {
                    break;
                }
            }
        }

        foreach(int id,  int_flowid_list) {
            query2.execSelect(QString("select int_flowid, num_num - num_back, ch_suitflag, ch_dishno, vch_print_memo from cey_u_orderdish where int_flowid = %1 ")
                              .arg(id));
            query2.next();
            num_backData d;
            ch_suitflag = query2.recordValue("ch_suitflag").toString();
            vch_print_memo = query2.recordValue("vch_print_memo").toByteArray();
            dish_num = query2.recordValue(1).toDouble();

            d.int_order_flow_id = id;
            d.num_back_add = dish_num;
            d.dt_back_operdate = dt;
            d.vch_back_operid = operid;
            d.vch_reason = vch_reason;
            d.vch_print_memo.select(vch_print_memo);//套菜全退

            //主商品或普通商品
            if(d.num_back_add <= dishnum_back_total) {
            } else {
                d.num_back_add = dishnum_back_total;
            }
            //子套菜
            if(ch_suitflag == "Y") {
                if(back_num_P_org == 0) {
                    errstring = QObject::tr("操作失败");
                    goto failure;
                }
                d.num_back_add = back_num_P/back_num_P_org * dish_num;
                //                d.vch_print_memo = "";
            }
            d_list.append(d);
            //主商品或普通商品
            dishnum_back_total -= d.num_back_add;
            //子套菜
            if(ch_suitflag == "Y") {
                dishnum_back_total += d.num_back_add;
            }
        }
        return true;
    }
    errstring = QObject::tr("商品没有找到");
failure:
    return false;
}
