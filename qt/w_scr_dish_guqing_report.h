#ifndef W_SCR_DISH_GUQING_REPORT_H
#define W_SCR_DISH_GUQING_REPORT_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"

namespace Ui {
class w_scr_dish_guqing_report;
}

class w_scr_dish_guqing_report : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_guqing_report(QWidget *parent = 0);
    ~w_scr_dish_guqing_report();

private slots:
    void tosave();
    void toexit();
    void tosave_showall(bool f);
    void on_lineEdit_filter_textChanged(const QString &arg1);

    void torefresh();
private:
    bool saveData();
private:
    Ui::w_scr_dish_guqing_report *ui;
    
    lds_model_sqltablemodel *tablemodel;
};

#endif // W_SCR_DISH_GUQING_REPORT_H
