#include "lds_dialog_input_double.h"
#include "ui_lds_dialog_input_dialog_double.h"
#include <QTimer>
#include "lds.h"
#include <QtDebug>
#include "lds_roundeddialog_rect_align.h"
#include "lds_messagebox.h"

lds_dialog_input_double::lds_dialog_input_double(QWidget *parent, bool show_second_line) :
    QDialog(parent),
    ui(new Ui_lds_dialog_input_dialog_double)
{
    ui->setupUi(this);

    ui->label->setText("");
    ui->label_2->setText("");
    ui->label_desc->setText("");

    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(toexit()));

    lds::showWidget(ui->label_2, show_second_line);
    lds::showWidget(ui->lineEdit_2, show_second_line);
    lds::hideWidget(ui->checkBox);
    lds::hideWidget(ui->label_desc);

    this->setWindowTitle(MESSAGE_TITLE_VOID);
}

lds_dialog_input_double::~lds_dialog_input_double()
{
    delete ui;
}

void lds_dialog_input_double::showLabelDesc(const QString &desc)
{
    lds::showWidget(ui->label_desc);
    ui->label_desc->setText(desc);
}

void lds_dialog_input_double::disableDot()
{
    ui->widget_key3x4->setDotEnabled(false);
}

void lds_dialog_input_double::connect_discount_amount_transfer(double total)
{
    this->total = total;
    connect(ui->lineEdit, SIGNAL(valueChanged(double)), this, SLOT(udpate_amount(double)));
    connect(ui->lineEdit_2, SIGNAL(valueChanged(double)), this, SLOT(update_discount(double)));
}

void lds_dialog_input_double::update_discount(double value)
{
    if(ui->lineEdit_2 == qApp->focusWidget()) {
        ui->lineEdit->setTrueValue(int(value / total * 100.0));
    }
}

void lds_dialog_input_double::udpate_amount(double value)
{
    if(ui->lineEdit == qApp->focusWidget()) {
        ui->lineEdit_2->setValue(value / 100.0 * total );
    }
}

void lds_dialog_input_double::took()
{
    this->accept();
}

void lds_dialog_input_double::toexit()
{
    this->reject();
}

void lds_dialog_input_double::delay_set_focus()
{
    QTimer::singleShot(100,this,SLOT(lineselectall()));
}

void lds_dialog_input_double::lineselectall()
{
    if(ui->lineEdit->isEnabled()
            && !ui->lineEdit->isReadOnly()
            && !ui->lineEdit->isHidden())  {
        ui->lineEdit->selectAll();
        ui->lineEdit->setFocus();
    } else{
        ui->lineEdit_2->selectAll();
        ui->lineEdit_2->setFocus();
    }
}
