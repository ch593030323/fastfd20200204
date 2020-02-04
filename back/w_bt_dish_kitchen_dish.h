#ifndef W_BT_DISH_KITCHEN_DISH_H
#define W_BT_DISH_KITCHEN_DISH_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"
#include "lds_thread.h"
#include "public_ins_maker.h"
#include "lds_model_mapwidget_vertical.h"
#include "w_bt_dish.h"
#include "public_dialog.h"
#include "pointerautodel.h"
#include <QDataWidgetMapper>

class QComboBox;
namespace Ui {
class w_bt_dish_kitchen_dish;
}
///======================================
class w_bt_dish_kitchen_dish_thread : public lds_thread {
    Q_OBJECT
public:
    w_bt_dish_kitchen_dish_thread(QObject *parent);
    ~w_bt_dish_kitchen_dish_thread();
    //from QThread
protected:
    virtual void run();
};
//======================================
class w_bt_dish_kitchen_dish_thread_event : public lds_thread_event {
    Q_OBJECT
public:
    static QString getSqlReplaceIndexSql(const QString &key, int index, char rv);
    static QString getSqlReplaceIndex(const QString &key, int index, char rv);
public:
    w_bt_dish_kitchen_dish_thread_event(QObject *parent = 0);
    //from lds_thread_event
public slots:
    virtual void toExec() ;
private :
    void kitchen_key(lds_query_hddpos  &query, const QString &key);
    bool kitchen_update_transaction(lds_query_db &query_local,
                                    lds_query_db &query,
                                    int int_kitchen_section, int int_indexbox, int key_state);
    bool kitchen_update_transaction(lds_query_db &query,
                                    const QString &ch_billno,
                                    int key_state);
    void kitchen_ins_make(const w_bt_dish::kitchenState key_state,
                          public_ins_maker &print_format,
                          lds_query_db &query_local,
                          int x,
                          int int_indexbox,


                          int vch_plan_id_TRY_count
                          );
    bool kitchen_ins_print(const w_bt_dish::kitchenState key_state,
                           public_ins_maker &print_format,
                           lds_query_db &query_local,
                           int x,
                           int int_indexbox,

                           const QString &ch_check_state,
                           lds_query_db &query,
                           const QString &int_print_id,
                           int int_print_count,
                           QString &errstring);

    QByteArray kitchen_json_make(const w_bt_dish::kitchenState key_state,
                                 lds_query_db &query_local,
                                 int x, int int_indexbox,
                                 int vch_plan_id_TRY_count
                                 );
    bool kitchen_json_send(const w_bt_dish::kitchenState key_state,
                           public_ins_maker &print_format,
                           lds_query_db &query_local,
                           int x,
                           int int_indexbox,

                           const QString &ch_check_state,
                           lds_query_db &query,
                           const QString &int_print_id,
                           const QByteArray &json_value,
                           QString &errstring);


    QString get_bp_fmt_table(const QString &vch_tablename, const QString &ch_tableno);
    QString get_bp_fmt_operid(const QString &vch_operator, const QString &vch_operid);
    QString get_bp_fmt_waiter(const QString &vch_waiter_name, const QString &vch_waiter);
    ///tablemodel
    /////////////////////////////
    bool print_open_fail_exist(const QString &ip);
    void print_open_fail_insert(const QString &ip);
    void print_open_fail_clear();

    QString f_get_sysparm(const QString &as_parameter, const QVariant &as_default=QVariant());
    QString f_get_vch_company();
    QVariant f_get_serial_no_cur(const QString &ch_billno);
    QVariant f_get_vch_tablename(const QString &ch_tableno);

    QVariant f_get_from_cey_bt_kitchen_print(const QString &int_id, const QString &valuename);
    QVariant f_get_from_cey_u_table(const QString &ch_billno, const QString &valuename);
    QVariant f_get_from_cey_bt_dish(const QString &ch_dishno, const QString &valuename);
    QVariant f_get_from_cey_bt_unit(const QString &ch_unitno, const QString &valuename);
    QVariant f_get_from_cey_bt_table(const QString &ch_tableno, const QString &valuename);
    QVariant f_get_from_cey_bt_kitchen_plan(const QString &vch_plan_id, const QString &valuename);

    void login_emit_signal_name_id_state(const QString &int_id, uchar state);
    /////////////////////////////

public:
    QString errstring;
    public_ins_maker print_format;
private:
    pointerAutoDel<lds_model_sqltablemodel> tablemodel;
};
///～======================================

class w_bt_dish_kitchen_dish_print_state_thread : public lds_thread {
    Q_OBJECT
public:
    w_bt_dish_kitchen_dish_print_state_thread(QObject *parent);
    ~w_bt_dish_kitchen_dish_print_state_thread();
protected:
    virtual void run();
};
///
class w_bt_dish_kitchen_dish_print_state_thread_event : public lds_thread_event {
    Q_OBJECT
public:
    w_bt_dish_kitchen_dish_print_state_thread_event(QObject * parent = 0)
        : lds_thread_event(parent) {}
public slots:
    //from lds_thread_event
    void toExec() ;

private:
    void login_emit_signal_name_id_state(const QString &int_id, uchar state);
};
///
///


//=======================================

class w_bt_dish_kitchen_dish : public public_Dialog
{
    Q_OBJECT
public:
    explicit w_bt_dish_kitchen_dish(QWidget *parent = 0);
    w_bt_dish_kitchen_dish(QWidget *parent, bool tmpdish);
    ~w_bt_dish_kitchen_dish();

    virtual void retranslateView();

    void initView();
    void initViewTmp();
    void initViewNormal();
public:
    void refresh();
private slots:
    void torefresh();
    void treeselect();
    void toexit();
    void tokitchenplan();
    void toprintset();
    void tomore();
    void tosave();
    void tokitchenparam();
    void toSwitchLanguage();
    void tolostclear();
private:
    bool saveData();
    int checkDirty();
private:
    Ui::w_bt_dish_kitchen_dish *ui;
    QString sql_where_app;
public:
    lds_model_sqltablemodel *tablemodel;
};

class Ui_w_bt_dish_kitchen_dish_more;
class w_bt_dish_kitchen_dish_more : public QDialog {
    Q_OBJECT

public:
    explicit w_bt_dish_kitchen_dish_more(lds_model_sqltablemodel *tablemodel, QWidget *parent = 0);
    virtual ~w_bt_dish_kitchen_dish_more();
    void setDesc(const QString &desc);
    void hideServicePart();
private slots:
    void took();
    void tocancel();
private:
    void fillData_1(QComboBox *com, const QMap<QString, QVariant> * map);
    bool hasRepeated(QStringList list, bool skip_empty = true);
private:
    Ui_w_bt_dish_kitchen_dish_more *ui;
    lds_model_sqltablemodel *tablemodel;
};

class BoxPrintIndex {
public:
    BoxPrintIndex() {}
    int getIndex(const QString &ch_billno, const QString &vch_print_id, const QString &vch_plan_id,
                 int int_dish_type, const QString &ch_dish_typeno, const QString &ch_sub_typeno, QString &int_indexbox_desc);
    QMap<QString , int> keyIdxMap;
};

class Ui_w_bt_dish_kitchen_dish_param;
class lds_model_sqltablemodel;
class w_bt_dish_kitchen_dish_param : public lds_model_mapwidget_vertical {
    Q_OBJECT

public:
    explicit w_bt_dish_kitchen_dish_param(QWidget *parent = 0);
private slots:
    void took();
    void tocancel();
    void to_kitchen_start_toggled(bool checked);

    void toSaaSCheck(int index);

    void toHideExSet();
    void toShowExSet();
protected:
    virtual bool insert_sql_objectname(const QString &objectname);
private:
    bool saveData();
private:
    Ui_w_bt_dish_kitchen_dish_param *ui;
};

#endif // W_BT_DISH_KITCHEN_DISH_H
