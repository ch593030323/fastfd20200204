#ifndef W_SCR_BALANCE_SERVICE_H
#define W_SCR_BALANCE_SERVICE_H

#include <QDialog>

namespace Ui {
class w_scr_balance_service;
}

class w_scr_balance_service : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_balance_service(QWidget *parent);
    ~w_scr_balance_service();
    QString get_returnvalue() const;
private slots:
    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();

private:
    Ui::w_scr_balance_service *ui;
    QString is_return;
};

#endif // W_SCR_BALANCE_SERVICE_H
