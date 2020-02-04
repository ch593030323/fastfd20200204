#ifndef W_SCR_DISH_NUM_COOK_DIALOG_H
#define W_SCR_DISH_NUM_COOK_DIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include "ftableview_standmodel_sql_none.h"
#include <QTextEdit>
#include "vch_print_memodata.h"
#include "lds_const_var.h"

class frontViewDialog_toolbtn;
class fexpandmain_model_sqltablemodel;
class fexpandmain_model_sqltablemodel_data;
class lds_model_sqltablemodel_d;
namespace Ui {
class w_scr_dish_num_cook_Dialog;
}

class w_scr_dish_num_cook_Dialog : public QDialog
{
    Q_OBJECT
public:
    static void static_to_present(QWidget *parent,
                                  fexpandmain_model_sqltablemodel  *tablemodel,
                                  int row,
                                  float num_num);
public:
    w_scr_dish_num_cook_Dialog(const fexpandmain_model_sqltablemodel_data *model,
                               int row, QWidget *parent);

    ~w_scr_dish_num_cook_Dialog();
    double get_cur_dish_num();
    const fexpandmain_model_sqltablemodel  *model();
private:
    void init_ui();
    void refresh(const fexpandmain_model_sqltablemodel_data *model, int row);
    void refresh_sub(int row);//_frommodel
    void enable_cook(bool f) const;

    //cook
    void cook_refresh();
    QMap<QString, double > cur_cook_data();

private slots:
    void toallcook();
    void toclearcook();
    void took();
    void tocancel();
    void topresent();
    void totakeweight();
    void tocookset();
    void tomore();

    void tochangenum(const QModelIndex &index);

    void toswtichTab();
    void tochangevch_operID();

private:
    void saveCurdata();
    QString operName(const QString &vch_operid);
private:
    vch_print_memoData print_memo(const QMap<QString, double > &map);
private:
    Ui::w_scr_dish_num_cook_Dialog *ui;
    fexpandmain_model_sqltablemodel  *tablemodel;
    const fexpandmain_model_sqltablemodel_data *frommodel;
    lds_const_int frommodel_row;
    QTextEdit *label_info;

    ftableview_standmodel_sql_none *cook_model;
    /////////
//protected:
//    bool eventFilter(QObject *o, QEvent *e);
};

#endif // W_SCR_DISH_NUM_COOK_DIALOG_H
