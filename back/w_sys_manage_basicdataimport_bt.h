#ifndef W_SYS_MANAGE_BASICDATAIMPORT_BT_H
#define W_SYS_MANAGE_BASICDATAIMPORT_BT_H

#include <QDialog>

namespace Ui {
class w_sys_manage_basicdataimport_bt;
}

class w_sys_manage_basicdataimport_bt : public QDialog
{
    Q_OBJECT

public:
    explicit w_sys_manage_basicdataimport_bt(QWidget *parent = 0);
    ~w_sys_manage_basicdataimport_bt();

private slots:
    void took();
    void tocancel();
    void on_toolButton_clicked();
    void topreview();

private:
    Ui::w_sys_manage_basicdataimport_bt *ui;
};

#endif // W_SYS_MANAGE_BASICDATAIMPORT_BT_H
