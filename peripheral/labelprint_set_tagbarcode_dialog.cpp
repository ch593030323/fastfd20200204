#include "labelprint_set_tagbarcode_dialog.h"
#include "ui_labelprint_set_tagbarcode_dialog.h"
#include <QSettings>
#include "commonheader.h"
#include "lds.h"


labelprint_set_tagbarcode_dialog::labelprint_set_tagbarcode_dialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::labelprint_set_tagbarcode_dialog),
    printer(0)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    this->setWindowTitle(QObject::tr("标签设置"));

    bool istag = lds::sysconf->value(("label_print/istag"),true).toBool();
    ui->radioButton_barcode->setChecked(!istag);
    ui->radioButton_tag->setChecked(istag);

    ui->lineEdit_gap->setText(lds::sysconf->value("label_print/gap","0").toString());
    ui->lineEdit_height->setText(lds::sysconf->value("label_print/height","320").toString());
    ui->lineEdit_width->setText(lds::sysconf->value("label_print/width","720").toString());

    ui->comboBox_column->setCurrentIndex(ui->comboBox_column->findText(lds::sysconf->value("label_print/column","2").toString()));
    ui->comboBox_danwei->setCurrentIndex(ui->comboBox_danwei->findText(lds::sysconf->value("label_print/danwei","dots").toString()));
    ui->comboBox_fengbianlv->setCurrentIndex(ui->comboBox_fengbianlv->findText(lds::sysconf->value("label_print/fengbianlv","200DPI").toString()));

    ui->label_g->setText(ui->comboBox_danwei->currentText());
    ui->label_h->setText(ui->comboBox_danwei->currentText());
    ui->label_w->setText(ui->comboBox_danwei->currentText());

    update3label();
    pre_danwei = ui->comboBox_danwei->currentText();
    pre_fengbianlv = ui->comboBox_fengbianlv->currentText();

    connect(ui->comboBox_fengbianlv,SIGNAL(currentIndexChanged(QString)),this,SLOT(update3fengbianlv(QString)));
    connect(ui->comboBox_danwei,SIGNAL(currentIndexChanged(QString)),this,SLOT(update3danwei(QString)));
    connect(ui->radioButton_barcode,SIGNAL(clicked()),this,SLOT(update3label()));
    connect(ui->radioButton_tag,SIGNAL(clicked()),this,SLOT(update3label()));

    ui->radioButton_barcode->click();
}

labelprint_set_tagbarcode_dialog::~labelprint_set_tagbarcode_dialog()
{
    if(printer) {delete printer;printer=0;}
    delete ui;
}

void labelprint_set_tagbarcode_dialog::tosave()
{
    lds::sysconf->setValue("label_print/istag", ui->radioButton_tag->isChecked());

    lds::sysconf->setValue("label_print/gap", ui->lineEdit_gap->text());
    lds::sysconf->setValue("label_print/height", ui->lineEdit_height->text());
    lds::sysconf->setValue("label_print/width", ui->lineEdit_width->text());

    lds::sysconf->setValue("label_print/column", ui->comboBox_column->currentText());
    lds::sysconf->setValue("label_print/danwei", ui->comboBox_danwei->currentText());
    lds::sysconf->setValue("label_print/fengbianlv", ui->comboBox_fengbianlv->currentText());

    //    this->accept();
}

void labelprint_set_tagbarcode_dialog::toexit()
{
    //    this->reject();
}

void labelprint_set_tagbarcode_dialog::update3fengbianlv(const QString &fengbianlv)
{
    if(pre_danwei == "mm") {
    } else {
        double f1 = 1.0;
        if(pre_fengbianlv == "200DPI") {//[pre_fengbianlv] to [200dpi]
            f1 = 1.0;
        } else if(pre_fengbianlv == "300DPI") {
            f1 = 2.0 / 3.0;
        }

        double f2 = 1.0;
        if(fengbianlv == "200DPI") { //[200dpi] to [fengbianlv]
            f2 = 1.0;
        } else if(fengbianlv == "300DPI") {
            f2 = 3.0 / 2.0;
        }
        ui->lineEdit_gap->setText(QString::number((ui->lineEdit_gap->text().toFloat() * f1 * f2)));
        ui->lineEdit_width->setText(QString::number((ui->lineEdit_width->text().toFloat() * f1 * f2)));
        ui->lineEdit_height->setText(QString::number((ui->lineEdit_height->text().toFloat() * f1 * f2)));
    }
    //记录
    pre_danwei = ui->comboBox_danwei->currentText();
    pre_fengbianlv = ui->comboBox_fengbianlv->currentText();
}

void labelprint_set_tagbarcode_dialog::update3danwei(const QString &danwei)
{
    ui->label_g->setText(danwei);
    ui->label_h->setText(danwei);
    ui->label_w->setText(danwei);

    double f1 = 1.0;
    if(pre_danwei == "mm") {//[pre_danwei] to [200dpi]
        f1 = 8.0;
    } else if(pre_fengbianlv == "200DPI") {
        f1 = 1.0;
    } else if(pre_fengbianlv == "300DPI") {
        f1 = 2.0 / 3.0;
    }

    double f2 = 1.0;
    if(danwei == "mm") { //[200dpi] to [fengbianlv]
        f2 = 1.0 / 8.0;
    } else if(pre_fengbianlv == "200DPI") {
        f2 = 1.0;
    } else if(pre_fengbianlv == "300DPI") {
        f2 = 3.0 / 2.0;
    }
    ui->lineEdit_gap->setText(QString::number((ui->lineEdit_gap->text().toFloat() * f1 * f2)));
    ui->lineEdit_width->setText(QString::number((ui->lineEdit_width->text().toFloat() * f1 * f2)));
    ui->lineEdit_height->setText(QString::number((ui->lineEdit_height->text().toFloat() * f1 * f2)));

    //记录
    pre_danwei = ui->comboBox_danwei->currentText();
    pre_fengbianlv = ui->comboBox_fengbianlv->currentText();
}

void labelprint_set_tagbarcode_dialog::update3label()
{
    //8 5 17
    if(ui->radioButton_barcode->isChecked()) {
        ui->label_8->setText(QObject::tr("条码纸宽度"));
        ui->label_5->setText(QObject::tr("条码纸高度"));

        ui->label_4->setVisible(true);
        ui->comboBox_column->setVisible(true);

    } else if(ui->radioButton_tag->isChecked()) {
        ui->label_8->setText(QObject::tr("标签打印机宽度"));
        ui->label_5->setText(QObject::tr("标签打印机高度"));

        ui->label_4->setVisible(false);
        ui->comboBox_column->setVisible(false);
    }
}
