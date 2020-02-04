#include "w_scr_dish_restaurant_table_select.h"
#include "ui_w_scr_dish_restaurant_table_select.h"
#include "ftableview_standmodel_sql.h"
#include "public_sql.h"
#include "lds_query_hddpos.h"
#include <QtDebug>
#include "w_scr_dish_restaurant_div_pop.h"

w_scr_dish_restaurant_table_select::w_scr_dish_restaurant_table_select(const QString &unch_tableno,QWidget *parent) :
    QDialog(parent)
{
    ui = new Ui_w_scr_dish_restaurant_table_select;
    ui->setupUi(this);

    //4
    lds_query_hddpos  query;
    query.execSelect("select ch_typeno,  vch_typename  FROM cey_bt_table_type union select '%' as ch_typeno, '"+QObject::tr("所有类型")+"' as vch_typename ORDER BY ch_typeno ASC");
    while(query.next()) {
        ui->comboBox_tabletype->addItem(query.recordValue("vch_typename").toString(), query.recordValue("ch_typeno").toString());
    }
    //5
    restraurantDelegate *delegatetable = new restraurantDelegate(this);
    modeltable = new ftableview_standmodel_sql_table;
    modeltable->sql_cur =
                " select y.vch_tablename, y.ch_tableno, "
                + restraurantDelegate::sql_ch_billnos
                + "as ch_billnos, "
                " ifnull((select int_person from cey_u_table where cey_u_table.ch_billno = y.ch_billno), y.int_person) as int_person,"
                " (select num_cost from cey_u_table where cey_u_table.ch_billno = y.ch_billno) as num_cost "
                " from cey_bt_table y"
                " where "
                " ch_tableno <> '"+unch_tableno+"'"
                ///未开台餐桌
                + " and "+restraurantDelegate::sql_ch_billnos +" <> ',,,,,,,,,'  ";
            ;

    modeltable->sqlother = modeltable->sql_cur + " and y.ch_typeno like '%1' ";
    modeltable->sql_cur = modeltable->sqlother.arg("%");
    ui->tableView_table->setModel(modeltable);
    ui->tableView_table->setItemDelegate(delegatetable);
    ui->tableView_table->updateFixed_fillData_addAnimation(8, 4, 90, 90);
    ui->tableView_table->enablePop(restaurantTableView::pop_single| restaurantTableView::pop_select_KAITAI);
    //6
    modeltable->refreshcur();

    //7
    connect(ui->comboBox_tabletype,SIGNAL(currentIndexChanged(int)),this,SLOT(totabletype(int)));
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));

    this->setWindowTitle(QObject::tr("餐桌"));
}

w_scr_dish_restaurant_table_select::~w_scr_dish_restaurant_table_select()
{
    delete ui;
}

QString w_scr_dish_restaurant_table_select::get_ch_tableno()
{
    QString ch_tableno = ch_tableno_check_map.keys().value(0);
    return ch_tableno;
}

QString w_scr_dish_restaurant_table_select::get_ch_divno()
{
    QString ch_divno = ch_tableno_check_map.value(get_ch_tableno()).split(",", QString::SkipEmptyParts).value(0);
    return ch_divno;
}

QString w_scr_dish_restaurant_table_select::tabledesc()
{
    QString ch_tableno = ch_tableno_check_map.keys().value(0);
    QString ch_divno = ch_tableno_check_map.value(ch_tableno).split(",", QString::SkipEmptyParts).value(0);
    QString ret = QObject::tr("餐桌号") +":"+ch_tableno;
    if(!ch_divno.isEmpty()) {
        ret += ","+QObject::tr("分单号")+ch_divno;
    }
    return ret;

}

QString w_scr_dish_restaurant_table_select::get_ch_billno()
{
    QString ch_tableno = ch_tableno_check_map.keys().value(0);
    int ch_divno = ch_tableno_check_map.value(ch_tableno).split(",", QString::SkipEmptyParts).value(0).toInt();

    return lds_scr_table::cey_bt_table_ch_billno_get(ch_tableno, ch_divno);
}

void w_scr_dish_restaurant_table_select::refresh()
{
    modeltable->refreshcur();
}

void w_scr_dish_restaurant_table_select::totabletype(int index)
{
    modeltable->toinitBySqlother(ui->comboBox_tabletype->itemData(index).toString());
}

void w_scr_dish_restaurant_table_select::took()
{
    QList<QVariantList/*一个deep下的list*/> datalist =  modeltable->get_value_select_deep(QList<int/*deep*/>() << 1 << 2 << 7);
    QVariantList checkdata1 = datalist[0];
    QVariantList checkdata2 = datalist[1];
    QVariantList checkdata7 = datalist[2];

    ch_tableno_check_map.clear();

    if(checkdata1.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有行被选中"));
        return;
    }
    for(int k = 0, count = checkdata2.count(); k < count; k++) {
        QString ch_tableno = checkdata1[k].toString();
        QString ch_billnos = checkdata2[k].toString();
        QMap<int, QString> ch_billno_map = restraurantDelegate::get_ch_billno_div_map(ch_billnos);
        if(restraurantDelegate::ch_billno_is_div(ch_billno_map)) {//可以分单32
            QString checklist = checkdata7[k].toString();
            if(checklist.isEmpty()) {

            } else {
                ch_tableno_check_map.insert(ch_tableno, checklist);
            }
        } else {
            ch_tableno_check_map.insert(ch_tableno, "");
        }
    }
    if(ch_tableno_check_map.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有行被选中"));
        return;
    }
    this->accept();
}

void w_scr_dish_restaurant_table_select::tocancel()
{
    this->reject();
}
