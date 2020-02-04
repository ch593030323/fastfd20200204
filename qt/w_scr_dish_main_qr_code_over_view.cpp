#include "w_scr_dish_main_qr_code_over_view.h"
#include "ui_w_scr_dish_main_qr_code_over_view.h"
#include "lds_query_hddpos.h"
#include <QTimer>
#include "lds_messagebox.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "w_scr_dish_restaurant_dialog.h"
#include "public_sql.h"
#include "lds_roundeddialog_rect_pop.h"
#include "public_printby_ch_billno.h"
#include <QPainter>
#include "w_st_good_input_select_dish.h"
#include "w_scr_dish_main_qr_code_orderdish.h"
#include "w_scr_dish_main_qr_code_over_view_set.h"
#include "public_select_stringlist.h"
#include <QScrollBar>
#include "w_scr_dish_restaurant_div_pop.h"
#include "n_func.h"
#include "w_scr_dish_main_qr_code_over_view_cash_pay.h"
#include "fexpandmain_delegate_vch_printmemo.h"
#include <QMovie>

w_scr_dish_main_qr_code_over_view::w_scr_dish_main_qr_code_over_view(const QString &billType, QWidget *parent) :
    QDialog(parent),
    auto_accept_manual(false),
    billType(billType)
{
    init();
    lds::hideWidget(ui->checkBox, true);
    lds::hideWidget(ui->pushButton_set, true);
    lds::hideWidget(ui->tableView_record, true);
    lds::hideWidget(ui->tableView, false);
    lds::hideWidget(ui->pushButton_more, true);
    pushButton_bill_hide(true);

    refresh0();
}

w_scr_dish_main_qr_code_over_view::w_scr_dish_main_qr_code_over_view(QWidget *parent) :
    QDialog(parent),
    auto_accept_manual(true),
    #ifdef QT_DEBUG
    billType(w_scr_dish_main_qr_code::billTypeAVailable)
  #else
    billType(w_scr_dish_main_qr_code::billTypeAVailable)
  #endif
{
    init();
    view_set_app();

    QTimer::singleShot(100, this, SLOT(refresh0()));
}

w_scr_dish_main_qr_code_over_view::~w_scr_dish_main_qr_code_over_view()
{
    delete ui;
}

void w_scr_dish_main_qr_code_over_view::selectOrderSn(const QString &orderSn)
{
    for(int row = 0; row < standmodel->rowCount(); row ++) {
        if(standmodel->model_data(row, "int_id").toString() == orderSn) {
            ui->tableView->selectRow(row);
        }
    }
}

void w_scr_dish_main_qr_code_over_view::selectTableNo(const QString &tableNo)
{
    for(int row = 0; row < standmodel->rowCount(); row ++) {
        if(standmodel->model_data(row, "ch_tableno").toString() == tableNo) {
            ui->tableView->selectRow(row);
        }
    }
}

void w_scr_dish_main_qr_code_over_view::refresh0()
{
    pushButton_bill_check(billType);
}

void w_scr_dish_main_qr_code_over_view::refresh()
{
    pushButton_bill_check(this->sender());
    //!等待
    loadingRun loading(ui);
    loading.show();
    //!~等待

    QByteArray readall;
    QString errstring;
    int billTypeAVailable_count = 0;
    int billTypeRCV_count = 0;
    int billTypePaid_count = 0;
    int billTypeQuickOrder_count = 0;
    int billTypeYunAddDish_count = 0;

    for(int row = standmodel->rowCount() - 1; row >= 0; row --) {
        delete ui->tableView->indexWidget(standmodel->index(row, standmodel->fieldIndex("do1")));
        delete ui->tableView->indexWidget(standmodel->index(row, standmodel->fieldIndex("do2")));
        delete ui->tableView->indexWidget(standmodel->index(row, standmodel->fieldIndex("do3")));
        standmodel->removeRow(row);
    }

    bool ok;
    QList<QString> include_sn;
    lds_query_hddpos  query;
    query.execSelect(QString(" select b.ch_tableno,  b.vch_tablename, a.vch_qr_code_sn, a.num_cost, a.dt_operdate, a.ch_billno from cey_u_table a, cey_bt_table b where a.ch_billno = b.ch_billno and a.ch_billno like 'R%' ; "));
    while(query.next()) {
        include_sn << query.recordValue("vch_qr_code_sn").toString();
    }
    //3
    QList<w_scr_dish_main_qr_code::QrCodeMaster>  masters;
    ///已经确认
    masters << w_scr_dish_main_qr_code::qr_code_master_get(w_scr_dish_main_qr_code::billTypeRCV, include_sn, &ok);
    if(!ok) {
        loading.setText( QObject::tr("无法获取相关数据"));
        return;
    }
    ///已经支付
    masters << w_scr_dish_main_qr_code::qr_code_master_get(w_scr_dish_main_qr_code::billTypePaid, include_sn, &ok);
    if(!ok) {
        loading.setText( QObject::tr("无法获取相关数据"));
        return;
    }
    ///新的订单
    int masters_k = masters.count();
    masters << w_scr_dish_main_qr_code::qr_code_master_get(w_scr_dish_main_qr_code::billTypeAVailable, &ok);
    if(!ok) {
        loading.setText( QObject::tr("无法获取相关数据"));
        return;
    }
    //校对
    for(int k = masters_k; k < masters.count(); k++) {
        if(lds_query_hddpos::selectValue(QString(" select ch_tableno from cey_bt_table where ch_tableno = '%1' " ).arg(masters[k].tableNo)).isNull()) {
            if(1 == lds_messagebox::warning(this,MESSAGE_TITLE_VOID, QObject::tr("餐桌") + ":" + masters[k].tableNo +  QObject::tr("不存在"), QObject::tr("数据同步"), QObject::tr("取消"))) {
                return;
            } else {
                /*同步*/
                w_sys_manage_cloudsync_with_time_running::exec_keepdata(this);
                k = 0;
                continue;
            }
        }
    }

    ///普通订单
    //filter
    for(int k = masters.count() - 1; k >= 0; k--) {
        if(masters[k].sn.contains(ui->lineEdit_sn_tableNo->text())
                || masters[k].tableNo.contains(ui->lineEdit_sn_tableNo->text()) ) {
        } else {
            masters.removeAt(k);
        }
    }
    //按sn从大到小排序
    qSort(masters.begin(), masters.end(), w_scr_dish_main_qr_code::QrCodeMasterGreaterThan);

    ///先付款订单
    masters << w_scr_dish_main_qr_code::qr_code_bill_get_quick_order_asc(n_func::f_get_sysparm("qr_code_quick_order_cur_sn", "0"), include_sn, ok);

    ///云端添加
    QrCodeMasterDetailsList yun_add_dishes;
    if(false == w_scr_dish_main_qr_code::qr_code_bill_get_yun_add_dish_asc(n_func::f_get_sysparm("qr_code_yun_add_dish_cur_sn", "0"), readall, errstring)) {
        loading.setText(errstring);
        return;
    } else {
        cJSON_help obj(readall);
        cJSON *array = cJSON_help::getcJSON(obj, "data");
        int data_count = cJSON_help::array_count(array);
        QString orgName = cJSON_help::getcJSONvalue(obj, "orgName").toString();

        for(int k = 0; k < data_count; k++) {
            cJSON *item = cJSON_GetArrayItem(array, k);

            w_scr_dish_main_qr_code::QrCodeMasterDetails master_details;
            w_scr_dish_main_qr_code::qr_code_code_resolve_master(item, master_details.master);
            master_details.master.orgName = orgName;
            master_details.master.state = w_scr_dish_main_qr_code::billTypeYunAddDish;

            cJSON *webOrderDish = cJSON_help::getcJSON(item, "webOrderDish");
            int webOrderDish_count = cJSON_help::array_count(webOrderDish);

            for(int m = 0; m < webOrderDish_count; m ++) {
                w_scr_dish_main_qr_code::QrCodeDetail qr_detail;
                w_scr_dish_main_qr_code::qr_code_code_resolve_detail(cJSON_GetArrayItem(webOrderDish, m), qr_detail);
                master_details.details << qr_detail;
            }
            yun_add_dishes << master_details;
        }
    }

    ///
    //
    textEditAppend(true, QString(this->width() / 6, '-'));
    int auto_accept_index = 0;
    for(int k = 0; k < masters.count() + yun_add_dishes.count(); k++) {
        w_scr_dish_main_qr_code::QrCodeMasterDetails master_details;
        if(k < masters.count()) {
            master_details.master = masters[k];
        } else {
            master_details = yun_add_dishes[k - masters.count()];
        }
        if(false == refresh(master_details,
                            auto_accept_index,
                            billTypeAVailable_count,
                            billTypeRCV_count,
                            billTypePaid_count,
                            billTypeQuickOrder_count,
                            billTypeYunAddDish_count
                            )) {
            break;
        }
    }
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setColumnWidth(standmodel->fieldIndex("do1"), 100);
    ui->tableView->setColumnWidth(standmodel->fieldIndex("do2"), 100);
    ui->tableView->setColumnWidth(standmodel->fieldIndex("do3"), 100);

    ui->pushButton_bill_all->setText(QObject::tr("全部订单") + QString("(%1)").arg(billTypeAVailable_count + billTypeRCV_count + billTypePaid_count + billTypeQuickOrder_count + billTypeYunAddDish_count));
    ui->pushButton_bill_new->setText(QObject::tr("新的订单") + QString("(%1)").arg(billTypeAVailable_count));
    ui->pushButton_bill_rcv->setText(QObject::tr("已经确定") + QString("(%1)").arg(billTypeRCV_count));
    ui->pushButton_bill_paid->setText(QObject::tr("已经支付") + QString("(%1)").arg(billTypePaid_count));
    ui->pushButton_bill_quick_order->setText(QObject::tr("先付款订单") + QString("(%1)").arg(billTypeQuickOrder_count));
    ui->pushButton_bill_yun_add_dish->setText(QObject::tr("云端添加") + QString("(%1)").arg(billTypeYunAddDish_count));
}

bool w_scr_dish_main_qr_code_over_view::refresh(const w_scr_dish_main_qr_code::QrCodeMasterDetails &master_details, int &auto_accept_index, int &billTypeAVailable_count, int &billTypeRCV_count, int &billTypePaid_count, int &billTypeQuickOrder_count, int &billTypeYunAddDish_count)
{
    const w_scr_dish_main_qr_code::QrCodeMaster &master = master_details.master;
    QString errstring;

    auto_accept_index ++;
    //自动接单
    if(auto_accept_manual && lds::sysconf->value("w_scr_dish_main_qr_code_over_view_set/auto_accept", false).toBool()) {
        if(master.state == w_scr_dish_main_qr_code::billTypeAVailable) {
            if(!w_scr_dish_main_qr_code::qr_code_can_order(master.tableNo, errstring)) {
                textEditAppend(false, QString().sprintf("%02d. ",auto_accept_index) + "sn:" + master.sn + "," + QObject::tr("餐桌") +":" + master.tableNo + QObject::tr("确认订单") + "," + QObject::tr("操作失败") + "," + errstring);
            } else {
                QrCode_cey_u_orderdishList detail_list;
                w_scr_dish_main_qr_code::orderdish_generate(master.sn, detail_list);
                QString null_ch_dishno;
                if(!w_scr_dish_main_qr_code::orderdish_is_valid(detail_list, null_ch_dishno)) {
                    textEditAppend(false, QString().sprintf("%02d. ",auto_accept_index) + "sn:" + master.sn + "," + QObject::tr("餐桌") +":" + master.tableNo + QObject::tr("确认订单") + "," + QObject::tr("操作失败") + "," + QObject::tr("商品不存在") + ":" + null_ch_dishno);
                } else {
                    if(!w_scr_dish_main_qr_code::orderdish_bill_create_do_RCV_transaction(master, detail_list, errstring)) {
                        textEditAppend(false, QString().sprintf("%02d. ",auto_accept_index) + "sn:" + master.sn + "," + QObject::tr("餐桌") +":" + master.tableNo + QObject::tr("确认订单") + "," + QObject::tr("操作失败") + "," + errstring);
                    } else {
                        textEditAppend(true, QString().sprintf("%02d. ",auto_accept_index) + "sn:" + master.sn + "," + QObject::tr("餐桌") +":" + master.tableNo + QObject::tr("确认订单") + "," + QObject::tr("操作成功"));
                    }
                }
            }
        }
        //自动付款
        if(master.state == w_scr_dish_main_qr_code::billTypePaid) {
            w_scr_dish_main_qr_code::QrCodePay qr_code_pay_data;
            if(!w_scr_dish_main_qr_code::qr_code_has_paid(master.sn, qr_code_pay_data, errstring)) {
                textEditAppend(false, QString().sprintf("%02d. ",auto_accept_index) + "sn:" + master.sn + "," + QObject::tr("餐桌") +":" + master.tableNo + QObject::tr("确认支付") + "," + QObject::tr("操作失败") + ", bill not paid");
            } else {
                if(!w_scr_dish_main_qr_code::qr_code_bill_do_pay_transaction(master.tableNo,
                                                                             lds_query_hddpos::selectValue(QString(" select ch_billno from cey_bt_table where ch_tableno = '%1' ").arg(master.tableNo)).toString(),
                                                                             qr_code_pay_data,
                                                                             errstring)) {
                    textEditAppend(false, QString().sprintf("%02d. ",auto_accept_index) + "sn:" + master.sn + "," + QObject::tr("餐桌") +":" + master.tableNo + QObject::tr("确认支付") + "," + QObject::tr("操作失败") + "," + errstring);
                } else {
                    textEditAppend(false, QString().sprintf("%02d. ",auto_accept_index) + "sn:" + master.sn + "," + QObject::tr("餐桌") +":" + master.tableNo + QObject::tr("确认支付") + "," + QObject::tr("操作成功"));
                    textEditAppend(true, "sn:" + master.sn + "," + qr_code_pay_data.information());
                }
            }
        }
        //先付款订单
        if(master.state == w_scr_dish_main_qr_code::billTypeQuickOrder) {
            master.print();
            if(false == w_scr_dish_main_qr_code::qr_code_bill_quick_order_do_OK_transaction(master, errstring)) {
                textEditAppend(false, QString().sprintf("%02d. ",auto_accept_index) + "sn:" + master.sn + "," + QObject::tr("餐桌") +":" + master.tableNo + QObject::tr("先付款订单") + "," + errstring);
                return false;//先付款订单只能顺序执行，若第一次失败，则之后的将不会继续
            }
            textEditAppend(true, QString().sprintf("%02d. ",auto_accept_index) + "sn:" + master.sn + "," + QObject::tr("餐桌") +":" + master.tableNo + QObject::tr("先付款订单") + "," + QObject::tr("操作成功"));
        }
        //云端添加
        if(master.state == w_scr_dish_main_qr_code::billTypeYunAddDish) {
            if(false == biilYunAddDishTransaction(master, master_details.details, errstring)) {
                textEditAppend(false, QString().sprintf("%02d. ",auto_accept_index) + "sn:" + master.sn + "," + QObject::tr("餐桌") +":" + master.tableNo + QObject::tr("云端添加") + "," + errstring);
                return false;//先付款订单只能顺序执行，若第一次失败，则之后的将不会继续
            }
            textEditAppend(true, QString().sprintf("%02d. ",auto_accept_index) + "sn:" + master.sn + "," + QObject::tr("餐桌") +":" + master.tableNo + QObject::tr("云端添加") + "," + QObject::tr("操作成功"));
        }
        //
        return true;
    }
    //
    if(master.state == w_scr_dish_main_qr_code::billTypeAVailable) {
        billTypeAVailable_count ++;
    }
    if(master.state == w_scr_dish_main_qr_code::billTypePaid) {
        billTypePaid_count ++;
    }
    if(master.state == w_scr_dish_main_qr_code::billTypeRCV) {
        billTypeRCV_count ++;
    }
    if(master.state == w_scr_dish_main_qr_code::billTypeQuickOrder) {
        billTypeQuickOrder_count ++;
    }
    if(master.state == w_scr_dish_main_qr_code::billTypeYunAddDish) {
        billTypeYunAddDish_count ++;
    }

    if(pushButton_bill_curusrdata().toString().length() != 0 //所有订单
            && pushButton_bill_curusrdata().toString() != master.state ) {//指定订单
        //        continue;
        return true;
    }
    //
    //billTypeAVailable
    int row = standmodel->rowCount();
    standmodel->insertRow(row);
    standmodel->model_data_set(row, "int_id", master.sn);
    standmodel->model_data_set(row, "ch_tableno", master.tableNo);
    standmodel->model_data_set(row, "num_total", master.mount);
    standmodel->model_data_set(row, "do1", "");
    standmodel->model_data_set(row, "do2", "");
    standmodel->model_data_set(row, "ch_billno", "");

    if(master.state == w_scr_dish_main_qr_code::billTypeAVailable) {
        standmodel->model_data_set(row, "ch_state", QObject::tr("新的订单"));
        ui->tableView->setIndexWidget(standmodel->index(row, standmodel->fieldIndex("do1")), billgetBtn(row, master));
        ui->tableView->setIndexWidget(standmodel->index(row, standmodel->fieldIndex("do2")), billcancelBtn(row, master));
        ui->tableView->setIndexWidget(standmodel->index(row, standmodel->fieldIndex("do3")), billdetailBtn(row, master));

    }
    if(master.state == w_scr_dish_main_qr_code::billTypePaid) {
        standmodel->model_data_set(row, "ch_state", QObject::tr("已经支付"));
        standmodel->model_data_set(row, "ch_billno", lds_query_hddpos::selectValue(QString(" select ch_billno from cey_bt_table where ch_tableno = '%1' ").arg(master.tableNo)));
        ui->tableView->setIndexWidget(standmodel->index(row, standmodel->fieldIndex("do1")), billpayBtn(row, master));
        ui->tableView->setIndexWidget(standmodel->index(row, standmodel->fieldIndex("do3")), billdetailBtn(row, master));
    }
    if(master.state == w_scr_dish_main_qr_code::billTypeRCV) {
        standmodel->model_data_set(row, "ch_state", QObject::tr("已经确定"));
        standmodel->model_data_set(row, "ch_billno", lds_query_hddpos::selectValue(QString(" select ch_billno from cey_bt_table where ch_tableno = '%1' ").arg(master.tableNo)));

        ui->tableView->setIndexWidget(standmodel->index(row, standmodel->fieldIndex("do2")), billdetailBtn(row, master));
        ui->tableView->setIndexWidget(standmodel->index(row, standmodel->fieldIndex("do3")), billTypeRCVMoreBtn(row, master));
    }
    if(master.state == w_scr_dish_main_qr_code::billTypeQuickOrder) {
        standmodel->model_data_set(row, "ch_state", QObject::tr("先付款订单"));
        if(1 == billTypeQuickOrder_count) {//第一单
            ui->tableView->setIndexWidget(standmodel->index(row, standmodel->fieldIndex("do1")), biilquickOrderBtn(row, master));
        }
        ui->tableView->setIndexWidget(standmodel->index(row, standmodel->fieldIndex("do3")), billdetailBtn(row, master));
    }
    if(master.state == w_scr_dish_main_qr_code::billTypeYunAddDish) {
        standmodel->model_data_set(row, "ch_state", QObject::tr("云端添加"));
        if(1 == billTypeYunAddDish_count) {//第一单
            ui->tableView->setIndexWidget(standmodel->index(row, standmodel->fieldIndex("do1")), biilYunAddDishBtn(row, master_details));
        }
        ui->tableView->setIndexWidget(standmodel->index(row, standmodel->fieldIndex("do3")), billdetailBtn_yun_add_dish(row, master_details));
    }

    return true;
}

void w_scr_dish_main_qr_code_over_view::torefresh_check()
{
    if(false == this_is_topLevel())
        return;
    if(ui->label_loading->movie()->state() == QMovie::Running) {
        return;
    }
    refresh();
}

void w_scr_dish_main_qr_code_over_view::toset()
{
    w_scr_dish_main_qr_code_over_view_set dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
    view_set_app();
}

void w_scr_dish_main_qr_code_over_view::torefresh_auto()
{
    if(false == this_is_topLevel())
        return;
    if(ui->label_loading->movie()->state() == QMovie::Running) {
        return;
    }
    //暂停、开启手动接单
    if(ui->checkBox->isChecked()) {
        return;
    }

    ui->pushButton_refresh->setText(QObject::tr("刷新") + "(" + QString::number(time_line) + ")");
    if(time_line <= 0) {
        time_line = lds::sysconf->value("w_scr_dish_main_qr_code_over_view_set/during_refresh", 10).toInt();
#ifdef QT_DEBUG
    time_line =1;
#endif
        torefresh_check();
        return;
    }
    time_line --;
}

void w_scr_dish_main_qr_code_over_view::tobillget()
{
    //!等待
    loadingRun loading(ui);
    loading.show();
    //!~等待


    PushButton *b = static_cast<PushButton *>(this->sender());
    if(b) {
        int row = b->row;
        if(row >= 0) {
            QString errstring;
            QString null_ch_dishno;
            QrCode_cey_u_orderdishList detail_list;

            if(false == w_scr_dish_main_qr_code::qr_code_can_order(b->master.tableNo, errstring)) {
                loading.setText(errstring);
                return;
            }
            //
            w_scr_dish_main_qr_code::orderdish_generate(b->master.sn, detail_list);
            //
            while(false == w_scr_dish_main_qr_code::orderdish_is_valid(detail_list, null_ch_dishno)) {
                if(1 == lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  null_ch_dishno + ":" +QObject::tr("商品不存在"), QObject::tr("数据同步"), QObject::tr("取消"))) {
                    return;
                }
                /*同步*/
                w_sys_manage_cloudsync_with_time_running::exec_keepdata(this);
            }
            //
            if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, b->text(), QObject::tr("确定"), QObject::tr("取消"))) {
                return;
            }
            if(w_scr_dish_main_qr_code::orderdish_bill_create_do_RCV_transaction(b->master, detail_list, errstring)) {
                loading.setText(QObject::tr("操作成功"));
                refresh();
                return;
            }
            loading.setText(errstring);
        }
    }
}

void w_scr_dish_main_qr_code_over_view::tobillpay()
{
    //!等待
    loadingRun loading(ui);
    loading.show();
    //!~等待

    PushButton *b = static_cast<PushButton *>(this->sender());
    if(b) {
        int row = b->row;
        if(row >= 0) {
            QString errstring;
            w_scr_dish_main_qr_code::QrCodePay qr_code_pay_data;
            QString sn = standmodel->model_data(row, "int_id").toString();
            QString ch_tableno = standmodel->model_data(row, "ch_tableno").toString();
            QString ch_billno = standmodel->model_data(row, "ch_billno").toString();

            //not_paid
            if(false == w_scr_dish_main_qr_code::qr_code_has_paid(
                        sn,
                        qr_code_pay_data,
                        errstring)) {
                loading.setText(QObject::tr("操作失败") + "not_paid");
                return;
            }
            //
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("二维码点餐") + "\n" +
                                        QObject::tr("本单已支付") + "\n" +
                                        QObject::tr("餐桌") + ":" +ch_tableno+ "\n"  +
                                        qr_code_pay_data.information()
                                        );
            if(false == w_scr_dish_main_qr_code::qr_code_bill_do_pay_transaction(ch_tableno, ch_billno, qr_code_pay_data, errstring)) {
                loading.setText(errstring);
                return;
            }
            loading.setText(QObject::tr("操作成功"));
            refresh();
        }
    }
}

void w_scr_dish_main_qr_code_over_view::tobillcancel()
{
    //!等待
    loadingRun loading(ui);
    loading.show();
    //!~等待

    PushButton *b = static_cast<PushButton *>(this->sender());
    if(b) {
        int row = b->row;
        if(row >= 0) {
            if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, b->text(), QObject::tr("确定"), QObject::tr("取消"))) {
                return;
            }
            QString errstring;
            if(w_scr_dish_main_qr_code::qr_code_bill_do_CANCEL(b->master.sn, errstring)) {
                loading.setText(QObject::tr("操作成功"));
                refresh();
                return;
            }
            loading.setText(errstring);
        }
    }
}

void w_scr_dish_main_qr_code_over_view::tobilldetail()
{
    PushButton *b = static_cast<PushButton *>(this->sender());
    if(b) {
        showdetail(b, operaDetail);
    }
}

void w_scr_dish_main_qr_code_over_view::tobilldetail_yun_add_dish()
{
    PushButton *b = static_cast<PushButton *>(this->sender());
    showdetail(b, operaDetailYunAdd);
}

void w_scr_dish_main_qr_code_over_view::tobillreprint()
{
    PushButton *b = static_cast<PushButton *>(this->sender());
    if(b) {
        int row = b->row;
        if(row >= 0) {
            QString ch_billno = standmodel->model_data(row, "ch_billno").toString();

            public_printby_ch_billno printer(ch_billno);
            printer.toprint_preprint(printer.defpath(), QObject::tr("预打单"));
            public_sql::preprinttablemap.insert(ch_billno, true);
        }
    }
}

void w_scr_dish_main_qr_code_over_view::tobillquit()
{
    PushButton *b = static_cast<PushButton *>(this->sender());
    if(b) {
        showdetail(b, operaQuit);
    }
}

void w_scr_dish_main_qr_code_over_view::tobilldishadd()
{
    PushButton *b = static_cast<PushButton *>(this->sender());
    if(b) {
        dishadd(b->row);
    }
}

void w_scr_dish_main_qr_code_over_view::tobillmoreBtn()
{
    public_select_stringlist dialog(QStringList () << QObject::tr("另加")<<  QObject::tr("退货")<<  QObject::tr("消台")<<  QObject::tr("预打单")<<  QObject::tr("现金支付"), this);
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        switch(dialog.curSelect(Qt::UserRole + 1).toInt()) {
        case 0://另加
            tobilldishadd();
            break;
        case 1://退货
            tobillquit();
            break;
        case 2://消台
            tobilltablecancel();
            break;
        case 3://预打单
            tobillreprint();
            break;
        case 4://现金支付
            tobillcashpay();
            break;
        default:
            break;
        }
    }
}

void w_scr_dish_main_qr_code_over_view::tobilltablecancel()
{
    //!等待
    loadingRun loading(ui);
    loading.show();
    //!~等待

    PushButton *b = static_cast<PushButton *>(this->sender());
    if(b) {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("消台"), QObject::tr("确定"), QObject::tr("取消"))) {
            return;
        }

        double num_num = 0;
        foreach(const w_scr_dish_main_qr_code::QrCodeDetail & d, w_scr_dish_main_qr_code::qr_code_detail_get(b->master.sn)) {
            if(d.flag == -1) {//退货
                continue;
            }
            num_num += d.number;
        }
        if(num_num != 0) {
            qDebug() << __LINE__ << num_num;
            loading.setText(QObject::tr("数量不为0"));
            return;
        }

        QString errstring;
        lds_query_hddpos::tran_saction();
        lds_query_hddpos  query;
        QString ch_billno = standmodel->model_data(b->row, "ch_billno").toString();
        QString ch_tableno = b->master.tableNo;

        QString ch_payno = lds_scr_table::get_max_ch_payno_add1();
        if(query.execUpdate("cey_bt_table", qrtVariantPairList() << qrtVariantPair("ch_billno", ""), QString("ch_tableno= '%1' ").arg(ch_tableno), &errstring)) {
            if(query.execInsert("cey_u_checkout_master",qrtVariantPairList()
                                << qrtVariantPair("ch_billno", ch_billno)
                                << qrtVariantPair("ch_tableno", ch_tableno)
                                << qrtVariantPair("num_cost", 0)
                                << qrtVariantPair("num_discount", 0)
                                << qrtVariantPair("num_present", 0)

                                << qrtVariantPair("num_service", 0)
                                //                                        << qrtVariantPair("vch_service_desc", "")
                                << qrtVariantPair("num_lowcost", 0)
                                << qrtVariantPair("num_blotout", 0)
                                << qrtVariantPair("num_run", 0)

                                << qrtVariantPair("vch_run_operID", "")
                                << qrtVariantPair("ch_state", "Y")
                                << qrtVariantPair("vch_operID", lds::gs_operid)
                                << qrtVariantPair("dt_operdate", n_func::f_get_sysdatetime())
                                << qrtVariantPair("vch_operID2", "")

                                << qrtVariantPair("num_rate", 0)
                                << qrtVariantPair("ch_payno", ch_payno)
                                << qrtVariantPair("vch_memberno", "")
                                << qrtVariantPair("ch_member_belong_state", "")
                                << qrtVariantPair("ch_yun_payno", "")

                                << qrtVariantPair("num_cash", 0),  &errstring)) {
                if(query.execUpdate("cey_u_table","ch_payno", ch_payno, qrtEqual("ch_billno", ch_billno),  &errstring)) {
                    if(query.execUpdate("cey_u_orderdish","ch_payno", ch_payno, qrtEqual("ch_billno", ch_billno),  &errstring)) {
                        if(w_scr_dish_main_qr_code::qr_code_bill_do_CANCEL(b->master.sn, errstring)) {
                            lds_query_hddpos::com_mit();
                            loading.setText(QObject::tr("操作成功"));
                            refresh();
                            return;
                        }
                    }
                }
            }
        }
        lds_query_hddpos::roll_back();
        loading.setText(errstring);
    }
}

void w_scr_dish_main_qr_code_over_view::tobillcashpay()
{
    PushButton *b = static_cast<PushButton *>(this->sender());
    if(b) {
        int row = b->row;
        if(row >= 0) {
            QString errstring;
            w_scr_dish_main_qr_code::QrCodePay qr_code_pay_data;
            QString orderSn = standmodel->model_data(row, "int_id").toString();
            QString ch_tableno = standmodel->model_data(row, "ch_tableno").toString();
            QString ch_billno = standmodel->model_data(row, "ch_billno").toString();

            w_scr_dish_main_qr_code_over_view_cash_pay dialog(b->master.mount, this);
            if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
                qr_code_pay_data.num_cost = b->master.mount;
                qr_code_pay_data.num_round = dialog.get_num_round();
                qr_code_pay_data.num_cash = dialog.get_num_cash();

                qr_code_pay_data.num_money = dialog.get_num_money();
                qr_code_pay_data.num_change =  dialog.get_num_change();

                //
                if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("二维码点餐") + "\n" +
                                                 QObject::tr("餐桌") + ":" +ch_tableno+ "\n"  +
                                                 qr_code_pay_data.information(), QObject::tr("确定"), QObject::tr("取消")
                                                 )) {
                    return;
                }

                lds_query_hddpos::tran_saction();
                QString ch_payno;
                if(w_scr_dish_main_qr_code::qr_code_bill_do_pay(ch_tableno, ch_billno, qr_code_pay_data, ch_payno, errstring)) {
                    if(w_scr_dish_main_qr_code::qr_code_bill_cashpay(ch_payno, orderSn, dialog.get_num_cost(), dialog.get_num_cash(), errstring)) {
                        lds_query_hddpos::com_mit();
                        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
                        w_scr_dish_main_qr_code::qr_code_bill_do_pay_print(ch_billno);
                        refresh();
                        return;
                    }
                }
                lds_query_hddpos::roll_back();
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
            }
        }
    }
}

void w_scr_dish_main_qr_code_over_view::tobiilquickOrder()
{
    qDebug() << __FUNCTION__ << __LINE__;

    QString errstring;
    PushButton *b = static_cast<PushButton *>(this->sender());
    b->master.print();
    if(false == w_scr_dish_main_qr_code::qr_code_bill_quick_order_do_OK_transaction(b->master, errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));//应收0.01 但没有支付方式
    refresh();
}

void w_scr_dish_main_qr_code_over_view::tobiilYunAddDish()
{
    PushButton *b = static_cast<PushButton *>(this->sender());
    QString errstring;
    if(false == biilYunAddDishTransaction(b->master, b->details, errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    refresh();
}

void w_scr_dish_main_qr_code_over_view::tomin()
{
    this->hide();
}

void w_scr_dish_main_qr_code_over_view::toexit()
{
    this->reject();
}

void w_scr_dish_main_qr_code_over_view::todishquit()
{
    PushButton *b = static_cast<PushButton *>(this->sender());
    if(b) {
        lds_query_hddpos  query;
        QString ch_billno = standmodel->model_data(b->row, "ch_billno").toString();
        QString errstring;
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("退货"), QObject::tr("确定"), QObject::tr("取消"))) {
            return;
        }
        if(w_scr_dish_main_qr_code::orderdish_dish_quit_transaction(ch_billno, b->dish_sn, errstring)) {
            w_scr_dish_main_qr_code::QrCodePay qr_code_pay_data;
            w_scr_dish_main_qr_code::qr_code_has_paid(b->master.sn, qr_code_pay_data, errstring);
            query.execUpdate("cey_u_table", "num_cost", qr_code_pay_data.num_cost, qrtEqual("ch_billno", ch_billno));

            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            showdetailRefresh(b->row, b->master, operaQuit, b->dish_table, b->dish_model);
            return;
        }
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
    }
}

void w_scr_dish_main_qr_code_over_view::todishadd()
{
    PushButton *b = static_cast<PushButton *>(this->sender());
    if(b) {
        if(dishadd(b->row)) {
            showdetailRefresh(b->row, b->master, operaAdd, b->dish_table, b->dish_model);
        }
    }
}

void w_scr_dish_main_qr_code_over_view::table_record_vscroll_set_max()
{
    ui->tableView_record->verticalScrollBar()->setValue(ui->tableView_record->verticalScrollBar()->maximum());
}

void w_scr_dish_main_qr_code_over_view::toAddDishSn()
{
    w_scr_dish_main_qr_code_over_view_dialog dialog(w_scr_dish_main_qr_code::billTypeRCV, this);
    dialog.resize(this->size());
    transparentCenterDialog(&dialog).exec();
}

void w_scr_dish_main_qr_code_over_view::processPendingDatagrams()
{
    while(udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
#ifdef Q_OS_WIN
        lds_thread::msecSleep(5);//window下解决办法
#endif

        qDebug() << datagram;
        if( "quit" == datagram) {
            qApp->quit();
        }
    }
}

QPushButton *w_scr_dish_main_qr_code_over_view::billpayBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master)
{
    PushButton *btn = new PushButton;
    btn->setText(QObject::tr("确认支付"));
    btn->row = row;
    btn->master = master;
    connect(btn, SIGNAL(clicked()), this, SLOT(tobillpay()));
    return btn;
}

QPushButton *w_scr_dish_main_qr_code_over_view::billgetBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master)
{
    PushButton *btn = new PushButton;
    btn->setText(QObject::tr("确认订单"));
    btn->row = row;
    btn->master = master;
    connect(btn, SIGNAL(clicked()), this, SLOT(tobillget()));
    return btn;
}

QPushButton *w_scr_dish_main_qr_code_over_view::billcancelBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master)
{
    PushButton *btn = new PushButton;
    btn->setText(QObject::tr("取消订单"));
    btn->row = row;
    btn->master = master;
    connect(btn, SIGNAL(clicked()), this, SLOT(tobillcancel()));
    return btn;
}

QPushButton *w_scr_dish_main_qr_code_over_view::billdetailBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master)
{
    PushButton *btn = new PushButton;
    btn->setText(QObject::tr("详细"));
    btn->row = row;
    btn->master = master;
    connect(btn, SIGNAL(clicked()), this, SLOT(tobilldetail()));
    return btn;
}

QPushButton *w_scr_dish_main_qr_code_over_view::billdetailBtn_yun_add_dish(int row, const w_scr_dish_main_qr_code::QrCodeMasterDetails &master_details)
{
    PushButton *btn = new PushButton;
    btn->setText(QObject::tr("详细"));
    btn->row = row;
    btn->master = master_details.master;
    btn->details = master_details.details;
    connect(btn, SIGNAL(clicked()), this, SLOT(tobilldetail_yun_add_dish()));
    return btn;
}

QPushButton *w_scr_dish_main_qr_code_over_view::billreprintBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master)
{
    PushButton *btn = new PushButton;
    btn->setText(QObject::tr("预打单"));
    btn->row = row;
    btn->master = master;
    connect(btn, SIGNAL(clicked()), this, SLOT(tobillreprint()));
    return btn;
}

QPushButton *w_scr_dish_main_qr_code_over_view::billquitBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master)
{
    PushButton *btn = new PushButton;
    btn->setText(QObject::tr("退货"));
    btn->row = row;
    btn->master = master;
    connect(btn, SIGNAL(clicked()), this, SLOT(tobillquit()));
    return btn;
}

QPushButton *w_scr_dish_main_qr_code_over_view::billdishaddBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master)
{
    PushButton *btn = new PushButton;
    btn->setText(QObject::tr("另加"));
    btn->row = row;
    btn->master = master;
    connect(btn, SIGNAL(clicked()), this, SLOT(tobilldishadd()));
    return btn;
}

QPushButton *w_scr_dish_main_qr_code_over_view::billTypeRCVMoreBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master)
{
    PushButton *btn = new PushButton;
    btn->setText(QObject::tr("更多"));
    btn->row = row;
    btn->master = master;
    connect(btn, SIGNAL(clicked()), this, SLOT(tobillmoreBtn()));
    return btn;
}

QPushButton *w_scr_dish_main_qr_code_over_view::biilquickOrderBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master)
{
    PushButton *btn = new PushButton;
    btn->setText(QObject::tr("确认"));
    btn->row = row;
    btn->master = master;
    connect(btn, SIGNAL(clicked()), this, SLOT(tobiilquickOrder()));
    return btn;
}

QPushButton *w_scr_dish_main_qr_code_over_view::biilYunAddDishBtn(int row, const w_scr_dish_main_qr_code::QrCodeMasterDetails &master_details)
{
    PushButton *btn = new PushButton;
    btn->setText(QObject::tr("确认"));
    btn->row = row;
    btn->master = master_details.master;
    btn->details = master_details.details;
    connect(btn, SIGNAL(clicked()), this, SLOT(tobiilYunAddDish()));
    return btn;
}

QPushButton *w_scr_dish_main_qr_code_over_view::dishquitBtn(int master_row, const w_scr_dish_main_qr_code::QrCodeMaster &master, const QString &dish_sn, lds_tableView *dish_table, lds_model_sqlstandardmodel *dish_model, bool hasquit)
{
    PushButton *btn = new PushButton;
    btn->setText(QObject::tr("退货"));
    if(hasquit)
        btn->setText(QObject::tr("已退货"));
    btn->row = master_row;
    btn->dish_sn = dish_sn;
    btn->dish_table = dish_table;
    btn->dish_model = dish_model;
    btn->master = master;
    btn->setDisabled(hasquit);
    connect(btn, SIGNAL(clicked()), this, SLOT(todishquit()));
    return btn;
}

QPushButton *w_scr_dish_main_qr_code_over_view::dishaddBtn(int master_row, const w_scr_dish_main_qr_code::QrCodeMaster &master, lds_tableView *dish_table, lds_model_sqlstandardmodel *dish_model)
{
    PushButton *btn = new PushButton;
    btn->setText(QObject::tr("另加"));
    btn->row = master_row;
    btn->dish_table = dish_table;
    btn->dish_model = dish_model;
    btn->master = master;
    btn->setDisabled(true);
    connect(btn, SIGNAL(clicked()), this, SLOT(todishadd()));
    return btn;
}

bool w_scr_dish_main_qr_code_over_view::biilYunAddDishTransaction(const w_scr_dish_main_qr_code::QrCodeMaster &master, const QList<w_scr_dish_main_qr_code::QrCodeDetail> &details, QString &errstring)
{
    QrCode_cey_u_orderdishList order_list;
    w_scr_dish_main_qr_code::orderdish_generate(details, order_list);
    QString ch_billno = lds_query_hddpos::selectValue(QString(" select ch_billno from cey_bt_table where ch_tableno = '%1'; ").arg(master.tableNo)).toString();
    QString flowid_greater_than = QString(" int_flowid > %1 ").arg(lds_query_hddpos::selectValue(QString(" select max(int_flowid) from cey_u_orderdish where ch_billno = '%1'; ").arg(ch_billno)).toLongLong());
    if(ch_billno == "") {
        errstring = QObject::tr("餐桌未开台");
        return false;
    }

    lds_query_hddpos::tran_saction();
    if(w_scr_dish_main_qr_code::orderdish_bill_append(ch_billno, order_list, errstring)) {
        if(n_func::f_set_sysparm("qr_code_yun_add_dish_cur_sn", details.last().sn, "", errstring)) {
            lds_query_hddpos::com_mit();
            //
            //消费单
            if(lds::sysconf->value("system_setting/luodanzongdan").toString() == "1") {
                public_printby_ch_billno printer(ch_billno);
                printer.toprint_preprint(lds::sysconf->value("system_setting/printerpath").toString(), QObject::tr("消费单"), flowid_greater_than);
            }
            return true;
        }
    }
    lds_query_hddpos::roll_back();
    return false;
}

void w_scr_dish_main_qr_code_over_view::hideButton(int row, int column)
{
    QPushButton *b = static_cast<QPushButton *>(ui->tableView->indexWidget(standmodel->index(row, column)));
    if(b) {
        b->hide();
    }
}

void w_scr_dish_main_qr_code_over_view::showdetail(PushButton *btn, operaType type)
{
    if(operaAdd == type) {
        if(false == dishadd(btn->row)) {
            return;
        }
    }

    QString dish_info;
    dish_info += btn->master.orgName + "\n";
    QDialog *dialog = new QDialog;
    lds_tableView *table = new lds_tableView(dialog);
    dialog->resize(ui->tableView->size());
    dialog->setWindowTitle(MESSAGE_TITLE_VOID);
    table->setGeometry(0, 0, dialog->width(), dialog->height());
    lds_model_sqlstandardmodel *model = new lds_model_sqlstandardmodel(dialog);
    model->setHorizontalHeaderLabels(QStringList()  << "ch_dishno" << "num_num" << "num_price" << "vch_print_memo" << "num_total" << "ch_state"<< "vch_operid");
    table->setTransferHheader();
    table->setModel(model);
    table->setEditTriggers(QTableView::NoEditTriggers);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setItemDelegate(new delegate_dish_detail(dialog));
    table->setItemDelegateForColumn(model->fieldIndex("vch_print_memo"), new fexpandmain_delegate_vch_printmemo(this));

    if(operaDetailYunAdd == type) {
        showdetailRefresh(btn->details, table, model);
    } else {
        showdetailRefresh(btn->row, btn->master, type, table, model);
    }

    lds_roundeddialog_rect_pop d(dialog, this, dialog->windowFlags() | Qt::FramelessWindowHint);
    d.updatePos(ui->tableView->mapToGlobal(QPoint(0, 0)));
    d.exec();

    refresh();
}

void w_scr_dish_main_qr_code_over_view::showdetailRefresh(int master_row, const w_scr_dish_main_qr_code::QrCodeMaster &master, operaType type, lds_tableView *table, lds_model_sqlstandardmodel *model)
{
    QrCode_cey_u_orderdishList detail_list;
    {
        //!等待
        loadingRun loading(ui);
        loading.show();
        //!~等待
        w_scr_dish_main_qr_code::orderdish_generate(master.sn, detail_list);
    }
    lds_model_sqltablemodel cache;
    cache.tablename_kvmap_insert_value("flag", "-1", QObject::tr("已退货"));
    cache.tablename_kvmap_insert_value("flag", "0", QObject::tr("云端点单"));
    cache.tablename_kvmap_insert_value("flag", "1", QObject::tr("本地加菜"));
    cache.tablename_kvmap_insert_value("flag", "11", QObject::tr("云端添加"));
    model->removeRows(0, model->rowCount());
    table->clearSpans();

    if(operaAdd == type) {
        model->appendRow(QList<QStandardItem* >()
                         << lds_model_sqlstandardmodel::newItem("")
                         << lds_model_sqlstandardmodel::newItem("")
                         << lds_model_sqlstandardmodel::newItem("")
                         << lds_model_sqlstandardmodel::newItem("")
                         << lds_model_sqlstandardmodel::newItem("")

                         << lds_model_sqlstandardmodel::newItem("")
                         << lds_model_sqlstandardmodel::newItem("")
                         );
        table->setIndexWidget(model->index(0, model->fieldIndex("ch_dishno")), dishaddBtn(master_row, master, table, model));
    }
    for(int k = 0; k < detail_list.count(); k++) {
        const w_scr_dish_main_qr_code::QrCode_cey_u_orderdish &detail_k = detail_list[k];
        int row = model->rowCount();
        model->appendRow(QList<QStandardItem*>()
                         << lds_model_sqlstandardmodel::newItem(cache.tablename_kvmap_value("cey_bt_dish", detail_k.ch_dishno, "ch_dishno",  "vch_dishname"))
                         << lds_model_sqlstandardmodel::newItem(detail_k.num_num)
                         << lds_model_sqlstandardmodel::newItem(detail_k.num_price)
                         << lds_model_sqlstandardmodel::newItem(detail_k.vch_print_memo.sqlValue())
                         << lds_model_sqlstandardmodel::newItem(detail_k.num_total)

                         << lds_model_sqlstandardmodel::newItem("")//detail_k.ch_suitflag)
                         << lds_model_sqlstandardmodel::newItem(detail_k.vch_operid)
                         );

        if(detail_k.ch_suitflag == "Y") {
            model->model_data_set(row, "ch_dishno", int(Qt::AlignVCenter | Qt::AlignRight), Qt::TextAlignmentRole);
        } else {
            model->model_data_set(row, "ch_state", cache.tablename_kvmap_value("flag", QString::number(detail_k.flag), detail_k.flag));
        }
        if(operaQuit == type && detail_k.ch_suitflag != "Y") {//非子套菜
            table->setIndexWidget(model->index(row, model->fieldIndex("ch_state")), dishquitBtn(master_row, master, detail_k.vch_qr_code_sn, table, model, detail_k.flag == -1));
        }
    }
    model->appendRow(QList<QStandardItem*>()
                     << lds_model_sqlstandardmodel::newItem(
                         master.orgName + "\t" +
                         QObject::tr("时间") + "::" + master.date.toString(yyyyMMddhhmmss) + "\t" +//因为时间有 : 号
                         //                         QObject::tr("序号") +"::" + master.sn + "\t" +
                         //                         QObject::tr("餐桌") +"::" + master.tableNo + "\t" +
                         //                         QObject::tr("状态") +"::" + w_scr_dish_main_qr_code::billTypeDesc(master.state) + "\t" +
                         QObject::tr("金额") +"::" + QString::number(master.mount, 'f', 2)));
    table->setSpan(model->rowCount() - 1, 0, 1, model->columnCount());
    table->resizeColumnsToContentsDelay();
    table->restore(0);
}

void w_scr_dish_main_qr_code_over_view::showdetailRefresh(QList<w_scr_dish_main_qr_code::QrCodeDetail> details, lds_tableView *table, lds_model_sqlstandardmodel *model)
{
    QrCode_cey_u_orderdishList order_list;
    lds_model_sqltablemodel cache;
    cache.tablename_kvmap_insert_value("flag", "-1", QObject::tr("已退货"));
    cache.tablename_kvmap_insert_value("flag", "0", QObject::tr("云端点单"));
    cache.tablename_kvmap_insert_value("flag", "1", QObject::tr("本地加菜"));
    cache.tablename_kvmap_insert_value("flag", "11", QObject::tr("云端添加"));
    model->removeRows(0, model->rowCount());
    table->clearSpans();

    w_scr_dish_main_qr_code::orderdish_generate(details, order_list);

    for(int k = 0; k < order_list.count(); k++) {
        const w_scr_dish_main_qr_code::QrCode_cey_u_orderdish &detail_k = order_list[k];
        int row = model->rowCount();
        model->appendRow(QList<QStandardItem*>()
                         << lds_model_sqlstandardmodel::newItem(cache.tablename_kvmap_value("cey_bt_dish", detail_k.ch_dishno, "ch_dishno",  "vch_dishname"))
                         << lds_model_sqlstandardmodel::newItem(detail_k.num_num)
                         << lds_model_sqlstandardmodel::newItem(detail_k.num_price)
                         << lds_model_sqlstandardmodel::newItem(detail_k.vch_print_memo.sqlValue())
                         << lds_model_sqlstandardmodel::newItem(detail_k.num_total)

                         << lds_model_sqlstandardmodel::newItem("")//detail_k.ch_suitflag)
                         << lds_model_sqlstandardmodel::newItem(detail_k.vch_operid)
                         );

        if(detail_k.ch_suitflag == "Y") {
            model->model_data_set(row, "ch_dishno", int(Qt::AlignVCenter | Qt::AlignRight), Qt::TextAlignmentRole);
        } else {
            model->model_data_set(row, "ch_state", cache.tablename_kvmap_value("flag", QString::number(detail_k.flag), detail_k.flag));
        }
    }

    table->resizeColumnsToContentsDelay();
    table->restore(0);
}

bool w_scr_dish_main_qr_code_over_view::dishadd(int master_row)
{
    QString ch_billno = standmodel->model_data(master_row, "ch_billno").toString();
    QString master_sn = standmodel->model_data(master_row, "int_id").toString();
    w_st_good_input_select_dish dialog(this);
    dialog.enable_single_selected();
    dialog.filter_by_qr_code_onlineflag();
    dialog.justShowColumns(QStringList() << "ch_dishno" << "vch_dishname" << "num_price" << "ch_unitno");
    dialog.resize(700, 500);
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        w_scr_dish_main_qr_code_orderdish order(ch_billno, master_sn, dialog.get_tablemodel_value(dialog.ret_row_list.value(0), "ch_dishno").toString(), this);
        if(QDialog::Accepted == transparentCenterDialog(&order).exec()) {
            return true;
        }
    }
    return false;
}

void w_scr_dish_main_qr_code_over_view::textEditAppend(bool success, const QString &text)
{
    QStandardItemModel *m = qobject_cast<QStandardItemModel *>(ui->tableView_record->model());
    QStandardItem *item = new QStandardItem(text);
    m->appendRow(item);
    if(success)
        item->setData(QColor("green"), Qt::TextColorRole);
    else
        item->setData(QColor("red"), Qt::TextColorRole);
    if(m->rowCount() > 100)
        m->removeRow(0);

    QTimer::singleShot(10, this, SLOT(table_record_vscroll_set_max()));
}

void w_scr_dish_main_qr_code_over_view::view_set_app()
{
    if(lds::sysconf->value("w_scr_dish_main_qr_code_over_view_set/auto_refresh", false).toBool()) {
        lds::hideWidget(ui->checkBox, false);
        timer->start();
    } else {
        lds::hideWidget(ui->checkBox, true);
        timer->stop();
    }
    if(lds::sysconf->value("w_scr_dish_main_qr_code_over_view_set/auto_accept", false).toBool()) {
        lds::hideWidget(ui->tableView, true);
        pushButton_bill_hide(true);

        lds::hideWidget(ui->tableView_record, false);
        lds::hideWidget(ui->pushButton_up, true);
        lds::hideWidget(ui->pushButton_down, true);
        lds::hideWidget(ui->pushButton_more, false);
    } else {
        lds::hideWidget(ui->tableView, false);
        pushButton_bill_hide(false);

        lds::hideWidget(ui->tableView_record, true);
        lds::hideWidget(ui->pushButton_up, false);
        lds::hideWidget(ui->pushButton_down, false);
        lds::hideWidget(ui->pushButton_more, true);
    }
}

void w_scr_dish_main_qr_code_over_view::init()
{
    ui = new Ui::w_scr_dish_main_qr_code_over_view;
    ui->setupUi(this);

    standmodel = new lds_model_sqlstandardmodel(this);
    standmodel->setHorizontalHeaderLabels(QStringList() <<  "int_id"  << "ch_tableno" << "ch_state" << "num_total" << "do1" << "do2"<< "do3" << "ch_billno");
    ui->tableView->setTransferHheader();
    ui->tableView->setModel(standmodel);
    ui->tableView->verticalHeader()->setDefaultSectionSize(50);
    ui->tableView->hideColumn(standmodel->fieldIndex("ch_billno"));
    ui->tableView->horizontalHeader()->setFixedHeight(22);
    //    ui->tableView->setFixedHeight(8 * 50 + 22 + ui->tableView->contentsMargins().top() + ui->tableView->contentsMargins().bottom());

    ui->label_loading->setMovie(new QMovie("://image/loading.gif", "", this));
    ui->label_loading->setScaledContents(true);
    ui->label_loading->setFixedSize(30, 30);
    udpSocket= new QUdpSocket(this);
    udpSocket->bind(lds::SYS_QRCODE_ORDER_PORT);
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));

    ui->pushButton_bill_all->setData(Qt::UserRole, "");
    ui->pushButton_bill_new->setData(Qt::UserRole, w_scr_dish_main_qr_code::billTypeAVailable);
    ui->pushButton_bill_rcv->setData(Qt::UserRole, w_scr_dish_main_qr_code::billTypeRCV);
    ui->pushButton_bill_paid->setData(Qt::UserRole, w_scr_dish_main_qr_code::billTypePaid);
    ui->pushButton_bill_quick_order->setData(Qt::UserRole, w_scr_dish_main_qr_code::billTypeQuickOrder);
    ui->pushButton_bill_yun_add_dish->setData(Qt::UserRole, w_scr_dish_main_qr_code::billTypeYunAddDish);

    ui->tableView_record->setModel(new QStandardItemModel(this));
    ui->tableView_record->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_record->horizontalHeader()->hide();
    ui->tableView_record->verticalHeader()->hide();
    ui->tableView_record->horizontalHeader()->setStretchLastSection(true);
    ui->tableView_record->setShowGrid(false);
    ui->tableView_record->verticalHeader()->setDefaultSectionSize(15);

    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(torefresh_auto()));
    time_line = lds::sysconf->value("w_scr_dish_main_qr_code_over_view_set/during_refresh", 10).toInt();
#ifdef QT_DEBUG
    time_line =1;
#endif

    this->setWindowTitle(MESSAGE_TITLE_VOID);
    lds::hideWidget(ui->pushButton_exit);

    connect(ui->pushButton_min, SIGNAL(clicked()), this, SLOT(tomin()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->lineEdit_sn_tableNo, SIGNAL(textChanged(QString)), this, SLOT(torefresh_check()));
    connect(ui->lineEdit_sn_tableNo, SIGNAL(textChanged(QString)), this, SLOT(torefresh_check()));
    connect(ui->pushButton_down, SIGNAL(clicked()), ui->tableView, SLOT(toPageDown()));
    connect(ui->pushButton_up, SIGNAL(clicked()), ui->tableView, SLOT(toPageUp()));

    connect(ui->pushButton_bill_all, SIGNAL(clicked()), this, SLOT(torefresh_check()));
    connect(ui->pushButton_bill_new, SIGNAL(clicked()), this, SLOT(torefresh_check()));
    connect(ui->pushButton_bill_rcv, SIGNAL(clicked()), this, SLOT(torefresh_check()));
    connect(ui->pushButton_bill_paid, SIGNAL(clicked()), this, SLOT(torefresh_check()));
    connect(ui->pushButton_bill_quick_order, SIGNAL(clicked()), this, SLOT(torefresh_check()));
    connect(ui->pushButton_bill_yun_add_dish, SIGNAL(clicked()), this, SLOT(torefresh_check()));

    pushButton_bill_hide(true);

    connect(ui->pushButton_refresh, SIGNAL(clicked()), this, SLOT(torefresh_check()));
    connect(ui->pushButton_set, SIGNAL(clicked()), this, SLOT(toset()));

    connect(ui->pushButton_more, SIGNAL(clicked()), this, SLOT(toAddDishSn()));
    //
#ifdef QT_DEBUG
    QStandardItemModel *m = qobject_cast<QStandardItemModel *>(ui->tableView_record->model());
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));

    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));


    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));

    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));

    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));

    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));
    m->appendRow(new QStandardItem("fdjsighsidofjidsohfiodsfiodshiodshiods"));

    lds::hideWidget(ui->tableView, true);
    pushButton_bill_hide(true);

    lds::hideWidget(ui->tableView_record, false);
#endif
}

void w_scr_dish_main_qr_code_over_view::pushButton_bill_hide(bool hide)
{
    lds::hideWidget(ui->pushButton_bill_all, hide);
    lds::hideWidget(ui->pushButton_bill_new, hide);
    lds::hideWidget(ui->pushButton_bill_rcv, hide);
    lds::hideWidget(ui->pushButton_bill_paid, hide);
    lds::hideWidget(ui->pushButton_bill_quick_order, hide);
    lds::hideWidget(ui->pushButton_bill_yun_add_dish, hide);
}

void w_scr_dish_main_qr_code_over_view::pushButton_bill_emit_click()
{
    if(ui->pushButton_bill_all->isChecked()) ui->pushButton_bill_all->emitClick();
    if(ui->pushButton_bill_new->isChecked()) ui->pushButton_bill_new->emitClick();
    if(ui->pushButton_bill_rcv->isChecked()) ui->pushButton_bill_rcv->emitClick();
    if(ui->pushButton_bill_paid->isChecked()) ui->pushButton_bill_paid->emitClick();
    if(ui->pushButton_bill_quick_order->isChecked()) ui->pushButton_bill_quick_order->emitClick();
    if(ui->pushButton_bill_yun_add_dish->isChecked()) ui->pushButton_bill_yun_add_dish->emitClick();
}

QVariant w_scr_dish_main_qr_code_over_view::pushButton_bill_curusrdata()
{
    lds_toolbar_button *b = pushButton_bill_cur();
    if(b) {
        return b->getData(Qt::UserRole);
    }

    return QVariant();
}

lds_toolbar_button *w_scr_dish_main_qr_code_over_view::pushButton_bill_cur()
{
    if(ui->pushButton_bill_all->isChecked()) return ui->pushButton_bill_all;
    if(ui->pushButton_bill_new->isChecked()) return ui->pushButton_bill_new;
    if(ui->pushButton_bill_rcv->isChecked()) return ui->pushButton_bill_rcv;
    if(ui->pushButton_bill_paid->isChecked()) return ui->pushButton_bill_paid;
    if(ui->pushButton_bill_quick_order->isChecked()) return ui->pushButton_bill_quick_order;
    if(ui->pushButton_bill_yun_add_dish->isChecked()) return ui->pushButton_bill_yun_add_dish;

    return 0;
}

bool w_scr_dish_main_qr_code_over_view::this_is_topLevel()
{
    QWidget *w = this;
    foreach(QObject *o, w->children()) {
        w = qobject_cast<QWidget *>(o);
        if(w && w->isTopLevel()) {
            return false;
        }
    }
    return true;
}

void w_scr_dish_main_qr_code_over_view::pushButton_bill_check(QObject *obj)
{
    if(obj == ui->pushButton_bill_all
            || obj == ui->pushButton_bill_new
            || obj == ui->pushButton_bill_rcv
            || obj == ui->pushButton_bill_paid
            || obj == ui->pushButton_bill_quick_order
            || obj == ui->pushButton_bill_yun_add_dish
            ) {

        bool emit_click=  true;
        if(pushButton_bill_cur() == obj) {
            emit_click = false;
        }
        ui->pushButton_bill_all->setChecked(ui->pushButton_bill_all == obj);
        ui->pushButton_bill_new->setChecked(ui->pushButton_bill_new == obj);
        ui->pushButton_bill_rcv->setChecked(ui->pushButton_bill_rcv == obj);
        ui->pushButton_bill_paid->setChecked(ui->pushButton_bill_paid == obj);
        ui->pushButton_bill_quick_order->setChecked(ui->pushButton_bill_quick_order == obj);
        ui->pushButton_bill_yun_add_dish->setChecked(ui->pushButton_bill_yun_add_dish == obj);

        if(emit_click) {
            pushButton_bill_emit_click();
        }
    }
}

void w_scr_dish_main_qr_code_over_view::pushButton_bill_check(const QString &billType)
{
    lds_toolbar_button *b = pushButton_bill_cur();
    bool emit_click=  true;
    if(b
            && b->getData(Qt::UserRole).toString() == billType
            ) {
        emit_click = false;
    }
    ui->pushButton_bill_all->setChecked(ui->pushButton_bill_all->getData(Qt::UserRole).toString() == billType);
    ui->pushButton_bill_new->setChecked(ui->pushButton_bill_new->getData(Qt::UserRole).toString() == billType);
    ui->pushButton_bill_rcv->setChecked(ui->pushButton_bill_rcv->getData(Qt::UserRole).toString() == billType);
    ui->pushButton_bill_paid->setChecked(ui->pushButton_bill_paid->getData(Qt::UserRole).toString() == billType);
    ui->pushButton_bill_quick_order->setChecked(ui->pushButton_bill_quick_order->getData(Qt::UserRole).toString() == billType);
    ui->pushButton_bill_yun_add_dish->setChecked(ui->pushButton_bill_yun_add_dish->getData(Qt::UserRole).toString() == billType);

    if(emit_click) {
        pushButton_bill_emit_click();
    }
}

w_scr_dish_main_qr_code_over_view::delegate_dish_detail::delegate_dish_detail(QObject *parent)
    : lds_tableview_delegate_virtual(parent)
{

}

void w_scr_dish_main_qr_code_over_view::delegate_dish_detail::drawText(QPainter *painter, QStyle::State state, const QRect &painter_text_rect, Qt::Alignment displayAlignment, const QString &painter_text) const
{
    if(false == painter_text.contains("\t")) {
        lds_tableview_delegate_virtual::drawText(painter, state, painter_text_rect, displayAlignment, painter_text);
        return;
    }

    QFont font = painter->font();
    QFontMetrics fm(font);
    QStringList textList = painter_text.split("\t");
    QRect rect = painter_text_rect;
    foreach(const QString &text, textList) {
        QString p0, p1;
        if(text.contains("::")) {
            p0 =text.split("::").value(0) + ":";
            p1 =text.split("::").value(1) + "  ";
        } else {
            p1 = text + "  ";
        }

        if(state & QStyle::State_Selected) {

        } else {
            painter->setPen(QColor("black"));
        }
        painter->drawText(QStyle::alignedRect(painter->layoutDirection(), Qt::AlignVCenter | Qt::AlignLeft, QSize(fm.width(p0), fm.height()), rect),  Qt::AlignVCenter | Qt::AlignLeft, p0);
        rect.adjust(fm.width(p0), 0, 0, 0);

        if(state & QStyle::State_Selected) {

        } else {
            painter->setPen(QColor("#ff546b"));
        }
        painter->drawText(QStyle::alignedRect(painter->layoutDirection(), Qt::AlignVCenter | Qt::AlignLeft, QSize(fm.width(p1), fm.height()), rect), Qt::AlignVCenter | Qt::AlignLeft, p1);
        rect.adjust(fm.width(p1), 0, 0, 0);
    }
}



w_scr_dish_main_qr_code_over_view::PushButton::PushButton(QWidget *parent)
    : QPushButton(parent)
   ,  dish_table(0)
   ,  dish_model(0)
{

}


w_scr_dish_main_qr_code_over_view_dialog::w_scr_dish_main_qr_code_over_view_dialog(const QString &billType, QWidget *parent)
    : w_scr_dish_main_qr_code_over_view(billType, parent)
{
    lds::showWidget(ui->pushButton_exit);
    lds::showWidget(ui->pushButton_min, false);
}

w_scr_dish_main_qr_code_over_view_dialog::~w_scr_dish_main_qr_code_over_view_dialog()
{

}


w_scr_dish_main_qr_code_over_view::loadingRun::loadingRun(Ui::w_scr_dish_main_qr_code_over_view *ui) : ui(ui)
{

}

w_scr_dish_main_qr_code_over_view::loadingRun::~loadingRun()
{
    ui->label_loading->movie()->stop();
    ui->widget_ctrl->setEnabled(true);
}

void w_scr_dish_main_qr_code_over_view::loadingRun::show()
{
    ui->label_loading->movie()->start();
    ui->widget_ctrl->setEnabled(false);
}

void w_scr_dish_main_qr_code_over_view::loadingRun::setText(const QString &text)
{
//    ui->label_loading->movie()->stop();
//    ui->widget_ctrl->setEnabled(true);
    //
    ui->label_message->setText(text);
}
