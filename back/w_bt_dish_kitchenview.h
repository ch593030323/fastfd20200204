#ifndef W_BT_DISH_KITCHENVIEW_H
#define W_BT_DISH_KITCHENVIEW_H

#include <QDialog>

namespace Ui {
class w_bt_dish_kitchenview;
}

class w_bt_dish_kitchenview : public QDialog
{
    Q_OBJECT

public:
    explicit w_bt_dish_kitchenview(QWidget *parent = 0);
    ~w_bt_dish_kitchenview();

private:
    Ui::w_bt_dish_kitchenview *ui;
};

#endif // W_BT_DISH_KITCHENVIEW_H
