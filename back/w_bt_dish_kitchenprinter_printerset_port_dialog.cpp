#include "w_bt_dish_kitchenprinter_printerset_port_dialog.h"
#include "ui_w_bt_dish_kitchenprinter_printerset_port_dialog.h"
#include "w_bt_dish_kitchenprinter_printerset_port.h"
#include <QtDebug>
#include "printer_0.h"
#include "printer_pos_vidpid.h"
#include "printer_pos_serial.h"
#include "printer_pos_parallel.h"
#include "lds_messagebox.h"

w_bt_dish_kitchenprinter_printerset_port_dialog::w_bt_dish_kitchenprinter_printerset_port_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_bt_dish_kitchenprinter_printerset_port_dialog)
{
    ui->setupUi(this);

    ui->frame_print->setGeometry(QRect(10, 10, 500, 90));
    ui->widget_btn->setGeometry(QRect(10, 100, 500, 30));
    resize(520, 150);
    print_help = new w_bt_dish_kitchenprinter_printerset_port(this);
    print_help->addControl(ui->frame_print);

    connect(ui->pushButton_ok, SIGNAL(clicked()), this ,SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this ,SLOT(tocancel()));
    connect(ui->pushButton_test, SIGNAL(clicked()), this ,SLOT(totest()));
}

w_bt_dish_kitchenprinter_printerset_port_dialog::~w_bt_dish_kitchenprinter_printerset_port_dialog()
{
    delete ui;
}

QString w_bt_dish_kitchenprinter_printerset_port_dialog::printerPort()
{
    return printer_port;
}

void w_bt_dish_kitchenprinter_printerset_port_dialog::setPrinterport(const QString &path)
{
    print_help->setPrinterport(path);
}

void w_bt_dish_kitchenprinter_printerset_port_dialog::took()
{
    printer_port = print_help->printerport();
    this->accept();
}

void w_bt_dish_kitchenprinter_printerset_port_dialog::tocancel()
{
    this->reject();
}

void w_bt_dish_kitchenprinter_printerset_port_dialog::totest()
{
    //打印测试成功
    printer_port = print_help->printerport();
    QSharedPointer<Printer_POS> printer = Printer_POS::get_pos_printer_by_path(printer_port);
    if(printer.isNull()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, "PRINTER_IS_NULL");
        return;
    }
    if(false == printer->tryOpen(printer_port)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, printer->lastError());
        return;
    }
    printer->write("PRINT TEST \n", "");
}
