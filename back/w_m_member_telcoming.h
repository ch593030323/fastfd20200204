#ifndef W_M_MEMBER_TELCOMING_H
#define W_M_MEMBER_TELCOMING_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"
#include <QStandardItemModel>

namespace Ui {
class w_m_member_telComing;
}

class w_m_member_telComing : public QDialog
{
    Q_OBJECT

public:
    explicit w_m_member_telComing(bool isQ000, const QString &vch_tel, QWidget *parent = 0);
    ~w_m_member_telComing();

    QString ret_ch_tableno;
    QString ret_vch_memberno;
    QString ret_tel;
    bool delTelFileRow(int row_tel);
    bool delTelFileTelNo(const QString &tel);
private slots:
    void toaddm();
    void toorder();
    void toexit();

    void torefresh();
    void toTelDel();
    void torefreshM();
    void toQ000_mread();
    void toSaveDelRecord(bool f);
private:
    Ui::w_m_member_telComing *ui;
    lds_model_sqltablemodel *tablemodel;
    QStandardItemModel *model_tel;
    QString vch_tel_for_first;
};

#endif // W_M_MEMBER_TELCOMING_H
