#ifndef W_SYS_MANAGE_OUTER_PAY_SET_TAX_TYPE_H
#define W_SYS_MANAGE_OUTER_PAY_SET_TAX_TYPE_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"

namespace Ui {
class w_sys_manage_outer_pay_set_tax_type;
}

class w_sys_manage_outer_pay_set_tax_type : public QDialog
{
    Q_OBJECT

public:
    explicit w_sys_manage_outer_pay_set_tax_type(QWidget *parent = 0);
    ~w_sys_manage_outer_pay_set_tax_type();

    static  QString taxDesc();
    static QString int_rate_name(const lds_model_sqltablemodel *m, int index);
    static double int_rate_worked(const lds_model_sqltablemodel *m, int index);


private slots:
    void took();
    void tocancel();

    void tochange_taxname1();
    void tochange_taxname2();
    void tochange_taxname3();
private:
    void updateTaxName(const QString &sys_parameter_key, const QString &defValue);
    void resetTaxName();
private:
    Ui::w_sys_manage_outer_pay_set_tax_type *ui;
};

#endif // W_SYS_MANAGE_OUTER_PAY_SET_TAX_TYPE_H
