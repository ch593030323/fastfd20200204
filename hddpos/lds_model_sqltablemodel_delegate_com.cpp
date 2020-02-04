#include "lds_model_sqltablemodel_delegate_com.h"
#include <QComboBox>
#include <QtDebug>
#include "lds_model_sqltablemodel.h"
#include "lds_model_sqltablemodel_d.h"


lds_model_sqltablemodel_delegate_com::lds_model_sqltablemodel_delegate_com(QObject *parent)
    :lds_tableview_delegate_virtual(parent)
{
    initPointer();_type = 0;
}

lds_model_sqltablemodel_delegate_com::~lds_model_sqltablemodel_delegate_com()
{
    delete model_cache;
}

lds_model_sqltablemodel_delegate_com::lds_model_sqltablemodel_delegate_com(QObject *parent, lds_model_sqltablemodel *m, const QString &tablename, const QString &sql_keyname, const QString &sql_valuename, const QString &sql_filter)
    : lds_tableview_delegate_virtual(parent)
{
    initPointer();_type = 1;
    //1
    show_value_m = m;
    show_value_tablename = tablename;
    show_value_sql_keyname = sql_keyname;
    show_value_sql_valuename = sql_valuename;
    show_value_sql_filter = sql_filter;
    //2
    show_com_map_ptr = 0;
    //3 可编辑，只有value，没有key
    //    show_com_editable_value_list;
    //4
    show_com_model = 0;
}

lds_model_sqltablemodel_delegate_com::lds_model_sqltablemodel_delegate_com(QObject *parent, const QMap<QString, QVariant> *map_ptr)
    : lds_tableview_delegate_virtual(parent)
{
    initPointer();_type = 2;
    show_com_map_ptr = map_ptr;
}

lds_model_sqltablemodel_delegate_com::lds_model_sqltablemodel_delegate_com(QObject *parent, const QMap<QString, QVariant> &map)
    : lds_tableview_delegate_virtual(parent)
{
    initPointer();_type = 13;
    show_com_map = map;

}

lds_model_sqltablemodel_delegate_com::lds_model_sqltablemodel_delegate_com(QObject *parent, const ldsVariantMap &map)
    : lds_tableview_delegate_virtual(parent)
{
    initPointer();_type = 13;
    show_com_map = map.d;
}

lds_model_sqltablemodel_delegate_com::lds_model_sqltablemodel_delegate_com(QObject *parent, const QStringList &value_list)
    : lds_tableview_delegate_virtual(parent)
{
    initPointer();_type = 3;
    show_com_editable_value_list = value_list;
}

lds_model_sqltablemodel_delegate_com::lds_model_sqltablemodel_delegate_com(QObject *parent, const QAbstractItemModel *model)
    : lds_tableview_delegate_virtual(parent)
{
    initPointer();_type = 4;
    show_com_model = model;
}

lds_model_sqltablemodel_delegate_com::lds_model_sqltablemodel_delegate_com(QObject *parent, const QString &tablename, const QString &sql_keyname, const QString &sql_valuename, const QString &sql_filter)
    : lds_tableview_delegate_virtual(parent)
{
    initPointer();_type = 6;
    //1
    show_value_tablename = tablename;
    show_value_sql_keyname = sql_keyname;
    show_value_sql_valuename = sql_valuename;
    show_value_sql_filter = sql_filter;
}

void lds_model_sqltablemodel_delegate_com::update_show_value(const QString &tablename, const QString &sql_keyname, const QString &sql_valuename, const QString &sql_filter)
{
    show_value_tablename = tablename;
    show_value_sql_keyname = sql_keyname;
    show_value_sql_valuename = sql_valuename;
    show_value_sql_filter = sql_filter;
    clear_model_cache();
}

void lds_model_sqltablemodel_delegate_com::clear_model_cache()
{
    model_cache->tablename_kvmap_delete_for_select_and_autoselect();
}

lds_model_sqltablemodel_delegate_com::lds_model_sqltablemodel_delegate_com(QObject *parent, const lds_model_sqltablemodel_d *d)
    : lds_tableview_delegate_virtual(parent)
{
    initPointer();_type = 8;
    model_d = d;
}

//lds_model_sqltablemodel_delegate_com::lds_model_sqltablemodel_delegate_com(QObject *parent, const QString vch_print_memo)
//    : lds_tableview_delegate_virtual(parent)
//{
//    Q_UNUSED(vch_print_memo)
//    initPointer();_type = 10;
//}

lds_model_sqltablemodel_delegate_com::lds_model_sqltablemodel_delegate_com(QObject *parent, QComboBox *com)
    : lds_tableview_delegate_virtual(parent)
{
    initPointer();_type = 11;
    show_combobox = com;
}

lds_model_sqltablemodel_delegate_com::lds_model_sqltablemodel_delegate_com(QObject *parent, int rgb_color)
    : lds_tableview_delegate_virtual(parent)
{
    Q_UNUSED(rgb_color)
    initPointer();_type = 12;
}

lds_model_sqltablemodel_delegate_com::lds_model_sqltablemodel_delegate_com(QObject *parent, const qrtVariantPairList &pairList)
    : lds_tableview_delegate_virtual(parent)
{
    initPointer();_type = 14;
    this->pairList = pairList;
}

//该函数不能有tableview的row导入
QString lds_model_sqltablemodel_delegate_com::paint_transfer(const QVariant &dd, const QString &field_name) const
{
    QString key = dd.toString();
    QVariant value;
    if(false == b_paint_transfer) return key;
    switch(_type) {
    case 1://show value
        if(show_value_m) {
            value = show_value_m->tablename_kvmap_value(show_value_tablename, key, show_value_sql_keyname, show_value_sql_valuename, show_value_sql_filter);
        }
        break;
    case 2://show com
        if(show_com_map_ptr) {
            value = show_com_map_ptr->value(key);
        }
        break;
    case 13://show com
        value = show_com_map.value(key);
        break;
    case 11:
    {
        value = show_combobox->itemText(show_combobox->findData(key));
    }
        break;
    case 3://show com editable
        //3 可编辑，只有value，没有key
        value = key;
        break;
    case 4://show com
        if(show_com_model) {
            for(int row = 0, rowcount = show_com_model->rowCount(); row < rowcount; row ++) {
                if(key == show_com_model->data(show_com_model->index(row, 0)).toString()) {
                    value = show_com_model->data(show_com_model->index(row, 1/*value*/));
                    break;
                }
            }
        }
        break;
    case 6:
    {
        value = model_cache->tablename_kvmap_value(show_value_tablename, key, show_value_sql_keyname, show_value_sql_valuename, show_value_sql_filter);
    }
        break;
    case 8:
    {
        if(model_d) {
            value = model_d->data(key, 0);
        }
    }
        break;

        //    case 10:
        //    {

        //    }
        //        break;

    case 12:
    {
        value = key;
    }
        break;
    case 14:
    {
        foreach(const qrtVariantPair &pair, pairList) {
            if(pair.first == key) {
                value = pair.second.toString();
                break;
            }
        }
    }
        break;
    }

    if(b_empty_return_key && value.isNull()) {
        return key;
    }
    if(value.isNull()) {
        return paint_transfer_default_value;
    }
    return value.toString();
}

void lds_model_sqltablemodel_delegate_com::enable_paint_transfer()
{
    b_paint_transfer = true;
}

void lds_model_sqltablemodel_delegate_com::enable_empty_return_key()
{
    b_empty_return_key = true;
}

void lds_model_sqltablemodel_delegate_com::enable_paintbtn()
{
    b_enable_paintbtn = true;
}

void lds_model_sqltablemodel_delegate_com::enable_paintbtn_color()
{
    enable_paintbtn();
    b_enable_paintbtn_color = true;
}

void lds_model_sqltablemodel_delegate_com::set_paint_transfer_default_value(const QString &default_value)
{
    paint_transfer_default_value = default_value;
}

void lds_model_sqltablemodel_delegate_com::initPointer()
{
    show_value_m = 0;
    show_com_map_ptr = 0;
    show_com_model = 0;
    model_d = 0;
    model_cache = new lds_model_sqltablemodel;
    show_combobox = 0;
}

///////////////////////////////////
void lds_model_sqltablemodel_delegate_com::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(b_enable_paintbtn) {
        if(b_enable_paintbtn_color) {
            lds_tableview_delegate_virtual::paintBtn(painter, option, index, index.data(), 1);
            return;
        } else {
            lds_tableview_delegate_virtual::paintBtn(painter, option, index, index.data(), 0);
            return;
        }
    }

    lds_tableview_delegate_virtual::paintText(painter, option, index, index.data());
}

QWidget *lds_model_sqltablemodel_delegate_com::createEditor(QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    QWidget *editor = 0;
    QString key = index.data().toString();
    switch(_type) {
    case 1://show value
    {
        editor = lds_tableview_delegate_virtual::createEditor(aParent, option, index);
        goto end;
    }
        break;
    case 2://show com
    {
        QComboBox *com = new QComboBox(aParent);
        for(QMap<QString, QVariant>::const_iterator itr = show_com_map_ptr->begin(); itr != show_com_map_ptr->end(); itr++) {
            com->addItem(itr.value().toString(), itr.key());
        }
        com->setCurrentIndex(com->findData(key));
        editor = com;
        goto end;
    }
        break;
    case 13://show com
    {
        QComboBox *com = new QComboBox(aParent);
        for(QMap<QString, QVariant>::const_iterator itr = show_com_map.begin(); itr != show_com_map.end(); itr++) {
            com->addItem(itr.value().toString(), itr.key());
        }
        com->setCurrentIndex(com->findData(key));
        editor = com;
        goto end;
    }
        break;
    case 3://show com editable
    {
        QComboBox *com = new QComboBox(aParent);
        com->setEditable(true);
        for(int k = 0; k < show_com_editable_value_list.count(); k++) {
            com->addItem(show_com_editable_value_list[k]);
        }
        if(com->findText(key) >= 0) {
            com->setCurrentIndex(com->findText(key));
        } else {
            com->setEditText(key);
        }
        editor = com;
        goto end;
    }
        break;
    case 4://show com
    {
        QComboBox *com = new QComboBox(aParent);
        for(int row = 0, rowcount = show_com_model->rowCount(); row < rowcount; row ++) {
            com->addItem(show_com_model->data(show_com_model->index(row, 1)).toString(), show_com_model->data(show_com_model->index(row, 0)));
        }
        com->setCurrentIndex(com->findData(key));
        editor = com;
        goto end;
    }
        break;
    case 8://lds_model_sqltablemodel_d
    {
        QComboBox *com = new QComboBox(aParent);
        lds_model_sqltablemodel_d::fillData(com, model_d);
        com->setCurrentIndex(com->findData(key));
        editor = com;
        goto end;
    }
        break;
    default:
    {
        editor = lds_tableview_delegate_virtual::createEditor(aParent, option, index);
        goto end;
    }
        break;
    }
end:
    return editor;
}

void lds_model_sqltablemodel_delegate_com::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *com = qobject_cast<QComboBox *>(editor);
    if(com) {
        if(com->isEditable()) {
            lds_tableview_delegate_virtual::setModelData(com->currentText(), model, index);
            return;
        }
        lds_tableview_delegate_virtual::setModelData(com->itemData(com->currentIndex()), model, index);
        return;
    }

    lds_tableview_delegate_virtual::setModelData(editor, model, index);
}

void lds_model_sqltablemodel_delegate_com::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    lds_tableview_delegate_virtual::setEditorData(editor, index);
}
