#ifndef W_SCR_DISH_PAY_KITCHEN_QUIT_REQUEST_H
#define W_SCR_DISH_PAY_KITCHEN_QUIT_REQUEST_H

#include <QDialog>

namespace Ui {
class w_scr_dish_pay_kitchen_quit_request;
}

class w_scr_dish_pay_kitchen_quit_request : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_pay_kitchen_quit_request(QWidget *parent = 0);
    ~w_scr_dish_pay_kitchen_quit_request();

private:
    Ui::w_scr_dish_pay_kitchen_quit_request *ui;
};

#endif // W_SCR_DISH_PAY_KITCHEN_QUIT_REQUEST_H
