#ifndef W_SCR_DISH_MAIN_QUIT_H
#define W_SCR_DISH_MAIN_QUIT_H

#include <QDialog>
#include "fexpandmain_model_sqltablemodel_data.h"
#include "lds_model_sqlstandardmodel.h"

namespace Ui {
class w_scr_dish_main_quit;
}

class w_scr_dish_main_quit : public QDialog
{
    Q_OBJECT

public:
    struct quitData{
        quitData() : num_dish(0), int_quit_flow_id(0) {}

        vch_print_memoData d;
        double num_dish;
        qlonglong int_quit_flow_id;//确定退单后有效
    };
    enum rowType{
        rowDish,
        rowCook
    };

public:
    explicit w_scr_dish_main_quit(const fexpandmain_model_sqltablemodel_data *tablemodel, QWidget *parent = 0);
    ~w_scr_dish_main_quit();

    void row_data_map2List(QList<num_backData> &list_quit, QMap<int, qrtVariantPairList> &list_insert);
    void row_data_map_print();
private:
    bool row_data_map_update(qlonglong int_flowid, qlonglong &int_quit_flow_id);
    bool dishquit_1_only(QString &errstring, const num_backData &d);
    QMap<int, quitData> row_data_map;
private slots:
    void took();
    void tocancel();

    void toClearAll();
    void toSelectAll();

    void toQuitBill();
    void toquit_bill_kitchen_view();

    void toUpdateRow(const QModelIndex &index);
    void toUpdateTableNum(double num_dish);
private:
    void refresh();
    bool commit();
private:
    bool make_row_data_map();
private:
    Ui::w_scr_dish_main_quit *ui;
    lds_model_sqlstandardmodel *standardmodel;
    const fexpandmain_model_sqltablemodel_data *tablemodel;
};

#endif // W_SCR_DISH_MAIN_QUIT_H
