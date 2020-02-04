#ifndef PUSHBUTTON_UNDERLINE_H
#define PUSHBUTTON_UNDERLINE_H

#include <QPushButton>
#include "lds_pushbutton_wrap.h"

class pushbutton_underline : public QPushButton
{
    Q_OBJECT
public:
    explicit pushbutton_underline(QWidget *parent = 0);
    void setText(const QString &text);
};

#endif // PUSHBUTTON_UNDERLINE_H
