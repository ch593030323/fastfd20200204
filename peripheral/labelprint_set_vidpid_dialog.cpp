#include "labelprint_set_vidpid_dialog.h"
#include "ui_labelprint_set_vidpid_dialog.h"
#include "commonheader.h"
#include "printer_pos_vidpid.h"
#include "uinormal_message.h"
#include <QPainter>
#include <QFile>
#include "printer_pos_vidpid.h"

labelprint_set_vidpid_dialog::labelprint_set_vidpid_dialog(labelprint_set_dialog *parent) :
    QDialog(parent),
    ui(new Ui::labelprint_set_vidpid_dialog),
    _parent(parent)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels(QStringList() << "VID" << "PID" << QObject::tr("端口号"));
    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView->setSelectionMode(QTableView::SingleSelection);
    //3
    QList<Printer_POS_VIDPID::vidpid_Data>  dlist = Printer_POS_VIDPID::get_vidpid_print_type_list();
    foreach(const Printer_POS_VIDPID::vidpid_Data &d, dlist) {
        model->appendRow(QList<QStandardItem*>()
                        << new QStandardItem<< new QStandardItem<< new QStandardItem
                        );
        model->setData(model->index(model->rowCount() - 1, 0), d.idVendor);
        model->setData(model->index(model->rowCount() - 1, 1), d.idProduct);
        model->setData(model->index(model->rowCount() - 1, 2), d.device_address);
    }
}

labelprint_set_vidpid_dialog::~labelprint_set_vidpid_dialog()
{
    delete ui;
}

void labelprint_set_vidpid_dialog::on_pushButton_app_clicked()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        uinormal_message::warning(this, QObject::tr("提示"), QObject::tr("没有行被选中"));
        return;
    }
    ret_value = QString("%1&%2").arg(model->item(row, 0)->text()).arg(model->item(row, 1)->text());
    this->accept();
}

void labelprint_set_vidpid_dialog::on_pushButton_exit_clicked()
{
    this->reject();
}
