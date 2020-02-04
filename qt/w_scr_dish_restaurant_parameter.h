#ifndef W_SCR_DISH_RESTAURANT_PARAMETER_H
#define W_SCR_DISH_RESTAURANT_PARAMETER_H

#include <QDialog>

namespace Ui {
class w_scr_dish_restaurant_parameter;
}

class w_scr_dish_restaurant_parameter : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_restaurant_parameter(QWidget *parent = 0);
    ~w_scr_dish_restaurant_parameter();

    bool map_showChanged();
private slots:
    void tosave();
    void toexit();
private:
    Ui::w_scr_dish_restaurant_parameter *ui;
    QString old_map_show;
};

#endif // W_SCR_DISH_RESTAURANT_PARAMETER_H
