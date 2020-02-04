#ifndef W_SCR_DISH_RESTAURANT_TABLE_COPY_H
#define W_SCR_DISH_RESTAURANT_TABLE_COPY_H

#include <QDialog>
#include <QModelIndex>
class w_scr_dish_restaurant_div_pop_only_table;
class ftableview_standmodel_sql_table;

namespace Ui {
class w_scr_dish_restaurant_table_copy;
}

class w_scr_dish_restaurant_table_copy : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_restaurant_table_copy(const QString &ch_tableno, QWidget *parent);
    ~w_scr_dish_restaurant_table_copy();
private slots:
    void totabletype(int index);
    void tocurindex_checklist(const QString &checklist);

    void tocancel();
    void took();
    void toSetgrayTo(const QModelIndex &index_to);

    void tosavekitchenout(bool f);
    void tosavekitchenprint(bool f);
    void toupdatedestination(const QModelIndex &index);
private:
    Ui::w_scr_dish_restaurant_table_copy *ui;
    w_scr_dish_restaurant_div_pop_only_table *_9Box;
    ftableview_standmodel_sql_table *modeltable;
};

#endif // W_SCR_DISH_RESTAURANT_TABLE_COPY_H
