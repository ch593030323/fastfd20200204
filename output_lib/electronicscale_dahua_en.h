#ifndef ELECTRONICSCALE_DAHUA_EN_H
#define ELECTRONICSCALE_DAHUA_EN_H

#include "electronicscale_dahua.h"

class ElectronicScale_DAHUA_EN : public ElectronicScale_DAHUA
{
public:
    ElectronicScale_DAHUA_EN();

    virtual QString downloadPre();

    virtual QByteArray downloadAfter();
};

#endif // ELECTRONICSCALE_DAHUA_EN_H
