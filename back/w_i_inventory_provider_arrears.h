#ifndef W_I_INVENTORY_PROVIDER_ARREARS_H
#define W_I_INVENTORY_PROVIDER_ARREARS_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"

namespace Ui {
class w_i_inventory_provider_arrears;
}

class w_i_inventory_provider_arrears : public QDialog
{
    Q_OBJECT

public:
    explicit w_i_inventory_provider_arrears(const QString &ch_providerNo, QWidget *parent = 0);
    ~w_i_inventory_provider_arrears();

private slots:
    void topay();
    void toexit();
    void toselect();
private:
    Ui::w_i_inventory_provider_arrears *ui;
    lds_model_sqltablemodel *tablemodel;
    const QString ch_providerNo;
};

#endif // W_I_INVENTORY_PROVIDER_ARREARS_H
