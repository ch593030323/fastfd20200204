#ifndef LDS_PUSHBUTTON_OBLIQUE_H
#define LDS_PUSHBUTTON_OBLIQUE_H

#include <QPushButton>
#include "lds_pushbutton.h"
#include "lds_bool.h"

class lds_pushbutton_oblique : public lds_pushbutton
{
    Q_OBJECT
public:
    explicit lds_pushbutton_oblique(QWidget *parent = 0);
};

#endif // LDS_PUSHBUTTON_OBLIQUE_H
