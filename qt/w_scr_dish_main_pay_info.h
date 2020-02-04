#ifndef W_SCR_DISH_MAIN_PAY_INFO_H
#define W_SCR_DISH_MAIN_PAY_INFO_H

#include <QFrame>
class Ui_fexpandmain_payover;

class w_scr_dish_main_pay_info : public QFrame
{
    Q_OBJECT
public:
    explicit w_scr_dish_main_pay_info(QWidget *parent = 0);

    ~w_scr_dish_main_pay_info();

    Ui_fexpandmain_payover *ui;

};

#endif // W_SCR_DISH_MAIN_PAY_INFO_H
