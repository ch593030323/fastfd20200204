#ifndef W_SCR_DISH_RESTAURANT_DIV_POP_H
#define W_SCR_DISH_RESTAURANT_DIV_POP_H

#include <QDialog>
#include <QModelIndexList>
#include "lds_query_hddpos.h"

class restaurantTableView;
class ftableview;
class restraurant9BoxDelegate;
class ftableview_standmodel_sql_none;
class lds_model_sqltablemodel;

namespace Ui {
class w_scr_dish_restaurant_div_pop;
}

class w_scr_dish_restaurant_div_pop : public QDialog
{
    Q_OBJECT

public:
    enum{
        Model_ch_billno = Qt::UserRole,
        Model_sqr = Qt::UserRole + 1,
        Model_check = Qt::UserRole + 4
    };
private:
    static void initFTable(ftableview *tableView);
public:
    w_scr_dish_restaurant_div_pop(QWidget *parent);
    ~w_scr_dish_restaurant_div_pop();

    //根据ch_billno_map 初始化九宫格
    void update_by_ch_billnos_ch_tableno(const QMap<int/*sqr*/, QString/*ch_billno*/> &ch_billno_map, const QString &ch_tableno = "", bool show_num_cost = true);
    void update_by_ch_billnos(const QMap<int/*sqr*/, QString/*ch_billno*/> &ch_billno_map);

    //获取当前选中的indexlist，返回的index中，可能check状态不为true
    QModelIndexList get_cur_check_index_list();
    //
    QModelIndex getModeIndex(int row, int column);
    //
    QString get_ch_billno_by_index(const QModelIndex &index);
    QString get_sqr_by_curindex();
    QString get_ch_billno_by_sqr(QString sqr);
    //直接调用tableview-》setCurrentIndex函数，若是modelcheck，则不会清空，
    void setcurindex_by_sqr(int sqr);
    void load_div_check_respone(restaurantTableView *check_proxy);
    /////////////////////////////////////////////////////////////////
public:
    void enableMutilSelected();
    void enableSinglSelectedWhenMutil();
    void enableItemFlagSelectedWithAll();
    void enableItemFlagSelectedWith_nochbillno();
    void enableItemFlagSelectedWith_chbillno_bar();
    void setFlagWith_KAITAI_YUDA();
signals:
    void signal_div_click(const QModelIndex &index);

public slots:
    void setDesc(const QString &desc);
    void setShowDesc(const QString &text);
private slots:
    void tocancel();
    void toSendChecklist();
protected:
    Ui::w_scr_dish_restaurant_div_pop *ui;
    ftableview_standmodel_sql_none *model ;
    restraurant9BoxDelegate *delegate9box;

    QMap<int/*sqr*/, QString/*ch_billno*/> ch_billno_map;
private:
    restaurantTableView *check_proxy;
};

//用于消台时，弹出选择窗口
class w_scr_dish_restaurant_div : public w_scr_dish_restaurant_div_pop{
    Q_OBJECT
public:
    explicit w_scr_dish_restaurant_div(const QMap<int/*sqr*/, QString/*ch_billno*/> &ch_billno_map, const QString &ch_tableno, QWidget *parent);

};
//用于开台界面的小的table
class w_scr_dish_restaurant_div_pop_only_table : public w_scr_dish_restaurant_div_pop{
    Q_OBJECT
public:
    w_scr_dish_restaurant_div_pop_only_table(bool isTransparent = false, QWidget *parent = 0);
    void trycombine();//尝试组合， 只有0的时候可以成功
    bool iscombine;

};

#endif // W_SCR_DISH_RESTAURANT_DIV_POP_H
