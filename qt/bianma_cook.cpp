#include "bianma_cook.h"
#include "ui_bianma_cook.h"
#include <QtDebug>
#include <QTimer>

bianma_cook::bianma_cook(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::bianma_cook)
{
    ui->setupUi(this);
    connect(ui->pushButton_11,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_20,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_14,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_13,SIGNAL(clicked()),this,SLOT(tocenter()));

    connect(ui->pushButton_22,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_21,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_17,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_25,SIGNAL(clicked()),this,SLOT(tocenter()));

    connect(ui->pushButton_19,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_7,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_31,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_12,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_6,SIGNAL(clicked()),this,SLOT(tocenter()));

    connect(ui->pushButton_28,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_39,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_15,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_24,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_34,SIGNAL(clicked()),this,SLOT(tocenter()));

    connect(ui->pushButton_10,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_8,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_18,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_33,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_26,SIGNAL(clicked()),this,SLOT(tocenter()));

    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_5,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_36,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_4,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_16,SIGNAL(clicked()),this,SLOT(tocenter()));

    connect(ui->pushButton_9,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_38,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_32,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_37,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_enter,SIGNAL(clicked()),this,SLOT(tocenter()));

    connect(ui->pushButton_35,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_29,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_23,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_27,SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_30,SIGNAL(clicked()),this,SLOT(tocenter()));

    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SIGNAL(edit_text(QString)));
//    ui->lineEdit->setFocusPolicy(Qt::NoFocus);
//    ui->lineEdit->setReadOnly(true);
    QTimer::singleShot(100, ui->lineEdit, SLOT(setFocus()));
}

bianma_cook::~bianma_cook()
{
    delete ui;
}

void bianma_cook::cleartext()
{
    ui->lineEdit->clear();
}

void bianma_cook::tocenter()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    if(b) {
        if(b->text() == "<-") {
            ui->lineEdit->backspace();
        } else if(b->text() == "enter") {
            emit edit_text("enter");
        } else if(b->text() == "clear") {
            ui->lineEdit->clear();
        } else {
            ui->lineEdit->insert(b->text());
        }
    }
}

void bianma_cook::showEvent(QShowEvent *e)
{
    cleartext();
    QWidget::showEvent(e);
}
