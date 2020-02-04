#ifndef W_M_MEMBER_PARAMSET2_H
#define W_M_MEMBER_PARAMSET2_H

#include <QDialog>
#include "ftableview_standmodel_sql_none.h"

namespace Ui {
class w_m_member_paramset2;
}

class w_m_member_paramset2 : public QDialog
{
    Q_OBJECT

public:
    explicit w_m_member_paramset2(QWidget *parent = 0);
    ~w_m_member_paramset2();

private slots:
    void toUpdateSection(const QModelIndex &index);
    void toisrecharge(bool flag);

    void took();
    void tocancel();

    void inputmemberno(int index);
    void tomclearall();
    void toicpwddef();
private:
    bool opData(bool issave = false);
private:
    Ui::w_m_member_paramset2 *ui;
    ftableview_standmodel_sql_none *backlist_model;
};

#endif // W_M_MEMBER_PARAMSET2_H
