#ifndef W_SCR_DISH_MAIN_SELECT_RETURN_H
#define W_SCR_DISH_MAIN_SELECT_RETURN_H

#include <QDialog>
#include "fexpandmain_model_sqltablemodel_data.h"
#include "lds_model_sqlstandardmodel.h"
#include "w_scr_dish_main_separate.h"

namespace Ui {
class w_scr_dish_main_select_return;
}

class w_scr_dish_main_select_return : public w_scr_dish_main_separate
{
    Q_OBJECT

public:
    explicit w_scr_dish_main_select_return(const fexpandmain_model_sqltablemodel_data *tablemodel, QWidget *parent = 0);
};

#endif // W_SCR_DISH_MAIN_SELECT_RETURN_H
