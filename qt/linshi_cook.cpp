#include "linshi_cook.h"
#include "ui_linshi_cook.h"
#include <QDebug>
#include "backheader.h"
#include "ftableview_standmodel_sql.h"
#include "ftableview_delegate.h"
#include "w_bt_dish_base.h"
#include "lds_messagebox.h"
#include <QVariant>
#include "w_bt_dish_kitchen_dish.h"
#include "fexpandmain_2.h"

linshi_cook::linshi_cook(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::linshi_cook)
{
    ui->setupUi(this);

    model_dish = new ftableview_standmodel_sql;
    model_dish->sql_cur = "select CONCAT(vch_dishname,'\n',num_price), ch_dishno as dishno1, ch_dishno as dishno2   from cey_bt_dish where ch_stopflag <> 'Y' and ch_tempflag = 'Y'";
    ui->tableView->setModel(model_dish);
    /// ---------------delegate -------------------begin
    //修改delegate的某些样式单独修改
    ftableview_delegate *d = new ftableview_delegate(ui->tableView);
    ui->tableView->setProperty("outer_stylesheet", "tableView_dish");
    d->setColorLight(3);
    d->keepConnect(ui->tableView);
    d->set_pay_unit_prefix(n_func::f_get_sysparm("pay_unit_prefix", " "));
    ui->tableView->setItemDelegate(d);
    /// ---------------delegate -------------------end
    ui->tableView->updateFixed_fillData_addAnimation(6, 4, 100, 100);

    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_new,SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_kitchen_plan,SIGNAL(clicked()),this,SLOT(tokitchen_plan()));
    connect(ui->pushButton_change, SIGNAL(clicked()), this, SLOT(tochange()));

    model_dish->initDelay();
}

linshi_cook::~linshi_cook()
{
    delete ui;
}

void linshi_cook::tonew()
{
    QPushButton *b = qobject_cast<QPushButton* >(this->sender());
    w_bt_dish_base_new dialog(this, "", "", "", "Y");
    dialog.setWindowTitle(b->text());
    connect(&dialog,SIGNAL(signalsave()),model_dish,SLOT(refreshcur()));
    transparentCenterDialog(&dialog, false).exec();
}

void linshi_cook::took()
{
    if(false == ui->tableView->currentIndex().isValid()) {
        lds_messagebox::warning(this, _TEXT_SLOT(this), QObject::tr("没有商品被选中"));
        return;
    }
    ch_dishno = ui->tableView->currentIndex().data(Qt::UserRole+1).toString();
    if(ch_dishno.isEmpty()) {
        lds_messagebox::warning(this, _TEXT_SLOT(this), QObject::tr("没有商品被选中"));
        return;
    }
    if(dataChecked()) {
        this->accept();
    }
}

void linshi_cook::tocancel()
{
    if(dataChecked()) {
        this->reject();
    }
}

void linshi_cook::tokitchen_plan()
{
    w_bt_dish_kitchen_dish dialog(this, false);
    dialog.refresh();
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    dialog.move(0, 0);
    dialog.exec();
}

void linshi_cook::tochange()
{
    QModelIndex index = ui->tableView->currentIndex();
    if(false == index.isValid()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有商品被选中"));
        return;
    }

    if(index.data(Qt::UserRole + 1).toString() == "-3") return;

    fexpandmain_2::updateDishImage(this,index, ui->tableView->m, "");
}

bool linshi_cook::dataChecked()
{
    lds_query_hddpos  query;
    query.execSelect("SELECT count(0) FROM cey_bt_dish  WHERE  ch_tempflag = 'Y' "
                     " AND isnull(vch_kitchen_out_id1)     "
                     " AND isnull(vch_kitchen_out_id2)     "
                     " AND isnull(vch_kitchen_out_id3)     "
                     " AND isnull(vch_kitchen_print_id1)   "
                     " AND isnull(vch_kitchen_print_id2)   "
                     " AND isnull(vch_kitchen_print_id3)   "
                     );
    query.next();
    if(0 < query.record().value(0).toInt()) {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("商品未添加厨打方案"), QObject::tr("继续退出"), QObject::tr("取消"))) {
            return false;
        }
    }
    return true;
}
