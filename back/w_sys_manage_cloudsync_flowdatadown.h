#ifndef W_SYS_MANAGE_CLOUDSYNC_FLOWDATADOWN_H
#define W_SYS_MANAGE_CLOUDSYNC_FLOWDATADOWN_H

#include <QDialog>

namespace Ui {
class w_sys_manage_cloudsync_flowdatadown;
}

class w_sys_manage_cloudsync_flowdatadown : public QDialog
{
    Q_OBJECT

public:
    explicit w_sys_manage_cloudsync_flowdatadown(QWidget *parent = 0);
    ~w_sys_manage_cloudsync_flowdatadown();

private:
    Ui::w_sys_manage_cloudsync_flowdatadown *ui;
};

#endif // W_SYS_MANAGE_CLOUDSYNC_FLOWDATADOWN_H
