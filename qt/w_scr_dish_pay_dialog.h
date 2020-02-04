#ifndef W_SCR_DISH_PAY_DIALOG_H
#define W_SCR_DISH_PAY_DIALOG_H

#include <QDialog>
#include "lds_roundeddialog_rect.h"
#include "w_scr_dish_pay_widget.h"

class fexpandmain_model_sqltablemodel_data;

namespace Ui {
class w_scr_dish_pay_dialog;
}

class w_scr_dish_pay_dialog : public lds_roundeddialog_rect
{
    Q_OBJECT

public:
    explicit w_scr_dish_pay_dialog(w_scr_dish_pay_widget::operatModes m, QWidget *parent,
                                   const fexpandmain_model_sqltablemodel_data *tablemodel);
    ~w_scr_dish_pay_dialog();

    w_scr_dish_pay_widget *pay_widget();
private:
    Ui::w_scr_dish_pay_dialog *ui;
};

#endif // W_SCR_DISH_PAY_DIALOG_H
