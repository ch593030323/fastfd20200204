#include "w_m_member_list_recharge_scheme.h"
#include "ui_w_m_member_list_recharge_scheme.h"
#include "n_func.h"
#include "lds_tableview_delegate_time.h"
#include "lds_messagebox.h"

w_m_member_list_recharge_scheme::w_m_member_list_recharge_scheme(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_m_member_list_recharge_scheme)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("方案"));


    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels(QStringList() << tr("实收金额") << tr("充值金额") << tr("有效期至"));
    model->setVerticalHeaderLabels(QStringList() << tr("方案") + "1" << tr("方案") + "2");
    model->setItem(0, 0, newFieldItem("recharge_schm_real_500"));
    model->setItem(0, 1, newFieldItem("recharge_schm_amt_500"));
    model->setItem(0, 2, newFieldItem("recharge_schm_dt_500"));
    model->setItem(1, 0, newFieldItem("recharge_schm_real_1000"));
    model->setItem(1, 1, newFieldItem("recharge_schm_amt_1000"));
    model->setItem(1, 2, newFieldItem("recharge_schm_dt_1000"));

    ui->tableView->setModel(model);
    ui->tableView->setColumnWidth(0, 120);
    ui->tableView->setColumnWidth(1, 120);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setItemDelegateForColumn(2, new lds_tableview_delegate_time(QVariant::DateTime, this));

    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
}

w_m_member_list_recharge_scheme::~w_m_member_list_recharge_scheme()
{
    delete ui;
}

void w_m_member_list_recharge_scheme::took()
{
    for(int row = 0; row < model->rowCount(); row ++) {
        for(int column = 0; column < model->columnCount(); column ++) {
            QVariant value = model_data(row, column, Qt::DisplayRole);
            QString key = model_data(row, column, Qt::UserRole + 1).toString();
            if(value.type() == QVariant::DateTime)
                n_func::f_set_sysparm(key, value.toDateTime().toString(yyyyMMddhhmmss));
            else
                n_func::f_set_sysparm(key, value.toString());
        }
    }

    lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("保存成功"));
    this->accept();
}

void w_m_member_list_recharge_scheme::tocancel()
{
    this->reject();
}

QStandardItem * w_m_member_list_recharge_scheme::newFieldItem(const QString &field_name)
{
    QStandardItem *item = new QStandardItem(n_func::f_get_sysparm(field_name));
    item->setData(field_name, Qt::UserRole + 1);
    return item;
}

QVariant w_m_member_list_recharge_scheme::model_data(int row, int column, int role)
{
    return model->data(model->index(row, column), role);
}
