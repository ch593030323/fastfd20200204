#include "w_scr_dish_restaurant_dishinfo.h"
#include "ui_w_scr_dish_restaurant_dishinfo.h"
#include <QtDebug>
#include "lds_model_sqltablemodel_delegate_com.h"
#include<QTimer>

w_scr_dish_restaurant_dishinfo::w_scr_dish_restaurant_dishinfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_restaurant_dishinfo)
{
    ui->setupUi(this);
    comboBox_additem();
    //tableview
    querymodel = new report_querymodel;
    ui->tableView->setTransferHheader();
    ui->tableView->setModel(querymodel);
    ui->tableView->setSelectionMode(QTableView::SingleSelection);
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(torefresh()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_up, SIGNAL(clicked()), ui->tableView, SLOT(toPageUp()));
    connect(ui->pushButton_down, SIGNAL(clicked()), ui->tableView, SLOT(toPageDown()));
    QTimer::singleShot(10,this, SLOT(torefresh()));
}

w_scr_dish_restaurant_dishinfo::~w_scr_dish_restaurant_dishinfo()
{
    delete ui;
}

void w_scr_dish_restaurant_dishinfo::deleteItemDelegate(const QString &fieldname)
{
    if(ui->tableView->itemDelegateForColumn(querymodel->fieldIndex(fieldname))) {
        delete ui->tableView->itemDelegateForColumn(querymodel->fieldIndex(fieldname));
    }
}

void w_scr_dish_restaurant_dishinfo::comboBox_additem()
{
    //由于linguist翻译报警告，所以增加本函数
    ui->comboBox->addItem(QObject::tr("餐桌类型统计"),
                          " SELECT "
                          "    a.ch_typeno,"
                          "    count(a.ch_tableno) as num_table,"
                          "    sum(if(ifnull(a.ch_billno, '') = '', 1, 0)) as num_kongxian,"
                          "    sum(if(ifnull(a.ch_billno, '') = '', 0, 1)) as num_kaitai,"
                          "    sum(b.num_cost) as num_cost"
                          " FROM"
                          "    cey_bt_table a  left join  cey_u_table b ON a.ch_billno = b.ch_billno"
                          "    group by a.ch_typeno "
                          );
    ui->comboBox->addItem(QObject::tr("餐桌明细统计"),
                          " Select "
                          "    a.ch_typeno,  a.vch_tablename, if(isnull(a.ch_billno) or a.ch_billno = '', 'N', 'Y') as ch_state, b.num_cost"
                          " from"
                          "    cey_bt_table a left join cey_u_table b on a.ch_billno = b.ch_billno"
                          );
    ui->comboBox->addItem(QObject::tr("消费明细统计"),
                          "  select "
                          "  a.ch_typeno,"
                          "  a.ch_tableno,"
                          "  (select  num_cost from cey_u_table  where cey_u_table.ch_billno = a.ch_billno) as num_cost,"
                          "  b.ch_dishno,"
                          "  b.num_num - b.num_back as num_dish"
                          "  from"
                          "  cey_bt_table a,"
                          "  cey_u_orderdish b"
                          " where"
                          "  a.ch_billno = b.ch_billno"
                          );
}

void w_scr_dish_restaurant_dishinfo::torefresh()
{
    //query
    querymodel->setQuery(ui->comboBox->curusrdata().toString());
    //clear
    ui->tableView->clearSpans();
    for(int column = 0; column < ui->tableView->horizontalHeader()->count(); column ++ ) {
        lds_tableview_delegate_virtual *d = static_cast<lds_tableview_delegate_virtual *>(ui->tableView->itemDelegateForColumn(column));
        if(d) {//此处，static_cast 强转时，会变成delegate_check, 结果
            d->disable_paint_transfer();
        }
    }

    //setdelegate
    switch(ui->comboBox->currentIndex()) {
    case 0:
    {
        deleteItemDelegate("ch_typeno");
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("ch_typeno"),
                                                new lds_model_sqltablemodel_delegate_com(this, "cey_bt_table_type",  "ch_typeno", "vch_typename"));
    }
        break;
    case 1:
    {
        deleteItemDelegate("ch_typeno");
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("ch_typeno"),
                                                new lds_model_sqltablemodel_delegate_com(this, "cey_bt_table_type",  "ch_typeno", "vch_typename"));

        deleteItemDelegate("ch_state");
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("ch_state"),
                                                new lds_model_sqltablemodel_delegate_com(this, ldsVariantMap("N", QObject::tr("空闲"), "Y", QObject::tr("在用"))));

        ui->tableView->tableviewSetpan(ui->tableView, querymodel->fieldIndex("ch_typeno"), QList<int>() << querymodel->fieldIndex("ch_typeno"));
    }
        break;
    case 2:
    {
        deleteItemDelegate("ch_typeno");
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("ch_typeno"),
                                                new lds_model_sqltablemodel_delegate_com(this, "cey_bt_table_type",  "ch_typeno", "vch_typename"));

        deleteItemDelegate("ch_dishno");
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("ch_dishno"),
                                                new lds_model_sqltablemodel_delegate_com(this, "cey_bt_dish",  "ch_dishno", "vch_dishname"));

        deleteItemDelegate("ch_tableno");
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("ch_tableno"),
                                                new lds_model_sqltablemodel_delegate_com(this, "cey_bt_table",  "ch_tableno", "vch_tablename"));

        ui->tableView->tableviewSetpan(ui->tableView, querymodel->fieldIndex("ch_typeno"),
                                       QList<int>() << querymodel->fieldIndex("ch_typeno"), false);
        ui->tableView->tableviewSetpan(ui->tableView, querymodel->fieldIndex("ch_tableno"),
                                       QList<int>() << querymodel->fieldIndex("ch_tableno"), false);
    }
        break;
    }
}

void w_scr_dish_restaurant_dishinfo::toexit()
{
    this->reject();
}
