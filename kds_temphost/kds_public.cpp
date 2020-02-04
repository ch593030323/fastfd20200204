#include "kds_public.h"
#include <QTextCodec>
#include <QFile>
#include <QtDebug>
#include <QPixmap>

kds_public::kds_public(QObject *parent) :
    QObject(parent)
{
    /*


    kds_public::kds_order_content order_d;
    order_d.bill_no = "1";
    order_d.bill_serial = "2";
    order_d.bill_time = "3";
    order_d.bill_memo = "4";
    order_d.table_no = "5";
    order_d.table_name = "6";
    order_d.dish_list.append(kds_public::kds_order_dish());
    order_d.dish_list.last().row = 0;
    order_d.dish_list.last().no = "dish/no";
    order_d.dish_list.last().name = "dish/name";
    order_d.dish_list.last().num = 12.3;
    order_d.dish_list.last().item_list.append((kds_public::kds_order_item){"010", "APX", 1.1, 0.2});
    order_d.dish_list.last().item_list.append((kds_public::kds_order_item){"010", "APX", 1.1, 0.2});
    order_d.dish_list.append(kds_public::kds_order_dish());
    order_d.dish_list.last().row = 0;
    order_d.dish_list.last().no = "dish/no";
    order_d.dish_list.last().name = "dish/name";
    order_d.dish_list.last().num = 12.3;
    order_d.dish_list.last().item_list.append((kds_public::kds_order_item){"010", "APX", 1.1, 0.2});
    order_d.dish_list.last().item_list.append((kds_public::kds_order_item){"010", "APX", 1.1, 0.2});
    order_d.dish_list.last().item_list.append((kds_public::kds_order_item){"010", "APX", 1.1, 0.2});
    qDebug() << kds_public::kds_order_content2Json(order_d);

    kds_public::kds_return_content order_d;
    order_d.bill_no = "1";
    order_d.dish_list.append(kds_public::kds_return_dish());
    order_d.dish_list.last().row = 0;
    order_d.dish_list.last().num = 12.3;
    order_d.dish_list.last().item_list.append((kds_public::kds_return_item){"010",1.1});
    order_d.dish_list.last().item_list.append((kds_public::kds_return_item){"010",1.1});
    order_d.dish_list.append(kds_public::kds_return_dish());
    order_d.dish_list.last().row = 0;
    order_d.dish_list.last().num = 12.3;
    order_d.dish_list.last().item_list.append((kds_public::kds_return_item){"010",1.1});
    order_d.dish_list.last().item_list.append((kds_public::kds_return_item){"010",1.1});
    order_d.dish_list.last().item_list.append((kds_public::kds_return_item){"010",1.1});
    qDebug() << kds_public::kds_return_content2Json(order_d);

*/
}

QString kds_public::kds_order_content2Json(const kds_public::kds_order_content &d)
{
    QString json;
    json += "{";
    json += "\"type\":\"order\",";
    json += "\"bill_no\":\"" + d.bill_no + "\",";
    json += "\"bill_serial\":\"" + d.bill_serial + "\",";
    json += "\"bill_time\":\"" + d.bill_time + "\",";
    json += "\"bill_memo\":\"" + d.bill_memo + "\",";
    json += "\"table_no\":\"" + d.table_no + "\",";
    json += "\"table_name\":\"" + d.table_name + "\",";
    json += "\"dish_list\":[";
    foreach(const kds_order_dish &dish, d.dish_list) {
        json += "{";
        json += "\"row\":" + QString::number(dish.row) + ",";
        json += "\"no\":\"" + dish.no + "\",";
        json += "\"name\":\"" + dish.name + "\",";
        json += "\"num\":" + QString::number(dish.num) + ",";
        json += "\"type_no\":\"" + dish.type_no + "\",";
        json += "\"item_list\":[";
        foreach(const kds_order_item &item, dish.item_list) {
            json += "{";
            json += "\"no\":\"" + item.no + "\",";
            json += "\"name\":\"" + item.name + "\",";
            json += "\"num\":" + QString::number(item.num) + ",";
            json += "\"price\":" + QString::number(item.price) + ",";
            json.chop(1);
            json += "},";
        }
        json.chop(1);
        json += "]";
        json += "},";
    }
    json.chop(1);
    json += "]";
    json += "}";
    return json;
}

QString kds_public::kds_return_content2Json(const kds_public::kds_return_content &d)
{
    QString json;
    json += "{\"type\":\"return\",";
    json += "\"bill_no\":\"" + d.bill_no + "\",";
    json += "\"bill_reason\":\"" + d.bill_reason + "\",";
    json += "\"dish_list\":[";
    foreach(const kds_return_dish &dish, d.dish_list) {
        json += "{";
        json += "\"row\":" + QString::number(dish.row) + ",";
        json += "\"num\":" + QString::number(dish.num) + ",";
        json += "\"item_list\":[";
        foreach(const kds_return_item &item, dish.item_list) {
            json += "{";
            json += "\"no\":\"" + item.no + "\",";
            json += "\"num\":" + QString::number(item.num) + ",";
            json.chop(1);
            json += "},";
        }
        json.chop(1);
        json += "]";
        json += "},";
    }
    json.chop(1);
    json += "]";
    json += "}";
    return json;
}

void kds_public::setSysTextCodec()
{
    //设置编号
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//中文可用//包含头文件
    QTextCodec::setCodecForLocale(codec);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#else
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#endif
}

QByteArray kds_public::stylesheetData()
{
    QByteArray b;
    QFile file(QString(":/kds/image/fastfd_skyblue.qss"));
    if(file.open(QFile::ReadOnly)) {
        b = file.readAll();
    }
    return b;
}
