#include "pay_helper_input.h"
#include "ui_pay_helper_input.h"
#include "pay_helper_set.h"
#include <QMessageBox>
#include "pay_helper_pay.h"


pay_helper_input::pay_helper_input(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::pay_helper_input)
{
    ui->setupUi(this);

    this->setWindowTitle(QObject::tr("支付助手"));
    updateOkText();

    //
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_backspace, SIGNAL(clicked()), this, SLOT(tobackspace()));
    connect(ui->pushButton_8, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_7, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_14, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_10, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_12, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_15, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_set, SIGNAL(clicked()), this, SLOT(toset()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
}

pay_helper_input::~pay_helper_input()
{
    delete ui;
}

void pay_helper_input::took()
{
    pay();
}

void pay_helper_input::tocancel()
{
    this->accept();
}

void pay_helper_input::toset()
{
    pay_helper_set dialog(this);
    dialog.exec();
    updateOkText();
}

void pay_helper_input::toexit()
{
    if(1 == QMessageBox::question(this, this->windowTitle(), QObject::tr("确定退出"), QObject::tr("确定"), QObject::tr("取消"))) {
        return;
    }
    qApp->quit();
}

void pay_helper_input::tonum()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    if(b == 0)
        return;
    ui->lineEdit->insert(b->text());
}

void pay_helper_input::tobackspace()
{
    ui->lineEdit->backspace();
}

void pay_helper_input::updateOkText()
{
    QSettings sysconf(qApp->applicationDirPath() + "/conf.ini", QSettings::IniFormat, this);
    QString k = sysconf.value("pay_helper_set/quick_pay_type", "").toString();

    if(k == "") {
        ui->label->setText(QObject::tr("金额"));
    }
    if(k == "a") {
        ui->label->setText(QObject::tr("支付宝"));
    }
    if(k == "ma") {
        ui->label->setText(QObject::tr("摇钱树支付") + "/" + QObject::tr("支付宝"));
    }
    if(k == "mw") {
        ui->label->setText(QObject::tr("摇钱树支付") + "/" + QObject::tr("微信"));
    }
    if(k == "m") {
        ui->label->setText(QObject::tr("摇钱树支付"));
    }
}

void pay_helper_input::pay()
{
    bool ok;
    ui->lineEdit->text().toDouble(&ok);
    if(!ok) {
        QMessageBox::warning(this, this->windowTitle(), QObject::tr("金额必须为有效数字"));
        return;
    }
    QSettings sysconf(qApp->applicationDirPath() + "/conf.ini", QSettings::IniFormat, this);
    const QString k = sysconf.value("pay_helper_set/quick_pay_type", "").toString();
    QString p;
    if(k == "") {
        switch(QMessageBox::question(this, this->windowTitle(), "", QObject::tr("支付宝"), QObject::tr("摇钱树支付"), QObject::tr("取消"))) {
        case 0:
            p = "a";
            break;
        case 1:
            switch(QMessageBox::question(this, this->windowTitle(), QObject::tr("摇钱树支付"), QObject::tr("支付宝"), QObject::tr("微信"), QObject::tr("取消"))) {
            case 0:
                p = "ma";
                break;
            case 1:
                p = "mw";
                break;
            case 2:
                return;
            }

            break;
        case 2:
            return;
        }
    }
    if(k == "a") {
        p = "a";
    }
    if(k == "ma") {
        p = "ma";
    }
    if(k == "mw") {
        p = "mw";
    }
    if(k == "m") {
        switch(QMessageBox::question(this, this->windowTitle(), QObject::tr("摇钱树支付"), QObject::tr("支付宝"), QObject::tr("微信"), QObject::tr("取消"))) {
        case 0:
            p = "ma";
            break;
        case 1:
            p = "mw";
            break;
        case 2:
            return;
        }
    }

    pay_helper_pay dialog(ui->lineEdit->text(), p, this);
    if(QDialog::Accepted == dialog.exec()) {
        this->accept();
    }
}
