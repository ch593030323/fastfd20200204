#ifndef W_SCR_DISH_MAIN_TABLE_PAY_H
#define W_SCR_DISH_MAIN_TABLE_PAY_H

#include <QDialog>
#include "w_scr_dish_main.h"
#include "w_scr_dish_pay_widget.h"

namespace Ui {
class w_scr_dish_main_table_pay;
}

class w_scr_dish_main_table_pay : public w_scr_dish_main
{
    Q_OBJECT

public:
    explicit w_scr_dish_main_table_pay(
            fexpandmain_model_sqltablemodel_data *tablemodel,
            QWidget *parent = 0,
            const QList<int> &rowListAsc = QList<int>(),
            w_scr_dish_pay_widget::operatModes flags = w_scr_dish_pay_widget::RESTAURANT_PAY
            );
    ~w_scr_dish_main_table_pay();

private:
    Ui::w_scr_dish_main_table_pay *ui;
};

#endif // W_SCR_DISH_MAIN_TABLE_PAY_H
