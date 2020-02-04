#ifndef VCH_PRINT_MEMODATA_H
#define VCH_PRINT_MEMODATA_H

#include <QVariant>
#include <QSqlDatabase>

/**
 *套菜的自套菜有做法时，会出现主套菜vch_print_memoData::count() == 0 但是total() != 0的情况，
 * 判断一条商品是否有套菜，一定要两个条件来判断(vch_print_memoData::count() != 0 || num_price_add != 0)
 *
 */
class vch_print_memoData
{
public:
    explicit vch_print_memoData();
    explicit vch_print_memoData(const QString &sql_value);

    void select(const QString &sql_value);
    void select();
    bool next();
    QVariant nextValue(const QString &string);

    QVariant value(int index, const QString &string) const;
    QVariant value(const QString &no, const QString &string) const;
    int count() const;

    QString jsonToString(double num_x = 1, bool with_price = true) const;
    QString sqlValue(double num_x = 1) const;

    void append(const QString &no, const QString &name, double num, double price);
    void append(const vch_print_memoData &d);
    void duplicate(QSqlDatabase db, const QString &no, double num = 1);

    int indexOf(const QString &no) const;
    void setValue(int index, const QString &string, const QVariant &value);
    void setValue(const QString &no, const QString &string, const QVariant &value);
    void fill(const QString &string, const QVariant &value);

    double total() const;
    double num() const;
    void clear();

    void from100110(QSqlDatabase db, const QString &str100110);

    void print();

    vch_print_memoData & sub(const vch_print_memoData &other);
    vch_print_memoData & add(const vch_print_memoData &other);

    static double total(const QString &sql_value);
private:
    QList<QMap<QString, QVariant> > data_rect;
    int data_rect_index;
    void init();
};

#endif // VCH_PRINT_MEMODATA_H
