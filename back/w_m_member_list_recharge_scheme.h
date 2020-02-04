#ifndef W_M_MEMBER_LIST_RECHARGE_SCHEME_H
#define W_M_MEMBER_LIST_RECHARGE_SCHEME_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class w_m_member_list_recharge_scheme;
}

class w_m_member_list_recharge_scheme : public QDialog
{
    Q_OBJECT

public:
    explicit w_m_member_list_recharge_scheme(QWidget *parent = 0);
    ~w_m_member_list_recharge_scheme();

private slots:
    void took();
    void tocancel();

private:
    QStandardItem *newFieldItem(const QString &field_name);
    QVariant model_data(int row, int column, int role = Qt::DisplayRole);
private:
    Ui::w_m_member_list_recharge_scheme *ui;
    QStandardItemModel *model;
};

#endif // W_M_MEMBER_LIST_RECHARGE_SCHEME_H
