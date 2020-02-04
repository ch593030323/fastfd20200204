#ifndef PUBLIC_PUSHBUTTON_PAY_H
#define PUBLIC_PUSHBUTTON_PAY_H

#include <QPushButton>
#include "lds_pushbutton_wrap.h"

class public_pushbutton_pay : public lds_pushbutton_wrap
{
    Q_OBJECT
public:
    explicit public_pushbutton_pay(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);

};

#endif // PUBLIC_PUSHBUTTON_PAY_H
