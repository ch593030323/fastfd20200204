#ifndef TAKE_CASHIER_H
#define TAKE_CASHIER_H

#include <QDialog>
#include "sqlquerymodel_linenum1.h"
#include "frontviewdialog_virtual.h"

namespace Ui {
class take_cashier;
}

class take_cashier : public QDialog
{
    Q_OBJECT
    
public:
    explicit take_cashier(QWidget *parent);
    ~take_cashier();
    void init_control();
    int row;
    SqlQueryModel_Linenum1 *model_top;
    QSqlQueryModel *model_bottom;
private slots:
    void on_pushButton_5_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();//付款

    void toselectDish(const QString &ch_billno);
    void toselectDish();
private:
    Ui::take_cashier *ui;
};

#endif // TAKE_CASHIER_H
