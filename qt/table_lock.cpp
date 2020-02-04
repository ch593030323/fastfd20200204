#include "table_lock.h"
#include "ui_table_lock.h"
#include <QSqlRecord>
#include "lds_tableview_delegate_check.h"
#include "lds_query_hddpos.h"
#include <QDebug>
#include "lds_messagebox.h"
#include "lds_model_sqltablemodel.h"
#include "public_sql.h"
#include <QTimer>
#include <QSqlError>
#include "lds.h"

table_lock::table_lock(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::table_lock)
{
    ui->setupUi(this);
    //!com
    ui->comboBox->addItem(QObject::tr("已锁桌台"),
                          "if('Y' = ifnull(ch_lockflag, 'N'), 1, 0) + if('Y' = ifnull(ch_lockflag_1, 'N'), 1, 0) + if('Y' = ifnull(ch_lockflag_2, 'N'), 1, 0) +"
                          "if('Y' = ifnull(ch_lockflag_3, 'N'), 1, 0) + if('Y' = ifnull(ch_lockflag_4, 'N'), 1, 0) + if('Y' = ifnull(ch_lockflag_5, 'N'), 1, 0) +"
                          "if('Y' = ifnull(ch_lockflag_6, 'N'), 1, 0) + if('Y' = ifnull(ch_lockflag_7, 'N'), 1, 0) + if('Y' = ifnull(ch_lockflag_8, 'N'), 1, 0) > 0   ");
    ui->comboBox->addItem(QObject::tr("所有桌台"), "");
    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("cey_bt_table");
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);

    tablemodel->save_set_header("ch_tableno", QObject::tr("餐桌号"));

    for(int k = 0; k < 9; k++ ) {
        if(k == 0) {
            tablemodel->save_set_header("ch_billno", QObject::tr("当前单号"));
            tablemodel->save_set_header("ch_lockflag", QObject::tr("是否锁台"), true);
        } else {
            //            tablemodel->save_set_header("ch_billno_"+QString::number(k), QObject::tr("当前单号")+QString::number(k));
            tablemodel->save_set_header("ch_lockflag_"+QString::number(k), QObject::tr("是否锁台")+QString::number(k), true);
        }
    }

    ui->tableView->setItemDelegate(new lds_tableview_delegate_check(QVector<int>()
                                                                    << tablemodel->fieldIndex("ch_lockflag"  )
                                                                    << tablemodel->fieldIndex("ch_lockflag_1")
                                                                    << tablemodel->fieldIndex("ch_lockflag_2")

                                                                    << tablemodel->fieldIndex("ch_lockflag_3")
                                                                    << tablemodel->fieldIndex("ch_lockflag_4")
                                                                    << tablemodel->fieldIndex("ch_lockflag_5")

                                                                    << tablemodel->fieldIndex("ch_lockflag_6")
                                                                    << tablemodel->fieldIndex("ch_lockflag_7")
                                                                    << tablemodel->fieldIndex("ch_lockflag_8")

                                                                   ,  ui->tableView, true));
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());

    ui->comboBox->setCurrentIndex(lds::sysconf->value("table_lock/lock_table", 0).toInt());
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(toselect()));
    QTimer::singleShot(0, this, SLOT(toselect()));
}

table_lock::~table_lock()
{
    delete ui;
}

void table_lock::on_pushButton_7_clicked()//确定
{
    if(saveData()) {
        this->accept();
    }
}

void table_lock::on_pushButton_14_clicked()//取消
{
    if(tablemodel->model_is_dirty()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"),QObject::tr("取消"))) {
        case 0:
            if(!saveData()) {
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

void table_lock::on_pushButton_allunlock_clicked()
{
    for(int row = 0, rowcount = tablemodel->rowCount(); row<rowcount; row++) {
        tablemodel->model_data_set(row, "ch_lockflag" ,  "N");
        tablemodel->model_data_set(row, "ch_lockflag_1", "N");
        tablemodel->model_data_set(row, "ch_lockflag_2", "N");

        tablemodel->model_data_set(row, "ch_lockflag_3", "N");
        tablemodel->model_data_set(row, "ch_lockflag_4", "N");
        tablemodel->model_data_set(row, "ch_lockflag_5", "N");

        tablemodel->model_data_set(row, "ch_lockflag_6", "N");
        tablemodel->model_data_set(row, "ch_lockflag_7", "N");
        tablemodel->model_data_set(row, "ch_lockflag_8", "N");
    }
    on_pushButton_7_clicked();
}

void table_lock::toselect()
{
    if(tablemodel->model_is_dirty()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"),QObject::tr("取消"))) {
        case 0:
            if(!saveData()) {
                return;
            }
            break;
        case 1:
            break;
        case 2:
            return;
        }
    }

    tablemodel->setFilter(ui->comboBox->itemData(ui->comboBox->currentIndex()).toString());
    ui->tableView->resizeColumnsToContents();

    if(this->sender() == ui->comboBox) {
        lds::sysconf->setValue("table_lock/lock_table", ui->comboBox->currentIndex());
    }
}

bool table_lock::saveData()
{
    lds_query_hddpos::tran_saction();
    if(tablemodel->trySubmitAll()) {
        lds_query_hddpos::com_mit();
        return true;
    }
    lds_query_hddpos::roll_back();
    return false;
}
