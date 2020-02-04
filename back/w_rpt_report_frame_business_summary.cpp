#include "w_rpt_report_frame_business_summary.h"
#include "ui_w_rpt_report_frame_business_summary.h"
#include "lds_query_hddpos.h"
#include <QtDebug>
#include "n_func.h"
#include "w_rpt_report_dataexport.h"
#include "w_rpt_report_printformat.h"
#include "public_sql.h"
#include "lds.h"
#include <QTimer>

w_rpt_report_frame_business_summary::w_rpt_report_frame_business_summary(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_rpt_report_frame_business_summary)
{
    ui->setupUi(this);

    lds::setwflagFrameless(this);
    querymodel = new report_querymodel(this);
    ui->tableView->setTransferHheader();
    ui->tableView->setModel(querymodel);
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    standmodel2 = new report_standmodel(this);
    ui->tableView_2->setModel(standmodel2);
    ui->tableView_2->verticalHeader()->hide();
    ui->tableView_2->horizontalHeader()->hide();
    ui->tableView_2->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_2->resize_decimal_enable();

    ui->comboBox->addItem(QObject::tr("品项统计"));
    ui->comboBox->addItem(QObject::tr("品类统计"));


    QDateTime dt = n_func::f_get_sysdatetime();
    w_rpt_report::reportBeginDtSet(ui->dateTimeEdit);
    ui->dateTimeEdit_2->setDateTime(dt);


    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(to_exit()));
    connect(ui->pushButton_query, SIGNAL(clicked()), this, SLOT(to_query()));
    connect(ui->pushButton_export, SIGNAL(clicked()), this, SLOT(to_export()));
    connect(ui->pushButton_print, SIGNAL(clicked()), this, SLOT(to_print()));

    QTimer::singleShot(0, this, SLOT(right_check_and_init()));
}

w_rpt_report_frame_business_summary::~w_rpt_report_frame_business_summary()
{
    delete ui;
}

void w_rpt_report_frame_business_summary::showLoginerContent()
{
    if(lds::gs_operid == "0000") {
        is_show_login_data= false;
        return;
    }

    lds_query_hddpos  query;
    query.execSelect(QString("select * from cey_bt_dish_type_oper where ch_state = 'Y' and vch_operid = '%1' ;").arg(lds::gs_operid));
    is_show_login_data = query.next();
}

void w_rpt_report_frame_business_summary::right_check_and_init()
{
    if(false == backheader::operate_right_check_code(this, "0224")) {//商品销售统计
        this->reject();
        return;
    }
    to_query();
}

void w_rpt_report_frame_business_summary::to_query()
{
    //
    QString sql0 = QString(
                " sum(a.num_num - a.num_back) as num_dish,"
                " sum((a.num_num - a.num_back) * a.num_price ) as num_total,"
                " sum((a.num_num - a.num_back) * a.num_price  * (1 - a.int_discount * 0.01)) as num_discount, "
                " sum(if(a.ch_presentflag = 'Y',  (a.num_num - a.num_back) * a.num_price,   0)) as num_present, "
                " sum(if(a.ch_presentflag = 'Y',  0,  (a.num_num - a.num_back) * a.num_price * 0.01 * a.int_discount)) as num_total2,"
                " sum(a.num_price_add) as num_price_add,"
                " sum(if(a.ch_presentflag = 'Y',  0,  (a.num_num - a.num_back) * a.num_price * 0.01 * a.int_discount) + a.num_price_add) as num_total3"
                " FROM cey_u_orderdish a,   cey_u_checkout_master b, cey_bt_dish c, cey_bt_dish_type d"
                " WHERE a.ch_dishno = c.ch_dishno"
                " AND a.ch_payno = b.ch_payno"
                " AND b.ch_state = 'Y'"
                " and c.ch_dish_typeno = d.ch_dish_typeno"
                " and b.dt_operdate >= '%1' and b.dt_operdate <= '%2' "
                + (is_show_login_data ? qrtEqual(" and b.vch_operid", lds::gs_operid) : "")
                )
            .arg(ui->dateTimeEdit->dateTimeStr())
            .arg(ui->dateTimeEdit_2->dateTimeStr())
            ;
    if(ui->comboBox->currentIndex() == 0) {
        //品项统计
        querymodel->setQuery(QString("SELECT "
                                     "c.vch_dishname,"
                                     + sql0 +
                                     " Group by a.ch_dishno ;")
                             )
                ;
    }

    if(ui->comboBox->currentIndex() == 1) {
        //品类统计
        querymodel->setQuery(QString(" SELECT"
                                     " d.vch_dish_typename,"
                                     + sql0 +
                                     " GROUP BY  c.ch_dish_typeno;")
                             );
    }

    ui->tableView->resizeColumnsToContents();

    standmodel2->removeRows(0, standmodel2->rowCount());
    QList<QStandardItem *> items;
    lds_query_hddpos  query;
    ////////////////数量 小计 总计 加价  总计（加价）
    query.execSelect("select " + sql0);
    query.next();
    items.clear();
    items << new QStandardItem("num_dish") << standmodel2->newItem(query.recordValue("num_dish"));
    items << new QStandardItem("num_total") << standmodel2->newItem(query.recordValue("num_total"));
    items << new QStandardItem("num_total2") << standmodel2->newItem(query.recordValue("num_total2"));
    items << new QStandardItem("num_price_add") << standmodel2->newItem(query.recordValue("num_price_add"));
    items << new QStandardItem("num_total3") << standmodel2->newItem(query.recordValue("num_total3"));
    standmodel2->appendRow(items);
    ////////////////台数 人数
    query.execSelect(QString("SELECT"
                             "    count(a.ch_tableno) as num_table_count,"
                             "    sum(a.int_person) int_person"
                             " FROM"
                             "    cey_u_table a, cey_u_checkout_master b"
                             " WHERE"
                             "    a.ch_payno = b.ch_payno"
                             " AND b.ch_state = 'Y'"
                             " AND b.dt_operdate >= '%1'"
                             " AND b.dt_operdate <= '%2'"
                             + (is_show_login_data ? qrtEqual(" and b.vch_operid", lds::gs_operid) : "")
                             )
                     .arg(ui->dateTimeEdit->dateTimeStr())
                     .arg(ui->dateTimeEdit_2->dateTimeStr())
                     );
    query.next();
    items.clear();
    items << new QStandardItem("num_table_count") << standmodel2->newItem(query.recordValue("num_table_count").toDouble());
    items << new QStandardItem("int_person") << standmodel2->newItem(query.recordValue("int_person"));
    standmodel2->appendRow(items);

    ////////////////消费 折扣等
    query.execSelect(QString("SELECT"
                             " IFNULL(sum(b.num_cost), 0) as num_cost,"
                             " IFNULL(sum(b.num_discount), 0) as num_discount,"
                             " IFNULL(sum(b.num_present), 0) as num_present,"
                             " IFNULL(sum(b.num_rate), 0) as num_rate,"
                             " IFNULL(sum(b.num_blotout), 0) as num_blotout,"
                             " IFNULL(sum(b.num_service), 0) as num_service,"
                             " IFNULL(sum(b.num_lowcost), 0) as num_lowcost,"
                             " IFNULL(sum(b.num_free), 0) as num_free,"
                             " IFNULL(sum(b.num_cash), 0) as num_cash"
                             " FROM"
                             " cey_u_table a,  cey_u_checkout_master b"
                             " WHERE a.ch_payno = b.ch_payno"
                             " AND b.ch_state = 'Y'"
                             " AND b.dt_operdate >= '%1'"
                             " AND b.dt_operdate <= '%2' "
                             + (is_show_login_data ? qrtEqual(" and b.vch_operid", lds::gs_operid) : "")
                             )
                     .arg(ui->dateTimeEdit->dateTimeStr())
                     .arg(ui->dateTimeEdit_2->dateTimeStr())
                     );
    query.next();

    items.clear();
    items << new QStandardItem("num_cost") << standmodel2->newItem(query.recordValue("num_cost"));
    items << new QStandardItem("num_discount") << standmodel2->newItem(query.recordValue("num_discount"));
    items << new QStandardItem("num_present") << standmodel2->newItem(query.recordValue("num_present"));
    items << new QStandardItem("num_rate") << standmodel2->newItem(query.recordValue("num_rate"));
    items << new QStandardItem("num_blotout") << standmodel2->newItem(query.recordValue("num_blotout"));
    standmodel2->appendRow(items);

    items.clear();
    items << new QStandardItem("num_service") << standmodel2->newItem(query.recordValue("num_service"));
    items << new QStandardItem("num_lowcost") << standmodel2->newItem(query.recordValue("num_lowcost"));
    items << new QStandardItem("num_free") << standmodel2->newItem(query.recordValue("num_free"));
    items << new QStandardItem("num_cash") << standmodel2->newItem(query.recordValue("num_cash"));
    standmodel2->appendRow(items);


    ////////////////收银方式 收银实收 收银次数
    items.clear();
    query.execSelect(QString("SELECT"
                             " a.ch_paymodeno,"
                             " (SELECT d.vch_paymodename FROM cey_bt_paymode d WHERE d.ch_paymodeno = a.ch_paymodeno) as vch_paymodename,"
                             " sum(a.num_realamount) as num_realamount,"
                             " count(a.ch_paymodeno) as num_paymode_count"
                             " FROM cey_u_checkout_detail a, cey_u_checkout_master b"
                             " WHERE a.ch_payno = b.ch_payno"
                             " AND b.dt_operdate >= '%1'"
                             " AND b.dt_operdate <= '%2'"
                             " AND b.ch_state = 'Y' "
                             " AND a.ch_paymodeno <> 'ZZ' "
                             + (is_show_login_data ? qrtEqual(" and b.vch_operid", lds::gs_operid) : "")
                             +" GROUP BY  a.ch_paymodeno  ORDER BY a.ch_paymodeno desc;")
                     .arg(ui->dateTimeEdit->dateTimeStr())
                     .arg(ui->dateTimeEdit_2->dateTimeStr())
                     );
    while(query.next()) {
        items << standmodel2->newItem(query.recordValue("vch_paymodename"));
        items << standmodel2->newItem(query.recordValue("num_realamount"));

        if(items.count() == 10) {
            standmodel2->appendRow(items);
            items.clear();
        }
    }
    if((items).count() > 0) {
        standmodel2->appendRow(items);
    }
    ui->tableView_2->resizeColumnsToContents(true);
    ui->tableView_2->setFixedHeight(ui->tableView_2->verticalHeader()->count() * ui->tableView_2->verticalHeader()->defaultSectionSize());
}

void w_rpt_report_frame_business_summary::to_export()
{
    w_rpt_report_dataExport dialog(ui->tableView, ui->tableView_2, this);
    dialog.setWindowTitle(this->windowTitle());
    dialog.defFilepath(this->windowTitle());
    transparentCenterDialog(&dialog).exec();
}

void w_rpt_report_frame_business_summary::to_print()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());

    w_rpt_report_printFormat *dialog
            = new w_rpt_report_printFormat(w_rpt_report_printFormat::show_model_quick, this->windowTitle(), lds::gs_operid, n_func::f_get_sysdatetime_str(), this->metaObject()->className(),
                                           ui->dateTimeEdit->dateTimeStr(), ui->dateTimeEdit_2->dateTimeStr(),
                                           ui->tableView, ui->tableView_2,
                                           0);
    dialog->pop(b, this);
}

void w_rpt_report_frame_business_summary::to_exit()
{
    this->reject();
}
