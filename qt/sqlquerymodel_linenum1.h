#ifndef SQLQUERYMODEL_LINENUM1_H
#define SQLQUERYMODEL_LINENUM1_H


#include <QStringList>
#include <QDebug>

#include <QSqlQueryModel>

class SqlQueryModel_Linenum1:public QSqlQueryModel
{
public:
    SqlQueryModel_Linenum1(QObject *parent = 0);
    void setlist(const QStringList &l);
    QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;
private:
    QStringList list;
};

#endif // SQLQUERYMODEL_LINENUM1_H
