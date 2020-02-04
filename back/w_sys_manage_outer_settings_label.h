#ifndef W_SYS_MANAGE_OUTER_SETTINGS_LABEL_H
#define W_SYS_MANAGE_OUTER_SETTINGS_LABEL_H

#include "labelprint_main_dialog.h"

class w_sys_manage_outer_settings_label : public labelprint_main_dialog
{
    Q_OBJECT
public:
    explicit w_sys_manage_outer_settings_label(QWidget *parent = 0);
protected slots:
    virtual void togoodadd();//添加

};

#endif // W_SYS_MANAGE_OUTER_SETTINGS_LABEL_H
