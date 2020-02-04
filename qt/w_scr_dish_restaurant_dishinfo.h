#ifndef W_SCR_DISH_RESTAURANT_DISHINFO_H
#define W_SCR_DISH_RESTAURANT_DISHINFO_H

#include <QDialog>
#include "w_rpt_report_frame.h"

namespace Ui {
class w_scr_dish_restaurant_dishinfo;
}

class w_scr_dish_restaurant_dishinfo : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_restaurant_dishinfo(QWidget *parent = 0);
    ~w_scr_dish_restaurant_dishinfo();

private:
    void deleteItemDelegate(const QString &fieldname);
    void comboBox_additem();
private slots:
    void torefresh();
    void toexit();
private:
    Ui::w_scr_dish_restaurant_dishinfo *ui;
    report_querymodel *querymodel;
};

#endif // W_SCR_DISH_RESTAURANT_DISHINFO_H
