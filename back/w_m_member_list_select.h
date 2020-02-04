#ifndef W_M_MEMBER_LIST_SELECT_H
#define W_M_MEMBER_LIST_SELECT_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"

namespace Ui {
class w_m_member_list_select;
}

class w_m_member_list_select : public QDialog
{
    Q_OBJECT

public:
    explicit w_m_member_list_select(QWidget *parent = 0);
    ~w_m_member_list_select();
    QString ret_m;
public slots:
    void torefresh();
private slots:
    void took();
    void tocancel();
private:
    Ui::w_m_member_list_select *ui;
    lds_model_sqltablemodel *tablemodel;
};

#endif // W_M_MEMBER_LIST_SELECT_H
