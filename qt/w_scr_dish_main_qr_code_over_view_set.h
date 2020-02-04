#ifndef W_SCR_DISH_MAIN_QR_CODE_OVER_VIEW_SET_H
#define W_SCR_DISH_MAIN_QR_CODE_OVER_VIEW_SET_H

#include <QDialog>

namespace Ui {
class w_scr_dish_main_qr_code_over_view_set;
}

class w_scr_dish_main_qr_code_over_view_set : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_main_qr_code_over_view_set(QWidget *parent = 0);
    ~w_scr_dish_main_qr_code_over_view_set();

private slots:
    void tocancel();
    void took();

    void toupdate(bool checked);
private:
    Ui::w_scr_dish_main_qr_code_over_view_set *ui;
};

#endif // W_SCR_DISH_MAIN_QR_CODE_OVER_VIEW_SET_H
