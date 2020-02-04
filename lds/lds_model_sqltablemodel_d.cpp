#include "lds_model_sqltablemodel_d.h"
#include <QComboBox>
#include <QtDebug>
/*
        vch_dishname
10010
10011
10012
10013
*/

void lds_model_sqltablemodel_d::fillData(QComboBox *com, const lds_model_sqltablemodel_d *d)
{
    com->clear();
    for(int row = 0; row < d->rowCount(); row ++) {
        com->addItem(d->data(row, 0).toString(), d->map_rowstr.key(row));
    }
}

lds_model_sqltablemodel_d::lds_model_sqltablemodel_d()
{
}

lds_model_sqltablemodel_d::~lds_model_sqltablemodel_d()
{
    clear();
}

bool lds_model_sqltablemodel_d::isExisted(const QString &rowstr, const QString &columnstr) const
{
    return isExistedRow(rowstr) && isExistedColumn(columnstr);
}

bool lds_model_sqltablemodel_d::isExistedRow(const QString &rowstr) const
{
    return map_rowstr.find(rowstr) != map_rowstr.end();
}

bool lds_model_sqltablemodel_d::isExistedColumn(const QString &columnstr) const
{
    return map_columnstr.find(columnstr) != map_columnstr.end();
}

bool lds_model_sqltablemodel_d::isValid(const QString &rowstr, const QString &columnstr) const
{
    return data(rowstr, columnstr).isValid();
}

bool lds_model_sqltablemodel_d::isFill(const QString &rowstr, const QString &columnstr) const
{
    return model.find(modelKey(rowstr, columnstr)) != model.end();
}

QVariant lds_model_sqltablemodel_d::data(const QString &rowstr, const QString &columnstr) const
{
    return model.value(modelKey(rowstr, columnstr));
}

QVariant lds_model_sqltablemodel_d::data(int row, int column) const
{
    return model.value(modelKey(row, column));
}

QVariant lds_model_sqltablemodel_d::data(int row, const QString &columnstr) const
{
    return data(row, map_columnstr.value(columnstr, -1));
}

QVariant lds_model_sqltablemodel_d::data(const QString &rowstr, int column) const
{
    return data(map_rowstr.value(rowstr, -1), column);
}

void lds_model_sqltablemodel_d::setData(const QString &rowstr, const QString &columnstr, const QVariant &value) const
{
    if(!isExistedRow(rowstr)) appendRow(rowstr);
    if(!isExistedColumn(columnstr)) appendColumn(columnstr);

    model.insert(modelKey(rowstr, columnstr), value);
}

//void lds_model_sqltablemodel_d::setData(int row, const QString &columnstr, const QVariant &value) const
//{
//    if(row >= 0
//            && row < rowCount()) {
//        if(!isExistedColumn(columnstr)) appendColumn(columnstr);
//        model.insert(modelKey(row, columnstr), value);
//    }
//}

void lds_model_sqltablemodel_d::appendRow(const QString &rowstr) const
{
    map_rowstr.insert(rowstr, rowCount());
}

void lds_model_sqltablemodel_d::appendColumn(const QString &columnstr) const
{
    map_columnstr.insert(columnstr, columnCount());
}

void lds_model_sqltablemodel_d::clear() const
{
    clearRow();
}

void lds_model_sqltablemodel_d::clearRow() const
{
    model.clear();
    map_rowstr.clear();
}

int lds_model_sqltablemodel_d::rowCount() const
{
    return map_rowstr.count();
}

int lds_model_sqltablemodel_d::columnCount() const
{
    return map_columnstr.count();
}

const QMap<QString, int> &lds_model_sqltablemodel_d::getMapRowstr() const
{
    return map_rowstr;
}

const QMap<QString, int> &lds_model_sqltablemodel_d::getMapColumnstr() const
{
    return map_columnstr;
}

void lds_model_sqltablemodel_d::print() const
{
#ifdef QT_DEBUG
#else
    return;
#endif

    QString line;
    qDebug() << "--------------begin--------------";
    //    line += "[row]\t";
    line += "[Key]\t";
    for(int column = 0; column < columnCount(); column++) {
        line += "["+map_columnstr.key(column)+"]";
        line += "\t";
    }
    qDebug() << line;
    for(int row = 0; row < rowCount(); row++) {
        line = "";
        //        line += QString::number(row + 1) + "\t";
        line += map_rowstr.key(row) + ":\t";
        for(int column = 0; column < columnCount(); column++) {
            if(data(row, column).isValid()) {
                line += data(row, column).toString().replace("\n", "\\n");
            } else {
                line += "null";
            }
            line += "\t";
        }
        qDebug() << line;
    }

    //    qDebug() << "model:" << model;
    //    qDebug() << "map_rowstr:" << map_rowstr;
    //    qDebug() << "map_columnstr:" << map_columnstr;
    qDebug() << "--------------end--------------";
}

QString lds_model_sqltablemodel_d::headerString(int section, Qt::Orientation orientation) const
{
    if(orientation == Qt::Vertical) {
        return map_rowstr.key(section);
    }
    if(orientation == Qt::Horizontal) {
        return map_columnstr.key(section);
    }
    return "";
}

QString lds_model_sqltablemodel_d::key(int row) const
{
    return headerString(row, Qt::Vertical);
}

qrtVariantPairList lds_model_sqltablemodel_d::rowRecord(int row) const
{
    qrtVariantPairList r;
    for(int column = 0; column < columnCount(); column ++) {
        r << qrtVariantPair(headerString(column, Qt::Horizontal), data(row, column));
    }
    return r;
}

qrtVariantPairList lds_model_sqltablemodel_d::rowRecord(const QString &rowstr) const
{
    return rowRecord(map_rowstr.value(rowstr, -1));
}

int lds_model_sqltablemodel_d::indexOf(const qrtVariantPairList &select_list) const
{
    for(int row = 0; row < rowCount(); row ++) {
        int check = 0;
        for(int k = 0; k < select_list.count(); k ++) {
            if(data(row, select_list[k].first) == select_list[k].second) {
                check ++;
            }
        }

        if(check == select_list.count()) {
            return row;
        }
    }
    return -1;
}

int lds_model_sqltablemodel_d::keyIndexOf(const QString &key) const
{
    return map_rowstr.value(key, -1);
}

int lds_model_sqltablemodel_d::findValue(const qrtVariantPairList &select_list) const
{
    for(int row = 0; row < rowCount(); row ++) {
        int check = 0;
        for(int k = 0; k < select_list.count(); k ++) {
            if(data(row, select_list[k].first) == select_list[k].second) {
                check ++;
            }
        }

        if(check == select_list.count()) {
            return row;
        }
    }
    return -1;
}

int lds_model_sqltablemodel_d::findValueCount(const qrtVariantPairList &select_list) const
{
    int count = 0;
    for(int row = 0; row < rowCount(); row ++) {
        int check = 0;
        for(int k = 0; k < select_list.count(); k ++) {
            if(data(row, select_list[k].first) == select_list[k].second) {
                check ++;
            }
        }

        if(check == select_list.count()) {
            count ++;
        }
    }
    return count;
}

QString lds_model_sqltablemodel_d::modelKey(int row, int column) const
{
    return QString("(%1,%2)").arg(row).arg(column);
}

QString lds_model_sqltablemodel_d::modelKey(const QString &rowstr, const QString &columnstr) const
{
    return modelKey(map_rowstr.value(rowstr, -1), map_columnstr.value(columnstr, -1));
}

QString lds_model_sqltablemodel_d::modelKey(int row, const QString &columnstr) const
{
    return modelKey(row, map_columnstr.value(columnstr, -1));
}
