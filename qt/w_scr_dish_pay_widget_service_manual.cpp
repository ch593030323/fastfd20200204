#include "w_scr_dish_pay_widget_service_manual.h"
#include "ui_w_scr_dish_pay_widget_service_manual.h"
#include "w_scr_dish_pay_widget.h"
#include "ui_w_scr_dish_pay_widget.h"
#include "w_scr_dish_pay_widget.h"

w_scr_dish_pay_widget_service_manual::w_scr_dish_pay_widget_service_manual(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_pay_widget_service_manual)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("手工处理服务费"));
    ui->doubleSpinBox_num_cost->setEnabled(false);
    ui->doubleSpinBox_num_lowcost->setEnabled(false);
    ui->doubleSpinBox_num_realreceive->setEnabled(false);
    ui->doubleSpinBox_num_paid->setEnabled(false);
    ui->doubleSpinBox_num_blotout->setEnabled(false);
    ui->widget->showExtra(true);

    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));

    connect(ui->doubleSpinBox_num_service, SIGNAL(valueChanged(double)), this, SLOT(toservicechange(double)));
    connect(ui->doubleSpinBox_num_money, SIGNAL(valueChanged(double)), this, SLOT(topaychange(double)));
}

w_scr_dish_pay_widget_service_manual::~w_scr_dish_pay_widget_service_manual()
{
    delete ui;
}

void w_scr_dish_pay_widget_service_manual::writeUi(Ui_w_scr_dish_pay_widget *ui)
{
    obj.writeUi(ui);
}

void w_scr_dish_pay_widget_service_manual::readUi(Ui_w_scr_dish_pay_widget *ui, w_scr_dish_pay_widget *obj)
{
    this->obj.readUi(ui, obj);
    this->obj.writeUi(this->ui);
    this->ui->doubleSpinBox_num_money->setMinimum(0);
    this->ui->doubleSpinBox_num_service->setMinimum(0);
}

void w_scr_dish_pay_widget_service_manual::took()
{
    this->accept();
}

void w_scr_dish_pay_widget_service_manual::tocancel()
{
    this->reject();
}

void w_scr_dish_pay_widget_service_manual::toservicechange(double value)
{
    if(qApp->focusWidget() != ui->doubleSpinBox_num_service)
        return;
    obj.setService(value);
    obj.writeUi(ui);
}

void w_scr_dish_pay_widget_service_manual::topaychange(double value)
{
    if(qApp->focusWidget() != ui->doubleSpinBox_num_money)
        return;
    obj.setMoney(value);
    obj.writeUi(ui);
}


w_scr_dish_pay_widget_service_manual_obj::w_scr_dish_pay_widget_service_manual_obj(QObject *parent) : QObject(0)
{

}

bool w_scr_dish_pay_widget_service_manual_obj::setService(bool enable, double service)
{
    PayInterData x = d;
    if(!updateService(x, enable, service)) {
        return false;
    }
    d = x;
    return true;
}

bool w_scr_dish_pay_widget_service_manual_obj::setService(double service)
{
    return setService(true, service);
}

bool w_scr_dish_pay_widget_service_manual_obj::setMoney(double money)
{
    PayInterData x = d;
    if(!updateMoney(x, money)) {
        return false;
    }
    d = x;
    return true;
}

void w_scr_dish_pay_widget_service_manual_obj::print()
{
    d.print(__LINE__);
}

void w_scr_dish_pay_widget_service_manual_obj::writeUi(Ui_w_scr_dish_pay_widget *ui)
{
    d.writeUi(ui);
}

void w_scr_dish_pay_widget_service_manual_obj::writeUi(Ui::w_scr_dish_pay_widget_service_manual *ui)
{
    ui->doubleSpinBox_num_cost->setValue(d.num_cost);
    ui->doubleSpinBox_num_service->setValue(d.num_service);
    //    ui->doubleSpinBox_num_pay_low->setValue(d.num_pay_low);
    ui->doubleSpinBox_num_lowcost->setValue(d.num_lowcost);
    ui->doubleSpinBox_num_paid->setValue(d.num_paid);
    ui->doubleSpinBox_num_realreceive->setValue(d.num_realreceive);
    ui->doubleSpinBox_num_money->setValue(d.num_money);
    ui->doubleSpinBox_num_blotout->setValue(d.num_blotout);

    ui->doubleSpinBox_num_service->setPrefix(d.enable_service ? "" : QObject::tr("免"));
    ui->doubleSpinBox_num_lowcost->setPrefix(d.enable_lowcost ? "" : QObject::tr("免"));

}

void w_scr_dish_pay_widget_service_manual_obj::readUi(Ui_w_scr_dish_pay_widget *ui, w_scr_dish_pay_widget *obj)
{
    d.readUi(ui, obj);
}

bool w_scr_dish_pay_widget_service_manual_obj::isServiceEnable()
{
    return d.enable_service;
}

bool w_scr_dish_pay_widget_service_manual_obj::updateMoney(PayInterData &d, double money)
{
    d.enable_service = true;

    d.num_money = money;
    d.num_realreceive = d.num_money + d.num_paid;

    double A = d.num_realreceive + d.num_blotout;
    if(d.enable_lowcost == false) {
        if(d.num_cost >  A) {
            qDebug() << "no lowcost,  min num_money :" << (d.num_cost - d.num_blotout - d.num_paid);
            return false;
        }
        d.num_service = A - d.num_cost;
        d.num_lowcost = 0;
        goto end;
    }
    if(d.num_pay_low >  A) {
        qDebug() << "min num_money :" << (d.num_pay_low - d.num_blotout - d.num_paid);
        return false;
    }

    if(d.num_pay_low < A ) {//允许的话默认lowcost不变
        if(d.num_lowcost < A - d.num_cost) {
            d.num_lowcost;
            d.num_service = A - d.num_cost - d.num_lowcost;
        } else {
            d.num_lowcost = A - d.num_cost;
            d.num_service = 0;
        }
        goto end;
    }
    if(d.num_pay_low == A ) {//nothing happened

    }
end:
    return d.isCorrect();
}

bool w_scr_dish_pay_widget_service_manual_obj::updateService(w_scr_dish_pay_widget_service_manual_obj::PayInterData &d, bool enable, double service)
{
    d.num_service = service;
    d.enable_service = enable;
    //
    d.num_lowcost = qMax(0.0, d.num_pay_low - d.num_cost - d.curService());
    double v = d.num_cost + d.curLow() + d.curService();
    d.num_realreceive = w_scr_dish_pay_widget::valuerountbysys(v);
    d.num_blotout = v - d.num_realreceive;
    d.num_money = d.num_realreceive - d.num_paid;

    return d.isCorrect();
}

void w_scr_dish_pay_widget_service_manual_obj::PayInterData::print(int line)
{
    qDebug() << line <<  "num_cost:" << num_cost;
    qDebug() << line <<  "num_service:" << num_service;
    qDebug() << line <<  "enable_service:" << enable_service;
    qDebug() << line <<  "num_pay_low:" << num_pay_low;
    qDebug() << line <<  "num_lowcost:" << num_lowcost;
    qDebug() << line <<  "enable_lowcost:" << enable_lowcost;
    qDebug() << line <<  "num_paid:" << num_paid;
    qDebug() << line <<  "num_realreceive:" << num_realreceive;
    qDebug() << line <<  "num_money:" << num_money;
    qDebug() << line <<  "num_blotout:" << num_blotout;
}


double w_scr_dish_pay_widget_service_manual_obj::PayInterData::curService()
{
    return enable_service ? num_service : 0;
}

double w_scr_dish_pay_widget_service_manual_obj::PayInterData::curLow()
{
    return enable_lowcost ? num_lowcost : 0;
}

bool w_scr_dish_pay_widget_service_manual_obj::PayInterData::isCorrect()
{
    return  num_lowcost >=0
            && num_service >= 0
            && num_realreceive >= 0
            && num_money >= 0
            && num_realreceive == (num_cost + curService() + curLow() - num_blotout);
}

void w_scr_dish_pay_widget_service_manual_obj::PayInterData::writeUi(Ui_w_scr_dish_pay_widget *ui)
{
    ui->lineEdit_service->setPrefix(enable_service ? "" : QObject::tr("免"));
    ui->lineEdit_lowcost->setPrefix(enable_lowcost ? "" : QObject::tr("免"));
    ui->lineEdit_service->setValue(num_service);
    ui->lineEdit_lowcost->setValue(num_lowcost);
    ui->lineEdit_realreceive->setValue(num_realreceive);
    ui->lineEdit_moneypay->setValue(num_money);
    ui->lineEdit_blotout->setValue(num_blotout);
}

void w_scr_dish_pay_widget_service_manual_obj::PayInterData::readUi(Ui_w_scr_dish_pay_widget *ui, w_scr_dish_pay_widget *obj)
{
    num_cost = obj->pay_without_low_service();
    enable_service = ui->lineEdit_service->prefix().count() == 0;
    enable_lowcost = ui->lineEdit_lowcost->prefix().count() == 0;
    num_pay_low = obj->pay_keep_data.num_pay_low;
    num_paid = obj->hasPaid();

    num_service = ui->lineEdit_service->value();
    num_lowcost = ui->lineEdit_lowcost->value();
    num_realreceive = ui->lineEdit_realreceive->value();
    num_money = ui->lineEdit_moneypay->value();
    num_blotout = ui->lineEdit_blotout->value();
}
