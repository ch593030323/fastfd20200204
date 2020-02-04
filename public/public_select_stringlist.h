#ifndef PUBLIC_SELECT_STRINGLIST_H
#define PUBLIC_SELECT_STRINGLIST_H

#include <QDialog>
#include "ftableview_standmodel_sql_none.h"
#include <QPair>

namespace Ui {
class public_select_stringlist;
}

class public_select_stringlist : public QDialog
{
    Q_OBJECT

public:
    public_select_stringlist(const QStringList &list, QWidget *parent = 0);
    public_select_stringlist(const QList<QPair<QString, QString> > &pairlist, QWidget *parent = 0);
    public_select_stringlist(const QList<ftableview_standmodel_sql_none::noneData> &list_data, QWidget *parent = 0);
    ~public_select_stringlist();

    QVariant curSelect(int role);

    void setContentSize(int rowcount, int columncount);
private slots:
    void took();

private:
    void init();
private:
    Ui::public_select_stringlist *ui;
    ftableview_standmodel_sql_none *cook_model;
};

#endif // PUBLIC_SELECT_STRINGLIST_H
