#ifndef W_BT_DISH_PAYMODE_CCB_H
#define W_BT_DISH_PAYMODE_CCB_H

#include <QDialog>

namespace Ui {
class w_bt_dish_paymode_CCB;
}

class w_bt_dish_paymode_CCB : public QDialog
{
    Q_OBJECT

public:
    explicit w_bt_dish_paymode_CCB(QWidget *parent = 0);
    ~w_bt_dish_paymode_CCB();

public slots:
    void todefault();
    void took();
    void tocancel();
private:
    Ui::w_bt_dish_paymode_CCB *ui;
};

#endif // W_BT_DISH_PAYMODE_CCB_H
