#include "w_scr_dish_main_qr_code_orderdish.h"
#include "ui_w_scr_dish_main_qr_code_orderdish.h"
#include "ftableview_delegate.h"
#include "n_func.h"
#include <QtDebug>
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "transparentcenterdialog.h"
#include <QtDebug>
#include "cJSON.h"
#include "cjson_help.h"
#include "w_scr_dish_main_qr_code.h"
#include "fexpandmain_model_sqltablemodel_data.h"
#include "public_printby_ch_billno.h"

w_scr_dish_main_qr_code_orderdish::w_scr_dish_main_qr_code_orderdish(const QString &ch_billno, const QString &master_sn, const QString &ch_dishno, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::w_scr_dish_main_qr_code_orderdish),
      master_sn(master_sn),
      ch_dishno(ch_dishno),
      ch_billno(ch_billno)
{
    ui->setupUi(this);
    compop.setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    ui->comboBox->setParent(&compop);

    ui->pushButton_dish->setText(lds_query_hddpos::selectValue(QString(" select vch_dishname from cey_bt_dish where ch_dishno = '%1' ").arg(ch_dishno)).toString());
    ui->doubleSpinBox->setValue(1);//lds_query_hddpos::selectValue(QString(" select a.num_default from cey_bt_unit a, cey_bt_dish b where a.ch_unitno = b.ch_unitno and b.ch_dishno = '%1'; ").arg(ch_dishno)).toDouble());
    //
    cook_model = new ftableview_standmodel_sql_none;
    {
        ftableview_delegate *d = new ftableview_delegate(ui->tableView_cook);
        d->keepConnect(ui->tableView_cook);
        d->b_shwoGrid = true;
        QString pay_unit_prefix = n_func::f_get_sysparm("pay_unit_prefix");

        ui->tableView_cook->setModel(cook_model);
        ui->tableView_cook->setProperty("outer_stylesheet", "tableView_subtype");
        ui->tableView_cook->setItemDelegate(d);
        ui->tableView_cook->updateFixed_fillData(3, 2, 130, 40);
        ui->tableView_cook->setSelectionMode(QTableView::MultiSelection);

        QList<ftableview_standmodel_sql_none::noneData> listdata_list;
        lds_query_hddpos  query;
        query.execSelect(QString(" select a.ch_no, a.ch_dishno, b.vch_cp_memo, b.num_add_price from cey_bt_cp_dish a, cey_bt_cp_memo b where a.ch_no = b.ch_no and a.ch_dishno = '%1' ").arg(ch_dishno));
        while(query.next()) {
            listdata_list.append(ftableview_standmodel_sql_none::noneData(
                                     QString("%1(%2)").arg(query.recordValue("vch_cp_memo").toString(), pay_unit_prefix + query.recordValue("num_add_price").toString()),
                                     query.recordValue("ch_no")));
        }
        cook_model->setPrepareList(listdata_list);
        cook_model->toinit();

    }

    //
    suit_model = new ftableview_standmodel_sql_none;
    {
        ftableview_delegate *d = new ftableview_delegate(ui->tableView_suit);
        d->keepConnect(ui->tableView_suit);

        ui->tableView_suit->setModel(suit_model);
        ui->tableView_suit->setProperty("outer_stylesheet", "tableView_dish");
        ui->tableView_suit->setItemDelegate(d);
        ui->tableView_suit->updateFixed_fillData(2, 2, 200, 40);
        ui->tableView_suit->enable_release_clear_selection(true);

        compop.resize(200, 40);
        ui->comboBox->setGeometry(QRect(0, 0, compop.width(), compop.height()));
        ui->comboBox->resetTableView(40);
        ui->pushButton_dish->setFixedHeight(40);

        QList<ftableview_standmodel_sql_none::noneData> listdata_list;
        lds_query_hddpos  query;
        query.execSelect(QString(" select a.int_flowid, a.ch_suitno, b.ch_dishno, b.vch_dishname, a.num_num from cey_bt_dish_suit a, cey_bt_dish b where a.ch_dishno = b.ch_dishno and a.ch_suitno = '%1' ").arg(ch_dishno));
        while(query.next()) {
            listdata_list.append(ftableview_standmodel_sql_none::noneData(
                                     query.recordValue("vch_dishname").toString() + QString("(x%1)").arg(query.recordValue("num_num").toString()),//text
                                     query.recordValue("int_flowid"),//key
                                     ""//result
                                     ));
        }
        suit_model->setPrepareList(listdata_list);
        suit_model->toinit();
    }

    ui->label_cook->setHidden(cook_model->data_list().isEmpty());
    ui->tableView_cook->setHidden(cook_model->data_list().isEmpty());
    ui->label_suit->setHidden(suit_model->data_list().isEmpty());
    ui->tableView_suit->setHidden(suit_model->data_list().isEmpty());
    if(cook_model->data_list().count() > 0 || suit_model->data_list().count() > 0) {
        ui->doubleSpinBox->setEnabled(false);
        ui->pushButton_add->setHidden(true);
        ui->pushButton_sub->setHidden(true);
    }

    resize(400, 80);
    //
    this->setWindowTitle(MESSAGE_TITLE_VOID);
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->tableView_suit, SIGNAL(clicked(QModelIndex)), this, SLOT(tochangesuit(QModelIndex)));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(tosuitreplace(int)));
    connect(ui->pushButton_add, SIGNAL(clicked()), this, SLOT(toaddnum()));
    connect(ui->pushButton_sub, SIGNAL(clicked()), this, SLOT(tosubnum()));
}

w_scr_dish_main_qr_code_orderdish::~w_scr_dish_main_qr_code_orderdish()
{
    delete ui;
}

QByteArray w_scr_dish_main_qr_code_orderdish::zf()
{
    cJSON_help json(cJSON_Array);
    foreach(const QVariant &c, cook_model->currentSelected(1/*ch_no*/)) {
        cJSON_help::_cJSON_AddValueToArray(json, c);
    }
    return cJSON_help::getcJSONprint(json);
}

QByteArray w_scr_dish_main_qr_code_orderdish::tc()
{
    cJSON_help json(cJSON_Array);
    foreach(const QVariant &c, suit_model->currentSelected(2/*result*/)) {
        cJSON_help::_cJSON_AddValueToArray(json, c);
    }
    return cJSON_help::getcJSONprint(json);
}

void w_scr_dish_main_qr_code_orderdish::tochangesuit(const QModelIndex &index)
{
    QString int_flowid = index.data(Qt::UserRole + 1).toString();
    if(int_flowid == "") {
        return;
    }
    if(int_flowid == "-1") {
        suit_model->togeneratePre();
        return;
    }
    if(int_flowid == "-2") {
        suit_model->togenerateNext();
        return;
    }

    lds_query_hddpos  query;
    query.execSelect(QString(" select a.ch_dishno, b.vch_dishname, a.num_num from cey_bt_dish_suit a, cey_bt_dish b where a.ch_dishno = b.ch_dishno and a.int_flowid = %1 ").arg(int_flowid));
    query.next();
    QString ch_dishno_origin = query.recordValue("ch_dishno").toString();

    ui->comboBox->clear();
    ui->comboBox->addItem(query.recordValue("vch_dishname").toString() + QString("(x%1)").arg(query.recordValue("num_num").toString()), "");
    query.execSelect(QString("select a.ch_dishno, b.vch_dishname, a.num_num_change from cey_bt_suit_change a, cey_bt_dish b where a.ch_dishno = b.ch_dishno and int_flowid  = %1; ").arg(int_flowid));
    while(query.next()) {
        ui->comboBox->addItem(
                    query.recordValue("vch_dishname").toString() + QString("(x%1)").arg(query.recordValue("num_num_change").toString())//text
                   ,  ch_dishno_origin + "," + query.recordValue("ch_dishno").toString()//result
                    );
    }
    ui->comboBox->setCurrentIndex(ui->comboBox->findData(index.data(Qt::UserRole + 2/*result*/)));
    //
    compop.move(ui->tableView_suit->mapToGlobal(
                    QPoint(ui->tableView_suit->columnViewportPosition(index.column()), ui->tableView_suit->rowViewportPosition(index.row()))
                    )
                );
    compop.raise();
    compop.show();
}

void w_scr_dish_main_qr_code_orderdish::tosuitreplace(int row)
{
    if(ui->comboBox != qApp->focusWidget()) {
        return;
    }
    QModelIndex index =  ui->tableView_suit->currentIndex();
    if(index.isValid()) {
        suit_model->setData(index, ui->comboBox->itemText(row), Qt::UserRole/*text*/);
        suit_model->setData(index, ui->comboBox->itemData(row), Qt::UserRole + 2/*result*/);

        suit_model->set_list_data(suit_model->data(index, Qt::UserRole + 1), ui->comboBox->itemText(row), Qt::UserRole/*text*/);
        suit_model->set_list_data(suit_model->data(index, Qt::UserRole + 1), ui->comboBox->itemData(row), Qt::UserRole + 2/*result*/);
    }
}

void w_scr_dish_main_qr_code_orderdish::took()
{
    if(ui->doubleSpinBox->value() <= 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("点单数量不能小于等于0"));
        return;
    }
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("订单确认"), QObject::tr("确定"), QObject::tr("取消"))) {
        return;
    }
    fexpandmain_model_sqltablemodel  cache;
    lds_query_hddpos  query;
    QString errstring;
    _SHOW_LOADING(this);
    QString dish_sn;
    if(false == w_scr_dish_main_qr_code::qr_code_unique_sn(dish_sn, errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }

    lds_query_hddpos::tran_saction();
    w_scr_dish_main_qr_code::QrCodeDetail qr_detail;
    qr_detail = (w_scr_dish_main_qr_code::QrCodeDetail) {
            dish_sn,//获取最大的sn
            "",//nickname
            ch_dishno,
            ui->doubleSpinBox->value(),//number
            0,//price

            zf(),
            0,//mount
            tc(),
            1};
    //获取插入orderdish数据
    QString flowid_greater_than = QString(" int_flowid > %1 ").arg(lds_query_hddpos::selectValue(QString(" select max(int_flowid) from cey_u_orderdish where ch_billno = '%1'; ").arg(ch_billno)).toLongLong());
    QrCode_cey_u_orderdishList qr_orderdish_list;
    w_scr_dish_main_qr_code::orderdish_generate(QrCodeDetailList() << qr_detail, qr_orderdish_list);
    if(w_scr_dish_main_qr_code::orderdish_bill_append(ch_billno, qr_orderdish_list, errstring)) {
        //云端更新
        if(w_scr_dish_main_qr_code::qr_code_bill_do_DISHAPPEND(master_sn,
                                                               dish_sn,
                                                               cache.get_cey_u_table_value(ch_billno, "ch_tableno").toString(),
                                                               ch_dishno,
                                                               ui->doubleSpinBox->value(),
                                                               zf(),
                                                               tc(),
                                                               cache.get_cey_bt_dish_value(ch_dishno, "ch_dish_typeno").toString(),
                                                               errstring
                                                               )) {
            lds_query_hddpos::com_mit();
            //更新num_cost
            w_scr_dish_main_qr_code::QrCodePay qr_code_pay_data;
            w_scr_dish_main_qr_code::qr_code_has_paid(master_sn, qr_code_pay_data, errstring);
            query.execUpdate("cey_u_table", "num_cost", qr_code_pay_data.num_cost, qrtEqual("ch_billno", ch_billno));

            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            //消费单
            if(lds::sysconf->value("system_setting/luodanzongdan").toString() == "1") {
                public_printby_ch_billno printer(ch_billno);
                printer.toprint_preprint(lds::sysconf->value("system_setting/printerpath").toString(), QObject::tr("消费单"), flowid_greater_than);
            }
            this->accept();
            return;
        }
    }
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
}

void w_scr_dish_main_qr_code_orderdish::tocancel()
{
    this->reject();
}

void w_scr_dish_main_qr_code_orderdish::toaddnum()
{
    ui->doubleSpinBox->setValue(ui->doubleSpinBox->value() + 1);
}

void w_scr_dish_main_qr_code_orderdish::tosubnum()
{
    ui->doubleSpinBox->setValue(qMax(0.0, ui->doubleSpinBox->value() - 1));
}
