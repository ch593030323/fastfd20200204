#ifndef W_SCR_DISH_COOK_H
#define W_SCR_DISH_COOK_H

#include <QDialog>
#include <QModelIndex>
#include "vch_print_memodata.h"

namespace Ui {
class w_scr_dish_cook;
}
class ftableview_standmodel_sql_none;
class w_scr_dish_cook : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_cook(QWidget *parent = 0);
     w_scr_dish_cook(const QString &dishno, QWidget *parent = 0);
    ~w_scr_dish_cook();

    void intData();

    vch_print_memoData vch_print_memo;
    Ui::w_scr_dish_cook *ui;
private slots:
    void refresh(const QString &ch_dishno);
    void took();
    void tocancel();

    void tochangenum(const QModelIndex &index);
private:
    QString ch_dishno;
    ftableview_standmodel_sql_none *model;
};

#endif // W_SCR_DISH_COOK_H
