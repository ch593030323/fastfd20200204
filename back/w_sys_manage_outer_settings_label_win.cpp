#include "w_sys_manage_outer_settings_label_win.h"
#include "ui_w_sys_manage_outer_settings_label_win.h"
#include "label_img_header.h"
#include "lds.h"
#include <QDir>
#include"lds_messagebox.h"
#include "w_sys_manage_outer_settings_img_show.h"
#include "transparentcenterdialog.h"

w_sys_manage_outer_settings_label_win::w_sys_manage_outer_settings_label_win(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_outer_settings_label_win)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("标签打印机"));
    ui->comboBox_printerName->addItems(label_img_header::printerNames());
    foreach(const QFileInfo &info, QDir("userdata/settings/print/label_win").entryInfoList()) {
        if(info.fileName().startsWith("label")) {
            ui->comboBox_labelFormat->addItem(info.fileName(), info.filePath());
        }
    }
    ui->comboBox_printerName->setcomtext(lds::sysconf->value("w_sys_manage_outer_settings_label_win/label_printer_name").toString());
    ui->comboBox_labelFormat->setcomusrdata(lds::sysconf->value("w_sys_manage_outer_settings_label_win/label_format"));


    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_test, SIGNAL(clicked()), this, SLOT(totest()));
    connect(ui->pushButton_preview, SIGNAL(clicked()), this, SLOT(topreview()));
}

w_sys_manage_outer_settings_label_win::~w_sys_manage_outer_settings_label_win()
{
    delete ui;
}

void w_sys_manage_outer_settings_label_win::took()
{
    lds::sysconf->setValue("w_sys_manage_outer_settings_label_win/label_printer_name", ui->comboBox_printerName->currentText());
    lds::sysconf->setValue("w_sys_manage_outer_settings_label_win/label_format", ui->comboBox_labelFormat->curusrdata());

    this->accept();
}

void w_sys_manage_outer_settings_label_win::toexit()
{
    this->reject();
}

void w_sys_manage_outer_settings_label_win::totest()
{
    QString errstring;
    label_img_header::LabelTable table;
    if(!getPrintTable(table, errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    label_img_header::print(table);
}

void w_sys_manage_outer_settings_label_win::topreview()
{
    QString errstring;
    label_img_header::LabelTable table;
    if(!getPrintTable(table, errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    w_sys_manage_outer_settings_img_show dialog(this);
    dialog.showPixmap(label_img_header::pixmapFirst(table));
    transparentCenterDialog(&dialog).exec();
}

bool w_sys_manage_outer_settings_label_win::getPrintTable(label_img_header::LabelTable &table, QString &errstring)
{
    bool ok = false;
    QString text = lds::fileOpenReadAll(ui->comboBox_labelFormat->curusrdata().toString(), &ok, &errstring);
    if(!ok)  {
        return false;
    }
    table = label_img_header::resolveLabel(text);
    table.printerName = ui->comboBox_printerName->currentText();
    return true;
}
