#ifndef W_SCR_DISH_PREORDER_DIALOG_H
#define W_SCR_DISH_PREORDER_DIALOG_H

#include <QDialog>

namespace Ui {
class w_scr_dish_preorder_Dialog;
}

class w_scr_dish_preorder_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_preorder_Dialog(QWidget *parent);
    ~w_scr_dish_preorder_Dialog();

private:
    Ui::w_scr_dish_preorder_Dialog *ui;
};

#endif // W_SCR_DISH_PREORDER_DIALOG_H
