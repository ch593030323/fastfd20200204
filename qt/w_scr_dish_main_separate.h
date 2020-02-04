#ifndef W_SCR_DISH_MAIN_SEPARATE_H
#define W_SCR_DISH_MAIN_SEPARATE_H

#include <QDialog>
#include "fexpandmain_model_sqltablemodel_data.h"
#include "lds_model_sqlstandardmodel.h"

namespace Ui {
class w_scr_dish_main_separate;
}

class w_scr_dish_main_separate : public QDialog
{
    Q_OBJECT

public:
    struct separateData{
        separateData() : num_dish(0) {}

        vch_print_memoData d;
        double num_dish;
    };
    enum rowType{
        rowDish,
        rowCook
    };

public:
    explicit w_scr_dish_main_separate(const fexpandmain_model_sqltablemodel_data *tablemodel, QWidget *parent = 0);
    ~w_scr_dish_main_separate();

    QMap<int, separateData> row_data_map;
private slots:
    void took();
    void tocancel();

    void toClearAll();
    void toSelectAll();

    void toUpdateRow(const QModelIndex &index);
    void toUpdateTableNum(double num_dish);
private:
    Ui::w_scr_dish_main_separate *ui;
    lds_model_sqlstandardmodel *standardmodel;
    const fexpandmain_model_sqltablemodel_data *tablemodel;
};

#endif // W_SCR_DISH_MAIN_SEPARATE_H
