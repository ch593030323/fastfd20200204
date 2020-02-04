#ifndef W_BT_DISH_PAYMODE_LIANTUOFU_H
#define W_BT_DISH_PAYMODE_LIANTUOFU_H

#include <QDialog>

namespace Ui {
class w_bt_dish_paymode_liantuofu;
}

class w_bt_dish_paymode_liantuofu : public QDialog
{
    Q_OBJECT

public:
    explicit w_bt_dish_paymode_liantuofu(QWidget *parent = 0);
    ~w_bt_dish_paymode_liantuofu();

private slots:
    void took();
    void tocancel();
    void todemo();
    void totest();
private:
    Ui::w_bt_dish_paymode_liantuofu *ui;
};

#endif // W_BT_DISH_PAYMODE_LIANTUOFU_H
