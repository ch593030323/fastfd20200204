#include "kds_quit.h"
#include "ui_kds_quit.h"
#include "kds.h"
#include "vch_print_memodata.h"
#include "lds_pushbutton.h"
#include "lds.h"
#include "kds_quit_reason.h"
#include "lds_roundeddialog_rect_align.h"
#include "lds_messagebox.h"
#include <QTimer>
#include "cjson_help.h"
#include "w_bt_dish_flag.h"

kds_quit::kds_quit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::kds_quit)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("退货"));

    ui->tableWidget->appHorizontalHeader("vch_plan_id", "ID");
    ui->tableWidget->appHorizontalHeader("vch_dishname", QObject::tr("名称"));
    ui->tableWidget->appHorizontalHeader("vch_print_memo", QObject::tr("做法"));
    ui->tableWidget->appHorizontalHeader("num_back", QObject::tr("退货数量"));
    ui->tableWidget->appHorizontalHeader("int_orderflow", "", false);
    ui->tableWidget->appHorizontalHeader("ch_state", "", false);
    //
    ui->tableWidget->appHorizontalHeader("accept", "");
    ui->tableWidget->appHorizontalHeader("refuse", "");
    ui->tableWidget->appHorizontalHeader("step", "", false);
    ui->tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->tableWidget->setColumnWidth(ui->tableWidget->fieldIndex("vch_plan_id"), 50);
    ui->tableWidget->setColumnWidth(ui->tableWidget->fieldIndex("vch_dishname"), 180);
    ui->tableWidget->setColumnWidth(ui->tableWidget->fieldIndex("vch_print_memo"), 130);
    ui->tableWidget->setColumnWidth(ui->tableWidget->fieldIndex("num_back"), 120);
    ui->tableWidget->setColumnWidth(ui->tableWidget->fieldIndex("accept"), 60);
    ui->tableWidget->setColumnWidth(ui->tableWidget->fieldIndex("refuse"), 60);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(60);

    ui->pushButton_ok->setEnabled(false);
    //
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    //
    refresh();
}

kds_quit::~kds_quit()
{
    delete ui;
}

void kds_quit::took()
{
    this->accept();
}

void kds_quit::tobuttoncheck()
{
    lds_pushbutton *b = qobject_cast<lds_pushbutton *>(this->sender());
    if(b == 0)
        return;
    QString vch_memo;
    lds_query_db query;
    int k = b->getData("row").toInt();
    const QString type = b->getData("type").toString();
    if(type == "refuse") {
        kds_quit_reason dialog(this);
        if(QDialog::Rejected == lds_roundeddialog_rect_align_600x400(&dialog).exec()) {
            return;
        }
        vch_memo = dialog.reason();
    }
    //
    ui->tableWidget->removeWidget(k, "accept");
    ui->tableWidget->removeWidget(k, "refuse");
    ui->tableWidget->setSpan(k, ui->tableWidget->fieldIndex("accept"), 1, 2);
    //
    if(kds::selectHasNext(" select 1 from cey_u_orderdish_print_kt_quit_queue where  int_orderflow = " + ui->tableWidget->text(k, "int_orderflow"))) {
        if(type == "accept") {
            updateStateTransaction(k, KDS::quit_state_processing, vch_memo);
            ui->tableWidget->setNumber(k, "step", 10);
        }
        if(type == "refuse") {
            updateStateTransaction(k, KDS::quit_state_rejected, vch_memo);
        }
    } else {
        updateStateTransaction(k, KDS::quit_state_unexisted, vch_memo);
    }
}

void kds_quit::torefresh()
{
    if( 1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("刷新"), QObject::tr("确定"), QObject::tr("取消")) ) {
        return;
    }
    refresh();
}

void kds_quit::totimecheck()
{
    if("Y" == kds::selectValue(QString(" select ch_refresh_order_quit from cey_bt_kitchen_view where vch_plan_id = '%1' ")
                               .arg(kds::cey_bt_kitchen_view_data("vch_plan_id").toString()))) {
        kds::cey_bt_kitchen_view_update("ch_refresh_order_quit", "N");
        refresh();
    } else {
        int ch_state;
        QString vch_memo;
        QString int_orderflow;
        lds_query_db query;
        for(int k = 0; k < ui->tableWidget->rowCount(); k ++) {
            int_orderflow = ui->tableWidget->text(k, "int_orderflow");
            int step = ui->tableWidget->number(k, "step");
            if(step == 1) {
                updateStateTransaction(k, KDS::quit_state_expired);
                step = 0;
            }
            if(step > 1) {
                ui->tableWidget->setText(k, "accept", QObject::tr("正在退货") + QString("(%1)").arg(step));
                query.execSelect(" select ch_state, vch_memo from cey_u_orderdish_print_kt_quit_queue where int_orderflow = " + int_orderflow);
                if(query.next()) {
                    ch_state = query.recordValue("ch_state").toInt();
                    vch_memo = query.recordValue("vch_memo").toString();
                } else {
                    ch_state = KDS::quit_state_unexisted;
                }
                switch (ch_state) {
                case KDS::quit_state_finish:
                    updateStateTransaction(k, ch_state);
                    step = 0;
                    break;
                case KDS::quit_state_error:
                    updateStateTransaction(k, ch_state, vch_memo);
                    step = 0;
                    break;
                case KDS::quit_state_unexisted:
                    updateStateTransaction(k, ch_state);
                    step = 0;
                    break;
                }
                step -- ;
                ui->tableWidget->setNumber(k, "step", step);
            }
        }
        //
        int quit_finish_count = 0;
        for(int k = 0; k < ui->tableWidget->rowCount(); k ++) {
            int ch_state = ui->tableWidget->number(k, "ch_state");
            if(KDS::quit_state_wait == ch_state
                    || KDS::quit_state_processing == ch_state) {
                break;
            } else {
                quit_finish_count ++;
            }
        }

        ui->pushButton_ok->setEnabled(quit_finish_count == ui->tableWidget->rowCount());
        QTimer::singleShot(1000, this, SLOT(totimecheck()));
    }
}

void kds_quit::refresh()
{
    QMap<QString, qrtVariantPairList> backqueue;
    lds_query_db query;
    query.db.transaction();
    query.execSelect(QString(" select * from cey_u_orderdish_print_kt_quit_queue where vch_plan_id =  '%1'").arg(kds::cey_bt_kitchen_view_data("vch_plan_id").toString()));
    while(query.next()) {
        QString int_orderflow = query.recordValue("int_orderflow").toString();
        int ch_state = query.recordValue("ch_state").toInt();
        int cook_state = kds::selectValue(QString("select ch_state from cey_u_orderdish_print_kt_order where int_flowID = '%1' ").arg(int_orderflow)).toInt();
        //
        backqueue[int_orderflow] = qrtVariantPairList();
        kds::recordFill(query.record(), backqueue[int_orderflow]);
        if(ch_state == KDS::quit_state_wait) {
            if(KDS::cookPrepared != cook_state) {
                kds::updateTable("cey_u_orderdish_print_kt_quit_queue", qrtVariantPairList()
                                 << qrtVariantPair("ch_state", KDS::quit_state_rejected)
                                 << qrtVariantPair("vch_memo", KDS::cookStateName(cook_state)) ,
                                 QString("int_orderflow = '%1' ").arg(int_orderflow)
                                 );
                qrtVariant::pairSetValue(backqueue[int_orderflow], "ch_state", KDS::quit_state_rejected);
                qrtVariant::pairSetValue(backqueue[int_orderflow], "vch_memo", KDS::cookStateName(cook_state));
            }
        }
    }
    query.db.commit();

    for(int k = 0; k < ui->tableWidget->rowCount(); k ++) {
        QString int_orderflow = ui->tableWidget->text(k, "int_orderflow");
        if(backqueue.find(int_orderflow) == backqueue.end()) {
            setRowContent(k, KDS::quit_state_unexisted, backqueue);
        } else {
            setRowContent(k, qrtVariant::pairListValue(backqueue[int_orderflow], "ch_state").toInt(), backqueue);
            backqueue.remove(int_orderflow);
        }
    }
    for(QMap<QString, qrtVariantPairList>::iterator k = backqueue.begin(); k != backqueue.end(); k ++) {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setText(row, "int_orderflow", qrtVariant::pairListValue(k.value(), "int_orderflow").toString());
        setRowContent(row, qrtVariant::pairListValue(k.value(), "ch_state").toInt(), backqueue);
    }

    QTimer::singleShot(1000, this, SLOT(totimecheck()));
}

void kds_quit::setRowContent(int k, int ch_state, QMap<QString, qrtVariantPairList> &backqueue)
{
    if(KDS::quit_state_unexisted == ch_state) {
        //        ui->tableWidget->setText(k, "vch_plan_id", vch_plan_id);
        //        ui->tableWidget->setText(k, "vch_dishname", vch_dishname);
        ui->tableWidget->setText(k, "vch_print_memo", "");
        ui->tableWidget->setText(k, "num_back", "");
        //        ui->tableWidget->setText(k, "int_orderflow", int_orderflow);
        ui->tableWidget->setNumber(k, "ch_state", ch_state);

        ui->tableWidget->setText(k, "accept", KDS::quitStateName(ch_state));
        ui->tableWidget->setText(k, "refuse", "");
        ui->tableWidget->setText(k, "step", 0);

        ui->tableWidget->removeWidget(k, "accept");
        ui->tableWidget->removeWidget(k, "refuse");
        ui->tableWidget->setSpan(k, ui->tableWidget->fieldIndex("accept"), 1, 2);
        return;
    }
    QString int_orderflow = ui->tableWidget->text(k, "int_orderflow");
    const qrtVariantPairList &pair = backqueue[int_orderflow];
    //    int ch_state = qrtVariant::pairListValue(pair, "ch_state").toInt();
    QString num_back = qrtVariant::pairListValue(pair, "num_back").toString();
    QString vch_plan_id = qrtVariant::pairListValue(pair, "vch_plan_id").toString();
    //    QString int_orderflow = qrtVariant::pairListValue(pair, "int_orderflow").toString();
    QString vch_memo = qrtVariant::pairListValue(pair, "vch_memo").toString();
    QString ch_dishno = kds::selectValue(QString(" select ch_dishno from cey_u_orderdish where int_flowID = '%1' ")
                                         .arg(qrtVariant::pairListValue(pair, "int_orderflow").toString())).toString();
    QString vch_dishname = kds::selectValue(QString(" select vch_dishname from cey_bt_dish where ch_dishno = '%1' ").arg(ch_dishno)).toString();
    QString vch_print_memo = vch_print_memoData(qrtVariant::pairListValue(pair, "vch_print_memo").toString()).jsonToString(1, false);

    ui->tableWidget->setText(k, "vch_plan_id", vch_plan_id);
    ui->tableWidget->setText(k, "vch_dishname", vch_dishname);
    ui->tableWidget->setText(k, "vch_print_memo", vch_print_memo);
    ui->tableWidget->setText(k, "num_back", num_back);
    ui->tableWidget->setText(k, "int_orderflow", int_orderflow);
    ui->tableWidget->setNumber(k, "ch_state", ch_state);

    ui->tableWidget->setText(k, "accept", "");
    ui->tableWidget->setText(k, "refuse", "");
    ui->tableWidget->setText(k, "step", 0);

    ui->tableWidget->removeWidget(k, "accept");
    ui->tableWidget->removeWidget(k, "refuse");
    ui->tableWidget->setSpan(k, ui->tableWidget->fieldIndex("accept"), 1, 1);

    //
    lds_pushbutton *b;
    if(KDS::quit_state_wait == ch_state) {
        ui->tableWidget->setWidget(k, "accept", b = new lds_pushbutton);
        b->setData("row", k);
        b->setData("type", "accept");
        b->setFocusPolicy(Qt::NoFocus);
        //            b->setText(btn_text);
        b->setIcon(QPixmap(":/kds/button_ok.png"));
        b->setIconSize(QSize(30, 30));
        connect(b, SIGNAL(clicked()), this, SLOT(tobuttoncheck()));
    } else {
        ui->tableWidget->setText(k, "accept", KDS::quitStateName(ch_state) + (vch_memo.count() > 0 ? ":" : "") + vch_memo);
        ui->tableWidget->setSpan(k, ui->tableWidget->fieldIndex("accept"), 1, 2);
    }
    //
    if(KDS::quit_state_wait == ch_state) {
        ui->tableWidget->setWidget(k, "refuse", b = new lds_pushbutton);
        b->setData("row", k);
        b->setData("type", "refuse");
        b->setFocusPolicy(Qt::NoFocus);
        //            b->setText(QObject::tr("退货拒绝"));
        b->setIcon(QPixmap(":/kds/button_cancel.png"));
        b->setIconSize(QSize(30, 30));
        connect(b, SIGNAL(clicked()), this, SLOT(tobuttoncheck()));
    }
}

void kds_quit::updateStateTransaction(int row, int ch_state, const QString &vch_memo)
{
    QString errstring;
    lds_query_db query;
    ui->tableWidget->setNumber(row, "ch_state", ch_state);
    ui->tableWidget->setText(row, "accept", KDS::quitStateName(ch_state));
    QString int_orderflow = ui->tableWidget->text(row, "int_orderflow");

    query.db.transaction();
    if(KDS::quit_state_unexisted == ch_state) {
    }
    if(KDS::quit_state_rejected == ch_state){
        if(!query.execUpdate("cey_u_orderdish_print_kt_quit_queue", "ch_state", ch_state, "vch_memo", vch_memo, "int_orderflow = " + int_orderflow)) {
            query.db.rollback();
            return;
        }
    }
    if(KDS::quit_state_expired == ch_state
            || KDS::quit_state_processing == ch_state){
        if(!query.execUpdate("cey_u_orderdish_print_kt_quit_queue", "ch_state", ch_state, "int_orderflow = " + int_orderflow)) {
            query.db.rollback();
            return;
        }
    }
    if(KDS::quit_state_finish == ch_state) {
        if(kds::cey_u_orderdish_num_dish(int_orderflow, true) <= 0) {
            if(!kds::update_print_kt_dish(int_orderflow, KDS::cookCanceled, errstring)) {
                query.db.rollback();
                return;
            }
        }
    }
    query.db.commit();
}
