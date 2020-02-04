#include "vch_print_memodata.h"
#include <QStringList>
#include <QtDebug>
#include "lds_query_db.h"

vch_print_memoData::vch_print_memoData()
{
    init();
}

vch_print_memoData::vch_print_memoData(const QString &sql_value)
{
    select(sql_value);
}

void vch_print_memoData::select(const QString &sql_value)
{
    init();
    int k = 0;
    foreach(const QString &d, sql_value.split("\t")) {
        if(k % 4 == 0) {
            data_rect.append(QMap<QString, QVariant>());
            data_rect.last().insert("no", d);
        }
        if(k % 4 == 1) {
            data_rect.last().insert("name", d);
        }
        if(k % 4 == 2) {
            data_rect.last().insert("num", d.toDouble());
        }
        if(k % 4 == 3) {
            data_rect.last().insert("price", d.toDouble());
        }
        k ++;
    }

    if(data_rect.last().count() != 4)
        data_rect.removeLast();
}

void vch_print_memoData::select()
{
    data_rect_index = -1;
}

bool vch_print_memoData::next()
{
    data_rect_index ++;
    return data_rect_index < count();
}

QVariant vch_print_memoData::nextValue(const QString &string)
{
    return value(data_rect_index, string);
}

QVariant vch_print_memoData::value(int index, const QString &string) const
{
    return data_rect.value(index).value(string);
}

QVariant vch_print_memoData::value(const QString &no, const QString &string) const
{
    return value(indexOf(no), string);
}

int vch_print_memoData::count() const
{
    return data_rect.count();
}

QString vch_print_memoData::jsonToString(double num_x, bool with_price) const
{
    QString desc;
    QString name;
    double price;
    double num;

    for(int k = 0; k < count(); k++) {
        if(value(k, "num").toDouble() == 0)
            continue;

        name = value(k, "name").toString();
        price = value(k, "price").toDouble();
        num = value(k, "num").toDouble() * num_x;

        desc += name;

        if(price != 0.00 && with_price)
            desc += "(" + QString::number(price) + ")";

        if(num != 1.00)
            desc += "x" + QString::number(num);

        desc += ",";
    }
    desc.chop(1);
    return desc;
}

QString vch_print_memoData::sqlValue(double num_x) const
{
    QString ret;
    for(int k = 0; k < count(); k++) {
        if(value(k, "num").toDouble() == 0)
            continue;
        ret += value(k, "no").toString() + "\t";
        ret += value(k, "name").toString() + "\t";
        ret += QString::number(value(k, "num").toDouble() * num_x) + "\t";
        ret += value(k, "price").toString() + "\t";
    }
    return ret;
}

void vch_print_memoData::append(const QString &no, const QString &name, double num, double price)
{
    data_rect.append(QMap<QString, QVariant>());

    data_rect.last().insert("no", no);
    data_rect.last().insert("name", name);
    data_rect.last().insert("price", price);
    data_rect.last().insert("num", num);
}

void vch_print_memoData::append(const vch_print_memoData &d)
{
    for(int k = 0; k < d.count(); k ++) {
        data_rect.append(QMap<QString, QVariant>());

        data_rect.last().insert("no", d.value(k, "no"));
        data_rect.last().insert("name", d.value(k, "name"));
        data_rect.last().insert("price", d.value(k, "price"));
        data_rect.last().insert("num", d.value(k, "num"));
    }
}

void vch_print_memoData::duplicate(QSqlDatabase db, const QString &no, double num)
{
    int index = indexOf(no);
    if(index == -1) {
        lds_query_db  query(db);
        query.execSelect(QString(" select num_add_price, vch_cp_memo from cey_bt_cp_memo where ch_no = '%1' ").arg(no));
        query.next();
        double price = query.recordValue("num_add_price").toDouble();
        QString name= query.recordValue("vch_cp_memo").toString();

        append(no, name, num, price);
    } else {
        setValue(index, "num", value(index, "num").toDouble() + num);
    }
}

int vch_print_memoData::indexOf(const QString &no) const
{
    for(int k = 0; k < count(); k++) {
        if(no == value(k, "no").toString()) {
            return k;
        }
    }
    return -1;
}

void vch_print_memoData::setValue(int index, const QString &string, const QVariant &value)
{
    if(index >= 0 && index < data_rect.count()) {
        data_rect[index].insert(string, value);
    }
}

void vch_print_memoData::setValue(const QString &no, const QString &string, const QVariant &value)
{
    setValue(indexOf(no), string, value);
}

void vch_print_memoData::fill(const QString &string, const QVariant &value)
{
    for(int k = 0; k < count(); k++) {
        setValue(k, string, value);
    }
}

double vch_print_memoData::total() const
{
    double ret = 0;
    for(int k = 0; k < count(); k++) {
        ret += value(k, "num").toDouble() * value(k, "price").toDouble();
    }
    return ret;
}

double vch_print_memoData::num() const
{
    double n = 0;
    for(int k = count() - 1; k >= 0; k--) {
        n += value(k, "num").toDouble();
    }

    return n;
}

void vch_print_memoData::clear()
{
    init();
}

void vch_print_memoData::from100110(QSqlDatabase db, const QString &str100110)
{
    clear();
    for(int k = 0; k < str100110.count(); k += 3) {
        duplicate(db, str100110.mid(k, 3));
    }
}

void vch_print_memoData::print()
{
    qDebug() << jsonToString();
}

vch_print_memoData &vch_print_memoData::sub(const vch_print_memoData &other)
{
    for(int k = 0; k < count(); k++) {
        QString no = value(k, "no").toString();
        int other_index = other.indexOf(no);
        if(other_index >= 0)
            setValue(k, "num", value(k, "num").toDouble() - other.value(other_index, "num").toDouble());
    }
    return *this;
}

vch_print_memoData &vch_print_memoData::add(const vch_print_memoData &other)
{
    for(int k = 0; k < count(); k++) {
        QString no = value(k, "no").toString();
        int other_index = other.indexOf(no);
        if(other_index >= 0)
            setValue(k, "num", value(k, "num").toDouble() + other.value(other_index, "num").toDouble());
    }
    return *this;
}

double vch_print_memoData::total(const QString &sql_value)
{
    vch_print_memoData d(sql_value);
    return d.total();
}

void vch_print_memoData::init()
{
    data_rect.clear();
    data_rect_index = -1;
}
