#include "labelprint_project_self_dialog.h"
#include "ui_labelprint_project_self_dialog.h"

labelprint_project_self_Dialog::labelprint_project_self_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::labelprint_project_self_Dialog)
{
    ui->setupUi(this);

    standmodel = new QStandardItemModel;
    standmodel->setHorizontalHeaderLabels(QStringList() << QObject::tr("打印项"));
    ui->tableView->setModel(standmodel);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    //2
    connect(ui->pushButton_add,SIGNAL(clicked()),this,SLOT(toadd()));
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_sub,SIGNAL(clicked()),this,SLOT(tosub()));
}

labelprint_project_self_Dialog::~labelprint_project_self_Dialog()
{
    delete ui;
}

void labelprint_project_self_Dialog::toadd()
{
    standmodel->appendRow(new QStandardItem);
}

void labelprint_project_self_Dialog::tosub()
{
    int row = ui->tableView->currentIndex().row();
    if(row > 0) {
        if(standmodel->item(row)->flags() & Qt::ItemIsEditable) {
            standmodel->removeRow( row );
        }
    }
}

void labelprint_project_self_Dialog::took()
{
    this->accept();
}

void labelprint_project_self_Dialog::tocancel()
{
    this->reject();
}
