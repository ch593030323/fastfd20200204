#include "lds_model_mapwidget_vertical.h"
#include <QtDebug>
#include "lds.h"
#include "lds_query_hddpos.h"

lds_model_mapwidget_vertical::lds_model_mapwidget_vertical(QWidget *parent) : QDialog(parent)
{
    tablemodel = 0;
    mapper = 0;
}

bool lds_model_mapwidget_vertical::commitData(QString *errstring)
{
    if(tablemodel == 0) {
        if(errstring) *errstring = "tablemodel is null";
        return false;
    }
    bool f =  tablemodel->trySubmitAll(errstring);
    mapper->setCurrentIndex(1);
    return f;
}

void lds_model_mapwidget_vertical::scan_all_init_value(const QString &table_name, const QString &sql_key_name, const QString sql_value_name, const QString &filter)
{
    this->table_name = table_name;
    this->sql_key_name = sql_key_name;
    this->sql_value_name = sql_value_name;
    this->filter = filter;

    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable(table_name);

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tablemodel);
    mapper->setOrientation(Qt::Vertical);

    scan_all_objectname_add_map_setFilter(sql_key_name, filter);
    mappedSetCurrentIndex(1);
}

void lds_model_mapwidget_vertical::scan_all_objectname_add_map_setFilter(const QString &sql_key_name, const QString &filter)
{
    map.clear();
    mapper->clearMapping();
    //1
    tablemodel->setFilter(sql_key_name + " like '" + filter + "%' ");
    scan_one(this, filter);
    //2
    for(int row = 0, rowcount = tablemodel->rowCount(); row < rowcount; row++) {
        QMap<QString, QWidget *>::iterator itr = map.find(tablemodel->model_data(row, "vch_parameter").toString());
        qDebug() << __LINE__ << tablemodel->model_data(row, "vch_parameter").toString();
        if(itr != map.end()) {
            if(itr.value()->inherits("QTextEdit")) {
                mapper->addMapping(itr.value(), row, "plainText");
            } else if(itr.value()->inherits("lds_ComboBox")) {
                mapper->addMapping(itr.value(), row, "comusrdata");
            } else if(itr.value()->inherits("lds_timeEdit")) {
                mapper->addMapping(itr.value(), row, "time");
            } else if(itr.value()->inherits("lds_datetimeEdit")) {
                mapper->addMapping(itr.value(), row, "dateTime");
            } else if(itr.value()->inherits("QCheckBox")) {
                mapper->addMapping(itr.value(), row, "checked");
            } else {
                mapper->addMapping(itr.value(), row);
            }
            map.take(itr.key());
        }
    }
    //3
    if(map.count() > 0) {
        QMapIterator<QString, QWidget *> mapitr(map);
        while(mapitr.hasNext()) {
            mapitr.next();
            insert_sql_objectname(mapitr.key());
        }
        scan_all_objectname_add_map_setFilter(sql_key_name, filter);
    }
}

bool lds_model_mapwidget_vertical::insert_sql_objectname(const QString &objectname)
{
    lds_query_hddpos  query;
    return query.execInsert(table_name, qrtVariantPairList()
                            << qrtVariantPair(sql_key_name, objectname)
                            << qrtVariantPair(sql_value_name, ""));
}

void lds_model_mapwidget_vertical::mappedSetCurrentIndex(int index)
{
    mapper->setCurrentIndex(index);
}

int lds_model_mapwidget_vertical::mappedSection(QWidget *editor)
{
    return mapper->mappedSection(editor);
}

bool lds_model_mapwidget_vertical::mappedSubmit()
{
    return mapper->submit();
}

QVariant lds_model_mapwidget_vertical::model_data(const QString &key_name)
{
    if(tablemodel) {
        for(int row = 0; row < tablemodel->rowCount(); row ++) {
            if(tablemodel->model_data(row, sql_key_name).toString() == key_name) {
                return  tablemodel->model_data(row, sql_value_name);
            }
        }
    }
    return QVariant();
}

bool lds_model_mapwidget_vertical::model_data_set(const QString &key_name, const QVariant &value)
{
    if(tablemodel) {
        for(int row = 0; row < tablemodel->rowCount(); row ++) {
            if(tablemodel->model_data(row, sql_key_name).toString() == key_name) {
                return  tablemodel->model_data_set(row, sql_value_name, value);
            }
        }
    }
    return false;
}

void lds_model_mapwidget_vertical::scan_one(QObject *o, const QString &filter)
{
    if(o) {
        foreach(QObject *oo, o->children()) {
            scan_one(oo, filter);
            if(oo->objectName().contains(filter)) {
                map.insert(oo->objectName(), qobject_cast<QWidget *>(oo));
            }
        }
    }
}

