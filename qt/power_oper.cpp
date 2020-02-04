#include "power_oper.h"
#include "ui_power_oper.h"
#include "lds_messagebox.h"
#include <QDebug>
#include "backheader.h"
#include <QSqlRecord>
#include <QSqlQueryModel>

power_oper::power_oper(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::power_oper)
{
    ui->setupUi(this);
    btn_t_f = false;
    ui->lineEdit->setFocus();
    ui->lineEdit_2->setEchoMode(QLineEdit::Password);
}

power_oper::~power_oper()
{
    delete ui;
}

void power_oper::on_pushButton_clicked()//0
{
    if(ui->lineEdit->hasFocus())
    {
        ui->lineEdit->insert("0");
    }
    if(ui->lineEdit_2->hasFocus())
    {
        ui->lineEdit_2->insert("0");
    }
}

void power_oper::on_pushButton_2_clicked()//1
{
    if(ui->lineEdit->hasFocus())
    {
        ui->lineEdit->insert("1");
    }
    if(ui->lineEdit_2->hasFocus())
    {
        ui->lineEdit_2->insert("1");
    }
}

void power_oper::on_pushButton_3_clicked()//2
{
    if(ui->lineEdit->hasFocus())
    {
        ui->lineEdit->insert("2");
    }
    if(ui->lineEdit_2->hasFocus())
    {
        ui->lineEdit_2->insert("2");
    }
}

void power_oper::on_pushButton_4_clicked()//3
{
    if(ui->lineEdit->hasFocus())
    {
        ui->lineEdit->insert("3");
    }
    if(ui->lineEdit_2->hasFocus())
    {
        ui->lineEdit_2->insert("3");
    }
}

void power_oper::on_pushButton_8_clicked()//4
{
    if(ui->lineEdit->hasFocus())
    {
        ui->lineEdit->insert("4");
    }
    if(ui->lineEdit_2->hasFocus())
    {
        ui->lineEdit_2->insert("4");
    }
}

void power_oper::on_pushButton_6_clicked()//5
{
    if(ui->lineEdit->hasFocus())
    {
        ui->lineEdit->insert("5");
    }
    if(ui->lineEdit_2->hasFocus())
    {
        ui->lineEdit_2->insert("5");
    }
}

void power_oper::on_pushButton_5_clicked()//6
{
    if(ui->lineEdit->hasFocus())
    {
        ui->lineEdit->insert("6");
    }
    if(ui->lineEdit_2->hasFocus())
    {
        ui->lineEdit_2->insert("6");
    }
}

void power_oper::on_pushButton_7_clicked()//7
{
    if(ui->lineEdit->hasFocus())
    {
        ui->lineEdit->insert("7");
    }
    if(ui->lineEdit_2->hasFocus())
    {
        ui->lineEdit_2->insert("7");
    }
}

void power_oper::on_pushButton_12_clicked()//8
{
    if(ui->lineEdit->hasFocus())
    {
        ui->lineEdit->insert("8");
    }
    if(ui->lineEdit_2->hasFocus())
    {
        ui->lineEdit_2->insert("8");
    }
}

void power_oper::on_pushButton_10_clicked()//9
{
    if(ui->lineEdit->hasFocus())
    {
        ui->lineEdit->insert("9");
    }
    if(ui->lineEdit_2->hasFocus())
    {
        ui->lineEdit_2->insert("9");
    }
}

void power_oper::on_pushButton_15_clicked()//d
{
    if(ui->lineEdit->hasFocus())
    {
        ui->lineEdit->backspace();
    }
    if(ui->lineEdit_2->hasFocus())
    {
        ui->lineEdit_2->backspace();
    }
}

void power_oper::on_pushButton_16_clicked()//t
{
    QSqlQueryModel model;
    model.setQuery(QString("Select vch_operID,vch_password from cey_sys_operator where ch_flag = 'Y' "));
    if(ui->lineEdit->text().isEmpty())
    {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作员代码不能为空"));
        
        return;
    }

    if(ui->lineEdit->text() != "0000")
    {
        int k = 0;
        for(int i = 0; i < model.rowCount(); i++)
        {
            if(model.record(i).value(0).toString() == ui->lineEdit->text().trimmed())
            {
                k++;
                if(ui->lineEdit_2->text().trimmed() != model.record(i).value(1).toString())
                {
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作密码错误"));
                    
                    return;
                }
            }
        }

        if(k == 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作员代码不存在"));
            
            return;
        }
    }  else {
        if(ui->lineEdit_2->text().trimmed() != "0000")
        {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作密码错误"));
            
            return;
        }
    }

    operid = ui->lineEdit->text().trimmed();
    btn_t_f = true;
    this->close();
}

void power_oper::on_pushButton_17_clicked()//f
{
    btn_t_f = false;
    this->close();
}
