#ifndef W_SCR_DISH_MAIN_BAR_CANCEL_H
#define W_SCR_DISH_MAIN_BAR_CANCEL_H

#include <QDialog>
#include "w_rpt_report_frame.h"
class Ui_w_scr_dish_main_bar_cancel_wine;

namespace Ui {
class w_scr_dish_main_bar_cancel;
}

class w_scr_dish_main_bar_cancel : public QDialog
{
    Q_OBJECT
public:
    static bool cey_bt_table_bar_dish_existed(const QString &ch_tableno, int int_div_id);
    static bool cey_bt_table_bar_clear_transaction(const QString &ch_tableno, int int_div_id, QString *errstring = 0);
    static void cey_bt_table_bar_print(QWidget *parent, const QString &ch_tableno, int int_div_id);
public:
    explicit w_scr_dish_main_bar_cancel(const QString &ch_tableno, int int_div_id, QWidget *parent = 0);
    virtual ~w_scr_dish_main_bar_cancel();

private slots:
    void tocancel();
    void took();
    void toprint();

    void torefresh();
protected:
    Ui::w_scr_dish_main_bar_cancel *ui;
    QString ch_tableno;
    int int_div_id;
    lds_model_sqltablemodel *tablemodel;
};

class w_scr_dish_main_bar_cancel_print : public w_scr_dish_main_bar_cancel{
    Q_OBJECT

public:w_scr_dish_main_bar_cancel_print(const QString &ch_tableno, int int_div_id, QWidget *parent = 0);
    virtual ~w_scr_dish_main_bar_cancel_print();

};
#endif // W_SCR_DISH_MAIN_BAR_CANCEL_H
