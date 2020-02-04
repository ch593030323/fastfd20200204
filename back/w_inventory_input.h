#ifndef W_INVENTORY_INPUT_H
#define W_INVENTORY_INPUT_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"

namespace Ui {
class w_inventory_input;
}

class w_inventory_input : public QDialog
{
    Q_OBJECT
public:
    static bool detail_control_transaction(const QString &ch_state, int master_row, const lds_model_sqltablemodel *tablemodel_master, const lds_model_sqltablemodel *tablemodel_detail, QString *errstring = 0);
public:
    explicit w_inventory_input(const QString &ch_sheetType, QWidget *parent = 0);
    ~w_inventory_input();


private slots:
    void torefresh();
    void to_update_tableview_detail(int row);
    void toexit();
    void toaudit();
    void tobad();
    void tochange();
    void todel();
    void tonew();
    void toexport();

private:
    void detail_control(const QString &ch_state);//Y N O
    QString key_name(const QString &tablename, const QString &key_value, const QString &sql_key_name, const QString &sql_value_name);
private:
    Ui::w_inventory_input *ui;

    QString ch_sheetType;
    lds_model_sqltablemodel *tablemodel_master;
    lds_model_sqltablemodel *tablemodel_detail;
};

#endif // W_INVENTORY_INPUT_H
