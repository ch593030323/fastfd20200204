#include "w_m_member_telcoming_table.h"
#include "ui_w_m_member_telcoming_table.h"
#include "public_sql.h"
#include "w_scr_dish_restaurant_dialog.h"
#include <QtDebug>
#include "restauranttableview.h"

w_m_member_telComing_table::w_m_member_telComing_table(const telComingData &member_from_tel, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_m_member_telComing_table)
{
    ui->setupUi(this);
    this->member_from_tel = member_from_tel;
    lds_query_hddpos  query;
    query.execSelect("select ch_typeno,  vch_typename  FROM cey_bt_table_type union select '%' as ch_typeno, '"+QObject::tr("所有类型")+"' as vch_typename ORDER BY ch_typeno ASC");
    while(query.next()) {
        ui->comboBox_tabletype->addItem(query.recordValue("vch_typename").toString(), query.recordValue("ch_typeno").toString());
    }
    //
    restraurantDelegate *delegatetable = new restraurantDelegate(ui->tableView);
    modeltable = new ftableview_standmodel_sql_table;
    modeltable->sql_cur =
            " select y.vch_tablename, y.ch_tableno, "
            + restraurantDelegate::sql_ch_billnos
            + "as ch_billnos, "
            " ifnull((select int_person from cey_u_table where cey_u_table.ch_billno = y.ch_billno), y.int_person) as int_person,"
            " (select num_cost from cey_u_table where cey_u_table.ch_billno = y.ch_billno) as num_cost "
            " from cey_bt_table y "
            " where "
            ///未满单
            + restraurantDelegate::sql_ch_billnos +" like '%,,%' and "
            ///not已开台未分单
            + " not("+restraurantDelegate::sql_ch_billnos +"like '%,,,,,,,,,' and ifnull(ch_billno, '') <> '') "
            ;
    modeltable->sqlother = modeltable->sql_cur + " and y.ch_typeno like '%1%' ";
    ui->tableView->setModel(modeltable);
    ui->tableView->setItemDelegate(delegatetable);
    ui->tableView->updateFixed_fillData(6, 4, 90, 90);
    ui->tableView->enable_release_clear_selection(true);
    ui->tableView->disConnectCenter();

    totabletype(0);

    connect(ui->pushButton_exit, SIGNAL(clicked()),  this, SLOT(toexit()));
    connect(ui->comboBox_tabletype,SIGNAL(currentIndexChanged(int)),this,SLOT(totabletype(int)));
    connect(ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(toopen(QModelIndex)));

    setWindowTitle(QObject::tr("餐桌"));
}

w_m_member_telComing_table::~w_m_member_telComing_table()
{
    delete ui;
}

void w_m_member_telComing_table::totabletype(int index)
{
    modeltable->toinitBySqlother(ui->comboBox_tabletype->itemData(index).toString());
}

void w_m_member_telComing_table::toexit()
{
    this->reject();
}

void w_m_member_telComing_table::toopen(const QModelIndex &index)
{
    QString ch_tableno = index.data(Qt::UserRole + 1 /*ch_tableno*/).toString();
    if(ch_tableno.toInt() < 0) {//内部使用
        ui->tableView->centerControl(index);//一般和discenter联合使用
        return;
    }

    if(w_scr_dish_restaurant_dialog::tablemessageOrder(
                this,
                ch_tableno,
                modeltable->sql_cur,
                false, //是否外部强制分单
                member_from_tel, //电话信息
                true //请求是否点单
                )) {
        ret_ch_tableno = ch_tableno;
        this->accept();
        return;
    }
    modeltable->refreshcur();
}
