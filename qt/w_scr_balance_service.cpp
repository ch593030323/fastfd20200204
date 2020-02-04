#include "w_scr_balance_service.h"
#include "ui_w_scr_balance_service.h"
#include "lds_messagebox.h"

w_scr_balance_service::w_scr_balance_service(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_balance_service)
{
    ui->setupUi(this);

}

w_scr_balance_service::~w_scr_balance_service()
{
    delete ui;
}

QString w_scr_balance_service::get_returnvalue() const
{
    return is_return;
}

void w_scr_balance_service::on_pushButton_ok_clicked()
{
    QString ls_return;
    QString ls_service;

    if(ui->radioButton->isChecked()) {//按系统设置收取服务费
        ls_return = "Y";
    } else if(ui->radioButton_2->isChecked()) {//手工指定服务费额
        ls_return = "N";
        ls_service = QString::number(ui->lineEdit->value());
        bool ok=false;
        ls_service.toFloat(&ok);
        if(ok == false) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QObject::tr("手动处理服务费") + "\n" +QObject::tr("指定服务费必须是数字"));
            return;
        }
        ls_return = ls_return + "&" + ls_service;
    } else if(ui->radioButton_3->isChecked()) {//手工指定服务费率
        ls_return = "R";
        ls_service = QString::number(ui->lineEdit_2->value());
        bool ok=false;
        ls_service.toInt(&ok);
        if(ok == false) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("手动处理服务费") + "\n" + QObject::tr("指定服务费必须是数字"));
            return;
        }
        ls_return = ls_return + "&" + ls_service;
    }

    is_return = ls_return;
    this->accept();
}

void w_scr_balance_service::on_pushButton_cancel_clicked()
{
    this->reject();
}
