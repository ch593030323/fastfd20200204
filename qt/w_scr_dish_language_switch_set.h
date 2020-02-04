#ifndef W_SCR_DISH_LANGUAGE_SWITCH_SET_H
#define W_SCR_DISH_LANGUAGE_SWITCH_SET_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
class w_scr_dish_language_switch_set;
}

class w_scr_dish_language_switch_set : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_language_switch_set(QWidget *parent = 0);
    ~w_scr_dish_language_switch_set();

private slots:
    void took();
    void tocancel();
    void to_language_clear();
    void to_language_set(const QModelIndex &index);
    void to_language_set(const QString &key);

    void to_lookup();
private:
    Ui::w_scr_dish_language_switch_set *ui;
};

#endif // W_SCR_DISH_LANGUAGE_SWITCH_SET_H
