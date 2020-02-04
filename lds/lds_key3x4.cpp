#include "lds_key3x4.h"
#include "ui_lds_key3x4.h"
#include <QPointer>
#include <QApplication>
#include <QLineEdit>
#include <QAbstractSpinBox>
#include <QKeyEvent>

lds_key3x4::lds_key3x4(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::lds_key3x4)
{
    ui->setupUi(this);
    this->setMinimumWidth(200);
    this->setMinimumHeight(240);

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
    connect(ui->pushButton_backspace,SIGNAL(clicked()),this,SLOT(set_num()));

    showExtra(false);
}

lds_key3x4::~lds_key3x4()
{
    delete ui;
}

void lds_key3x4::setDotEnabled(bool f)
{
    ui->pushButton_dot->setEnabled(f);
}

void lds_key3x4::showExtra(bool show)
{
//    ui->pushButton_num_extra->setHidden(!show);
//    ui->pushButton_num_extra_2->setHidden(!show);
//    ui->pushButton_num_extra_3->setHidden(!show);
//    ui->pushButton_num_extra_4->setHidden(!show);
}

void lds_key3x4::set_num()
{
    QPushButton *btn=qobject_cast<QPushButton *>(sender());
    QPointer<QWidget> w = qApp->focusWidget();
    if(!w) return;

    //清除
    if(btn == ui->pushButton_backspace) {
        QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Backspace,Qt::NoModifier);
        qApp->sendEvent(w, &keyPress);
        return;
//        QLineEdit *l = qobject_cast<QLineEdit *>(w);
//        if(l) {l->clear();return;}
//        QAbstractSpinBox *s = qobject_cast<QAbstractSpinBox *>(w);
//        if(s) {s->clear();return;}
//        return;
    }
    //键盘数字
    QKeyEvent keyPress(QEvent::KeyPress, btn->text().data()->unicode(), Qt::NoModifier, btn->text());
    qApp->sendEvent(w, &keyPress);
}
