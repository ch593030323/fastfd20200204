#ifndef LDS_MODEL_SQLTABLEMODEL_DELEGATE_COM_H
#define LDS_MODEL_SQLTABLEMODEL_DELEGATE_COM_H

#include "lds_tableview_delegate_virtual.h"
#include "lds_bool.h"
#include "lds_query_db.h"
#include "ldsvariantmap.h"

class QTableView;
class QComboBox;
class lds_model_sqltablemodel;
class lds_model_sqltablemodel_d;
class lds_model_sqltablemodel_delegate_com : public lds_tableview_delegate_virtual
{
public:
    lds_model_sqltablemodel_delegate_com(QObject *parent);
    virtual ~lds_model_sqltablemodel_delegate_com();

    //QAbstractItemView does not take ownership of delegate.
    ///show value
    ///------------------1
    lds_model_sqltablemodel_delegate_com(
            QObject *parent,

            lds_model_sqltablemodel *m,
            const QString &tablename,
            const QString &sql_keyname,
            const QString &sql_valuename,
            const QString &sql_filter = ""
            );
    ///show com
    ///------------------2
    lds_model_sqltablemodel_delegate_com(
            QObject *parent,
            const QMap<QString/*key*/, QVariant/*value*/>* map_ptr
            );
    //13
    lds_model_sqltablemodel_delegate_com(
            QObject *parent,
            const QMap<QString/*key*/, QVariant/*value*/> & map
            );
    //14
    lds_model_sqltablemodel_delegate_com(
            QObject *parent,
            const ldsVariantMap & map
            );
    ///show com editable
    ///------------------3
    lds_model_sqltablemodel_delegate_com(
            QObject *parent,
            const QStringList & value_list
            );
    ///show com
    ///------------------4
    lds_model_sqltablemodel_delegate_com(
            QObject *parent,
            const QAbstractItemModel *model
            );
    ///show 尼波兰
    ///------------------5
    ///show value, 使用delegate自己的map
    ///------------------6
    lds_model_sqltablemodel_delegate_com(
            QObject *parent,

            const QString &tablename,
            const QString &sql_keyname,
            const QString &sql_valuename,
            const QString &sql_filter = ""
            );
    void update_show_value(
            const QString &tablename,
            const QString &sql_keyname,
            const QString &sql_valuename,
            const QString &sql_filter = "");
    void clear_model_cache();
    ///------------------7

    ///show com----------8
    lds_model_sqltablemodel_delegate_com(
            QObject *parent,
            const lds_model_sqltablemodel_d *d
            );

    ///------------------9

//    ///------------------10
//    lds_model_sqltablemodel_delegate_com(
//            QObject *parent,
//            const QString vch_print_memo
//            );
    ///------------------11
    lds_model_sqltablemodel_delegate_com(
            QObject *parent,
            QComboBox *com//regard as map<QString , QVarint>
            );

//    ///------------------12
    lds_model_sqltablemodel_delegate_com(
            QObject *parent,
            int rgb_color
            );
    ///------------------14
    lds_model_sqltablemodel_delegate_com(
            QObject *parent,
            const qrtVariantPairList & pairList
            );
public:
    virtual QString paint_transfer(const QVariant &dd, const QString &field_name) const ;

    void enable_paint_transfer();
    void enable_empty_return_key();//若是value为空则返回key
    void enable_paintbtn();//实现显示按钮的功能
    void enable_paintbtn_color();//实现例如：10杯的功能
    void set_paint_transfer_default_value(const QString &default_value);
private:
    lds_bool_default_false b_empty_return_key;//若value 为空返回key
    lds_bool_default_false b_enable_paintbtn;//按钮
    lds_bool_default_false b_enable_paintbtn_color;
    QString paint_transfer_default_value;
    ///////////////////////////2//////////////////////

    void initPointer();

protected:
    void paint(QPainter * painter,const QStyleOptionViewItem & option, const QModelIndex & index) const;

    virtual QWidget *createEditor(QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;

private:
    int  _type;
    //1
    lds_model_sqltablemodel *show_value_m;
    QString show_value_tablename;
    QString show_value_sql_keyname;
    QString show_value_sql_valuename;
    QString show_value_sql_filter;

    //2
    const QMap<QString/*key*/, QVariant/*value*/>* show_com_map_ptr;
    QMap<QString/*key*/, QVariant/*value*/> show_com_map;
    QComboBox* show_combobox;
    //3
    QStringList show_com_editable_value_list;

    //4
    const QAbstractItemModel * show_com_model;//key value
    //6
    lds_model_sqltablemodel *model_cache;
    //8
    const lds_model_sqltablemodel_d *model_d;
    //14
    qrtVariantPairList pairList;

};

#endif // LDS_MODEL_SQLTABLEMODEL_DELEGATE_COM_H
