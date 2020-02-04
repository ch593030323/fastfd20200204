#ifndef W_SCR_DISH_MAIN_VIEW_SCALE_H
#define W_SCR_DISH_MAIN_VIEW_SCALE_H

#include <QDialog>
#include "w_scr_dish_main.h"
#include "electroniccountprice_resolve.h"

namespace Ui {
class w_scr_dish_main_view_scale;
}

class w_scr_dish_main_view_scale : public w_scr_dish_main
{
    Q_OBJECT

public:
    explicit w_scr_dish_main_view_scale(QWidget *parent = 0);
    ~w_scr_dish_main_view_scale();

protected slots:
    void menuhide();
    void updateTakebillState();
    void updateTakebillDesc();
    void retranslateView();
    void loadData();
    void orderdishRowChanged(const QModelIndex &current, const QModelIndex &previous) ;
    void updatememberchecked();
    void updateTotal(double total);
    bool getWeightValue(int row, double &w);
    bool dishAppend(const QString &ch_dishno, const vch_print_memoData &vch_print_memo, double num_price, int int_discount, const bool time_promotion);

private slots:
    void toqupi();
    void tozhiling();
    void tostable();

    void quipistate(bool f);
    void lingweistate(bool f);
    void stablestate(bool scale_stable);

    void updategetweight();

    void updategetTare(const QString &w);

    void toCheckStable();

    void to_cloud_inventory_in();
    void to_cloud_download();
    void to_cloud_upload();
    void to_cloud_inventory_out();
    void to_cloud_inventory_request();
    void to_cloud_flow_upload();
private:
    void setScaleTableOk(bool enable);
private:
    Ui::w_scr_dish_main_view_scale *ui;
    int check_weight_count;
    const int stable_weight_count;
    QString stable_before_weight;

    Printer_POS_SERIAL scale_printer;
};

#endif // W_SCR_DISH_MAIN_VIEW_SCALE_H
