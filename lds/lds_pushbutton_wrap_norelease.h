#ifndef LDS_PUSHBUTTON_WRAP_NORELEASE_H
#define LDS_PUSHBUTTON_WRAP_NORELEASE_H

#include "lds_pushbutton_wrap.h"

class lds_pushbutton_wrap_noRelease : public lds_pushbutton_wrap
{
    Q_OBJECT
public:
    lds_pushbutton_wrap_noRelease(QWidget *parent = 0);

    void mouseReleaseEvent(QMouseEvent *);
};

#endif // LDS_PUSHBUTTON_WRAP_NORELEASE_H
