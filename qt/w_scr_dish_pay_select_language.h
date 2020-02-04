#ifndef W_SCR_DISH_PAY_SELECT_LANGUAGE_H
#define W_SCR_DISH_PAY_SELECT_LANGUAGE_H

#include <QDialog>
#include <QModelIndex>
class ftableview_standmodel_sql_none;
namespace Ui {
class w_scr_dish_pay_select_language;
}

class w_scr_dish_pay_select_language : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_pay_select_language(QWidget *parent = 0);
    ~w_scr_dish_pay_select_language();

private slots:
    void took();
    void tocancel();

    void to_save_pay_select_language(bool f);
private:
    Ui::w_scr_dish_pay_select_language *ui;
    ftableview_standmodel_sql_none *model ;
};

#endif // W_SCR_DISH_PAY_SELECT_LANGUAGE_H
