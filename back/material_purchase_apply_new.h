#ifndef MATERIAL_PURCHASE_APPLY_NEW_H
#define MATERIAL_PURCHASE_APPLY_NEW_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"

namespace Ui {
class material_purchase_apply_new;
}

class material_purchase_apply_new : public QDialog
{
    Q_OBJECT

public:
    explicit material_purchase_apply_new(const QString &BILL_NO, QWidget *parent = 0);
    ~material_purchase_apply_new();

private slots:
    void tonew();
    void todel();
    void toexit();

    void torefresh();
    void tosave();
private:
    bool saveData();
    bool isDirty();
private:
    Ui::material_purchase_apply_new *ui;
    lds_model_sqltablemodel *tablemodel_detail;
};

#endif // MATERIAL_PURCHASE_APPLY_NEW_H
