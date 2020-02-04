#ifndef LDS_MODEL_SQLSTANDARDMODEL_H
#define LDS_MODEL_SQLSTANDARDMODEL_H

#include <QStandardItemModel>
#include "lds_query_db.h"
#include "lds_bool.h"

class lds_model_sqlstandardmodel : public QStandardItemModel
{
public:
    lds_model_sqlstandardmodel(QObject *parent);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    void setQuery(const QString &select_str_arg);
    void execQuery();
    void execQuery(const QStringList &args);
    void execQuery(const QString &arg1);
    void execQuery(const QString &arg1, const QString &arg2);

    QString lastQuery();

    int fieldIndex(const QString &field_name);

    void save_set_header(const QString &field_name,  bool iseditable = false);

    QVariant model_data(int row, const QString &field_name, int role = Qt::DisplayRole);
    QVariant model_data(int row, int column, int role = Qt::DisplayRole);
    bool model_data_set(int row, int column, const QVariant &value, int role = Qt::DisplayRole);
    bool model_data_set(int row, const QString &field_name, const QVariant &value, int role = Qt::DisplayRole);

    static QStandardItem *newItem(const QVariant &value, int role = Qt::DisplayRole);
    static QStandardItem *newItem(const QIcon &icon, const QVariant &value, int role = Qt::DisplayRole);
    static void setItemData(QStandardItem *item, const QVariant &value, int role = Qt::DisplayRole);

    void enableNoItemFlags();
    void restoreOriginFlagsSet();

    void appendStringList(const QStringList &list);
private:
    QString select_str_arg;
    QString select_str;

    QMap<QString, bool> field_name_name_map;

    int selectcount;
    lds_bool_default_false bNoItemFlags;
    lds_bool_default_false bOriginFlagsSet;
};

#endif // LDS_MODEL_SQLSTANDARDMODEL_H
