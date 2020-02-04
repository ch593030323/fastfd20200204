#ifndef W_SCR_DISH_RESTAURANT_TABLE_TRANSFER_H
#define W_SCR_DISH_RESTAURANT_TABLE_TRANSFER_H

#include <QDialog>
#include <QModelIndex>
class w_scr_dish_restaurant_div_pop_only_table;
class ftableview_standmodel_sql_table;

namespace Ui {
class w_scr_dish_restaurant_table_transfer;
}

class w_scr_dish_restaurant_table_transfer : public QDialog
{
    Q_OBJECT
public:
    explicit w_scr_dish_restaurant_table_transfer(const QString &ch_tableno, QWidget *parent);
    ~w_scr_dish_restaurant_table_transfer();

private slots:
    void totabletype(int index);
    void tosavekitchenout(bool f);
    void tosavekitchenprint(bool f);
    void toupdatedestination(const QModelIndex &index);

    void tocancel();
    void took();
private:
    Ui::w_scr_dish_restaurant_table_transfer *ui;
    w_scr_dish_restaurant_div_pop_only_table *_9Box;
    ftableview_standmodel_sql_table *modeltable;
    QString ch_tableno_from;
};

#endif // W_SCR_DISH_RESTAURANT_TABLE_TRANSFER_H
