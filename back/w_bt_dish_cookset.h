#ifndef W_BT_DISH_COOKSET_H
#define W_BT_DISH_COOKSET_H

#include <QDialog>
#include <QMap>
#include "lds_model_sqltablemodel.h"

#include "w_bt_dish.h"

class Ui_w_bt_dish_cookset_typeset_dialog;
class Ui_w_bt_dish_cookset_Dialog;
class QSqlQueryModel;
class w_bt_dish_cookset : public QDialog
{
    Q_OBJECT

public:
    explicit w_bt_dish_cookset(QWidget *parent = 0);
    lds_model_sqltablemodel *tablemodel;
    QSqlQueryModel *querymodel_2;QString sql_select_3;
    Ui_w_bt_dish_cookset_Dialog *ui;
public slots:
    void torefresh();
private slots:
    void t1_t2_t3();
    void t1_t2();
    void t2_t3();

    void tocooktype();
    void tospecialcook();
    void todel();
    void todishset();
    void todishsets();
    void toexport();
    void toexit();
signals:

    
};

//做法类型
class Ui_w_bt_dish_unitset_Dialog;
class lds_model_sqltablemodel;
class w_bt_dish_cookset_cooktype:public QDialog{
    Q_OBJECT
public:
    explicit w_bt_dish_cookset_cooktype(QWidget *parent = 0);
private slots:
    void tonew();
    void todel();
    bool tosave();
    void torefresh();
    void toexit();
private :
    Ui_w_bt_dish_unitset_Dialog *ui;
    lds_model_sqltablemodel *tablemodel;
};

//特殊做法
class Ui_w_bt_dish_cookset_specialcook__Dialog;
class w_bt_dish_cookset_specialcook:public QDialog{
    Q_OBJECT
public:
    explicit w_bt_dish_cookset_specialcook(QWidget *parent = 0);
private slots:
    void tonew();
    void todel();
    bool tosave();
    void torefresh();
    void toexit();
    void totable_2();
private :
    Ui_w_bt_dish_cookset_specialcook__Dialog *ui;
    lds_model_sqltablemodel *tablemodel;
    lds_model_sqltablemodel *tablemodel_2;
};

//做法类别设置
class Ui_w_bt_dish_cookset_dishset;
class w_bt_dish_cookset_typeset : public QDialog {
    Q_OBJECT
public:
    explicit w_bt_dish_cookset_typeset(QWidget *parent = 0);
    ~w_bt_dish_cookset_typeset();
    void enableSaveAndAppDish(const QString &ch_dishno_app);
private slots:
    void tonew2();
    void todel2();
    void tosave();
    void toRefreshType();
    void toexit();

    void tableview2Update(const QModelIndex &typeIndex);

    void tonew();
    void tochange();
    void todel();
private:
    bool saveData();
protected:
    lds_model_sqltablemodel *tablemodel;
    lds_model_sqltablemodel *tablemodel_2;
    Ui_w_bt_dish_cookset_typeset_dialog *ui;
private:
    QString ch_dishno_app;
};

//做法设置之商品设置
class Ui_w_bt_dish_cookset_dishset_Dialog;
class w_bt_dish_cookset_dishset:public QDialog{
    Q_OBJECT
public:
    explicit w_bt_dish_cookset_dishset(QWidget *parent = 0);
private slots:
    void toSelectAll();
    void toselectno();
    void toapp();
    void toappall();
    void took();
    void toexit();
    void torefresh();
    void treeselect();
private:
    bool appone();
    bool app(const QString &ch_dishno, QString *errstring = 0);
private:
    Ui_w_bt_dish_cookset_dishset_Dialog *ui;
    lds_model_sqltablemodel *tablemodel_2;QString t2_ch_typeno;
    w_bt_dish_cookset *_parent;
};

#endif // W_BT_DISH_COOKSET_H
