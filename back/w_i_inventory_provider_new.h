#ifndef W_I_INVENTORY_PROVIDER_NEW_H
#define W_I_INVENTORY_PROVIDER_NEW_H

#include <QDialog>
#include <QMap>
#include "lds_query_variant.h"

class Ui_w_i_inventory_provider_new;
class lds_model_sqltablemodel;

class controlViewMap{
public:
    struct objData{
        QObject *obj;
        QByteArray propertyNam;
    };
public:
    controlViewMap() {}
    void insert(const QString &fieldName, QObject *obj, const QByteArray &propertyName);
    QVariant value(const QString &fieldName);
    void setValue(const QString &fieldName, const QVariant &value);
    void setValue(const lds_model_sqltablemodel *m, int row);
    void setKey(const QString &fieldName);

    qrtVariantPairList updateBindValues();
    QString updateWhere();

    qrtVariantPairList insertBindValues();
private:
    QMap<QString, objData> obj_map;
    QString key_fieldName;
};

class w_i_inventory_provider_new : public QDialog
{
    Q_OBJECT

public:
    static void initControlMap(const lds_model_sqltablemodel *tablemodel, Ui_w_i_inventory_provider_new *ui, controlViewMap *control_map);
    static bool check(controlViewMap *control_map, QString &errstring);
public:
    explicit w_i_inventory_provider_new(const lds_model_sqltablemodel *tablemodel, QWidget *parent = 0);
    ~w_i_inventory_provider_new();

private slots:
    void took();
    void tocancel();
private:
    Ui_w_i_inventory_provider_new *ui;
    controlViewMap control_map;
};

class w_i_inventory_provider_modify : public QDialog
{
    Q_OBJECT

public:
    explicit w_i_inventory_provider_modify(const lds_model_sqltablemodel *tablemodel,  int row, QWidget *parent = 0);
    ~w_i_inventory_provider_modify();

private slots:
    void took();
    void tocancel();
private:
    Ui_w_i_inventory_provider_new *ui;
    controlViewMap control_map;
};

#endif // W_I_INVENTORY_PROVIDER_NEW_H
