#ifndef W_SCR_DISH_RESTAURANT_TABLE_SELECT_H
#define W_SCR_DISH_RESTAURANT_TABLE_SELECT_H

#include <QDialog>
#include "ftableview_standmodel_sql_table.h"
class Ui_w_scr_dish_restaurant_table_select;
class w_scr_dish_restaurant_table_select : public QDialog
{
    Q_OBJECT
public:
    explicit w_scr_dish_restaurant_table_select(const QString &unch_tableno, QWidget *parent = 0);
    ~w_scr_dish_restaurant_table_select();

    QString get_ch_tableno();
    QString get_ch_divno();
    QString tabledesc();
    QString get_ch_billno();

    void refresh();
signals:

public slots:
    void totabletype(int  index);

    void took();
    void tocancel();
private:
    Ui_w_scr_dish_restaurant_table_select *ui;
    ftableview_standmodel_sql_table *modeltable;
    QMap<QString/*ch_tableno*/, QString /*checklist*/> ch_tableno_check_map;
};

#endif // W_SCR_DISH_RESTAURANT_TABLE_SELECT_H
