#include "public_printby_ch_billno_over_view_set.h"
#include "ui_public_printby_ch_billno_over_view_set.h"
#include <QtGui>

public_printby_ch_billno_over_view_set::public_printby_ch_billno_over_view_set(int paperwidth, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::public_printby_ch_billno_over_view_set)
{
    ui->setupUi(this);
    ui->comboBox->addItem("80mm", 48);
    ui->comboBox->addItem("58mm", 32);

    int index = ui->comboBox->findData(paperwidth);
    if(index >= 0) {
        ui->comboBox->setCurrentIndex(index);
    } else {
        ui->comboBox->setEditText(QString::number(paperwidth));
    }

    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(accept()));
}

public_printby_ch_billno_over_view_set::~public_printby_ch_billno_over_view_set()
{
    delete ui;
}

int public_printby_ch_billno_over_view_set::paperwidth()
{
    int index = ui->comboBox->findText(ui->comboBox->currentText());
    if(index >= 0) {
        return ui->comboBox->itemData(ui->comboBox->currentIndex()).toInt();
    }
    return   ui->comboBox->currentText().toInt();
}
