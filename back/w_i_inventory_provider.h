#ifndef W_I_INVENTORY_PROVIDER_H
#define W_I_INVENTORY_PROVIDER_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"

namespace Ui {
class w_i_inventory_provider;
}

class w_i_inventory_provider : public QDialog
{
    Q_OBJECT

public:
    explicit w_i_inventory_provider(QWidget *parent = 0);
    ~w_i_inventory_provider();

    QVariant curData(const QString &field_name);
private slots:
    void toadd();
    void tomodify();
    void todel();
    void toexit();
    void toarrears();

    void toselect();
    void took();
protected:
    Ui::w_i_inventory_provider *ui;
    lds_model_sqltablemodel *tablemodel;
};


class w_i_inventory_provider_costumer : public w_i_inventory_provider
{
    Q_OBJECT
public:
    explicit w_i_inventory_provider_costumer(QWidget *parent = 0);
};

class w_i_inventory_provider_costumer_select : public w_i_inventory_provider_costumer
{
    Q_OBJECT
public:
    explicit w_i_inventory_provider_costumer_select(QWidget *parent = 0);
};

#endif // W_I_INVENTORY_PROVIDER_H
