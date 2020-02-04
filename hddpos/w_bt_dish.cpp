#include "w_bt_dish.h"
#include "lds_query_hddpos.h"

QStandardItemModel w_bt_dish::MODEL_DISH_DEDUCTTYPE;
QStandardItemModel w_bt_dish::MODEL_DISH_SERVICE_MODE;
QStandardItemModel w_bt_dish::MODEL_DISH_SERVICE_MODE_CUT;
QStandardItemModel w_bt_dish::MODEL_DISH_SERVICE_MODE_CUT_HELP;
QStandardItemModel w_bt_dish::MODEL_DISH_LOW_MODE;
QStandardItemModel w_bt_dish::MODEL_UNIT_NUM_TYPE;
QStandardItemModel w_bt_dish::MODEL_UNIT;
QStandardItemModel w_bt_dish::MODEL_TABLE_SERVICE_FLAG_MODE;
QStandardItemModel w_bt_dish::MODEL_TABLE_INT_PERSON;

QList< QPair<QString, QString> > w_bt_dish::kvaddresslist;
QMap<QString, QString> w_bt_dish::kcmap;
QMap<QString, QString> w_bt_dish::kcmapdesc;
//商品资料维护

w_bt_dish::w_bt_dish(QWidget *parent) : QDialog(parent)
{
}

void w_bt_dish::clearCache()
{
    //基础资料
    MODEL_DISH_DEDUCTTYPE.clear();//t提成方式
    //基础资料
    MODEL_DISH_SERVICE_MODE.clear();//服务费模式
    MODEL_DISH_SERVICE_MODE_CUT.clear();//服务费模式
    MODEL_DISH_SERVICE_MODE_CUT_HELP.clear();//服务费模式
    //基础资料
    MODEL_DISH_LOW_MODE.clear();//低消方式
    //基础资料
    MODEL_UNIT_NUM_TYPE.clear();//数量类型
    //基础资料
    MODEL_UNIT.clear();//单位
    //餐桌资料
    MODEL_TABLE_SERVICE_FLAG_MODE.clear();//Y:折前服务率; N:折后服务率
    //餐桌人数说明
    MODEL_TABLE_INT_PERSON.clear();

    kvaddresslist.clear();
    kcmap.clear();
    kcmapdesc.clear();
}

QStandardItemModel& w_bt_dish::get_MODEL_DISH_DEDUCTTYPE()
{
    QList<QStandardItem *> items;
    if(MODEL_DISH_DEDUCTTYPE.rowCount() == 0) {
        items.clear();items<<new QStandardItem("1")<<new QStandardItem(QObject::tr("比例提成"));MODEL_DISH_DEDUCTTYPE.appendRow(items);
        items.clear();items<<new QStandardItem("2")<<new QStandardItem(QObject::tr("定额提成"));MODEL_DISH_DEDUCTTYPE.appendRow(items);
    }
    return MODEL_DISH_DEDUCTTYPE;
}

QStandardItemModel& w_bt_dish::get_MODEL_DISH_SERVICE_MODE()
{
    QList<QStandardItem *> items;
    if(MODEL_DISH_SERVICE_MODE.rowCount() == 0) {
        items.clear();items<<new QStandardItem("0")<<new QStandardItem(QObject::tr("不处理"));MODEL_DISH_SERVICE_MODE.appendRow(items);
        items.clear();items<<new QStandardItem("1")<<new QStandardItem(QObject::tr("按时间收费"));MODEL_DISH_SERVICE_MODE.appendRow(items);
        items.clear();items<<new QStandardItem("2")<<new QStandardItem(QObject::tr("消费额服务费率"));MODEL_DISH_SERVICE_MODE.appendRow(items);
        items.clear();items<<new QStandardItem("3")<<new QStandardItem(QObject::tr("餐桌定额"));MODEL_DISH_SERVICE_MODE.appendRow(items);
        items.clear();items<<new QStandardItem("4")<<new QStandardItem(QObject::tr("商品消费服务率"));MODEL_DISH_SERVICE_MODE.appendRow(items);

    }
    return MODEL_DISH_SERVICE_MODE;
}

QStandardItemModel& w_bt_dish::get_MODEL_DISH_SERVICE_MODE_CUT()
{
    QList<QStandardItem *> items;
    if(MODEL_DISH_SERVICE_MODE_CUT.rowCount() == 0) {
        items.clear();items<<new QStandardItem("0")<<new QStandardItem(QObject::tr("不处理"));MODEL_DISH_SERVICE_MODE_CUT.appendRow(items);
        items.clear();items<<new QStandardItem("1")<<new QStandardItem(QObject::tr("按时间收费"));MODEL_DISH_SERVICE_MODE_CUT.appendRow(items);
        items.clear();items<<new QStandardItem("3")<<new QStandardItem(QObject::tr("餐桌定额"));MODEL_DISH_SERVICE_MODE_CUT.appendRow(items);
        items.clear();items<<new QStandardItem("4")<<new QStandardItem(QObject::tr("商品消费服务率"));MODEL_DISH_SERVICE_MODE_CUT.appendRow(items);

    }
    return MODEL_DISH_SERVICE_MODE_CUT;
}

QStandardItemModel& w_bt_dish::get_MODEL_DISH_SERVICE_MODE_CUT_HELP()
{
    QList<QStandardItem *> items;
    if(MODEL_DISH_SERVICE_MODE_CUT_HELP.rowCount() == 0) {
        items.clear();items<<new QStandardItem("1")<<new QStandardItem("");MODEL_DISH_SERVICE_MODE_CUT_HELP.appendRow(items);
        items.clear();items<<new QStandardItem("3")<<new QStandardItem("num_ration");MODEL_DISH_SERVICE_MODE_CUT_HELP.appendRow(items);
        items.clear();items<<new QStandardItem("4")<<new QStandardItem("int_rate");MODEL_DISH_SERVICE_MODE_CUT_HELP.appendRow(items);

    }
    return MODEL_DISH_SERVICE_MODE_CUT_HELP;
}

QStandardItemModel& w_bt_dish::get_MODEL_DISH_LOW_MODE()
{
    QList<QStandardItem *> items;
    if(MODEL_DISH_LOW_MODE.rowCount() == 0) {
        items.clear();items<<new QStandardItem("1")<<new QStandardItem(QObject::tr("餐桌低消"));MODEL_DISH_LOW_MODE.appendRow(items);
        items.clear();items<<new QStandardItem("2")<<new QStandardItem(QObject::tr("人均低消"));MODEL_DISH_LOW_MODE.appendRow(items);

    }
    return MODEL_DISH_LOW_MODE;
}

QStandardItemModel& w_bt_dish::get_MODEL_UNIT_NUM_TYPE()
{
    QList<QStandardItem *> items;
    if(MODEL_UNIT_NUM_TYPE.rowCount() == 0) {
        items.clear();items<<new QStandardItem("1")<<new QStandardItem(QObject::tr("整数"));MODEL_UNIT_NUM_TYPE.appendRow(items);
        items.clear();items<<new QStandardItem("2")<<new QStandardItem(QObject::tr("小数"));MODEL_UNIT_NUM_TYPE.appendRow(items);

    }
    return MODEL_UNIT_NUM_TYPE;
}

QStandardItemModel& w_bt_dish::get_MODEL_UNIT()
{
    QList<QStandardItem *> items;
    if(MODEL_UNIT.rowCount() == 0) {
        items.clear();items<<new QStandardItem(QObject::tr("千克"))<<new QStandardItem(QObject::tr("千克"));MODEL_UNIT.appendRow(items);
        items.clear();items<<new QStandardItem(QObject::tr("500克"))<<new QStandardItem(QObject::tr("500克"));MODEL_UNIT.appendRow(items);
        items.clear();items<<new QStandardItem(QObject::tr("50克"))<<new QStandardItem(QObject::tr("50克"));MODEL_UNIT.appendRow(items);
        items.clear();items<<new QStandardItem(QObject::tr("克"))<<new QStandardItem(QObject::tr("克"));MODEL_UNIT.appendRow(items);
    }
    return MODEL_UNIT;
}

QStandardItemModel& w_bt_dish::get_MODEL_TABLE_SERVICE_FLAG_MODE()
{
    QList<QStandardItem *> items;
    if(MODEL_TABLE_SERVICE_FLAG_MODE.rowCount() == 0) {
        //        items.clear();items<<new QStandardItem("Y")<<new QStandardItem(QObject::tr("折前服务率"));MODEL_TABLE_SERVICE_FLAG_MODE.appendRow(items);
        items.clear();items<<new QStandardItem("N")<<new QStandardItem(QObject::tr("折后服务率"));MODEL_TABLE_SERVICE_FLAG_MODE.appendRow(items);

    }
    return MODEL_TABLE_SERVICE_FLAG_MODE;
}

QStandardItemModel& w_bt_dish::get_MODEL_TABLE_INT_PERSON()
{
    QList<QStandardItem *> items;
    if(MODEL_TABLE_INT_PERSON.rowCount() == 0) {
        //        MODEL_TABLE_INT_PERSON.appendRow(QList<QStandardItem *>() << new QStandardItem(QObject::tr("单人桌")+"(1x1)") << new QStandardItem("1x1"));
        //        MODEL_TABLE_INT_PERSON.appendRow(QList<QStandardItem *>() << new QStandardItem(QObject::tr("双人桌横向")+"(2x1)") << new QStandardItem("2x1"));
        //        MODEL_TABLE_INT_PERSON.appendRow(QList<QStandardItem *>() << new QStandardItem(QObject::tr("双人桌纵向")+"(1x2)") << new QStandardItem("1x2"));
        //        MODEL_TABLE_INT_PERSON.appendRow(QList<QStandardItem *>() << new QStandardItem(QObject::tr("四人桌")+"(2x2)") << new QStandardItem("2x2"));
        //        MODEL_TABLE_INT_PERSON.appendRow(QList<QStandardItem *>() << new QStandardItem(QObject::tr("六人桌横向")+"(3x2)") << new QStandardItem("3x2"));
        //        MODEL_TABLE_INT_PERSON.appendRow(QList<QStandardItem *>() << new QStandardItem(QObject::tr("六人桌纵向")+"(2x3)") << new QStandardItem("2x3"));
        //        MODEL_TABLE_INT_PERSON.appendRow(QList<QStandardItem *>() << new QStandardItem(QObject::tr("八人桌及以上")+"(3x3)") << new QStandardItem("3x3"));

    }
    return MODEL_TABLE_INT_PERSON;
}

QMap<QString, QString> &w_bt_dish::get_kcmap()
{
    if(kcmap.count() == 0) {
        kcmap.insert("N", "green");//一般支付方式
        kcmap.insert("T", "crimson");//外卖支付方式
        kcmap.insert("S", "purple");//其他
    }
    return kcmap;
}

QMap<QString, QString> &w_bt_dish::get_kcmapdesc()
{
    if(kcmapdesc.count() == 0) {
        kcmapdesc.insert("green", QObject::tr("普通支付"));
        kcmapdesc.insert("crimson", QObject::tr("外卖验证"));
        kcmapdesc.insert("purple", QObject::tr("其他"));
    }
    return kcmapdesc;
}

QList<QPair<QString, QString> > &w_bt_dish::get_kvaddresslist()
{
    if(kvaddresslist.count() == 0) {
        kvaddresslist.append(QPair<QString, QString>(QObject::tr("美团"), "http://e.meituan.com"));
        kvaddresslist.append(QPair<QString, QString>(QObject::tr("百度糯米"), "http://login.nuomi.com/account/login"));
        kvaddresslist.append(QPair<QString, QString>(QObject::tr("大众点评"), "http://www.dianping.com/login"));
        kvaddresslist.append(QPair<QString, QString>(QObject::tr("饿了么"), "http://account.ele.me/login"));
        kvaddresslist.append(QPair<QString, QString>(QObject::tr("窝窝网"), "http://user.55tuan.com"));

        kvaddresslist.append(QPair<QString, QString>(QObject::tr("拉手网"), "http://www.lashou.com/account/login"));
        kvaddresslist.append(QPair<QString, QString>(QObject::tr("聚美优品"), "http://www.jumei.com/i/account/login"));
        kvaddresslist.append(QPair<QString, QString>(QObject::tr("58团购"), "http://passport.58.com/login"));
    }
    return kvaddresslist;
}

void w_bt_dish::get_vch_plan_k3o3(QString &state, QString &id, const QString &ch_dishno, w_bt_dish::kitchenState s0)
{
    lds_query_hddpos  query;
    QString vch_plan_k3o3_state;
    QString vch_plan_k3o3_id;
    query.execSelect(QString("select vch_kitchen_print_id1,  vch_kitchen_print_id2,  vch_kitchen_print_id3  from cey_bt_dish where ch_kitchen_print_flag = 'Y' and ch_dishno = '%1' ")
               .arg(ch_dishno));
    query.next();
    QStringList k3 = QStringList() << query.recordValue(0).toString() << query.recordValue(1).toString() << query.recordValue(2).toString();
    vch_plan_k3o3_id += (k3[0].isEmpty() ? "00" : k3[0]) +  (k3[1].isEmpty() ? "00" : k3[1]) + (k3[2].isEmpty() ? "00" : k3[2]);
    vch_plan_k3o3_state += w_bt_dish::k2o3_state2hex_char(k3[0].isEmpty() ? w_bt_dish::KITCHEN_FINISH : s0);
    vch_plan_k3o3_state += w_bt_dish::k2o3_state2hex_char(k3[1].isEmpty() ? w_bt_dish::KITCHEN_FINISH : s0);
    vch_plan_k3o3_state += w_bt_dish::k2o3_state2hex_char(k3[2].isEmpty() ? w_bt_dish::KITCHEN_FINISH : s0);
    query.execSelect(QString("select vch_kitchen_out_id1,  vch_kitchen_out_id2,  vch_kitchen_out_id3  from cey_bt_dish where ch_kitchen_out_flag = 'Y' and ch_dishno = '%1' ")
               .arg(ch_dishno));
    query.next();
    QStringList o3 = QStringList() << query.recordValue(0).toString() << query.recordValue(1).toString() << query.recordValue(2).toString();
    vch_plan_k3o3_id += (o3[0].isEmpty() ? "00" : o3[0]) +  (o3[1].isEmpty() ? "00" : o3[1]) + (o3[2].isEmpty() ? "00" : o3[2]);
    vch_plan_k3o3_state += w_bt_dish::k2o3_state2hex_char(o3[0].isEmpty() ? w_bt_dish::KITCHEN_FINISH : s0);
    vch_plan_k3o3_state += w_bt_dish::k2o3_state2hex_char(o3[1].isEmpty() ? w_bt_dish::KITCHEN_FINISH : s0);
    vch_plan_k3o3_state += w_bt_dish::k2o3_state2hex_char(o3[2].isEmpty() ? w_bt_dish::KITCHEN_FINISH : s0);

    state = vch_plan_k3o3_state;
    id  = vch_plan_k3o3_id;
}

//0-f
char w_bt_dish::k2o3_state2hex_char(w_bt_dish::kitchenState state)
{
    return QByteArray::number(state, 16).at(0);
}

QString w_bt_dish::k2o3_stae2hex_str(w_bt_dish::kitchenState state)
{
    return QString(k2o3_state2hex_char(state));
}

QString w_bt_dish::k2o3_name(w_bt_dish::kitchenState state)
{
    if(KITCHEN_MIN_DEF == state) return QObject::tr("厨打");
    if(KITCHEN_FINISH == state) return QObject::tr("完成");
    if(KITCHEN_VOID == state) return QObject::tr("空");
    if(KITCHEN_QUIT == state) return QObject::tr("退货");
    if(KITCHEN_CUICAI == state) return QObject::tr("催起");

    if(KITCHEN_JIAOQI == state) return QObject::tr("叫起");
    if(KITCHEN_GUAQI == state) return QObject::tr("挂起");
    if(KITCHEN_ZHUANTAI == state) return QObject::tr("转台");
    if(KITCHEN_FUZHI == state) return QObject::tr("商品复制");
    if(KITCHEN_BINGTAI == state) return QObject::tr("并台");

    if(KITCHEN_JIACAI == state) return QObject::tr("另加");
    if(KITCHEN_WAIMAI == state) return QObject::tr("外卖");
    if(KITCHEN_ZHUANTAI_CAIPIN == state) return QObject::tr("商品转台");
    if(KITCHEN_QUIT_KUAICAN == state) return QObject::tr("退货");
    if(KITCHEN_MAX == state) return QObject::tr("最大值");



    return "";
}


QString w_bt_dish::model_data(QAbstractItemModel *m, int row, int column)
{
    QString ret;
    if(row < m->rowCount() && column < m->columnCount() ) {
        ret = m->data(m->index(row, column)).toString();
    }
    return ret;
}

