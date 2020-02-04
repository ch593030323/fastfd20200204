#ifndef MATERIAL_PURCHASE_APPLY_H
#define MATERIAL_PURCHASE_APPLY_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"
//采购申请表
namespace Ui {
class material_purchase_apply;
}

class material_purchase_apply : public QDialog
{
    Q_OBJECT

public:
    explicit material_purchase_apply(QWidget *parent = 0);
    ~material_purchase_apply();
private slots:
    void to_update_tableview_detail(int row);

    void toaudit();//ch_state : Y
    void toexit();
    void todel();
    void tonew();//ch_state : N
    void torefresh();

    void tochange();
private:
    bool billnoIsAduitedbyYun(const QString &billno);

    void billnApp(const QString &BILL_NO);
private:
    Ui::material_purchase_apply *ui;
    lds_model_sqltablemodel *tablemodel_master;
    lds_model_sqltablemodel *tablemodel_detail;
};

#endif // MATERIAL_PURCHASE_APPLY_H
