#ifndef W_SCR_DISH_SELECT_OPERID_H
#define W_SCR_DISH_SELECT_OPERID_H

#include <QDialog>
#include "ftableview_standmodel_sql_none.h"

class Ui_w_scr_dish_special_cook;
class w_scr_dish_select_operid : public QDialog
{
    Q_OBJECT
public:
    explicit w_scr_dish_select_operid(const QString &curOperId, QWidget *parent);
    ~w_scr_dish_select_operid();
    QString selectOperid();
private slots:
    void took();
    void toexit();
private:
    QStringList hasSelected();
    QStringList typenolist;
    Ui_w_scr_dish_special_cook *ui;
    ftableview_standmodel_sql_none *cook_model;
};

#endif // W_SCR_DISH_SELECT_OPERID_H
