#ifndef W_SCR_DISH_CUICAI_CHOOSE_H
#define W_SCR_DISH_CUICAI_CHOOSE_H

#include <QDialog>
#include "ftableview_standmodel_sql_none.h"

class Ui_w_scr_dish_special_cook;
class w_scr_dish_cuicai_choose : public QDialog
{
    Q_OBJECT
public:
    explicit w_scr_dish_cuicai_choose(QWidget *parent);
    ~w_scr_dish_cuicai_choose();

    void reloaddata(const QMap<QString, QString> &map);
    QStringList hasSelected();
    Ui_w_scr_dish_special_cook *ui;
signals:

public slots:
    void took();
    void toexit();
private:
    QStringList typenolist;
    ftableview_standmodel_sql_none *cook_model;
};

#endif // W_SCR_DISH_CUICAI_CHOOSE_H
