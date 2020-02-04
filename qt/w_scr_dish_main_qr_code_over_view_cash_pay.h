#ifndef W_SCR_DISH_MAIN_QR_CODE_OVER_VIEW_CASH_PAY_H
#define W_SCR_DISH_MAIN_QR_CODE_OVER_VIEW_CASH_PAY_H

#include <QDialog>

namespace Ui {
class w_scr_dish_main_qr_code_over_view_cash_pay;
}

class w_scr_dish_main_qr_code_over_view_cash_pay : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_main_qr_code_over_view_cash_pay(double num_cost, QWidget *parent = 0);
    ~w_scr_dish_main_qr_code_over_view_cash_pay();

    double get_num_money();
    double get_num_change();
    double get_num_cost();
    double get_num_cash();
    double get_num_round();


private slots:
    void took();
    void tocancel();

    void toNum();
    void updateChange();
protected:
    void keyPressEvent(QKeyEvent *e);
private:
    Ui::w_scr_dish_main_qr_code_over_view_cash_pay *ui;

    double num_cost;
};

#endif // W_SCR_DISH_MAIN_QR_CODE_OVER_VIEW_CASH_PAY_H
