#ifndef W_SCR_DISH_RESTAURANT_TABLE_CMB_H
#define W_SCR_DISH_RESTAURANT_TABLE_CMB_H

#include <QDialog>
#include <QModelIndex>
class w_scr_dish_restaurant_div_pop_only_table;
class ftableview_standmodel_sql_table;

namespace Ui {
class w_scr_dish_restaurant_table_cmb;
}

class w_scr_dish_restaurant_table_cmb : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_restaurant_table_cmb(const QString &ch_tableno, QWidget *parent);
    ~w_scr_dish_restaurant_table_cmb();

private slots:
    void totabletype(int index);
    void tocurindex_checklist(const QString &checklist);

    void tocancel();
    void took();
    void toSetgrayFrom(const QModelIndex &index_to);
    void tosavestate(bool f);

    void tosavekitchenout(bool f);
    void tosavekitchenprint(bool f);
    void toupdatedestination(const QModelIndex &index);
private:
    Ui::w_scr_dish_restaurant_table_cmb *ui;
    w_scr_dish_restaurant_div_pop_only_table *_9Box;
    ftableview_standmodel_sql_table *modeltable;
    QMap<QString, QString> ch_billno_desc_map;


    struct cmbData{
        QString ch_billno;
        QString ch_tableno;
        int sqr;
    };
};

#endif // W_SCR_DISH_RESTAURANT_TABLE_CMB_H
