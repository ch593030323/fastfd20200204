#ifndef W_SCR_DISH_PAY_SET_OGG_H
#define W_SCR_DISH_PAY_SET_OGG_H

#include <QDialog>

namespace Ui {
class w_scr_dish_pay_set_ogg;
}

class w_scr_dish_pay_set_ogg : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_pay_set_ogg(QWidget *parent = 0);
    ~w_scr_dish_pay_set_ogg();

private slots:
    void tostart();
    void tostop();
    void tocancel();
    void totest();
private:
    Ui::w_scr_dish_pay_set_ogg *ui;
};

#endif // W_SCR_DISH_PAY_SET_OGG_H
