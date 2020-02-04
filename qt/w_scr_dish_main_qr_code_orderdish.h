#ifndef W_SCR_DISH_MAIN_QR_CODE_ORDERDISH_H
#define W_SCR_DISH_MAIN_QR_CODE_ORDERDISH_H

#include <QDialog>
#include "ftableview_standmodel_sql_none.h"

namespace Ui {
class w_scr_dish_main_qr_code_orderdish;
}

class w_scr_dish_main_qr_code_orderdish : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_main_qr_code_orderdish(const QString &ch_billno, const QString &master_sn, const QString &ch_dishno, QWidget *parent = 0);
    ~w_scr_dish_main_qr_code_orderdish();

private slots:
    void tochangesuit(const QModelIndex &index);
    void tosuitreplace(int row);
    void took();
    void tocancel();
    void toaddnum();
    void tosubnum();
private:
    QByteArray zf();
    QByteArray tc();
private:
    Ui::w_scr_dish_main_qr_code_orderdish *ui;
    ftableview_standmodel_sql_none *cook_model;
    ftableview_standmodel_sql_none *suit_model;
    QWidget compop;
    const QString master_sn;
    const QString ch_dishno;
    const QString ch_billno;
};

#endif // W_SCR_DISH_MAIN_QR_CODE_ORDERDISH_H
