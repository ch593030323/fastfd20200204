#include "saas_guide_num_key.h"
#include "ui_saas_guide_num_key.h"
#include <QTimer>

saas_guide_num_key::saas_guide_num_key(const QString &num, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::saas_guide_num_key)
{
    ui->setupUi(this);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(tonum()));

    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(tonum()));

    connect(ui->pushButton_7, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_8, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_9, SIGNAL(clicked()), this, SLOT(tonum()));

    connect(ui->pushButton_10, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_11, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_12, SIGNAL(clicked()), this, SLOT(tonum()));

    ui->lineEdit->setText(num);
    ui->lineEdit->selectAll();
    QTimer::singleShot(10, ui->lineEdit, SLOT(setFocus()));
}

saas_guide_num_key::~saas_guide_num_key()
{
    delete ui;
}

QString saas_guide_num_key::retNum()
{
    return ui->lineEdit->text();
}

void saas_guide_num_key::tonum()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    if(b) {
        if("<<" == b->text()) {
            ui->lineEdit->backspace();
        } else if("OK" == b->text()) {
            this->accept();
        } else {
            ui->lineEdit->insert(b->text());
        }
    }
}
