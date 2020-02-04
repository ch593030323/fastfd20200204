
#include "w_bt_dish_quit.h"
//退货原因
#include "ui_w_bt_dish_unitset_dialog.h"
#include "lds_model_sqltablemodel.h"
#include "backheader.h"
#include "lds_messagebox.h"
#include <QSqlRecord>
#include "lds_query_hddpos.h"
#include "backheader.h"
#include "w_bt_dish.h"
#include "qsqlerror.h"

w_bt_dish_quit::w_bt_dish_quit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_bt_dish_unitset_Dialog)
{
    ui->setupUi(this);
    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_reason");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);

    tablemodel->save_set_header("int_flowid", QObject::tr("编号"));
    tablemodel->save_set_header("vch_type", QObject::tr("退货原因"));
    tablemodel->save_set_header("vch_reason", QObject::tr("原因说明"), true);
    tablemodel->enable_vheaderdata_dirty_translate();

    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    //! toolbar
    ui->widget->setup(QStringList()<<QObject::tr("新增")<<QObject::tr("删除")<</*QObject::tr("刷新")<<*/QObject::tr("保存")<<QObject::tr("退出"),Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("新增")),SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget->index_widget(QObject::tr("删除")),SIGNAL(clicked()),this,SLOT(todel()));
//    connect(ui->widget->index_widget(QObject::tr("刷新")),SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(QObject::tr("保存")),SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));

    //!refresh
    QTimer::singleShot(0, this, SLOT(torefresh()));
}

void w_bt_dish_quit::tonew()
{
    int rowcount=tablemodel->rowCount();
    tablemodel->insertRow(rowcount);
    tablemodel->model_data_set(rowcount, "int_flowid", tablemodel->max_column_add_1_dec("int_flowid"));
    tablemodel->model_data_set(rowcount, "vch_type", QObject::tr("退货原因"));
    ui->tableView->setCurrentIndex(tablemodel->model_index(rowcount, "vch_reason"));
}

void w_bt_dish_quit::todel()
{
    tablemodel->removeRow(ui->tableView->currentIndex().row());
}

bool w_bt_dish_quit::tosave()
{
    for(int r=0,r_count=tablemodel->rowCount();r<r_count;r++) {
        if(tablemodel->model_data(r, "vch_reason").toString().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("原因不能为空"));
            return false;
        }
    }
    if(tablemodel->trySubmitAll()) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        return true;
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误")+QString("%1").arg(__LINE__));
        return false;
    }
}

void w_bt_dish_quit::torefresh()
{
    tablemodel->select();
    ui->tableView->resizeColumnsToContents();
}

void w_bt_dish_quit::toexit()
{
    if(tablemodel->model_is_dirty()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"),QObject::tr("取消"))) {
        case 0:
            if(!tosave()) {
                return;
            }
            break;
        case 1:
            break;
        case 2:
            return;
        }
    }
    this->reject();
}
