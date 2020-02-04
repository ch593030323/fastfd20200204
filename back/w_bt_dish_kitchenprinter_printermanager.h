#ifndef W_BT_DISH_KITCHENPRINTER_PRINTERMANAGER_H
#define W_BT_DISH_KITCHENPRINTER_PRINTERMANAGER_H
#include <QDialog>
#include<QSqlRelationalTableModel>
#include <QQueue>
#include <QPair>
#include "public_mainprinter.h"
#include <QDateTime>
#include "lds_tableview_delegate_check.h"
#include "lds_model_sqltablemodel.h"

class Printer_POS;
class Printer_POS_NET;
class Printer_POS_VIDPID;
class lds_tableView;
class lds_tableView;
class Ui_w_bt_dish_Kitchenprinter_printermanager;
class w_bt_dish_Kitchenprinter_printermanager : public QDialog
{
    Q_OBJECT
public:
    explicit w_bt_dish_Kitchenprinter_printermanager(QWidget *parent);
private slots:
    void tonew();
    void tonewkitchen();
    void tochange();
    void todel();
    void torefresh();
    void toexit();
    void toprinttest();
    void datachanged(const QModelIndex &, const QModelIndex &);
    void toprint_select(int index);
private:
    Ui_w_bt_dish_Kitchenprinter_printermanager *ui;
    lds_model_sqltablemodel *tablemodel;
    bool dchange;
    QMap<QString, QVariant> ch_areano_keyValueMap;
};

#endif // W_BT_DISH_KITCHENPRINTER_PRINTERMANAGER_H
