#ifndef W_ST_GOOD_INPUT_SELECT_DISH_H
#define W_ST_GOOD_INPUT_SELECT_DISH_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"

namespace Ui {
class w_st_good_input_select_dish;
}

class w_st_good_input_select_dish : public QDialog
{
    Q_OBJECT

public:
    explicit w_st_good_input_select_dish(QWidget *parent = 0);
    ~w_st_good_input_select_dish();

    QList<int> ret_row_list;
    QVariant get_tablemodel_value(int row, const QString &valuename);

    void filter_by_itemflag();//库存管理
    void filter_by_material();//原料
    void filter_by_st_check();//库存盘点
    void filter_by_no_ch_dishno(const QString &ch_dishno);//商品
    void filter_by_no_ch_dishno_2(const QString &ch_dishno);//商品2
    void filter_by_no_dish_suit();//主商品
    void filter_by_no_dish_tempflag();//临时菜
    void filter_by_qr_code_onlineflag();//已上架

    void filter_by_other(const QString &filter);

    void enable_single_selected();
    void justShowColumns(const QStringList & vch_columns);
private:
    QString s_filter_by_itemflag;
    QString s_filter_by_material;
    QString s_filter_by_st_check;
    QString s_filter_by_no_ch_dishno;
    QString s_filter_by_no_ch_dishno_2;
    QString s_filter_by_no_dish_suit;
    QString s_filter_by_no_dish_temp;
    QString s_filter_by_dish_online;
    QString s_filter_by_other;

    QString filter_sql();
private slots:
    void treeselect();
    void torefresh();
    void took();
    void toexit();

    void show_type_when_empty_dish(bool flag);

    void toyunsync();
private:
    Ui::w_st_good_input_select_dish *ui;
    lds_model_sqltablemodel *tablemodel;
};

#endif // W_ST_GOOD_INPUT_SELECT_DISH_H
