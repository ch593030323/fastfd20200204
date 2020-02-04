#ifndef W_SCR_DISH_M_HANG_PAY_DIALOG_H
#define W_SCR_DISH_M_HANG_PAY_DIALOG_H

#include <QDialog>
#include"w_rpt_report_frame.h"
class QStandardItemModel;

namespace Ui {
class w_scr_dish_m_hang_pay_Dialog;
}

class w_scr_dish_m_hang_pay_Dialog : public QDialog
{
    Q_OBJECT
public:
    struct rowMP{
        int r;
        QString m;
        QString n;
        double p;
    };
public:
    explicit w_scr_dish_m_hang_pay_Dialog(QWidget *parent);
    ~w_scr_dish_m_hang_pay_Dialog();

private slots:
    void toexit();
    void topay();
    void todetail();

    void toselect();
private:
    Ui::w_scr_dish_m_hang_pay_Dialog *ui;
    report_standmodel *standmodel;
    QString sql;
    QString sql2;
};

#endif // W_SCR_DISH_M_HANG_PAY_DIALOG_H
