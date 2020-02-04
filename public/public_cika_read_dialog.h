#ifndef PUBLIC_CIKA_READ_DIALOG_H
#define PUBLIC_CIKA_READ_DIALOG_H

#include <QDialog>
#include "lds_dialog_input.h"
#include <QSqlQueryModel>

class QStandardItemModel;
class public_cika_read_Dialog : public lds_dialog_input
{
    Q_OBJECT
    
public:
    explicit public_cika_read_Dialog(QWidget *parent = 0);
    ~public_cika_read_Dialog();
    QString getcardno();
    void setmemberlist();
private slots:
    void updatemodel();
    void toapp(const QModelIndex &index);
private:
    QStandardItemModel *standmodel;
    QString _sql;
    QSqlQueryModel *querymodel;
    QString cardno;
};

#endif // PUBLIC_CIKA_READ_DIALOG_H
