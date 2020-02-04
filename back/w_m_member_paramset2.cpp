#include "w_m_member_paramset2.h"
#include "ui_w_m_member_paramset2.h"
#include "ftableview_delegate.h"
#include "w_m_member.h"
#include "n_func.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"

w_m_member_paramset2::w_m_member_paramset2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_m_member_paramset2)
{
    ui->setupUi(this);
    //1
    backlist_model = new ftableview_standmodel_sql_none;
    backlist_model->set_row_column(2, 1);

    backlist_model->model_data_set(0, 0,"anyvalue", Qt::UserRole+1);backlist_model->model_data_set(0, 0, QObject::tr("会员设置"), Qt::UserRole);
    backlist_model->model_data_set(1, 0,"anyvalue", Qt::UserRole+1);backlist_model->model_data_set(1, 0, QObject::tr("卡设置"), Qt::UserRole);

    ftableview_delegate *d = new ftableview_delegate(ui->tableView_list);
    d->keepConnect(ui->tableView_list);
    d->indexisEmptyDrawGrad = true;
    ui->tableView_list->setProperty("outer_stylesheet", "tableView_type");

    ui->tableView_list->setModel(backlist_model);
    ui->tableView_list->setItemDelegate(d);
    ui->tableView_list->setHeaderResizeMode(Qt::Horizontal, QHeaderView::Stretch);

    ui->tableView_list->verticalHeader()->setDefaultSectionSize(50);
    //
    ui->tableView_list->setCurrentIndex(backlist_model->index(0, 0));
    ui->stackedWidget->setCurrentIndex(0);
    //
    ui->comboBox_vip_card_type->addItemsByModel(&w_m_member::get_CARD_TYPE_MODE());

    //
    opData();

    connect(ui->tableView_list, SIGNAL(clicked(QModelIndex)), this, SLOT(toUpdateSection(QModelIndex)));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->checkBox_save,SIGNAL(clicked(bool)),this,SLOT(toisrecharge(bool)));

    connect(ui->comboBox_vip_card_type,SIGNAL(currentIndexChanged(int)),this,SLOT(inputmemberno(int)));
    connect(ui->pushButton_m_clearAll, SIGNAL(clicked()),this,SLOT(tomclearall()));
    connect(ui->pushButton_ic_pwd_def, SIGNAL(clicked()),this,SLOT(toicpwddef()));
}

w_m_member_paramset2::~w_m_member_paramset2()
{
    delete ui;
}

void w_m_member_paramset2::toUpdateSection(const QModelIndex &index)
{
    ui->stackedWidget->setCurrentIndex(index.row());
}

void w_m_member_paramset2::toisrecharge(bool flag)
{
    if(flag) {
    } else {
        lds_query_hddpos  query;
        query.execSelect("select count(0) from t_m_member_type where ch_attribute='2' ");
        query.next();
        if(query.recordValue(0).toLongLong() > 0) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("会员类型中已经有充值卡会员类型"));
            ui->checkBox_save->setChecked(true);
            return;
        }
    }
}

void w_m_member_paramset2::took()
{
    if(false == opData(true)) {
        return;
    }
    this->accept();
}

void w_m_member_paramset2::tocancel()
{
    this->reject();
}

void w_m_member_paramset2::inputmemberno(int index)
{
    ui->checkBox_input_memberno->setVisible(index == 1);//磁卡
    ui->label_icpwd->setVisible(index == 2);
    ui->widget_ic->setVisible(index == 2);
    ui->widget_ic->setVisible(index == 2);//IC卡
}

void w_m_member_paramset2::tomclearall()
{
    lds_query_hddpos  query;
    QString errstring;
    lds_query_hddpos::tran_saction();
    if(0 == lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("该操作将会先清空所有的会员信息, 是否继续"), QObject::tr("继续"), QObject::tr("取消"))) {
        lds_dialog_input inputdialog(this);
        inputdialog.ui->label->setText(QObject::tr("密码") + ":");
        inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
        inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
        inputdialog.setWindowTitle(QObject::tr("即将配置初始化"));
        if(QDialog::Accepted==transparentCenterDialog(&inputdialog).exec()) {
            errstring = QObject::tr("密码错误");
            if(inputdialog.ui->lineEdit->text()==n_func::f_get_godpassword()) {
                errstring = QObject::tr("数据库错误");
                if(query.execDelete("t_m_gift")) {
                    if(query.execDelete("t_m_curamount")) {
                        if(query.execDelete("t_m_member")) {
                            if(query.execDelete("t_m_member_type")) {
                                if(query.execDelete("t_m_deposit")) {
                                    if(query.execDelete("t_m_pay")) {
                                        if(query.execDelete("t_m_point")) {
                                            if(query.execDelete("t_m_point_gift")) {
                                                lds_query_hddpos::com_mit();
                                                lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
                                                query.execSelect("select *from t_m_member where ch_cardflag = 'Y' ");
                                                ui->comboBox_vip_card_type->setEnabled(true);
                                                if(query.next()) {
                                                    ui->comboBox_vip_card_type->setEnabled(false);
                                                }
                                                return;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    lds_query_hddpos::roll_back();
    if(!errstring.isEmpty())
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
}

void w_m_member_paramset2::toicpwddef()
{
    ui->lineEdit_vip_ic_password->setText("FFFFFF");
}

bool w_m_member_paramset2::opData(bool issave)
{
    if(issave) {
        bool ok = true;
        QString errstring;
        if(ok) ok = n_func::f_m_set_parameter("member_ATT_mode", QString() + "1" +
                                  (ui->checkBox_save->isChecked() ? "2" : ""), QObject::tr("会员需要使用类型"), errstring );
        if(ok) ok = n_func::f_m_set_parameter("member_bank_print", ui->checkBox_member_bank->isChecked() ? "1" : "0", QObject::tr("会员作废的时候打印账单"), errstring);
        if(ok) ok = n_func::f_m_set_parameter("m_deposit_negative", ui->checkBox_m_deposit_negative->isChecked() ? "1" : "0", QObject::tr("充值与实收金额必须相等"), errstring);
        if(ok) ok = n_func::f_m_set_parameter("m_card_paydiscount", ui->checkBox_m_card_paydiscount->isChecked() ? "1" : "0", QObject::tr("充值卡会员付款时自动折扣"), errstring);

        if(ok) ok = n_func::f_m_set_parameter("m_max_deposit", ui->lineEdit_max_deposit->text(), QObject::tr("最大充值金额"), errstring);
        if(ok) ok = n_func::f_m_set_parameter("discount_view", ui->checkBox_discount_view->isChecked() ? "1" : "0", QObject::tr("折扣时显示会员折扣详情"), errstring);
        if(ok) ok = n_func::f_m_set_parameter("deposit_pay_all", ui->checkBox_deposit_pay_all->isChecked() ? "1" : "0", QObject::tr("充值卡付款需全单满付"), errstring);
        if(ok) ok = n_func::f_m_set_parameter("local_member_select", ui->checkBox_local_select->isChecked() ? "1" : "0", QObject::tr("启动本地会员营业时查卡"), errstring);
        if(ok) ok = n_func::f_m_set_parameter("m_fetch_base", ui->checkBox_m_fetch_base->isChecked() ? "1" : "0", QObject::tr("可取保本金额"), errstring);

        if(ok) ok = n_func::f_m_set_parameter("m_member_pay", ui->checkBox_member_pay->isChecked() ? "1" : "0", QObject::tr("充值会员付款允许负数"), errstring);
        if(ok) ok = n_func::f_m_set_parameter("vip_phone_verify", ui->checkBox_vip_phone_verify->isChecked() ? "1" : "0", QObject::tr("会员付款短信验证"), errstring);

//
        if(ok) ok = n_func::f_m_set_parameter("vip_ic_password", ui->lineEdit_vip_ic_password->text(), QObject::tr("IC卡读写密码"), errstring);
        if(ok) ok = n_func::f_m_set_parameter("vip_card_type", ui->comboBox_vip_card_type->comusrdata(), QObject::tr("卡类型"), errstring);
        if(ok) ok = n_func::f_m_set_parameter("input_memberno", ui->checkBox_input_memberno->isChecked() ? "1" : "0", QObject::tr("手动输入会员卡号"), errstring);

        if(false == ok)
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return ok;
    }
    ui->checkBox_discount->setChecked(n_func::f_m_get_parameter(this,"member_ATT_mode", "123").contains("1"));
    ui->checkBox_save->setChecked(n_func::f_m_get_parameter(this,"member_ATT_mode", "123").contains("2"));
    ui->checkBox_member_bank->setChecked(n_func::f_m_get_parameter(this,"member_bank_print","0") == "1");
    ui->checkBox_m_deposit_negative->setChecked(n_func::f_m_get_parameter(this,"m_deposit_negative","0") == "1");
    ui->checkBox_m_card_paydiscount->setChecked(n_func::f_m_get_parameter(this,"m_card_paydiscount","0") == "1");

    ui->lineEdit_max_deposit->setText(n_func::f_m_get_parameter(this,"m_max_deposit","30000"));
    ui->checkBox_discount_view->setChecked(n_func::f_m_get_parameter(this,"discount_view","0") == "1");
    ui->checkBox_deposit_pay_all->setChecked(n_func::f_m_get_parameter(this,"deposit_pay_all","0") == "1");
    ui->checkBox_local_select->setChecked(n_func::f_m_get_parameter(this,"local_member_select","1") == "1");
    ui->checkBox_m_fetch_base->setChecked(n_func::f_m_get_parameter(this,"m_fetch_base","0") == "1");

    ui->checkBox_member_pay->setChecked(n_func::f_m_get_parameter(this,"m_member_pay","0") == "1");
    ui->checkBox_vip_phone_verify->setChecked(n_func::f_m_get_parameter(this, "vip_phone_verify", "0") == "1");
    //
    ui->lineEdit_vip_ic_password->setText(n_func::f_m_get_parameter(this,"vip_ic_password","FFFFFF"));
    ui->comboBox_vip_card_type->setcomusrdata(n_func::f_m_get_parameter(this,"vip_card_type", "无会员卡"));
    ui->checkBox_input_memberno->setChecked(n_func::f_m_get_parameter(this, "input_memberno", "0") == "1");

    // 若是已经有发卡会员，卡类型无法修改
    ui->comboBox_vip_card_type->setEnabled(false == lds_query_hddpos::selectHasNext("select *from t_m_member where ch_cardflag = 'Y' "));
    toisrecharge(ui->checkBox_save->isChecked());
    inputmemberno(ui->comboBox_vip_card_type->currentIndex());
    return true;
}
