#ifndef ELECTRONICSCALE_DAHUA_07_H
#define ELECTRONICSCALE_DAHUA_07_H

#include "electronicscale_dahua.h"

class ElectronicScale_DAHUA_07 : public ElectronicScale_DAHUA
{
public:
    ElectronicScale_DAHUA_07();

    virtual QString downloadPre();

    virtual QByteArray downloadAfter();
};

#endif // ELECTRONICSCALE_DAHUA_07_H
