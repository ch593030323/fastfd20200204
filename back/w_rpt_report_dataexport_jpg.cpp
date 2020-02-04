#include "w_rpt_report_dataexport_jpg.h"
#include "ui_w_rpt_report_dataexport_jpg.h"
#include "printer_0.h"
#include "printer_pos_serial.h"
#include "bmp_info.h"
#include "printer_normal.h"
#include "lds_messagebox.h"

w_rpt_report_dataExport_JPG::w_rpt_report_dataExport_JPG(const QString &savePath, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_rpt_report_dataExport_JPG)
{
    ui->setupUi(this);
    this->savePath = savePath;
    this->setWindowTitle(savePath);
    ui->label_image->setScaledContents(true);
    ui->comboBox_printpath->addItems(printer_0::f_scan_usbPort());

    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_export, SIGNAL(clicked()), this, SLOT(toexport()));
    connect(ui->pushButton_print, SIGNAL(clicked()), this, SLOT(toprint()));
}

w_rpt_report_dataExport_JPG::~w_rpt_report_dataExport_JPG()
{
    delete ui;
}

void w_rpt_report_dataExport_JPG::setLabelPixmap(const QPixmap &pixmap)
{
    this->pixmap = pixmap;
    ui->label_image->setPixmap(pixmap.scaledToWidth(700, Qt::SmoothTransformation));
}

void w_rpt_report_dataExport_JPG::toprint()
{
    QString errstring;
    QByteArray pic_ins;
    Bmp_info::pic2Ins(pixmap.toImage(), pic_ins);
    blockIns ins;
    ins << pieceIns("", pic_ins);
    if(false == Printer_Normal::toprint(ui->comboBox_printpath->currentText(), ins, 1, &errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
}

void w_rpt_report_dataExport_JPG::tocancel()
{
    this->reject();
}

void w_rpt_report_dataExport_JPG::toexport()
{
    if(false == pixmap.save(savePath)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("保存失败"));
        return;
    }
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
}
