#ifndef MATERIAL_LOSS_APPLY_H
#define MATERIAL_LOSS_APPLY_H

#include <QDialog>
#include <QModelIndex>

class lds_model_sqltablemodel;
class QDataWidgetMapper;
class Ui_material_purchase_apply;
namespace Ui {
class material_loss_apply;
}

class material_loss_apply : public QDialog
{
    Q_OBJECT

public:
    explicit material_loss_apply(QWidget *parent = 0);
    ~material_loss_apply();

private slots:
    void to_update_tableview_detail(int row);

    void toaudit();
    void toexit();
    void torefresh();
private:
    void getDataFromYun();
    bool billnoIsAduitedbyYun(const QString &billno);
private:
    Ui_material_purchase_apply *ui;

    lds_model_sqltablemodel *tablemodel_master;
    lds_model_sqltablemodel *tablemodel_detail;
};

#endif // MATERIAL_LOSS_APPLY_H
