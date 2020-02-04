#ifndef W_SCR_DISH_PAY_SET_H
#define W_SCR_DISH_PAY_SET_H

#include <QDialog>
#include "w_sys_manage_outer_settings.h"

class w_scr_dish_pay_set : public w_sys_manage_outer_settings
{
    Q_OBJECT

public:
    explicit w_scr_dish_pay_set(QWidget *parent = 0);
    virtual ~w_scr_dish_pay_set();
private slots:
    virtual void took();
};

#endif // W_SCR_DISH_PAY_SET_H
