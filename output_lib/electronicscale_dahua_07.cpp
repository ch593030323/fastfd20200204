#include "electronicscale_dahua_07.h"

ElectronicScale_DAHUA_07::ElectronicScale_DAHUA_07()
{
}

QString ElectronicScale_DAHUA_07::downloadPre()
{
    return "IP";
}

QByteArray ElectronicScale_DAHUA_07::downloadAfter()
{
    return ElectronicScale_DAHUA::downloadAfter();
}
