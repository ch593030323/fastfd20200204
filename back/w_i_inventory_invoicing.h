#ifndef W_I_INVENTORY_INVOICING_H
#define W_I_INVENTORY_INVOICING_H

#include <QDialog>
#include "w_rpt_report.h"
class QSqlQueryModel;
class QStandardItemModel;
class Ui_w_i_inventory_Invoicing_Dialog;
class w_i_inventory_Invoicing : public QDialog
{
    Q_OBJECT
public:
    explicit w_i_inventory_Invoicing(QWidget *parent = 0);
    
private slots:
    void toselect();
    void toexit();
public slots:
private:
    Ui_w_i_inventory_Invoicing_Dialog *ui;
    QStandardItemModel *standmodel;
    
};

#endif // W_I_INVENTORY_INVOICING_H
