#ifndef PUBLIC_PUSHBUTTON_NUMKEY_AUTO_CONNECT_H
#define PUBLIC_PUSHBUTTON_NUMKEY_AUTO_CONNECT_H

#include "public_pushbutton_numkey.h"

class public_pushbutton_numkey_auto_connect : public public_pushbutton_numkey
{
    Q_OBJECT
public:
    public_pushbutton_numkey_auto_connect(QWidget *parent = 0);
public slots:
    void setNum();
};

#endif // PUBLIC_PUSHBUTTON_NUMKEY_AUTO_CONNECT_H
