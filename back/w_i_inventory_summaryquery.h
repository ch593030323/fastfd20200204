#ifndef W_I_INVENTORY_SUMMARYQUERY_H
#define W_I_INVENTORY_SUMMARYQUERY_H

#include <QDialog>
#include "w_rpt_report.h"
class QSqlQueryModel;
class Ui_w_i_inventory_summaryquery_Dialog;
class w_i_inventory_summaryquery : public QDialog
{
    Q_OBJECT
public:
    explicit w_i_inventory_summaryquery(QWidget *parent = 0);

private:
    void tab1sqlinit();
private slots:
    void toselect();
    void toexit();
private:
    Ui_w_i_inventory_summaryquery_Dialog*ui;
    QSqlQueryModel *querymodel;
    QString selectsqlpart0;
    QString selectsqlpart1;
    QString selectsqlpart2;
    QMap<QString , w_rpt_report::TABTYPE> tab1info;

};

#endif // W_I_INVENTORY_SUMMARYQUERY_H
