#include "barscale_num_input.h"
#include "ui_barscale_num_input.h"
#include <QPointer>
#include <QKeyEvent>
#include "commonheader.h"
#include <QSettings>
#include <QtDebug>
#include "lds.h"

barscale_num_input::barscale_num_input(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::barscale_num_input)
{
    ui->setupUi(this);

    updatem(lds::sysconf->value("barscale_num_input/mode", 0).toInt());

    connect(ui->pushButton_num0,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num1,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num2,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num3,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num4,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num5,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num6,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num7,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num8,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num9,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_dot,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_clear,SIGNAL(clicked()),this,SLOT(set_num()));

    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->pushButton_switch,SIGNAL(clicked()),this,SLOT(tomodelswitch()));
}

barscale_num_input::~barscale_num_input()
{
    delete ui;
}

QString barscale_num_input::retvalue()
{
    return ui->lineEdit->text();
}

void barscale_num_input::set_num()
{
    QPushButton *btn=qobject_cast<QPushButton *>(sender());
    QPointer<QWidget> w = qApp->focusWidget();
    if(!w) return;

    //清除
    if(btn == ui->pushButton_clear) {
        QLineEdit *l = qobject_cast<QLineEdit *>(w);
        if(l) {l->clear();return;}
        QAbstractSpinBox *s = qobject_cast<QAbstractSpinBox *>(w);
        if(s) {s->clear();return;}
        return;
    }
    //键盘数字
    QKeyEvent keyPress(QEvent::KeyPress, btn->text().data()->unicode(),Qt::NoModifier, btn->text());
    qApp->sendEvent(w, &keyPress);
    //    if(!w) return;
    //    QKeyEvent keyRelease(QEvent::KeyRelease, btn->text().data()->unicode(),Qt::NoModifier, QString());
    //    qApp->sendEvent(w, &keyRelease);
}

void barscale_num_input::tomodelswitch()
{
    ++mode;
    mode = (mode)%2;
    updatem(mode);
}

void barscale_num_input::updatem(int m)
{

    switch(m) {
    case 0:
        ui->pushButton_switch->setText(QObject::tr("替换"));
        break;
    case 1:
        ui->pushButton_switch->setText(QObject::tr("倍数"));
        break;
    }
    lds::sysconf->setValue("barscale_num_input/mode", m);
    mode = m;
}
