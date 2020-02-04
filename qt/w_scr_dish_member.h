#ifndef W_SCR_DISH_MEMBER_H
#define W_SCR_DISH_MEMBER_H

#include <QDialog>
#include <QStandardItemModel>
#include "n_func.h"
#include "w_scr_dish_pay_widget.h"
#include <QTimer>

namespace Ui {
class w_scr_dish_member;
}

class w_scr_dish_member : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_member(w_scr_dish_pay_widget *parent = 0);
    ~w_scr_dish_member();
    n_func::READCARDINFO cardinfo;

    void always_disable_num_pay();
private slots:
    void took();
    void tocancel();
    void toread();
    void tomore();

    void to_get_phone_code();
    void to_update_verify_state(const QString &code);
    void verify_phone_code_check();

    void topwdreset();
private:
    void show_phone_verify(bool isshow);
public:
    Ui::w_scr_dish_member *ui;
private:
    w_scr_dish_pay_widget *_parent;
    bool isfirst;
    lds_bool_default_false b_always_disable_num_pay;
    QString verify_rand_code;
    int verify_phone_code_60s;
};

#endif // W_SCR_DISH_MEMBER_H
