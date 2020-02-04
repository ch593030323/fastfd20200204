#ifndef PUBLIC_PUSHBUTTON_NUMKEY_H
#define PUBLIC_PUSHBUTTON_NUMKEY_H

#include <QPushButton>
#include "lds_pushbutton_wrap.h"

class public_pushbutton_numkey : public lds_pushbutton_wrap
{
    Q_OBJECT
public:
    explicit public_pushbutton_numkey(QWidget *parent = 0);

    void setText(const QString &text);
private:
    bool set_num_font;
};

#endif // PUBLIC_PUSHBUTTON_NUMKEY_H
