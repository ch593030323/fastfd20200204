#include "labelprint_goodinfo_dialog.h"
#include "ui_labelprint_goodinfo_dialog.h"
#include <QtDebug>
#include "ui_labelprint_goodinfo_add_dialog.h"
#include "commonheader.h"
#include "labelprint_goodinfo_add_dialog.h"
#include "uinormal_message.h"
#include "barscale_num_input.h"
#include "labelprint_project_self_dialog.h"
#include "lds.h"

labelprint_goodinfo_dialog::labelprint_goodinfo_dialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::labelprint_goodinfo_dialog),
    isfinishinput(false)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    this->setWindowTitle(QObject::tr("商品信息"));

    printmodel=new QStandardItemModel;
    inputallgoods_model=new QStandardItemModel;
    ui->tableView->setModel(printmodel);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    //    ui->tableView->setSelectionMode(QTableView::MultiSelection);

    connect(ui->pushButton_goodadd,SIGNAL(clicked()),this,SIGNAL(signal_add()));
    connect(ui->pushButton_goodremove,SIGNAL(clicked()),this,SLOT(togoodremove()));
    connect(ui->pushButton_print,SIGNAL(clicked()),this,SLOT(toprint()));

    connect(printmodel,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(printmodel_has_newrow(QModelIndex,int,int)));
    switch_mode = 1;
    on_pushButton_switch_mode_clicked();

    printmodel_selfh_List = lds::sysconf->value("labelprint_goodinfo_dialog/printmodel_selfh_List").toStringList();
    printmodel_selfh_conttent_List = lds::sysconf->value("labelprint_goodinfo_dialog/printmodel_selfh_conttent_List").toStringList();
}

labelprint_goodinfo_dialog::~labelprint_goodinfo_dialog()
{
    delete ui;
    delete inputallgoods_model;
}

void labelprint_goodinfo_dialog::tocommvalue(int column)
{
    QAbstractItemModel *model=ui->tableView->model();
    if(model) {
        QString first=model->index(0, column).data().toString();
        for(int r=0,rcount=model->rowCount();r<rcount;r++) {
            model->setData(model->index(r, column), first);
        }
    }
}

void labelprint_goodinfo_dialog::togoodremove()
{
    //1
    QModelIndexList indexlist=ui->tableView->selectionModel()->selectedIndexes();
    QSet<int> index_set;
    foreach(QModelIndex index, indexlist) {
        index_set.insert(index.row());
    }
    QList<int> index_list=index_set.toList();
    //2
    qSort(index_list.begin(), index_list.end(), qGreater<int>());
    foreach(int index, index_list) {
        printmodel->removeRow(index);
    }
}

void labelprint_goodinfo_dialog::printmodel_has_newrow(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)
    Q_UNUSED(last)
    if(isfinishinput) return;
    if(first==0) {
        for(int c=0,ccount=printmodel->columnCount();c<ccount;c++) {
            inputallgoods_model->setHorizontalHeaderItem(c, new QStandardItem(printmodel->headerData(c, Qt::Horizontal).toString()));
        }
    }
    QList<QStandardItem*> items;
    for(int c=0,ccount=printmodel->columnCount();c<ccount;c++) {
        items.append(new QStandardItem(printmodel->item(first, c)->text()));
    }
    inputallgoods_model->appendRow(items);
}

void labelprint_goodinfo_dialog::tosave()
{
}

void labelprint_goodinfo_dialog::toprint()
{
    emit signal_print();
}


void labelprint_goodinfo_dialog::on_pushButton_switch_mode_clicked()
{
    ui->tableView->selectionModel()->clear();
    if(switch_mode == 0) {
        ui->pushButton_switch_mode->setText(QObject::tr("批处理模式(点击切换)"));
        switch_mode = 1;
        ui->pushButton_allcontrol->setVisible(true);
        ui->pushButton_clearsection->setVisible(true);
        ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
        ui->tableView->setSelectionMode(QTableView::MultiSelection);
        ui->tableView->setSelectionBehavior(QTableView::SelectItems);
    } else
        if(switch_mode == 1) {
            ui->pushButton_switch_mode->setText(QObject::tr("正常模式(点击切换)"));
            switch_mode = 0;
            ui->pushButton_allcontrol->setVisible(false);
            ui->pushButton_clearsection->setVisible(false);
            ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
            ui->tableView->setSelectionMode(QTableView::SingleSelection);
            ui->tableView->setSelectionBehavior(QTableView::SelectItems);
        }
}

void labelprint_goodinfo_dialog::on_pushButton_allcontrol_clicked()
{
    QModelIndexList indexlist=ui->tableView->selectionModel()->selectedIndexes();
    if(indexlist.isEmpty()) {
        uinormal_message::warning(this, QObject::tr("批量修改"), QObject::tr("没有数据被选中"));
        return;
    }
    barscale_num_input inputdialog(this);
    inputdialog.setWindowTitle(QObject::tr("批量修改"));
    if(QDialog::Accepted == inputdialog.exec()) {
        QString retvalue = inputdialog.retvalue();
        bool ok;
        int value = retvalue.toInt(&ok);
        if(!ok)    value = retvalue.toInt(&ok, 16);
        foreach(QModelIndex index, indexlist) {
            if(index.isValid()) {
                if(inputdialog.mode == 0) {
                    if(ok) {
                        printmodel->setData(index, value);
                    } else {
                        printmodel->setData(index, retvalue);
                    }
                } else {
                    printmodel->setData(index, printmodel->data(index).toFloat()*retvalue.toFloat());
                }
            }
        }
    }
}

void labelprint_goodinfo_dialog::on_pushButton_clearsection_clicked()
{
    ui->tableView->selectionModel()->clear();
}

void labelprint_goodinfo_dialog::on_pushButton_print_project_self_clicked()
{
    //1
    labelprint_project_self_Dialog dialog(this);
    dialog.setWindowTitle(QObject::tr("提示"));
    QStandardItem *item;
    foreach(const QString &h, printmodel_defh_List) {
        dialog.standmodel->appendRow(item = new QStandardItem(h));
        item->setFlags(Qt::ItemIsEnabled);
    }
    foreach(const QString &h, printmodel_selfh_List) {
        dialog.standmodel->appendRow(item = new QStandardItem(h));
    }
    //2
    if(QDialog::Accepted == dialog.exec()) {

        printmodel_selfh_List.clear();
        for(int row = 0; row < dialog.standmodel->rowCount(); row++) {
            if(dialog.standmodel->item(row)->flags() & Qt::ItemIsEditable) {
                printmodel_selfh_List.append(dialog.standmodel->item(row)->text());
            }
        }
        lds::sysconf->setValue("labelprint_goodinfo_dialog/printmodel_selfh_List", printmodel_selfh_List);

        printmodel->setHorizontalHeaderLabels(printmodel_defh_List + printmodel_selfh_List);
    }
}
