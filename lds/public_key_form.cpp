#include "public_key_form.h"
#include "ui_public_key_form.h"
#include <QFontDatabase>
#include <QtDebug>
#include <QPointer>
#include <QApplication>
#include <QLineEdit>
#include <QAbstractSpinBox>
#include <QKeyEvent>

public_key_Form::public_key_Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::public_key_Form)
{
    ui->setupUi(this);
    //3
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

}

public_key_Form::~public_key_Form()
{
    delete ui;
}

void public_key_Form::set_num()
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
