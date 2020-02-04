#include "pay_helper_loading.h"
#include "ui_pay_helper_loading.h"
#include <QMovie>
#include <QTimer>

pay_helper_loading::pay_helper_loading(QWidget *parent, const QString &text) :
    QDialog(parent),
    ui(new Ui::pay_helper_loading)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
    ui->label_loading->setMovie(new QMovie(this));
    ui->label_loading->movie()->setFileName("://rec/loading.gif");
    ui->label_loading->movie()->start();
    ui->label_text->setText(text);
}

pay_helper_loading::~pay_helper_loading()
{
    delete ui;
}

void pay_helper_loading::showdelay()
{
    this->show();

    QEventLoop loop;
    QTimer::singleShot(100, &loop, SLOT(quit()));
    loop.exec();
}
