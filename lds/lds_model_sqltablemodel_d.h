#ifndef LDS_MODEL_SQLTABLEMODEL_D_H
#define LDS_MODEL_SQLTABLEMODEL_D_H

#include <QVariant>
#include "lds_query_variant.h"

class QComboBox;
class lds_model_sqltablemodel_d{
public:
    static void fillData(QComboBox *com, const lds_model_sqltablemodel_d* d);
public:
    lds_model_sqltablemodel_d();
    virtual ~lds_model_sqltablemodel_d();

    bool isExisted(const QString &rowstr, const QString &columnstr) const;
    bool isExistedRow(const QString &rowstr) const;
    bool isExistedColumn(const QString &columnstr) const;
    bool isValid(const QString &rowstr, const QString &columnstr) const;
    bool isFill(const QString &rowstr, const QString &columnstr) const;

    QVariant data(const QString &rowstr, const QString &columnstr) const;
    QVariant data(int row, int column) const;
    QVariant data(int row, const QString &columnstr) const;
    QVariant data(const QString &rowstr, int column) const;
    //若isExisted is false， 则插入
    void setData(const QString &rowstr, const QString &columnstr , const QVariant &value) const;
//    void setData(int row, const QString &columnstr , const QVariant &value) const;

    void appendRow(const QString &rowstr) const;
    void appendColumn(const QString &columnstr) const;
    void clear() const;
    void clearRow() const;

    int rowCount() const;
    int columnCount() const;
    const QMap<QString, int> & getMapRowstr() const;
    const QMap<QString, int> & getMapColumnstr() const;
    void print() const;

    QString headerString(int section, Qt::Orientation orientation) const;
    QString key(int row) const;
    qrtVariantPairList rowRecord(int row) const;
    qrtVariantPairList rowRecord(const QString &rowstr) const;

    /// \return return matched row
    int indexOf(const qrtVariantPairList &select_list) const;
    int keyIndexOf(const QString &key) const;

    /// \return compare row, like select count(0) from A where F1 and F2
    int findValue(const qrtVariantPairList &select_list) const;
    int findValueCount(const qrtVariantPairList &select_list) const;
private:
    QString modelKey(int row, int column) const;//(1, 2)
    QString modelKey(const QString &rowstr, const QString &columnstr) const;//(1, 2)
    QString modelKey(int row, const QString &columnstr) const;//(1, 2)
    mutable QMap<QString, QVariant> model;
    mutable QMap<QString, int> map_rowstr;
    mutable QMap<QString, int> map_columnstr;
};

#endif // LDS_MODEL_SQLTABLEMODEL_D_H
