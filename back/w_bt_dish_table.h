#ifndef W_BT_DISH_TABLE_H
#define W_BT_DISH_TABLE_H

#include <QDialog>

class Ui_w_bt_dish_Dialog;
class QSqlQueryModel;
class lds_treeWidget;
class lds_model_sqltablemodel;
class lds_tableView;
class Ui_w_bt_dish_table_new2;
class Ui_w_bt_dish_table_tableset_Dialog;
class Ui_w_bt_dish_unitset_Dialog;
class Ui_w_bt_dish_table_new_Dialog;
class QDataWidgetMapper;
class QTableView;
class Ui_w_bt_table_news_Dialog;

class w_bt_dish_table : public QDialog
{
    Q_OBJECT
public:
    explicit w_bt_dish_table(QWidget *parent = 0);
    ~w_bt_dish_table();
public:
    static bool tableDelTransaction(const QString &ch_tableno, QString *errstring);
public slots:
    void torefresh();
    void setToolbarShown (bool shown);
    void treeselect();//tree update
private slots:
    void tonew();//新建
    void tochange();//修改
    void todel();//删除
    void tonews();//批量新建
    void totable();//桌类设置
    void toarea();//区域设置
    void toexport();//导出
    void toimport();//导出
    void toexit();//退出

public:
    Ui_w_bt_dish_Dialog *ui;
    lds_model_sqltablemodel *tablemodel;
};

//修改
class w_bt_dish_table_virtual:public QDialog{
    Q_OBJECT
public:
    explicit w_bt_dish_table_virtual(QWidget *parent);
    ~w_bt_dish_table_virtual();

    void update_ch_typeno(const QString &ch_typeno);
    void update_ch_areano(const QString &ch_areano);
private slots:
    virtual bool tosave() = 0;
protected:
    bool saveCheck();
protected:
    Ui_w_bt_dish_table_new2 *ui;
};

class w_bt_dish_table_change:public QDialog{
    Q_OBJECT
public:
    explicit w_bt_dish_table_change(lds_model_sqltablemodel *tablemodel, lds_tableView *tableview, QWidget *parent);
private slots:
     void tosave();
     void toexit();
     void toup();
     void todown();
private:
    bool saveData();
private:
    Ui_w_bt_dish_table_new_Dialog *ui;
    QDataWidgetMapper *mapper;
    lds_model_sqltablemodel *tablemodel;
    lds_tableView *tableview;

};

class w_bt_dish_table_new2:public w_bt_dish_table_virtual{
    Q_OBJECT
public:
    explicit w_bt_dish_table_new2(QWidget *parent);
protected slots:
    void toexit();

    void toarea();
    void totype();

    void torefresh();
    void torefreshTableno(int index);
    //from w_bt_dish_table_virtual
    virtual bool tosave();
    //from w_bt_dish_table_new2
    virtual void torefresh_ch_tableno(const QVariant &ch_typeno);
};

class w_bt_dish_table_change2:public w_bt_dish_table_new2{
    Q_OBJECT
public:
    explicit w_bt_dish_table_change2(const QString &ch_tableno, QWidget *parent);

private:
    void update_ch_tableno(const QString &ch_tableno);
private slots:
    //from w_bt_dish_table_virtual
    virtual bool tosave();
    //from w_bt_dish_table_new2
    virtual void torefresh_ch_tableno(const QVariant &/*ch_typeno*/);

    void toDel();
};

class w_bt_dish_table_new3 : public  QDialog{
    Q_OBJECT
public:
    explicit w_bt_dish_table_new3(QWidget *parent);
    ~w_bt_dish_table_new3();

private slots:
    void tonew();
    void todel();
    bool tosave();
    void torefresh();
    void toexit();
private:
    Ui_w_bt_dish_unitset_Dialog *ui;
    lds_model_sqltablemodel *tablemodel;
};

//批量新增
class w_bt_dish_table_news:public QDialog{
    Q_OBJECT
public:
    explicit w_bt_dish_table_news(QWidget *parent);
private slots:
    void togenrate();
    void toexit();
private:
    Ui_w_bt_table_news_Dialog *ui;
    w_bt_dish_table *_parent;
};

//餐桌设置
class w_bt_dish_table_tableset:public QDialog{
    Q_OBJECT
public:
    explicit w_bt_dish_table_tableset(QWidget *parent);
private slots:
    void tonew();
    void todel();
    bool tosave();
    void toexit();
    void update_enabled(int index);
private:
    Ui_w_bt_dish_table_tableset_Dialog *ui;
    lds_model_sqltablemodel *tablemodel;
    QDataWidgetMapper *mapper;
    w_bt_dish_table *_parent;
};

class w_bt_dish_table_tableset_cut : public QDialog {
    Q_OBJECT
public:
    explicit w_bt_dish_table_tableset_cut(QWidget *parent = 0);
    ~w_bt_dish_table_tableset_cut();
private slots:
    void tonew();
    void todel();
    void tosave();
    void toexit();

    void torefresh();
private:
    Ui_w_bt_dish_unitset_Dialog *ui;
    lds_model_sqltablemodel *tablemodel;
};

//区域设置
class w_bt_dish_table_areaset:public QDialog{
    Q_OBJECT
public:
    explicit w_bt_dish_table_areaset(QWidget *parent);
private slots:
    void tonew();
    void todel();
    void torefresh();
    bool tosave();
    void toexit();
private:
    int findareano(const QString &areano, int exceptrow);

private:
    Ui_w_bt_dish_unitset_Dialog *ui;
    lds_model_sqltablemodel *tablemodel;
    QDataWidgetMapper *mapper;
};
#endif // W_BT_DISH_TABLE_H
