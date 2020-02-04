#include "lds_dialog_input3.h"
#include "ui_lds_dialog_input3.h"
#include "lds_roundeddialog_rect_align.h"

lds_dialog_input3::lds_dialog_input3(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::lds_dialog_input3)
{
    ui->setupUi(this);
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
}

lds_dialog_input3::~lds_dialog_input3()
{
    delete ui;
}

void lds_dialog_input3::took()
{
    this->accept();
}

void lds_dialog_input3::toexit()
{
    this->reject();
}

