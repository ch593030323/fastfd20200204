#include "electronicscale_dahua_en.h"

ElectronicScale_DAHUA_EN::ElectronicScale_DAHUA_EN()
{
}

QString ElectronicScale_DAHUA_EN::downloadPre()
{
    return "!0W";
}

QByteArray ElectronicScale_DAHUA_EN::downloadAfter()
{
    return QByteArray::fromHex("0D0A03");
}
