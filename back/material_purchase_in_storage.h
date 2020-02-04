#ifndef MATERIAL_PURCHASE_IN_STORAGE_H
#define MATERIAL_PURCHASE_IN_STORAGE_H

#include <QDialog>

class lds_model_sqltablemodel;
class QDataWidgetMapper;
class Ui_material_purchase_apply;
namespace Ui {
class material_purchase_in_storage;
}

class material_purchase_in_storage : public QDialog
{
    Q_OBJECT

public:
    explicit material_purchase_in_storage(QWidget *parent = 0);
    ~material_purchase_in_storage();
private slots:
    void to_update_tableview_detail(int row);

    void toaudit();
    void to_exit();
    void torefresh();
private:
    void getDataFromYun();
    bool billnoIsAduitedbyYun(const QString &billno);
public:
    static bool local_inventory_in_storage(QWidget *parent, const QString &billNo, const QString &yun_table_master, const QString &yun_table_detail, const QString &key);
private:
    Ui_material_purchase_apply *ui;

    lds_model_sqltablemodel *tablemodel_master;
    lds_model_sqltablemodel *tablemodel_detail;

    struct inventory_in_Data{
        float num0;
        float num1;
        QString dishname;
    };
};

#endif // MATERIAL_PURCHASE_IN_STORAGE_H
