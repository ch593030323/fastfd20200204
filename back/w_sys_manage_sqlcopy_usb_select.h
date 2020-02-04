#ifndef W_SYS_MANAGE_SQLCOPY_USB_SELECT_H
#define W_SYS_MANAGE_SQLCOPY_USB_SELECT_H

#include <QDialog>
#include "ftableview_standmodel_sql_none.h"


namespace Ui {
class w_sys_manage_sqlcopy_usb_select;
}

class w_sys_manage_sqlcopy_usb_select : public QDialog
{
    Q_OBJECT

public:
    explicit w_sys_manage_sqlcopy_usb_select(QWidget *parent = 0);
    ~w_sys_manage_sqlcopy_usb_select();

    QVariant retData(int role);
public slots:
    void took();
    void tocancel();
private:
    Ui::w_sys_manage_sqlcopy_usb_select *ui;
    ftableview_standmodel_sql_none *model;
    QModelIndex index;
};

#endif // W_SYS_MANAGE_SQLCOPY_USB_SELECT_H
