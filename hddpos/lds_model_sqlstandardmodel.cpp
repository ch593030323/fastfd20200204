#include "lds_model_sqlstandardmodel.h"
#include <QSqlRecord>
#include <QSqlIndex>
#include <QtDebug>
#include "lds_query_thread.h"

lds_model_sqlstandardmodel::lds_model_sqlstandardmodel(QObject *parent)
    : QStandardItemModel(parent)
{
    selectcount = 0;
}

Qt::ItemFlags lds_model_sqlstandardmodel::flags(const QModelIndex &index) const
{
    if(bOriginFlagsSet) {
        return QStandardItemModel::flags(index);
    }
    if(bNoItemFlags) {
        return Qt::NoItemFlags;
    }
    if(field_name_name_map.value(headerData(index.column(), Qt::Horizontal).toString(), false)) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void lds_model_sqlstandardmodel::setQuery(const QString &select_str_arg)
{
    this->select_str = this->select_str_arg = select_str_arg;
}

void lds_model_sqlstandardmodel::execQuery()
{
    lds_query_db query(lds_query_thread::getThreadDbAndOpen());
    query.execSelect(select_str);
    qDebug() << this << "selectcount:" << ++selectcount;
    //horizontal
    if(this->columnCount() == 0) {
        int qcount = query.recordCount();
        QStringList headerlabels;
        for(int k= 0; k < qcount; k++) {
            headerlabels << query.record().fieldName(k);
        }
        this->setHorizontalHeaderLabels(headerlabels);
    }
    const int qcount = query.recordCount();
    int del_row_begin = 0;
    const int del_row_count = this->rowCount();
    while(query.next()) {
        int row;
        if(del_row_begin < del_row_count) {
            row =del_row_begin;
            del_row_begin ++;
        } else {
            row = this->rowCount();
            QList<QStandardItem *> items;
            for(int k= 0; k < qcount; k++) {
                items << new QStandardItem;
            }
            QStandardItemModel::appendRow(items);
        }
        //
        for(int k = 0; k < qcount; k++) {
            this->item(row, k)->setData(query.recordValue(k), Qt::EditRole);
        }
    }

    //removeRows
    this->removeRows(del_row_begin, del_row_count - del_row_begin);
}

void lds_model_sqlstandardmodel::execQuery(const QStringList &args)
{
    select_str = select_str_arg;
    foreach(const QString &arg, args) {
        select_str = select_str.arg(arg);
    }
    execQuery();
}

void lds_model_sqlstandardmodel::execQuery(const QString &arg1)
{
    execQuery(QStringList() << arg1);
}

void lds_model_sqlstandardmodel::execQuery(const QString &arg1, const QString &arg2)
{
    execQuery(QStringList() << arg1 << arg2);
}

QString lds_model_sqlstandardmodel::lastQuery()
{
    return select_str;
}

int lds_model_sqlstandardmodel::fieldIndex(const QString &field_name)
{
    for(int k = 0; k < this->columnCount(); k ++)    {
        if(headerData(k, Qt::Horizontal).toString().compare(field_name, Qt::CaseInsensitive) == 0) {
            return k;
        }
    }
    return -1;
}

void lds_model_sqlstandardmodel::save_set_header(const QString &field_name, bool iseditable)
{
    field_name_name_map.insert(field_name, iseditable);
}

QVariant lds_model_sqlstandardmodel::model_data(int row, const QString &field_name, int role)
{
    QModelIndex index = this->index(row, fieldIndex(field_name));
    if(index.isValid()) {
        return data(index, role);
    } else {
        return QVariant();
    }
}

QVariant lds_model_sqlstandardmodel::model_data(int row, int column, int role)
{
    QModelIndex index = this->index(row, column);
    if(index.isValid()) {
        return data(index, role);
    } else {
        return QVariant();
    }
}

bool lds_model_sqlstandardmodel::model_data_set(int row, int column, const QVariant &value, int role)
{
    return setData(this->index(row, column), value, role);
}

bool lds_model_sqlstandardmodel::model_data_set(int row, const QString &field_name, const QVariant &value, int role)
{
    return model_data_set(row, this->fieldIndex(field_name), value, role);
}

QStandardItem *lds_model_sqlstandardmodel::newItem(const QVariant &value, int role)
{
    QStandardItem *item = new QStandardItem;
    item->setData(value, role);
    return item;
}

QStandardItem *lds_model_sqlstandardmodel::newItem(const QIcon &icon, const QVariant &value, int role)
{
    QStandardItem *item = new QStandardItem;
    item->setData(value, role);
    item->setIcon(icon);
    return item;
}

void lds_model_sqlstandardmodel::setItemData(QStandardItem *item, const QVariant &value, int role)
{
    item->setData(value, role);
}

void lds_model_sqlstandardmodel::enableNoItemFlags()
{
    bNoItemFlags = true;
}

void lds_model_sqlstandardmodel::restoreOriginFlagsSet()
{
    bOriginFlagsSet = true;
}

void lds_model_sqlstandardmodel::appendStringList(const QStringList &list)
{
    QList<QStandardItem*> items;
    foreach(const QString &str, list) {
        items << new QStandardItem(str);
    }
    appendRow(items);
}
