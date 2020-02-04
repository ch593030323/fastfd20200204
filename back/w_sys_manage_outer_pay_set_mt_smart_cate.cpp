#include "w_sys_manage_outer_pay_set_mt_smart_cate.h"
#include "ui_w_sys_manage_outer_pay_set_mt_smart_cate.h"
#include "n_func.h"
#include "lds_messagebox.h"

w_sys_manage_outer_pay_set_mt_smart_cate::w_sys_manage_outer_pay_set_mt_smart_cate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_outer_pay_set_mt_smart_cate)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("智慧餐厅"));
    ui->comboBox_businessId->resetTableView(30);
    ui->comboBox_businessId->addItem(tr("团购"), 1);
    ui->comboBox_businessId->addItem(tr("闪惠"), 3);

    ui->lineEdit_appAuthToken->setText(n_func::f_get_sysparm("mt_smart_cate_appAuthToken"));
    ui->lineEdit_developerId->setText(n_func::f_get_sysparm("mt_smart_cate_developerId"));
    ui->lineEdit_ePoiId->setText(n_func::f_get_sysparm("mt_smart_cate_ePoiId"));
    ui->lineEdit_signKey->setText(n_func::f_get_sysparm("mt_smart_cate_signKey"));
    ui->comboBox_businessId->setcomusrdata(n_func::f_get_sysparm("mt_smart_cate_businessId"));

    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this, SLOT(took()));
    connect(ui->pushButton_test, SIGNAL(clicked()),this, SLOT(totest()));
    connect(ui->pushButton_demo, SIGNAL(clicked()),this, SLOT(todemo()));
}

w_sys_manage_outer_pay_set_mt_smart_cate::~w_sys_manage_outer_pay_set_mt_smart_cate()
{
    delete ui;
}

void w_sys_manage_outer_pay_set_mt_smart_cate::took()
{
    QString errstring;
    lds_query_hddpos::tran_saction();

    if(false == n_func::f_set_sysparm("mt_smart_cate_appAuthToken", ui->lineEdit_appAuthToken->text().trimmed(), "", errstring)) goto rollback;
    if(false == n_func::f_set_sysparm("mt_smart_cate_developerId", ui->lineEdit_developerId->text().trimmed(), "", errstring)) goto rollback;
    if(false == n_func::f_set_sysparm("mt_smart_cate_ePoiId", ui->lineEdit_ePoiId->text().trimmed(), "", errstring)) goto rollback;
    if(false == n_func::f_set_sysparm("mt_smart_cate_signKey", ui->lineEdit_signKey->text().trimmed(), "", errstring)) goto rollback;
    if(false == n_func::f_set_sysparm("mt_smart_cate_businessId", ui->comboBox_businessId->curusrdata(), "", errstring)) goto rollback;

    lds_query_hddpos::com_mit();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
    this->accept();
    return;
rollback:
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
}

void w_sys_manage_outer_pay_set_mt_smart_cate::tocancel()
{
    this->reject();
}

void w_sys_manage_outer_pay_set_mt_smart_cate::totest()
{

}

void w_sys_manage_outer_pay_set_mt_smart_cate::todemo()
{
    ui->lineEdit_signKey->setText("2sacty3dpls8majp");
    ui->lineEdit_ePoiId->setText("3334");
    ui->lineEdit_appAuthToken->setText("f8ddd868905c0b2adecdf193beca6eea90cd274a02fb7267b6c27e8ca46b2be3a8edad4eb81fecff1b6fc67caf90086c");
    ui->lineEdit_developerId->setText("104713");
    ui->comboBox_businessId->setcomusrdata(1);
}
