#ifndef W_SCR_DISH_RESTAURANT_DISH_DIALOG_H
#define W_SCR_DISH_RESTAURANT_DISH_DIALOG_H

#include <QDialog>

namespace Ui {
class w_scr_dish_restaurant_dish_dialog;
}

class w_scr_dish_restaurant_dish_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_restaurant_dish_dialog(QWidget *parent);
    ~w_scr_dish_restaurant_dish_dialog();
public slots:
    void torefresh();
private slots:
    void on_pushButton_pay_clicked();

private:
    Ui::w_scr_dish_restaurant_dish_dialog *ui;
    QString cur_type;
    QString cur_subtype;
};

#endif // W_SCR_DISH_RESTAURANT_DISH_DIALOG_H
