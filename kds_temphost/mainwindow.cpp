#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cjson_help.h"
#include <QtDebug>
#include "lds_query_db.h"
#include "lds_query_hddpos.h"
#include "w_bt_dish_kitchen_view_plan.h"
#include "kds_dishtype_setting.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tcpserver = new QTcpServer(this);
    qDebug() << __LINE__ << "listen:" << tcpserver->listen(QHostAddress::Any, 9100);
    connect(tcpserver, SIGNAL(newConnection()), this, SLOT(toTcpConnect()));
    ui->pushButton_kitchen_settings->setIcon(QPixmap(":/image/login_image/bg2.jpg"));
    qDebug() << __LINE__ << QPixmap(":/image/login_image/bg2.jpg").isNull();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//!指定的类别->厨显方案
//!类别不存在->厨显方案

bool MainWindow::dishOrder(const QString &json, QString &errstring)
{
    cJSON_path p(json.toLocal8Bit());
    lds_query_hddpos query;
    vch_print_memoData printd;
    lds_query_hddpos::tran_saction();
    if(!query.execInsertDuplicateUpdate("cey_bt_table", "ch_tableno", qrtVariantPairList()
                                        << qrtVariantPair("ch_billno", p.path_value("{bill_no"))
                                        << qrtVariantPair("ch_tableno", p.path_value("{table_no"))
                                        << qrtVariantPair("vch_tablename", p.path_value("{table_name"))
                                        )) {
        errstring = query.errorString();
        lds_query_hddpos::roll_back();
        return false;
    }
    if(!query.execInsertDuplicateUpdate("cey_u_table", "ch_billno", qrtVariantPairList()
                                        << qrtVariantPair("ch_billno", p.path_value("{bill_no"))
                                        << qrtVariantPair("ch_tableno", p.path_value("{table_no"))
                                        << qrtVariantPair("vch_memo", p.path_value("{bill_memo"))
                                        << qrtVariantPair("dt_operdate", p.path_value("{bill_time"))
                                        )) {
        errstring = query.errorString();
        lds_query_hddpos::roll_back();
        return false;
    }
    if(!query.execInsertDuplicateUpdate("cey_u_master", "ch_billno", qrtVariantPairList()
                                        << qrtVariantPair("ch_billno", p.path_value("{bill_no"))
                                        << qrtVariantPair("vch_memo", p.path_value("{bill_serial"))
                                        )) {
        errstring = query.errorString();
        lds_query_hddpos::roll_back();
        return false;
    }
    for(int k = 0; k < p.array_count("{dish_list"); k ++) {
        if(!query.execInsertDuplicateUpdate("cey_bt_dish", "ch_dishno", qrtVariantPairList()
                                            << qrtVariantPair("ch_dishno", p.array_value("{dish_list[%1{no", k))
                                            << qrtVariantPair("vch_dishname", p.array_value("{dish_list[%1{name", k))
                                            )) {
            errstring = query.errorString();
            lds_query_hddpos::roll_back();
            return false;
        }
        printd.clear();
        p.select_array(QString("{dish_list[%1{item_list").arg(k));
        while(p.select_next()) {
            if(!query.execInsertDuplicateUpdate("cey_bt_cp_memo", "ch_no", qrtVariantPairList()
                                                << qrtVariantPair("ch_no", p.select_value("no"))
                                                << qrtVariantPair("vch_cp_memo", p.select_value("name"))
                                                << qrtVariantPair("num_add_price", p.select_value("price")))
                    ) {
                errstring = query.errorString();
                lds_query_hddpos::roll_back();
                return false;
            }
            printd.duplicate(query.db, p.select_value("no").toString(), p.select_value("num").toDouble());
        }
        if(!query.execInsert("cey_u_orderdish", qrtVariantPairList()
                             << qrtVariantPair("int_id", p.array_value("{dish_list[%1{row", k))
                             << qrtVariantPair("ch_billno", p.path_value("{bill_no"))
                             << qrtVariantPair("ch_tableno", p.path_value("{table_no"))
                             << qrtVariantPair("ch_dishno", p.array_value("{dish_list[%1{no", k))
                             << qrtVariantPair("num_num", p.array_value("{dish_list[%1{num", k))
                             << qrtVariantPair("num_back", 0)
                             << qrtVariantPair("vch_print_memo", printd.jsonToString())
                             << qrtVariantPair("num_price_add", printd.total())
                             )) {
            errstring = query.errorString();
            lds_query_hddpos::roll_back();
            return false;
        }
    }
    lds_query_hddpos::com_mit();
    return true;
}

bool MainWindow::dishReturn(const QString &json, QString &errstring, QTcpSocket *tcp)
{
    cJSON_path p(json.toLocal8Bit());
    if(p.isNull())
        return false;
    QString ch_billno = p.path_value("{bill_no").toString();
    QList<num_backData> list_quit = fillBackinfo(&p);
    QString vch_plan_k3o3_id;
    QString vch_plan_id;
    QString int_order_flow_id;
    QString num_back;
    QString ch_dishno;
    QString int_id;
    QList<QPair<num_backData, qrtVariantPairList> > backqueue;

    lds_query_hddpos query;
    lds_query_hddpos::tran_saction();
    if(!query.execDeleteSelectKey("cey_u_orderdish_print_kt_quit_queue", QString(" select int_flowID as int_orderflow from cey_u_orderdish where ch_billno= '%1' ").arg(ch_billno))) {
        lds_query_hddpos::roll_back();
        errstring = query.errorString();
        return false;
    }
    foreach(const num_backData &d, list_quit) {
        vch_plan_k3o3_id = query.pathValue(QString("cey_u_orderdish/vch_plan_k3o3_id:int_flowID = %1 ").arg(d.int_order_flow_id)).toString();
        vch_plan_id = vch_plan_k3o3_id.left(2);
        if(!query.pathExisted("cey_bt_kitchen_view/vch_plan_id:vch_plan_id = '%1' and vch_kitchen_type = 'kitchen' ", vch_plan_id)) {
            continue;
        }
        backqueue.append(qMakePair(d, qrtVariantPairList()
                                   <<  qrtVariantPair("vch_plan_id", vch_plan_id)
                                   <<  qrtVariantPair("int_orderflow", d.int_order_flow_id)
                                   <<  qrtVariantPair("ch_state", KDS::quit_state_wait)
                                   <<  qrtVariantPair("vch_memo", d.vch_reason)
                                   <<  qrtVariantPair("vch_back_operID", d.vch_back_operid)
                                   <<  qrtVariantPair("vch_print_memo", d.vch_print_memo.sqlValue())
                                   <<  qrtVariantPair("num_back", d.num_back_add)
                                   ));
        if(!query.execInsert("cey_u_orderdish_print_kt_quit_queue", backqueue.last().second)) {
            lds_query_hddpos::roll_back();
            errstring = query.errorString();
            return false;
        }
        if(!query.execUpdate("cey_bt_kitchen_view", "ch_refresh_order_quit", "Y", QString( "vch_plan_id = '%1' ").arg(vch_plan_id))) {
            lds_query_hddpos::roll_back();
            errstring = query.errorString();
            return false;
        }
    }
    lds_query_hddpos::com_mit();
    //
//    forever {
    int max_try_time = 10;
    for(;max_try_time > 0; max_try_time --) {
        QEventLoop loop;
        QList<int> success_list;
        QList<int> refuse_list;
        for(int k = 0; k < backqueue.count(); k ++) {
            const num_backData &backqueue_d = backqueue[k].first;
            const qrtVariantPairList &backqueue_p = backqueue[k].second;
            num_back = qrtVariant::pairListValue(backqueue_p, "num_back").toString();
            vch_plan_id = qrtVariant::pairListValue(backqueue_p, "vch_plan_id").toString();
            int_order_flow_id = qrtVariant::pairListValue(backqueue_p, "int_orderflow").toString();
            ch_dishno = query.pathValue("cey_u_orderdish/ch_dishno:int_flowID=%1", int_order_flow_id).toString();
            int_id = query.pathValue("cey_u_orderdish/int_id:int_flowID=%1", int_order_flow_id).toString();
            int ch_state = query.pathValue("cey_u_orderdish_print_kt_quit_queue/ch_state:int_orderflow=%1", int_order_flow_id).toInt();
            QString vch_memo = query.pathValue("cey_u_orderdish_print_kt_quit_queue/vch_memo:int_orderflow=%1", int_order_flow_id).toString();
            if(ch_state != qrtVariant::pairListValue(backqueue_p, "ch_state").toInt()) {
                qrtVariant::pairSetValue(backqueue[k].second, "ch_state", ch_state);
                tcpWriteString(tcp, "{\"row\":" + int_id +",\"state\":" + QString::number(ch_state) + ",\"state_desc\":\"" + KDS::quitStateName(ch_state) + "\"," + "\"message\":\"" + vch_memo + "\"}");
            }
            tcpWriteString(tcp, QString("%1/%2").arg(__FILE__).arg(__LINE__));

            switch(ch_state) {
            case KDS::quit_state_wait://等待退货
                break;
                //
            case KDS::quit_state_processing://正在退货
                lds_query_hddpos::tran_saction();
                if(w_bt_dish_back::static_toquit_1_only(errstring, backqueue_d)) {
                    query.execUpdate("cey_u_orderdish_print_kt_quit_queue", "ch_state", KDS::quit_state_finish, "int_orderflow=" + int_order_flow_id);
                } else {
                    query.execUpdate("cey_u_orderdish_print_kt_quit_queue", "ch_state", KDS::quit_state_error, "vch_memo", errstring, "int_orderflow=" + int_order_flow_id);
                }
                lds_query_hddpos::com_mit();

                break;
            case KDS::quit_state_finish://退货完成
                success_list << k;

                break;
            case KDS::quit_state_rejected://退货拒绝
                refuse_list << k;

                break;
            case KDS::quit_state_error://退货报错

                break;
            }//switch
        }//for
        QTimer::singleShot(1000, &loop, SLOT(quit()));
        loop.exec();
    }//forever

    if(max_try_time == 0) {
        errstring = "time out";
        return false;
    }
    return true;
}

QList<num_backData> MainWindow::fillBackinfo(cJSON_path *p)
{
    //!默认退单的操作员都是0000
    //!默认退单的时间都是机器当前时间
    QList<num_backData> backlist;
    lds_query_hddpos query;
    for(int k = 0; k < p->array_count("{dish_list"); k ++) {
        QString bill_on = p->path_value("{bill_no").toString();
        QString int_id = p->array_value("{dish_list[%1{row", k).toString();
        QString int_flowid = query.pathValue(QString("cey_u_orderdish/int_flowid:ch_billno='%1' and int_id = %2")
                                             .arg(bill_on).arg(int_id)).toString();
        vch_print_memoData vch_print_memo(query.pathValue("cey_u_orderdish/vch_print_memo:int_flowid='%1' ", int_flowid).toString());
        vch_print_memoData vch_print_memo_quit;
        p->select_array(QString("{dish_list[%1{item_list").arg(k));
        while(p->select_next()) {
            vch_print_memo_quit.duplicate(query.db, p->select_value("no").toString(), p->select_value("num").toDouble());
        }

        num_backData d;
        d.int_order_flow_id =  int_flowid.toLongLong();
        d.num_back_add = p->array_value("{dish_list[%1{num", k).toDouble();
        d.dt_back_operdate = QDateTime::currentDateTime();
        d.vch_back_operid = "0000";
        d.vch_reason = p->path_value("{bill_reason").toString();
        d.vch_print_memo = vch_print_memo;
        d.vch_print_memo_remain = vch_print_memo.sub(vch_print_memo_quit);
        backlist << d;
    }
    return backlist;
}

void MainWindow::toTcpConnect()
{
    while(tcpserver->hasPendingConnections()) {
        QTcpSocket *tcp = tcpserver->nextPendingConnection();
        connect(tcp, SIGNAL(readyRead()), this, SLOT(toTcpDataAppend()));
        showStatusMessage(tcp->peerAddress().toString() + "connect to server");
    }
}

void MainWindow::toTcpDataAppend()
{
    QTcpSocket *tcp = qobject_cast<QTcpSocket *>(this->sender());
    if(tcp == 0)
        return;
    showStatusMessage(tcp->peerAddress().toString() + "'s data received");
    tcpDataMap[tcp] += tcp->readAll();
    if(tcpDataMap[tcp].trimmed().endsWith("}")) {
        QString errstring;
        cJSON_path p(tcpDataMap[tcp]);
        bool flag = false;
        QString reponse_text;
        if(p.isValid()) {
            tcpDataMap[tcp] = "";
            if(p.path_value("{type").toString() == "order") {
                showStatusMessage(tcp->peerAddress().toString() + " dishOrder");
                flag = dishOrder(p.toByteArray(), errstring);
            }
            if(p.path_value("{type").toString() == "return") {
                showStatusMessage(tcp->peerAddress().toString() + " dishReturn");
                flag = dishReturn(p.toByteArray(), errstring, tcp);
            }
            //
            reponse_text = "{\"code\":" + QString::number(flag) + ", \"message\":\"" + errstring + "\"}";
            showStatusMessage(tcp->peerAddress().toString() + " feed back");
            tcp->write(reponse_text.toLocal8Bit());
        }
    }
}

qint64 MainWindow::tcpWriteString(QTcpSocket *tcp, const QString &d)
{
    return tcp->write(d.toLocal8Bit());
}

void MainWindow::showStatusMessage(const QString &message)
{
    QList<QTcpSocket *> list = tcpDataMap.keys();
    int running = 0;
    int stopping = 0;
    for(int k = 0; k < list.count(); k ++) {
        if(list[k]->state() == QTcpSocket::ConnectedState) {
            running ++;
        } else {
            stopping ++;
        }
    }
    QString text;
    text += tcpserver->isListening() ? QObject::tr("正在运行") : QObject::tr("停止运行");
    text += QString("(%1/%2)").arg(running).arg(running + stopping);
    text += "," + message;
    ui->statusBar->showMessage(text);
}

void MainWindow::on_pushButton_kitchen_settings_clicked()
{
    w_bt_dish_kitchen_view_plan dialog(this);
    dialog.exec();
}

void MainWindow::on_pushButton_kitchen_dish_settings_clicked()
{
    kds_dishtype_setting dialog(this);
    dialog.exec();
}

void MainWindow::on_pushButton_exit_clicked()
{
    this->close();
}
