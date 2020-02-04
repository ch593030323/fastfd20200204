#include "qt_xiangshan_pay_info.h"
#include "ui_qt_xiangshan_pay_info.h"

qt_XIANGSHAN_pay_info::qt_XIANGSHAN_pay_info(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::qt_XIANGSHAN_pay_info)
{
    ui->setupUi(this);
}

qt_XIANGSHAN_pay_info::~qt_XIANGSHAN_pay_info()
{
    delete ui;
}

void qt_XIANGSHAN_pay_info::upatePay(const QString &payName, const QString &payAmount)
{
    ui->label_paymode->setText(payName);
    ui->label_pay->setText(payAmount);
}
