#ifndef W_SCR_DISH_M_HANG_DIALOG_H
#define W_SCR_DISH_M_HANG_DIALOG_H

#include <QDialog>
#include <QModelIndex>

class w_m_member_list;
namespace Ui {
class w_scr_dish_m_hang_Dialog;
}

class w_scr_dish_m_hang_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_m_hang_Dialog(QWidget *parent);
    ~w_scr_dish_m_hang_Dialog();
    QString m;
    QString pno;
private slots:
    void updatem(int row);
    void took();
    void tocancel();
    void toreadcard();
private:
    void updatestate(const QString &vch_memberno, const QString &ch_paymodeno, const QString &ch_state, const QString &ch_cardflag, float num_amount, const QDateTime &dt_limit);
private:
    Ui::w_scr_dish_m_hang_Dialog *ui;
    w_m_member_list *outerdialog;
};

#endif // W_SCR_DISH_M_HANG_DIALOG_H
