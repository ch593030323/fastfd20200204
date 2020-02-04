#include "w_bt_dish_kitchenprinter_printermanager.h"
#include "backheader.h"
#include "w_bt_dish.h"
#include "lds_tableview_delegate_check.h"
#include "lds_query_hddpos.h"
#include "lds_messagebox.h"
#include <QSqlError>
#include "lds_combobox.h"
#include "n_func.h"
#include "public_logindialog.h"
#include "printer_pos_net.h"
#include "lds_query_hddpos.h"
#include "printer_0.h"
#include "printer_pos.h"
#include "printer_pos_net.h"
#include "printer_pos_vidpid.h"
#include <QHeaderView>
#include "w_bt_dish_kitchen_dish.h"
#include "ui_w_bt_dish_unitset_dialog.h"
#include "lds_tableview_delegate_check.h"
#include "backheader.h"
#include "lds_pushbuttonlock.h"
#include "lds_query_hddpos.h"
#include "w_bt_dish_kitchenprinter_printerset.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "printer_normal.h"
#include "ui_w_bt_dish_Kitchenprinter_printermanager.h"

w_bt_dish_Kitchenprinter_printermanager::w_bt_dish_Kitchenprinter_printermanager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_bt_dish_Kitchenprinter_printermanager)
{
    ui->setupUi(this);
    dchange = false;

    ui->comboBox->addItem(QObject::tr("所有"), "%");
    ui->comboBox->addItem(QObject::tr("打印机"), "N");
    ui->comboBox->addItem(QObject::tr("厨显"), "Y");

    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
    //    ui->tableView->setSortingEnabled(true);
    //    connect(ui->tableView->horizontalHeader(),SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),tablemodel,SLOT(toSort(int,Qt::SortOrder)));

    tablemodel->setTable("cey_bt_kitchen_print");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->save_set_header("int_id", QObject::tr("编号"));
    tablemodel->save_set_header("vch_printname", QObject::tr("打印机名称"));
    //    tablemodel->save_set_header("vch_printtype", QObject::tr("打印机型号"));
    tablemodel->save_set_header("vch_printip", QObject::tr("打印机端口"));
    tablemodel->save_set_header("ch_areaNo", QObject::tr("区域"));
    tablemodel->save_set_header("ch_kitchen_view", QObject::tr("类型"));
    ui->tableView->setModel(tablemodel);
    //    tablemodel->setSort(ui->tableView->horizontalHeader()->sortIndicatorSection(), ui->tableView->horizontalHeader()->sortIndicatorOrder());

    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_printtype"), new lds_model_sqltablemodel_delegate_com(this,  &Printer_Normal::get_normal_print_map_ref()));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_areaNo"), new lds_model_sqltablemodel_delegate_com(this, "cey_bt_table_area", "ch_areaNo","vch_areaname"));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_kitchen_view"), new lds_model_sqltablemodel_delegate_com(this, ui->comboBox));

    ui->comboBox->setcomusrdata("N");
    //    ui->comboBox->setcomusrdata(lds::sysconf->value("w_bt_dish_Kitchenprinter_printermanager/print_select", "N"));

    //! toolbar
    connect(ui->pushButton_newprint, SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->pushButton_newkitchen, SIGNAL(clicked()),this,SLOT(tonewkitchen()));
    connect(ui->pushButton_change, SIGNAL(clicked()),this,SLOT(tochange()));
    connect(ui->pushButton_del, SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_test, SIGNAL(clicked()),this,SLOT(toprinttest()));
    connect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(tochange()));
    connect(tablemodel,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(datachanged(QModelIndex,QModelIndex)));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(toprint_select(int)));
    QTimer::singleShot(0, this, SLOT(torefresh()));
}

void w_bt_dish_Kitchenprinter_printermanager::tonew()
{
    QString errstring;
    int max_int_id_1 = tablemodel->max_column_add_1__max_sql_add_1_dec("int_id");
    w_bt_dish_Kitchenprinter_printerSet dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    dialog.initData(QString::number(max_int_id_1), QObject::tr("打印机")+QString::number(max_int_id_1));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        int rowcount=tablemodel->rowCount();
        tablemodel->insertRow(rowcount);
        tablemodel->model_data_set(rowcount, "int_id", dialog.getno());
        tablemodel->model_data_set(rowcount, "vch_printname", dialog.getname());
        tablemodel->model_data_set(rowcount, "ch_kitchen_view", dialog.get_ch_kitchen_view());
        tablemodel->model_data_set(rowcount, "vch_printtype", dialog.getprinttype());
        tablemodel->model_data_set(rowcount, "vch_instype", dialog.getinstype());
        tablemodel->model_data_set(rowcount, "vch_printip", dialog.getport());
        tablemodel->model_data_set(rowcount, "int_paperwidth", dialog.getpaperwidth());
        tablemodel->model_data_set(rowcount, "ch_areano", dialog.get_ch_areano());
        tablemodel->model_data_set(rowcount, "int_xinye_beep_count", dialog.get_xinye_beep());
        if(!tablemodel->trySubmitAll(&errstring)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        }
        ui->tableView->resizeColumnsToContents();
    }
}

void w_bt_dish_Kitchenprinter_printermanager::tonewkitchen()
{
    QString errstring;
    int max_int_id_1 = tablemodel->max_column_add_1__max_sql_add_1_dec("int_id");
    w_bt_dish_Kitchenprinter_printerSet dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    dialog.initData(QString::number(max_int_id_1), QObject::tr("厨显")+QString::number(max_int_id_1), "Y");
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        int rowcount=tablemodel->rowCount();
        tablemodel->insertRow(rowcount);
        tablemodel->model_data_set(rowcount, "int_id", dialog.getno());
        tablemodel->model_data_set(rowcount, "vch_printname", dialog.getname());
        tablemodel->model_data_set(rowcount, "ch_kitchen_view", dialog.get_ch_kitchen_view());
        tablemodel->model_data_set(rowcount, "vch_printtype", dialog.getprinttype());
        tablemodel->model_data_set(rowcount, "vch_instype", dialog.getinstype());
        tablemodel->model_data_set(rowcount, "vch_printip", dialog.getport());
        tablemodel->model_data_set(rowcount, "int_paperwidth", dialog.getpaperwidth());
        tablemodel->model_data_set(rowcount, "ch_areano", dialog.get_ch_areano());
        tablemodel->model_data_set(rowcount, "int_xinye_beep_count", dialog.get_xinye_beep());
        if(!tablemodel->trySubmitAll(&errstring)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        }
        ui->tableView->resizeColumnsToContents();
    }
}

void w_bt_dish_Kitchenprinter_printermanager::todel()
{
    QString ipport;
    int row = ui->tableView->currentIndex().row();
    if(row >= 0) {
        lds_query_hddpos  query;
        query.execSelect(QString(" SELECT * FROM cey_bt_kitchen_plan where int_print_id = '%1'; ").arg(tablemodel->model_data(row,  "int_id").toString()));
        if(query.next()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已被厨打方案占用"));
            return;
        }

        ipport = tablemodel->model_data(row,"vch_printip").toString().trimmed();
        tablemodel->removeRow(ui->tableView->currentIndex().row());
        tablemodel->trySubmitAll();
        ui->tableView->resizeColumnsToContents();
    }
}

void w_bt_dish_Kitchenprinter_printermanager::tochange()
{
    int row =ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有行被选中"));
        return;
    }
    w_bt_dish_Kitchenprinter_printerSet dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    dialog.initData(tablemodel->model_data(row,"int_id").toString(),
                    tablemodel->model_data(row,"vch_printname").toString(),
                    tablemodel->model_data(row,"ch_kitchen_view").toString(),
                    tablemodel->model_data(row,"vch_printtype").toString(),
                    tablemodel->model_data(row,"vch_instype").toInt(),
                    tablemodel->model_data(row,"vch_printip").toString(),
                    tablemodel->model_data(row,"int_paperwidth").toInt(),
                    tablemodel->model_data(row,"ch_areano").toString(),
                    tablemodel->model_data(row,"int_xinye_beep_count").toInt()
                    );
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        tablemodel->model_data_set(row, "int_id", dialog.getno());
        tablemodel->model_data_set(row, "vch_printname", dialog.getname());
        tablemodel->model_data_set(row, "ch_kitchen_view", dialog.get_ch_kitchen_view());
        tablemodel->model_data_set(row, "vch_printtype", dialog.getprinttype());
        tablemodel->model_data_set(row, "vch_instype", dialog.getinstype());
        tablemodel->model_data_set(row, "vch_printip", dialog.getport());
        tablemodel->model_data_set(row, "int_paperwidth", dialog.getpaperwidth());
        tablemodel->model_data_set(row, "ch_areano", dialog.get_ch_areano());
        tablemodel->model_data_set(row, "int_xinye_beep_count", dialog.get_xinye_beep());
        tablemodel->trySubmitAll();
        ui->tableView->restore();
    }
}

void w_bt_dish_Kitchenprinter_printermanager::torefresh()
{
    QString ch_kitchen_view = ui->comboBox->itemData(ui->comboBox->currentIndex()).toString();
    tablemodel->setFilter(QString("ch_kitchen_view like '%1' ").arg(ch_kitchen_view));
    ui->tableView->resizeColumnsToContents();

    lds::showWidget(ui->pushButton_newkitchen, true);
    lds::showWidget(ui->pushButton_newprint, true);
    if("N" == ch_kitchen_view) {
        lds::showWidget(ui->pushButton_newkitchen, false);
    }
    if("Y" == ch_kitchen_view) {
        lds::showWidget(ui->pushButton_newprint, false);
    }
}

void w_bt_dish_Kitchenprinter_printermanager::toexit()
{
    if(dchange) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("有数据修改,请检测:厨打打印机、出品打印机 设置情况是否正确"));
    }
    this->reject();
}

void w_bt_dish_Kitchenprinter_printermanager::toprinttest()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有行被选中"));
        return;
    }
    QString path=tablemodel->model_data(row,"vch_printip").toString();
    if(path.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("打印路径为空"));
        return;
    }
    lds_pushbuttonlock b(qobject_cast<QWidget *>(this->sender()));
    b.toLock();

    backheader::syn_dataprin_test_byresolveself(
                this,
                tablemodel->model_data(row, "ch_kitchen_view").toString(),
                tablemodel->model_data(row, "vch_printip").toString(),
                tablemodel->model_data(row, "vch_instype").toInt(),
                tablemodel->model_data(row, "vch_printtype").toString(),
                tablemodel->model_data(row, "int_paperwidth").toInt(),
                tablemodel->model_data(row, "int_xinye_beep_count").toInt()
                );
}

void w_bt_dish_Kitchenprinter_printermanager::datachanged(const QModelIndex &/*topLeft*/, const QModelIndex &/*bottomRight*/)
{
    if(dchange) return;
    dchange = true;
}

void w_bt_dish_Kitchenprinter_printermanager::toprint_select(int index)
{
    lds::sysconf->setValue("w_bt_dish_Kitchenprinter_printermanager/print_select", ui->comboBox->itemData(index));
    torefresh();
}
