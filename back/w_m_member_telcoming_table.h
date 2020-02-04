#ifndef W_M_MEMBER_TELCOMING_TABLE_H
#define W_M_MEMBER_TELCOMING_TABLE_H

#include <QDialog>
#include "ftableview_standmodel_sql_table.h"

namespace Ui {
class w_m_member_telComing_table;
}

class w_m_member_telComing_table : public QDialog
{
    Q_OBJECT
public:
    //w_m_member_telComing_table::telComingData
    struct telComingData{
        QString vch_memberno;
        QString vch_tel;
    };

public:
    explicit w_m_member_telComing_table(const telComingData &member_from_tel, QWidget *parent = 0);
    ~w_m_member_telComing_table();

    QString ret_ch_tableno;
private slots:
    void totabletype(int index);

    void toexit();
    void toopen(const QModelIndex &index);
private:
    Ui::w_m_member_telComing_table *ui;
    ftableview_standmodel_sql_table *modeltable;
    telComingData member_from_tel;
};

#endif // W_M_MEMBER_TELCOMING_TABLE_H
