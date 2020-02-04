#ifndef W_SYS_MANAGE_OPERATORRIGHT_RIGHT_H
#define W_SYS_MANAGE_OPERATORRIGHT_RIGHT_H

#include <QDialog>
#include "lds_model_sqlstandardmodel.h"
#include "ldsvariantmap.h"
namespace Ui {
class w_sys_manage_operatorright_right;
}
class w_sys_manage_operatorright_right : public QDialog
{
    Q_OBJECT

public:
    explicit w_sys_manage_operatorright_right(const ldsStringMap &map, QWidget *parent = 0);
    ~w_sys_manage_operatorright_right();

private slots:
    void torefresh();
    void toexit();
    void toupdateOper(const QModelIndex &index);
    void tooper(const QModelIndex &index);
    void toselectAll();
    void toclearAll();
private:
    void updateOper(const QString &ch_areano, int oper_row);
private:
    Ui::w_sys_manage_operatorright_right *ui;
    lds_model_sqlstandardmodel *model_oper;
    lds_model_sqlstandardmodel *model_area;
private:
    QMap<QString, QString> table_map;
};


#endif // W_SYS_MANAGE_OPERATORRIGHT_RIGHT_H
