#ifndef W_SCR_DISH_DISH_QUIT_H
#define W_SCR_DISH_DISH_QUIT_H

#include <QDialog>
#include "fexpandmain_2.h"
#include "ftableview_standmodel_sql_none.h"

class fexpandmain_model_sqltablemodel;

namespace Ui {
class w_scr_dish_dish_quit;
}

class w_scr_dish_dish_quit : public QDialog
{
    Q_OBJECT
public:
    explicit w_scr_dish_dish_quit(QWidget *parent);
    ~w_scr_dish_dish_quit();
    void initData(int row, const fexpandmain_model_sqltablemodel_data *frommodel, const QString &ch_billno);
private slots:
    void took();
    void tocancel();
    void toallquit();
private:
    Ui::w_scr_dish_dish_quit *ui;
    const fexpandmain_model_sqltablemodel_data *tablemodel;
    int const_row;
    QString _ch_billno;
    ftableview_standmodel_sql_none *cookmodel;

};

#endif // W_SCR_DISH_DISH_QUIT_H
