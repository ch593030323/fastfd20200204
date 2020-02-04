#include "kds.h"
#include <QFile>
#include <QtDebug>
#include <QSqlDatabase>
#include "lds_query_db.h"
#include <QSqlError>
#include "lds.h"
#include <QFontDatabase>
#include <QFileInfo>
#include <QPainter>
#include <QApplication>
#include <QMessageBox>
#include <QProcess>
#include "cjson_help.h"
#include "lds_lan_terminal.h"
#include "vch_print_memodata.h"
#include "printer_pos.h"

QString kds::color_prepared = "#277237";
QString kds::color_prepared_warn = "#de2426";
QString kds::color_cooking = "#2a85c7";
QString kds::color_finished = "#4f5f85";
QString kds::color_token = "#383838";
QString kds::color_cancel = "#ff00ff";

const QString kds::log_file_path = "userdata/kds/log.txt";
lds_ogg_play_thread *kds::mediaPlayer = 0;
QString kds::_FONT_NUM_Family;
QString kds::cur_language;
const QString kds::kdsdd_request_setTitle = "title";
const QString kds::kdsdd_request_setCooking = "cooking";
const QString kds::kdsdd_request_setService = "service";
const QString kds::kdsdd_request_setCookingService = "cooking,service";
const QString kds::kdsdd_request_close = "close";
const QString kds::kdsdd_request_size = "size";

lds_model_sqltablemodel_m* kds::model = 0;
QMap<QString, QTranslator*> kds::translator_map;

QString kds::print_flag_cook = "10";
/*
fastfdfood_kitchen:快餐厨显
fastfdfood_service:快餐出品
restaurant_kitchen:酒楼厨显
restaurant_service:酒楼出品
*/
QElapsedTimer kds::voice_play_timer;


bool kds::db_open(QSqlDatabase &db){
    //  This must be done before the connection is opened
    db.setConnectOptions("MYSQL_OPT_RECONNECT=1;");
    return db.open();
}

void kds::db_close(QSqlDatabase &db){
    db.setConnectOptions();
    db.close();
}


QByteArray kds::btn_detail_json(const QString &bill_no, const QString &vch_plan_id, int less_than_state)
{
    billPlanDetail d;
    lds_query_db query;
    d.bill_no = bill_no;
    d.vch_plan_id = vch_plan_id;
    query.execSelect(QString(" select int_flowID from cey_u_orderdish_print_kt_order where vch_plan_id =  '%1' and ch_billno = '%2' and ch_state < %3 order by ch_state desc")
                     .arg(vch_plan_id)
                     .arg(bill_no)
                     .arg(less_than_state)
                     );
    while(query.next()) {
        d.flow_id_list << query.recordValue("int_flowID").toString();
    }
    QList<billPlanDetail> list;
    list << d;
    return btn_detail_json(list);
}

QByteArray kds::btn_detail_json(const QString &bill_no, const QString &vch_plan_id, int less_than_state, int id)
{
    return cJSON_path::replaceItemValue(btn_detail_json(bill_no, vch_plan_id, less_than_state), "{items[0", "id", id);
}

QByteArray kds::btn_detail_json(const QList<billPlanDetail> &bill_flow_id_list)
{
    vch_print_memoData item_practice;
    QString  kt_kitchen_shop = kds::f_get_sysparm("kt_kitchen_shop");
    QString json;
    json += QString("{\"row_count\": 9,\"row_begin\": 0,\"row_id_visible\": 0,\"item_more_visible\": 1, \"less_than_state\":%1, \"exception_finish_sql\":\"\", \"items\": [ ").arg(KDS::cookMax);
    foreach(const billPlanDetail &d, bill_flow_id_list) {
        QString table_no = kds::model->tablename_kvmap_value("cey_u_table", d.bill_no, "ch_billno", "ch_tableno").toString();
        QString state = kds::selectValue(QString("select ch_state from cey_u_orderdish_print_kt_table where ch_billno = '%1' and vch_plan_id = '%2'  ").arg(d.bill_no).arg(d.vch_plan_id)).toString();
        QDateTime time = kds::selectValue(QString("select dt_operdate from cey_u_orderdish_print_kt_table where ch_billno = '%1' and vch_plan_id = '%2'  ").arg(d.bill_no).arg(d.vch_plan_id)).toDateTime();
        QString bill_text = table_no;
        if(kt_kitchen_shop == "restaurant"){
            bill_text = kds::model->tablename_kvmap_value("cey_bt_table", table_no, "ch_tableno", "vch_tablename").toString();
            if(table_no == "Q000")
                bill_text = QObject::tr("快餐");
        }
        if(kt_kitchen_shop == "fastfdfood"){
            bill_text = kds::model->tablename_kvmap_value("cey_u_master", d.bill_no, "ch_billno", "vch_memo").toString();
        }
        json +=
                "{"
                "\"type\":\"title\","
                "\"flow_id\":\"\","
                "\"bill_no\":\"" + d.bill_no + "\","
                "\"bill_text\":\"" + bill_text + "\","
                "\"state\":\"" + state + "\","
                "\"time\":\"" + time.toString(yyyyMMddhhmmss) + "\","
                "\"id\":\"\","
                "\"item_practice\":\"\""
                "},";
        foreach(const QString &flow_id, d.flow_id_list) {
            json +=
                    "{"
                    "\"type\":\"item\","
                    "\"flow_id\":" + flow_id+ ","
                    "\"bill_no\":\"\","
                    "\"bill_text\":\"\","
                    "\"state\":\"\","
                    "\"time\":\"\","
                    "\"id\":\"\","
                    "\"item_practice\":\"\""
                    "},";
            item_practice.select(model->tablename_kvmap_value("cey_u_orderdish", flow_id, "int_flowID", "vch_print_memo").toString());
            if(item_practice.count() > 0) {
                json +=
                        "{"
                        "\"type\":\"item_practice\","
                        "\"flow_id\":" + flow_id+ ","
                        "\"bill_no\":\"\","
                        "\"bill_text\":\"\","
                        "\"state\":\"\","
                        "\"time\":\"\","
                        "\"id\":\"\","
                        "\"item_practice\":\"" + item_practice.jsonToString(1, false) + "\""
                        "},";
            }
        }
    }
    //type, flow_id, bill_no, bill_text, id
    json.chop(1);
    json = json + "] }";

    return json.toLocal8Bit();
}

void kds::print_cook(const QString &flow_id)
{
    if(false == lds::sysconf->value("kds_main_info_print/check", false).toBool())
        return;
    QString printport = lds::sysconf->value("kds_main_info_print/printport").toString();
    QSharedPointer<Printer_POS> printer = Printer_POS::get_pos_printer_by_path(printport);
    if(false == printer->tryOpen(printport)) {
        return;
    }
    QString table_no = kds::model->tablename_kvmap_value("cey_u_orderdish", flow_id, "int_flowID", "ch_tableno").toString();
    QString dish_no = kds::model->tablename_kvmap_value("cey_u_orderdish", flow_id, "int_flowID", "ch_dishno").toString();
    QString table_name = kds::model->tablename_kvmap_value("cey_bt_table", table_no, "ch_tableno", "vch_tablename").toString();
    QString dish_name =kds::model->tablename_kvmap_value("cey_bt_dish", dish_no, "ch_dishno", "vch_dishname").toString();
    if(table_name == "")
        table_name = table_no;
    QByteArray printerdata;
    static int gs_w = 1;
    gs_w ++;
    printerdata += QByteArray::fromHex("1b40 1D2111");
    printerdata += printer_0::trans_UTF82GBK(QObject::tr("餐桌") + ":" + table_name) + "\n";
    printerdata += printer_0::trans_UTF82GBK(QObject::tr("菜品") + ":" + dish_name) +"\n";
    if(flow_id.count() <= 5)
        printerdata += QByteArray::fromHex("1D7703");//走纸5x， 切纸
    else
        printerdata += QByteArray::fromHex("1D7702");//走纸5x， 切纸
    printerdata += QByteArray::fromHex(printer_0::draw_bar_code(print_flag_cook + flow_id));
    printerdata += QByteArray::fromHex("1B6405 1D5601");//走纸5x， 切纸

    printer->writedirect(printerdata);
}

kds::kds(QObject *parent) : QObject(parent)
{
}

QByteArray kds::stylesheetData()
{
#ifdef QT_DEBUG
    QFile file(QString("/home/cz/文档/fastfdmain/fastfdmain_KDS/kds/skin.qss"));
#else
    QFile file(QString(":/kds/skin.qss"));
#endif
    if(file.open(QFile::ReadOnly)) {
        qDebug() << file.errorString();
        return file.readAll();
    }
    return QByteArray();
}

int kds::func_dec_price()
{
    return 2;
}

int kds::func_dec_num()
{
    return 2;
}

int kds::func_dec_amt()
{
    return 2;
}

QString kds::color_state(int state, const QDateTime &dt)
{
    int mins_off = 0;
    QString bgcolor;
    switch(state) {
    case KDS::cookPrepared://
        mins_off = dt.secsTo(kds::currentDateTime()) / 60;
        bgcolor = color_prepared;
        if(mins_off >= cey_bt_kitchen_view_data("int_order_warn_mins").toInt()) {
            bgcolor = kds::color_prepared_warn;
        }
        break;
    case KDS::cookCooking://
        bgcolor = color_cooking;
        break;
    case KDS::cookFinished://
        bgcolor = color_finished;
        break;
    case KDS::cookToken://
        bgcolor = color_token;
        break;
    case KDS::cookCanceled://
        bgcolor = color_cancel;
        break;
    }
    return bgcolor;
}

QSize kds::xstringToSize(const QString &sizestr, const QString &defSize)
{
    if(sizestr.contains("x"))
        return QSize(sizestr.split("x").value(0).toInt(), sizestr.split("x").value(1).toInt());
    return QSize(defSize.split("x").value(0).toInt(), defSize.split("x").value(1).toInt());
}

QSize kds::xstringToSize(const QString &sizestr, const QSize &defSize)
{
    if(sizestr.contains("x"))
        return QSize(sizestr.split("x").value(0).toInt(), sizestr.split("x").value(1).toInt());
    return defSize;
}

QString kds::sizeToxstring(const QSize &size)
{
    return QString("%1x%2").arg(size.width()).arg(size.height());
}

void kds::recordFill(const QSqlRecord &record, qrtVariantPairList &d)
{
    for(int k = 0; k < record.count(); k++) {
        d.append(qrtVariantPair(record.fieldName(k), record.value(k)));
    }
}

QVariant kds::selectValue(const QString &sql)
{
    lds_query_db query;
    if(query.execSelect(sql)) {
        if(query.next()) {
            return query.recordValue(0);
        }
    }
    return QVariant();
}

QVariant kds::selectValue(const QString &sql, const QString &arg0)
{
    return selectValue(sql.arg(arg0));
}

QVariant kds::selectValue(const QString &sql, int arg0)
{
    return selectValue(sql.arg(arg0));
}

bool kds::selectHasNext(const QString &sql)
{
    lds_query_db query;
    query.execSelect(sql);
    return query.next();
}

bool kds::selectHasNext(const QString &sql, const QString &arg0)
{
    return selectHasNext(sql.arg(arg0));
}

bool kds::selectHasNext(const QString &sql, int arg0)
{
    return selectHasNext(sql.arg(arg0));
}

void kds::writeLog(const QString &message)
{
        if(QFileInfo(log_file_path).created().date().daysTo(QDate::currentDate()) > 2) {
            QFile::remove(log_file_path);
        }
        lds::fileAppendByte(log_file_path, (kds::currentDateTime().toString(yyyyMMddhhmmss) + "\t" + message+ "\n").toLocal8Bit());
}

bool kds::is_fastfdfood_kitchen()
{
    if(kds::model == 0)
        return false;
    return
            kds::f_get_sysparm("kt_kitchen_shop") == "fastfdfood"
            && kds::cey_bt_kitchen_view_data("vch_kitchen_type") == "kitchen"
            ;
}

bool kds::is_fastfdfood_service()
{
    if(kds::model == 0)
        return false;
    return
            kds::f_get_sysparm("kt_kitchen_shop") == "fastfdfood"
            && kds::cey_bt_kitchen_view_data("vch_kitchen_type") == "service"
            ;
}

bool kds::is_restaurant_kitchen()
{
    if(kds::model == 0)
        return false;
    return
            kds::f_get_sysparm("kt_kitchen_shop") == "restaurant"
            && kds::cey_bt_kitchen_view_data("vch_kitchen_type") == "kitchen"
            ;
}

bool kds::is_restaurant_service()
{
    if(kds::model == 0)
        return false;
    return
            kds::f_get_sysparm("kt_kitchen_shop") == "restaurant"
            && kds::cey_bt_kitchen_view_data("vch_kitchen_type") == "service"
            ;
}

QStringList kds::numStringToOggName(const QString &num_string)
{
    QStringList list;
    foreach(const QChar &c, num_string)
        list << lds_ogg_play::charOggName(c);
    return list;
}

QString kds::FONT_NUM_Family()
{
    if(_FONT_NUM_Family.count() == 0) {
        ///home/cz/文档/build-fastfdmain_new-Desktop-Debug/userdata/settings/font/LiquidCrystal-Normal.otf
        int FONT_ID = QFontDatabase::addApplicationFont("userdata/kds/DIGITAL-Regular.ttf");
        if(-1 != FONT_ID) {
            _FONT_NUM_Family = QFontDatabase::applicationFontFamilies(FONT_ID).first();
        } else {
            _FONT_NUM_Family = qApp->font().family();
        }
    }
    return _FONT_NUM_Family;
}

void kds::setTranslator(const QString &la)
{
    QTranslator *t = getTranslator(cur_language);
    if(cur_language  != la) {
        cur_language = la;
        qApp->removeTranslator(t);
        qApp->installTranslator(getTranslator(la));
    }
}

QTranslator *kds::getTranslator(QString la)
{
    //    /home/cz/文档/build-fastfdmain_KDS-Desktop-Debug/
    if("default" == la) {
        la = lds::get_sys_language();
    }
    QString qmpath = QString("userdata/kds/qm/%1_fastfd.qm").arg(la);
    if(QFile::exists(qmpath)) {
        if(kds::translator_map.find(la) == kds::translator_map.end()) {
            QTranslator *tr = new QTranslator;
            tr->load(qmpath);
            kds::translator_map.insert(la, tr);
        }
        return kds::translator_map[la];
    }
    return 0;
}

void kds::kdsdd_datagram_make(const kds::kdsddData &dd, QByteArray &datagram)
{
    datagram = "{";
    datagram += QString("\"type\":\"%1\",").arg(dd.type);
    datagram += QString("\"size\":\"%1\",").arg(dd.size);
    datagram += "\"list_cooking\":[";
    foreach(const QString &str, dd.list_cooking)
        datagram += QString("\"%1\",").arg(str);
    if(datagram.endsWith(","))
        datagram.chop(1);
    datagram+= "],";
    datagram += "\"list_service\":[";
    foreach(const QString &str, dd.list_service)
        datagram += QString("\"%1\",").arg(str);
    if(datagram.endsWith(","))
        datagram.chop(1);
    datagram+= "]}";
};

void kds::kdsdd_datagram_resolve(kds::kdsddData &dd, const QByteArray &datagram)
{
    cJSON_path p(datagram);
    if(p.isNull())
        return;

    dd.type =  p.path_value("{type").toString();
    dd.size =  p.path_value("{size").toString();
    dd.list_cooking.clear();
    for(int k = 0; k < p.array_count("{list_cooking"); k ++)
        dd.list_cooking << p.array_value("{list_cooking[%1", k).toString();
    dd.list_service.clear();
    for(int k = 0; k < p.array_count("{list_service"); k ++)
        dd.list_service << p.array_value("{list_service[%1", k).toString();
}

void kds::kdsdd_send(const kds::kdsddData &dd)
{
    if(kds::model == 0)
        return;
    if(kds::kdsdd_request_close == dd.type
            || "Y" == cey_bt_kitchen_view_data("ch_screen_secondary").toString()) {
        QByteArray datagram;
        kdsdd_datagram_make(dd, datagram);
        lds::doublescreensenddata(datagram);
    }
}

void kds::kdsdd_send_type(const QString &type)
{
    kds::kdsdd_send(kds::kdsddData::create(type));
}

void kds::kdsdd_send_scree_size(const QString &size)
{
    kds::kdsddData d;
    d.type = kdsdd_request_size;
    d.size = size;
    kds::kdsdd_send(d);
}

void kds::kds_second_screen_read(QMap<QString, QString> &map)
{
    QFile file("userdata/kds/kds_second_screen.txt");
    file.open(QFile::ReadOnly);
    while(!file.atEnd()) {
        QString line = file.readLine().trimmed();
        int k = line.indexOf("=");
        map.insert(line.mid(0, k), line.mid(k + 1));
    }
}

void kds::kds_second_screen_translate()
{
    QMap<QString, QString> map;
    kds_second_screen_read(map);
    if(map.value("isTest", "true") == "true") {
        QString d =
                "company_name="+QObject::tr("好搭档智能厨房显示系统") + "\n"
                "information="+QObject::tr("请留意您的取单号码取餐(Please check your number)") + "\n"
                "service="+QObject::tr("请取餐(Served)") + "\n"
                "cooking="+QObject::tr("准备中(Coming)") + "\n"
                "soft_name=KDS System\n"
                "logo=userdata/kds/fastfdbacklogo.png\n"
                "isTest=true"
                ;
        lds::fileWriteString("userdata/kds/kds_second_screen.txt", d);
    }
}

QDateTime kds::currentDateTime()
{
    if(!QSqlDatabase::database(QSqlDatabase::defaultConnection, false).isOpen())
        return QDateTime();

    return selectValue("select now()").toDateTime();
}

bool kds::update_print_kt_dish(const QString &flow_id, int target_state, QString &errstring, bool *via_cook)
{
    lds_query_db query;
    QString vch_plan_id = kds::selectValue("select vch_plan_id from cey_u_orderdish_print_kt_order where int_flowID = " + flow_id).toString();
    QString ch_billno = kds::selectValue("select ch_billno from cey_u_orderdish_print_kt_order where int_flowID = " + flow_id).toString();
    double num_dish = kds::selectValue(QString(" select num_num - num_back from cey_u_orderdish where int_flowID = %1 ").arg(flow_id)).toDouble();
    QString ch_dishno = kds::selectValue(QString(" select ch_dishno from cey_u_orderdish where int_flowID = %1 ").arg(flow_id)).toString();
    QString where_order_sql = QString(" vch_plan_id = '%1' and  ch_billno = '%2' ").arg(vch_plan_id).arg(ch_billno);
    QString where_dish_sql = QString(" vch_plan_id = '%1' and  ch_dishno = '%2' ").arg(vch_plan_id).arg(ch_dishno);
    QString select_sql;
    int int_finish;
    int int_cook;
    int ch_state = kds::selectValue("select ch_state from cey_u_orderdish_print_kt_order where int_flowID = " + flow_id).toInt();
    const bool via_cook_flag = (ch_state < KDS::cookCooking && target_state >= KDS::cookCooking);
    //
    kds::writeLog("flow:" + flow_id);
    kds::writeLog("dish:" + ch_dishno);
    kds::writeLog("cur:" + KDS::cookStateName(ch_state));
    kds::writeLog("target:" + KDS::cookStateName(target_state));
    //
    if(target_state <= ch_state) {
        errstring = KDS::cookStateName(ch_state);
//        errstring = "target state must greater than current state";
        return false;
    }
    if(target_state == KDS::cookToken && ch_state != KDS::cookFinished) {
        errstring = "current state must be finished state when take order";
        return false;
    }
    if(ch_state < KDS::cookFinished && target_state >= KDS::cookFinished) {
        double num_finish = kds::selectValue(" select num_finish from cey_u_orderdish_print_kt_dish where " + where_dish_sql).toDouble() + num_dish;
        if(!query.execUpdate("cey_u_orderdish_print_kt_dish", "num_finish", num_finish, where_dish_sql)) {
            errstring = query.errorString();
            return false;
        }
    }
    if(target_state == KDS::cookFinished) {
        kds::cey_bt_kitchen_view_update_service("ch_refresh_order_update", "Y");
    }
    if(target_state == KDS::cookCooking) {
        double num_cook = kds::selectValue(" select num_cook from cey_u_orderdish_print_kt_dish where " + where_dish_sql).toDouble()+ num_dish;
        if(!query.execUpdate("cey_u_orderdish_print_kt_dish", "num_cook", num_cook, where_dish_sql)) {
            errstring = query.errorString();
            return false;
        }
    }
    if(ch_state == KDS::cookCooking) {
        double num_cook = kds::selectValue(" select num_cook from cey_u_orderdish_print_kt_dish where " + where_dish_sql).toDouble() - num_dish;
        if(!query.execUpdate("cey_u_orderdish_print_kt_dish", "num_cook", num_cook, where_dish_sql)) {
            errstring = query.errorString();
            return false;
        }
    }
    if(target_state == KDS::cookCanceled) {
        if(kds::cey_u_orderdish_num_dish(flow_id) > 0) {
            errstring = "num_dish cannot greater than 0 when cancel dish";
            return false;
        }
    }
    //
    if(!query.execUpdate("cey_u_orderdish_print_kt_order", "ch_state", target_state, QString("int_flowID = %1 ").arg(flow_id))) {
        errstring = query.errorString();
        return false;
    }
    //
    select_sql = QString(" select min(ch_state) as ch_state, sum(ch_state = %1) as int_finish, sum(ch_state = %2) as int_cook from cey_u_orderdish_print_kt_order ")
            .arg(KDS::cookFinished)
            .arg(KDS::cookCooking);
    select_sql +=  " where " + where_order_sql;
    query.execSelect(select_sql);
    query.next();
    ch_state = query.recordValue("ch_state").toInt();
    int_finish = query.recordValue("int_finish").toInt();
    int_cook = query.recordValue("int_cook").toInt();
    if(!query.execUpdate("cey_u_orderdish_print_kt_table", qrtVariantPairList()
                         << qrtVariantPair("ch_state", ch_state)
                         << qrtVariantPair("int_finish", int_finish)
                         << qrtVariantPair("int_cook", int_cook), where_order_sql
                         )) {
        errstring = query.errorString();
        return false;
    }

    if(via_cook)
        *via_cook = via_cook_flag;
    return true;
}

bool kds::updateTable(const QString &tablename, const qrtVariantPairList &key_value_list, const QString &where_sql, QString *errstring)
{
    lds_query_db q;
    return q.execUpdate(tablename, key_value_list, where_sql, errstring);
}

bool kds::pos_kdsdd_existed()
{
#ifdef QT_DEBUG
    return false;
#endif
    QStringList kdsddlist;
    lds::cmd_ps_pid(kdsddlist, "pos/kdsd", "pos/kdsdd");
    return kdsddlist.count() > 0;
}

void kds::pos_kdsdd_start()
{
    //开始
    QString cmd_exec =qApp->applicationDirPath()+"/kdsdd";
    QStringList cmd_arg;
    cmd_arg << "-ss" + kds::cey_bt_kitchen_view_data("vch_screen_secondary_size").toString();

#if defined (Q_OS_UNIX)
#ifdef QT_NO_DEBUG
    QString param = QString("-display Transformed:Rot%1:LinuxFb:/dev/fb1:1 -nomouse -nokeyboard -qws")
            .arg(0);//为逆时针旋转
    cmd_exec += " " + param;
#endif
    system(QString(cmd_exec + " " + cmd_arg.join(" ")).toLocal8Bit() + "&");
#else
    QProcess::startDetached(cmd_exec, cmd_arg);
#endif
}

bool kds::softCanStart()
{
#ifdef Q_OS_UNIX
    //fastfddlist fastfdlist
    QStringList fastfddlist;
    QStringList fastfdlist;
    //相比SoftPos少一个s，以免后面匹配Pos时，多一个
    lds::cmd_ps_pid(fastfdlist, "pos/kd", "pos/kds");
    //相比fastfdd少一个d，以免后面匹配Pos时，多一个
    lds::cmd_ps_pid(fastfddlist, "pos/kdsd", "pos/kdsdd");

    if(fastfdlist.count() > 1 || fastfddlist.count() >= 1) {//软件软已经启动，并且本次启动fastfdlist>1；副屏启动时
        if(1 == QMessageBox::critical(0, "", QObject::tr("软件已启动") +":"+"self"+QString::number(qApp->applicationPid())+"fastfdlist:"+fastfdlist.join("|")+"fastfddlist:"+fastfddlist.join("|"),
                                      QObject::tr("关闭软件"),
                                      QObject::tr("退出"))) {
            return false;
        }

        QProcess p;
        //fastfdd异常的话，全部杀死
        foreach(const QString &pid, fastfddlist) {
            p.start("kill -9 "+pid);
            p.waitForReadyRead(5*1000);
            p.close();
        }
        //fastfd异常的话，全部杀死
        foreach(const QString &pid, fastfdlist) {
            if(pid == QString::number(qApp->applicationPid())) {//本身最后杀死
                continue;
            }
            p.start("kill -9 "+pid);
            p.waitForReadyRead(5*1000);
            p.close();
        }
        //杀死本身
        p.start("kill -9 "+QString::number(qApp->applicationPid()));
        p.waitForReadyRead(5*1000);
        p.close();
        return false;
    }
#endif
    return true;
}


void kds::kdsddData::print() const
{
    qDebug() << "--------------------begin-------------------";
    qDebug() << "type:" << type;
    qDebug() << "list_cooking:" << list_cooking;
    qDebug() << "list_service:" << list_service;
    qDebug() << "size:" << size;
    qDebug() << "--------------------end-------------------";
}

kds::kdsddData kds::kdsddData::create(const QString &type)
{
    kds::kdsddData d;
    d.type =type;
    return d;
}

QList<QRectF> kds::paintTableBrief(QPainter *painter, const QByteArray &json, QRectF rect)
{
    QList<QRectF> row_rect_list;
    cJSON_path p(json);
    if(p.isNull()) {
        return row_rect_list;
    }
    QRectF r;
    int flow_id_right;
    int sawtooth_right;
    QString type;
    QString bgcolor;
    QDateTime time;
    QString is_suit;
    int item_name_left;
    QString ch_dishno;
    double num_total;
    bool draw_more;
    QString flow_id;
    QString bill_no;
    QString id;
    int state;
    const int row_begin = p.path_value("{row_begin").toInt();
    const int row_id_visible = p.path_value("{row_id_visible").toInt();
    const int row_count = p.path_value("{row_count").toInt();
    const int item_more_visible = p.path_value("{item_more_visible").toInt();

    if(painter) painter->setRenderHint(QPainter::Antialiasing);
    if(painter) painter->fillRect(rect, QColor(37, 48, 65));
    int off = 5;
    qreal h = (rect.height() - off) / row_count;
    if(painter) painter->setFont(kds::fontPixelSize(painter->font(), h * 0.6));
    int row_id = row_begin;

    for(int k = 0; k < row_count; k ++) {
        row_rect_list << QRectF(rect.x(), rect.y() + k * h, rect.width(), h).adjusted(off, off, -off, 0);
    }
    p.select_array("{items", row_begin - 1);
    for(int k = 0; k < row_count && p.select_next(); k ++) {
        r = row_rect_list[k];
        flow_id_right = kds::flowIdRight(r);
        sawtooth_right = kds::sawtoothRight(r);
        type = p.select_value("type").toString();
        //item
        if(type == "item") {
            flow_id = p.select_value("flow_id").toString();
            state = model->tablename_kvmap_value("cey_u_orderdish_print_kt_order", flow_id, "int_flowID", "ch_state").toInt();

            time = model->tablename_kvmap_value("cey_u_orderdish", flow_id, "int_flowID", "dt_operdate").toDateTime();
            is_suit = model->tablename_kvmap_value("cey_u_orderdish", flow_id, "int_flowID", "ch_suitflag").toString();
            bgcolor = kds::color_state(state, time);
            if(painter) painter->fillRect(r, QColor(bgcolor));
            if(painter) painter->fillPath(sawtoothPath(r.adjusted(r.width() - sawtooth_right, 1, -flow_id_right, -1), 5), QColor("white"));
            if(painter) painter->fillRect(r.adjusted(1, 1, -sawtooth_right, -1), QColor("white"));

            item_name_left = 0;
            if(row_id_visible == 1) {
                item_name_left = h;
                if(is_suit != "Y") {
                    if(painter) painter->setPen(QColor("black"));
                    if(painter) painter->drawText(QRect(r.x(), r.y(), item_name_left, r.height()), Qt::AlignRight | Qt::AlignVCenter, QString::number(row_id) +". ");
                    row_id ++;
                }
            }
            if(row_id_visible == 0 && is_suit == "Y") {
                item_name_left = h;
            }
            ch_dishno = model->tablename_kvmap_value("cey_u_orderdish", flow_id, "int_flowID", "ch_dishno").toString();
            if(painter) painter->setPen(QColor("black"));
            if(painter) painter->drawText(r.adjusted(item_name_left + 1, 1, -sawtooth_right, -1), Qt::AlignLeft | Qt::AlignVCenter, model->tablename_kvmap_value("cey_bt_dish", ch_dishno, "ch_dishno", "vch_dishname").toString());
            if(painter) painter->setPen(QColor("white"));
            if(painter) painter->drawText(r.adjusted(r.width() - flow_id_right, 0, 0, 0), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(cey_u_orderdish_num_dish(flow_id)));
        }
        //item_practice
        if(type == "item_practice") {
            if(painter) painter->fillRect(r, QColor("white"));
            if(painter) painter->setPen(QColor("black"));
            if(painter) painter->drawText(r.adjusted(item_name_left + 1, 1, -1, -1), Qt::AlignLeft | Qt::AlignVCenter, p.select_value("item_practice").toString());
        }
        //title
        if(type == "title") {
            bill_no = p.select_value("bill_no").toString();
            id = p.select_value("id").toString();
            state = p.select_value("state").toInt();
            time = p.select_value("time").toDateTime();

            row_id = row_begin + 1;
            bgcolor = kds::color_state(state, time);
            int mins_off = time.secsTo(kds::currentDateTime());
            if(painter) painter->fillRect(r, QColor(bgcolor));
            if(painter) painter->setPen(QColor("white"));
            if(painter) painter->drawText(r.adjusted(5, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, p.select_value("bill_text").toString());
            if(id.toInt() > 0) {
                if(painter) painter->drawEllipse(QRectF(r.x() + (r.width() - r.height()) / 2, r.y(), r.height(), r.height()).adjusted(2, 2, -2, -2 ));
                if(painter) painter->drawText(r, Qt::AlignHCenter | Qt::AlignVCenter, id);
            }
            if(painter) painter->save();
            if(painter) painter->setFont(fontFamily(painter->font(), kds::FONT_NUM_Family(), 1.4));
            if(painter) painter->setPen(QColor("white"));
            if(painter) painter->drawText(rectPaintTime(r, painter), Qt::AlignRight | Qt::AlignVCenter, QString().sprintf("%02d:%02d", mins_off / 60, mins_off % 60));
            if(painter) painter->restore();
        }
        //above
        draw_more = false;
        if(k == 0 && row_begin > 0) {
            draw_more = true;
            num_total = 0;
            do {
                if(p.select_value("type").toString() == "item") {
                    num_total += cey_u_orderdish_num_dish(p.select_value("flow_id").toString());
                }
            } while(p.select_previous());

            p.select_seek(row_begin);
        }
        //more
        if(k == row_count - 1 && p.select_has_next()) {
            draw_more = true;
            p.select_previous();
            num_total = 0;
            do {
                if(p.select_value("type").toString() == "item") {
                    num_total += cey_u_orderdish_num_dish(p.select_value("flow_id").toString());
                }
            } while(p.select_next());
        }
        //draw_more
        if(draw_more) {
            if(painter) painter->fillRect(r, QColor(bgcolor));
            if(painter) painter->fillPath(sawtoothPath(r.adjusted(r.width() - sawtooth_right, 1, -flow_id_right, -1), 5), QColor("white"));
            if(painter) painter->fillRect(r.adjusted(1, 1, -sawtooth_right, -1), QColor("white"));

            if(painter) painter->setPen(QColor("black"));
            if(painter && item_more_visible) painter->drawText(r.adjusted(1, 1, -sawtooth_right, -1), Qt::AlignHCenter | Qt::AlignVCenter, "...");
            if(painter) painter->setPen(QColor("white"));
            if(painter) painter->drawText(r.adjusted(r.width() - flow_id_right, 0, 0, 0), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(num_total));
        }
    }

    return row_rect_list;
}

QList<QRectF> kds::paintTableBrief(QPainter *painter, const QModelIndex &index, QRectF rect)
{
    painter->fillRect(rect, QColor(18, 26, 34));
    return paintTableBrief(painter, index.data().toByteArray(), lds::indexRect(rect, index, 10));
}

QPainterPath kds::sawtoothPath(QRectF r, int count)
{
    QPainterPath path;
    count = count * 2;
    path.moveTo(r.x(), r.y());
    for(int k = 1; k <= count; k ++) {
        if(k % 2 == 1) {
            path.lineTo(r.x() + r.width(), r.y() + r.height() / count * k);
        } else {
            path.lineTo(r.x(), r.y() + r.height() / count * k);
        }
    }
    path.closeSubpath();
    return path;
}

bool kds::paint_path_select_next(cJSON_path *p)
{
    while(p->select_next()) {
        if("item" == p->select_value("type").toString()) {
            QString flow_id = p->select_value("flow_id").toString();
            int state = model->tablename_kvmap_value("cey_u_orderdish_print_kt_order", flow_id, "int_flowID", "ch_state").toInt();
            if(state < KDS::cookToken) {
                return true;
            }
        }
        if("title" == p->select_value("type").toString()) {
            return true;
        }
    }
    return false;
}

void kds::kds_model_setdata(const QString &table_name, const QString &key, const QVariant &value)
{
    if(kds::model== 0)
        return;
    kds::model->tablename_kvmap_insert_value(table_name, key, value);
}

QVariant kds::kds_model_data(const QString &table_name, const QString &key, const QVariant &def)
{
    if(kds::model== 0)
        return def;
    if(!kds::model->tablename_key_existed(table_name, key)){
        return def;
    }
    return kds::model->tablename_kvmap_value(table_name, key);
}

QString kds::f_get_sysparm(const QString &as_parameter, const QString &def)
{
    //    kt_kitchen_type = "kitchen_print";//kitchen_view,kitchen_print
    //    kt_kitchen_shop = "fastfdfood";//restaurant,fastfdfood
    //    vch_kitchen_type = "service";//service,kitchen
    //#ifdef QT_DEBUG
    //    if(as_parameter == "kt_kitchen_type") return "kitchen_print";
    //    if(as_parameter == "kt_kitchen_shop") return "restaurant";
    //#endif
    if(kds::model == 0)
        return "";
    return kds::model->tablename_kvmap_value("cey_sys_parameter", as_parameter, "vch_parameter", "vch_value", "", def, true).toString();
}

QVariant kds::cey_bt_kitchen_view_data(const QString &field_name)
{
    if(kds::model == 0) {
        QSqlDatabase db = QSqlDatabase::database(QSqlDatabase::defaultConnection, false);
        if(db.isOpen()) {
            return kds::selectValue(" select " + field_name + " from cey_bt_kitchen_view where vch_terminal = " + sql_string_equal(lds::terminalCode));
        }
        return "";
    }
    //#ifdef QT_DEBUG
    //    if(field_name == "vch_kitchen_type") return "service";
    //#endif
    //    if("vch_plan_id" == field_name && kds::model->tablename_kvmap_field_existed("cey_bt_kitchen_view", lds::terminalCode, "vch_plan_id") == false) {
    //        QString vch_plan_id = selectValue("select vch_plan_id from cey_bt_kitchen_view where vch_terminal = '%1' ", lds::terminalCode).toString();
    //        QString vch_kitchen_type = selectValue("select vch_kitchen_type from cey_bt_kitchen_view where vch_terminal = '%1' ", lds::terminalCode).toString();
    //        lds_query_db query;
    //        if("service" == vch_kitchen_type) {
    //            query.execSelect(QString("select vch_plan_id from cey_bt_kitchen_view where vch_plan_id_service = '%1' ")
    //                             .arg(vch_plan_id));
    //            vch_plan_id = "(";
    //            while(query.next()) {
    //                vch_plan_id += QString(" '%1' ,").arg(query.recordValue("vch_plan_id").toString());
    //            }
    //            vch_plan_id += " null ,";
    //            vch_plan_id.chop(1);
    //            vch_plan_id += ")";
    //        }
    //        if("kitchen" == vch_kitchen_type) {
    //            vch_plan_id = QString("('%1')").arg(vch_plan_id);
    //        }
    //#ifdef QT_DEBUG
    //        vch_plan_id = "('12')";
    //#endif
    //        //创造一个自动刷新的model struct
    //        kds::model->tablename_kvmap_value("cey_bt_kitchen_view", lds::terminalCode, "vch_terminal", field_name);
    //        //~创造一个自动刷新的model struct
    //        kds::model->tablename_kvmap_insert_field("cey_bt_kitchen_view", lds::terminalCode, "vch_plan_id", vch_plan_id);
    //    }
    return kds::model->tablename_kvmap_value("cey_bt_kitchen_view", lds::terminalCode, "vch_terminal", field_name);
}

bool kds::cey_bt_kitchen_view_update(const QString &field_name, const QVariant &value)
{
    lds_query_db query;
    return query.execUpdate("cey_bt_kitchen_view", field_name, value, QString(" vch_plan_id = '%1' ").arg(cey_bt_kitchen_view_data("vch_plan_id").toString()));
}

bool kds::cey_bt_kitchen_view_update_service(const QString &field_name, const QVariant &value)
{
    QString vch_plan_id_service = kds::selectValue(" select vch_plan_id_service from cey_bt_kitchen_view where vch_plan_id = '%1' ", cey_bt_kitchen_view_data("vch_plan_id").toString()).toString();
    lds_query_db query;
    return query.execUpdate("cey_bt_kitchen_view", field_name, value, QString(" vch_plan_id = '%1' ").arg(vch_plan_id_service));
}

void kds::cey_bt_kitchen_view_clear()
{
    kds::model->tablename_kvmap_delete("cey_bt_kitchen_view");
}

void kds::voice_play()
{
    if("Y" == kds::cey_bt_kitchen_view_data("ch_voice_count")) {
        if(false == kds::voice_play_timer.isValid()
                || kds::voice_play_timer.elapsed() / 1000 > 15) {
            kds::voice_play_timer.start();
        } else {
            return;
        }
        //
        for(int k = 0; k < kds::cey_bt_kitchen_view_data("int_voice_count").toInt(); k ++) {
            kds::mediaPlayer->play("userdata/kds/ogg/", QStringList() << "new_order.mp3");
        }
    }
}

double kds::cey_u_orderdish_num_dish(const QString &flow_id, bool clear)
{
    if(clear) {
        model->tablename_kvmap_delete("cey_u_orderdish");
    }
    return
            model->tablename_kvmap_value("cey_u_orderdish", flow_id, "int_flowID", "num_num").toDouble() -
            model->tablename_kvmap_value("cey_u_orderdish", flow_id, "int_flowID", "num_back").toDouble();
}

QString kds::sql_plan_service_in()
{
    QString vch_plan_id = selectValue("select vch_plan_id from cey_bt_kitchen_view where vch_terminal = '%1' ", lds::terminalCode).toString();
    QString vch_kitchen_type = selectValue("select vch_kitchen_type from cey_bt_kitchen_view where vch_terminal = '%1' ", lds::terminalCode).toString();
    lds_query_db query;
    if("service" == vch_kitchen_type) {
        query.execSelect(QString("select vch_plan_id from cey_bt_kitchen_view where vch_plan_id_service = '%1' ")
                         .arg(vch_plan_id));
        vch_plan_id = "(";
        while(query.next()) {
            vch_plan_id += QString(" '%1' ,").arg(query.recordValue("vch_plan_id").toString());
        }
        vch_plan_id += " null ,";
        vch_plan_id.chop(1);
        vch_plan_id += ")";

        return vch_plan_id;
    }
    return "";
}

QString kds::sql_string_equal(int value)
{
    return QString(" '%1' ").arg(value);
}

QString kds::sql_string_equal(const QString &value)
{
    return QString(" '%1' ").arg(value);
}

QString kds::sql_plan_kitchen_equal()
{
    return QString(" '%1' ").arg(cey_bt_kitchen_view_data("vch_plan_id").toString());
}

QFont kds::fontPixelSize(const QFont &font, int pixel)
{
    QFont f = font;
    f.setPixelSize(pixel);
    return f;
}

QFont kds::fontScaled(const QFont &font, qreal scale)
{
    QFont f = font;
    f.setPointSizeF(f.pointSizeF() * scale);
    return f;
}

QFont kds::fontFamily(const QFont &font, const QString &family, qreal scale)
{
    QFont f = font;
    f.setFamily(family);
    f.setPixelSize(f.pixelSize() * scale);
    return f;
}

QRectF kds::rectPaintTime(const QRectF &rect, QPainter *painter)
{
    if(painter == 0) {
        return QRectF();
    }
    QFontMetricsF fm(painter->font());
    qreal w = fm.width("9999:99");
    qreal h = fm.height();
    qreal y_off = (rect.height() - h) / 2.0;
    return QRectF(rect.x() + rect.width() - 5 - w, rect.y() + y_off, w, h);
}

int kds::flowIdRight(const QRectF &rect)
{
    return rect.width() / 7;
}

int kds::sawtoothRight(const QRectF &rect)
{
    return flowIdRight(rect) + 5;
}
