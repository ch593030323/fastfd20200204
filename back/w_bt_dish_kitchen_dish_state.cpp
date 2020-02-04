#include "w_bt_dish_kitchen_dish_state.h"
#include "ui_w_bt_dish_kitchen_dish_state.h"

#include "lds_query_hddpos.h"
#include "w_bt_dish.h"
#include "w_bt_dish_kitchen_dish.h"
#include <QScrollBar>
#include "lds_model_sqltablemodel_delegate_com.h"
#include <QTableWidget>
#include "public_logindialog.h"

w_bt_dish_kitchen_dish_state::w_bt_dish_kitchen_dish_state(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_bt_dish_kitchen_dish_state)
{
    ui->setupUi(this);
    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    
    tablemodel->setTable("cey_u_orderdish_print_tr");
    tablemodel->save_set_header("ch_dishno", QObject::tr("商品"));
    tablemodel->save_set_header("ch_billno", QObject::tr("单号"));
    //强制执行select
    ui->tableView->setModel(tablemodel);
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_dishno"), new lds_model_sqltablemodel_delegate_com(
                                                parent, tablemodel, "cey_bt_dish","ch_dishno","vch_dishname"
                                                ));
    ui->tableView->setSelectionMode(QTableView::MultiSelection);
    ui->tableView->setColumnWidth(tablemodel->fieldIndex("ch_dishno"), 300);
    ui->tableView->setColumnWidth(tablemodel->fieldIndex("ch_billno"), 160);

    connect(ui->pushButton_exit,SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_refresh,SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->pushButton_up,SIGNAL(clicked()),ui->tableView,SLOT(toPageUp()));
    connect(ui->pushButton_down,SIGNAL(clicked()),ui->tableView,SLOT(toPageDown()));
    connect(ui->pushButton_kitchen_print,SIGNAL(clicked()),this,SLOT(to_kitchen_print()));
    connect(ui->radioButton_lost, SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->radioButton_printed, SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->pushButton_lost_clear,SIGNAL(clicked()),this,SLOT(tolostclear()));
    QTimer::singleShot(0, this, SLOT(torefresh()));

}

w_bt_dish_kitchen_dish_state::~w_bt_dish_kitchen_dish_state()
{
    delete ui;
}

bool w_bt_dish_kitchen_dish_state::kitchen_data_adjust_dbtrans()
{
    lds_query_hddpos  query;
    lds_query_hddpos  querygoing;
    QString state =  QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_FINISH));
    QString k3o3_state;
    QString k3o3_id;
    int int_flowID;

    lds_query_hddpos::tran_saction();
    query.execSelect(QString("select int_flowID, vch_plan_k3o3_state, vch_plan_k3o3_id from cey_u_orderdish where vch_plan_k3o3_state <> '%1' ")
               .arg(state));

    while(query.next()) {
        int_flowID = query.recordValue("int_flowID").toInt();
        k3o3_state = query.recordValue("vch_plan_k3o3_state").toString();
        k3o3_id = query.recordValue("vch_plan_k3o3_id").toString();
        QString r;
        for(int k = 0; k < 6; k++) {
            if(k3o3_id.mid(k*2, 2) == "00") {
                r += "1";
            } else {
                r +=k3o3_state.mid(k, 1);
            }
        }
        if(r == state) {
            if(!querygoing.execUpdate("cey_u_orderdish",
                                      "vch_plan_k3o3_state",state,
                                      qrtEqual("int_flowID", int_flowID))) {
            }
        }
    }
    lds_query_hddpos::com_mit();
    return true;
}

void w_bt_dish_kitchen_dish_state::toexit()
{
    this->reject();
}

void w_bt_dish_kitchen_dish_state::torefresh()
{
    static int onepagestep = -1;
    if(onepagestep == -1) {
        QTableWidget t;
        t.setColumnCount(1);
        onepagestep = ( ui->tableView->height() - ui->tableView->horizontalScrollBar()->height()- t.horizontalHeader()->height() ) / ui->tableView->verticalHeader()->defaultSectionSize();
    }

    QElapsedTimer etimer;
    etimer.start();
    QString state =  QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_FINISH));
    QString filter;
//    if(!kitchen_data_adjust_dbtrans()) {
//        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误"));
//        return;
//    }

    ui->pushButton_lost_clear->setText(QObject::tr("补打"));
    filter = QString("vch_plan_k3o3_state = '%1' ").arg(state);//已厨打的
    if(ui->radioButton_lost->isChecked()) {
        ui->pushButton_lost_clear->setText(QObject::tr("清除漏打单"));
        filter = QString("vch_plan_k3o3_state <> '%1' ").arg(state);//未厨打的
    }

    //已设置厨打的
    filter += " and vch_plan_k3o3_id <> '"+QString(12, '0')+"' order by ch_billno  desc ";

    tablemodel->setFilter(filter);

    if(ui->radioButton_lost->isChecked()) {
        ui->tableView->setSelectionMode(QTableView::SingleSelection);
    } else {
        ui->tableView->setSelectionMode(QTableView::MultiSelection);
    }
}

void w_bt_dish_kitchen_dish_state::to_kitchen_print()
{
    w_bt_dish_kitchen_dish *dialog = this->findChild<w_bt_dish_kitchen_dish *>("w_bt_dish_kitchen_dish");
    if(dialog == 0) {
        dialog = new w_bt_dish_kitchen_dish(this);
        dialog->setObjectName("w_bt_dish_kitchen_dish");
        dialog->setWindowTitle(QObject::tr("厨房打印机"));
        dialog->resize(lds::MAIN_WINDOW_SIZE);
        dialog->move(0, 0);
    }
    dialog->refresh();
    dialog->exec();
}

void w_bt_dish_kitchen_dish_state::tolostclear()
{
    lds_query_hddpos  query;
    QString state;
    QString id;
    if(ui->radioButton_lost->isChecked()) {//清除漏打单
        state =  QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_FINISH));
        id = QString(12, '0');
        if(tablemodel->rowCount() == 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
            return;
        }
        if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, _TEXT_SLOT(this), QObject::tr("确定"), QObject::tr("取消"))) {
            /** */ lds_query_hddpos::tran_saction();
            //清除 未厨打的 的bug
            if(!query.execDelete("cey_u_orderdish_print_tr", QString(" vch_plan_k3o3_state <> '%1' ")
                           .arg(state)
                           )) {
                goto rollback;
            }
            lds_query_hddpos::com_mit();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            torefresh();
            return;
        }
    } else {//补打
        QModelIndexList indexlist = ui->tableView->selectionModel()->selectedRows();
        if(indexlist.isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有行被选中"));
            return;
        }
        /** */ lds_query_hddpos::tran_saction();
        state =  QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_MIN_DEF));
        foreach(const QModelIndex &index, indexlist) {
            if(!query.execUpdate("cey_u_orderdish","vch_plan_k3o3_state", state,
                                 qrtEqual("int_flowID", tablemodel->model_data(index.row(), "int_orderflow").toInt()))) {
                goto rollback;
            }
        }
        lds_query_hddpos::com_mit();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        torefresh();
        public_sql::save_login->to_everytime_to_print_failed_kitchen_bill();
        return;
    }

    return;
rollback:
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
}

QString w_bt_dish_kitchen_dish_state::get_state_desc(const QByteArray &state, int index)
{
    //state:221002
    if(state.count() <= index) return "";
    if(index < 0) return "";
    //
    char s = state[index];
    w_bt_dish::kitchenState key = (w_bt_dish::kitchenState)QByteArray().append(s).toInt(0, 16);
    //
    if(key == w_bt_dish::KITCHEN_FINISH)  return "";
    if(key == w_bt_dish::KITCHEN_VOID) return "";
    //
    return QObject::tr("正在") + "\"" +w_bt_dish::k2o3_name(key)+"\"";
}
