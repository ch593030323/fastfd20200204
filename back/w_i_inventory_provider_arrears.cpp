#include "w_i_inventory_provider_arrears.h"
#include "ui_w_i_inventory_provider_arrears.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "w_i_inventory.h"
#include "w_i_inventory_provider_arrears_pay.h"
#include <QTimer>
#include "w_i_inventory.h"

w_i_inventory_provider_arrears::w_i_inventory_provider_arrears(const QString &ch_providerNo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_i_inventory_provider_arrears),
    ch_providerNo(ch_providerNo)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("采购付款"));
    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_st_provider_pay");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->save_set_header("ch_sheetno",QObject::tr("单据编号"));
    tablemodel->save_set_header("ch_sheetType",QObject::tr("单据类型"));
    tablemodel->save_set_header("ch_providerNo",QObject::tr("供应商编号"));
    tablemodel->save_set_header("num_cost",QObject::tr("税前"));
    tablemodel->save_set_header("num_tax",QObject::tr("税收"));
    tablemodel->save_set_header("num_cash",QObject::tr("总金额"));
    tablemodel->save_set_header("num_remain",QObject::tr("欠款"));

    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_sheetType"),
                                            new lds_model_sqltablemodel_delegate_com(this, &w_i_inventory::get_MODEL_INVENTORY_SHEET_NAME()));

    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_pay, SIGNAL(clicked()), this, SLOT(topay()));
    QTimer::singleShot(100, this, SLOT(toselect()));
}

w_i_inventory_provider_arrears::~w_i_inventory_provider_arrears()
{
    delete ui;
}

void w_i_inventory_provider_arrears::topay()
{
    int row=ui->tableView->currentIndex().row();
    if(row<0) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    if(tablemodel->model_data(row, "num_remain").toDouble() == 0) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("没有欠款"));
        return;
    }
    w_i_inventory_provider_arrears_pay dialog(tablemodel, row, this);
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        toselect();
    }
}

void w_i_inventory_provider_arrears::toexit()
{
    this->reject();
}

void w_i_inventory_provider_arrears::toselect()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0 )
        row = 0;
    tablemodel->setFilter(QString("num_remain <> 0 and ch_providerNo = '%1' ").arg(ch_providerNo));
    ui->tableView->selectRow(row);
    if(ui->tableView->currentIndex().row() >= 0) {
        ui->tableView->resizeColumnsToContentsDelay();
    }
}
