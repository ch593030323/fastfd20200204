#ifndef W_SYS_MANAGE_OUTER_PAY_SET_LOCALE_WIZARD_H
#define W_SYS_MANAGE_OUTER_PAY_SET_LOCALE_WIZARD_H

#include <QDialog>
#include <QModelIndex>

#define LOCALE_RECEIPT_PAY_FILE_PATH(country)  lds::sysconf->value("w_sys_manage_outer_pay_set_locale_wizard/" + country + "_print_receipt_pay", lds::localdataLocalePath + "/" + country + "/print_receipt_pay.txt").toString()
#define LOCALE_RECEIPT_PAY_SQL_PATH(country) QString("locale_rept_" + country)
#define LOCALE_RECEIPT_REPRINT_SQL_PATH(country) QString("locale_rept_" + country)
#define LOCALE_RECEIPT_DIR QString("userdata/settings/locale")

class ftableview_standmodel_sql_none;
class public_pushbuton_number;

namespace Ui {
class w_sys_manage_outer_pay_set_locale_wizard;
}

class w_sys_manage_outer_pay_set_locale_wizard : public QDialog
{
    Q_OBJECT
public:
    static bool bp_country_print_img(const QString &country);
    static bool bp_country_print_img();
public:
    explicit w_sys_manage_outer_pay_set_locale_wizard(QWidget *parent = 0);
    ~w_sys_manage_outer_pay_set_locale_wizard();

private slots:
    void took();
    void tocancel();

    void totax_type();
    void to_receipt();
    void to_receipt(const QString &country, const QByteArray &byte);

    void to_set_tax_1_name();
    void to_set_tax_2_name();
    void to_set_tax_3_name();

    void to_set_tax_1_value();
    void to_set_tax_2_value();
    void to_set_tax_3_value();

    void to_set_service_value();

    void to_set_print();
    void to_update_setting(const QModelIndex &index);

    void toChooseTab(bool checked);
    void to_save_tip();
    void toreceipt_txt_edit();

private:
    void updateTaxName(const QString &sys_parameter_key, const QString &defValue);
    void tax_settax(public_pushbuton_number *b, const QString &field_name, const QString &pay_fixed_tax_name);
    void tip_setvalue(public_pushbuton_number *b, const QString &tip_sql_name);
    void reTranslate();
protected:
    void paintEvent(QPaintEvent *e);
private:
    Ui::w_sys_manage_outer_pay_set_locale_wizard *ui;
    ftableview_standmodel_sql_none *none_model;

};

#endif // W_SYS_MANAGE_OUTER_PAY_SET_LOCALE_WIZARD_H
