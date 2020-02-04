#include "saas_guide_hard_test.h"
#include "ui_saas_guide_hard_test.h"
#include "printer_0.h"
#include "w_bt_dish_kitchenprinter_printerset.h"
#include "lds_messagebox.h"
#include "backheader.h"
#include "reader_rf.h"
#include "printer_pos.h"

saas_guide_hard_test::saas_guide_hard_test(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::saas_guide_hard_test)
{
    ui->setupUi(this);

    ui->comboBox_kexian_port->addItems(printer_0::f_scan_ttyPort());
    ui->comboBox_kexian_type->addItems(CommonHeader::creat_clientdisplay_ledtypes());
    tocdrawer_default();

    connect(ui->pushButton_beep, SIGNAL(clicked()), this, SLOT(tobeep()));
    connect(ui->pushButton_cdrawer_default, SIGNAL(clicked()), this, SLOT(tocdrawer_default()));
    connect(ui->pushButton_cdrawer_test, SIGNAL(clicked()), this, SLOT(tocdrawer_test()));
    connect(ui->pushButton_display_test, SIGNAL(clicked()), this, SLOT(todisplay_test()));
//    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_printer_manage, SIGNAL(clicked()), this, SLOT(toprinter_manage()));
    connect(ui->pushButton_printer_test, SIGNAL(clicked()), this, SLOT(toprinter_test()));
}

saas_guide_hard_test::~saas_guide_hard_test()
{
    delete ui;
}

void saas_guide_hard_test::tobeep()
{
    Reader_RF r;
    r.beep();
}

void saas_guide_hard_test::tocdrawer_default()
{
    ui->lineEdit_cdrawer->setText("1B 70 00 28 28");
}

void saas_guide_hard_test::tocdrawer_test()
{
    QSharedPointer<Printer_POS> printer = Printer_POS::get_pos_printer_by_path(ui->comboBox_printer->currentText());

    if(printer.isNull()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("打印机不存在"));
        return;
    }
    if(!printer->tryOpen(ui->comboBox_printer->currentText())) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, printer->lastError());
        return;
    }

    printer->writeHex(ui->lineEdit_cdrawer->text());
}

void saas_guide_hard_test::todisplay_test()
{
    static int sqr=-0;
    QString path = ui->comboBox_kexian_port->currentText();
    if(path.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有端口被选择"));
        return;
    }
    public_sql::display_led.setType(ui->comboBox_kexian_type->currentText());
    if(sqr%5==0) {
        if(!public_sql::display_led.startShowinfo("welcome", path)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,public_sql::display_led.lastError());
        }
    } else {
        if(!public_sql::display_led.writeCom(sqr%5, 888888.88, path)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,public_sql::display_led.lastError());
        }
    }
    sqr ++;
}

void saas_guide_hard_test::toprinter_manage()
{
    w_bt_dish_Kitchenprinter_printerSet0 dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    dialog.initData("01", "P01", "N", 0, 0, ui->comboBox_printer->currentText());
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        ui->comboBox_printer->setEditText(dialog.getport());
    }
}

void saas_guide_hard_test::toprinter_test()
{
    backheader::syn_dataprin_test_byresolveself(this, "N", ui->comboBox_printer->currentText(), 0, "", 0, 4);
}

void saas_guide_hard_test::toexit()
{
 //   this->reject();
}
