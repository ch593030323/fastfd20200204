#ifndef LDS_PUSHBUTTON_WRAP_NOPRESS_H
#define LDS_PUSHBUTTON_WRAP_NOPRESS_H

#include "lds_pushbutton_wrap.h"

class lds_pushbutton_wrap_noPress : public lds_pushbutton_wrap
{
    Q_OBJECT
public:
    lds_pushbutton_wrap_noPress(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *);
};

#endif // LDS_PUSHBUTTON_WRAP_NOPRESS_H
