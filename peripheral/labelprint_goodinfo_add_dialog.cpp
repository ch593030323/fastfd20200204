#include "labelprint_goodinfo_add_dialog.h"
#include "ui_labelprint_goodinfo_add_dialog.h"

labelprint_goodinfo_add_dialog::labelprint_goodinfo_add_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_labelprint_goodinfo_add_dialog),
    model(new QStandardItemModel)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    ui->tableView->setSelectionMode(QTableView::MultiSelection);
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->setModel(model);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
    connect(ui->pushButton_clear,SIGNAL(clicked()),this,SLOT(clearselected()));
}

void labelprint_goodinfo_add_dialog::clearselected()
{
    ui->tableView->selectionModel()->clear();
}
