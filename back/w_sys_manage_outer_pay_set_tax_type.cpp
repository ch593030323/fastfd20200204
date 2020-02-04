#include "w_sys_manage_outer_pay_set_tax_type.h"
#include "ui_w_sys_manage_outer_pay_set_tax_type.h"
#include "n_func.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "ui_lds_dialog_input_dialog.h"
#include "transparentcenterdialog.h"

w_sys_manage_outer_pay_set_tax_type::w_sys_manage_outer_pay_set_tax_type(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_outer_pay_set_tax_type)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("税率类型"));

    ui->comboBox_type->resetTableView(30);
    ui->comboBox_type->addItem(tr("不启用"), -1);
    ui->comboBox_type->addItem(tr("单一"), 1);
    ui->comboBox_type->addItem(tr("三选一"), 2);
    ui->comboBox_type->addItem(tr("累加"), 3);

    ui->comboBox_type->setcomusrdata(n_func::f_get_sysparm("tax_type", 0));
    ui->checkBox_tax1->setChecked(n_func::f_get_sysparm("tax_check1") == "1");
    ui->checkBox_tax2->setChecked(n_func::f_get_sysparm("tax_check2") == "1");
    ui->checkBox_tax3->setChecked(n_func::f_get_sysparm("tax_check3") == "1");

    //
    resetTaxName();

    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok,SIGNAL(clicked()),this, SLOT(took()));
    connect(ui->pushButton_change_taxname1, SIGNAL(clicked()), this, SLOT(tochange_taxname1()));
    connect(ui->pushButton_change_taxname2, SIGNAL(clicked()), this, SLOT(tochange_taxname2()));
    connect(ui->pushButton_change_taxname3, SIGNAL(clicked()), this, SLOT(tochange_taxname3()));
}

w_sys_manage_outer_pay_set_tax_type::~w_sys_manage_outer_pay_set_tax_type()
{
    delete ui;
}

QString w_sys_manage_outer_pay_set_tax_type::taxDesc()
{
    int tax_type = n_func::f_get_sysparm("tax_type", 0).toInt();

    QString tax_check1 = n_func::f_get_sysparm("tax_check1");
    QString tax_check2 = n_func::f_get_sysparm("tax_check2");
    QString tax_check3 = n_func::f_get_sysparm("tax_check3");

    QString tax_check_names;
    if(tax_check1 == "1")  tax_check_names += n_func::f_get_sysparm("tax_name1", QObject::tr("税率") + "1") + " +";
    if(tax_check2 == "1")  tax_check_names += n_func::f_get_sysparm("tax_name2", QObject::tr("税率") + "2") + " +";
    if(tax_check3 == "1")  tax_check_names += n_func::f_get_sysparm("tax_name3", QObject::tr("税率") + "3") + " +";
    tax_check_names.chop(1);

    if(tax_type == -1) {
        return QObject::tr("不启用");
    }
    if(tax_type == 1) {
        return QObject::tr("单一") + " /" + tax_check_names;
    }
    if(tax_type == 2) {
        return QObject::tr("三选一") + " /" + QObject::tr("默认") + " :" + tax_check_names;
    }
    if(tax_type == 3) {
        return QObject::tr("累加")+ " /" + tax_check_names;
    }
    return "";
}

QString w_sys_manage_outer_pay_set_tax_type::int_rate_name(const lds_model_sqltablemodel *m, int index)
{
    QString tax_name1 = m->tablename_kvmap_value("cey_sys_parameter", "tax_name1", "vch_parameter", "vch_value", "", QObject::tr("税率") + "1").toString();
    QString tax_name2 = m->tablename_kvmap_value("cey_sys_parameter", "tax_name2", "vch_parameter", "vch_value", "", QObject::tr("税率") + "2").toString();
    QString tax_name3 = m->tablename_kvmap_value("cey_sys_parameter", "tax_name3", "vch_parameter", "vch_value", "", QObject::tr("税率") + "3").toString();

    if(1 == index)
        return tax_name1;
    if(2 == index)
        return tax_name2;
    if(3 == index)
        return tax_name3;
    return "";
}

double w_sys_manage_outer_pay_set_tax_type::int_rate_worked(const lds_model_sqltablemodel *m, int index)
{
    int tax_check1 = m->tablename_kvmap_value("cey_sys_parameter", "tax_check1", "vch_parameter", "vch_value").toInt();
    int tax_check2 = m->tablename_kvmap_value("cey_sys_parameter", "tax_check2", "vch_parameter", "vch_value").toInt();
    int tax_check3 = m->tablename_kvmap_value("cey_sys_parameter", "tax_check3", "vch_parameter", "vch_value").toInt();

    double pay_fixed_tax_value = m->tablename_kvmap_value("cey_sys_parameter", "pay_fixed_tax_value", "vch_parameter", "vch_value").toDouble();
    double pay_fixed_tax_2_value = m->tablename_kvmap_value("cey_sys_parameter", "pay_fixed_tax_2_value", "vch_parameter", "vch_value").toDouble();
    double pay_fixed_tax_3_value = m->tablename_kvmap_value("cey_sys_parameter", "pay_fixed_tax_3_value", "vch_parameter", "vch_value").toDouble();

    int tax_type = m->tablename_kvmap_value("cey_sys_parameter", "tax_type", "vch_parameter", "vch_value", "", 0).toInt();
    if(0 == tax_type)
        return 0;

    if(1 == index&& 1 == tax_check1)
        return pay_fixed_tax_value;
    if(2 == index&& 1 == tax_check2)
        return pay_fixed_tax_2_value;
    if(3 == index&& 1 == tax_check3)
        return pay_fixed_tax_3_value;

    return 0;
}

void w_sys_manage_outer_pay_set_tax_type::took()
{
    QString tax_check1 = ui->checkBox_tax1->isChecked() ? "1" : "0";
    QString tax_check2 = ui->checkBox_tax2->isChecked() ? "1" : "0";
    QString tax_check3 = ui->checkBox_tax3->isChecked() ? "1" : "0";
    int tax_type = ui->comboBox_type->curusrdata().toInt();

    if(tax_type == 1) {//单一
        if(1 != tax_check1.toInt() + tax_check2.toInt() + tax_check3.toInt()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("请选择一种税率，作为当前税率"));
            return;
        }
    }
    if(tax_type == 2) {//三选一
        if(1 != tax_check1.toInt() + tax_check2.toInt() + tax_check3.toInt()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("请选择一种税率，作为默认税率"));
            return;
        }
    }
    if(tax_type == 3) {//累加
        if(0 == tax_check1.toInt() + tax_check2.toInt() + tax_check3.toInt()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有税率被选中"));
            return;
        }
    }

    n_func::f_set_sysparm("tax_type", tax_type);
    n_func::f_set_sysparm("tax_check1", tax_check1);
    n_func::f_set_sysparm("tax_check2", tax_check2);
    n_func::f_set_sysparm("tax_check3", tax_check3);

    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
    this->accept();
}

void w_sys_manage_outer_pay_set_tax_type::tocancel()
{
    this->reject();
}

void w_sys_manage_outer_pay_set_tax_type::tochange_taxname1()
{
    updateTaxName("tax_name1", ui->checkBox_tax1->text());
}

void w_sys_manage_outer_pay_set_tax_type::tochange_taxname2()
{
    updateTaxName("tax_name2", ui->checkBox_tax2->text());
}

void w_sys_manage_outer_pay_set_tax_type::tochange_taxname3()
{
    updateTaxName("tax_name3", ui->checkBox_tax3->text());
}

void w_sys_manage_outer_pay_set_tax_type::updateTaxName(const QString &sys_parameter_key, const QString &defValue)
{
    lds_dialog_input dialog(this);
    dialog.setWindowTitle(QObject::tr("税率名称"));
    dialog.hideKey();
    dialog.ui->label->setText("");
    dialog.ui->lineEdit->setText(defValue);
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        QString name = dialog.ui->lineEdit->text().trimmed();
        if(name.isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("税率名称为空"));
            return;
        }
        n_func::f_set_sysparm(sys_parameter_key, name);
    }
    resetTaxName();
}

void w_sys_manage_outer_pay_set_tax_type::resetTaxName()
{
    QString tax_name1 = n_func::f_get_sysparm("tax_name1", QObject::tr("税率") + "1");
    QString tax_name2 = n_func::f_get_sysparm("tax_name2", QObject::tr("税率") + "2");
    QString tax_name3 = n_func::f_get_sysparm("tax_name3", QObject::tr("税率") + "3");

    ui->checkBox_tax1->setText(tax_name1);
    ui->checkBox_tax2->setText(tax_name2);
    ui->checkBox_tax3->setText(tax_name3);
}
