#ifndef W_SCR_DISH_MAIN_VIEW_800X600_H
#define W_SCR_DISH_MAIN_VIEW_800X600_H

#include <QDialog>
#include "w_scr_dish_main.h"

namespace Ui {
class w_scr_dish_main_view_800x600;
}

class w_scr_dish_main_view_800x600 : public w_scr_dish_main
{
    Q_OBJECT

public:
    explicit w_scr_dish_main_view_800x600(QWidget *parent = 0);
    ~w_scr_dish_main_view_800x600();
protected slots:
     void menuhide();
     void updateTakebillState();
     void updateTakebillDesc();
     void retranslateView();
     void loadData();
     void orderdishRowChanged(const QModelIndex &, const QModelIndex &);
     void updatememberchecked();
private:
    Ui::w_scr_dish_main_view_800x600 *ui;
};

#endif // W_SCR_DISH_MAIN_VIEW_800X600_H
