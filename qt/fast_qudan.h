#ifndef FAST_QUDAN_H
#define FAST_QUDAN_H

#include <QDialog>
#include <QStandardItemModel>
#include "lds_model_sqltablemodel.h"

#include "fexpandmain_model_sqltablemodel_data.h"

namespace Ui {
class fast_qudan;
}

class fast_qudan : public QDialog
{
    Q_OBJECT
    
public:
    explicit fast_qudan(QWidget *parent);
    ~fast_qudan();
    int int_flowid;
    static bool pendTransaction(QString &errstring, fexpandmain_model_sqltablemodel_data *tablemodel, int &int_flowid_return);
    static bool take(QString &errstring, fexpandmain_model_sqltablemodel_data *tablemodel, int int_flowid);
    static void printTakebill(fexpandmain_model_sqltablemodel_data *tablemodel);
    static QString right_justified(const QString &in, int width);
    static QString left_justified(const QString &in, int width);
private slots:
    void tocancel();
    void took();

    void toupdate2();
    void todel();

private:
    Ui::fast_qudan *ui;
    QSqlQueryModel *querymodel;
    QSqlQueryModel *querymodel_2;
    QString sql2;
};

#endif // FAST_QUDAN_H
