#include "lds_tablewidget.h"
#include <QHeaderView>
#include <QtDebug>

lds_tablewidget::lds_tablewidget(QWidget *parent) :
    QTableWidget(parent)
{
}

int lds_tablewidget::fieldIndex(const QString &key) const
{
    for(int k = 0; k < horizontalHeaderList.count(); k ++) {
        if(horizontalHeaderList[k].first == key) {
            return k;
        }
    }
    return -1;
}

void lds_tablewidget::appHorizontalHeader(const QString &key, const QString &text, bool show)
{
    int column = horizontalHeaderList.count();
    setColumnCount(column + 1);
    this->setHorizontalHeaderItem(column, new QTableWidgetItem(text));
    horizontalHeaderList << qMakePair(key, text);
    setColumnHidden(column, !show);
}

QModelIndex lds_tablewidget::index(int row, const QString &key)
{
    return index(row, fieldIndex(key));
}

QModelIndex lds_tablewidget::index(int row, int column)
{
    return this->model()->index(row, column);
}

void lds_tablewidget::setNumber(int row, const QString &key, int number)
{
    int column = fieldIndex(key);
    QTableWidgetItem *itm = item(row, column);
    if(itm == 0) {
        setItem(row, column, itm = new QTableWidgetItem);
    }
    itm->setData(Qt::DisplayRole, number);
}

void lds_tablewidget::setText(int row, const QString &key, const QString &text)
{
    setText(row, fieldIndex(key), text);
}

void lds_tablewidget::setText(int row, int column, const QString &text)
{
    QTableWidgetItem *itm = item(row, column);
    if(itm == 0) {
        setItem(row, column, new QTableWidgetItem(text));
    } else {
        itm->setText(text);
    }
}

QVariant lds_tablewidget::data(int row, const QString &key, int role)
{
    return data(row, fieldIndex(key), role);
}

QVariant lds_tablewidget::data(int row, int column, int role)
{
    QTableWidgetItem *itm = item(row, column);
    if(itm != 0) {
        return itm->data(role);
    }
    return QVariant();
}

int lds_tablewidget::number(int row, const QString &key) const
{
    QTableWidgetItem *itm = item(row, fieldIndex(key));
    if(itm != 0) {
        return itm->data(Qt::DisplayRole).toInt();
    }
    return -1;
}

QString lds_tablewidget::text(int row, const QString &key) const
{
    return text(row, fieldIndex(key));
}

QString lds_tablewidget::text(int row, int column) const
{
    QTableWidgetItem *itm = item(row, column);
    if(itm == 0) {
        itm = new QTableWidgetItem;
        ((lds_tablewidget *)this)->setItem(row, column, itm);
    }
    return itm->text();
}

int lds_tablewidget::indexOf(const QString &fieldName, const QString &fieldValue) const
{
    for(int row = 0; row < this->rowCount(); row ++) {
        if(text(row, fieldName) == fieldValue) {
            return row;
        }
    }
    return -1;
}

void lds_tablewidget::setFlags(int row, const QString &key, Qt::ItemFlags flags)
{
    int column = fieldIndex(key);
    QTableWidgetItem *itm = item(row, column);
    itm->setFlags(flags);
}

QStringList lds_tablewidget::fieldNameList()
{
    QStringList lst;
    for(int k = 0;  k < horizontalHeaderList.count(); k ++) {
        lst << horizontalHeaderList[k].first;
    }
    return lst;
}

void lds_tablewidget::setContentSize(int rowcount, int columncount)
{
    while(this->rowCount() > 0) this->removeRow(0);
    while(this->columnCount() > 0) this->removeColumn(0);

    setRowCount(rowcount);
    setColumnCount(columncount);
    for(int row = 0; row < rowcount; row ++) {
        for(int column = 0; column < columncount; column ++) {
            this->setItem(row, column, new QTableWidgetItem);
        }
    }
}

int lds_tablewidget::count()
{
    return this->rowCount() * this->columnCount();
}

void lds_tablewidget::updateAllIndex()
{
    for(int row = 0; row < rowCount(); row ++) {
        for(int column = 0; column < columnCount(); column ++) {
            update(index(row, column));
        }
    }
}

void lds_tablewidget::setWidget(int row, const QString &key, QWidget *b)
{
    QModelIndex idx = index(row, key);
    if(false == idx.isValid())
        return;
    setIndexWidget(idx, b);
}

void lds_tablewidget::removeWidget(int row, const QString &key)
{
    QModelIndex idx = index(row, key);
    if(false == idx.isValid())
        return;
    QWidget *w = indexWidget(idx);
    if(w == 0)
        return;
    //   If index widget A is replaced with index widget B, index widget A will be deleted
    setIndexWidget(idx, 0);
}
