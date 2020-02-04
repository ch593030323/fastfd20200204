#ifndef W_I_INVENTORY_CHECK_H
#define W_I_INVENTORY_CHECK_H

#include <QDialog>
#include "w_rpt_report.h"
#include "w_rpt_report_frame.h"

class QSqlQueryModel;
class QStandardItemModel;
class Ui_w_i_inventory_check_Dialog;

class w_i_inventory_check : public QDialog
{
    Q_OBJECT
public:
    class QueryModel:public QSqlQueryModel{
    public:
        explicit QueryModel(QObject *parent = 0):QSqlQueryModel(parent) {}
        QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;

    };
public:
    explicit w_i_inventory_check(QWidget *parent = 0);
private slots:
    void toselect();
    void toexit();
public slots:
private:
    Ui_w_i_inventory_check_Dialog *ui;
    QSqlQueryModel *querymodel;
    QString selectsql;
};

#endif // W_I_INVENTORY_CHECK_H
