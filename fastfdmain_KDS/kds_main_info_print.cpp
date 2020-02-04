#include "kds_main_info_print.h"
#include "ui_kds_main_info_print.h"
#include "lds.h"
#include "w_bt_dish_kitchenprinter_printerset_port_dialog.h"
#include "lds_roundeddialog_rect_align.h"
#include "kds.h"

kds_main_info_print::kds_main_info_print(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::kds_main_info_print)
{
    ui->setupUi(this);
    ui->checkBox->setChecked(lds::sysconf->value("kds_main_info_print/check", false).toBool());
    ui->label_printerpath->setText(lds::sysconf->value("kds_main_info_print/printport").toString());

    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_printerset, SIGNAL(clicked()), this, SLOT(toprintset()));
}

kds_main_info_print::~kds_main_info_print()
{
    delete ui;
}

void kds_main_info_print::tocancel()
{
    this->reject();
}

void kds_main_info_print::took()
{
    lds::sysconf->setValue("kds_main_info_print/check", ui->checkBox->isChecked());
    this->accept();
}

void kds_main_info_print::toprintset()
{
    w_bt_dish_kitchenprinter_printerset_port_dialog dialog(this);
    dialog.setWindowTitle(QObject::tr("打印机"));
    dialog.setPrinterport(lds::sysconf->value("kds_main_info_print/printport").toString());
    if(QDialog::Accepted == lds_roundeddialog_rect_align_600x400(&dialog).exec()) {
        lds::sysconf->setValue("kds_main_info_print/printport", dialog.printerPort());
        ui->label_printerpath->setText(dialog.printerPort());
    }

#ifdef QT_DEBUG
    kds::print_cook("386");
#endif
}
