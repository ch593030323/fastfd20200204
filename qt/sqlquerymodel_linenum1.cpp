#include "sqlquerymodel_linenum1.h"

SqlQueryModel_Linenum1::SqlQueryModel_Linenum1(QObject *parent):QSqlQueryModel(parent) {
    list.clear();}

QVariant  SqlQueryModel_Linenum1::headerData(int section, Qt::Orientation orientation,int role) const
{
    if(list.isEmpty()) {
        return QSqlQueryModel::headerData(section, orientation, role);
    }
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal && section < list.count()) {
        return list.value(section);
    }

    return QSqlQueryModel::headerData(section, orientation, role);
}

QVariant SqlQueryModel_Linenum1::data(const QModelIndex &item, int role) const{
    return QSqlQueryModel::data(item, role);
}

void SqlQueryModel_Linenum1::setlist(const QStringList &l)
{
    list = l;
}

