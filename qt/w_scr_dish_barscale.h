#ifndef W_SCR_DISH_BARSCALE_H
#define W_SCR_DISH_BARSCALE_H

#include <QDialog>
#include "barscale_main_dialog.h"

class w_scr_dish_barscale : public barscale_main_dialog
{
    Q_OBJECT
public:
    explicit w_scr_dish_barscale(QWidget *parent);

protected slots:
    virtual void togoodadd();//添加

};

#endif // W_SCR_DISH_BARSCALE_H
