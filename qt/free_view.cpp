#include "free_view.h"
#include "ui_free_view.h"
#include "lds_messagebox.h"
#include <QSqlRecord>
#include <QDateTime>
#include <QDebug>
#include "n_func.h"
#include "backheader.h"
#include <QtDebug>
#include <QTimer>
#include "n_func.h"
#include <QSqlQueryModel>
#include "lds.h"
#include "fexpandmain_model_sqltablemodel_data.h"


free_view::free_view(const QString & str, const QString &ch_paymodeno, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::free_view)
{
    ui->setupUi(this);
    this->ch_paymodeno = ch_paymodeno;
    btn_t_f = false;
    line2_num = 0;
    num = str;
    init_control();

    ui->lineEdit_3->setText(lds::gs_operid);
    ui->lineEdit_4->setText(lds::gs_password);

    if(ch_paymodeno != "ZZ") {
        ui->label_discount->hide();
        ui->spinBox_discount->hide();
    } else {
        ui->label_2->setText(QObject::tr("优惠"));
    }


    connect(ui->spinBox_discount, SIGNAL(valueChanged(int)),this,SLOT(tokeepamount()));
    connect(ui->lineEdit_amount, SIGNAL(valueChanged(double)),this,SLOT(tokeepamount()));


    QTimer::singleShot(0, this, SLOT(todelayfocus()));
}

free_view::~free_view()
{
    delete ui;
}

void free_view::init_control()
{
    ui->lineEdit->setEnabled(false);
    ui->lineEdit->setValue(num.toDouble());
    ui->lineEdit_amount->setRange(0, num.toDouble());
    ui->lineEdit_amount->setValue(num.toDouble());
    ui->spinBox_discount->setRange(0, 100);
    ui->spinBox_discount->setValue(0);
    if(public_sql::discount_off_mode())
        ui->spinBox_discount->enableValueMaxMinus();
    ui->lineEdit_4->setEchoMode(QLineEdit::Password);
}

void free_view::setinputenabled(bool f)
{
    ui->lineEdit_amount->setEnabled(f);
}

void free_view::on_pushButton_5_clicked()//取消
{
    btn_t_f = false;
    this->close();
}

void free_view::on_pushButton_ok_clicked()
{

    //先判断权限
    QSqlQueryModel model;
    QString ls_error;
    model.setQuery(QString("Select vch_operID,vch_password from cey_sys_operator where ch_flag = 'Y' "));
    if(ui->lineEdit_3->text().isEmpty())
    {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作员代码不能为空"));
        return;
    }

    if(!(ls_error = n_func::f_confirm_user_pwd(ui->lineEdit_3->text(), ui->lineEdit_4->text())).isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  ls_error);
        return;
    }

    if(ui->lineEdit_amount->value() > ui->lineEdit->value())
    {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作金额不能大于结算金额"));
        return;
    }

    if(!m.isEmpty()) {
        lds_query_hddpos  query;
        double num_amount;
        query.execSelect(QString("select num_amount  from t_m_curamount where vch_memberno = '%1' ")
                         .arg(m));
        query.next();
        num_amount = query.recordValue("num_amount").toDouble();

        if(1 == lds_messagebox::warning(this, MESSAGE_TITLE_VOID
                                       , QObject::tr("会员余额") + ":" +lds::septNumber(num_amount) + "\n" + QObject::tr("挂账金额") + ":" +lds::septNumber(ui->lineEdit_amount->value())
                                       , QObject::tr("确定"),QObject::tr("取消"))) {
            return;
        }
    }

    oper_id = ui->lineEdit_3->text().trimmed();
    line2_num = ui->lineEdit_amount->value();
    beizhu = ui->lineEdit_5->text();
    time = n_func::f_get_sysdatetime_str();
    btn_t_f = true;
    this->close();
}

void free_view::todelayfocus()
{
    ui->lineEdit_amount->setFocus();
    ui->lineEdit_amount->selectAll();
}

void free_view::tokeepamount()
{
    if(ui->lineEdit_amount == qApp->focusWidget()) {
        ui->spinBox_discount->setValue(100 - int(100*ui->lineEdit_amount->value()/ui->lineEdit->value()));
    }

    if(ui->spinBox_discount == qApp->focusWidget()) {
        ui->lineEdit_amount->setValue(ui->lineEdit->value()*0.01*(100 - ui->spinBox_discount->value()));
    }
}
