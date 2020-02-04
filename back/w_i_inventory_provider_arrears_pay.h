#ifndef W_I_INVENTORY_PROVIDER_ARREARS_PAY_H
#define W_I_INVENTORY_PROVIDER_ARREARS_PAY_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"

namespace Ui {
class w_i_inventory_provider_arrears_pay;
}

class w_i_inventory_provider_arrears_pay : public QDialog
{
    Q_OBJECT

public:
    explicit w_i_inventory_provider_arrears_pay(const lds_model_sqltablemodel *tablemodel, int tablemodel_row, QWidget *parent = 0);
    ~w_i_inventory_provider_arrears_pay();

private slots:
    void tocancel();
    void topay();
private:
    Ui::w_i_inventory_provider_arrears_pay *ui;
    const lds_model_sqltablemodel *tablemodel;
    const int tablemodel_row;
    int flag;
};

#endif // W_I_INVENTORY_PROVIDER_ARREARS_PAY_H
