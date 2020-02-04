#include "w_scr_dish_m_hang_dialog.h"
#include "ui_w_scr_dish_m_hang_dialog.h"
#include <QSqlRecord>
#include "w_m_member_list.h"
#include "lds_tableview.h"
#include "lds_messagebox.h"
#include "lds_query_hddpos.h"
#include "w_sys_manage_cloudsync.h"
#include "public_sql.h"
#include "n_func.h"
#include "lds.h"
#include "ui_w_m_member_dialog.h"

w_scr_dish_m_hang_Dialog::w_scr_dish_m_hang_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_m_hang_Dialog)
{
    ui->setupUi(this);
    //1
    outerdialog = new w_m_member_list;
    outerdialog->setToolbarShown(false);
    ui->verticalLayout_2->addWidget(outerdialog);
    //2
    connect(outerdialog->getTableview(), SIGNAL(selectChanged(int)),this,SLOT(updatem(int)));
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_readcard,SIGNAL(clicked()),this,SLOT(toreadcard()));
    connect(outerdialog, SIGNAL(finished(int)),this,SLOT(done(int)));
    connect(outerdialog->ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(took()));
    ui->label_member->clear();

    ui->pushButton_readcard->hide();
    ui->pushButton_ok->setFocus();
}

w_scr_dish_m_hang_Dialog::~w_scr_dish_m_hang_Dialog()
{
    delete ui;
}

void w_scr_dish_m_hang_Dialog::updatem(int row)
{
    lds_model_sqltablemodel *m = static_cast<lds_model_sqltablemodel *>(outerdialog->getTablemodel());
    lds_query_hddpos  query;
    QString vch_memberno = m->model_data(row, "vch_memberno").toString();
    QString ch_state = m->model_data(row, "ch_state").toString();
    QString ch_cardflag = m->model_data(row, "ch_cardflag").toString();
    QDateTime dt_limit = m->model_data(row, "dt_limit").toDateTime();
    float num_amount;
    query.execSelect(QString("select num_amount  from t_m_curamount where vch_memberno = '%1' ")
                     .arg(vch_memberno));
    query.next();
    num_amount = query.recordValue("num_amount").toFloat();
    updatestate(vch_memberno, "02", ch_state, ch_cardflag, num_amount, dt_limit);
}

void w_scr_dish_m_hang_Dialog::took()
{
    m = ui->label_member->text().trimmed();
    pno = ui->label_ch_paymodeno->text();
    if(m.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("所选会员为空"));
        return;
    }
    if(!ui->label_state->text().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, ui->label_state->text());
        return;
    }
    this->accept();
}

void w_scr_dish_m_hang_Dialog::tocancel()
{
    this->reject();
}

void w_scr_dish_m_hang_Dialog::toreadcard()
{
}

void w_scr_dish_m_hang_Dialog::updatestate(const QString &vch_memberno, const QString &ch_paymodeno, const QString &ch_state, const QString &ch_cardflag, float num_amount, const QDateTime &dt_limit)
{
    static bool isyun = false;
    if(isyun) {
        if(ch_paymodeno == "02") {
            if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("正在将云会员改为本地会员,是否继续"), QObject::tr("继续"), QObject::tr("取消"))) {
                outerdialog->getTableview()->clearSelection();
                return;
            }
        }
    }
    ui->label_state->clear();
    ui->label_num_remain->clear();
    //1
    if(ch_state != "1") {
        ui->label_state->setText(ui->label_state->text()+"/"+QObject::tr("非正常卡"));//ch_state
    }
    if(ch_cardflag != "Y") {
        ui->label_state->setText(ui->label_state->text()+"/"+QObject::tr("未发卡"));//ch_cardflag
    }
    if(dt_limit < n_func::f_get_sysdatetime()) {
        ui->label_state->setText(ui->label_state->text()+"/"+QObject::tr("超过有效期"));//ch_cardflag
    }
    if(!ui->label_state->text().isEmpty()) {
        ui->label_state->setText(QObject::tr("卡状态") + ":" +ui->label_state->text());
    }
    //2
    if(ui->label_state->text().isEmpty()) {
        ui->label_num_remain->setNum(num_amount);//num_amount
    }
    ui->label_num_remain->setValue(ui->label_num_remain->value());
    //3
    ui->label_member->setText(vch_memberno);
    ui->label_ch_paymodeno->setText(ch_paymodeno);
    if(ch_paymodeno == "10") {
        ui->label_member->setStyleSheet("color:green;");
        isyun = true;
    } else {
        ui->label_member->setStyleSheet("");
        isyun = false;
    }
}
