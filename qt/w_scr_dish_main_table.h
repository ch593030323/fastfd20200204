#ifndef W_SCR_DISH_MAIN_TABLE_H
#define W_SCR_DISH_MAIN_TABLE_H

#include <QDialog>

namespace Ui {
class w_scr_dish_main_table;
}

class w_scr_dish_main_table : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_main_table(QWidget *parent = 0);
    ~w_scr_dish_main_table();

private:
    Ui::w_scr_dish_main_table *ui;
};

#endif // W_SCR_DISH_MAIN_TABLE_H
