#ifndef W_BT_DISH_KITCHEN_VIEW_PLAN_H
#define W_BT_DISH_KITCHEN_VIEW_PLAN_H

#include <QDialog>
#include <QDataWidgetMapper>
#include "lds_model_sqltablemodel.h"

namespace Ui {
class w_bt_dish_kitchen_view_plan;
}

class w_bt_dish_kitchen_view_plan : public QDialog
{
    Q_OBJECT

public:
    explicit w_bt_dish_kitchen_view_plan(QWidget *parent = 0);
    ~w_bt_dish_kitchen_view_plan();

public slots:
    void torefresh();
    void torefresh_machine();
private slots:
    void took();
    void tocancel();
    void tomapperCommit(QWidget*editor);
    void tomapperSelect(int row);
    void toogg_new_order();
    void toadjust_2ndscreen(int index);

private:
    void updatevch_plan_id_service_list(int row);
    bool saveData();
    void mapperAdd(QWidget *widget, const QByteArray &propertyName = "");
private:
    Ui::w_bt_dish_kitchen_view_plan *ui;
    lds_model_sqltablemodel *tablemodel;
    QDataWidgetMapper *mapper;
};

#endif // W_BT_DISH_KITCHEN_VIEW_PLAN_H
