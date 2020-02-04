#include "w_bt_dish_kitchenprinter_printerset_port.h"
#include "printer_0.h"
#include "printer_pos_vidpid.h"
#include "printer_pos_serial.h"
#include "lds_messagebox.h"
#include "printer_pos_parallel.h"
#include "lds_roundeddialog_rect.h"
#include <QTableView>
#include "lds_toolbar.h"
#include "lds_toolbar_button.h"
#include "lds_combobox.h"
#include <QHeaderView>

w_bt_dish_kitchenprinter_printerset_port::w_bt_dish_kitchenprinter_printerset_port(QObject *parent) :
    QObject(parent)
{
}

w_bt_dish_kitchenprinter_printerset_port::~w_bt_dish_kitchenprinter_printerset_port()
{
}

void w_bt_dish_kitchenprinter_printerset_port::setPrinterport(const QString &port)
{
    //1
    if(printer_0::comPortIsValid(port)) {
        QStringList list = port.split(",");
        ui_comboBox_port_tty->setCurrentIndex(ui_comboBox_port_tty->findText(list.value(0)));
        ui_comboBox_baud->setCurrentIndex(ui_comboBox_baud->findText(list.value(1)));
        ui_comboBox_porttype->setCurrentIndex(1);
        ui_comboBox_flowcontrol->setCurrentIndex(ui_comboBox_flowcontrol->findData(list.value(5).toInt()));
    } else if(printer_0::usbPortIsValid(port)) {
        ui_comboBox_port_usb->setCurrentIndex(ui_comboBox_port_usb->findText(port));
        if(ui_comboBox_port_usb->currentIndex() == -1) {
            ui_comboBox_porttype->setCurrentIndex(0);
        }
        if(port.contains("&")) {
            ui_comboBox_port_usb->setEditText(port);
        }
    } else if(port.count(".") == 3) {
        QStringList list = port.split(":");
        ui_lineEdit_ip->setText(list.value(0));
        ui_lineEdit_port->setText(list.value(1));
        ui_comboBox_porttype->setCurrentIndex(2);
    } else if(port == Printer_POS_PARALLEL::printPort()) {
        ui_comboBox_porttype->setCurrentIndex(3);
    }

    //2
    toporttype(ui_comboBox_porttype->currentIndex());
}

QString w_bt_dish_kitchenprinter_printerset_port::printerport()
{
    QString p;
    if(ui_comboBox_port_usb->isVisible()&&!ui_comboBox_port_usb->currentText().isEmpty()) {
        p += ui_comboBox_port_usb->currentText();
    }
    if(ui_comboBox_port_tty->isVisible()&&!ui_comboBox_port_tty->currentText().isEmpty()) {
        p += ui_comboBox_port_tty->currentText();
    }

    if(ui_comboBox_baud->isVisible()&&!ui_comboBox_baud->currentText().isEmpty()) {
        p += ","+ui_comboBox_baud->currentText();
    }
    if(ui_comboBox_flowcontrol->isVisible() && !ui_comboBox_flowcontrol->currentText().isEmpty()) {
        p += QString(5-p.count(","), ',')+QString::number(ui_comboBox_flowcontrol->itemData(ui_comboBox_flowcontrol->currentIndex()).toInt());
    }

    if(ui_lineEdit_ip->isVisible()&&!ui_lineEdit_ip->text().isEmpty()) {
        p += ui_lineEdit_ip->text();
    }

    if(ui_lineEdit_port->isVisible()&&!ui_lineEdit_port->text().isEmpty()) {
        p += ":"+ui_lineEdit_port->text();
    }

    return p;
}

void w_bt_dish_kitchenprinter_printerset_port::refreshPort()
{
    toporttype(ui_comboBox_porttype->currentIndex());
}

void w_bt_dish_kitchenprinter_printerset_port::set_com_usb_enable(bool e)
{
    ui_comboBox_port_usb->setEnabled(e);
}

void w_bt_dish_kitchenprinter_printerset_port::onlyNetPort()
{
    ui_comboBox_porttype->setCurrentIndex(2);//网口
    ui_comboBox_porttype->setEnabled(false);
}

void w_bt_dish_kitchenprinter_printerset_port::addControl(QFrame *frame_print)
{
    this->frame_print = frame_print;

    ui_lineEdit_ip = frame_print->findChild<QLineEdit* >("lineEdit_ip");
    ui_comboBox_port_tty = frame_print->findChild<QComboBox* >("comboBox_port_tty");
    ui_comboBox_port_usb = frame_print->findChild<QComboBox* >("comboBox_port_usb");
    ui_label_baud = frame_print->findChild<QLabel* >("label_baud");
    ui_label_flowcontrol = frame_print->findChild<QLabel* >("label_flowcontrol");
    ui_label_type = frame_print->findChild<QLabel* >("label_type");
    ui_label_port = frame_print->findChild<QLabel* >("label_port");
    ui_pushButton_usb_vp = frame_print->findChild<QPushButton* >("pushButton_usb_vp");
    ui_lineEdit_port = frame_print->findChild<QLineEdit* >("lineEdit_port");
    ui_comboBox_baud = frame_print->findChild<QComboBox* >("comboBox_baud");
    ui_comboBox_porttype = frame_print->findChild<lds_ComboBox* >("comboBox_porttype");
    ui_comboBox_flowcontrol = frame_print->findChild<lds_ComboBox* >("comboBox_flowcontrol");

    ui_comboBox_porttype->addItems(printer_0::creat_normal_printpathtypes());
    ui_comboBox_port_tty->addItems(printer_0::f_scan_ttyPort());
    ui_comboBox_port_usb->addItems(printer_0::f_scan_usbPort());
    ui_comboBox_baud->addItems(Printer_POS_SERIAL::creat_baudrate_list());
    ui_comboBox_flowcontrol->addItem(QObject::tr("无流控"), FLOW_OFF);
    ui_comboBox_flowcontrol->addItem("XONXOFF", FLOW_XONXOFF);

    connect(ui_comboBox_porttype, SIGNAL(currentIndexChanged(int)),this,SLOT(toporttype(int)));
    connect(ui_pushButton_usb_vp, SIGNAL(clicked()),this,SLOT(toselectvidpidlist()));
    refreshPort();
}

void w_bt_dish_kitchenprinter_printerset_port::toporttype(int index)
{
    switch(index) {
    case 0://USB
        ui_label_baud->hide();ui_label_baud->setText(QObject::tr("波特率"));
        ui_comboBox_baud->hide();
        ui_lineEdit_ip->hide();ui_label_port->setText(QObject::tr("端口"));
        ui_lineEdit_port->hide();
        ui_comboBox_port_tty->hide();
        ui_comboBox_port_usb->show();
        ui_comboBox_port_usb->setEnabled(true);
        ui_pushButton_usb_vp->show();
        ui_comboBox_flowcontrol->hide();
        ui_label_flowcontrol->hide();
        ui_lineEdit_port->setText("");
#ifdef Q_OS_WIN
        ui_pushButton_usb_vp->hide();
#endif
        ui_comboBox_port_usb->clear();
        ui_comboBox_port_usb->addItems(printer_0::f_scan_usbPort());
        break;
    case 1://串口
        ui_label_baud->show();ui_label_baud->setText(QObject::tr("波特率"));
        ui_comboBox_baud->show();
        ui_lineEdit_ip->hide();ui_label_port->setText(QObject::tr("端口"));
        ui_lineEdit_port->hide();
        ui_comboBox_port_tty->show();
        ui_comboBox_port_usb->hide();
        ui_comboBox_port_usb->setEnabled(true);
        ui_pushButton_usb_vp->hide();
        ui_comboBox_flowcontrol->show();
        ui_label_flowcontrol->show();
        ui_lineEdit_port->setText("");
        break;
    case 2://网口
        ui_label_baud->show();ui_label_baud->setText(QObject::tr("端口"));
        ui_comboBox_baud->hide();
        ui_lineEdit_ip->show();ui_label_port->setText("IP");
        ui_lineEdit_port->show();
        ui_comboBox_port_tty->hide();
        ui_comboBox_port_usb->hide();
        ui_comboBox_port_usb->setEnabled(true);
        ui_pushButton_usb_vp->hide();
        ui_comboBox_flowcontrol->hide();
        ui_label_flowcontrol->hide();
        ui_lineEdit_port->setText("9100");
        if(ui_lineEdit_ip->text().isEmpty()) {
            ui_lineEdit_ip->setText("192.168.");
            ui_lineEdit_ip->setFocus();
        }
        break;
    case 3://并口
        ui_label_baud->hide();ui_label_baud->setText(QObject::tr("波特率"));
        ui_comboBox_baud->hide();
        ui_lineEdit_ip->hide();ui_label_port->setText(QObject::tr("端口"));
        ui_lineEdit_port->hide();
        ui_comboBox_port_tty->hide();
        ui_comboBox_port_usb->show();
        ui_comboBox_port_usb->setEnabled(false);
        ui_pushButton_usb_vp->hide();
        ui_comboBox_flowcontrol->hide();
        ui_label_flowcontrol->hide();
        ui_comboBox_port_usb->setEditText(Printer_POS_PARALLEL::printPort());
        break;
    }
}

void w_bt_dish_kitchenprinter_printerset_port::toselectvidpidlist()
{
    QDialog dialog(frame_print);
    dialog.resize(600, 400);
    QTableView tableView(&dialog);
    tableView.setGeometry(10, 10, 580, 340);
    QPushButton Okbtn(QObject::tr("确定"), &dialog);
    Okbtn.setGeometry(440, 355, 70, 40);
    QPushButton Cancelbtn(QObject::tr("取消"), &dialog);
    Cancelbtn.setGeometry(520, 355, 70, 40);

    QStandardItemModel model;
    model.setHorizontalHeaderLabels(QStringList() << "VID" << "PID" << QObject::tr("端口号"));
    tableView.setModel(&model);
    tableView.horizontalHeader()->setStretchLastSection(true);
    tableView.setEditTriggers(QTableView::NoEditTriggers);
    QList<Printer_POS_VIDPID::vidpid_Data>  dlist = Printer_POS_VIDPID::get_vidpid_print_type_list();
    foreach(const Printer_POS_VIDPID::vidpid_Data &d, dlist) {
        model.appendRow(QList<QStandardItem*>()
                        << new QStandardItem<< new QStandardItem<< new QStandardItem
                        );
        model.setData(model.index(model.rowCount() - 1, 0), QString().sprintf("%d", d.idVendor));
        model.setData(model.index(model.rowCount() - 1, 1), QString().sprintf("%d", d.idProduct));
        model.setData(model.index(model.rowCount() - 1, 2), QString().sprintf("%d", d.device_address));
    }

    dialog.setWindowTitle(_TEXT_SLOT(this));
    connect(&Okbtn, SIGNAL(clicked()),&dialog, SLOT(accept()));
    connect(&Cancelbtn, SIGNAL(clicked()),&dialog, SLOT(reject()));
    if(QDialog::Accepted == lds_roundeddialog_rect_align_600x400(&dialog).exec()) {
        int row = tableView.currentIndex().row();
        if(row >= 0) {
            ui_comboBox_port_usb->setEditText(QString("%1&%2").arg(model.item(row, 0)->text()).arg(model.item(row, 1)->text()));
        }
    }
}
