#ifndef SAAS_GUIDE_SCREEN_H
#define SAAS_GUIDE_SCREEN_H

#include "w_sys_manage_outer_settings.h"
class saas_guide_screen: public w_sys_manage_outer_settings
{
public:
    saas_guide_screen(QWidget *parent);
private slots:
    virtual void took();
};

#endif // SAAS_GUIDE_SCREEN_H
