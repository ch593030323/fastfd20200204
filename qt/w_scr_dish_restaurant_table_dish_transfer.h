#ifndef W_SCR_DISH_RESTAURANT_TABLE_DISH_TRANSFER_H
#define W_SCR_DISH_RESTAURANT_TABLE_DISH_TRANSFER_H

#include <QDialog>
class lds_model_sqltablemodel;
class lds_tableView;
namespace Ui {
class w_scr_dish_restaurant_table_dish_transfer;
}

class w_scr_dish_restaurant_table_dish_transfer : public QDialog
{
    Q_OBJECT

public:
public:
    explicit w_scr_dish_restaurant_table_dish_transfer(const QString &ch_billno, const QString &ch_tableno, QWidget *parent = 0);
    ~w_scr_dish_restaurant_table_dish_transfer();

private slots:
    void toselecttable();
    void tocurselectall();
    void tocurunselectall();

    void tosavekitchenout(bool f);
    void tosavekitchenprint(bool f);

    void took();
    void tocancel();

    void torefresh();
private:
    QString getTableNo(const QString &ch_billno);
    void create_model(QWidget *parent, lds_model_sqltablemodel *tablemodel, lds_tableView *tableview);
private:
    Ui::w_scr_dish_restaurant_table_dish_transfer *ui;
    lds_model_sqltablemodel *tablemodel_cur;
    lds_model_sqltablemodel *tablemodel_des;
    QString cur_ch_billno;
    QString des_ch_billno;
};

#endif // W_SCR_DISH_RESTAURANT_TABLE_DISH_TRANSFER_H
