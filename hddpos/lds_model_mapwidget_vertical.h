#ifndef LDS_MODEL_MAPWIDGET_VERTICAL_H
#define LDS_MODEL_MAPWIDGET_VERTICAL_H

#include <QDialog>
#include <QDataWidgetMapper>
#include "lds_model_sqltablemodel.h"

class lds_model_mapwidget_vertical : public QDialog
{
public:
    lds_model_mapwidget_vertical(QWidget *parent);

    bool commitData(QString *errstring = 0);
protected:
    void scan_all_init_value(const QString &table_name, const QString &sql_key_name, const QString sql_value_name, const QString &filter);
    void scan_all_objectname_add_map_setFilter(const QString &sql_key_name, const QString &filter);

    //不存在则插入空值
    virtual bool insert_sql_objectname(const QString &objectname);
    virtual void mappedSetCurrentIndex(int index);

    int mappedSection(QWidget *editor);
    bool mappedSubmit();

    QVariant model_data(const QString &key_name);
    bool model_data_set(const QString &key_name, const QVariant &value);
private:
    void scan_one(QObject *o, const QString &filter);
    QMap<QString, QWidget *> map;
    QString table_name;
    QString sql_key_name;
    QString sql_value_name;
    QString filter;
    QDataWidgetMapper *mapper;
public:
    lds_model_sqltablemodel *tablemodel;
};
#endif // LDS_MODEL_MAPWIDGET_VERTICAL_H
