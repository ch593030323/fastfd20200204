#ifndef SAAS_GUIDE_LABEL_H
#define SAAS_GUIDE_LABEL_H

#include "w_sys_manage_outer_settings.h"
class saas_guide_label : public w_sys_manage_outer_settings
{
public:
    saas_guide_label(QWidget *parent);
private slots:
    virtual void took();
};

#endif // SAAS_GUIDE_LABEL_H
