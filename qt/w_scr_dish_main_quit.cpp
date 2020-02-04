#include "w_scr_dish_main_quit.h"
#include "ui_w_scr_dish_main_quit.h"
#include "lds_doublespinbox.h"
#include "lds_tableview_delegate_check.h"
#include "lds_messagebox.h"
#include "lds_dialog_input.h"
#include "transparentcenterdialog.h"
#include "ui_lds_dialog_input_dialog.h"
#include "fexpandmain_2.h"
#include <QTimer>
#include "w_bt_dish_flag.h"

w_scr_dish_main_quit::w_scr_dish_main_quit(const fexpandmain_model_sqltablemodel_data *tablemodel, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_main_quit)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("退货"));
    ui->lineEdit_vch_dishname->setEnabled(false);
    ui->doubleSpinBox_num_dish_org->setEnabled(false);
    this->tablemodel = tablemodel;

    standardmodel = new lds_model_sqlstandardmodel(this);
    standardmodel->setHorizontalHeaderLabels(QStringList() << "ch_state" << "vch_dishname" <<  "vch_dishname_org" <<  "ch_dishno" << "num_price" << "tablemodel_row" << "ch_type" << "num_dish" << "num_dish_org");
    standardmodel->save_set_header("ch_state", true);
    ui->tableView->setModel(standardmodel);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setTransferHheader();
    ui->tableView->justShowColumns(QList<int>() << standardmodel->fieldIndex("ch_state") << standardmodel->fieldIndex("vch_dishname") << standardmodel->fieldIndex("num_dish"));
    ui->tableView->setColumnWidth(standardmodel->fieldIndex("ch_state"), 80);
    ui->tableView->setColumnWidth(standardmodel->fieldIndex("vch_dishname"), 200);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setItemDelegateForColumn(standardmodel->fieldIndex("ch_state"), new lds_tableview_delegate_check(standardmodel->fieldIndex("ch_state"), ui->tableView, true));
    //
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_clearAll, SIGNAL(clicked()), this, SLOT(toClearAll()));
    connect(ui->pushButton_selectAll, SIGNAL(clicked()), this, SLOT(toSelectAll()));
    connect(ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(toUpdateRow(QModelIndex)));
    connect(ui->doubleSpinBox_num_dish, SIGNAL(valueChanged(double)), this, SLOT(toUpdateTableNum(double)));
    connect(ui->pushButton_down,SIGNAL(clicked()), ui->tableView, SLOT(toPageDown()));
    connect(ui->pushButton_up,SIGNAL(clicked()), ui->tableView, SLOT(toPageUp()));
    connect(ui->pushButton_quit_bill,SIGNAL(clicked()), this, SLOT(toQuitBill()));
    connect(ui->pushButton_quit_bill_kitchen_view,SIGNAL(clicked()), this, SLOT(toquit_bill_kitchen_view()));

    lds::showWidget(ui->pushButton_quit_bill_kitchen_view, n_func::f_get_sysparm("kt_kitchen_type") == "kitchen_view");

    refresh();
}

w_scr_dish_main_quit::~w_scr_dish_main_quit()
{
    delete ui;
}

void w_scr_dish_main_quit::row_data_map2List(QList<num_backData> &list_quit, QMap<int, qrtVariantPairList> &list_insert)
{
    list_quit.clear();
    list_insert.clear();
    QDateTime dt = lds_query_hddpos::selectNow();

    //
    for(int row = tablemodel->rowCount() - 1; row >= 0; row --) {
        if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert) {
            list_insert.insert(row, tablemodel->model_data_pairlist(row));
        }
    }
    //
    for(QMap<int, quitData>::const_iterator k = row_data_map.begin(); k != row_data_map.end(); k ++) {
        int row = k.key();
        const quitData &dish = k.value();
        if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert) {
            double num_num = tablemodel->model_data(row, "num_num").toDouble()  - dish.num_dish;
            double num_back = tablemodel->model_data(row, "num_back").toDouble();
            vch_print_memoData print_memo(tablemodel->model_data(row, "vch_print_memo").toString());
            print_memo.sub(dish.d);
            if((num_num - num_back) == 0 && print_memo.count() == 0) {
                list_insert.remove(row);
            } else {
                lds_query_hddpos::replace(list_insert[row], "num_num", num_num);
                lds_query_hddpos::replace(list_insert[row], "vch_print_memo", print_memo.sqlValue());
                lds_query_hddpos::replace(list_insert[row], "num_price_add", print_memo.total());
            }
        } else {
            num_backData d;
            d.int_order_flow_id =  tablemodel->model_data(row, "int_flowid").toLongLong();
            d.num_back_add = dish.num_dish;//tablemodel->model_data_NIBOLAN(row, "num_num-num_back");
            d.dt_back_operdate = dt;
            d.vch_back_operid = lds::gs_operid;
            d.vch_reason = ui->lineEdit_vch_reason->text();
            d.vch_print_memo = dish.d;
            d.int_quit_flow_id = dish.int_quit_flow_id;
            d.vch_print_memo_remain = vch_print_memoData(tablemodel->model_data(row, "vch_print_memo").toString()).sub(dish.d);

            list_quit << d;
        }
    }
}

void w_scr_dish_main_quit::row_data_map_print()
{
    for(QMap<int, quitData>::iterator k = row_data_map.begin(); k != row_data_map.end(); k++ ) {
        qDebug() << k.value().num_dish;
        k.value().d.print();
    }
}

bool w_scr_dish_main_quit::row_data_map_update(qlonglong int_flowid, qlonglong &int_quit_flow_id)
{
    for(QMap<int, quitData>::iterator k = row_data_map.begin(); k != row_data_map.end(); k++ ) {
        if(tablemodel->model_data(k.key(), "int_flowid").toLongLong() == int_flowid) {
            row_data_map[k.key()].int_quit_flow_id = int_quit_flow_id;
            return true;
        }
    }
    return false;
}

bool w_scr_dish_main_quit::dishquit_1_only(QString &errstring, const num_backData &d)
{
    if(w_bt_dish_back::static_toquit_1_only(errstring, d)) {
        row_data_map_update(d.int_order_flow_id, d.int_quit_flow_id);
        return true;
    }
    return false;
}

void w_scr_dish_main_quit::took()
{
    if(!make_row_data_map())
        return;
    if(!commit())
        return;
    this->accept();
}

void w_scr_dish_main_quit::tocancel()
{
    this->reject();
}

void w_scr_dish_main_quit::toClearAll()
{
    for(int row = 0; row < standardmodel->rowCount(); row ++) {
        standardmodel->model_data_set(row, "ch_state", "N");
    }
}

void w_scr_dish_main_quit::toSelectAll()
{
    for(int row = 0; row < standardmodel->rowCount(); row ++) {
        standardmodel->model_data_set(row, "ch_state", "Y");
    }
}

void w_scr_dish_main_quit::toQuitBill()
{
    row_data_map.clear();
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("是否整单退货？"),QObject::tr("确认"),QObject::tr("取消"))) {
        return;
    }
    //fill row_data_map
    for(int row = 0; row < standardmodel->rowCount(); row ++) {
        rowType ch_type = (rowType)standardmodel->model_data(row, "ch_type").toInt();
        double num_dish = standardmodel->model_data(row, "num_dish_org").toDouble();
        int tablemodel_row = standardmodel->model_data(row, "tablemodel_row").toInt();

        if(false == row_data_map.contains(tablemodel_row)) {
            row_data_map.insert(tablemodel_row, quitData());
        }

        if(rowDish == ch_type) {
            row_data_map[tablemodel_row].num_dish = num_dish;
        }
        if(rowCook == ch_type) {
            row_data_map[tablemodel_row].d.append(
                        standardmodel->model_data(row, "ch_dishno").toString(),
                        standardmodel->model_data(row, "vch_dishname_org").toString(),
                        standardmodel->model_data(row, "num_dish").toDouble(),
                        standardmodel->model_data(row, "num_price").toDouble()
                        );
        }
    }
    if(!commit())
        return;
    this->accept();
}

void w_scr_dish_main_quit::toquit_bill_kitchen_view()
{
    if(!make_row_data_map()) {
        return;
    }
    lds_query_hddpos query;
    QList<num_backData> list_quit;
    QString vch_plan_k3o3_id;
    QString vch_plan_id;
    QString vch_memo;
    QMap<int, qrtVariantPairList> map_insert;
    QList<QPair<num_backData, qrtVariantPairList> > backqueue;
    QString int_order_flow_id;
    row_data_map2List(list_quit, map_insert);
    QString ch_billno = tablemodel->cur_ch_billno();

    //    lds_dialog_input inputdialog(this);
    //    inputdialog.setWindowTitle(QObject::tr("点单退货"));
    //    inputdialog.hideSecondLineEdt();
    //    inputdialog.hideKey();
    //    inputdialog.ui->label->setText(QObject::tr("退货原因"));
    //    if(QDialog::Rejected == transparentCenterDialog(&inputdialog).exec()) {
    //        return;
    //    }
    //    vch_memo = inputdialog.ui->lineEdit->text().trimmed();
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("点单退货"), QObject::tr("确定"), QObject::tr("取消"))) {
        return;
    }

    lds_query_hddpos::tran_saction();
    if(!query.execDeleteSelectKey("cey_u_orderdish_print_kt_quit_queue", QString(" select int_flowID as int_orderflow from cey_u_orderdish where ch_billno= '%1' ").arg(ch_billno))) {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.errorString());
        return;
    }
    foreach(const num_backData &d, list_quit) {
        vch_plan_k3o3_id = lds_query_hddpos::selectValue(QString(" select vch_plan_k3o3_id from cey_u_orderdish where int_flowID = %1 ").arg(d.int_order_flow_id)).toString();
        int k = 0;
        //        for(int k = 0; k < 3; k ++)
        {
            vch_plan_id = vch_plan_k3o3_id.mid(k * 2, 2);
            if(!lds_query_hddpos::selectHasNext(QString(" select 1 from cey_bt_kitchen_view where vch_plan_id = '%1' and vch_kitchen_type = 'kitchen' ").arg(vch_plan_id))) {
                continue;
            }
            backqueue.append(qMakePair(d, qrtVariantPairList()
                                       <<  qrtVariantPair("vch_plan_id", vch_plan_id)
                                       <<  qrtVariantPair("int_orderflow", d.int_order_flow_id)
                                       <<  qrtVariantPair("ch_state", KDS::quit_state_wait)
                                       <<  qrtVariantPair("vch_memo", vch_memo)
                                       <<  qrtVariantPair("vch_back_operID", d.vch_back_operid)
                                       <<  qrtVariantPair("vch_print_memo", d.vch_print_memo.sqlValue())
                                       <<  qrtVariantPair("num_back", d.num_back_add)
                                       ));
            if(!query.execInsert("cey_u_orderdish_print_kt_quit_queue", backqueue.last().second)) {
                lds_query_hddpos::roll_back();
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.errorString());
                return;
            }
            if(!query.execUpdate("cey_bt_kitchen_view", "ch_refresh_order_quit", "Y", QString( "vch_plan_id = '%1' ").arg(vch_plan_id))) {
                lds_query_hddpos::roll_back();
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.errorString());
                return;
            }
        }
    }
    lds_query_hddpos::com_mit();


    QString errstring;
    QString message;
    QString ch_dishno;
    QString num_back;
    vch_print_memoData printmemod;
    QList<int> success_list;
    QList<int> refuse_list;
    lds_messagebox_loading_show loading(this, "");
    loading.addButton(QObject::tr("取消"));
    loading.showMessage(ui->pushButton_quit_bill_kitchen_view->text() + "/wait...");
    loading.show();
    QEventLoop loop;
    connect(&loading, SIGNAL(signal_btn_clicked(int)), &loop, SLOT(quit()));
    forever {
        message = "";
        success_list.clear();
        refuse_list.clear();
        for(int k = 0; k < backqueue.count(); k ++) {
            const num_backData &backqueue_d = backqueue[k].first;
            const qrtVariantPairList &backqueue_p = backqueue[k].second;
            message += "<tr>";
            num_back = qrtVariant::pairListValue(backqueue_p, "num_back").toString();
            vch_plan_id = qrtVariant::pairListValue(backqueue_p, "vch_plan_id").toString();
            int_order_flow_id = qrtVariant::pairListValue(backqueue_p, "int_orderflow").toString();
            ch_dishno = lds_query_hddpos::selectValue(QString(" select ch_dishno from cey_u_orderdish where int_flowID = '%1' ")
                                                      .arg(qrtVariant::pairListValue(backqueue_p, "int_orderflow").toString())).toString();
            message += QString("<td width=\"100\"  ><p align=\"left\" >%1</p></td>").arg(
                        lds_query_hddpos::selectValue(QString(" select vch_dishname from cey_bt_dish where ch_dishno = '%1' ").arg(ch_dishno)).toString());
            printmemod.select(qrtVariant::pairListValue(backqueue_p, "vch_print_memo").toString());
            message += QString("<td width=\"100\"  ><p align=\"center\" >%1</p></td>").arg(
                        printmemod.jsonToString(1, false));
            message += QString("<td width=\"100\"  ><p align=\"center\" >%1</p></td>").arg(
                        "x" + num_back);
            int ch_state = lds_query_hddpos::selectValue(QString(" select ch_state from cey_u_orderdish_print_kt_quit_queue where int_orderflow = %1 ")
                                                         .arg(int_order_flow_id)).toInt();
            switch(ch_state) {
            case KDS::quit_state_wait://等待退货
                message += QString("<td width=\"100\"  ><p align=\"center\" >%1</p></td>").arg(
                            KDS::quitStateName(ch_state)
                            );
                break;
                //
            case KDS::quit_state_processing://正在退货
                query.db.transaction();
                if(dishquit_1_only(errstring, backqueue_d)) {
                    query.execUpdate("cey_u_orderdish_print_kt_quit_queue", "ch_state", KDS::quit_state_finish, "int_orderflow=" + int_order_flow_id);
                } else {
                    query.execUpdate("cey_u_orderdish_print_kt_quit_queue", "ch_state", KDS::quit_state_error, "vch_memo", errstring, "int_orderflow=" + int_order_flow_id);
                }
                query.db.commit();

                message += QString("<td width=\"100\"  ><p align=\"center\" >%1</p></td>").arg(
                            KDS::quitStateName(ch_state)
                            );
                break;
            case KDS::quit_state_finish://退货完成
                success_list << k;
                message += QString("<td width=\"100\"  ><p align=\"center\" ><span style=\" color:green;\">%1</span></p></td>").arg(
                            KDS::quitStateName(ch_state)
                            );
                break;
            case KDS::quit_state_rejected://退货拒绝
                refuse_list << k;
                message += QString("<td width=\"100\"  ><p align=\"center\" ><span style=\" color:red;\">%1</span></p></td>").arg(
                            KDS::quitStateName(ch_state) + ":" + lds_query_hddpos::selectValue(QString(" select vch_memo from cey_u_orderdish_print_kt_quit_queue where  int_orderflow = %1")
                                                                                               .arg(int_order_flow_id)).toString()

                            );
                break;
            case KDS::quit_state_error://退货报错
                message += QString("<td width=\"100\"  ><p align=\"center\" ><span style=\" color:red;\">%1</span></p></td>").arg(
                            KDS::quitStateName(ch_state) + ":" + lds_query_hddpos::selectValue(QString(" select vch_memo from cey_u_orderdish_print_kt_quit_queue where  int_orderflow = %1 ")
                                                                                               .arg(int_order_flow_id)).toString()

                            );
                break;
            }
            message += "</tr>";
        }
        loading.showMessage("<table>" + message + "</table>");
        QTimer::singleShot(1000, &loop, SLOT(quit()));
        loop.exec();
        //
        if(loading.result() == 0) {// click button, -1 is nothing
            break;
        }
        if(success_list.count() + refuse_list.count() > 0) {
            loading.setButtonText(0, QObject::tr("确定"));
        }
        if(success_list.count() + refuse_list.count() == backqueue.count()) {
            loading.setButtonText(0, QObject::tr("完成"));
        }
    }//forever

    lds_query_hddpos::tran_saction();
    for(int k = 0; k < backqueue.count(); k ++) {
        const qrtVariantPairList &backqueue_p = backqueue[k].second;

        if(!query.execDelete("cey_u_orderdish_print_kt_quit_queue", "int_orderflow ="+ qrtVariant::pairListValue(backqueue_p, "int_orderflow").toString())) {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.errorString());
            return;
        }
    }
    lds_query_hddpos::com_mit();
    if(success_list.count() > 0)
        this->accept();
    else
        this->reject();
    //发起请求,若是重复请求则提示并返回
    //发起请求,若超时则删除本次请求
    //正常响应后,由kt负责退菜
    //退菜成功,餐饮刷新界面,删除本次请求
    //批量处理时,先设置的厨显A,然后A改成出品,但是菜品的厨显还是A,如何避免
    //出品厨显,不能退单
    //plan_id 从厨显改成出品会破坏[消息只能发给厨显]的情况,如何避免
}

void w_scr_dish_main_quit::toUpdateRow(const QModelIndex &index)
{
    ui->lineEdit_vch_dishname->setText(standardmodel->model_data(index.row(), "vch_dishname_org").toString());

    ui->doubleSpinBox_num_dish_org->setValue(standardmodel->model_data(index.row(), "num_dish_org").toDouble());
    ui->doubleSpinBox_num_dish->setRange(0, ui->doubleSpinBox_num_dish_org->value());

    double value = standardmodel->model_data(index.row(), "num_dish").toDouble();
    ui->doubleSpinBox_num_dish->setValue(value );
    ui->doubleSpinBox_num_dish->setFocus();
}

void w_scr_dish_main_quit::toUpdateTableNum(double num_dish)
{
    standardmodel->model_data_set(ui->tableView->currentIndex().row(), "num_dish", num_dish);
}

void w_scr_dish_main_quit::refresh()
{
    standardmodel->removeRows(0, standardmodel->rowCount());
    //
    for(int row = 0; row < tablemodel->rowCount(); row ++) {
        vch_print_memoData d(tablemodel->model_data(row, "vch_print_memo").toString());
        if(d.count() == 0 && tablemodel->model_data_NIBOLAN(row, "num_num-num_back") == 0) {
            continue;
        }

        int cur_row;
        cur_row = standardmodel->rowCount();
        standardmodel->insertRow(cur_row);

        standardmodel->model_data_set(cur_row, "ch_state", "N");
        standardmodel->model_data_set(cur_row, "vch_dishname", tablemodel->get_cey_bt_dish_value(tablemodel->model_data(row, "ch_dishno").toString(), "vch_dishname"));
        standardmodel->model_data_set(cur_row, "vch_dishname_org", tablemodel->get_cey_bt_dish_value(tablemodel->model_data(row, "ch_dishno").toString(), "vch_dishname"));
        standardmodel->model_data_set(cur_row, "ch_dishno", tablemodel->model_data(row, "ch_dishno").toString());
        standardmodel->model_data_set(cur_row, "tablemodel_row", row);
        standardmodel->model_data_set(cur_row, "ch_type", rowDish);
        standardmodel->model_data_set(cur_row, "num_price", tablemodel->model_data(row, "num_price"));
        standardmodel->model_data_set(cur_row, "num_dish", tablemodel->model_data_NIBOLAN(row, "num_num-num_back"));
        standardmodel->model_data_set(cur_row, "num_dish_org", tablemodel->model_data_NIBOLAN(row, "num_num-num_back"));

        while(d.next()) {
            cur_row = standardmodel->rowCount();
            standardmodel->insertRow(cur_row);

            standardmodel->model_data_set(cur_row, "ch_state", "N");
            standardmodel->model_data_set(cur_row, "vch_dishname", QObject::tr("做法") + ":" + d.nextValue("name").toString());
            standardmodel->model_data_set(cur_row, "vch_dishname_org", d.nextValue("name").toString());
            standardmodel->model_data_set(cur_row, "ch_dishno", d.nextValue("no"));
            standardmodel->model_data_set(cur_row, "tablemodel_row", row);
            standardmodel->model_data_set(cur_row, "ch_type", rowCook);
            standardmodel->model_data_set(cur_row, "num_price", d.nextValue("price"));
            standardmodel->model_data_set(cur_row, "num_dish", d.nextValue("num"));
            standardmodel->model_data_set(cur_row, "num_dish_org", d.nextValue("num"));
        }
        //
    }
    //
    ui->tableView->selectRow(0);
    toUpdateRow(ui->tableView->currentIndex());
}

bool w_scr_dish_main_quit::commit()
{
    QString errstring;
    QList<num_backData> list_quit;
    QMap<int, qrtVariantPairList> map_insert;
    row_data_map2List(list_quit, map_insert);
    //
    lds_query_hddpos::tran_saction();
    int index;
    for(index = 0; index < list_quit.count(); index ++) {
        if(!dishquit_1_only(errstring, list_quit[index])) {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
            return false;
        }
    }

    lds_query_hddpos::com_mit();
    return true;
}

bool w_scr_dish_main_quit::make_row_data_map()
{
    double num_num = 0;
    row_data_map.clear();

    //fill row_data_map
    for(int row = 0; row < standardmodel->rowCount(); row ++) {
        if(standardmodel->model_data(row, "ch_state").toString() == "Y") {
            rowType ch_type = (rowType)standardmodel->model_data(row, "ch_type").toInt();
            double num_dish = standardmodel->model_data(row, "num_dish").toDouble();
            int tablemodel_row = standardmodel->model_data(row, "tablemodel_row").toInt();
            num_num += standardmodel->model_data(row, "num_dish").toDouble();

            if(false == row_data_map.contains(tablemodel_row)) {
                row_data_map.insert(tablemodel_row, quitData());
            }

            if(rowDish == ch_type) {
                row_data_map[tablemodel_row].num_dish = num_dish;
            }
            if(rowCook == ch_type) {
                row_data_map[tablemodel_row].d.append(
                            standardmodel->model_data(row, "ch_dishno").toString(),
                            standardmodel->model_data(row, "vch_dishname_org").toString(),
                            standardmodel->model_data(row, "num_dish").toDouble(),
                            standardmodel->model_data(row, "num_price").toDouble()
                            );
            }
        }
    }

    if(num_num == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return false;
    }

    //若需要退所有商品时，做法也要全部退掉
    QStringList unfull_name_list;
    for(QMap<int, quitData>::iterator k = row_data_map.begin(); k != row_data_map.end(); k++) {
        int tablemodel_row = k.key();
        quitData & dish = k.value();

        if(dish.num_dish == tablemodel->model_data_NIBOLAN(tablemodel_row, "num_num-num_back")) {
            vch_print_memoData vch_print_memo(tablemodel->model_data(tablemodel_row, "vch_print_memo").toString());
            vch_print_memo.sub(dish.d);

            if(vch_print_memo.num() != 0) {
                unfull_name_list << tablemodel->get_cey_bt_dish_value(tablemodel->model_data(tablemodel_row, "ch_dishno").toString(), "vch_dishname").toString() + ",";
                dish.d.select(tablemodel->model_data(tablemodel_row, "vch_print_memo").toString());
            }
        }
    }

    if(unfull_name_list.count() > 0) {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, unfull_name_list.join(",") + "\n" + QObject::tr("做法将会全部退掉"), QObject::tr("确定"), QObject::tr("取消"))) {
            return false;
        }
    }
    return true;
}
