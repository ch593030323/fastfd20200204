#ifndef W_SCR_DISH_RESTAURANT_PREORDER_H
#define W_SCR_DISH_RESTAURANT_PREORDER_H

#include <QDialog>
#include <QSqlQueryModel>


class Ui_w_scr_dish_restaurant_preorder_add_Dialog;
class QStandardItemModel;
class ftableview_standmodel_sql_table;
class fexpandmain_model_sqltablemodel;
class Ui_w_scr_dish_restaurant_preorder_set;
namespace Ui {
class w_scr_dish_restaurant_preorder;
}

class w_scr_dish_restaurant_preorder : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_restaurant_preorder(QWidget *parent);
    ~w_scr_dish_restaurant_preorder();

private slots:
    void toprevious();
    void tonext();
    void toexit();
    void toadd();
    void tochange();
    void tocancel();
    void tosetting();
    void toprintbill();
    void toorderopentable();
    void tosearch();
    void tosaveChstate(bool f);
public slots:
    void torefresh();
private:
    bool todel(const QString &ch_bookno);
private:
    Ui::w_scr_dish_restaurant_preorder *ui;
    class  DateQueryModel : public QSqlQueryModel{
    public:
        DateQueryModel(QObject *parent = 0) : QSqlQueryModel(parent) {}
        QVariant data(const QModelIndex &item, int role) const;
    };
    DateQueryModel *querymodel;
    QString selectsql;
};

//////////////////////////////////////新增
class w_scr_dish_restaurant_preorder_add_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_restaurant_preorder_add_Dialog(QWidget *parent);
    ~w_scr_dish_restaurant_preorder_add_Dialog();

    Ui_w_scr_dish_restaurant_preorder_add_Dialog *ui;
private slots:
    void took();
    void tocancel();
    void tomember();
    void tosave();
    void tosetovertime();
    void toupdateWarndiff(const QDateTime &dt);
    void tosavewarndiff(int value);
    void todeltable();
    void toupdateTabledesc();
public slots:
    void torefresh();
protected:
    bool saveData();
    bool checkDtcomePeriod(
            const QDateTime &dt_come,
            const QDateTime & dt_come_left,
            const QDateTime & dt_come_right
            );
    ftableview_standmodel_sql_table *modeltable;
};


/////////////////////////////////////修改
/// \brief The w_scr_dish_restaurant_preorder_set class
class w_scr_dish_restaurant_preorder_set : public QDialog{
    Q_OBJECT

public:
    explicit w_scr_dish_restaurant_preorder_set(QWidget *parent);
    ~w_scr_dish_restaurant_preorder_set();
    Ui_w_scr_dish_restaurant_preorder_set *ui;
private slots:
    void took();
    void  tocancel();
};
#endif // W_SCR_DISH_RESTAURANT_PREORDER_H
