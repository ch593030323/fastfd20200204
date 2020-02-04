#ifndef W_BT_DISH_BASE_H
#define W_BT_DISH_BASE_H

#include <QDialog>
#include "w_sys_manage.h"
#include "lds_model_sqltablemodel.h"
#include <QMap>
#include <QDoubleSpinBox>
#include "ftableview_standmodel_sql.h"

class QSqlQueryModel;
class QTreeWidgetItem;
class Ui_w_bt_dish_Dialog;
class lds_tableView;
class lds_CheckBox;
class lds_spinbox;
class Ui_w_bt_dish_new_dialog_2;
class lds_checkbox_unwrap;
class ftableview_delegate;
//信息
class w_bt_dish_base : public QDialog
{
    Q_OBJECT
public:
    explicit w_bt_dish_base(QWidget *parent = 0);
    ~w_bt_dish_base();
    static lds_model_sqltablemodel *creatmodel(QObject *parent);
    void setToolbarShown(bool f);
public slots:
    void torefresh();//树状图刷新，tableview刷新
    void treeselect();//tableview刷新

    void to_dish_type();
    void totypeset();
    void tounitset();
    void tonew();
    void tochange();
    void todel();
    void toexport();
    void toimport();
    void toexit();
    void tomaterial();

    //yun 会被线程触发，不会被手动调用
    void to_yun_version_diff(int d);
public:
    Ui_w_bt_dish_Dialog *ui;
    lds_model_sqltablemodel *tablemodel;
    QString tablemodel_filter;
    QString sql_select;
};

//品类设置
class Ui_w_bt_dish_typeset_dialog;
class w_bt_dish_base_typeset:public QDialog{
    Q_OBJECT
public:
    explicit w_bt_dish_base_typeset(QWidget *parent = 0);
private slots:
    void tonew2();
    void todel2();
    bool tosave();
    void torefresh();
    void toexit();
    void tolabelselect();

    void tableview2update(const QModelIndex &index);
    void toupdateColor(const QModelIndex &index);

    void tonew();
    void tochange();
    void todel();

    void tosave_show_dish_type(bool f);
    void tosave_dish_color_keep_subtype(bool f);
private:
    void replace(const QString &ch_dish_typeno, const QString &vch_dish_typename);
    bool saveData();
protected:
    lds_model_sqltablemodel *tablemodel,*tablemodel_2;
    Ui_w_bt_dish_typeset_dialog *ui;
};

//大类设置
class Ui_w_bt_dish_unitset_Dialog;
class w_bt_dish_base_dish_type:public QDialog{
    Q_OBJECT
public:
    explicit w_bt_dish_base_dish_type(w_bt_dish_base *parent = 0);
private slots:
    void tonew();
    void todel();
    bool tosave();
    void torefresh();
    void toexit();
private:
    Ui_w_bt_dish_unitset_Dialog *ui;
    lds_model_sqltablemodel *tablemodel;
    w_bt_dish_base *_parent;
};

//单位设置
class w_bt_dish_base_unitset:public QDialog{
    Q_OBJECT
public:
    explicit w_bt_dish_base_unitset(QWidget *parent = 0);
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
//修改
class QDataWidgetMapper;
class QTableView;
class QComboBox;
class QLineEdit;
class w_bt_dish_base_new_virtual : public QDialog{
    Q_OBJECT
public:
    explicit w_bt_dish_base_new_virtual(lds_model_sqltablemodel *tablemodel, QWidget *parent);
    ~w_bt_dish_base_new_virtual();
protected:
    virtual bool saveData() = 0;
protected slots:
    virtual void tocontinue() = 0;
    virtual void toexit() = 0;
    virtual void delayInitData();

    void spellupdate(const QString &v);//拼音简码更新
    void mpriceupdate(double v);//拼音简码更新
    void mpriceupdate();//拼音简码更新
    void update_ch_deducttype(int type);
    void updatesubdishtype();
    void updatesubdishtypeSubmit();
    void updatesubdishtypemodel(const QString &ch_dish_typeno);
    /*model和mapper需要自己创建*/
    void addmapping(lds_model_sqltablemodel *model, QDataWidgetMapper *mapper);

    bool saveCheck();
    void dishImageupdate();
    void enableDishImage(bool enabled);
    void toshow_group_info();

    void totab_advance();
    void totab_basic();
    void totab_item_set();

    void toitemdel();
    void toitemnews();
    void toitem_change();

    void frame_setEnable(bool e);
    void frame2_setEnable(bool e);


    void to_ch_unitno();
    void to_ch_dish_typeno();
    void to_ch_sub_typeno();
    void to_vch_pur_unitno();
    void toSaaSCheck(bool check);

    void showStackedIndex(int index);
    void toSetCountWeight(bool check);

    void toShowTax();
    void toChangeTax1Name();
    void toChangeTax2Name();
    void toChangeTax3Name();

    void to_product_find_online_by_barcode();
protected:
    bool dish_item_save();
    void dish_item_refresh();
    void item_count_weight_refresh();

    void enable_material();
protected:
    lds_bool_default_false b_enable_material;
protected:
    void editorsetValue(QLineEdit *editor, const QString &v, const QString &fieldName);
    void editorsetValue(lds_ComboBox *editor, const QString &v, const QString &fieldName);
    void editorsetValue(lds_ComboBox *editor, int v, const QString &fieldName);
    void editorsetValue(lds_CheckBox *editor, const QString &v, const QString &fieldName);
    void editorsetValue(lds_checkbox_unwrap *editor, const QString &v, const QString &fieldName);
    void editorsetValue(QDoubleSpinBox *editor, double v, const QString &fieldName);
    void editorsetValue(QSpinBox *editor, int v, const QString &fieldName);
public:
    Ui_w_bt_dish_new_dialog_2 *ui;
protected:
    QDataWidgetMapper *mapper;
    lds_model_sqltablemodel *tablemodel;
    QString imagedishdir;
    bool isDiry();
protected slots:
    void toupdateDishImage();
    void touclearDishImage();
    void toupdateDishColor();
    void toupdateItemFlag(const QString &f);
private:
    void showTaxWidget();
    void showTaxWidget(bool isshow);
    void updateTaxName(const QString &sys_parameter_key, const QString &defValue);
private:
    lds_model_sqltablemodel *dish_item_model;
    ftableview_standmodel_sql *model_dish;
    ftableview_delegate *model_dish_d;

};

class w_bt_dish_base_change:public w_bt_dish_base_new_virtual{
    Q_OBJECT
public:
    explicit w_bt_dish_base_change(QWidget *parent, lds_tableView *tableview, lds_model_sqltablemodel *tablemodel);
    virtual ~w_bt_dish_base_change();
    void setTypeEnabled(bool isenabled);

    void refresh();//根据一些信息，充值所有信息
signals:
    void signal_parent_to_refresh();
protected slots:
    void tosave();

    void toup();//下按钮
    void todown();//上按钮
    void mapperupdate();//mapper->setCurrentIndex(_parent->getTableview()->currentIndex().row());

protected:
    bool saveData();
protected slots:
    virtual void tocontinue();
    virtual void toexit();
    virtual void delayInitData();
private:
    lds_tableView *tableview;
};
//新增
class w_bt_dish_base_new:public w_bt_dish_base_new_virtual{
    Q_OBJECT
public:
    explicit w_bt_dish_base_new(QWidget *parent , const QString &ch_dish_typeno, const QString &ch_sub_typeno, const QString &ch_groupno, const QString &ch_tempflag);
    virtual ~w_bt_dish_base_new();
signals:
    void signalsave();
private slots:
    void tosave();

    void toupdatedishnoWithmax1();
    void tokeeprefresh();//保留大类小类编号
    void tonewrefresh();//保留大类小类刷新编号
    void torefresh();//保留大类小类
    //w_bt_dish_base_new_virtual
protected:
    bool saveData();
private slots:
    void tocontinue();
    void toexit();
    void delayInitData();
    //~w_bt_dish_base_new_virtual
private:
    QString ch_dish_typeno;
    QString ch_sub_typeno;
    QString ch_groupno;
    QString ch_tempflag;
};
class w_bt_dish_base_change_by_ch_dishno:public w_bt_dish_base_new_virtual{
    Q_OBJECT
public:
    explicit w_bt_dish_base_change_by_ch_dishno(QWidget *parent, const QString &ch_dishno);
    virtual ~w_bt_dish_base_change_by_ch_dishno();
protected slots:
    void tosave();
    virtual void tocontinue();
    virtual void toexit();

    void mapperupdate();
protected:
    bool saveData();
protected slots:
    virtual void delayInitData();
private:
    QString _ch_dishno;
};
//139 188 71647    冯工

#endif // W_BT_DISH_BASE_H
