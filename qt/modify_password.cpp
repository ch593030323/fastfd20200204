#include "modify_password.h"
#include "ui_modify_password.h"
#include "n_func.h"
#include "lds_query_hddpos.h"
#include <QSqlRecord>
#include "lds_messagebox.h"
#include "backheader.h"
#include <QSqlQueryModel>

modify_password::modify_password(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::modify_password)
{
    ui->setupUi(this);
    ui->lineEdit->setEnabled(false);
    ui->lineEdit->setText(lds::gs_operid);
    ui->lineEdit_2->setEchoMode(QLineEdit::Password);
    ui->lineEdit_3->setEchoMode(QLineEdit::Password);
    ui->lineEdit_4->setEchoMode(QLineEdit::Password);


}

modify_password::~modify_password()
{
    delete ui;
}

void modify_password::on_pushButton_5_clicked()
{
    this->close();
}

void modify_password::on_pushButton_4_clicked()
{
    QSqlQueryModel model;
    lds_query_hddpos  query;
    lds_query_hddpos::tran_saction();

    if(ui->lineEdit_2->text().trimmed().isEmpty() || ui->lineEdit_3->text().trimmed().isEmpty() || ui->lineEdit_4->text().trimmed().isEmpty())
    {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("内容填写不完整"));
        
        return;
    }

    if(ui->lineEdit->text() == "0000")
    {
        if(!n_func::f_cmp_syspwd(ui->lineEdit_2->text().trimmed()))//ui->lineEdit_2->text().trimmed() != n_func::f_get_sysparm("system_pass", "0000"))
        {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("旧密码错误"));

            return;
        }

        if(ui->lineEdit_3->text().trimmed() != ui->lineEdit_4->text().trimmed())
        {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("新密码与确认密码不一致"));

            return;
        }

        if(!(n_func::f_set_sysparm("system_pass", ui->lineEdit_4->text().trimmed(), QObject::tr("系统管理员密码"))).isEmpty()) {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
            return;
        }
    }
    else
    {
        model.setQuery(QString("select vch_password from cey_sys_operator where vch_operID = '%1' ").arg(ui->lineEdit->text()));
        if(ui->lineEdit_2->text().trimmed() != model.record(0).value(0).toString().trimmed())
        {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("旧密码错误"));

            return;
        }

        if(ui->lineEdit_3->text().trimmed() != ui->lineEdit_4->text().trimmed())
        {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("新密码与确认密码不一致"));

            return;
        }

        if(query.execUpdate("cey_sys_operator","vch_password", ui->lineEdit_4->text().trimmed(),qrtEqual("vch_operID", ui->lineEdit->text().trimmed())))
        {

        }
        else
        {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());

            return;
        }
    }
    lds_query_hddpos::com_mit();

    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("修改密码成功，请记住新密码"));

    this->close();
}
