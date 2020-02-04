#ifndef LDS_GETDISHAVAILABLE_H
#define LDS_GETDISHAVAILABLE_H

#include <QString>

class lds_getdishAvailable
{
public:
    lds_getdishAvailable();
    static int availableSpaceMb(const QString &path);
};

#endif // LDS_GETDISHAVAILABLE_H
