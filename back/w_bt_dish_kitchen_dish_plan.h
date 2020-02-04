#ifndef W_BT_DISH_KITCHEN_DISH_PLAN_H
#define W_BT_DISH_KITCHEN_DISH_PLAN_H

#include <QDialog>
#include <QDataWidgetMapper>
#include <QComboBox>
#include "lds_tableview.h"
#include "lds_model_sqltablemodel.h"
#include "lds_model_sqltablemodel_d.h"

namespace Ui {
class w_bt_dish_kitchen_dish_plan;
}

class w_bt_dish_kitchen_dish_plan : public QDialog
{
    Q_OBJECT

public:
    explicit w_bt_dish_kitchen_dish_plan(QWidget *parent = 0);
    ~w_bt_dish_kitchen_dish_plan();

    lds_tableView *tableview();
public slots:
    void torefresh();
private slots:
    void tonew();
    void tochange();
    void todel();
    void toexit();
private:
    Ui::w_bt_dish_kitchen_dish_plan *ui;
public:
    lds_model_sqltablemodel*tablemodel;
};

class Ui_w_bt_dish_kitchen_dish_plan_new;
class w_bt_dish_kitchen_dish_plan_new_virtual: public QDialog {
    Q_OBJECT

public:
    explicit w_bt_dish_kitchen_dish_plan_new_virtual(w_bt_dish_kitchen_dish_plan *parent);
    virtual ~w_bt_dish_kitchen_dish_plan_new_virtual();
protected:
    bool saveCheck();
private slots:
    void toprintset();
    void toch_check_state_enable(int index);
protected:
    Ui_w_bt_dish_kitchen_dish_plan_new *ui;
    w_bt_dish_kitchen_dish_plan *_parent;
};

class w_bt_dish_kitchen_dish_plan_new: public w_bt_dish_kitchen_dish_plan_new_virtual {
    Q_OBJECT

public:
    explicit w_bt_dish_kitchen_dish_plan_new(w_bt_dish_kitchen_dish_plan *parent);
    virtual ~w_bt_dish_kitchen_dish_plan_new();
private slots:
    void tosave();
    void toexit();
};

class w_bt_dish_kitchen_dish_plan_change: public w_bt_dish_kitchen_dish_plan_new_virtual {
    Q_OBJECT

public:
    explicit w_bt_dish_kitchen_dish_plan_change(w_bt_dish_kitchen_dish_plan *parent);
    virtual ~w_bt_dish_kitchen_dish_plan_change();
private slots:
    bool tosave();
    void toexit();
    void toup();
    void todown();
private:
    bool saveData();
private:
    QDataWidgetMapper *mapper;
};
#endif // W_BT_DISH_KITCHEN_DISH_PLAN_H
