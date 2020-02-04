#include "barscale_net_set_dialog.h"
#include "ui_barscale_net_set_dialog.h"
#include <QFile>
#include <QSettings>
#include <QtDebug>
#include "ui_barscale_net_set_change_dialog.h"
#include "commonheader.h"
#include "uinormal_message.h"
#include "lds.h"

barscale_net_set_dialog::barscale_net_set_dialog(barscale_main_dialog *parent) :
    QDialog(parent),
    _parent(parent)
{
    ui = new Ui::barscale_net_set_dialog;
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    this->setWindowTitle(QObject::tr("网络设置-新增"));

    model = new QStandardItemModel;
    model->setHorizontalHeaderLabels(QStringList() << QObject::tr("秤号") << QObject::tr("秤名") << QObject::tr("地址/端口") << QObject::tr("状态"));
    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
    load_setting();
}

barscale_net_set_dialog::~barscale_net_set_dialog()
{
    delete ui;
}

void barscale_net_set_dialog::on_pushButton_clicked()
{
    barscale_net_set_new dialog(this);
    dialog.ui->checkBox_isuse->setChecked(true);
    if(QDialog::Accepted==dialog.exec()) {
        model->appendRow(QList<QStandardItem*>()
                         << new QStandardItem(dialog.ui->lineEdit_no->text())
                         << new QStandardItem(dialog.ui->comboBox_name->currentText())
                         << new QStandardItem(dialog.ui->lineEdit_ip->text()+":"+dialog.ui->lineEdit_port->text())
                         << new QStandardItem(dialog.ui->checkBox_isuse->isChecked()?QObject::tr("使用"):QObject::tr("不使用"))
                         );
    }
}

void barscale_net_set_dialog::on_pushButton_2_clicked()
{
    int row;
    if((row=ui->tableView->currentIndex().row()) >= 0) {
        barscale_net_set_change dialog(this);
        dialog.ui->lineEdit_no->setText(model->item(row, 0)->text());
        dialog.ui->comboBox_name->setCurrentIndex(dialog.ui->comboBox_name->findText(model->item(row, 1)->text()));
        dialog.ui->lineEdit_ip->setText(model->item(row, 2)->text().split(":").value(0));
        dialog.ui->lineEdit_port->setText(model->item(row, 2)->text().split(":").value(1));
        dialog.ui->checkBox_isuse->setChecked(model->item(row, 3)->text()==QObject::tr("使用"));

        if(QDialog::Accepted==dialog.exec()) {
            model->item(row, 0)->setText(dialog.ui->lineEdit_no->text());
            model->item(row, 1)->setText(dialog.ui->comboBox_name->currentText());
            model->item(row, 2)->setText(dialog.ui->lineEdit_ip->text()+":"+dialog.ui->lineEdit_port->text());
            model->item(row, 3)->setText(dialog.ui->checkBox_isuse->isChecked()?QObject::tr("使用"):QObject::tr("不使用"));
        }
    }
}

void barscale_net_set_dialog::on_pushButton_3_clicked()
{
    int row;
    if((row=ui->tableView->currentIndex().row()) >= 0) {
        model->removeRow(row);
    }
}

void barscale_net_set_dialog::on_pushButton_4_clicked()
{
    save_setting();
    this->reject();
}

void barscale_net_set_dialog::load_setting()
{
    model->removeRows(0,model->rowCount());
    //从保存路径读取配置信息
    QByteArray ba = lds::sysconf->value("barscale/netinfo").toByteArray();
    QDataStream out(&ba, QIODevice::ReadOnly);
    while(!out.atEnd()) {
        QList<QStandardItem *> items;
        QString p1, p2, p3, p4;
        out >> p1 >> p2 >> p3 >> p4;
        items <<new QStandardItem(p1);
        items <<new QStandardItem(p2);
        items <<new QStandardItem(p3);
        items <<new QStandardItem(p4);

        model->appendRow(items);
    }
}

void barscale_net_set_dialog::save_setting()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    for(int r = 0, cout = model->rowCount(); r < cout; r++) {
        out << model->item(r, 0)->text();
        out << model->item(r, 1)->text();
        out << model->item(r, 2)->text();
        out << model->item(r, 3)->text();
    }
    lds::sysconf->setValue("barscale/netinfo", ba);
}

//=======================
barscale_net_set_change::barscale_net_set_change(QWidget *parent):
    QDialog(parent),
    ui(new Ui_barscale_net_set_change_dialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    this->setWindowTitle(QObject::tr("网络设置-修改"));
    ui->comboBox_name->addItems(CommonHeader::creat_barcode_scaletypes());
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->comboBox_name,SIGNAL(currentIndexChanged(QString)),this,SLOT(update_port(QString)));

}

void barscale_net_set_change::toexit()
{
    this->reject();
}

void barscale_net_set_change::took()
{
    this->accept();
}

void barscale_net_set_change::update_port(const QString &scaletype)
{
    ui->lineEdit_port->setText(CommonHeader::creat_barcode_scale_netport_init(scaletype));
}


//========================
barscale_net_set_new::barscale_net_set_new(barscale_net_set_dialog *parent):
    barscale_net_set_change(parent)
{
    ui->comboBox_name->setCurrentIndex(ui->comboBox_name->findText(lds::sysconf->value("barscale/scaletype").toString()));
    ui->lineEdit_no->setText(QString::number(parent->model->rowCount()+1));
    update_port(ui->comboBox_name->currentText());
}

void barscale_net_set_new::toexit()
{
    this->reject();
}

void barscale_net_set_new::took()
{
    this->accept();
}
