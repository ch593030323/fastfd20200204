#include "w_scr_dish_main_qr_code_over_view_cash_pay.h"
#include "ui_w_scr_dish_main_qr_code_over_view_cash_pay.h"
#include <QKeyEvent>
#include "w_scr_dish_pay_widget.h"
#include "lds_messagebox.h"

w_scr_dish_main_qr_code_over_view_cash_pay::w_scr_dish_main_qr_code_over_view_cash_pay(double num_cost, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_main_qr_code_over_view_cash_pay)
{
    ui->setupUi(this);
    this->num_cost = num_cost;
    double num_cash = w_scr_dish_pay_widget::valuerountbysys(num_cost);
    double num_round =  num_cash - num_cost;
    //
    ui->lineEdit_cost->setLabelText(QObject::tr("应收")); ui->lineEdit_cost->setBgColor(QColor("#ff546b"));
    ui->lineEdit_cash->setLabelText(QObject::tr("实收")); ui->lineEdit_cash->setBgColor(QColor("#ff546b"));
    ui->lineEdit_round->setLabelText(QObject::tr("抹零")); ui->lineEdit_round->setBgColor(QColor("#ff546b"));

    ui->lineEdit_money->setLabelText(QObject::tr("现金")); ui->lineEdit_money->setBgColor(QColor("#green"));ui->lineEdit_money->enableSelectAll();
    ui->lineEdit_change->setLabelText(QObject::tr("找零")); ui->lineEdit_change->setBgColor(QColor("#ff546b"));

    ui->lineEdit_money->setValue(num_cash);ui->lineEdit_money->selectAll();
    ui->lineEdit_change->setValue(0);
    ui->lineEdit_cost->setValue(num_cost);
    ui->lineEdit_cash->setValue(num_cash);
    ui->lineEdit_round->setValue(num_round);

    this->setWindowTitle(QObject::tr("现金支付"));

    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));

    connect(ui->pushButton_12, SIGNAL(clicked()), this, SLOT(toNum()));
    connect(ui->pushButton_7, SIGNAL(clicked()), this, SLOT(toNum()));
    connect(ui->pushButton_16, SIGNAL(clicked()), this, SLOT(toNum()));
    connect(ui->pushButton_9, SIGNAL(clicked()), this, SLOT(toNum()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(toNum()));
    connect(ui->pushButton_10, SIGNAL(clicked()), this, SLOT(toNum()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(toNum()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(toNum()));
    connect(ui->pushButton_8, SIGNAL(clicked()), this, SLOT(toNum()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(toNum()));
    connect(ui->pushButton_11, SIGNAL(clicked()), this, SLOT(toNum()));
    connect(ui->pushButton_17, SIGNAL(clicked()), this, SLOT(toNum()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(toNum()));
    connect(ui->pushButton_15, SIGNAL(clicked()), this, SLOT(toNum()));
    connect(ui->pushButton_14, SIGNAL(clicked()), this, SLOT(toNum()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(toNum()));
    connect(ui->lineEdit_money,SIGNAL(valueChanged(double)),this,SLOT(updateChange()));

}

w_scr_dish_main_qr_code_over_view_cash_pay::~w_scr_dish_main_qr_code_over_view_cash_pay()
{
    delete ui;
}

double w_scr_dish_main_qr_code_over_view_cash_pay::get_num_money()
{
        return ui->lineEdit_money->value();
}

double w_scr_dish_main_qr_code_over_view_cash_pay::get_num_change()
{
        return ui->lineEdit_change->value();
}

double w_scr_dish_main_qr_code_over_view_cash_pay::get_num_cost()
{
        return ui->lineEdit_cost->value();
}

double w_scr_dish_main_qr_code_over_view_cash_pay::get_num_cash()
{
        return ui->lineEdit_cash->value();
}

double w_scr_dish_main_qr_code_over_view_cash_pay::get_num_round()
{
    return ui->lineEdit_round->value();
}

void w_scr_dish_main_qr_code_over_view_cash_pay::took()
{
    if(ui->lineEdit_change->value() < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("付款金额不足"));
        return;
    }
    //
    this->accept();
}

void w_scr_dish_main_qr_code_over_view_cash_pay::tocancel()
{
    this->reject();
}

void w_scr_dish_main_qr_code_over_view_cash_pay::toNum()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    if(b) {
        if("10" == b->text()) {
            ui->lineEdit_money->setValue(10);
            ui->lineEdit_money->selectAll();
        }
        if("20" == b->text()) {
            ui->lineEdit_money->setValue(20);
            ui->lineEdit_money->selectAll();
        }
        if("50" == b->text()) {
            ui->lineEdit_money->setValue(50);
            ui->lineEdit_money->selectAll();
        }
        if("100" == b->text()) {
            ui->lineEdit_money->setValue(100);
            ui->lineEdit_money->selectAll();
        }
        if(b->text().length() == 1) {
            if("C" == b->text()) {
                ui->lineEdit_money->setValue(0);
            } else {
                ui->lineEdit_money->setValue(b->text());
            }
        }
    }
}

void w_scr_dish_main_qr_code_over_view_cash_pay::updateChange()
{
    double change = ui->lineEdit_money->value() - ui->lineEdit_cash->value();
    ui->lineEdit_change->setValue(change);
}

void w_scr_dish_main_qr_code_over_view_cash_pay::keyPressEvent(QKeyEvent *e)
{
    const int kvalue = e->key();
    if( (kvalue >= Qt::Key_0 && kvalue <= Qt::Key_9)
            || kvalue == Qt::Key_Period) {
        ui->lineEdit_money->setValue(e->text());

    } else  if(kvalue == Qt::Key_Delete
               || kvalue == Qt::Key_Backspace) {
        ui->lineEdit_money->setValue(0);
    }

    QWidget::keyPressEvent(e);
}
